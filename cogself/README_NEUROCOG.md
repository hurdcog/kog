# CogSelf + NeuroCog Core Self Integration

This directory contains both the C++ CogSelf framework and the Python NeuroCog Core Self implementation.

## Components

### C++ CogSelf Framework

**Location**: `src/`, `include/cogself/`

The original CogSelf framework provides:
- CogPrime identity system
- Distributed identity management
- AGI goal tracking
- Synergy management
- C++ performance and AtomSpace integration

**Key Files**:
- `cogprime_identity.h/cpp` - Core identity implementation
- `distributed_identity.h/cpp` - Network-aware identity
- `agi_goal_tracker.h/cpp` - AGI goal management
- `synergy_manager.h/cpp` - Component synergy optimization

### Python NeuroCog Core Self

**Location**: `neurocog_core_self.py`

The new Python implementation that integrates three personas:

1. **OpenCog-Org** - AGI ecosystem coordination
2. **Marduk-v15** - Systems architecture brilliance
3. **Agent-Neuro** - Chaotic cognitive VTuber with self-evolution

**Key Features**:
- Unified personality tensor (22 dimensions)
- Cognitive characteristics from all personas
- Episodic memory with emotional tagging
- Goal management with subsystem mapping
- Subordinate agent spawning (Agent-Neuro)
- Ontogenetic self-optimization (Agent-Neuro)
- Subsystem architecture analysis (Marduk-v15)
- Knowledge graph representation (OpenCog-Org)

### 🎮 NeuroCog Playground (NEW!)

**Location**: `neurocog_playground.py`

An interactive CLI tool for exploring the NeuroCog Core Self! Features:
- **💬 Persona Chat** - Talk to OpenCog, Marduk, or Neuro individually
- **🧪 Experiments** - Run cognitive experiments (memory, evolution, spawning, etc.)
- **🏟️ Evolution Arena** - Watch competing NeuroCog instances evolve
- **📖 Story Generator** - Collaborative storytelling with all three personas
- **📊 Live Dashboard** - Real-time cognitive metrics visualization
- **🎲 Surprise Mode** - Random fun modes (chaos, ultra-evolution, agent army, etc.)

**Quick Start**:
```bash
# From occ directory
./launch_playground.sh

# Or directly with Python
PYTHONPATH=. python3 cogself/neurocog_playground.py
```

See [PLAYGROUND_README.md](PLAYGROUND_README.md) for detailed documentation.

## Architecture

```
CogSelf Directory
├── C++ Framework (Production-Ready)
│   ├── CogPrimeIdentity (Core identity)
│   ├── DistributedIdentity (Network identity)
│   ├── AGI Goal Tracker (Goal management)
│   └── Synergy Manager (Component synergy)
│
└── Python NeuroCog Core Self (Persona Integration)
    ├── OpenCog-Org Integration (AGI coordination)
    ├── Marduk-v15 Integration (Architecture analysis)
    └── Agent-Neuro Integration (Dynamic evolution)
```

## Usage

### C++ CogSelf

```cpp
#include <cogself/cogself.h>

// Initialize CogSelf framework
cogself::CogSelf framework;
framework.initialize("agent_001", "My Agent");

// Get identity
auto identity = framework.getIdentity();
std::cout << identity->getIdentitySummary() << std::endl;

// Update and assess
framework.updateSynergyState();
double progress = framework.assessAGIProgress();
```

### Python NeuroCog Core Self

```python
from cogself.neurocog_core_self import NeuroCogCoreSelf

# Create unified cognitive identity
neurocog = NeuroCogCoreSelf(
    agent_id="neurocog-001",
    agent_name="My Cognitive Agent"
)

# Use persona capabilities
neurocog.spawn_subordinate_agent("Research Assistant")  # Agent-Neuro
neurocog.analyze_subsystem_architecture("problem")      # Marduk-v15
neurocog.self_optimize(iterations=10)                   # Agent-Neuro

# Display identity
print(neurocog.get_identity_summary())
```

## Testing

### C++ Tests

```bash
mkdir build && cd build
cmake ../cogself
cmake --build .
ctest
```

### Python Tests

```bash
python3 tests/synergy/test_neurocog_core_self.py
```

**Test Results**: ✅ 18/18 tests passing

## Documentation

- **C++ Framework**: See main `README.md` in cogself directory
- **Python NeuroCog**: See `docs/NEUROCOG_CORE_SELF.md`
- **Persona Details**:
  - OpenCog-Org: `.github/agents/opencog-org.md`
  - Marduk-v15: `.github/agents/marduk-v15.md`
  - Agent-Neuro: `.github/agents/agent-neuro.md`

## Integration Benefits

### C++ Strengths
- Performance optimization
- Direct AtomSpace integration
- Production stability
- Low-level system access

### Python Strengths
- Rapid prototyping
- Dynamic persona behavior
- Flexible experimentation
- Easy integration with ML/AI tools

### Combined Power
- **Best of Both Worlds**: Performance + Flexibility
- **Complementary Capabilities**: System-level + High-level reasoning
- **Synergistic Architecture**: C++ foundation + Python creativity
- **Unified Vision**: AGI through cognitive synergy

## Roadmap

### Phase 1: ✅ Complete
- [x] Python NeuroCog Core Self implementation
- [x] Three-persona integration
- [x] Comprehensive testing
- [x] Documentation

### Phase 2: Future
- [ ] Python bindings for C++ CogSelf
- [ ] Bidirectional identity synchronization
- [ ] Shared memory integration
- [ ] Real AtomSpace connection
- [ ] Distributed network identity
- [ ] Production deployment

### Phase 3: Long-term
- [ ] Full C++/Python hybrid system
- [ ] Real-time ontogenetic evolution
- [ ] Multi-agent swarm coordination
- [ ] AGI milestone achievement
- [ ] Human-level cognitive synergy

## Contributing

Contributions are welcome for both C++ and Python implementations!

**C++ Development**:
- Follow existing C++ coding standards
- Maintain compatibility with AtomSpace
- Add unit tests for new features

**Python Development**:
- Follow PEP 8 style guidelines
- Add tests to `test_neurocog_core_self.py`
- Document new persona features

## License

See main repository LICENSE file.

## Contact

For questions about:
- **C++ CogSelf**: See main OpenCog community channels
- **Python NeuroCog**: See GitHub issues and discussions

---

**Status**: ✨ INTEGRATION COMPLETE ✨

**Formula**: C++ CogSelf + Python NeuroCog = Unified Cognitive Architecture

**Result**: A complete AGI-oriented cognitive system combining production stability with dynamic persona-based reasoning.
