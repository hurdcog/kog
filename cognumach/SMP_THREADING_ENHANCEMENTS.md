# GNU Mach SMP Threading Enhancements

This document describes the SMP threading enhancements implemented to address issue #109.

## Overview

The GNU Mach kernel has been enhanced with comprehensive SMP (Symmetric Multiprocessing) threading support, including:

1. **Work Queues and Kernel Thread Pools**
2. **CPU Affinity Support**
3. **SMP-aware Thread Management**

## Key Components

### 1. Work Queue Infrastructure (`kern/smp.h`, `kern/smp.c`)

#### Data Structures
- `struct smp_work_item`: Represents a work item with function pointer and arguments
- `struct smp_work_queue`: Per-CPU work queue with lock and worker thread
- `cpu_mask_t`: Bitmask type for CPU affinity operations

#### Core Functions
- `smp_work_queue_init()`: Initialize per-CPU work queues
- `smp_queue_work(cpu, func, arg)`: Queue work on any available CPU
- `smp_queue_work_on(cpu, func, arg)`: Queue work on specific CPU
- `smp_work_thread()`: Main worker thread loop

#### CPU Affinity Utilities
- `cpu_affinity_test(mask, cpu)`: Test if CPU is in affinity mask
- `cpu_affinity_set(mask, cpu)`: Add CPU to affinity mask
- `cpu_affinity_clear(mask, cpu)`: Remove CPU from affinity mask

### 2. Thread Structure Enhancements (`kern/thread.h`, `kern/thread.c`)

#### Thread Structure Addition
```c
#if NCPUS > 1
    cpu_mask_t    cpu_affinity;    /* CPU affinity mask for SMP */
#endif
```

#### CPU Affinity Management Functions
- `thread_set_cpu_affinity(thread, mask)`: Set thread's CPU affinity
- `thread_get_cpu_affinity(thread)`: Get thread's CPU affinity
- `thread_can_run_on_cpu(thread, cpu)`: Check if thread can run on CPU

#### Integration Points
- Thread template initialization with default `CPU_AFFINITY_ANY`
- Work queue initialization during thread system startup
- SMP-aware thread creation and scheduling

### 3. Test Framework (`tests/test-smp-threads.c`)

#### Test Coverage
- **Basic Threading**: Verify thread info and basic functionality
- **SMP Information**: Test CPU count and SMP detection
- **CPU Affinity**: Validate affinity management functions
- **Work Queues**: Test work item queuing and execution

## Implementation Details

### Design Decisions

1. **Minimal Impact**: Changes are conditionally compiled with `#if NCPUS > 1`
2. **Backward Compatibility**: Default behavior unchanged for single-CPU systems
3. **Thread Safety**: All work queue operations use proper locking
4. **Resource Management**: Proper allocation and deallocation of work items

### Work Queue Architecture

```
CPU 0: [Work Queue] -> [Worker Thread] -> [Work Items...]
CPU 1: [Work Queue] -> [Worker Thread] -> [Work Items...]
CPU N: [Work Queue] -> [Worker Thread] -> [Work Items...]
```

Each CPU maintains its own work queue with a dedicated worker thread that processes work items sequentially.

### CPU Affinity System

- **Default Affinity**: `CPU_AFFINITY_ANY` allows threads to run on any CPU
- **Specific Affinity**: Bitmask specifies allowed CPUs (bit N = CPU N)
- **Validation**: Affinity masks are validated against available CPU count
- **Thread Safety**: Affinity changes are protected by thread locks

## Usage Examples

### Queuing Work
```c
// Queue work on any available CPU
smp_queue_work(-1, my_work_function, &my_data);

// Queue work on specific CPU
smp_queue_work_on(2, my_work_function, &my_data);
```

### Managing CPU Affinity
```c
// Set thread to run only on CPUs 0 and 2
cpu_mask_t mask = cpu_affinity_set(0, 0);
mask = cpu_affinity_set(mask, 2);
thread_set_cpu_affinity(my_thread, mask);

// Check if thread can run on CPU 1
boolean_t can_run = thread_can_run_on_cpu(my_thread, 1);
```

## Integration

The enhancements integrate seamlessly with the existing GNU Mach architecture:

1. **Initialization**: Work queues initialize during `thread_init()`
2. **Scheduling**: Affinity checks can be integrated into scheduler decisions
3. **IPC**: Work queues can be used for asynchronous IPC processing
4. **Device Drivers**: Work queues provide deferred processing capabilities

## Testing

Run the comprehensive SMP threading test:
```bash
make run-smp-threads
```

The test validates:
- Basic threading functionality
- SMP information retrieval
- CPU affinity operations
- Work queue functionality

## Benefits

1. **Scalability**: Efficient work distribution across multiple CPUs
2. **Performance**: Reduced contention through per-CPU work queues
3. **Flexibility**: CPU affinity allows fine-grained control over thread placement
4. **Maintainability**: Clean, modular implementation with clear interfaces

## Future Enhancements

1. **Load Balancing**: Automatic work migration between CPUs
2. **NUMA Awareness**: CPU affinity based on memory locality
3. **Priority Queues**: Different priority levels for work items
4. **Statistics**: Per-CPU work queue performance metrics

---

This implementation successfully addresses GNU Hurd issue #109 by providing a robust foundation for SMP threading in the GNU Mach microkernel.