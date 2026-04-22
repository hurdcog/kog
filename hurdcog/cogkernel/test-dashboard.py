#!/usr/bin/env python3
"""
Test script for Cognitive Fusion Reactor Master Control Dashboard
Validates server functionality and API endpoints
"""

import sys
import os
import json

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def test_imports():
    """Test that all required modules can be imported"""
    print("Testing imports...")
    try:
        import http.server
        from urllib.parse import urlparse, parse_qs
        import datetime
        import json
        import random
        print("✅ All imports successful")
        return True
    except ImportError as e:
        print(f"❌ Import failed: {e}")
        return False

def test_reactor_status():
    """Test ReactorStatus class"""
    print("\nTesting ReactorStatus class...")
    try:
        # Import the server module
        import importlib.util
        spec = importlib.util.spec_from_file_location("fusion_reactor_server", "fusion-reactor-server.py")
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)

        # Create a ReactorStatus instance
        reactor = module.ReactorStatus()

        # Test basic attributes
        assert reactor.status == "ONLINE", "Status should be ONLINE"
        assert reactor.fusion_mode == "meta-evolution", "Fusion mode should be meta-evolution"
        assert reactor.version == "1.0.0", "Version should be 1.0.0"

        # Test phases
        assert len(reactor.phases) == 6, "Should have 6 phases"
        for phase_id, phase in reactor.phases.items():
            assert phase["status"] == "COMPLETE", f"Phase {phase_id} should be COMPLETE"
            assert phase["progress"] == 100, f"Phase {phase_id} should have 100% progress"

        # Test metrics
        assert "atomspace" in reactor.metrics, "Should have atomspace metrics"
        assert "ecan" in reactor.metrics, "Should have ECAN metrics"
        assert "neural_symbolic" in reactor.metrics, "Should have neural-symbolic metrics"
        assert "distributed_mesh" in reactor.metrics, "Should have distributed mesh metrics"
        assert "meta_cognition" in reactor.metrics, "Should have meta-cognition metrics"
        assert "system_health" in reactor.metrics, "Should have system health metrics"

        # Test get_status
        status = reactor.get_status()
        assert status["status"] == "ONLINE", "Status in get_status should be ONLINE"
        assert "phases" in status, "get_status should include phases"
        assert "metrics" in status, "get_status should include metrics"
        assert "logs" in status, "get_status should include logs"

        # Test update_metrics
        initial_nodes = reactor.metrics["atomspace"]["nodes"]
        reactor.update_metrics()
        # Nodes should have changed (but within bounds)
        assert 10000 <= reactor.metrics["atomspace"]["nodes"] <= 15000, "Atomspace nodes should be within bounds"

        # Test add_log
        initial_log_count = len(reactor.logs)
        reactor.add_log("Test log entry")
        assert len(reactor.logs) == initial_log_count + 1, "Log should be added"
        assert reactor.logs[-1]["message"] == "Test log entry", "Log message should match"

        print("✅ ReactorStatus tests passed")
        return True
    except Exception as e:
        print(f"❌ ReactorStatus test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

def test_html_file():
    """Test that dashboard HTML file exists and is valid"""
    print("\nTesting dashboard HTML file...")
    try:
        html_path = "fusion-reactor-dashboard.html"
        if not os.path.exists(html_path):
            print(f"❌ HTML file not found: {html_path}")
            return False

        with open(html_path, 'r') as f:
            content = f.read()

        # Check for key components
        required_elements = [
            "Cognitive Fusion Reactor",
            "Master Control Dashboard",
            "AtomSpace Status",
            "ECAN Attention",
            "Neural-Symbolic",
            "Phase Implementation Matrix",
            "5D Cognitive Tensor Architecture",
            "System Event Log"
        ]

        for element in required_elements:
            if element not in content:
                print(f"❌ Missing required element: {element}")
                return False

        print(f"✅ Dashboard HTML file valid ({len(content)} bytes)")
        return True
    except Exception as e:
        print(f"❌ HTML file test failed: {e}")
        return False

def test_server_script():
    """Test that server script exists and is valid Python"""
    print("\nTesting server script...")
    try:
        script_path = "fusion-reactor-server.py"
        if not os.path.exists(script_path):
            print(f"❌ Server script not found: {script_path}")
            return False

        # Try to compile the script
        with open(script_path, 'r') as f:
            code = f.read()

        compile(code, script_path, 'exec')

        print(f"✅ Server script valid ({len(code)} bytes)")
        return True
    except SyntaxError as e:
        print(f"❌ Server script syntax error: {e}")
        return False
    except Exception as e:
        print(f"❌ Server script test failed: {e}")
        return False

def test_documentation():
    """Test that documentation exists"""
    print("\nTesting documentation...")
    try:
        docs = [
            "MASTER_CONTROL_DASHBOARD.md",
            "../FUSION_REACTOR_QUICK_START.md"
        ]

        for doc in docs:
            if not os.path.exists(doc):
                print(f"❌ Documentation not found: {doc}")
                return False

            with open(doc, 'r') as f:
                content = f.read()

            print(f"✅ Found {doc} ({len(content)} bytes)")

        print("✅ All documentation files present")
        return True
    except Exception as e:
        print(f"❌ Documentation test failed: {e}")
        return False

def test_startup_script():
    """Test that startup script exists and is executable"""
    print("\nTesting startup script...")
    try:
        script_path = "start-dashboard.sh"
        if not os.path.exists(script_path):
            print(f"❌ Startup script not found: {script_path}")
            return False

        # Check if executable
        is_executable = os.access(script_path, os.X_OK)
        if not is_executable:
            print(f"❌ Startup script is not executable")
            return False

        with open(script_path, 'r') as f:
            content = f.read()

        print(f"✅ Startup script valid and executable ({len(content)} bytes)")
        return True
    except Exception as e:
        print(f"❌ Startup script test failed: {e}")
        return False

def main():
    """Run all tests"""
    print("=" * 80)
    print("🧬 Cognitive Fusion Reactor Dashboard - Test Suite")
    print("=" * 80)

    tests = [
        ("Imports", test_imports),
        ("ReactorStatus", test_reactor_status),
        ("HTML File", test_html_file),
        ("Server Script", test_server_script),
        ("Documentation", test_documentation),
        ("Startup Script", test_startup_script)
    ]

    results = []
    for name, test_func in tests:
        try:
            result = test_func()
            results.append((name, result))
        except Exception as e:
            print(f"❌ Test {name} crashed: {e}")
            results.append((name, False))

    # Summary
    print("\n" + "=" * 80)
    print("Test Summary")
    print("=" * 80)

    passed = sum(1 for _, result in results if result)
    total = len(results)

    for name, result in results:
        status = "✅ PASSED" if result else "❌ FAILED"
        print(f"{status} - {name}")

    print("=" * 80)
    print(f"Results: {passed}/{total} tests passed")

    if passed == total:
        print("🎉 All tests passed!")
        return 0
    else:
        print(f"⚠️  {total - passed} test(s) failed")
        return 1

if __name__ == '__main__':
    sys.exit(main())
