#!/bin/bash
# Comprehensive test to demonstrate the Guix SSR syntax fix

echo "=== Guix SSR Syntax Fix Validation ==="
echo

echo "1. Testing that all Guix files parse correctly..."
for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo -n "   Testing $file: "
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
        echo "✓ PASS"
    else
        echo "✗ FAIL"
        exit 1
    fi
done

echo
echo "2. Checking that arguments use SSR-safe syntax..."

# Check that files don't use the problematic backtick + quote pattern
for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo -n "   Checking $file for old syntax: "
    if grep -q "#:configure-flags" "$file" && grep -A1 "#:configure-flags" "$file" | grep -q "'("; then
        echo "✗ FAIL - still contains problematic backtick+quote syntax"
        exit 1
    else
        echo "✓ PASS - uses SSR-safe list syntax"
    fi
done

echo
echo "3. Verifying new list-based syntax..."

for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo -n "   Checking $file for new syntax: "
    if grep -q "#:configure-flags" "$file" && grep -A1 "#:configure-flags" "$file" | grep -q "(list"; then
        echo "✓ PASS - uses explicit list construction"
    else
        echo "✗ FAIL - missing explicit list syntax"
        exit 1
    fi
done

echo
echo "4. Testing parentheses balance..."

for file in "guix.scm" ".guix/modules/opencog-package.scm" "packaging/opencog.scm"; do
    echo -n "   Checking $file parentheses: "
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
        echo "✓ PASS - balanced"
    else
        echo "✗ FAIL - unbalanced"
        exit 1
    fi
done

echo
echo "=== Summary ==="
echo "✓ All Guix files now use SSR-safe syntax"
echo "✓ Replaced backtick+quote with explicit list construction"  
echo "✓ Fixed: \`(#:configure-flags '(...)) → (#:configure-flags (list ...))"
echo "✓ All parentheses are properly balanced"
echo "✓ Files parse correctly without syntax errors"
echo
echo "The Guix build CI should now work correctly!"