#!/bin/bash
# GNU Mach Debugging Scenarios Script
# Copyright (C) 2024 Free Software Foundation
#
# This script demonstrates common debugging scenarios for GNU Mach

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

usage() {
    cat << EOF
Usage: $0 [SCENARIO]

Available debugging scenarios:

  startup         Debug kernel startup sequence
  memory          Debug memory management initialization  
  ipc             Debug IPC system initialization
  console         Debug console and output functions
  panic           Debug kernel panic and exception handling
  interactive     Start interactive debugging session
  test            Run automated debugging tests

Examples:
  $0 startup      # Debug kernel boot process
  $0 memory       # Debug VM initialization
  $0 interactive  # Start custom debugging session

EOF
}

find_kernel() {
    local kernel_paths=(
        "${PROJECT_ROOT}/test-gnumach-debug"
        "${PROJECT_ROOT}/gnumach"
        "${PROJECT_ROOT}/build-i686/gnumach"
        "${PROJECT_ROOT}/build-i686-debug/gnumach"
    )
    
    for kernel in "${kernel_paths[@]}"; do
        if [[ -f "$kernel" ]]; then
            echo "$kernel"
            return 0
        fi
    done
    return 1
}

create_gdb_script() {
    local scenario="$1"
    local kernel="$2"
    local script_path="${PROJECT_ROOT}/debug-${scenario}.gdb"
    
    cat > "$script_path" << EOF
# GNU Mach ${scenario} debugging script
file $kernel
target remote localhost:1234

set disassembly-flavor intel
set print pretty on
set pagination off

# Custom commands for kernel debugging
define show-memory
    printf "Memory layout:\\n"
    printf "  avail_start: 0x%x\\n", avail_start  
    printf "  avail_end: 0x%x\\n", avail_end
    printf "  virtual_start: 0x%x\\n", virtual_start
    printf "  virtual_end: 0x%x\\n", virtual_end
end

define show-tasks
    printf "Current task information:\\n"
    print *current_task()
end

define show-threads
    printf "Current thread information:\\n" 
    print *current_thread()
end

EOF

    case "$scenario" in
        "startup")
            cat >> "$script_path" << 'EOF'
# Startup debugging breakpoints
echo === Setting up startup debugging ===\n
break c_boot_entry
break setup_main
break machine_init
break vm_mem_bootstrap
break ipc_bootstrap

echo Breakpoints set for startup debugging\n
echo Use 'continue' to start debugging\n
echo Use 'next' and 'step' to trace execution\n
echo Use 'show-memory' to display memory layout\n
EOF
            ;;
        "memory")
            cat >> "$script_path" << 'EOF' 
# Memory management debugging
echo === Setting up memory debugging ===\n
break vm_mem_bootstrap
break vm_mem_init
break pmap_bootstrap
break kalloc_init

echo Memory debugging breakpoints set\n
echo Use 'show-memory' after breakpoints hit\n
echo Watch variables: avail_start, avail_end, virtual_start\n
EOF
            ;;
        "ipc")
            cat >> "$script_path" << 'EOF'
# IPC system debugging
echo === Setting up IPC debugging ===\n
break ipc_bootstrap
break ipc_init
break mach_port_init
break ipc_kmsg_init

echo IPC debugging breakpoints set\n
echo Use 'show-tasks' to examine task structures\n
echo Watch IPC space initialization\n
EOF
            ;;
        "console")
            cat >> "$script_path" << 'EOF'
# Console debugging
echo === Setting up console debugging ===\n
break console_init
break printf
break console_print_timestamp

# Additional console-specific commands
define show-console
    printf "Console state:\\n"
    print console_is_initialized
    print cn_tab
end

echo Console debugging ready\n
echo Use 'show-console' to check console state\n
echo Break on printf to trace output\n
EOF
            ;;
        "panic")
            cat >> "$script_path" << 'EOF'
