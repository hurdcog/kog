# GNU Mach Startup Debugging Guide

This guide provides comprehensive instructions for debugging GNU Mach kernel startup and common debugging scenarios using QEMU and GDB.

## Quick Start for Startup Debugging

The most common debugging scenario is stepping through the kernel boot process:

```bash
# 1. Build GNU Mach with debug symbols
./configure --enable-kdb --host=i686-gnu CC='gcc -m32' LD='ld -melf_i386'
make

# 2. Start QEMU with GDB (will pause at startup)
./scripts/setup-qemu-gdb.sh -k gnumach

# 3. In another terminal, connect GDB and set startup breakpoints
gdb -x debug.gdb
(gdb) break setup_main
(gdb) break c_boot_entry
(gdb) continue
```

## Table of Contents

1. [Understanding GNU Mach Boot Process](#understanding-gnu-mach-boot-process)
2. [Essential Startup Debugging Breakpoints](#essential-startup-debugging-breakpoints)
3. [Debugging Early Boot Issues](#debugging-early-boot-issues)
4. [Memory Management During Startup](#memory-management-during-startup)
5. [IPC Initialization Debugging](#ipc-initialization-debugging)
6. [Device Driver Loading](#device-driver-loading)
7. [Console and Output Debugging](#console-and-output-debugging)
8. [Common Startup Problems](#common-startup-problems)
9. [Advanced GDB Techniques for Kernel Debugging](#advanced-gdb-techniques)

## Understanding GNU Mach Boot Process

GNU Mach follows this startup sequence:

```
Multiboot Loader
    ↓
Assembly Entry Point (i386/i386/locore.S)
    ↓
C Boot Entry (kern/startup.c:c_boot_entry)
    ↓  
Setup Main (kern/startup.c:setup_main)
    ↓
Kernel Subsystem Initialization
    ↓
First User Task Bootstrap
```

### Key Startup Functions

1. **`c_boot_entry`**: First C function called after assembly setup
2. **`setup_main`**: Main kernel initialization function  
3. **`vm_mem_bootstrap`**: Early memory management setup
4. **`ipc_bootstrap`**: IPC system initialization
5. **`machine_init`**: Architecture-specific initialization
6. **`user_bootstrap`**: First user task creation

## Essential Startup Debugging Breakpoints

Set these breakpoints to debug common startup scenarios:

```gdb
# Core startup sequence
break c_boot_entry
break setup_main
break machine_init

# Memory management initialization
break vm_mem_bootstrap
break vm_mem_init
break pmap_bootstrap

# IPC system startup
break ipc_bootstrap
break ipc_init
break mach_port_init

# Device and driver initialization  
break machine_init
break device_service_create
break console_init

# Bootstrap task creation
break user_bootstrap
break task_create
```

## Debugging Early Boot Issues

### Problem: Kernel Hangs During Boot

```gdb
# Set breakpoint at entry and step through
(gdb) break c_boot_entry
(gdb) continue
(gdb) next
(gdb) step

# Check CPU state
(gdb) info registers
(gdb) x/20i $pc

# Examine stack
(gdb) backtrace
(gdb) frame 0
(gdb) info locals
```

### Problem: Memory Initialization Failures

```gdb
# Debug memory setup
(gdb) break vm_mem_bootstrap
(gdb) continue

# Examine memory layout
(gdb) print/x boot_info
(gdb) print/x avail_start
(gdb) print/x avail_end

# Check memory descriptors
(gdb) x/10x phys_map_addr
```

### Problem: Early Panic or Exception

```gdb
# Break on exception handlers
(gdb) break panic  
(gdb) break Debugger
(gdb) break trap

# When hit, examine the fault
(gdb) backtrace
(gdb) info registers
(gdb) print/x fault_address
```

## Memory Management During Startup

### Debugging VM Bootstrap

```gdb
# Set breakpoints on VM initialization
(gdb) break vm_mem_bootstrap
(gdb) break pmap_bootstrap
(gdb) break vm_page_bootstrap

# Examine physical memory layout
(gdb) continue
(gdb) print/x first_phys_addr
(gdb) print/x last_phys_addr
(gdb) print/x virtual_avail
(gdb) print/x virtual_end
```

### Checking Memory Allocators

```gdb
# Debug kernel memory allocation
(gdb) break kmem_init
(gdb) break kalloc_init

# Watch for allocation failures
(gdb) watch kalloc_map
(gdb) condition $bpnum kalloc_map == 0
```

## IPC Initialization Debugging

### Debugging Port Creation and Messaging

```gdb
# Break on IPC bootstrap
(gdb) break ipc_bootstrap
(gdb) break ipc_init
(gdb) break mach_port_init

# Debug port operations
(gdb) break mach_port_allocate_name
(gdb) break ipc_kmsg_get
(gdb) break ipc_kmsg_send

# Check IPC space initialization
(gdb) print/x default_pset->tasks
(gdb) print/x kernel_task
```

## Device Driver Loading

### Console Driver Debugging

```gdb
# Debug console initialization
(gdb) break console_init
(gdb) break cninit

# Debug output functions
(gdb) break printf  
(gdb) break console_print_timestamp

# Check console device status
(gdb) print/x console_is_initialized
(gdb) print/x cn_tab
```

### General Device Debugging

```gdb
# Device subsystem initialization
(gdb) break device_service_create
(gdb) break device_init

# Driver loading
(gdb) break linux_init
(gdb) break configure_device_drivers
```

## Console and Output Debugging

GNU Mach includes enhanced console timestamp functionality:

### Debugging Console Timestamps

```gdb
# Break on timestamp functions
(gdb) break console_timestamp_init
(gdb) break console_print_timestamp

# Check timestamp configuration
(gdb) print console_timestamp_enabled
(gdb) print console_timestamp_format
(gdb) print console_timestamp_boot_time

# Manually enable timestamps
(gdb) call console_timestamp_enable(1)
(gdb) call console_timestamp_set_format(TIMESTAMP_FORMAT_PRECISE)
```

### Debugging Printf and Kernel Messages

```gdb
# Break on all printf calls
(gdb) break printf
(gdb) commands
    printf "Printf called with: %s\n", $rdi
    continue
end

# Or break conditionally
(gdb) break printf if strstr($rdi, "panic") != 0
```

## Common Startup Problems

### 1. Multiboot Header Issues

```gdb
# Check multiboot information
(gdb) break c_boot_entry
(gdb) continue
(gdb) print/x boot_info
(gdb) print boot_info.flags
(gdb) x/10x boot_info.mods_addr
```

### 2. Stack Overflow During Initialization

```gdb
# Check stack usage
(gdb) break setup_main
(gdb) continue
(gdb) print $sp
(gdb) print/x kernel_stack
(gdb) print ($sp - kernel_stack)  # Stack usage
```

### 3. Interrupt Setup Problems

```gdb
# Debug interrupt initialization
(gdb) break idt_init
(gdb) break interrupt_processor_bind

# Check interrupt state
(gdb) print/x master_icu
(gdb) print/x slaves_icu
```

## Advanced GDB Techniques for Kernel Debugging

### Custom GDB Commands for Kernel Debugging

Add these to your `.gdbinit` or debug script:

```gdb
# Show kernel task list
define show-tasks
    set $task = (task_t)queue_first(&all_tasks)
    while ($task != (task_t)&all_tasks)
        printf "Task %p: pid=%d\n", $task, $task->pid
        set $task = (task_t)queue_next(&$task->tasks)
    end
end

# Show thread information  
define show-threads
    set $thread = current_thread()
    printf "Current thread: %p\n", $thread
    printf "  State: %d\n", $thread->state
    printf "  Priority: %d\n", $thread->sched_pri
end

# Show memory regions
define show-memory
    printf "Virtual memory layout:\n"
    printf "  virtual_start: 0x%x\n", virtual_start
    printf "  virtual_end: 0x%x\n", virtual_end
    printf "  avail_start: 0x%x\n", avail_start
    printf "  avail_end: 0x%x\n", avail_end
end
```

### Hardware Breakpoints for Kernel Code

```gdb
# Use hardware breakpoints for kernel entry points
(gdb) hbreak setup_main
(gdb) hbreak trap

# Watch critical data structures
(gdb) watch kernel_map
(gdb) watch current_thread_var
```

### Examining Kernel Data Structures

```gdb
# Print complex kernel structures
(gdb) set print pretty on
(gdb) print *current_task()
(gdb) print *kernel_map

# Follow linked lists
(gdb) set $node = all_tasks.next
(gdb) while ($node != &all_tasks)
    print *((task_t)$node)
    set $node = $node->next
end
```

## Automated Debugging Scripts

The repository includes several helper scripts:

### Using the Debug Helper

```bash
# Automated debugging setup
./scripts/debug-helper.sh

# This will:
# - Find or create a suitable kernel
# - Test QEMU setup
# - Provide debugging instructions
```

### Custom Debugging Scenarios

```bash
# Debug specific subsystems
./scripts/setup-qemu-gdb.sh -k gnumach -a "-d cpu,guest_errors"

# Debug with different memory sizes
./scripts/setup-qemu-gdb.sh -m 256M

# Debug without waiting (for automated testing)
./scripts/setup-qemu-gdb.sh -n
```

## Integration with Test Suite

The debugging infrastructure integrates with the test suite:

```bash
# Run debugging tests
make test-gdb-stub

# Debug specific tests interactively
make debug-hello
make debug-vm
make debug-console-timestamps
```

## Troubleshooting Debugging Setup

### GDB Connection Issues

```bash
# Check if QEMU is listening
netstat -ln | grep 1234

# Test with different port
./scripts/setup-qemu-gdb.sh -p 1235
```

### Kernel Architecture Issues

```bash
# Check kernel architecture  
file gnumach

# For 32-bit kernels, ensure you use:
./scripts/setup-qemu-gdb.sh  # Auto-detects architecture
```

### Missing Debug Symbols

```bash
# Rebuild with debug symbols
make clean
./configure --enable-kdb CFLAGS="-g -O1 -fno-omit-frame-pointer"
make

# Verify symbols are present
readelf -S gnumach | grep debug
```

---

This guide provides the foundation for debugging GNU Mach startup and runtime behavior. For more advanced scenarios, refer to the main debugging guide and the GNU Hurd documentation.