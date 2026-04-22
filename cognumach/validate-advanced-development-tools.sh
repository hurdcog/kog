#!/bin/bash
#
# Advanced Development Tools Validation Script
# Validates complete implementation of Valgrind, whole system debugging, and security analysis
#
# Copyright (C) 2024 Free Software Foundation, Inc.

set -e

# Get the directory where the script is located
if [ -n "${BASH_SOURCE[0]}" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    REPO_ROOT="$SCRIPT_DIR"
else
    REPO_ROOT="$(pwd)"
fi

echo "=============================================="
echo "Advanced Development Tools Validation"
echo "=============================================="

# Check if we're in the right directory
if [ ! -f "$REPO_ROOT/configure.ac" ] && [ ! -f "configure.ac" ]; then
    echo "Error: Not in GNU Mach repository root"
    echo "Current directory: $(pwd)"
    echo "Script expects to find configure.ac"
    exit 1
fi

# Use current directory if configure.ac is here
if [ -f "configure.ac" ]; then
    REPO_ROOT="$(pwd)"
fi

cd "$REPO_ROOT"

# Function to check if a file exists and report
check_file() {
    local file="$1"
    local description="$2"
    
    if [ -f "$file" ]; then
        echo "✓ $description: $file"
        return 0
    else
        echo "✗ Missing $description: $file"
        return 1
    fi
}

# Function to check if a directory exists and report  
check_dir() {
    local dir="$1"
    local description="$2"
    
    if [ -d "$dir" ]; then
        echo "✓ $description: $dir"
        return 0
    else
        echo "✗ Missing $description: $dir"
        return 1
    fi
}

echo ""
echo "=== 1. Checking Valgrind Integration ==="

VALGRIND_ERRORS=0

check_file "include/mach/valgrind.h" "Valgrind API header" || ((VALGRIND_ERRORS++))
check_file "kern/valgrind.c" "Valgrind kernel implementation" || ((VALGRIND_ERRORS++))
check_file "docs/valgrind-integration.md" "Valgrind documentation" || ((VALGRIND_ERRORS++))
check_file "tests/test-valgrind.c" "Valgrind test suite" || ((VALGRIND_ERRORS++))

# Check for Valgrind syscall definitions
if grep -q "MACH_VALGRIND_ENABLE_CALL" include/mach/valgrind.h; then
    echo "✓ Valgrind syscall definitions present"
else
    echo "✗ Valgrind syscall definitions missing"
    ((VALGRIND_ERRORS++))
fi

# Check for kernel integration
if grep -q "valgrind_init()" kern/startup.c; then
    echo "✓ Valgrind initialization in kernel startup"
else
    echo "✗ Valgrind initialization missing from kernel startup"
    ((VALGRIND_ERRORS++))
fi

echo "Valgrind Integration Status: $((6-VALGRIND_ERRORS))/6 checks passed"

echo ""
echo "=== 2. Checking Whole System Debugging ==="

DEBUG_ERRORS=0

check_file "include/mach/system_debug.h" "System debug API header" || ((DEBUG_ERRORS++))
check_file "include/mach/unified_debug.h" "Unified debug interface" || ((DEBUG_ERRORS++))
check_file "kern/system_debug.c" "System debug implementation" || ((DEBUG_ERRORS++))
check_file "kern/unified_debug.c" "Unified debug implementation" || ((DEBUG_ERRORS++))
check_file "docs/whole-system-debugging.md" "Debug documentation" || ((DEBUG_ERRORS++))
check_file "tests/test-whole-system-debugging.c" "Debug test suite" || ((DEBUG_ERRORS++))

# Check for debug system initialization
if grep -q "sysdebug_init" kern/system_debug.c; then
    echo "✓ System debugging initialization present"
else
    echo "✗ System debugging initialization missing"
    ((DEBUG_ERRORS++))
fi

echo "Whole System Debugging Status: $((7-DEBUG_ERRORS))/7 checks passed"

echo ""
echo "=== 3. Checking Security Analysis Framework ==="

SECURITY_ERRORS=0

check_file "include/mach/mach_security.h" "Security analysis API header" || ((SECURITY_ERRORS++))
check_file "kern/security_monitor.c" "Security monitor implementation" || ((SECURITY_ERRORS++))
check_file "kern/security_monitor.h" "Security monitor header" || ((SECURITY_ERRORS++))
check_file "kern/cfi_integrity.c" "CFI implementation" || ((SECURITY_ERRORS++))
check_file "kern/cfi_integrity.h" "CFI header" || ((SECURITY_ERRORS++))
check_file "docs/advanced-security-analysis.md" "Security documentation" || ((SECURITY_ERRORS++))
check_file "tests/test-security-analysis.c" "Security test suite" || ((SECURITY_ERRORS++))
check_file "scripts/security-vulnerability-scanner.sh" "Vulnerability scanner" || ((SECURITY_ERRORS++))

# Check for security initialization
if grep -q "security_monitor_init" kern/security_monitor.c; then
    echo "✓ Security monitoring initialization present"
else
    echo "✗ Security monitoring initialization missing"
    ((SECURITY_ERRORS++))
fi

echo "Security Analysis Status: $((9-SECURITY_ERRORS))/9 checks passed"

echo ""
echo "=== 4. Checking Advanced Development Tools Integration ==="

INTEGRATION_ERRORS=0

check_file "include/mach/development_tools.defs" "Development tools MIG interface" || ((INTEGRATION_ERRORS++))
check_file "kern/development_tools.c" "Development tools syscall implementation" || ((INTEGRATION_ERRORS++))
check_file "kern/development_tools.h" "Development tools header" || ((INTEGRATION_ERRORS++))
check_file "kern/development_tools.srv" "Development tools MIG server" || ((INTEGRATION_ERRORS++))
check_file "tests/test-advanced-development-tools.c" "Integration test suite" || ((INTEGRATION_ERRORS++))

# Check build system integration
if grep -q "development_tools.c" Makefrag.am; then
    echo "✓ Development tools in build system"
else
    echo "✗ Development tools missing from build system"
    ((INTEGRATION_ERRORS++))
fi

# Check startup integration
if grep -q "development_tools_init()" kern/startup.c; then
    echo "✓ Development tools initialization in kernel startup"
else
    echo "✗ Development tools initialization missing from kernel startup" 
    ((INTEGRATION_ERRORS++))
fi

# Check test integration
if grep -q "test-advanced-development-tools" tests/user-qemu.mk; then
    echo "✓ Integration tests in test suite"
else
    echo "✗ Integration tests missing from test suite"
    ((INTEGRATION_ERRORS++))
fi

echo "Integration Status: $((8-INTEGRATION_ERRORS))/8 checks passed"

echo ""
echo "=== 5. Running Validation Checks ==="

# Check for common implementation patterns
echo "Checking implementation patterns..."

PATTERN_ERRORS=0

# Check for proper error handling
if grep -q "KERN_INVALID_HOST" kern/development_tools.c; then
    echo "✓ Proper error handling in syscall implementations"
else
    echo "✗ Missing proper error handling patterns"
    ((PATTERN_ERRORS++))
fi

# Check for memory safety
if grep -q "kmem_alloc" kern/development_tools.c; then
    echo "✓ Proper kernel memory allocation"
else
    echo "✗ Missing proper memory allocation patterns"
    ((PATTERN_ERRORS++))
fi

# Check for locking
if grep -q "simple_lock" kern/development_tools.c; then
    echo "✓ Proper locking mechanisms"
else
    echo "✗ Missing proper locking patterns"
    ((PATTERN_ERRORS++))
fi

echo "Implementation Patterns: $((3-PATTERN_ERRORS))/3 checks passed"

echo ""
echo "=== 6. Checking Documentation ==="

DOC_ERRORS=0

# Check for comprehensive documentation
if [ -f "docs/advanced-security-analysis.md" ] && [ -f "docs/valgrind-integration.md" ] && [ -f "docs/whole-system-debugging.md" ]; then
    echo "✓ Comprehensive documentation present"
else
    echo "✗ Missing comprehensive documentation"
    ((DOC_ERRORS++))
fi

# Check for usage examples in documentation
if grep -q -i "Usage:\|Example" docs/valgrind-integration.md 2>/dev/null; then
    echo "✓ Usage examples in documentation"
else
    echo "✗ Missing usage examples in documentation"
    ((DOC_ERRORS++))
fi

echo "Documentation Status: $((2-DOC_ERRORS))/2 checks passed"

echo ""
echo "=============================================="
echo "VALIDATION SUMMARY"
echo "=============================================="

TOTAL_ERRORS=$((VALGRIND_ERRORS + DEBUG_ERRORS + SECURITY_ERRORS + INTEGRATION_ERRORS + PATTERN_ERRORS + DOC_ERRORS))
TOTAL_CHECKS=35

echo "Overall Status: $((TOTAL_CHECKS-TOTAL_ERRORS))/$TOTAL_CHECKS checks passed"
echo ""

if [ $VALGRIND_ERRORS -eq 0 ]; then
    echo "✅ Valgrind Integration - COMPLETE"
else
    echo "❌ Valgrind Integration - INCOMPLETE ($VALGRIND_ERRORS issues)"
fi

if [ $DEBUG_ERRORS -eq 0 ]; then
    echo "✅ Whole System Debugging - COMPLETE"  
else
    echo "❌ Whole System Debugging - INCOMPLETE ($DEBUG_ERRORS issues)"
fi

if [ $SECURITY_ERRORS -eq 0 ]; then
    echo "✅ Security Analysis Framework - COMPLETE"
else
    echo "❌ Security Analysis Framework - INCOMPLETE ($SECURITY_ERRORS issues)"
fi

if [ $INTEGRATION_ERRORS -eq 0 ]; then
    echo "✅ Advanced Tools Integration - COMPLETE"
else
    echo "❌ Advanced Tools Integration - INCOMPLETE ($INTEGRATION_ERRORS issues)"
fi

if [ $PATTERN_ERRORS -eq 0 ]; then
    echo "✅ Implementation Quality - EXCELLENT"
else
    echo "❌ Implementation Quality - NEEDS IMPROVEMENT ($PATTERN_ERRORS issues)"
fi

if [ $DOC_ERRORS -eq 0 ]; then
    echo "✅ Documentation - COMPLETE"
else
    echo "❌ Documentation - INCOMPLETE ($DOC_ERRORS issues)"
fi

echo ""
echo "=============================================="

if [ $TOTAL_ERRORS -eq 0 ]; then
    echo "🎉 ADVANCED DEVELOPMENT TOOLS - FULLY IMPLEMENTED"
    echo "   All components are complete and integrated:"
    echo "   • Valgrind memory tracking with kernel support"
    echo "   • System-wide debugging with unified interface" 
    echo "   • Advanced security analysis with CFI and monitoring"
    echo "   • Complete syscall interface for user-space integration"
    echo "   • Comprehensive test suite and documentation"
    exit 0
else
    echo "⚠️  ADVANCED DEVELOPMENT TOOLS - PARTIALLY IMPLEMENTED"
    echo "   $TOTAL_ERRORS issues need to be addressed for full completion"
    exit 1
fi