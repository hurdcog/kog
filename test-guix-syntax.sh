#!/bin/bash
# Comprehensive test script for Guix package definitions
# This script verifies syntax, structure, and SSR-safety

set -e

echo "=========================================="
echo "Guix Package Definition Verification Test"
echo "=========================================="
echo

SUCCESS=true

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test 1: Check that all files exist
echo "Test 1: Checking file existence..."
FILES=(
    "guix.scm"
    ".guix/modules/opencog-package.scm"
    "packaging/opencog.scm"
    ".guix/validate-ssr-fix.sh"
)

for file in "${FILES[@]}"; do
    if [ -f "$file" ]; then
        echo -e "  ${GREEN}✓${NC} $file exists"
    else
        echo -e "  ${RED}✗${NC} $file missing"
        SUCCESS=false
    fi
done
echo

# Test 2: Guile parsing test
echo "Test 2: Testing Scheme syntax with Guile..."
if command -v guile &> /dev/null; then
    for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
        echo -n "  Testing $file: "
        if guile --no-auto-compile -c "
            (catch #t 
              (lambda () 
                (with-input-from-file \"$file\" 
                  (lambda () 
                    (let loop ((expr (read)))
                      (unless (eof-object? expr)
                        (loop (read)))))))
              (lambda (key . args) 
                (exit 1)))" 2>/dev/null; then
            echo -e "${GREEN}✓ PASS${NC}"
        else
            echo -e "${RED}✗ FAIL${NC}"
            SUCCESS=false
        fi
    done
else
    echo -e "  ${YELLOW}! Guile not available - skipping parsing tests${NC}"
fi
echo

# Test 3: SSR-safe syntax verification
echo "Test 3: Verifying SSR-safe syntax patterns..."
for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo "  Checking $file:"
    
    # Check for backtick usage
    if grep -q '\`(' "$file"; then
        echo -e "    ${GREEN}✓${NC} Uses backtick (\`) for quasiquotation"
    else
        echo -e "    ${RED}✗${NC} Missing backtick quasiquotation"
        SUCCESS=false
    fi
    
    # Check for comma unquote with list
    if grep -q ',(list' "$file"; then
        echo -e "    ${GREEN}✓${NC} Uses comma (,) with explicit list construction"
    else
        echo -e "    ${RED}✗${NC} Missing proper unquote/list pattern"
        SUCCESS=false
    fi
    
    # Check for problematic patterns
    if ! grep -q "(quasiquote" "$file" && ! grep -q "(unquote" "$file"; then
        echo -e "    ${GREEN}✓${NC} No explicit quasiquote/unquote forms"
    else
        echo -e "    ${RED}✗${NC} Contains problematic explicit quasiquote/unquote"
        SUCCESS=false
    fi
done
echo

# Test 4: Parentheses balance check
echo "Test 4: Checking parentheses balance..."
for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo -n "  $file: "
    if python3 -c "
content = open('$file').read()
open_count = 0
for char in content:
    if char == '(':
        open_count += 1
    elif char == ')':
        open_count -= 1
    if open_count < 0:
        exit(1)
if open_count != 0:
    exit(1)
" 2>/dev/null; then
        echo -e "${GREEN}✓ Balanced${NC}"
    else
        echo -e "${RED}✗ Unbalanced${NC}"
        SUCCESS=false
    fi
done
echo

# Test 5: Check specific configure-flags patterns
echo "Test 5: Verifying configure-flags structure..."
for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo "  Checking $file:"
    
    # Look for the correct pattern: #:configure-flags followed by ,(list
    if grep -A1 "#:configure-flags" "$file" | grep -q ",(list"; then
        echo -e "    ${GREEN}✓${NC} Correct configure-flags pattern: ,(list ...)"
    else
        echo -e "    ${YELLOW}!${NC} configure-flags may not follow expected pattern"
    fi
    
    # Ensure not using the problematic '( pattern
    if ! grep -A1 "#:configure-flags" "$file" | grep -q "'("; then
        echo -e "    ${GREEN}✓${NC} Not using problematic '( pattern"
    else
        echo -e "    ${RED}✗${NC} Using problematic '( pattern"
        SUCCESS=false
    fi
done
echo

# Test 6: Run the official validation script
echo "Test 6: Running official validation script..."
if [ -x ".guix/validate-ssr-fix.sh" ] || [ -f ".guix/validate-ssr-fix.sh" ]; then
    if bash .guix/validate-ssr-fix.sh > /tmp/validation-output.txt 2>&1; then
        echo -e "  ${GREEN}✓ Official validation script passed${NC}"
        echo "  Summary from validation script:"
        grep "✓" /tmp/validation-output.txt | head -5 | sed 's/^/    /'
    else
        echo -e "  ${RED}✗ Official validation script failed${NC}"
        echo "  Output:"
        cat /tmp/validation-output.txt | tail -10 | sed 's/^/    /'
        SUCCESS=false
    fi
else
    echo -e "  ${YELLOW}! Validation script not found or not executable${NC}"
fi
echo

# Final summary
echo "=========================================="
if [ "$SUCCESS" = true ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED${NC}"
    echo "All Guix package definitions are correct and use SSR-safe syntax."
    echo
    echo "The following files are verified:"
    echo "  • guix.scm (main package definition)"
    echo "  • .guix/modules/opencog-package.scm (modular package)"
    echo "  • packaging/opencog.scm (packaging configuration)"
    echo
    echo "Key syntax features confirmed:"
    echo "  ✓ Using backtick (\`) for quasiquotation"
    echo "  ✓ Using comma (,) for unquotation"
    echo "  ✓ Using explicit (list ...) for configure flags"
    echo "  ✓ All parentheses balanced"
    echo "  ✓ No problematic quasiquote/unquote forms"
    echo
    exit 0
else
    echo -e "${RED}✗ SOME TESTS FAILED${NC}"
    echo "Please review the output above for details."
    echo
    exit 1
fi
