# Cognitive Architecture Integration Example

This example demonstrates the complete OpenCog autonomous cognitive architecture with all integrated components.

## Components Demonstrated

### 1. CogGML Microkernel - Self-Aware Cognitive Shards
- Creates multiple cognitive shards (reasoning, learning, perception)
- Coordinates shards for synergistic processing
- Optimizes cognitive synergy across shards
- Demonstrates self-awareness capabilities

### 2. AtomSpace Accelerator - Inference Engine
- Initializes high-performance inference engine
- Enables query acceleration (2.5x speedup)
- Executes optimized queries over knowledge graphs
- Demonstrates performance optimization

### 3. Agentic Chatbots - Knowledge Integration
- Creates autonomous chatbot agents with specific roles
- Routes messages to appropriate agents
- Integrates knowledge with AtomSpace
- Demonstrates multi-agent coordination

### 4. CogSelf - AGI Cognitive Synergy Framework
- Tracks progress toward AGI goals
- Manages cognitive synergy across components
- Generates autonomous self-improvement plans
- Coordinates all cognitive architecture elements

## Building the Demo

```bash
cd examples/cognitive-architecture
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Running the Demo

Make sure all cognitive architecture libraries are built first:

```bash
# From the repository root
mkdir -p build-test/coggml && cd build-test/coggml
cmake ../../coggml && cmake --build .

mkdir -p ../cogself && cd ../cogself
cmake ../../cogself && cmake --build .

mkdir -p ../atomspace-accelerator && cd ../atomspace-accelerator
cmake ../../atomspace-accelerator && cmake --build .

mkdir -p ../agentic-chatbots && cd ../agentic-chatbots
cmake ../../agentic-chatbots && cmake --build .
```

Then run the demo:

```bash
cd examples/cognitive-architecture/build
LD_LIBRARY_PATH=../../../build-test/coggml:../../../build-test/cogself:../../../build-test/atomspace-accelerator:../../../build-test/agentic-chatbots:$LD_LIBRARY_PATH ./cognitive_demo
```

## What the Demo Shows

The demo demonstrates:

1. **Initialization**: All cognitive components initialize successfully
2. **Shard Creation**: Multiple self-aware cognitive shards are created and coordinated
3. **Inference Acceleration**: AtomSpace accelerator provides optimized query processing
4. **Agent Interaction**: Chatbot agents process messages and integrate knowledge
5. **AGI Progress**: CogSelf tracks progress toward AGI milestones
6. **Self-Improvement**: Framework generates autonomous improvement plans
7. **Cognitive Synergy**: Components work together for emergent intelligence
8. **Self-Awareness**: Introspection and performance monitoring capabilities

## Expected Output

The demo produces detailed output showing:
- Component initialization messages
- Active shard counts and coordination
- Query execution and optimization
- Agent interactions and knowledge integration
- AGI progress metrics (starts at ~6.25%)
- Synergy levels and optimization iterations
- Self-improvement plan generation
- Self-awareness reflections and suggestions

## Key Metrics Displayed

- **Active Shards**: 3 (reasoning, learning, perception)
- **Acceleration Factor**: 2.5x speedup
- **Active Agents**: 2 (ResearchBot, TutorBot)
- **Initial Synergy Level**: 0.25
- **AGI Progress**: 6.25% (initial baseline)

## Architecture Integration

This demo validates the integration of:
```
CogSelf (AGI Coordination)
    ├── CogGML Microkernel (3 active shards)
    ├── AtomSpace Accelerator (2.5x speedup)
    └── Agentic Chatbots (2 agents)
```

## Next Steps

After running this demo, you can:
- Extend the demo with additional cognitive shards
- Add more specialized chatbot agents
- Integrate with actual AtomSpace instances
- Implement custom synergy optimization strategies
- Connect to external knowledge sources

## License

See [LICENSE](../../LICENSE) in the root directory.
