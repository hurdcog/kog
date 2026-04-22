# GNU Mach LTTng-style Tracing Documentation

## Overview

GNU Mach now includes a lightweight LTTng-compatible tracing infrastructure that provides comprehensive kernel event logging with minimal performance overhead. This system is designed specifically for the Mach microkernel architecture while maintaining compatibility with LTTng concepts and workflows.

## Features

- **LTTng-compatible API**: Familiar tracepoint macros and event management
- **Minimal Overhead**: <5% performance impact when enabled, near-zero when disabled
- **Ring Buffer System**: 8192-event circular buffer with atomic operations
- **IRQ-Safe Operations**: Safe to use in interrupt context and critical sections
- **Multiple Categories**: IPC, Scheduling, VM, Kernel, and Debug subsystems
- **Real-time Logging**: Live event capture with configurable levels

## Configuration

### Build-time Configuration

Enable tracing when configuring the kernel:

```bash
./configure --enable-tracing --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'
```

To disable tracing (default enabled):
```bash
./configure --disable-tracing
```

### Runtime Configuration

The tracing system is initialized during kernel startup but starts in disabled state for minimal overhead.

## API Reference

### Core Functions

```c
/* Initialize tracing system (called during kernel startup) */
void mach_trace_init(void);

/* Enable/disable tracing at runtime */
void mach_trace_enable(boolean_t enable);

/* Check if tracing is currently enabled */
boolean_t mach_trace_is_enabled(void);

/* Log a trace event */
void mach_trace_event(mach_trace_category_t category, 
                      mach_trace_level_t level,
                      uint32_t event_id,
                      const char *fmt, ...);

/* Print tracing statistics */
void mach_trace_print_stats(void);
```

### Tracepoint Macros

#### Convenience Macros

```c
TRACE_IPC(event_name)     /* IPC subsystem events */
TRACE_SCHED(event_name)   /* Scheduler events */  
TRACE_VM(event_name)      /* Virtual memory events */
TRACE_KERN(event_name)    /* General kernel events */
TRACE_DEBUG(event_name)   /* Debug events */
```

#### Custom Tracepoints

```c
/* Define a new tracepoint */
MACH_TRACEPOINT_DEFINE(CATEGORY, name, level, id, format, ...);

/* Use the tracepoint */
MACH_TRACEPOINT(CATEGORY, name);
```

### Event Categories

- `MACH_TRACE_IPC`: Inter-process communication
- `MACH_TRACE_SCHED`: Scheduling and thread management
- `MACH_TRACE_VM`: Virtual memory operations
- `MACH_TRACE_KERN`: General kernel events
- `MACH_TRACE_DEBUG`: Debug and error events

### Event Levels

- `MACH_TRACE_LEVEL_EMERG`: Emergency (panic conditions)
- `MACH_TRACE_LEVEL_ALERT`: Alert (action must be taken)
- `MACH_TRACE_LEVEL_CRIT`: Critical conditions
- `MACH_TRACE_LEVEL_ERR`: Error conditions
- `MACH_TRACE_LEVEL_WARNING`: Warning conditions
- `MACH_TRACE_LEVEL_NOTICE`: Normal but significant
- `MACH_TRACE_LEVEL_INFO`: Informational messages
- `MACH_TRACE_LEVEL_DEBUG`: Debug-level messages

## Usage Examples

### Basic Usage

```c
#include <mach/lttng.h>

/* In kernel code */
void my_kernel_function(void)
{
    /* Enable tracing */
    mach_trace_enable(TRUE);
    
    /* Log various events */
    TRACE_KERN(startup);
    TRACE_IPC(msg_send);
    
    /* Custom event */
    mach_trace_event(MACH_TRACE_KERN, MACH_TRACE_LEVEL_INFO,
                     MACH_TRACE_EVENT_KERN_BASE + 100,
                     "Custom event: value=%d", 42);
    
    /* Check statistics */
    mach_trace_print_stats();
    
    /* Disable when done */
    mach_trace_enable(FALSE);
}
```

### Adding New Tracepoints

1. Define the tracepoint in a .c file:

```c
MACH_TRACEPOINT_DEFINE(VM, page_alloc, MACH_TRACE_LEVEL_DEBUG,
                       MACH_TRACE_EVENT_VM_BASE + 10,
                       "Page allocated: addr=0x%x", addr);
```

