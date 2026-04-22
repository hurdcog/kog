# Performance Analysis Framework

## Overview

The GNU Mach Performance Analysis Framework provides comprehensive system-wide profiling, real-time performance monitoring, and regression detection capabilities. This framework enables developers and system administrators to understand and optimize kernel performance.

## Features

### 1. System-Wide Profiling
- **Event Tracking**: Monitor key kernel events (IPC, VM, task/thread operations, context switches, interrupts, syscalls, page faults)
- **High-Resolution Timing**: Microsecond-precision timing for all operations
- **Low Overhead**: Designed for minimal performance impact (<5% overhead)
- **Configurable Sampling**: Adjustable sample rates and buffer sizes

### 2. Real-Time Performance Monitoring
- **Live Statistics**: Real-time collection of performance metrics
- **Threshold Alerts**: Configurable thresholds for latency, throughput, and error rates
- **Resource Tracking**: Monitor CPU, memory, and I/O resource usage
- **Event Correlation**: Cross-reference events for comprehensive analysis

### 3. Performance Regression Detection
- **Baseline Comparison**: Set performance baselines and detect deviations
- **Automated Alerts**: Automatic detection of performance regressions
- **Trend Analysis**: Historical performance tracking
- **Configurable Sensitivity**: Adjustable regression detection thresholds

## Architecture

### Core Components

1. **Performance Monitor (`kern/perf_analysis.c/h`)**
   - Central monitoring engine
   - Event collection and timing
   - Statistics aggregation
   - Sample buffer management

2. **Mach Interface (`include/mach/perf_monitor.defs/h`)**
   - User-space API for performance monitoring
   - RPC interfaces for configuration and data retrieval
   - Host-level operations

3. **Kernel Integration**
   - IPC instrumentation (`ipc/mach_msg.c`)
   - VM instrumentation (`vm/vm_user.c`)
   - Startup initialization (`kern/startup.c`)

4. **User-Space Tools**
   - Test suite (`tests/test-performance-analysis.c`)
   - Command-line utility (`scripts/perf-analysis.sh`)

### Event Types

| Event ID | Event Type | Description |
|----------|------------|-------------|
| 0 | PERF_EVENT_IPC_SEND | IPC message send operations |
| 1 | PERF_EVENT_IPC_RECEIVE | IPC message receive operations |
| 2 | PERF_EVENT_VM_ALLOC | Virtual memory allocation |
| 3 | PERF_EVENT_VM_FREE | Virtual memory deallocation |
| 4 | PERF_EVENT_TASK_CREATE | Task creation |
| 5 | PERF_EVENT_TASK_TERMINATE | Task termination |
| 6 | PERF_EVENT_THREAD_CREATE | Thread creation |
| 7 | PERF_EVENT_THREAD_TERMINATE | Thread termination |
| 8 | PERF_EVENT_CONTEXT_SWITCH | Context switching |
| 9 | PERF_EVENT_INTERRUPT | Interrupt handling |
| 10 | PERF_EVENT_SYSCALL | System call execution |
| 11 | PERF_EVENT_PAGE_FAULT | Page fault handling |

## Usage

### Programming Interface

#### Kernel Integration

The framework provides easy-to-use macros for kernel developers:

```c
#include <kern/perf_analysis.h>

// Record a simple event
PERF_EVENT_RECORD(PERF_EVENT_IPC_SEND, task_id, thread_id, data1, data2);

// Time a code block
PERF_TIME_BLOCK(PERF_EVENT_VM_ALLOC, task_id, thread_id, size, 0, {
    // Code to be timed
    result = vm_map_enter(...);
});

// Manual timing
uint64_t start = PERF_EVENT_TIME_START(PERF_EVENT_SYSCALL);
// ... syscall implementation ...
PERF_EVENT_TIME_END(PERF_EVENT_SYSCALL, start, task_id, thread_id, result, 0);
```

#### User-Space API

```c
#include <mach/perf_monitor.h>

// Enable monitoring
kern_return_t kr = perf_monitor_enable(mach_host_self(), TRUE);

// Configure parameters
kr = perf_monitor_configure(mach_host_self(), 1000, 4096);

// Get statistics
uint64_t count, total_time, min_time, max_time, avg_time, last_timestamp;
kr = perf_get_event_stats(mach_host_self(), PERF_EVENT_VM_ALLOC,
                         &count, &total_time, &min_time, &max_time, 
                         &avg_time, &last_timestamp);

// Set baseline and check regressions
kr = perf_set_baseline(mach_host_self());
boolean_t regression;
kr = perf_check_regression(mach_host_self(), PERF_EVENT_VM_ALLOC, 20, &regression);
```

### Command-Line Interface

```bash
# Enable performance monitoring
./scripts/perf-analysis.sh enable

# Configure monitoring (2K samples/sec, 8K buffer)
./scripts/perf-analysis.sh configure -r 2000 -b 8192

# Show VM allocation statistics
./scripts/perf-analysis.sh stats 2

# Set performance baseline
./scripts/perf-analysis.sh baseline

# Check for regressions (>30% increase)
./scripts/perf-analysis.sh check-regression -e 2 -t 30

# Real-time monitoring (update every 5 seconds)
./scripts/perf-analysis.sh monitor -i 5

# Run test suite
./scripts/perf-analysis.sh test
```

