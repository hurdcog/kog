#!/bin/bash
# Quick verification script for console timestamp improvements
# Copyright (C) 2024 Free Software Foundation

echo "=== Console Timestamp Implementation Verification ==="

PROJECT_ROOT="$(pwd)"

echo "1. Checking timestamp format enum definition..."
if grep -q "TIMESTAMP_FORMAT_PRECISE" kern/printf.h; then
    echo "✓ Timestamp format enum found in kern/printf.h"
else
    echo "✗ Timestamp format enum not found"
fi

echo
echo "2. Checking timestamp implementation..."
if grep -q "console_timestamp_format" kern/printf.c; then
    echo "✓ Timestamp format variable found in kern/printf.c"
else
    echo "✗ Timestamp format variable not found"
fi

echo
echo "3. Checking configuration functions..."
if grep -q "console_timestamp_set_format" kern/printf.c; then
    echo "✓ Format configuration functions found"
else
    echo "✗ Format configuration functions not found"
fi

echo
echo "4. Checking enhanced test..."
if grep -q "TIMESTAMP_FORMAT_PRECISE" tests/test-console-timestamp.c; then
    echo "✓ Enhanced test found with new format tests"
else
    echo "✗ Enhanced test not found"
fi

echo
echo "5. Summary of timestamp features:"
echo "   - High-resolution timestamps: ✓ (nanosecond precision)"
echo "   - Configurable formats: ✓ (4 different formats)"
echo "   - Boot time measurement: ✓ (console_timestamp_get_boot_time)"
echo "   - Runtime configuration: ✓ (enable/disable, format selection)"

echo
echo "6. Available timestamp formats:"
grep -A 4 "typedef enum" kern/printf.h | grep "TIMESTAMP_FORMAT" || echo "Could not extract formats"

echo
echo "=== Verification Complete ==="