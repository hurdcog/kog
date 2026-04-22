# CogPrime Identity + Distributed Cognition Integration Guide

## Overview

This document describes the integration of CogPrime identity with OpenCog's distributed cognition capabilities in the OCC (OpenCog Collection) repository. This integration enables cognitive agents with persistent self-identity to operate across distributed computational networks while maintaining identity coherence.

## Architecture

The integration consists of three main components:

### 1. CogPrime Identity Module (`cogself/cogprime_identity.h/cpp`)

Implements core self-identity for cognitive agents based on CogPrime principles:

- **Cognitive Characteristics**: Quantifiable traits defining agent capabilities
  - `general_intelligence`: Problem-solving and reasoning capacity
  - `cognitive_synergy`: Ability to integrate diverse cognitive processes
  - `self_awareness`: Meta-cognitive monitoring capabilities
  - `learning_capacity`: Skill acquisition and refinement ability
  - Custom characteristics can be added

- **Episodic Memory**: Time-stamped significant experiences
  - Event ID and description
  - Significance rating (0.0-1.0)
  - Contextual metadata
  - Timestamp tracking

- **Cognitive Goals**: Hierarchical goal system
  - Goal ID and description
  - Priority levels (0.0-1.0)
  - Progress tracking (0.0-1.0)
  - Active/completed status

- **Self-Coherence Metrics**:
  - `getSelfCoherence()`: Measures identity stability
  - `getIdentityStrength()`: Overall identity robustness
  - `getCognitiveIntegration()`: Goal coordination quality

### 2. Distributed Identity Manager (`cogself/distributed_identity.h/cpp`)

Manages identity across distributed cognitive networks:

- **Identity-Aware Nodes**: Network nodes tracking identity coherence
  - Node ID, address, and port
  - Processing capacity and active shards
  - Identity coherence level
  - Synergy alignment score

- **Identity-Aware Shards**: Cognitive tasks with identity dependencies
  - Shard ID and purpose
  - Identity requirement flag
  - Identity dependency level (0.0-1.0)
  - Assigned node tracking

- **Key Operations**:
  - Identity propagation to network nodes
  - Identity synchronization across network
  - Coherence monitoring and fragmentation detection
  - Identity evolution from distributed experiences

### 3. CogSelf Integration (`cogself/cogself.h/cpp`)

Central coordination framework enhanced with identity:

- Automatic identity initialization on framework startup
- Distributed cognition enablement
- Identity-aware synergy updates
- Enhanced improvement planning with identity insights

## Usage Examples

### Basic Identity Creation

```cpp
#include <cogself/cogprime_identity.h>

// Create identity
auto identity = std::make_shared<cogself::CogPrimeIdentity>(
    "agent_001", 
    "My Cognitive Agent"
);

// Add custom characteristics
identity->addCharacteristic(cogself::CognitiveCharacteristic(
    "creativity",
    "Ability to generate novel solutions",
    0.7,  // strength
    0.6   // stability
));

// Record experiences
identity->recordMemory(
    "discovery_001",
    "Made important breakthrough in pattern recognition",
    0.9  // high significance
);

// Set goals
identity->addGoal(
    "master_reasoning",
    "Achieve expert-level logical reasoning",
    0.85  // high priority
);

// Update goal progress
identity->updateGoalProgress("master_reasoning", 0.4);

// Get metrics
double coherence = identity->getSelfCoherence();
double strength = identity->getIdentityStrength();
double integration = identity->getCognitiveIntegration();

std::cout << identity->getIdentitySummary() << std::endl;
```

### CogSelf with Identity

```cpp
#include <cogself/cogself.h>

// Initialize CogSelf with identity
cogself::CogSelf framework;
framework.initialize("agent_002", "CogSelf Agent");

// Get identity
auto identity = framework.getIdentity();

// Record cognitive events
framework.recordCognitiveEvent(
    "task_completion",
    "Successfully completed complex reasoning task",
    0.8
);

// Update synergy state
framework.updateSynergyState();

// Get synergy level
double synergy = framework.getSynergyLevel();

// Generate improvement plan (includes identity insights)
std::string plan = framework.generateImprovementPlan();
std::cout << plan << std::endl;
```

### Distributed Cognition with Identity

