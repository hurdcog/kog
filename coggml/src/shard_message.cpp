/**
 * Shard Message Implementation
 */

#include "coggml/shard_message.h"

namespace coggml {

ShardMessage::ShardMessage(
    const std::string& senderId,
    const std::string& receiverId,
    MessageType type,
    const std::string& payload,
    MessagePriority priority
)
    : senderId_(senderId)
    , receiverId_(receiverId)
    , type_(type)
    , payload_(payload)
    , priority_(priority)
    , timestamp_(std::chrono::steady_clock::now())
{
}

long long ShardMessage::getAgeMs() const {
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp_);
    return duration.count();
}

} // namespace coggml
