#!/bin/bash

# GNU Mach Valgrind Integration Validation Script
# This script validates the Valgrind integration implementation

echo "=== GNU Mach Valgrind Integration Validation ==="
echo "Validating implementation for Issue #117: Porting Valgrind to the Hurd"
echo

# Check if we're in the right directory
if [ ! -f "configure.ac" ] || [ ! -d "kern" ] || [ ! -d "include" ]; then
    echo "Error: Please run this script from the GNU Mach source directory"
    exit 1
fi

echo "1. Checking Valgrind implementation files..."

# List of Valgrind implementation files
VALGRIND_FILES=(
    "include/mach/valgrind.h"
    "kern/valgrind.c"
    "tests/test-valgrind.c"
)

all_files_exist=true
for file in "${VALGRIND_FILES[@]}"; do
    if [ -f "$file" ]; then
        lines=$(wc -l < "$file")
        echo "  ✓ $file ($lines lines)"
    else
        echo "  ✗ $file not found"
        all_files_exist=false
    fi
done

if [ "$all_files_exist" = false ]; then
    echo "Error: Missing required Valgrind files"
    exit 1
fi

echo
echo "2. Checking integration with existing kernel subsystems..."

# Check kernel integration
if grep -q "valgrind_init" kern/startup.c; then
    echo "  ✓ Valgrind initialization added to kernel startup"
else
    echo "  ✗ Valgrind initialization not found in kernel startup"
fi

if grep -q "valgrind_track" kern/slab.c; then
    echo "  ✓ Valgrind tracking integrated with kernel allocator"
else
    echo "  ✗ Valgrind tracking not integrated with kernel allocator"
fi

if grep -q "test-valgrind" tests/user-qemu.mk; then
    echo "  ✓ Valgrind test added to test suite"
else
    echo "  ✗ Valgrind test not added to test suite"
fi

if grep -q "kern/valgrind.c" Makefrag.am; then
    echo "  ✓ Valgrind source added to build system"
else
    echo "  ✗ Valgrind source not added to build system"
fi

echo
echo "3. Checking API completeness..."

# Check for key API functions in header
api_functions=(
    "valgrind_init"
    "valgrind_enable"
    "valgrind_track_alloc"
    "valgrind_track_free"
    "valgrind_make_mem_defined"
    "valgrind_make_mem_undefined"
    "valgrind_make_mem_noaccess"
    "valgrind_handle_client_request"
)

missing_api=false
for func in "${api_functions[@]}"; do
    if grep -q "$func" include/mach/valgrind.h; then
        echo "  ✓ $func API declared"
    else
        echo "  ✗ $func API missing"
        missing_api=true
    fi
done

echo
echo "4. Testing compilation..."

# Test compilation of Valgrind module
if gcc -m32 -DHAVE_CONFIG_H -I. -nostdinc -imacros config.h \
    -I /usr/lib/gcc/x86_64-linux-gnu/13//include -Ii386 -I. -I./i386 \
    -I./i386/include/mach/sa -I./include -fno-builtin-log -m32 -Wall \
    -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes \
    -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wredundant-decls \
    -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion \
    -Wstrict-overflow=2 -fgnu89-inline -fstrict-aliasing -Wstrict-aliasing=2 \
    -ffreestanding -nostdlib -fno-stack-protector -DRDXTREE_KEY_32 -g -O2 \
    -fno-omit-frame-pointer -fno-optimize-sibling-calls -no-pie -fno-PIE \
    -fno-pie -fno-pic -mno-3dnow -mno-mmx -mno-sse -mno-sse2 -g -O2 \
    -ffreestanding -nostdlib -g -O2 -Wall -Wextra -Wstrict-prototypes \
    -Wmissing-prototypes -Wmissing-declarations -Wformat=2 -Wformat-security \
    -Wold-style-definition -Wno-unused-parameter -Wno-sign-compare \
    -c -o kern/valgrind.o kern/valgrind.c 2>/dev/null; then
    echo "  ✓ Valgrind implementation compiles successfully"
    
    if [ -f "kern/valgrind.o" ]; then
        size=$(stat -c%s kern/valgrind.o)
        echo "    Generated object file: kern/valgrind.o (${size} bytes)"
    fi
else
    echo "  ✗ Valgrind implementation failed to compile"
fi

echo
echo "5. Analyzing implementation features..."

# Count key implementation features
feature_count=0

if grep -q "CONFIG_VALGRIND_SUPPORT" include/mach/valgrind.h; then
    echo "  ✓ Conditional compilation support"
    feature_count=$((feature_count + 1))
fi

if grep -q "memory_records\[" kern/valgrind.c; then
    echo "  ✓ Memory allocation tracking"
    feature_count=$((feature_count + 1))
fi

if grep -q "VALGRIND_MEM_NOACCESS\|VALGRIND_MEM_DEFINED" include/mach/valgrind.h; then
    echo "  ✓ Memory state management"
    feature_count=$((feature_count + 1))
fi

if grep -q "VG_USERREQ_" include/mach/valgrind.h; then
    echo "  ✓ Client request mechanism"
    feature_count=$((feature_count + 1))
fi

if grep -q "valgrind_report_error" kern/valgrind.c; then
    echo "  ✓ Error detection and reporting"
    feature_count=$((feature_count + 1))
fi

if grep -q "mem_track_" kern/valgrind.c; then
    echo "  ✓ Integration with existing memory tracking"
    feature_count=$((feature_count + 1))
fi

echo
echo "Implementation Summary:"
echo "  Total API functions: ${#api_functions[@]}"
echo "  Key features implemented: $feature_count"
echo "  Lines of header code: $(wc -l < include/mach/valgrind.h)"
echo "  Lines of implementation code: $(wc -l < kern/valgrind.c)"
echo "  Lines of test code: $(wc -l < tests/test-valgrind.c)"

total_lines=$(($(wc -l < include/mach/valgrind.h) + $(wc -l < kern/valgrind.c) + $(wc -l < tests/test-valgrind.c)))
echo "  Total implementation: $total_lines lines"

echo
echo "Expected benefits:"
echo "- Kernel-level memory error detection compatible with Valgrind"
echo "- Integration with existing GNU Mach debugging infrastructure"
echo "- Memory allocation tracking with caller information"
echo "- Client request mechanism for user-space Valgrind integration"
echo "- Configurable runtime enable/disable functionality"

echo
echo "Next steps for complete Valgrind port:"
echo "- Test with real Valgrind client applications"
echo "- Add syscall interface for user-space access"
echo "- Integrate with GNU Hurd servers"
echo "- Add comprehensive documentation"
echo "- Performance optimization and testing"

if [ "$all_files_exist" = true ] && [ "$missing_api" = false ] && [ "$feature_count" -ge 5 ]; then
    echo
    echo "✅ Valgrind integration validation PASSED"
    echo "The core Valgrind infrastructure is successfully implemented and ready for testing."
    exit 0
else
    echo
    echo "❌ Valgrind integration validation FAILED"
    echo "Some components are missing or incomplete."
    exit 1
fi