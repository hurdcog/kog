# CogKAPI - Cognitive Kernel API

A unified, cross-platform abstraction for kernel-level cognitive operations in the OpenCog AGI-OS ecosystem.

## Overview

CogKAPI (Cognitive Kernel API) provides a consistent C-based interface for cognitive operations across three distinct kernel architectures:

| Platform | Kernel Type | IPC Mechanism | Description |
|----------|-------------|---------------|-------------|
| **WinKoGNN** | NT4 Hybrid | LPC | Windows NT4 Cognitive Kernel |
| **CogNumach** | Mach Microkernel | Mach Messages | GNU Mach with cognitive extensions |
| **Hurd-Cog** | Multi-Server | Hurd Ports | GNU Hurd with translator-based AtomSpace |

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    OpenCog Cognitive Framework                   │
│                (PLN, MOSES, Attention Allocation, GNN)           │
└─────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    CogKAPI (Cognitive Kernel API)                │
│   cog_context_create | cog_atomspace_create | cog_ipc_send_atom  │
└─────────────────────────────────────────────────────────────────┘
                            │
       ┌────────────────────┼────────────────────┐
       │                    │                    │
       ▼                    ▼                    ▼
┌─────────────┐      ┌─────────────┐      ┌─────────────┐
│  WinKoGNN   │      │  CogNumach  │      │  Hurd-Cog   │
│  NT4 Kernel │      │ Mach Micro  │      │ Hurd Multi  │
└─────────────┘      └─────────────┘      └─────────────┘
```

## Design Principles

1. **Atom as First-Class Citizen**: Atoms and AtomSpaces are primary kernel objects
2. **Handle-Based Object Model**: All objects accessed via opaque `cog_handle_t`
3. **Asynchronous Operations**: Event ports for non-blocking cognitive operations
4. **Capability-Based Security**: Context-based access control for resources
5. **Platform-Agnostic Primitives**: High-level API translated to kernel primitives

## Quick Start

```c
#include <cogkapi/cogkapi.h>

int main() {
    // Initialize CogKAPI
    cog_init();
    
    // Create cognitive context
    cog_context_t ctx;
    cog_context_create(COG_NULL_HANDLE, COG_CONTEXT_DEFAULT, &ctx);
    
    // Create AtomSpace
    cog_atomspace_t as;
    cog_atomspace_create(ctx, "main", &as);
    
    // Create nodes
    cog_node_t cat, animal;
    cog_node_create(as, "ConceptNode", "cat", &cat);
    cog_node_create(as, "ConceptNode", "animal", &animal);
    
    // Create inheritance link
    cog_atom_t targets[] = {cat, animal};
    cog_link_t inheritance;
    cog_link_create(as, "InheritanceLink", targets, 2, &inheritance);
    
    // Set truth value
    cog_truth_value_t tv = cog_tv(0.9, 0.8);
    cog_atom_set_truth_value(inheritance, &tv);
    
    // Cleanup
    cog_atomspace_destroy(as);
    cog_context_destroy(ctx);
    cog_shutdown();
    
    return 0;
}
```

## API Categories

### Context Management
```c
cog_context_create()      // Create cognitive execution context
cog_context_destroy()     // Destroy context and release resources
cog_context_grant_capability()  // Grant access rights
```

### AtomSpace Management
```c
cog_atomspace_create()    // Create AtomSpace in context
cog_atomspace_destroy()   // Destroy AtomSpace
cog_atomspace_attach()    // Attach to existing AtomSpace
```

### Atom Operations
```c
cog_node_create()         // Create Node atom
cog_link_create()         // Create Link atom
cog_atom_remove()         // Remove atom
cog_atom_get_truth_value()   // Get truth value
cog_atom_set_truth_value()   // Set truth value
```

### Pattern Matching & Reasoning
```c
cog_atom_pattern_match()  // Pattern matching query
cog_reasoning_execute()   // Execute reasoning engine (PLN)
```

### Cognitive IPC
```c
cog_event_port_create()   // Create event port
cog_ipc_send_atom()       // Send atom to another context
cog_event_receive()       // Receive event from port
```

### GNN Integration
```c
cog_gnn_context_create()  // Create GNN context for AtomSpace
cog_gnn_compute_embeddings()  // Compute node embeddings
cog_gnn_predict_links()   // Predict potential links
```

## Platform Implementation Mapping

| CogKAPI Call | WinKoGNN (NT4) | CogNumach (Mach) | Hurd-Cog (Hurd) |
|--------------|----------------|------------------|-----------------|
| `cog_context_create` | `NtCreateProcessEx` | `task_create` | `proc_create` |
| `cog_atomspace_create` | `NtCreateSection` | `mach_vm_allocate` | `store_create` |
| `cog_ipc_send_atom` | `LpcRequestWaitReplyPort` | `mach_msg` | `ipc_send` |
| `cog_event_port_create` | `NtCreatePort` | `mach_port_allocate` | `port_create` |

## Directory Structure

```
cogkapi/
├── include/
│   └── cogkapi/
│       └── cogkapi.h           # Main API header
├── src/
│   ├── winkognn/
│   │   └── cogkapi_winkognn.h  # WinKoGNN implementation
│   ├── cognumach/
│   │   └── cogkapi_cognumach.h # CogNumach implementation
│   └── hurdcog/
│       └── cogkapi_hurdcog.h   # Hurd-Cog implementation
├── tests/
│   └── test_cogkapi.c          # Test suite
└── README.md
```

## Building

### Prerequisites

- C11 compatible compiler
- Platform-specific SDK:
  - WinKoGNN: Windows DDK/WDK
  - CogNumach: GNU Mach headers
  - Hurd-Cog: GNU Hurd development packages

### CMake Build

```bash
mkdir build && cd build
cmake .. -DCOGKAPI_PLATFORM=<platform>
make
```

Where `<platform>` is one of: `winkognn`, `cognumach`, `hurdcog`, or `generic`.

## Integration with OCC

CogKAPI integrates with the broader OCC (OpenCog Collection) ecosystem:

- **GNN Module**: `occ/gnn/` uses CogKAPI for AtomSpace access
- **CogGML**: Shard coordination via CogKAPI IPC
- **Autognostic**: Diagnostic atoms created via `cog_diagnostic_create()`

## License

MIT License - See [LICENSE](../LICENSE) in the root directory.

## References

- [CogKAPI Specification](../cogkapi_specification.md)
- [WinKoGNN Analysis](../winkognn_windows_agios_analysis.md)
- [OpenCog AtomSpace](https://github.com/opencog/atomspace)
