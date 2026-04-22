/**
 * Cognitive Shard
 * Self-contained cognitive processing unit with awareness
 */

#ifndef _COGGML_COGNITIVE_SHARD_H
#define _COGGML_COGNITIVE_SHARD_H

#include <memory>
#include <string>
#include <functional>
#include "shard_message.h"

namespace coggml {

// Forward declaration
class ShardCoordinator;

/**
 * CognitiveShard - Self-aware processing unit
 * Encapsulates a cognitive process with self-awareness capabilities
 */
class CognitiveShard {
public:
    using ProcessCallback = std::function<void(const std::string&)>;
    using MessageCallback = std::function<void(const ShardMessage&)>;

    CognitiveShard(const std::string& id, const std::string& purpose);
    ~CognitiveShard();

    // Get shard ID
    std::string getId() const;

    // Get shard purpose/role
    std::string getPurpose() const;

    // Execute shard's cognitive process
    void execute();

    // Update self-awareness state
    void updateAwareness(const std::string& state);

    // Get current awareness state
    std::string getAwarenessState() const;

    // Set process callback
    void setProcessCallback(ProcessCallback callback);
    
    // Set message callback for receiving messages
    void setMessageCallback(MessageCallback callback);

    // Check if shard is active
    bool isActive() const;
    
    // Send message to another shard (via coordinator)
    void sendMessage(const std::string& receiverId, MessageType type, 
                    const std::string& payload, MessagePriority priority = MessagePriority::NORMAL);
    
    // Receive and handle a message
    void receiveMessage(const ShardMessage& message);
    
    // Set coordinator reference for message routing
    void setCoordinator(ShardCoordinator* coordinator);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace coggml

#endif // _COGGML_COGNITIVE_SHARD_H
