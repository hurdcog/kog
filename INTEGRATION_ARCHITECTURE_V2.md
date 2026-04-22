# AGI-OS Integration Architecture v2.0

**Version**: 2.0  
**Date**: December 12, 2025  
**Status**: Production Architecture with Corrected Dependencies  
**Supersedes**: INTEGRATION_ARCHITECTURE.md v1.0

## Critical Updates in v2.0

This version corrects critical omissions and consolidates integration components:

1. **Added atomspace-storage package** - Previously missing mandatory component
2. **Consolidated MIG** - Single source in cognumach, symlinked from hurdcog
3. **Corrected dependency chain** - Fixed cogserver and storage backend dependencies
4. **Updated build order** - Reflects actual dependency requirements

## Three-Layer Architecture

### Layer 0: CognuMach (Cognitive Microkernel)
- **Location**: `/cognumach/`
- **MIG Location**: `cognumach/mig/` (canonical source)
- **Dependencies**: None (foundation layer)

### Layer 1: HurdCog (Cognitive Operating System)
- **Location**: `/hurdcog/`
- **MIG Reference**: `hurdcog/mig -> ../cognumach/mig` (symlink)
- **Dependencies**: CognuMach

### Layer 2: OCC (OpenCog Collection)
- **Location**: Root level components
- **Dependencies**: CogUtil (required), HurdCog (optional for integration)

## Corrected Dependency Chain

```
cogutil (no deps)
  └── atomspace
      └── atomspace-storage (CRITICAL - was missing!)
          ├── atomspace-rocks
          ├── atomspace-pgres
          └── cogserver
              ├── attention
              ├── learn
              └── atomspace-cog
```

## Integration Bridges

### 1. AtomSpace-MachSpace Bridge
- **Package**: `atomspace-machspace`
- **Dependencies**: atomspace-storage, hurdcog
- **Purpose**: Kernel-level distributed hypergraph storage

### 2. Cognitive Scheduler
- **Package**: `cognumach-cognitive-scheduler`
- **Dependencies**: cognumach, hurdcog-machspace
- **Purpose**: Attention-aware CPU scheduling

### 3. HurdCog-OCC Bridge
- **Package**: `hurdcog-occ-bridge`
- **Dependencies**: hurdcog-machspace, ure
- **Purpose**: High-level OCC integration with kernel services

## Build Infrastructure

### Corrected Build Order

1. cogutil
2. atomspace
3. **atomspace-storage** ← NEW: Critical addition
4. atomspace-rocks, atomspace-pgres (parallel)
5. cogserver
6. ure
7. hurdcog
8. hurdcog-machspace
9. Integration bridges
10. Cognitive components

### Build Scripts

- **Debian packages**: `opencog-debian/build-all-corrected.sh`
- **Integration layer**: `integration/build-integration-layer.sh`

### MIG Consolidation

- **Canonical source**: `cognumach/mig/`
- **HurdCog reference**: `hurdcog/mig -> ../cognumach/mig`
- **Backup**: `hurdcog/mig.backup/` (for rollback)

## Package Count

**Total**: 33 packages (32 previously documented + atomspace-storage)

## Directory Structure

```
occ/
├── cognumach/
│   └── mig/                      # Canonical MIG source
├── hurdcog/
│   ├── mig -> ../cognumach/mig   # Symlink to canonical MIG
│   └── mig.backup/               # Original backup
├── atomspace-storage/            # NEW: Storage API component
├── integration/
│   ├── atomspace-machspace/
│   ├── cognitive-scheduler/
│   └── hurdcog-bridge/
└── opencog-debian/
    ├── atomspace-storage/        # NEW: Debian package
    └── ...
```

## Documentation References

- **Build Order**: `opencog-debian/BUILD_ORDER_CORRECTED.md`
- **MIG Consolidation**: `MIG_CONSOLIDATION.md`
- **Analysis**: `analysis_findings.md`
- **Original Architecture**: `INTEGRATION_ARCHITECTURE.md` (v1.0)

## Key Improvements

1. **Complete dependency chain** - No missing components
2. **Single MIG source** - Eliminates duplication
3. **Corrected build order** - Prevents build failures
4. **Production-ready packaging** - All 33 packages complete

## See Also

- Original architecture document: `INTEGRATION_ARCHITECTURE.md`
- Detailed build order: `opencog-debian/BUILD_ORDER_CORRECTED.md`
- Integration layer: `integration/README.md`
