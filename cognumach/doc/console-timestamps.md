# Console Timestamp Feature

## Overview
The console timestamp feature adds time-based prefixes to kernel printf output, improving debugging and log analysis capabilities.

## Implementation
- **Location**: `kern/printf.c`, `kern/printf.h`
- **Initialization**: Automatically initialized during kernel startup
- **Format**: `[seconds.milliseconds] message`

## Usage

### Automatic Timestamping
Timestamps are automatically added to new lines when using `printf()`:

```c
printf("Kernel starting\n");     // Output: [0.000] Kernel starting
printf("Memory initialized\n");  // Output: [0.045] Memory initialized
```

### Configuration
Control timestamp behavior at runtime:

```c
// Disable timestamps
console_timestamp_enable(FALSE);

// Re-enable timestamps  
console_timestamp_enable(TRUE);

// Check current state
if (console_timestamp_is_enabled()) {
    // Timestamps are active
}
```

### Boot Parameters
Control timestamps via kernel command line:

```bash
# Disable timestamps completely
kernel-args: notimestamps

# Use precise microsecond timestamps  
kernel-args: timestamp_format=precise

# Use simple padded format
kernel-args: timestamp_format=simple
```

### Global Variables
- `console_timestamps_enabled`: Boolean flag to enable/disable timestamps

## Technical Details

### Time Source
Uses the kernel's `uptime` global variable for high-resolution timing relative to console initialization.

### Performance
- Minimal overhead: only active when timestamps are enabled
- No time calculations when timestamps are disabled
- Efficient relative time computation

### Thread Safety
The current implementation is not thread-safe. For SMP systems, appropriate locking should be added.

## Testing
Test file: `tests/test-console-timestamp.c`

Run test to verify timestamp functionality:
```bash
# Build and test (when full build system is available)
make tests/test-console-timestamp
```

## Configuration Options
- Default: **Enabled** 
- Can be disabled by setting `console_timestamps_enabled = FALSE`
- **Boot parameters** (kernel command line):
  - `notimestamps` - Disable timestamps completely
  - `console_timestamps=off` - Disable timestamps
  - `console_timestamps=on` - Enable timestamps (explicit)
  - `timestamp_format=simple` - Use simple format [sss.mmm]
  - `timestamp_format=precise` - Use precise format [sss.mmm.uuu] 
  - `timestamp_format=uptime` - Use absolute uptime format
  - Default format is `relative` [seconds.milliseconds from console init]

## Future Enhancements
1. **Real-time timestamps**: Add wall-clock time option
2. **Format selection**: ~~Multiple timestamp formats~~ ✓ **IMPLEMENTED**
3. **Per-subsystem control**: Enable timestamps for specific kernel subsystems
4. ~~**Thread safety**: Add proper locking for SMP systems~~ ✓ **IMPLEMENTED**
5. ~~**Performance optimization**: Zero-overhead when disabled~~ ✓ **IMPLEMENTED**  
6. ~~**Boot parameter**: Control via kernel command line parameter~~ ✓ **IMPLEMENTED**

## Related Roadmap Items
- Part of **Phase 1: Foundation & Quick Wins**
- Supports debugging infrastructure improvements
- Foundation for advanced kernel instrumentation (Phase 4)