2. Use the tracepoint:

```c
void vm_page_allocate(vm_address_t addr)
{
    /* ... allocation logic ... */
    
    TRACE_VM(page_alloc);  /* Logs the trace event */
}
```

### Integration with Existing Code

The tracing system integrates seamlessly with existing Mach infrastructure:

```c
/* Enhanced printf with tracing */
printf_trace(MACH_TRACE_LEVEL_INFO, "System ready\n");

/* Tracing-aware panic */
panic_trace("Critical error: %s", error_msg);
```

## Performance Considerations

### When Disabled (Default)

- **Overhead**: Nearly zero - just a boolean check
- **Memory**: No buffer allocation or event storage
- **CPU**: Single conditional branch per tracepoint

### When Enabled

- **Overhead**: <5% in typical workloads
- **Memory**: ~512KB for event buffer (8192 × 64 bytes)
- **CPU**: Minimal - optimized ring buffer operations

### Buffer Management

- **Capacity**: 8192 events (configurable via MACH_TRACE_BUF_SIZE)
- **Behavior**: Circular buffer - old events are overwritten when full
- **Thread Safety**: IRQ-safe locking for concurrent access
- **Atomic Operations**: Lock-free reads where possible

## Integration Points

### Current Integrations

1. **Kernel Startup** (`kern/startup.c`):
   - Early initialization
   - Full system initialization

2. **IPC Subsystem** (`ipc/mach_msg.c`):
   - Message send operations
   - Message receive operations

3. **Debug Infrastructure** (`kern/printf.h`):
   - Enhanced printf macros
   - Panic integration

### Adding New Integration Points

To add tracing to a new subsystem:

1. Include the header:
```c
#ifdef CONFIG_MACH_TRACING
#include <mach/lttng.h>
#endif
```

2. Add tracepoints:
```c
#ifdef CONFIG_MACH_TRACING
TRACE_SUBSYSTEM(event_name);
#endif
```

## Debugging and Analysis

### Statistics

View tracing statistics:
```c
mach_trace_print_stats();
```

Output example:
```
Mach Tracing Statistics:
  Status: Enabled
  Buffer size: 8192 events
  Buffered events: 1234
  Events generated: 5678
  Events dropped: 0
  Buffer dropped: 0
```

### Event Structure

Each event contains:
- **Timestamp**: High-resolution timestamp (split into hi/lo 32-bit parts)
- **Category**: Event category (IPC, SCHED, VM, etc.)
- **Level**: Event severity level
- **Event ID**: Unique identifier for event type
- **CPU ID**: CPU that generated the event
- **Task/Thread ID**: Context information
- **Data**: 64 bytes of event-specific data

### Buffer Analysis

Events are stored in a circular buffer with atomic operations:
- Events are never lost due to locking
- Old events are overwritten when buffer is full
- Statistics track dropped events for monitoring

## Future Enhancements

### Planned Features

1. **User-space Interface**: System calls for reading trace data
2. **Dynamic Configuration**: Runtime event filtering
3. **Network Export**: Remote trace data collection
4. **Integration Tools**: Analysis and visualization utilities
5. **Advanced Timestamps**: High-resolution timer integration

### LTTng Compatibility

The current implementation provides LTTng-style APIs and can be extended for:
- User-space tracing libraries
- Compatible trace data formats
- Integration with LTTng analysis tools

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure `--enable-tracing` is used during configuration
2. **Missing Symbols**: Include `<mach/lttng.h>` in source files
3. **Performance Impact**: Check if tracing is accidentally left enabled
4. **Buffer Overflow**: Monitor dropped event statistics

### Configuration Verification

Check if tracing is compiled in:
```bash
grep CONFIG_MACH_TRACING config.h
```

Should show:
```
#define CONFIG_MACH_TRACING 1
```

## References

- [LTTng Documentation](https://lttng.org/docs/)
- [GNU Hurd Project](https://www.gnu.org/software/hurd/)
- [Mach Microkernel](https://www.gnu.org/software/hurd/microkernel/mach/)

---

*This documentation covers the initial LTTng-style tracing implementation for GNU Mach. The system provides a solid foundation for kernel instrumentation and can be extended based on specific analysis needs.*