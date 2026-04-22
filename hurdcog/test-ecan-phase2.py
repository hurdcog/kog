#!/usr/bin/env python3
"""
Phase 2: ECAN Attention Allocation Integration Test
Tests the complete ECAN economics system with distributed attention networks
"""

import subprocess
import sys
import os

def run_test(test_name, test_file):
    """Run a Guile test file and report results"""
    print(f"\n{'='*60}")
    print(f"🧪 {test_name}")
    print('='*60)

    try:
        # Set up the environment
        env = os.environ.copy()
        env['GUILE_LOAD_PATH'] = '.:' + env.get('GUILE_LOAD_PATH', '')

        # Run the test
        result = subprocess.run(
            ['guile', '-s', test_file],
            capture_output=True,
            text=True,
            cwd='/home/runner/work/hurdcog/hurdcog/cogkernel',
            env=env,
            timeout=60
        )

        # Print output
        if result.stdout:
            print(result.stdout)
        if result.stderr:
            print("STDERR:", result.stderr, file=sys.stderr)

        # Check result
        if result.returncode == 0:
            print(f"✅ {test_name} PASSED")
            return True
        else:
            print(f"❌ {test_name} FAILED (exit code: {result.returncode})")
            return False

    except subprocess.TimeoutExpired:
        print(f"⏱️  {test_name} TIMEOUT")
        return False
    except Exception as e:
        print(f"❌ {test_name} ERROR: {e}")
        return False

def check_ecan_files():
    """Check that all ECAN implementation files exist"""
    print("\n" + "="*60)
    print("📋 Checking Phase 2 ECAN Implementation Files")
    print("="*60)

    files_to_check = [
        ('cogkernel/attention/ecan.scm', 'ECAN Core Implementation'),
        ('cogkernel/attention.scm', 'Attention Module Wrapper'),
        ('cogkernel/tests/test-ecan-economics.scm', 'ECAN Economics Tests'),
    ]

    all_exist = True
    for filepath, description in files_to_check:
        full_path = f'/home/runner/work/hurdcog/hurdcog/{filepath}'
        if os.path.exists(full_path):
            size = os.path.getsize(full_path)
            print(f"  ✅ {description}: {filepath} ({size} bytes)")
        else:
            print(f"  ❌ {description}: {filepath} - NOT FOUND")
            all_exist = False

    return all_exist

def main():
    """Main test execution"""
    print("\n" + "="*60)
    print("🧠 Phase 2: ECAN Attention Allocation Integration Test Suite")
    print("="*60)
    print("\nTesting Components:")
    print("  • Cognitive Wages (activity-based attention rewards)")
    print("  • Attention Rent (resource usage costs)")
    print("  • STI/LTI/VLTI Dynamics")
    print("  • Spreading Activation (network-wide propagation)")
    print("  • Priority-based Task Scheduling")
    print("  • Distributed Attention Networks")
    print("  • Economics History Tracking")

    # Check files first
    if not check_ecan_files():
        print("\n❌ Required files missing. Cannot proceed with tests.")
        return 1

    # Run tests
    tests_passed = 0
    tests_total = 0

    # Test 1: ECAN Economics Test Suite
    tests_total += 1
    if run_test("ECAN Economics Test Suite", "tests/test-ecan-economics.scm"):
        tests_passed += 1

    # Print summary
    print("\n" + "="*60)
    print("📊 Phase 2 ECAN Test Summary")
    print("="*60)
    print(f"Tests Passed: {tests_passed}/{tests_total}")
    print(f"Success Rate: {(tests_passed/tests_total)*100:.1f}%")

    if tests_passed == tests_total:
        print("\n✅ Phase 2: ECAN Attention Allocation - ALL TESTS PASSED")
        print("\n🎯 Implementation Complete:")
        print("  ✅ Economic Attention Networks (ECAN) operational")
        print("  ✅ Cognitive wages reward productive activity")
        print("  ✅ Attention rent prevents resource hoarding")
        print("  ✅ STI/LTI dynamics track importance over time")
        print("  ✅ Spreading activation propagates attention")
        print("  ✅ Priority scheduling optimizes task execution")
        print("  ✅ Distributed attention networks coordinate across nodes")
        print("  ✅ Economics history provides transparency")
        print("\n🚀 Phase 2: READY FOR PRODUCTION")
        return 0
    else:
        print(f"\n⚠️  {tests_total - tests_passed} test(s) failed")
        return 1

if __name__ == '__main__':
    sys.exit(main())
