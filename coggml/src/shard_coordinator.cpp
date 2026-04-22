/**
 * Shard Coordinator Implementation
 */

#include "coggml/shard_coordinator.h"
#include <algorithm>
#include <iostream>
#include <mutex>
#include <chrono>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>

namespace coggml {

// Priority queue comparator for messages
struct MessagePriorityCompare {
    bool operator()(const ShardMessage& a, const ShardMessage& b) const {
        // Higher priority values should be processed first
        return static_cast<int>(a.getPriority()) < static_cast<int>(b.getPriority());
    }
};

class ShardCoordinator::Impl {
public:
    std::vector<std::shared_ptr<CognitiveShard>> shards;
    std::mutex coordMutex;
    
    // Priority-based message queue
    std::priority_queue<ShardMessage, std::vector<ShardMessage>, MessagePriorityCompare> messageQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    std::atomic<bool> processingActive{false};
    std::thread processingThread;
    
    // Communication statistics
    size_t totalMessagesSent{0};
    size_t totalMessagesDelivered{0};
    std::vector<double> deliveryTimes;
    size_t queuedMessages{0};
    size_t droppedMessages{0};
    
    // Configuration
    static constexpr size_t MAX_QUEUE_SIZE = 1000;
    static constexpr int BATCH_SIZE = 10;

    Impl() = default;
    
    void startProcessing() {
        if (!processingActive.exchange(true)) {
            processingThread = std::thread([this]() { processMessageQueue(); });
        }
    }
    
    void stopProcessing() {
        if (processingActive.exchange(false)) {
            queueCV.notify_all();
            if (processingThread.joinable()) {
                processingThread.join();
            }
        }
    }
    
    void processMessageQueue() {
        while (processingActive) {
            std::vector<ShardMessage> batch;
            
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                queueCV.wait_for(lock, std::chrono::milliseconds(100), [this]() {
                    return !processingActive || !messageQueue.empty();
                });
                
                if (!processingActive) {
                    break;
                }
                
                // Extract batch of messages
                while (!messageQueue.empty() && batch.size() < BATCH_SIZE) {
                    batch.push_back(messageQueue.top());
                    messageQueue.pop();
                    queuedMessages--;
                }
            }
            
            // Process batch without holding queue lock
            for (const auto& message : batch) {
                deliverMessage(message);
            }
        }
    }
    
    void deliverMessage(const ShardMessage& message) {
        auto startTime = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(coordMutex);
        
        if (message.isBroadcast()) {
            // Broadcast to all shards except sender
            size_t delivered = 0;
            for (auto& shard : shards) {
                if (shard && shard->isActive() && shard->getId() != message.getSenderId()) {
                    shard->receiveMessage(message);
                    delivered++;
                }
            }
            totalMessagesDelivered += delivered;
        } else {
            // Route to specific shard
            auto it = std::find_if(shards.begin(), shards.end(),
                [&message](const std::shared_ptr<CognitiveShard>& shard) {
                    return shard->getId() == message.getReceiverId();
                });
            
            if (it != shards.end() && (*it)->isActive()) {
                (*it)->receiveMessage(message);
                totalMessagesDelivered++;
            } else {
                droppedMessages++;
            }
        }
        
        // Track delivery time
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
        deliveryTimes.push_back(duration.count() / 1000.0); // Convert to milliseconds
    }
};

ShardCoordinator::ShardCoordinator() : pImpl(std::make_unique<Impl>()) {
    std::cout << "[ShardCoordinator] Coordinator initialized with async processing" << std::endl;
    pImpl->startProcessing();
}

ShardCoordinator::~ShardCoordinator() {
    pImpl->stopProcessing();
}

void ShardCoordinator::registerShard(std::shared_ptr<CognitiveShard> shard) {
    if (!shard) {
        return;
    }

    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    pImpl->shards.push_back(shard);
    
    // Set coordinator reference in the shard for message routing
    shard->setCoordinator(this);
    
    std::cout << "[ShardCoordinator] Registered shard: " << shard->getId() << std::endl;
}

