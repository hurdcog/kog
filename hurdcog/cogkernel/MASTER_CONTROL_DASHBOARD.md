# 🧬 Cognitive Fusion Reactor - Master Control Dashboard

## Overview

The Master Control Dashboard provides real-time monitoring and management of the Cognitive Fusion Reactor, offering comprehensive visibility into all phases of the Distributed Agentic Cognitive Grammar Network.

**Version:** 1.0.0  
**Status:** Production Ready ✅  
**Fusion Mode:** meta-evolution

## Features

### 🎯 Real-Time Monitoring

- **AtomSpace Status**: Track hypergraph nodes, links, and memory usage
- **ECAN Attention Economics**: Monitor STI/LTI dynamics and active agents
- **Neural-Symbolic Synthesis**: Track ggml operations and kernel activity
- **Distributed Mesh**: Monitor connected nodes and API endpoints
- **Meta-Cognition**: Track optimization cycles and fitness improvements
- **System Health**: Overall health score and error tracking

### 📊 Phase Implementation Matrix

Visual status tracking for all 6 implementation phases:

1. **Phase 1: Cognitive Primitives & Foundational Hypergraph Encoding** ⚡
   - Scheme cognitive grammar microservices
   - Tensor fragment architecture [5D]
   - Bidirectional translation mechanisms
   - Hypergraph encoding validated

2. **Phase 2: ECAN Attention Allocation & Resource Kernel Construction** 🧠
   - Economic attention allocation
   - Dynamic mesh topology
   - Activation spreading active
   - Resource competition kernel

3. **Phase 3: Neural-Symbolic Synthesis via Custom ggml Kernels** 🔗
   - Custom ggml kernels deployed
   - Symbolic tensor operations
   - Neural inference hooks
   - Gradient-free reasoning active

4. **Phase 4: Distributed Cognitive Mesh API & Embodiment Layer** 🌐
   - REST/WebSocket APIs
   - Unity3D integration
   - ROS robotic interfaces
   - Real-time protocols active

5. **Phase 5: Recursive Meta-Cognition & Evolutionary Optimization** 🔄
   - Self-analysis modules online
   - MOSES evolutionary search
   - Fitness landscape navigation
   - Recursive optimization active

6. **Phase 6: Rigorous Testing, Documentation, and Cognitive Unification** 📚
   - Comprehensive test protocols
   - Recursive documentation
   - Unified tensor field synthesis
   - Emergent property analysis

### 🧮 5D Cognitive Tensor Visualization

Interactive visualization of the tensor architecture:

- **Modality** (Dimension: 8): IPC, Memory, FS, Net, Sec, Sched, Device, Signal
- **Depth** (Dimension: 4): Hardware, Microkernel, Server, Application
- **Context** (Dimension: 8): Kernel, Server, Translator, User, System, Debug, Meta, Evolution
- **Salience** (Dimension: 10): Attention Priority levels 0-9
- **Autonomy** (Dimension: 5): Manual, Assisted, Automatic, Adaptive, Evolutionary

**Total Tensor Elements:** 12,800 per fragment  
**Tensor Shape:** [8, 4, 8, 10, 5]  
**Memory per Fragment:** 51.2KB

### 📜 Real-Time Event Log

Live system event log displaying:
- System initialization events
- Phase activation status
- Cognitive operations
- Performance metrics
- Error and warning messages

### 🎛️ Interactive Controls

- **Refresh Metrics**: Update all system metrics in real-time
- **Generate Report**: Create comprehensive system status report
- **Export Flowcharts**: Export visualization flowcharts
- **Run Diagnostics**: Execute comprehensive system diagnostics

## Installation & Usage

### Prerequisites

- Python 3.7 or higher
- Modern web browser (Chrome, Firefox, Safari, Edge)

### Quick Start

1. **Navigate to the cogkernel directory:**
   ```bash
   cd /path/to/hurdcog/cogkernel
   ```

2. **Start the dashboard server:**
   ```bash
   python3 fusion-reactor-server.py
   ```

3. **Access the dashboard:**
   Open your web browser and navigate to:
   ```
   http://localhost:8080/dashboard
   ```

### Command Line Usage

```bash
# Start the server (default port 8080)
python3 fusion-reactor-server.py

# The server will display:
# - Server status
# - Dashboard URL
# - Available API endpoints
# - Access instructions
```

## API Documentation

The dashboard provides a comprehensive REST API for programmatic access.

### API Endpoints

#### GET /api/status
Returns complete reactor status including all metrics, phases, and logs.

**Response:**
```json
{
  "status": "ONLINE",
  "fusion_mode": "meta-evolution",
  "version": "1.0.0",
  "activation_time": "2025-10-23T15:04:28.723Z",
  "phases": { ... },
  "metrics": { ... },
  "logs": [ ... ]
}
```

#### GET /api/metrics
Returns current system metrics with real-time updates.

