# GDB Stub Enhancement Documentation

## Overview

The GDB stub has been enhanced to support modern GDB debugging features including:

1. **Hardware Breakpoints**: Real x86 debug register support (DR0-DR7)
2. **GDB Remote Protocol**: Modern packet handling with checksums
3. **Serial/Network Communication**: Framework for remote debugging
4. **Modern GDB Features**: Support for newer protocol extensions

## Hardware Breakpoint Support

### Features
- Up to 4 hardware execution breakpoints using DR0-DR3
- Write, read, and access watchpoints
- Proper DR7 configuration for breakpoint types and lengths
- Hardware breakpoint management with slot tracking

### Usage
```c
// Set a hardware execution breakpoint
gdb_stub_set_breakpoint(GDB_BP_HARDWARE, 0x12345678, 1);

// Set a write watchpoint
gdb_stub_set_breakpoint(GDB_BP_WRITE_WATCH, 0x87654321, 4);

// Remove breakpoints
gdb_stub_remove_breakpoint(GDB_BP_HARDWARE, 0x12345678, 1);
```

### Debug Register Layout
- **DR0-DR3**: Breakpoint linear addresses
- **DR6**: Debug status register (which breakpoint hit)
- **DR7**: Debug control register (enable/configure breakpoints)

## GDB Remote Protocol

### Supported Commands
- `qSupported` - Capability negotiation
- `Z<type>,<addr>,<length>` - Set breakpoint
- `z<type>,<addr>,<length>` - Remove breakpoint
- `c` - Continue execution
- `s` - Single step
- `?` - Return last signal

### Packet Format
All packets follow GDB remote protocol format:
```
$<data>#<checksum>
```

Example:
```
$qSupported:PacketSize=1000;hwbreak+;swbreak+#checksum
```

## Serial Communication Setup

### For Remote Debugging

1. **Configure Serial Port**: The kernel should be configured with a serial console
2. **GDB Connection**: Connect GDB to the serial port:
   ```bash
   gdb gnumach
   (gdb) target remote /dev/ttyS0
   (gdb) set remote hardware-breakpoint-limit 4
   ```

3. **Network Debugging** (via serial-to-network bridge):
   ```bash
   # On host machine with serial connection
   socat TCP-LISTEN:1234 /dev/ttyS0,raw,echo=0
   
   # On development machine
   gdb gnumach
   (gdb) target remote hostname:1234
   ```

### Current Implementation
The current implementation outputs GDB packets to the console for debugging purposes. To enable actual remote debugging:

1. Replace `gdb_stub_putchar()` with actual serial UART output
2. Replace `gdb_stub_getchar()` with actual serial UART input  
3. Implement `gdb_stub_char_available()` to check UART FIFO

## Integration Points

### Kernel Initialization
The GDB stub is automatically initialized during kernel startup in `kern/startup.c`:
```c
#if MACH_KDB
    gdb_stub_init();
#endif
```

### Exception Handling
Hardware breakpoints and debug exceptions should call:
```c
gdb_stub_handle_exception(exception_type, saved_state);
```

## Testing

A comprehensive test suite is available:
```bash
make run-gdb-stub
```

The test covers:
- GDB stub initialization
- Hardware breakpoint management
- Protocol packet handling
- Debug register access

## Modern GDB Features

### Capabilities Reported
- `PacketSize=1000` - Support for large packets
- `hwbreak+` - Hardware breakpoint support
- `swbreak+` - Software breakpoint support

### Thread Support
Framework exists for thread-aware debugging:
```c
gdb_stub_thread_create(thread);
gdb_stub_thread_destroy(thread);
gdb_stub_thread_switch(old_thread, new_thread);
```

## Configuration

Default configuration provides:
- 4 hardware breakpoints
- 4 hardware watchpoints
- Thread-aware debugging enabled
- All breakpoint types supported

Configuration can be modified:
```c
struct gdb_stub_config config = {
    .enabled = TRUE,
    .hardware_breakpoints = TRUE,
    .software_breakpoints = TRUE,
    .watchpoints = TRUE,
    .max_breakpoints = 4,
    .max_watchpoints = 4
};
gdb_stub_configure(&config);
```

## Future Enhancements

1. **Serial Driver Integration**: Connect with actual UART drivers
2. **Network Support**: TCP/IP remote debugging
3. **Register Dump**: Full CPU state serialization for GDB
4. **Memory Access**: Safe kernel memory read/write for GDB
5. **Multi-processor**: Support for debugging SMP kernels
6. **Kernel Modules**: Support for debugging loadable modules