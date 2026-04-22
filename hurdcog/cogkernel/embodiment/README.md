# Embodiment Layer for Distributed Cognitive Mesh

**Status:** ✅ Phase 4 Implementation Complete

This directory contains the embodiment layer interfaces that enable the Distributed Agentic Cognitive Grammar Network to interact with physical and virtual environments through Unity3D, ROS, and WebSocket APIs.

## Overview

The embodiment layer provides real-time bidirectional communication between the cognitive fusion reactor and external agents, enabling embodied cognition and distributed intelligence across multiple platforms.

## Quick Start

### 1. Start the API Server

```bash
# Install dependencies
pip3 install -r requirements.txt

# Start the server
python3 cogkernel/embodiment/api_server.py

# Or use uvicorn for production
uvicorn cogkernel.embodiment.api_server:app --host 0.0.0.0 --port 8000
```

### 2. Test the API

```bash
# Run integration tests
python3 cogkernel/embodiment/test_api.py

# Or use pytest for detailed testing
pytest cogkernel/embodiment/test_api.py -v
```

### 3. Try the Web Example

Open `cogkernel/embodiment/websocket/example.html` in a web browser to interact with the cognitive mesh through a web interface.

## Components

### REST API Server (`api_server.py`) ✅
- **FastAPI-based REST API**: High-performance async web framework
- **WebSocket Support**: Real-time bidirectional communication
- **Agent Management**: Register and manage cognitive agents
- **Task Processing**: Submit and track cognitive tasks
- **Attention Control**: Query and modify attention allocation

### Unity3D Integration (`unity3d/`) ✅
- **CognitiveAgent.cs**: Unity component for cognitive agent control
- **AttentionVisualizer.cs**: 3D visualization of attention allocation
- **CognitiveWebSocketClient.cs**: Real-time WebSocket communication
- **Interactive Agent Controllers**: User interaction with cognitive agents

### ROS Integration (`ros/`) ✅
- **cognitive_planner.py**: ROS service for cognitive task planning
- **MultiRobotCoordinator**: Distributed multi-robot coordination
- **Message Definitions**: Custom ROS messages for cognitive data
- **CMakeLists.txt & package.xml**: ROS package configuration

### WebSocket Client (`websocket/`) ✅
- **cognitive_client.js**: JavaScript/TypeScript client library
- **example.html**: Interactive web interface demonstration
- **Real-time Communication**: Bidirectional event streaming
- **Agent Registration**: Dynamic agent connection management

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    External Agents                         │
│  (Unity3D Games, ROS Robots, Web Applications)             │
├─────────────────────────────────────────────────────────────┤
│                   Embodiment Layer                         │
│  ┌─────────────┬─────────────┬─────────────┬─────────────┐  │
│  │   Unity3D   │     ROS     │  WebSocket  │  REST API   │  │
│  │ Integration │ Integration │    API      │    Server   │  │
│  └─────────────┴─────────────┴─────────────┴─────────────┘  │
├─────────────────────────────────────────────────────────────┤
│                Cognitive Fusion Reactor                    │
│  (AtomSpace + ECAN + ggml + Meta-Cognition)                │
└─────────────────────────────────────────────────────────────┘
```

## API Specifications

### REST Endpoints

#### Cognitive State
- `GET /api/v1/cognitive/state` - Get current cognitive state
- `GET /api/v1/cognitive/tensor/{id}` - Get specific tensor state
- `POST /api/v1/cognitive/process` - Submit cognitive task
- `PUT /api/v1/cognitive/attention` - Update attention allocation

#### Agent Management
- `POST /api/v1/agents/register` - Register new cognitive agent
- `GET /api/v1/agents/{id}` - Get agent information
- `PUT /api/v1/agents/{id}` - Update agent configuration
- `DELETE /api/v1/agents/{id}` - Unregister agent

#### Attention Allocation
- `GET /api/v1/attention/allocation` - Get current attention distribution
- `GET /api/v1/attention/history` - Get attention allocation history
- `POST /api/v1/attention/focus` - Set attention focus target

### WebSocket Events

#### Cognitive State Updates
- `cognitive.state.update` - Real-time cognitive state changes
- `cognitive.tensor.modified` - Tensor modification events
- `cognitive.pattern.detected` - Pattern recognition events

#### Attention Events
- `attention.allocation.change` - Attention distribution changes
- `attention.focus.shift` - Attention focus transitions
- `attention.resource.competition` - Resource competition events

#### Agent Events
- `agent.registration` - New agent connection
- `agent.disconnection` - Agent disconnection
- `agent.status.update` - Agent status changes

## Unity3D Integration

### Cognitive Behavior Components

```csharp
public class CognitiveAgent : MonoBehaviour
{
    public CognitiveState currentState;
    public AttentionVisualizer attentionViz;
    public WebSocketClient cognitiveClient;
    
