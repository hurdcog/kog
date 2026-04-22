# OCC-HurdCog Integration Components

This document describes the new integration components added to enable seamless communication between OCC, HurdCog, and Cognumach for the AGI-OS stack.

---

## New Components

### 1. AtomSpace-HurdCog Bridge (`atomspace-hurdcog-bridge/`)

Provides bidirectional communication between OpenCog AtomSpace and HurdCog cognitive kernel.

**Features**:
- Efficient atom serialization for kernel storage
- IPC mechanism using Mach ports
- Asynchronous callback support
- Python and C++ interfaces
- Performance monitoring and statistics

**Quick Start**:
```python
from atomspace_hurdcog import AtomSpaceHurdCogBridge

bridge = AtomSpaceHurdCogBridge()
bridge.initialize()
```

### 2. Cognitive Dashboard (`cognitive-dashboard/`)

Real-time monitoring and visualization of AGI-OS cognitive state.

**Features**:
- AtomSpace statistics and visualization
- PLN inference tracking
- ECAN attention allocation monitoring
- HurdCog kernel status
- System performance metrics
- Web-based interface

**Quick Start**:
```bash
cd cognitive-dashboard
python3 server.py
# Access at http://localhost:5000
```

### 3. Enhanced Autogenesis (`autogenesis-enhanced/`)

Autonomous system evolution and self-improvement framework.

**Features**:
- Automatic discovery of improvement opportunities
- Code generation for enhancements
- Safety validation and testing
- Automatic rollback on failure
- Evolution history tracking

**Quick Start**:
```python
from evolution_manager import EvolutionManager

manager = EvolutionManager()
candidates = manager.discover_evolution_opportunities()
```

---

## Integration with Existing Deep Tree Echo Architecture

These new components complement the existing Deep Tree Echo Architecture (Phase 1) by providing:

- **Bridge to HurdCog**: Connects the Hypergraph Memory Space to the HurdCog cognitive kernel
- **Monitoring**: Visualizes the Echo Propagation Engine and Cognitive Grammar Kernel in action
- **Evolution**: Enables autonomous improvement of the entire AGI-OS stack

The integration components work alongside the existing `core/` modules without modification.

---

## Documentation

- **Integration Guide**: `../docs/integration/HURDCOG_INTEGRATION_GUIDE.md`
- **Architecture Overview**: `../docs/architecture.md`
- **AGI-OS Integration Plan**: `../AGI_OS_INTEGRATION_PLAN.md`

---

## License

GPL 3.0 or later, consistent with OpenCog and GNU Hurd licensing.
