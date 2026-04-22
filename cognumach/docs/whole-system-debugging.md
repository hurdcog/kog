# Whole System Debugging for GNU Mach

This document describes the whole system debugging infrastructure implemented for GNU Mach, providing system-wide debugging capabilities, cross-component debugging support, and a unified debugging interface.

## Overview

The whole system debugging infrastructure consists of three main components:

1. **System-wide debugging infrastructure** - Coordinates debugging across all kernel subsystems
2. **Cross-component debugging support** - Tracks interactions between different subsystems  
3. **Unified debugging interface** - Provides a single point of access for all debugging functionality

## Architecture

### System Debug Subsystems

The debugging infrastructure recognizes the following kernel subsystems:

- `SYSDEBUG_SUBSYSTEM_KERNEL` - Core kernel services
- `SYSDEBUG_SUBSYSTEM_VM` - Virtual memory management
- `SYSDEBUG_SUBSYSTEM_IPC` - Inter-process communication
- `SYSDEBUG_SUBSYSTEM_SCHED` - Scheduler and threads
- `SYSDEBUG_SUBSYSTEM_DEVICE` - Device drivers and I/O
- `SYSDEBUG_SUBSYSTEM_NET` - Network stack

### Event Types

The system tracks the following types of events:

- `SYSDEBUG_EVENT_INIT` - Subsystem initialization
- `SYSDEBUG_EVENT_SHUTDOWN` - Subsystem shutdown
- `SYSDEBUG_EVENT_ERROR` - Error conditions
- `SYSDEBUG_EVENT_WARNING` - Warning conditions
- `SYSDEBUG_EVENT_TRACE` - Execution traces
- `SYSDEBUG_EVENT_INTERACTION` - Cross-subsystem interactions
- `SYSDEBUG_EVENT_RESOURCE` - Resource allocation/deallocation
- `SYSDEBUG_EVENT_STATE` - State changes

### Debug Levels

Five debugging levels provide granular control:

- `SYSDEBUG_LEVEL_NONE` - No debugging output
- `SYSDEBUG_LEVEL_MINIMAL` - Critical events only
- `SYSDEBUG_LEVEL_NORMAL` - Normal debugging
- `SYSDEBUG_LEVEL_VERBOSE` - Detailed debugging  
- `SYSDEBUG_LEVEL_TRACE` - Full trace debugging

## API Reference

### Core Functions

```c
// Initialize system debugging
void sysdebug_init(void);

// Enable/disable debugging
void sysdebug_enable(boolean_t enable);
boolean_t sysdebug_is_enabled(void);

// Set debugging levels
void sysdebug_set_global_level(sysdebug_level_t level);
void sysdebug_set_subsystem_level(sysdebug_subsystem_t subsystem, 
                                 sysdebug_level_t level);

// Report events
void sysdebug_report_event(sysdebug_subsystem_t subsystem,
                          sysdebug_event_t event,
                          const char *message, ...);
```

### Cross-Component Debugging

```c
// Enable cross-component tracking
void sysdebug_enable_cross_component_tracking(boolean_t enable);

// Track interactions between subsystems
void sysdebug_track_interaction(sysdebug_subsystem_t from_subsystem,
                               sysdebug_subsystem_t to_subsystem,
                               sysdebug_event_t event_type,
                               const void *context_data);

// Dump interaction history
void sysdebug_dump_cross_component_interactions(void);
```

### Unified Interface

```c
// Initialize all debugging subsystems
void unified_debug_init(void);

// Enable/disable all debugging
void unified_debug_enable_all(boolean_t enable);

// Comprehensive state dump
void unified_debug_dump_all_state(void);

// Subsystem-specific integration
void unified_debug_vm_operation(const char *operation, void *address, vm_size_t size);
void unified_debug_ipc_message(mach_msg_header_t *msg, boolean_t send);
void unified_debug_thread_create(thread_t thread);
```

## Usage Examples

### Basic System Debugging

```c
#include <mach/system_debug.h>

// Initialize and enable debugging
sysdebug_init();
sysdebug_enable(TRUE);

// Set debugging levels
sysdebug_set_global_level(SYSDEBUG_LEVEL_VERBOSE);
sysdebug_set_subsystem_level(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_LEVEL_TRACE);

// Report events
SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_EVENT_TRACE, 
            "Allocating %zu bytes at %p", size, address);

SYSDEBUG_LOG(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_EVENT_ERROR,
            "Failed to send message: error %d", error_code);
```

### Cross-Component Debugging

```c
// Enable cross-component tracking
sysdebug_enable_cross_component_tracking(TRUE);

// Track interactions
SYSDEBUG_TRACE_INTERACTION(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_SUBSYSTEM_IPC,
                          SYSDEBUG_EVENT_INTERACTION, "vm_to_ipc_call");

// View interaction history
sysdebug_dump_cross_component_interactions();
```

### Unified Debugging Interface

