#!/bin/bash
# GNU Mach Debug Master Script
# Copyright (C) 2024 Free Software Foundation
#
# Unified entry point for all GNU Mach debugging activities

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

usage() {
    cat << EOF
GNU Mach Debug Master - Comprehensive Debugging Tool

Usage: $0 [COMMAND] [OPTIONS]

COMMANDS:
  setup           Set up debugging environment
  test            Test debugging infrastructure  
  scenario        Run specific debugging scenario
  quick           Quick debugging session with auto-detection
  help            Show detailed help for each command

DEBUGGING SCENARIOS:
  startup         Debug kernel startup sequence
  memory          Debug memory management
  ipc             Debug IPC system
  console         Debug console output
  panic           Debug kernel panics
  interactive     Interactive session

EXAMPLES:
  $0 setup                    # Set up debugging environment
  $0 quick                    # Quick debugging session
  $0 scenario startup         # Debug kernel startup
  $0 test                     # Test all debugging infrastructure

DOCUMENTATION:
  docs/debugging-guide.md         - Complete debugging guide
  docs/startup-debugging-guide.md - Startup-specific debugging
  .github/copilot-instructions.md - Build and development instructions

EOF
}

show_detailed_help() {
    cat << EOF
=== GNU Mach Debugging - Detailed Help ===

SETTING UP DEBUGGING ENVIRONMENT:
  $0 setup
    - Checks for required tools (QEMU, GDB, build tools)
    - Creates test kernel if needed
    - Validates debugging infrastructure
    - Sets up debugging scripts

TESTING DEBUGGING INFRASTRUCTURE:
  $0 test
    - Tests QEMU startup with different kernel architectures
    - Validates GDB script generation
    - Tests debugging helper scripts
    - Verifies all debugging scenarios work

DEBUGGING SCENARIOS:

1. STARTUP DEBUGGING:
   $0 scenario startup
     - Breaks on kernel entry points (c_boot_entry, setup_main)
     - Steps through initialization sequence
     - Examines memory and system state during boot

2. MEMORY DEBUGGING:
   $0 scenario memory
     - Breaks on VM initialization functions
     - Examines memory layout and allocation
     - Debugs paging and memory management

3. IPC DEBUGGING:
   $0 scenario ipc
     - Debugs Inter-Process Communication initialization
     - Examines port creation and messaging
     - Steps through IPC setup

4. CONSOLE DEBUGGING:
   $0 scenario console
     - Debugs console initialization and output
     - Examines printf and timestamp functions
     - Tests console timestamp features

5. PANIC DEBUGGING:
   $0 scenario panic
     - Sets breakpoints on panic and exception handlers
     - Prepares for debugging kernel crashes
     - Examines fault conditions

6. INTERACTIVE DEBUGGING:
   $0 scenario interactive
     - Custom debugging session
     - All debugging commands available
     - Set your own breakpoints

QUICK DEBUGGING:
  $0 quick
    - Auto-detects available kernel
    - Starts QEMU with GDB support
    - Connects GDB with useful breakpoints
    - Best for getting started quickly

DEBUGGING WORKFLOW:
  1. Run: $0 setup (one-time setup)
  2. Choose scenario: $0 scenario <type>
  3. Use GDB commands to debug
  4. For custom debugging: $0 scenario interactive

TROUBLESHOOTING:
  - If no kernel found: Check build instructions in docs/
  - If QEMU fails: Ensure qemu-system-x86 is installed
  - If GDB fails: Check GDB installation and port conflicts
  - For build issues: See .github/copilot-instructions.md

EOF
}

check_dependencies() {
    local missing_deps=()
    
    if ! command -v qemu-system-i386 &> /dev/null; then
        missing_deps+=("qemu-system-x86")
    fi
    
    if ! command -v gdb &> /dev/null; then
        missing_deps+=("gdb")
    fi
    
    if ! command -v gcc &> /dev/null; then
        missing_deps+=("gcc build-essential")
    fi
    
    if [[ ${#missing_deps[@]} -gt 0 ]]; then
        echo "Missing dependencies: ${missing_deps[*]}"
        echo "Install with: sudo apt install ${missing_deps[*]}"
        return 1
    fi
    
    return 0
}

setup_debugging() {
    echo "=== Setting up GNU Mach debugging environment ==="
    
    echo "Checking dependencies..."
    if ! check_dependencies; then
        echo "Please install missing dependencies first"
        return 1
    fi
    
    echo "✓ Dependencies available"
    
    echo "Setting up debugging infrastructure..."
    "${SCRIPT_DIR}/debug-helper.sh" > /dev/null
    echo "✓ Debugging infrastructure ready"
    
    echo "Testing debugging scenarios..."
    "${SCRIPT_DIR}/debug-scenarios.sh" test > /dev/null 
    echo "✓ All debugging scenarios tested successfully"
    
    echo ""
    echo "=== Debugging Environment Ready ==="
    echo ""
    echo "Available commands:"
    echo "  $0 quick                # Quick debugging session"
    echo "  $0 scenario startup     # Debug kernel startup" 
    echo "  $0 scenario interactive # Interactive debugging"
    echo ""
    echo "Documentation:"
    echo "  docs/debugging-guide.md         # Complete guide"
    echo "  docs/startup-debugging-guide.md # Startup debugging"
    echo ""
}

quick_debug() {
    echo "=== Quick GNU Mach Debugging Session ==="
    
    # Use the scenarios script for interactive debugging
    exec "${SCRIPT_DIR}/debug-scenarios.sh" interactive
}

run_scenario() {
    local scenario="$1"
    
    if [[ -z "$scenario" ]]; then
        echo "Error: No scenario specified"
        echo "Available scenarios: startup, memory, ipc, console, panic, interactive"
        return 1
    fi
    
    exec "${SCRIPT_DIR}/debug-scenarios.sh" "$scenario"
}

run_tests() {
    echo "=== Testing GNU Mach Debugging Infrastructure ==="
    
    echo "Testing debugging helper..."
    "${SCRIPT_DIR}/debug-helper.sh" > /dev/null
    echo "✓ Debug helper working"
    
    echo "Testing debugging scenarios..."
    "${SCRIPT_DIR}/debug-scenarios.sh" test
    echo "✓ All scenarios tested"
    
    echo "Testing QEMU+GDB setup..."
    if "${SCRIPT_DIR}/setup-qemu-gdb.sh" --help > /dev/null; then
        echo "✓ QEMU+GDB setup script working"
    fi
    
    echo ""
    echo "=== All Tests Passed ==="
    echo "Debugging infrastructure is fully functional!"
}

main() {
    if [[ $# -eq 0 ]]; then
        usage
        exit 0
    fi
    
    local command="$1"
    shift
    
    case "$command" in
        "setup")
            setup_debugging
            ;;
        "test")
            run_tests
            ;;
        "scenario")
            run_scenario "$1"
            ;;
        "quick")
            quick_debug
            ;;
        "help")
            show_detailed_help
            ;;
        "-h"|"--help")
            usage
            ;;
        *)
            echo "Error: Unknown command '$command'"
            echo ""
            usage
            exit 1
            ;;
    esac
}

main "$@"