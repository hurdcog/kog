# Phase 4 API - Quick Start Guide

## 🚀 Get Started in 5 Minutes

### Step 1: Install Dependencies

```bash
cd /path/to/hurdcog
pip3 install -r requirements.txt
```

### Step 2: Start the API Server

```bash
# Development mode (auto-reload)
python3 cogkernel/embodiment/api_server.py

# Production mode
uvicorn cogkernel.embodiment.api_server:app --host 0.0.0.0 --port 8000
```

### Step 3: Verify Installation

Open a new terminal and run:

```bash
curl http://localhost:8000/api/v1/health
```

Expected output:
```json
{
  "status": "healthy",
  "timestamp": "2025-10-23T15:00:00",
  "agents": 0,
  "active_connections": 0,
  "tasks_processed": 0
}
```

## 📋 Common Operations

### Submit a Cognitive Task

```bash
curl -X POST http://localhost:8000/api/v1/cognitive/process \
  -H "Content-Type: application/json" \
  -d '{
    "description": "Analyze sensor data",
    "priority": 7,
    "context": {"sensor": "temperature"}
  }'
```

### Register an Agent

```bash
curl -X POST http://localhost:8000/api/v1/agents/register \
  -H "Content-Type: application/json" \
  -d '{
    "agent_type": "sensor_agent",
    "capabilities": ["data_collection", "preprocessing"],
    "metadata": {"location": "lab1"}
  }'
```

### Get Cognitive State

```bash
curl http://localhost:8000/api/v1/cognitive/state
```

### Get Attention Allocation

```bash
curl http://localhost:8000/api/v1/attention/allocation
```

## 🌐 Web Interface

1. Start the API server
2. Open `cogkernel/embodiment/websocket/example.html` in a web browser
3. Click "Connect" to establish WebSocket connection
4. Try submitting tasks and registering agents

## 🎮 Unity3D Integration

1. Copy the C# files from `cogkernel/embodiment/unity3d/` to your Unity project
2. Attach `CognitiveAgent` component to a GameObject
3. Configure the server URL (default: http://localhost:8000)
4. The agent will automatically connect on Start()

```csharp
// Example usage
public class MyBehavior : MonoBehaviour
{
    CognitiveAgent cogAgent;
    
    void Start()
    {
        cogAgent = GetComponent<CognitiveAgent>();
    }
    
    async void Update()
    {
        if (Input.GetKeyDown(KeyCode.Space))
        {
            var result = await cogAgent.SubmitCognitiveTask(
                "Navigate to waypoint",
                priority: 8
            );
            Debug.Log($"Task completed: {result.confidence}");
        }
    }
}
```

## 🤖 ROS Integration

1. Copy `cogkernel/embodiment/ros/` to your ROS workspace
2. Build the package: `catkin_make`
3. Launch the cognitive planner:

```bash
# Single robot
rosrun hurdcog_ros cognitive_planner.py

# Multi-robot coordinator
rosrun hurdcog_ros cognitive_planner.py --multi-robot robot1 robot2 robot3
```

## 🧪 Running Tests

```bash
# Basic API test
python3 cogkernel/embodiment/test_api.py

# With pytest (if installed)
pytest cogkernel/embodiment/test_api.py -v

# Load testing
python3 -c "
import requests
for i in range(100):
    r = requests.post('http://localhost:8000/api/v1/cognitive/process',
                      json={'description': f'Task {i}', 'priority': 5})
    print(f'Task {i}: {r.status_code}')
"
```

## 📊 Monitoring

### Check Server Health

```bash
watch -n 1 'curl -s http://localhost:8000/api/v1/health | python3 -m json.tool'
```

### Monitor Active Connections

```bash
# View server logs
tail -f /path/to/server/logs

# Or in Python
import requests
health = requests.get('http://localhost:8000/api/v1/health').json()
print(f"Active connections: {health['active_connections']}")
print(f"Registered agents: {health['agents']}")
```

## 🔧 Configuration

### Environment Variables

```bash
# Set custom port
export API_PORT=8080
uvicorn cogkernel.embodiment.api_server:app --port $API_PORT

# Enable debug mode
export DEBUG=true
python3 cogkernel/embodiment/api_server.py
```

### Custom Server Settings

Edit `api_server.py`:

```python
# Change host/port
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8080)

# Add production settings
if __name__ == "__main__":
    import uvicorn
    uvicorn.run(
        app,
        host="0.0.0.0",
        port=8000,
        workers=4,  # Number of worker processes
        log_level="info"
    )
```

## 🐛 Troubleshooting

### Server won't start

```bash
# Check if port is in use
lsof -i :8000

# Kill existing process
kill -9 $(lsof -t -i :8000)

# Try different port
python3 cogkernel/embodiment/api_server.py --port 8001
```

### Can't connect from web browser

1. Check CORS settings in `api_server.py`
2. Ensure server is running: `curl http://localhost:8000/`
3. Check browser console for errors

### WebSocket connection fails

1. Verify WebSocket URL: `ws://localhost:8000/ws` (not wss://)
2. Check firewall settings
3. Test with curl:

```bash
websocat ws://localhost:8000/ws
```

## 📚 Next Steps

1. Read [PHASE4_API_IMPLEMENTATION.md](PHASE4_API_IMPLEMENTATION.md) for detailed documentation
2. Review [SECURITY_REVIEW.md](SECURITY_REVIEW.md) before production deployment
3. Check [README.md](README.md) for architecture overview
4. Explore example implementations in `unity3d/`, `ros/`, and `websocket/`

## 💡 Tips

- Use `--reload` flag during development for auto-reload on code changes
- Enable debug logging to see detailed request/response information
- Test with small loads before scaling to production
- Monitor memory usage with large numbers of connections
- Use connection pooling for client applications

## 🆘 Getting Help

- Check server logs for error messages
- Run health check: `curl http://localhost:8000/api/v1/health`
- Verify dependencies: `pip3 freeze | grep fastapi`
- Review test output: `python3 cogkernel/embodiment/test_api.py`

---

**Questions?** Review the [implementation documentation](PHASE4_API_IMPLEMENTATION.md) or check the example code in each integration directory.
