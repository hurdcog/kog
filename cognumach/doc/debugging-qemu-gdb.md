# Debugging GNU Mach in QEMU with GDB

This guide shows how to run the QEMU-based tests under a debugger and step into kernel code.

## Prerequisites

- Built kernel `gnumach`
- QEMU installed (`qemu-system-i386` or `qemu-system-x86_64`)
- GDB

## Quick start

1. Build and generate a test ISO:
   - `make debug-hello`
   - This starts QEMU paused and waiting for GDB on `tcp::1234`.

2. In another terminal, attach GDB to the kernel:
   - `gdb gnumach`
   - `target remote :1234`
   - Optional breakpoints:
     - `b setup_main` (early kernel startup in `kern/startup.c`)
     - `b exception` (exception path in `kern/exception.c`)
   - `continue`

3. To load symbols for the test module (optional):
   - GDB prints the path in the QEMU console output; e.g. `tests/module-hello`
   - `add-symbol-file tests/module-hello`
   - `b _start`

## Notes

- The tests print a `TEST_START_MARKER` before boot to help log parsing.
- The default QEMU options are set for headless runs; see `tests/user-qemu.mk` for `QEMU_OPTS` and CPU type.
- For interactive (non-timeout) debugging, use `make debug-<test>` not `make run-<test>`.
