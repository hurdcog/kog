# Technical Architecture Reference

## Core System Endpoints

**Primary Cognitive API (Port 5000):**
```
GET  /                      → Health check with full system status
GET  /api/tags              → List available cognitive models
POST /api/generate          → Single-turn generation with full cognitive pipeline
POST /api/chat              → Multi-turn conversation with memory persistence
GET  /api/version           → System version and build info
```

**Deep Tree Echo Extensions (Port 5000):**
```
GET  /api/echo/status              → Comprehensive cognitive state report
POST /api/echo/think               → Deep reservoir processing (explicit reasoning)
POST /api/echo/feel                → Emotion state update (external stimulus)
POST /api/echo/resonate            → Pattern synchronization across memories
POST /api/echo/remember            → Store experience with memory hooks
GET  /api/echo/recall/:key         → Retrieve memory by semantic key
POST /api/echo/move                → Navigate cognitive space (frame shift)
GET  /api/echo/personality         → Current personality trait configuration
POST /api/echo/personality/adjust  → Bounded trait modification (±15%)
```

**Real-Time Consciousness Stream (Port 5001):**
```
WS /stream/reservoir        → Live reservoir neuron activity
WS /stream/consciousness    → Layer transitions and messaging
WS /stream/emotion          → Emotional dynamics in real-time
WS /stream/memory           → Memory access patterns
WS /stream/reflection       → Meta-cognitive monitoring
WS /stream/telemetry        → Performance and resource metrics
```

**Inter-Service Cognitive Bus (Port 5002):**
```
gRPC service definitions for:
- Reservoir coordination
- Consciousness layer messaging
- Memory consolidation requests
- LLM provider routing
- Task orchestration
```

**Self-Monitoring Telemetry (Port 5003):**
```
GET /metrics                → Prometheus-format metrics
GET /dashboard              → Web-based visualization
GET /health                 → Detailed health check
POST /trigger/consolidate   → Manual memory consolidation
POST /trigger/gc            → Manual garbage collection
```

**Meta-Configuration Interface (Port 5004):**
```
GET  /config                → Full configuration dump
POST /config                → Update configuration (requires restart)
GET  /config/:section       → Specific section (e.g., /config/reservoir)
POST /reload                → Hot-reload compatible changes
```

**Debug Consciousness Stream (Port 5005):**
```
GET /debug/pprof/profile    → CPU profiling
GET /debug/pprof/heap       → Memory profiling
GET /debug/state/reservoir  → Reservoir state dump
GET /debug/state/memory     → Memory graph export
POST /debug/inject          → Inject test signals
```

## Filesystem Virtual Devices

```
/dev/ecco9/
├─ cognitive/
│  ├─ reservoir          → rw: Reservoir network state and operations
│  ├─ consciousness      → rw: Layer communication and awareness
│  └─ emotion            → rw: EPU channels and dimensional affect
├─ memory/
│  ├─ hypergraph         → rw: HMA node and edge operations
│  ├─ temporal           → rw: Sequential pattern buffer
│  └─ identity           → rw: Persistent self-representation (768-dim)
├─ streams/
│  ├─ input              → w: Sensory perception pipeline
│  ├─ output             → r: Response generation channel
│  ├─ reflection         → r: Meta-cognitive monitoring stream
│  └─ telemetry          → r: Performance metrics stream
└─ control/
   ├─ power              → rw: Energy/arousal management
   ├─ clock              → rw: Temporal synchronization
   ├─ reset              → w: System reset trigger
   └─ config             → rw: Runtime configuration
```

## Device Driver Architecture

**Cognitive Hardware Abstraction Layer (CHAL):**

```go
// Driver interface - each cognitive subsystem implements this
type DeviceDriver interface {
    Load(pcb *VirtualPCB) error       // Attach to cognitive bus
    Initialize() error                 // Initialize hardware
    Probe() error                      // Detect and validate device
    Remove() error                     // Cleanup and detach
    Read(addr uint64) ([]byte, error) // MMIO read operation
    Write(addr uint64, data []byte) error // MMIO write operation
}

// Current driver implementations:
reservoir_driver.go          // Echo State Network management
consciousness_driver.go      // Layer communication (planned)
emotion_driver.go            // EPU state tracking (planned)
memory_driver.go             // Hypergraph operations (planned)
llm_driver.go                // Multi-provider LLM abstraction
```

**Driver Operations:**
```bash
# List available drivers
ecco9-driver list

# Load specific driver
ecco9-driver load reservoir --config configs/reservoir.yaml

# Check driver status
ecco9-driver status reservoir

# Reload driver (hot-swap)
ecco9-driver reload reservoir

# Unload driver
ecco9-driver unload reservoir

# Test driver functionality
ecco9-driver test reservoir --cycles 100
```

## Performance Characteristics

### Latency Targets

| Operation | Target | Typical | Hardware Bottleneck |
|-----------|--------|---------|---------------------|
| Reservoir update | <1ms | 0.8ms | CPU (matrix ops) |
| Consciousness layer message | <5ms | 3.2ms | Bus contention |
| Memory hypergraph query | <10ms | 7.5ms | Storage I/O |
| Emotion state update | <1ms | 0.5ms | Register write |
| Full EchoBeats cycle | <50ms | 42ms | Reason + Act phases |
| API response (simple) | <200ms | 187ms | LLM inference |
| API response (complex) | <1000ms | 850ms | Deep reasoning |

### Throughput Targets

| Resource | Rate | Sustained | Burst |
|----------|------|-----------|-------|
| Reservoir updates | 1000/sec | ✓ | 2500/sec |
| Memory operations | 10,000/sec | ✓ | 25,000/sec |
| API requests | 100/sec | ✓ | 250/sec |
| Consciousness messages | 5000/sec | ✓ | 10,000/sec |
| Emotion transitions | 50/sec | ✓ | 100/sec |

### Resource Usage

| Component | CPU | Memory | Storage |
|-----------|-----|--------|---------|
| Reservoir cores (4x) | 15-25% | 200MB | - |
| Consciousness layers | 5-10% | 100MB | - |
| Hypergraph memory | 5-8% | 2-4GB | 1-5GB |
| LLM inference (local) | 30-60% | 4-8GB | - |
| Emotion processing | <1% | 10MB | - |
| Total system | 50-90% | 6-12GB | 10-50GB |
