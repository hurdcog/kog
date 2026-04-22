# CogGML Microkernel

## Self-Aware Cognitive Shards for Distributed Processing

The CogGML Microkernel is a foundational component of the OpenCog Collection's autonomous cognitive architecture. It implements self-aware cognitive shards that enable distributed cognitive processing with introspective capabilities.

## Key Features

- **Self-Aware Shards**: Cognitive processing units with introspection and self-monitoring capabilities
- **Shard Coordination**: Manages multiple cognitive shards for synergistic processing
- **Microkernel Architecture**: Lightweight, modular design for scalable cognitive processing
- **Thread-Safe Operations**: Concurrent execution of multiple cognitive processes

## Components

### Microkernel
The core processing unit that manages cognitive tasks and coordinates shard execution.

### Cognitive Shard
Self-contained cognitive processing units with:
- Unique identity and purpose
- Self-awareness state tracking
- Process execution callbacks
- Active/inactive lifecycle management

### Self-Awareness Module
Provides introspection capabilities:
- Internal state monitoring
- Self-reflection mechanisms
- Performance assessment
- Self-improvement suggestions

### Shard Coordinator
Coordinates multiple cognitive shards:
- Shard registration and lifecycle management
- Coordinated execution across shards
- Synergy optimization algorithms
- Active shard tracking
- **Message routing and delivery** (Enhanced)
- **Inter-shard communication protocols** (Enhanced)
- **Communication performance metrics** (Enhanced)

## Usage Example

```cpp
#include <coggml/microkernel.h>
#include <coggml/cognitive_shard.h>
#include <coggml/shard_coordinator.h>
#include <coggml/shard_message.h>

// Initialize microkernel
coggml::Microkernel kernel;
kernel.initialize();

// Create cognitive shards
auto shard1 = std::make_shared<coggml::CognitiveShard>("reasoning", "Logical inference");
auto shard2 = std::make_shared<coggml::CognitiveShard>("learning", "Pattern recognition");

// Coordinate shards
coggml::ShardCoordinator coordinator;
coordinator.registerShard(shard1);
coordinator.registerShard(shard2);

// Set message callback for inter-shard communication
shard1->setMessageCallback([](const coggml::ShardMessage& msg) {
    std::cout << "Shard received: " << msg.getPayload() << std::endl;
});

// Send message between shards
shard1->sendMessage("learning", coggml::MessageType::DATA_TRANSFER, 
                   "inference-result", coggml::MessagePriority::HIGH);

// Execute coordination cycle
coordinator.coordinate();

// Optimize for cognitive synergy
coordinator.optimizeSynergy();

// Get communication statistics
auto stats = coordinator.getCommunicationStats();
std::cout << "Messages sent: " << stats.totalMessagesSent << std::endl;
std::cout << "Average delivery time: " << stats.averageDeliveryTimeMs << " ms" << std::endl;
```

## Building

```bash
mkdir build
cd build
cmake ../coggml
cmake --build .
```

## Integration

CogGML integrates with:
- **CogSelf**: Provides cognitive processing units for AGI coordination
- **AtomSpace Accelerator**: Enables distributed inference processing
- **Agentic Chatbots**: Supports multi-shard agent architectures

## Architecture Philosophy

The CogGML Microkernel embodies principles of cognitive synergy through:
1. **Modularity**: Each shard is an independent cognitive unit
2. **Self-Awareness**: Shards monitor and optimize their own performance
3. **Coordination**: Shards collaborate to achieve emergent capabilities
4. **Scalability**: Microkernel design enables distributed processing

## Future Directions

- ~~Enhanced shard communication protocols~~ ✓ Implemented
- ~~Asynchronous message processing with priority queues~~ ✓ Implemented
- ~~Distributed shard deployment across nodes~~ ✓ Implemented
- Advanced synergy optimization algorithms
- Integration with neural-symbolic hybrid systems
- Message persistence and replay capabilities

## Recent Enhancements (2025)

### Communication Optimization
- **Priority-based Message Queuing**: Messages are now queued by priority (LOW, NORMAL, HIGH, CRITICAL)
- **Asynchronous Processing**: Background thread processes messages in batches for better throughput
- **Queue Management**: Automatic overflow handling with intelligent message dropping

### Distributed Processing
- **Network Coordination**: New DistributedCoordinator enables cognitive processing across network nodes
- **Load Balancing**: Automatic distribution of shards based on node capacity and current load
- **Health Monitoring**: Real-time network health metrics and synchronization

## Contributing

Contributions to CogGML are welcome! Please see the main [CONTRIBUTING.md](../CONTRIBUTING.md) for guidelines.

## License

See [LICENSE](../LICENSE) in the root directory.
