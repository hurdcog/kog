# GNU Mach Debugging Guide

This guide provides comprehensive instructions for debugging GNU Mach using QEMU and GDB.

## Quick Start

The fastest way to start debugging is using our automated debugging tools:

```bash
# Complete setup and testing
./scripts/debug-master.sh setup

# Quick debugging session (auto-detects kernel and starts GDB)
./scripts/debug-master.sh quick

# Debug specific scenarios
./scripts/debug-master.sh scenario startup     # Debug kernel boot
./scripts/debug-master.sh scenario memory      # Debug memory management
./scripts/debug-master.sh scenario interactive # Custom debugging

# Or use individual scripts directly:
./scripts/setup-qemu-gdb.sh                    # Start QEMU+GDB
./scripts/debug-helper.sh                      # Setup helper
./scripts/debug-scenarios.sh startup           # Specific scenarios
```

**For GNU Mach startup debugging specifically, see [docs/startup-debugging-guide.md](startup-debugging-guide.md)**

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Building for Debugging](#building-for-debugging)
3. [Automated Debugging Scripts](#automated-debugging-scripts)
4. [QEMU Setup](#qemu-setup)
5. [GDB Connection](#gdb-connection)
6. [Common Debugging Scenarios](#common-debugging-scenarios)
7. [Console Timestamp Features](#console-timestamp-features)
8. [Debugging Tips and Tricks](#debugging-tips-and-tricks)
9. [Troubleshooting](#troubleshooting)

## Prerequisites

### Required Software

- **QEMU**: System emulation for i386
  ```bash
  # Ubuntu/Debian
  sudo apt-get install qemu-system-x86

  # Fedora/RHEL
  sudo dnf install qemu-system-x86

  # macOS (with Homebrew)
  brew install qemu
  ```

- **GDB**: GNU Debugger
  ```bash
  # Ubuntu/Debian
  sudo apt-get install gdb

  # Fedora/RHEL
  sudo dnf install gdb

  # macOS (with Homebrew)
  brew install gdb
  ```

- **Build Tools**: For building GNU Mach
  ```bash
  # Ubuntu/Debian
  sudo apt-get install build-essential autoconf automake texinfo

  # Fedora/RHEL
  sudo dnf install gcc make autoconf automake texinfo
  ```

### Optional but Recommended

- **GNU MIG**: Mach Interface Generator (for complete builds)
- **Multiboot-compliant bootloader** (GRUB2 for real hardware testing)

## Automated Debugging Scripts

GNU Mach includes comprehensive debugging automation:

### Debug Master Script

The main entry point for all debugging activities:

```bash
# Complete setup and testing
./scripts/debug-master.sh setup

# Quick debugging session
./scripts/debug-master.sh quick

# Specific debugging scenarios
./scripts/debug-master.sh scenario startup
./scripts/debug-master.sh scenario memory
./scripts/debug-master.sh scenario ipc
./scripts/debug-master.sh scenario console
./scripts/debug-master.sh scenario panic
./scripts/debug-master.sh scenario interactive

# Test all debugging infrastructure
./scripts/debug-master.sh test

# Get detailed help
./scripts/debug-master.sh help
```

### Individual Scripts

For specific debugging tasks:

```bash
# QEMU+GDB setup with auto-detection
./scripts/setup-qemu-gdb.sh                    # Auto-detect kernel architecture
./scripts/setup-qemu-gdb.sh -k /path/to/kernel # Specific kernel
./scripts/setup-qemu-gdb.sh -p 1235           # Custom GDB port
./scripts/setup-qemu-gdb.sh -m 256M           # Custom memory size

# Debugging environment setup
./scripts/debug-helper.sh                      # Create test kernel if needed

# Specific debugging scenarios
./scripts/debug-scenarios.sh startup           # Kernel startup debugging
./scripts/debug-scenarios.sh memory            # Memory management
./scripts/debug-scenarios.sh interactive       # Custom session
./scripts/debug-scenarios.sh test              # Test all scenarios
```

### Integration with Test Suite

```bash
# Run debugging-related tests
make test-gdb-stub                              # Test GDB stub functionality

# Interactive debugging of tests
make debug-hello                                # Debug hello test
make debug-vm                                   # Debug VM test  
make debug-console-timestamps                   # Debug console timestamps
```

## Building for Debugging

```bash
# Generate configure script if needed
autoreconf -fiv

# Configure with debugging enabled
./configure --enable-kdb --host=i686-gnu

# Alternative: Configure with additional debug flags
./configure --enable-kdb --host=i686-gnu CFLAGS="-g -O1 -fno-omit-frame-pointer"
```

**Configuration Options:**
- `--enable-kdb`: Enables in-kernel debugger (DDB)
- `CFLAGS="-g"`: Include debugging symbols
- `CFLAGS="-O1"`: Lower optimization for better debugging
- `CFLAGS="-fno-omit-frame-pointer"`: Preserve frame pointers for stack traces

### Build

```bash
# Build the kernel
make -j$(nproc)

# Verify the build
ls -la gnumach
file gnumach  # Should show "not stripped" for debug symbols
```

## QEMU Setup

### Using the Automated Script

```bash
# Basic debugging setup
./scripts/setup-qemu-gdb.sh

# Custom options
./scripts/setup-qemu-gdb.sh -k ./gnumach -p 1234 -m 256M

# Testing mode (no GDB)
./scripts/setup-qemu-gdb.sh -n
```

### Manual QEMU Setup

```bash
# Start QEMU with GDB server
qemu-system-i386 -m 128M -kernel gnumach -s -S

# Alternative with specific port
qemu-system-i386 -m 128M -kernel gnumach -gdb tcp::1234 -S

# With additional debug options
qemu-system-i386 -m 128M -kernel gnumach -s -S -d cpu_reset,guest_errors
```

**QEMU Debug Options:**
- `-s`: Enable GDB server on port 1234
- `-S`: Pause CPU at startup
- `-d cpu_reset,guest_errors`: Enable debug logging
- `-monitor stdio`: Interactive QEMU monitor

## GDB Connection

### Quick Connection

```bash
# Use the generated script
gdb -x debug.gdb

# Or manually
gdb gnumach
(gdb) target remote localhost:1234
(gdb) continue
```

### Setting Up GDB Session

```gdb
# Load kernel symbols
file gnumach

# Connect to QEMU
target remote localhost:1234

# Useful GDB settings for kernel debugging
set disassembly-flavor intel
set print pretty on
set pagination off

# Continue execution
continue
```

## Common Debugging Scenarios

### 1. Boot Process Debugging

```gdb
# Break at kernel entry point
break startup.c:c_boot_entry

# Or break at main
break main

# Continue and examine startup
continue
info registers
backtrace
```

### 2. Console Output Debugging

```gdb
# Break on printf calls
break printf

# Break on console timestamp functions
break console_print_timestamp
break console_timestamp_init

# Continue and examine output
continue
step
info locals
```

### 3. Memory Management Debugging

```gdb
# Break on memory allocation
break kalloc
break kfree

# Break on VM operations
break vm_allocate
break vm_deallocate

# Examine memory state
info mem
x/10x $sp
```

### 4. Process/Thread Debugging

```gdb
# Break on thread operations
break thread_create
break thread_start

# Break on task operations
break task_create

# Examine thread state
info threads
thread apply all backtrace
```

### 5. Debugging Panics and Crashes

```gdb
# Break on panic
break panic
break Debugger

# When hit, examine state
backtrace
info registers
x/20x $sp
```

## Console Timestamp Features

GNU Mach includes enhanced console timestamp functionality for debugging:

### Timestamp Formats

```c
// Available formats
TIMESTAMP_FORMAT_RELATIVE    // [123.456] - seconds since boot
TIMESTAMP_FORMAT_UPTIME      // [123.456] - absolute uptime
TIMESTAMP_FORMAT_SIMPLE      // [123.456] - simple format
TIMESTAMP_FORMAT_PRECISE     // [123.456.789] - with microseconds
```

### Runtime Configuration

```gdb
# Enable/disable timestamps
call console_timestamp_enable(1)  # Enable
call console_timestamp_enable(0)  # Disable

# Change format
call console_timestamp_set_format(TIMESTAMP_FORMAT_PRECISE)

# Check current status
print console_timestamp_is_enabled()
print console_timestamp_get_format()
```

### Debug Boot Timing

```gdb
# Break after timestamp initialization
break console_timestamp_init

# Get boot time
call console_timestamp_get_boot_time(&boot_time)
print boot_time
```

## Debugging Tips and Tricks

### 1. Useful GDB Commands

```gdb
# Show kernel structures
ptype task_t
ptype thread_t

# Examine linked lists
set $node = queue_first(&all_tasks)
while ($node != &all_tasks)
    print *((task_t)$node)
    set $node = queue_next($node)
end

# Hardware watchpoints
watch *0x100000  # Watch memory location

# Conditional breakpoints
break printf if $rdi == 0  # Break if first arg is NULL
```

### 2. Kernel State Examination

```gdb
# Current CPU state
info registers
print current_thread()
print current_task()

# Memory mapping
print kernel_map
print kernel_pmap

# Scheduler state
print default_pset
print master_processor
```

### 3. Source Code Navigation

```gdb
# List source
list main
list printf.c:console_print_timestamp

# Search functions
info functions console_
info variables timestamp_

# Disassemble
disassemble main
disassemble /m main  # With source
```

### 4. QEMU Monitor Commands

In QEMU monitor (Ctrl+Alt+2 or `monitor` command):

```
# Show CPU state
info registers

# Memory examination
x/20x 0x100000

# Virtual memory translation
info tlb

# Memory mapping
info mem
```

## Troubleshooting

### Common Issues

#### 1. GDB Cannot Connect

```bash
# Check if QEMU is listening
netstat -ln | grep 1234

# Try different port
./scripts/setup-qemu-gdb.sh -p 1235
```

#### 2. No Debug Symbols

```bash
# Verify symbols in binary
file gnumach
readelf -S gnumach | grep debug

# Rebuild with debug symbols
make clean
./configure --enable-kdb CFLAGS="-g -O1"
make
```

#### 3. QEMU Crashes or Hangs

```bash
# Use QEMU monitor
qemu-system-i386 -monitor stdio ...

# Enable QEMU logging
qemu-system-i386 -d cpu,guest_errors ...
```

#### 4. Kernel Panic on Boot

```gdb
# Set breakpoint before suspected area
break startup.c:main

# Single step through boot process
next
step
```

### Performance Tips

1. **Use hardware acceleration when available:**
   ```bash
   # Linux with KVM
   ./scripts/setup-qemu-gdb.sh -a "-enable-kvm"

   # macOS with HVF
   ./scripts/setup-qemu-gdb.sh -a "-accel hvf"
   ```

2. **Optimize debug build:**
   ```bash
   ./configure --enable-kdb CFLAGS="-g -O1 -pipe"
   ```

3. **Use sparse breakpoints:**
   - Set breakpoints only where needed
   - Remove breakpoints when done
   - Use conditional breakpoints to reduce hits

### Getting Help

- **IRC**: #hurd on libera.chat
- **Mailing Lists**: bug-hurd@gnu.org, help-hurd@gnu.org
- **Documentation**: https://www.gnu.org/software/hurd/
- **Bug Reports**: https://savannah.gnu.org/bugs/?group=hurd

---

*This guide is part of the GNU Mach development tools improvements. For more information, see the project documentation.*