# WinKoGNN Integration Analysis for OCC

## Overview

This document analyzes the WinKoGNN (Windows Kognitive Graph Neural Network) repository and identifies integration opportunities with the cogpy/occ (OpenCog Collection) project.

## WinKoGNN Key Components

### 1. OpenCog Workbench Architecture
- **Multi-Agent Orchestrator**: Coordinates autonomous agents for distributed cognitive processing
- **AtomSpace Manager**: Python implementation of hypergraph-based knowledge representation
- **Reasoning Engine**: Logical inference with deduction, induction, abduction, modus ponens
- **Graph Neural Network**: Deep learning over knowledge graphs with link prediction
- **Agent-Zero Hypervisor**: VM management for agent isolation
- **NT4 Kernel Bridge**: Low-level system integration (Windows NT4 kernel source included)

### 2. GNN Integration with AtomSpace
The WinKoGNN provides a clean Python implementation of:
- Graph extraction from AtomSpace
- Node feature creation from atom properties (truth values, attention, connectivity)
- Self-supervised learning via link prediction
- Node embedding computation and similarity metrics

## OCC Current State

### Existing Components
1. **CogGML Microkernel**: Self-aware cognitive shards with distributed processing
2. **AtomSpace**: C++ implementation with Scheme bindings
3. **PLN**: Probabilistic Logic Networks for reasoning
4. **ECAN**: Economic Attention Networks
5. **URE**: Unified Rule Engine

### Missing/Incomplete
1. **GNN Integration**: No Python GNN module for AtomSpace
2. **Multi-Agent Orchestration**: Limited agent coordination
3. **Autognostic LLM Integration**: Per roadmap, needs implementation

## Integration Opportunities

### Priority 1: Fix Build Errors
- **atomspace-storage**: Missing `debian/source/format` file (FIXED)

### Priority 2: GNN Module Integration
Integrate WinKoGNN's GNN module into OCC:
```
occ/
├── gnn/
│   ├── __init__.py
│   ├── graph_network.py      # From WinKoGNN
│   ├── atomspace_bridge.py   # Bridge to C++ AtomSpace
│   └── link_predictor.py     # Link prediction utilities
```

### Priority 3: Multi-Agent Orchestration
Leverage WinKoGNN's orchestration patterns:
- Integrate with CogGML's shard coordination
- Add capability-based task routing
- Implement agent lifecycle management

### Priority 4: Autognostic Integration (Per Roadmap)
Align with implementation roadmap:
- Phase 1: Diagnostic atom types
- Phase 2: LLM integration for introspection
- Phase 3: Repair execution
- Phase 4: Environment loop
- Phase 5: Hierarchical expansion

## Implementation Plan

### Step 1: Create GNN Module
1. Copy WinKoGNN GNN components to OCC
2. Create bridge to C++ AtomSpace via Python bindings
3. Add integration tests

### Step 2: Enhance CogGML
1. Add GNN-based shard coordination
2. Implement attention-based message routing
3. Add neural-symbolic hybrid processing

### Step 3: Workflow Updates
1. Add GNN build/test to CI workflows
2. Create Python package for GNN module
3. Update documentation

## Files to Create/Modify

### New Files
- `occ/gnn/__init__.py`
- `occ/gnn/graph_network.py`
- `occ/gnn/atomspace_bridge.py`
- `occ/gnn/requirements.txt`
- `occ/gnn/tests/test_gnn.py`

### Modified Files
- `opencog-debian/atomspace-storage/debian/source/format` (FIXED)
- `.github/workflows/cogci.yml` (add GNN tests)
- `requirements.txt` (add numpy)

## Alignment with Roadmap

| Roadmap Phase | WinKoGNN Component | Integration Action |
|--------------|-------------------|-------------------|
| Phase 1: Foundation | AtomSpace Manager | Bridge to C++ AtomSpace |
| Phase 2: LLM Integration | Reasoning Engine | Extend for LLM prompts |
| Phase 3: Repair Execution | Multi-Agent Orchestrator | Task routing for repairs |
| Phase 4: Environment Loop | GNN Backend | Embedding-based state representation |
| Phase 5: Hierarchical | Agent-Zero Hypervisor | Multi-level agent hierarchy |

## Next Steps

1. ✅ Fix atomspace-storage debian/source/format
2. Create GNN module directory structure
3. Port WinKoGNN GNN code with OCC adaptations
4. Create AtomSpace bridge for Python bindings
5. Add CI workflow for GNN testing
6. Update documentation
