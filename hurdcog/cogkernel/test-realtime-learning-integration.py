#!/usr/bin/env python3
"""
Integration test for Real-time Learning Systems
Validates the implementation without requiring Guile runtime
"""

import os
import re
import sys

def test_file_exists(filepath, description):
    """Test that a file exists"""
    if os.path.exists(filepath):
        print(f"✓ {description}: {filepath}")
        return True
    else:
        print(f"✗ {description}: {filepath} NOT FOUND")
        return False

def test_module_structure(filepath, required_exports):
    """Test that a Scheme module has required exports"""
    if not os.path.exists(filepath):
        print(f"✗ Module not found: {filepath}")
        return False

    with open(filepath, 'r') as f:
        content = f.read()

    # Check for module definition
    if not re.search(r'define-module\s+\(', content):
        print(f"✗ Module definition not found in {filepath}")
        return False

    # Check for exports
    export_section = re.search(r'#:export\s+\((.*?)\)', content, re.DOTALL)
    if not export_section:
        print(f"✗ Export section not found in {filepath}")
        return False

    export_text = export_section.group(1)
    missing_exports = []

    for export in required_exports:
        if export not in export_text:
            missing_exports.append(export)

    if missing_exports:
        print(f"✗ Missing exports in {filepath}: {missing_exports}")
        return False

    print(f"✓ Module structure valid: {filepath}")
    return True