    public void UpdateCognitiveState(CognitiveStateEvent evt)
    {
        currentState = evt.state;
        attentionViz.UpdateVisualization(evt.attention);
        ApplyCognitiveBehavior(evt.actions);
    }
    
    public void SubmitCognitiveTask(string task)
    {
        cognitiveClient.Send(new CognitiveTaskMessage {
            agentId = gameObject.GetInstanceID(),
            task = task,
            priority = GetCurrentPriority()
        });
    }
}
```

### Attention Visualization

```csharp
public class AttentionVisualizer : MonoBehaviour
{
    public void UpdateVisualization(AttentionState attention)
    {
        // Visualize attention allocation as heat map
        var heatMap = GenerateAttentionHeatMap(attention);
        ApplyHeatMapToMaterial(heatMap);
        
        // Update particle systems for attention flow
        UpdateAttentionParticles(attention.flow);
    }
}
```

## ROS Integration

### Cognitive Planning Service

```cpp
class CognitivePlannerService
{
public:
    bool planCognitiveTask(
        cognitive_msgs::PlanRequest& request,
        cognitive_msgs::PlanResponse& response)
    {
        // Submit task to cognitive fusion reactor
        auto cognitiveResult = submitToCognitiveFusion(request.task);
        
        // Convert cognitive plan to ROS navigation plan
        response.plan = convertToROSPlan(cognitiveResult);
        response.confidence = cognitiveResult.confidence;
        
        return true;
    }
};
```

### Multi-Robot Coordination

```cpp
class CognitiveCoordinator
{
private:
    ros::ServiceClient cognitive_client_;
    std::vector<RobotAgent> robots_;
    
public:
    void coordinateRobots()
    {
        // Get distributed attention allocation
        auto attention = getCognitiveAttention();
        
        // Assign tasks based on cognitive priorities
        for (auto& robot : robots_) {
            auto task = attention.getTaskForAgent(robot.id);
            robot.assignTask(task);
        }
    }
};
```

## Performance Requirements

### Latency Targets
- **REST API Response**: <50ms for simple queries
- **WebSocket Latency**: <10ms for real-time updates
- **Unity3D Frame Rate**: Maintain 60+ FPS with cognitive processing
- **ROS Service Calls**: <100ms for cognitive planning requests

### Throughput Targets
- **Concurrent Connections**: 1000+ WebSocket connections
- **API Requests**: 10,000+ requests/second
- **Unity3D Agents**: 100+ cognitive agents per scene
- **ROS Topics**: 1000+ messages/second

### Reliability Targets
- **Uptime**: 99.9% availability
- **Error Rate**: <0.1% for API requests
- **Connection Stability**: <1% WebSocket disconnection rate
- **Data Integrity**: 100% message delivery guarantee

## Security

### Authentication
- JWT token-based authentication for REST APIs
- Certificate-based authentication for WebSocket connections
- ROS node authentication through cognitive credentials

### Authorization
- Role-based access control for cognitive operations
- Agent-specific permission management
- Attention allocation access controls

### Data Protection
- TLS encryption for all network communications
- Cognitive state data encryption at rest
- Privacy-preserving attention allocation

## Testing

### Integration Testing
- End-to-end Unity3D cognitive agent scenarios
- Multi-robot ROS coordination validation
- WebSocket connection stability testing
- REST API load testing

### Performance Testing
- Latency measurement under various loads
- Throughput benchmarking
- Memory usage profiling
- Connection scaling tests

### Cognitive Testing
- Embodied cognition behavior validation
- Attention allocation accuracy testing
- Real-world scenario validation
- Cognitive-physical interaction testing

---

*The embodiment layer enables the cognitive fusion reactor to transcend digital boundaries and engage with the physical world through intelligent, adaptive interfaces.*