### Testing

```bash
# Build and run the test suite
make tests/test-performance-analysis
make run-performance-analysis

# Run specific test categories
make run-benchmarks  # Performance benchmarks
```

## Configuration

### Default Settings

- **Sample Rate**: 1000 samples/second
- **Buffer Size**: 8192 samples
- **Latency Threshold**: 1000 microseconds
- **Throughput Threshold**: 1000 operations/second
- **Error Rate Threshold**: 5%
- **Regression Threshold**: 20%

### Tuning Guidelines

#### For Development
- **High Resolution**: Sample rate 2000-5000/sec
- **Large Buffer**: 16K-32K samples
- **Sensitive Thresholds**: 10-15% regression detection

#### For Production
- **Moderate Resolution**: Sample rate 100-1000/sec
- **Optimized Buffer**: 4K-8K samples
- **Conservative Thresholds**: 25-50% regression detection

#### For Debugging
- **Maximum Resolution**: Sample rate 5000+/sec
- **Large Buffer**: 32K+ samples
- **All Events**: Enable all event types

## Integration Points

### Existing Systems

The framework integrates with existing GNU Mach systems:

1. **Memory Tracking**: Leverages `kern/mem_track.c` for memory-related metrics
2. **Device Monitoring**: Integrates with `device/modern.c` performance tracking
3. **Console Timestamps**: Uses timestamp infrastructure for timing
4. **Test Framework**: Extends existing test infrastructure

### Future Extensions

1. **Hardware Performance Counters**: Integration with CPU performance monitoring units
2. **Trace Export**: Export to standard tracing formats (CTF, JSON)
3. **Visualization**: Web-based performance dashboards
4. **Machine Learning**: Automated anomaly detection

## Performance Impact

### Overhead Analysis

- **Monitoring Disabled**: 0% overhead
- **Basic Monitoring**: <1% overhead
- **Full Monitoring**: 2-5% overhead
- **High-Frequency Sampling**: 5-10% overhead

### Memory Usage

- **Framework Core**: ~16KB
- **Default Buffer**: ~256KB (8K samples × 32 bytes)
- **Large Configuration**: ~1MB (32K samples)

### Storage Requirements

- **Statistics**: ~1KB per event type
- **Historical Data**: Configurable retention
- **Export Formats**: Varies by format and duration

## Troubleshooting

### Common Issues

1. **Monitoring Not Available**
   - Ensure framework is compiled in
   - Check kernel initialization logs
   - Verify host permissions

2. **High Overhead**
   - Reduce sample rate
   - Decrease buffer size
   - Disable unnecessary event types

3. **Buffer Overruns**
   - Increase buffer size
   - Reduce sample rate
   - Read samples more frequently

4. **Regression False Positives**
   - Increase regression threshold
   - Ensure adequate baseline data
   - Account for system variability

### Debug Information

```bash
# Check framework status
dmesg | grep "Performance Analysis"

# Verify test functionality
./scripts/perf-analysis.sh test

# Monitor buffer status
./scripts/perf-analysis.sh stats
```

## Examples

### Basic Performance Monitoring

```c
// Enable monitoring for VM operations
perf_monitor_enable(mach_host_self(), TRUE);
perf_monitor_configure(mach_host_self(), 1000, 4096);

// Perform operations
for (int i = 0; i < 1000; i++) {
    vm_address_t addr;
    vm_allocate(mach_task_self(), &addr, 4096, TRUE);
    vm_deallocate(mach_task_self(), addr, 4096);
}

// Get statistics
struct perf_event_stats_user stats;
perf_get_event_stats(mach_host_self(), PERF_EVENT_VM_ALLOC, &stats);
printf("VM Allocations: %llu, Average time: %llu us\n", 
       stats.count, stats.avg_time_us);
```

### Regression Detection Workflow

```bash
#!/bin/bash
# Performance regression detection script

# Enable monitoring
./scripts/perf-analysis.sh enable

# Run baseline workload
echo "Running baseline workload..."
./my-benchmark-workload

# Set baseline
./scripts/perf-analysis.sh baseline

# Run new workload
echo "Running test workload..."
./my-test-workload

# Check for regressions
if ./scripts/perf-analysis.sh check-regression -e 2 -t 20; then
    echo "No significant regressions detected"
else
    echo "Performance regression detected!"
    exit 1
fi
```

## References

1. **GNU Mach Documentation**: Core kernel concepts
2. **Mach IPC**: Inter-process communication mechanisms
3. **Virtual Memory**: VM subsystem architecture
4. **Performance Optimization**: Previous optimization work
5. **Testing Framework**: Existing test infrastructure

## Contributing

When contributing to the performance analysis framework:

1. **Maintain Low Overhead**: All instrumentation should be lightweight
2. **Follow Patterns**: Use existing macros and patterns
3. **Add Tests**: Include test cases for new functionality
4. **Document Changes**: Update this documentation
5. **Validate Performance**: Measure overhead of new features

## License

Copyright (C) 2024 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2, or (at your option) any later version.