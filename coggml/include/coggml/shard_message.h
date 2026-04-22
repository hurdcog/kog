/**
 * Shard Message Protocol
 * Defines message types and structures for inter-shard communication
 */

#ifndef _COGGML_SHARD_MESSAGE_H
#define _COGGML_SHARD_MESSAGE_H

#include <string>
#include <chrono>
#include <memory>

namespace coggml {

/**
 * MessageType - Categories of shard communication
 */
enum class MessageType {
    DATA_TRANSFER,      // Transfer cognitive data between shards
    SYNC_REQUEST,       // Request synchronization with another shard
    SYNC_RESPONSE,      // Response to sync request
    AWARENESS_UPDATE,   // Broadcast awareness state changes
    TASK_REQUEST,       // Request task execution from another shard
    TASK_COMPLETE,      // Notify task completion
    ERROR_REPORT,       // Report errors or issues
    PERFORMANCE_METRIC  // Share performance metrics
};

/**
 * MessagePriority - Priority levels for message processing
 */
enum class MessagePriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

/**
 * ShardMessage - Message structure for inter-shard communication
 */
class ShardMessage {
public:
    ShardMessage(
        const std::string& senderId,
        const std::string& receiverId,
        MessageType type,
        const std::string& payload,
        MessagePriority priority = MessagePriority::NORMAL
    );

    // Getters
    std::string getSenderId() const { return senderId_; }
    std::string getReceiverId() const { return receiverId_; }
    MessageType getType() const { return type_; }
    std::string getPayload() const { return payload_; }
    MessagePriority getPriority() const { return priority_; }
    
    // Get message age in milliseconds
    long long getAgeMs() const;
    
    // Get timestamp
    std::chrono::steady_clock::time_point getTimestamp() const { return timestamp_; }
    
    // Check if message is broadcast (receiverId is "*")
    bool isBroadcast() const { return receiverId_ == "*"; }

private:
    std::string senderId_;
    std::string receiverId_;
    MessageType type_;
    std::string payload_;
    MessagePriority priority_;
    std::chrono::steady_clock::time_point timestamp_;
};

} // namespace coggml

#endif // _COGGML_SHARD_MESSAGE_H
