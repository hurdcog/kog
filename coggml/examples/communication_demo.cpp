/**
 * Example: Inter-Shard Communication
 * Demonstrates the enhanced message-passing protocol between cognitive shards
 */

#include <coggml/microkernel.h>
#include <coggml/cognitive_shard.h>
#include <coggml/shard_coordinator.h>
#include <coggml/shard_message.h>
#include <iostream>
#include <memory>

int main() {
    std::cout << "=== CogGML Enhanced Communication Protocol Demo ===" << std::endl;
    std::cout << std::endl;

    // Initialize microkernel
    coggml::Microkernel kernel;
    kernel.initialize();

    // Create cognitive shards
    auto reasoningShard = std::make_shared<coggml::CognitiveShard>(
        "reasoning", "Logical inference and deduction");
    auto learningShard = std::make_shared<coggml::CognitiveShard>(
        "learning", "Pattern recognition and learning");
    auto memoryShard = std::make_shared<coggml::CognitiveShard>(
        "memory", "Knowledge storage and retrieval");

    // Create coordinator
    coggml::ShardCoordinator coordinator;
    coordinator.registerShard(reasoningShard);
    coordinator.registerShard(learningShard);
    coordinator.registerShard(memoryShard);

    std::cout << "\n--- Setting up message callbacks ---" << std::endl;

    // Set up message handling for reasoning shard
    reasoningShard->setMessageCallback([](const coggml::ShardMessage& msg) {
        std::cout << "[Reasoning] Received " << static_cast<int>(msg.getType()) 
                  << " from " << msg.getSenderId() 
                  << ": " << msg.getPayload() << std::endl;
    });

    // Set up message handling for learning shard
    learningShard->setMessageCallback([](const coggml::ShardMessage& msg) {
        std::cout << "[Learning] Received " << static_cast<int>(msg.getType()) 
                  << " from " << msg.getSenderId() 
                  << ": " << msg.getPayload() << std::endl;
    });

    // Set up message handling for memory shard
    memoryShard->setMessageCallback([](const coggml::ShardMessage& msg) {
        std::cout << "[Memory] Received " << static_cast<int>(msg.getType()) 
                  << " from " << msg.getSenderId() 
                  << ": " << msg.getPayload() << std::endl;
    });

    std::cout << "\n--- Testing Direct Messaging ---" << std::endl;
    
    // Reasoning shard requests data from memory
    reasoningShard->sendMessage("memory", coggml::MessageType::DATA_TRANSFER,
                               "Request: relevant facts for inference",
                               coggml::MessagePriority::HIGH);

    // Learning shard sends pattern to reasoning
    learningShard->sendMessage("reasoning", coggml::MessageType::DATA_TRANSFER,
                              "Pattern detected: P(X) -> Q(X)",
                              coggml::MessagePriority::NORMAL);

    // Execute shards (processes pending messages)
    std::cout << "\n--- Executing coordination cycle ---" << std::endl;
    coordinator.coordinate();

    std::cout << "\n--- Testing Broadcast Messaging ---" << std::endl;
    
    // Memory broadcasts awareness update to all shards
    memoryShard->sendMessage("*", coggml::MessageType::AWARENESS_UPDATE,
                            "Memory cache optimized",
                            coggml::MessagePriority::LOW);

    // Execute another coordination cycle
    coordinator.coordinate();

    std::cout << "\n--- Optimizing Synergy ---" << std::endl;
    coordinator.optimizeSynergy();

    // Get communication statistics
    std::cout << "\n--- Communication Statistics ---" << std::endl;
    auto stats = coordinator.getCommunicationStats();
    std::cout << "Total messages sent: " << stats.totalMessagesSent << std::endl;
    std::cout << "Total messages delivered: " << stats.totalMessagesDelivered << std::endl;
    std::cout << "Messages in flight: " << stats.messagesInFlight << std::endl;
    std::cout << "Average delivery time: " << stats.averageDeliveryTimeMs << " ms" << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;
    
    return 0;
}
