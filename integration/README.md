# Integration Layer

**Purpose**: Provide integration bridges between Cognumach, HurdCog, and OCC layers.

## Components

### 1. AtomSpace-MachSpace Bridge

**Location**: `integration/atomspace-machspace/`

**Purpose**: Enable AtomSpace to use MachSpace as a distributed storage backend.

**Key Features**:
- Distributed hypergraph storage at kernel level
- Shared memory atom access
- Copy-on-write atom management
- Attention value synchronization
- IPC-based communication with HurdCog

**API**: Implements AtomSpace `StorageNode` interface

**Usage**:
```cpp
#include <opencog/persist/machspace/MachSpaceStorageNode.h>

// Create storage node
Handle storage = createNode(MACHSPACE_STORAGE_NODE, "machspace:///");

// Open connection
storage->open();

// Store atoms
storage->store_atom(some_atom);

// Fetch atoms
Handle fetched = storage->fetch_atom(some_handle);

// Close connection
storage->close();
```

**Build**:
```bash
cd integration/atomspace-machspace
mkdir build && cd build
cmake ..
make
sudo make install
```

### 2. Cognitive Scheduler

**Location**: `integration/cognitive-scheduler/`

**Purpose**: Provide attention-aware CPU scheduling for Cognumach microkernel.

**Key Features**:
- Attention-based CPU allocation
- Learning-based scheduling policies
- Cognitive workload classification
- Tensor Thread Fiber scheduling support
- Integration with ECAN attention values

**API**: C interface for kernel integration

**Usage**:
```c
#include "cognitive_scheduler.h"

// Initialize scheduler
cognitive_sched_init(SCHED_POLICY_ATTENTION);

// Set attention value for a task
attention_value_t av = {
    .sti = 0.9,
    .lti = 0.7,
    .vlti = 0.5
};
cognitive_sched_set_attention(task_id, &av);

// Classify workload
cognitive_sched_classify_workload(task_id, WORKLOAD_REASONING);

// Run learning algorithm
cognitive_sched_learn();

// Get statistics
sched_stats_t stats;
cognitive_sched_get_stats(&stats);
```

**Build**:
```bash
cd integration/cognitive-scheduler
gcc -c cognitive_scheduler.c -o cognitive_scheduler.o
ar rcs libcognitive_scheduler.a cognitive_scheduler.o
```

### 3. CogKernel Bridge

**Location**: `integration/cogkernel-bridge/`

**Purpose**: Enable URE rule execution at kernel level via HurdCog CogKernel.

**Key Features**:
- Kernel-level rule execution
- Distributed reasoning coordination
- System-wide inference
- MachSpace integration

**Status**: To be implemented

**Planned API**:
```scheme
;; Execute rule at kernel level
(cogkernel-execute-rule rule-handle)

;; Register rule for system-wide inference
(cogkernel-register-rule rule-handle)

;; Query kernel-level knowledge
(cogkernel-query pattern)
```

## Integration Architecture

```
┌─────────────────────────────────────────────────────────┐
│  OCC Layer (OpenCog Collection)                         │
│  - AtomSpace, PLN, ECAN, URE, Learning                  │
├─────────────────────────────────────────────────────────┤
│  Integration Bridges                                    │
│  - AtomSpace ↔ MachSpace                               │
│  - ECAN ↔ Cognitive Scheduler                          │
│  - URE ↔ CogKernel                                     │
├─────────────────────────────────────────────────────────┤
│  HurdCog Layer (Cognitive Operating System)             │
│  - MachSpace, CogKernel, Cognitive Translators          │
├─────────────────────────────────────────────────────────┤
│  Cognumach Layer (Cognitive Microkernel)                │
│  - Enhanced Mach, Cognitive Scheduler, IPC              │
└─────────────────────────────────────────────────────────┘
```

## Build Order

1. **Cognumach** (microkernel)
2. **HurdCog** (operating system)
3. **Cognitive Scheduler** (integration component)
4. **OCC Core** (CogUtil, AtomSpace)
5. **AtomSpace-MachSpace Bridge** (integration component)
6. **OCC Cognitive Components** (ECAN, PLN, URE)
7. **CogKernel Bridge** (integration component)

## Testing

Each integration component includes test programs:

- `integration/atomspace-machspace/test_machspace.cc`
- `integration/cognitive-scheduler/test_scheduler.c`
- `integration/cogkernel-bridge/test_cogkernel.scm`

Run tests after building:
```bash
cd integration
./run_integration_tests.sh
```

## Development Status

- [x] AtomSpace-MachSpace Bridge: Stub implementation
- [x] Cognitive Scheduler: Stub implementation
- [ ] CogKernel Bridge: To be implemented
- [ ] Full IPC integration: To be implemented
- [ ] Performance optimization: To be implemented
- [ ] Production testing: To be implemented

## Contributing

When adding new integration components:

1. Create subdirectory under `integration/`
2. Implement header and source files
3. Add CMakeLists.txt or Makefile
4. Document API in README
5. Add test programs
6. Update this README

## References

- [Integration Architecture](../INTEGRATION_ARCHITECTURE.md)
- [Repository Analysis](../REPOSITORY_ANALYSIS.md)
- [Build Order](../opencog-debian/BUILD_ORDER_ENHANCED.md)