void ShardCoordinator::unregisterShard(const std::string& shardId) {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    auto it = std::remove_if(pImpl->shards.begin(), pImpl->shards.end(),
        [&shardId](const std::shared_ptr<CognitiveShard>& shard) {
            if (shard->getId() == shardId) {
                // Clear coordinator reference for this shard
                shard->setCoordinator(nullptr);
                return true;
            }
            return false;
        });
    
    if (it != pImpl->shards.end()) {
        pImpl->shards.erase(it, pImpl->shards.end());
        std::cout << "[ShardCoordinator] Unregistered shard: " << shardId << std::endl;
    }
}

void ShardCoordinator::coordinate() {
    std::cout << "[ShardCoordinator] Coordinating " << pImpl->shards.size() << " shards" << std::endl;
    
    for (auto& shard : pImpl->shards) {
        if (shard && shard->isActive()) {
            shard->execute();
        }
    }
}

std::shared_ptr<CognitiveShard> ShardCoordinator::getShard(const std::string& shardId) const {
    auto it = std::find_if(pImpl->shards.begin(), pImpl->shards.end(),
        [&shardId](const std::shared_ptr<CognitiveShard>& shard) {
            return shard->getId() == shardId;
        });
    
    if (it != pImpl->shards.end()) {
        return *it;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<CognitiveShard>> ShardCoordinator::getAllShards() const {
    return pImpl->shards;
}

size_t ShardCoordinator::getActiveShardCount() const {
    return std::count_if(pImpl->shards.begin(), pImpl->shards.end(),
        [](const std::shared_ptr<CognitiveShard>& shard) {
            return shard && shard->isActive();
        });
}

void ShardCoordinator::optimizeSynergy() {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    std::cout << "[ShardCoordinator] Optimizing cognitive synergy across " 
              << getActiveShardCount() << " active shards" << std::endl;
    
    // Optimization logic for cognitive synergy
    for (auto& shard : pImpl->shards) {
        if (shard && shard->isActive()) {
            shard->updateAwareness("synergy-optimized");
        }
    }
}

void ShardCoordinator::routeMessage(const ShardMessage& message) {
    std::lock_guard<std::mutex> lock(pImpl->queueMutex);
    pImpl->totalMessagesSent++;
    
    // Check queue size limit
    if (pImpl->queuedMessages >= pImpl->MAX_QUEUE_SIZE) {
        // Drop lowest priority message if queue is full
        if (static_cast<int>(message.getPriority()) > static_cast<int>(MessagePriority::LOW)) {
            std::cout << "[ShardCoordinator] Queue full, queueing high priority message" << std::endl;
            pImpl->messageQueue.push(message);
            pImpl->queuedMessages++;
            pImpl->queueCV.notify_one();
        } else {
            pImpl->droppedMessages++;
            std::cerr << "[ShardCoordinator] Queue full, dropped low priority message" << std::endl;
        }
    } else {
        pImpl->messageQueue.push(message);
        pImpl->queuedMessages++;
        pImpl->queueCV.notify_one();
    }
}

ShardCoordinator::CommunicationStats ShardCoordinator::getCommunicationStats() const {
    std::lock_guard<std::mutex> lock(pImpl->coordMutex);
    
    CommunicationStats stats;
    stats.totalMessagesSent = pImpl->totalMessagesSent;
    stats.totalMessagesDelivered = pImpl->totalMessagesDelivered;
    stats.messagesInFlight = pImpl->queuedMessages;
    
    // Calculate average delivery time
    if (!pImpl->deliveryTimes.empty()) {
        double sum = 0.0;
        for (double time : pImpl->deliveryTimes) {
            sum += time;
        }
        stats.averageDeliveryTimeMs = sum / pImpl->deliveryTimes.size();
    } else {
        stats.averageDeliveryTimeMs = 0.0;
    }
    
    return stats;
}

} // namespace coggml