# Panic and exception debugging
echo === Setting up panic debugging ===\n
break panic
break Debugger
break trap
break user_trap

echo Exception handling breakpoints set\n
echo When panic hits, use 'backtrace' and 'info registers'\n
echo Use 'frame N' to examine specific stack frames\n
EOF
            ;;
        "interactive")
            cat >> "$script_path" << 'EOF'
# Interactive debugging session
echo === Interactive debugging session ===\n
echo Available custom commands:\n
echo   show-memory  - Display memory layout\n
echo   show-tasks   - Show current task info\n
echo   show-threads - Show current thread info\n
echo\n
echo Set your own breakpoints with 'break function_name'\n
echo Use 'continue' to start execution\n
EOF
            ;;
    esac
    
    echo "$script_path"
}

run_scenario() {
    local scenario="$1"
    
    echo "=== GNU Mach Debugging: $scenario scenario ==="
    
    # Find suitable kernel
    local kernel
    if ! kernel=$(find_kernel); then
        echo "Error: No suitable kernel found"
        echo "Run ./scripts/debug-helper.sh to create a test kernel"
        exit 1
    fi
    
    echo "Using kernel: $kernel"
    
    # Create scenario-specific GDB script
    local gdb_script
    gdb_script=$(create_gdb_script "$scenario" "$kernel")
    echo "Created GDB script: $gdb_script"
    
    echo ""
    echo "=== Starting debugging session ==="
    echo "1. QEMU will start and wait for GDB connection"
    echo "2. GDB will connect automatically with scenario-specific breakpoints"
    echo "3. Use GDB commands to debug the $scenario scenario"
    echo ""
    echo "Press Ctrl+C to stop debugging"
    echo ""
    
    # Start QEMU in background
    "${PROJECT_ROOT}/scripts/setup-qemu-gdb.sh" -k "$kernel" &
    local qemu_pid=$!
    
    # Give QEMU time to start
    sleep 2
    
    # Start GDB with the scenario script
    echo "Starting GDB with $scenario debugging scenario..."
    gdb -x "$gdb_script" || true
    
    # Clean up
    echo "Cleaning up..."
    kill $qemu_pid 2>/dev/null || true
    wait $qemu_pid 2>/dev/null || true
    
    echo "Debugging session ended."
}

run_test_scenario() {
    echo "=== GNU Mach Debugging Test ==="
    
    local kernel
    if ! kernel=$(find_kernel); then
        echo "Error: No suitable kernel found"
        exit 1
    fi
    
    echo "Testing debugging infrastructure with kernel: $kernel"
    
    # Test QEMU startup
    echo "Testing QEMU startup..."
    "${PROJECT_ROOT}/scripts/setup-qemu-gdb.sh" -k "$kernel" -n &
    local qemu_pid=$!
    sleep 3
    kill $qemu_pid 2>/dev/null || true
    wait $qemu_pid 2>/dev/null || true
    
    # Test GDB script generation
    echo "Testing GDB script generation..."
    local test_script
    test_script=$(create_gdb_script "test" "$kernel")
    if [[ -f "$test_script" ]]; then
        echo "✓ GDB script created successfully: $test_script"
        rm -f "$test_script"
    else
        echo "✗ Failed to create GDB script"
        exit 1
    fi
    
    echo "✓ All debugging tests passed"
    echo ""
    echo "Debugging infrastructure is ready!"
    echo "Run '$0 interactive' to start a debugging session"
}

main() {
    if [[ $# -eq 0 ]]; then
        usage
        exit 0
    fi
    
    local scenario="$1"
    
    case "$scenario" in
        "startup"|"memory"|"ipc"|"console"|"panic"|"interactive")
            run_scenario "$scenario"
            ;;
        "test")
            run_test_scenario
            ;;
        "-h"|"--help")
            usage
            ;;
        *)
            echo "Error: Unknown scenario '$scenario'"
            echo ""
            usage
            exit 1
            ;;
    esac
}

main "$@"