```cpp
#include <cogself/cogself.h>
#include <cogself/distributed_identity.h>

// Initialize framework
cogself::CogSelf framework;
framework.initialize("agent_003", "Distributed Agent");

// Enable distributed cognition
framework.enableDistributedCognition();

// Register network nodes
framework.registerDistributedNode("node_1", "192.168.1.10", 8001);
framework.registerDistributedNode("node_2", "192.168.1.11", 8002);
framework.registerDistributedNode("node_3", "192.168.1.12", 8003);

// Get distributed manager for advanced operations
auto distMgr = framework.getDistributedManager();

// Distribute identity-aware cognitive shards
distMgr->distributeShard(cogself::IdentityShard(
    "reasoning_shard",
    "Logical reasoning and inference",
    true,  // requires identity
    0.8    // high identity dependency
));

distMgr->distributeShard(cogself::IdentityShard(
    "learning_shard",
    "Pattern recognition and learning",
    true,
    0.7
));

// Propagate identity to all nodes
distMgr->propagateIdentityToAllNodes();

// Synchronize identity across network
framework.synchronizeDistributedIdentity();

// Monitor network coherence
double coherence = framework.getDistributedCoherence();

// Get detailed synergy metrics
auto metrics = distMgr->getIdentitySynergyMetrics();
std::cout << "Overall Coherence: " << metrics.overallCoherence << std::endl;
std::cout << "Network Integration: " << metrics.networkIntegration << std::endl;
std::cout << "Active Nodes: " << metrics.activeNodes << std::endl;
std::cout << "Active Shards: " << metrics.activeShards << std::endl;

// Record distributed cognitive events
distMgr->recordDistributedCognitiveEvent(
    "node_1",
    "reasoning_complete",
    "Completed complex reasoning task on distributed node"
);

// Update identity from distributed experience
distMgr->updateIdentityFromDistributedExperience();

// Validate identity integrity
bool valid = distMgr->validateIdentityIntegrity();
```

## Identity Evolution

Identity characteristics evolve through distributed cognitive processing:

```cpp
// Initial state
auto identity = framework.getIdentity();
auto initial = identity->getCharacteristic("cognitive_synergy");
std::cout << "Initial: " << initial.strength << std::endl;

// Perform cognitive cycles
for (int i = 0; i < 10; ++i) {
    framework.updateSynergyState();
    
    if (i % 3 == 0) {
        distMgr->synchronizeIdentityAcrossNetwork();
    }
}

// Identity evolves from distributed experience
distMgr->updateIdentityFromDistributedExperience();

// Check evolution
auto final = identity->getCharacteristic("cognitive_synergy");
std::cout << "Final: " << final.strength << std::endl;
```

## Building

The integration is built as part of the CogSelf library:

```bash
mkdir build
cd build
cmake ../cogself
cmake --build .
```

## Testing

Run the minimal test to verify core integration:

```bash
cd build
g++ -std=c++17 -I../cogself/include -L. \
    ../examples/test_cogprime_minimal.cpp \
    -lcogself -lpthread -o test_minimal

LD_LIBRARY_PATH=. ./test_minimal
```

## Key Concepts

### Identity Coherence

Identity coherence measures how stable and consistent the agent's self-model is:
- Based on characteristic stability values
- High coherence (>0.7) indicates robust identity
- Low coherence may indicate fragmentation or rapid change

### Identity Strength

Identity strength measures overall robustness:
- Combines characteristic strengths and memory significance
- Increased by significant experiences
- Enhanced through goal achievement

### Cognitive Integration

Measures how well goals are aligned and progressing:
- Based on active goal progress
- Influenced by cognitive synergy characteristic
- High integration indicates coordinated cognitive processes

### Identity Dependency

Shards specify how much they depend on identity (0.0-1.0):
- **0.0-0.3**: Low dependency (computation-heavy, identity-light)
- **0.4-0.6**: Medium dependency (balanced)
- **0.7-1.0**: High dependency (requires strong identity coherence)

Identity-dependent shards are preferentially assigned to nodes with high identity coherence.

## Best Practices

1. **Initialize Identity Early**: Create identity when framework starts
2. **Record Significant Events**: Use appropriate significance levels (0.7+ for breakthroughs)
3. **Monitor Coherence**: Check network coherence regularly in distributed scenarios
4. **Synchronize Periodically**: Call `synchronizeDistributedIdentity()` after significant changes
5. **Validate Integrity**: Use `validateIdentityIntegrity()` before critical operations
6. **Set Appropriate Dependencies**: Match shard identity dependency to actual requirements

## Integration with OpenCog Components

This integration works seamlessly with other OCC components:

- **CogGML Microkernel**: Identity-aware cognitive shards
- **AtomSpace**: Identity persistence in hypergraph
- **Agentic Chatbots**: Identity-informed responses
- **AtomSpace Accelerator**: Identity-aware query optimization

## Future Enhancements

Potential areas for extension:

- Identity persistence to/from AtomSpace hypergraph
- Multi-agent identity interaction and collaboration
- Identity-based security and access control
- Advanced identity evolution algorithms
- Identity visualization and monitoring tools

## References

- CogPrime Cognitive Architecture (Ben Goertzel et al.)
- OpenCog Hyperon Design
- Distributed Cognitive Systems Theory
- OpenCog Collection Documentation

## License

See [LICENSE](../LICENSE) in the root directory.