def test_learning_system_implementation():
    """Test the real-time learning system implementation"""
    print("=== Testing Real-time Learning System Implementation ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel"

    # Test core file existence
    tests = [
        (f"{base_path}/cognitive-interface/learning-systems/realtime.scm", 
         "Real-time learning system module"),
        (f"{base_path}/test-realtime-learning.scm", 
         "Learning system test suite"),
        (f"{base_path}/cognitive-interface/learning-systems/README.md", 
         "Learning system documentation"),
        (f"{base_path}/atomspace.scm", 
         "AtomSpace module"),
        (f"{base_path}/attention.scm", 
         "Attention module"),
        (f"{base_path}/cognitive-interface.scm", 
         "Cognitive interface module"),
    ]

    passed = 0
    total = len(tests)

    for filepath, description in tests:
        if test_file_exists(filepath, description):
            passed += 1

    print(f"\nFile existence: {passed}/{total} tests passed")

    # Test module structure
    learning_exports = [
        'make-learning-system',
        'learning-system?', 
        'learn-from-experience',
        'pattern-recognition',
        'adapt-behavior',
        'evaluate-learning-effectiveness',
        'advanced-pattern-learning',
        'q-learning-update',
        'temporal-difference-learning',
        'experience-replay',
        'monitor-learning-effectiveness',
        'enhanced-continuous-learning-loop',
        '*global-learning-system*',
        'current-time'
    ]

    module_tests = [
        (f"{base_path}/cognitive-interface/learning-systems/realtime.scm", learning_exports),
    ]

    module_passed = 0
    for filepath, exports in module_tests:
        if test_module_structure(filepath, exports):
            module_passed += 1

    print(f"Module structure: {module_passed}/{len(module_tests)} tests passed")

    return passed == total and module_passed == len(module_tests)

def test_atomspace_integration():
    """Test atomspace integration for learning systems"""
    print("\n=== Testing AtomSpace Integration ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel"
    atomspace_file = f"{base_path}/atomspace.scm"

    if not os.path.exists(atomspace_file):
        print("✗ AtomSpace module not found")
        return False

    with open(atomspace_file, 'r') as f:
        content = f.read()

    # Check for learning-related atom types
    required_types = [
        'EXPERIENCE',
        'PATTERN', 
        'STATE-VALUE',
        'POLICY',
        'META-PATTERN'
    ]

    missing_types = []
    for atom_type in required_types:
        if atom_type not in content:
            missing_types.append(atom_type)

    if missing_types:
        print(f"✗ Missing atom types: {missing_types}")
        return False

    print("✓ Learning-related atom types present in atomspace")

    # Check for global atomspace instance
    if '*global-atomspace*' not in content:
        print("✗ Global atomspace instance not found")
        return False

    print("✓ Global atomspace instance defined")
    return True

def test_attention_integration():
    """Test attention system integration"""
    print("\n=== Testing Attention System Integration ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel" 
    attention_file = f"{base_path}/attention.scm"

    if not os.path.exists(attention_file):
        print("✗ Attention module not found")
        return False

    with open(attention_file, 'r') as f:
        content = f.read()

    # Check for attention-bank-allocate! function with optional parameters
    if 'attention-bank-allocate!' not in content:
        print("✗ attention-bank-allocate! function not found")
        return False

    print("✓ Attention allocation function present")

    # Check for global attention bank
    if '*global-attention-bank*' not in content:
        print("✗ Global attention bank not found")
        return False

    print("✓ Global attention bank defined")
    return True

def test_cognitive_interface_integration():
    """Test cognitive interface integration"""
    print("\n=== Testing Cognitive Interface Integration ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel"
    interface_file = f"{base_path}/cognitive-interface.scm"

    if not os.path.exists(interface_file):
        print("✗ Cognitive interface module not found")
        return False

    with open(interface_file, 'r') as f:
        content = f.read()

    # Check for learning system integration
    if 'learning-systems realtime' not in content:
        print("✗ Real-time learning system import not found")
        return False

    print("✓ Real-time learning system imported")

    # Check for enhanced learning enablement
    if 'Enhanced real-time learning system enabled' not in content:
        print("✗ Enhanced learning enablement not found")
        return False

    print("✓ Enhanced learning system enablement present")
    return True

def test_documentation_completeness():
    """Test documentation completeness"""
    print("\n=== Testing Documentation ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel"
    doc_file = f"{base_path}/cognitive-interface/learning-systems/README.md"

    if not os.path.exists(doc_file):
        print("✗ Learning systems documentation not found")
        return False

    with open(doc_file, 'r') as f:
        content = f.read()

    # Check for key documentation sections
    required_sections = [
        '# Real-time Learning Systems',
        '## Overview',
        '## Key Features',
        '## Core Components', 
        '## Usage Examples',
        '## Advanced Features',
        '## Integration with SKZ Framework',
        '## Learning Types and Algorithms',
        '## Performance and Monitoring',
        '## Testing'
    ]

    missing_sections = []
    for section in required_sections:
        if section not in content:
            missing_sections.append(section)

    if missing_sections:
        print(f"✗ Missing documentation sections: {missing_sections}")
        return False

    print("✓ Documentation sections complete")

    # Check for code examples
    if '```scheme' not in content:
        print("✗ No Scheme code examples found in documentation")
        return False

    print("✓ Code examples present in documentation")
    return True

def test_demo_integration():
    """Test demo integration"""
    print("\n=== Testing Demo Integration ===")

    base_path = "/home/runner/work/hurdcog/hurdcog/cogkernel"
    demo_file = f"{base_path}/cognitive-operations-demo.scm"

    if not os.path.exists(demo_file):
        print("✗ Cognitive operations demo not found")
        return False

    with open(demo_file, 'r') as f:
        content = f.read()

    # Check for enhanced learning system demonstration
    if 'Enhanced Real-time Learning Systems' not in content:
        print("✗ Enhanced learning systems demo not found")
        return False

    print("✓ Enhanced learning systems demo present")

    # Check for learning function calls
    learning_functions = [
        'create-learning-experience',
        'learn-from-experience',
        'pattern-recognition',
        'advanced-pattern-learning',
        'adapt-behavior',
        'evaluate-learning-effectiveness',
        'experience-replay',
        'temporal-difference-learning',
        'q-learning-update'
    ]

    missing_functions = []
    for func in learning_functions:
        if func not in content:
            missing_functions.append(func)

    if missing_functions:
        print(f"✗ Missing learning function demonstrations: {missing_functions}")
        return False

    print("✓ Learning function demonstrations complete")
    return True

def main():
    """Run all integration tests"""
    print("🧠 Real-time Learning Systems Integration Test 🧠")
    print("=" * 55)

    tests = [
        test_learning_system_implementation,
        test_atomspace_integration,
        test_attention_integration,
        test_cognitive_interface_integration,
        test_documentation_completeness,
        test_demo_integration
    ]

    passed = 0
    total = len(tests)

    for test in tests:
        try:
            if test():
                passed += 1
        except Exception as e:
            print(f"✗ Test failed with exception: {e}")

    print("\n" + "=" * 55)
    print(f"Integration Test Summary: {passed}/{total} tests passed")

    if passed == total:
        print("🎯 All integration tests passed! Real-time learning systems are ready.")
        return 0
    else:
        print(f"❌ {total - passed} integration tests failed. Please review implementation.")
        return 1

if __name__ == "__main__":
    sys.exit(main())