```c
#include <mach/unified_debug.h>

// Initialize unified debugging
unified_debug_init();
unified_debug_enable_all(TRUE);

// Use convenience macros
UNIFIED_DEBUG_RESOURCE_ALLOC(SYSDEBUG_SUBSYSTEM_VM, "memory", size, ptr);
UNIFIED_DEBUG_STATE_CHANGE(SYSDEBUG_SUBSYSTEM_SCHED, "RUNNING", "BLOCKED");
UNIFIED_DEBUG_ERROR(SYSDEBUG_SUBSYSTEM_DEVICE, "Device not responding");

// Comprehensive debugging dump
unified_debug_dump_all_state();
```

## Integration with Existing Tools

### GDB Integration

The whole system debugging integrates with the existing GDB stub:

```c
// Set up system-wide GDB breakpoints
unified_debug_gdb_setup_system_breakpoints();

// Enable cross-component breakpoints
unified_debug_gdb_enable_cross_component_breaks(TRUE);
```

### Debug Scripts Integration

The system integrates with existing debug scripts in `scripts/debug-*`:

```bash
# Use debug master script with system debugging
./scripts/debug-master.sh scenario interactive

# In GDB, use unified debugging commands
(gdb) call unified_debug_dump_all_state()
(gdb) call sysdebug_dump_cross_component_interactions()
```

## Configuration

### Compile-time Configuration

The whole system debugging is enabled by default but can be controlled with:

```c
// In debug builds, all debugging is available
#ifdef DEBUG
#define SYSDEBUG_ENABLED 1
#endif

// Integration with existing KDB
#if MACH_KDB
// GDB integration available
#endif
```

### Runtime Configuration

```c
// Enable debugging for specific subsystems
sysdebug_set_subsystem_level(SYSDEBUG_SUBSYSTEM_VM, SYSDEBUG_LEVEL_TRACE);
sysdebug_set_subsystem_level(SYSDEBUG_SUBSYSTEM_IPC, SYSDEBUG_LEVEL_NORMAL);

// Enable cross-component tracking
sysdebug_enable_cross_component_tracking(TRUE);

// Use unified interface for convenience
unified_debug_enable_all(TRUE);
```

## Performance Considerations

### Minimal Overhead

- Event reporting has minimal overhead when debugging is disabled
- Cross-component tracking uses a small ring buffer
- Statistics tracking uses simple counters

### Debug Level Control

- Fine-grained control per subsystem
- Global override capability
- Runtime level changes without restart

### Memory Usage

- Fixed-size data structures
- Ring buffer for interaction history (32 entries)
- No dynamic memory allocation in debug paths

## Testing

### Automated Tests

Run the whole system debugging test:

```bash
make test-whole-system-debugging
```

This test validates:
- Basic system debugging functionality
- Cross-component debugging
- Unified debugging interface
- Integration with kernel subsystems
- Statistics and session management

### Manual Testing

```bash
# Enable debugging in a running kernel
echo "unified_debug_enable_all(1)" > /proc/mach_debug

# Check debugging state
echo "unified_debug_dump_all_state()" > /proc/mach_debug

# Test cross-component tracking
echo "sysdebug_enable_cross_component_tracking(1)" > /proc/mach_debug
```

## Troubleshooting

### Common Issues

1. **Debugging not enabled**: Ensure `unified_debug_init()` was called during kernel startup
2. **No output**: Check debugging levels with `sysdebug_get_subsystem_level()`
3. **Too much output**: Reduce debug levels or disable specific subsystems

### Debug Output

Debug output appears with prefixes indicating the source:
```
[SYSDEBUG:VM:TRACE] VM operation: page_fault at 0x40000000, size 4096
[SYSDEBUG:IPC:ERROR] Failed to send message: port dead
[UNIFIED_DEBUG] Performance measurement started: vm_allocation
```

### Performance Impact

Monitor debug overhead with:
```c
// Get debugging statistics
unsigned long stats[SYSDEBUG_EVENT_MAX];
sysdebug_get_statistics(SYSDEBUG_SUBSYSTEM_VM, stats);

// Reset counters
sysdebug_reset_statistics();
```

## Future Enhancements

### Planned Features

1. **Dynamic tracing**: Integration with DTrace-style probes
2. **Network debugging**: Remote debugging over network
3. **Visualization**: Tools for visualizing cross-component interactions
4. **Performance analysis**: Detailed timing and profiling integration

### Extension Points

The debugging infrastructure is designed for extension:

- Add new subsystems by extending `sysdebug_subsystem_t`
- Add new event types by extending `sysdebug_event_t`  
- Custom debugging contexts via `context_data` parameter
- Integration with external debugging tools

## References

- [GNU Mach Debugging Guide](debugging-guide.md)
- [Startup Debugging Guide](startup-debugging-guide.md)
- [GDB Stub Enhancement](gdb-stub-enhancement.md)
- [Enhanced Instrumentation](enhanced-instrumentation.md)

---

*This whole system debugging infrastructure addresses the requirements for system-wide debugging, cross-component debugging support, and unified debugging interface as specified in issue #118.*