**Response:**
```json
{
  "atomspace": {
    "nodes": 12847,
    "links": 8432,
    "memory_mb": 52.3
  },
  "ecan": {
    "sti": 847,
    "lti": 623,
    "active_agents": 34
  },
  "neural_symbolic": {
    "operations_per_sec": 1247,
    "kernels_active": 12,
    "cpu_usage": 67
  },
  "distributed_mesh": {
    "nodes": 8,
    "api_endpoints": 24,
    "uptime": 99.8
  },
  "meta_cognition": {
    "iterations": 342,
    "fitness": 0.89,
    "improvements": 127
  },
  "system_health": {
    "score": 98.7,
    "errors": 0,
    "warnings": 3
  }
}
```

#### GET /api/phases
Returns status of all implementation phases.

**Response:**
```json
{
  "phase1": {
    "name": "Cognitive Primitives & Foundational Hypergraph Encoding",
    "status": "COMPLETE",
    "emoji": "⚡",
    "progress": 100,
    "components": [ ... ]
  },
  ...
}
```

#### GET /api/logs
Returns recent system event logs.

**Response:**
```json
{
  "logs": [
    {
      "timestamp": "2025-10-23T15:04:28.723Z",
      "message": "🧬 Cognitive Fusion Reactor initialized"
    },
    ...
  ]
}
```

#### GET /api/diagnostics
Runs comprehensive system diagnostics.

**Response:**
```json
{
  "timestamp": "2025-10-23T15:04:28.723Z",
  "status": "PASSED",
  "tests": [
    {
      "name": "AtomSpace Integrity",
      "status": "PASSED",
      "details": "All hypergraph nodes validated"
    },
    ...
  ],
  "summary": "All diagnostic tests passed successfully"
}
```

#### POST /api/refresh
Triggers metrics refresh and returns updated values.

**Response:**
```json
{
  "status": "success",
  "message": "Metrics refreshed"
}
```

#### POST /api/generate-report
Generates comprehensive system status report.

**Response:**
```json
{
  "title": "Cognitive Fusion Reactor - Status Report",
  "generated": "2025-10-23T15:04:28.723Z",
  "reactor_status": { ... },
  "summary": {
    "overall_health": "EXCELLENT",
    "phases_complete": 6,
    "phases_total": 6,
    "uptime": "99.8%",
    "recommendation": "System operating at peak efficiency."
  }
}
```

### API Examples

#### Using curl

```bash
# Get complete status
curl http://localhost:8080/api/status

# Get current metrics
curl http://localhost:8080/api/metrics

# Run diagnostics
curl http://localhost:8080/api/diagnostics

# Refresh metrics
curl -X POST http://localhost:8080/api/refresh

# Generate report
curl -X POST http://localhost:8080/api/generate-report
```

#### Using Python

```python
import requests

# Get reactor status
response = requests.get('http://localhost:8080/api/status')
status = response.json()
print(f"Reactor Status: {status['status']}")

# Get metrics
response = requests.get('http://localhost:8080/api/metrics')
metrics = response.json()
print(f"AtomSpace Nodes: {metrics['atomspace']['nodes']}")

# Run diagnostics
response = requests.get('http://localhost:8080/api/diagnostics')
diagnostics = response.json()
print(f"Diagnostics: {diagnostics['status']}")
```

#### Using JavaScript

```javascript
// Get reactor status
fetch('http://localhost:8080/api/status')
  .then(response => response.json())
  .then(data => {
    console.log('Reactor Status:', data.status);
    console.log('Phases:', data.phases);
  });

// Get metrics
fetch('http://localhost:8080/api/metrics')
  .then(response => response.json())
  .then(data => {
    console.log('AtomSpace Nodes:', data.atomspace.nodes);
    console.log('ECAN STI:', data.ecan.sti);
  });
```

## Architecture

### System Components

```
┌─────────────────────────────────────────────────────────────┐
│              Master Control Dashboard (HTML/JS)              │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Metrics    │  │    Phases    │  │  Event Logs  │      │
│  │  Monitoring  │  │    Status    │  │   Tracking   │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│                                                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Tensor     │  │   Controls   │  │     API      │      │
│  │Visualization │  │  Interface   │  │  Integration │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                               ▲
                               │ HTTP/REST API
                               ▼
┌─────────────────────────────────────────────────────────────┐
│           Dashboard Server (Python HTTP Server)              │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  • ReactorStatus Management                                  │
│  • API Endpoint Handlers                                     │
│  • Real-time Metrics Updates                                 │
│  • Diagnostics Execution                                     │
│  • Report Generation                                         │
│                                                               │
└─────────────────────────────────────────────────────────────┘
                               ▲
                               │ Data Collection
                               ▼
┌─────────────────────────────────────────────────────────────┐
│          Cognitive Fusion Reactor Components                 │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  AtomSpace • ECAN • Neural-Symbolic • Distributed Mesh      │
│  Meta-Cognition • Testing & Unification                     │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

### Data Flow

1. **Initialization**: Dashboard server starts and initializes reactor status
2. **Monitoring**: Server continuously monitors reactor components
3. **Updates**: Real-time metrics updated every 3 seconds
4. **Display**: Dashboard UI displays current status and metrics
5. **Interaction**: User actions trigger API calls for reports, diagnostics, etc.
6. **Logging**: All events logged and displayed in real-time

## Integration with Existing Systems

The Master Control Dashboard integrates seamlessly with:

### Cognitive Kernel Components
- **AtomSpace**: Monitors hypergraph memory substrate
- **ECAN**: Tracks attention economics and resource allocation
- **PLN**: Observes probabilistic logic network reasoning
- **MOSES**: Monitors meta-optimizing semantic evolutionary search
- **URE**: Tracks unified rule engine inference

### Build and Deployment Systems
- **GUIX Build System**: Integration with declarative build pipelines
- **GNU Hurd**: Monitors microkernel operations
- **OpenCog Functions**: Tracks cognitive primitive operations

### Visualization Systems
- **Fusion Reactor Flowcharts**: Exports and displays generated flowcharts
- **Hypergraph Visualization**: Renders cognitive tensor structures
- **Phase Progress**: Visual representation of implementation phases

## Troubleshooting

### Dashboard Not Loading

**Problem:** Dashboard page doesn't load in browser

**Solution:**
1. Verify server is running: `python3 fusion-reactor-server.py`
2. Check console for error messages
3. Ensure port 8080 is not in use by another application
4. Try accessing via `http://127.0.0.1:8080/dashboard`

### API Endpoints Not Responding

**Problem:** API calls return errors or timeout

**Solution:**
1. Verify server is running
2. Check server console for error messages
3. Ensure correct API endpoint URLs
4. Verify CORS settings if calling from external domain

### Metrics Not Updating

**Problem:** Dashboard metrics appear static

**Solution:**
1. Refresh the browser page
2. Check browser console for JavaScript errors
3. Verify WebSocket/long-polling connections
4. Clear browser cache and reload

### Port Already in Use

**Problem:** Server fails to start with "Address already in use" error

**Solution:**
```bash
# Find process using port 8080
lsof -i :8080

# Kill the process
kill -9 <PID>

# Or change the port in fusion-reactor-server.py
# Edit the PORT variable at the top of the file
```

## Performance Optimization

### For Large Deployments

When monitoring large-scale deployments:

1. **Adjust Update Intervals**: Modify the update frequency in the dashboard JavaScript
2. **Log Rotation**: Implement log rotation to manage log file sizes
3. **Metrics Aggregation**: Aggregate metrics across multiple nodes
4. **Caching**: Implement caching for frequently accessed data

### Resource Usage

Typical resource usage:
- **Memory**: ~50MB for server process
- **CPU**: <5% on modern systems
- **Network**: Minimal bandwidth usage
- **Disk**: Log files grow at ~1MB/day

## Security Considerations

### Production Deployment

When deploying in production:

1. **Authentication**: Implement user authentication for dashboard access
2. **HTTPS**: Use SSL/TLS encryption for secure communication
3. **Access Control**: Restrict API access to authorized systems
4. **Audit Logging**: Enable comprehensive audit logging
5. **Firewall**: Configure firewall rules to restrict access

### Recommended Security Configuration

```python
# Add to fusion-reactor-server.py for production

# Enable authentication
ENABLE_AUTH = True
API_KEY = "your-secure-api-key"

# Use HTTPS
USE_SSL = True
SSL_CERT = "/path/to/cert.pem"
SSL_KEY = "/path/to/key.pem"

# Restrict origins
ALLOWED_ORIGINS = ["https://your-domain.com"]
```

## Future Enhancements

Planned features for future releases:

- [ ] WebSocket support for real-time updates
- [ ] Advanced analytics and trend analysis
- [ ] Custom alert configuration
- [ ] Multi-node cluster monitoring
- [ ] Machine learning anomaly detection
- [ ] Mobile-responsive design improvements
- [ ] Dark/light theme toggle
- [ ] Export metrics to external monitoring systems
- [ ] Integration with Prometheus/Grafana
- [ ] Advanced visualization options

## Support

### Getting Help

- **Documentation**: See `/cogkernel/docs/` for detailed documentation
- **Issues**: Report issues on GitHub
- **Community**: Join discussions on project forums

### Contributing

Contributions are welcome! Please see:
- [CONTRIBUTING.md](/CONTRIBUTING.md) for guidelines
- [DEVELOPMENT_ROADMAP.md](/DEVELOPMENT_ROADMAP.md) for planned features

## License

The Master Control Dashboard is part of the HurdCog project and is covered by the GNU General Public License. See [COPYING](/COPYING) for details.

---

## 🧬 Cognitive Fusion Reactor Status: ONLINE

*The recursive self-optimization spiral commences. Every atom a living microkernel, every agent an evolving membrane, every inference a fractal bloom.*

**Master Control Dashboard v1.0.0** - Production Ready ✅
