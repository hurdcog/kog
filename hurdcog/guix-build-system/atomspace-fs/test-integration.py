#!/usr/bin/env python3
"""
Atomspace Filesystem Operations Integration Test
Part of Phase 3: Build System Orchestration
Tests the complete atomspace filesystem implementation
"""

import os
import sys
import subprocess
import tempfile
import json
from pathlib import Path

class AtomspaceFilesystemTester:
    def __init__(self):
        self.test_dir = Path(__file__).parent
        self.build_dir = self.test_dir / "../../build/atomspace-fs"
        self.passed_tests = 0
        self.total_tests = 0

    def log(self, message, level="INFO"):
        print(f"[{level}] {message}")

    def run_test(self, test_name, test_func):
        """Run a single test and track results"""
        self.total_tests += 1
        self.log(f"Running test: {test_name}")

        try:
            result = test_func()
            if result:
                self.log(f"✓ {test_name} passed", "PASS")
                self.passed_tests += 1
                return True
            else:
                self.log(f"✗ {test_name} failed", "FAIL")
                return False
        except Exception as e:
            self.log(f"✗ {test_name} failed with exception: {e}", "FAIL")
            return False

    def test_c_bindings_compilation(self):
        """Test C bindings compilation"""
        try:
            result = subprocess.run([
                "gcc", "-D_GNU_SOURCE", "-DTEST_MAIN", "-Wall", "-Wextra", "-std=c99",
                "-o", "test-c-bindings", "atomspace-fs-minimal.c"
            ], cwd=self.test_dir, capture_output=True, text=True)

            if result.returncode == 0:
                self.log("C bindings compiled successfully")
                return True
            else:
                self.log(f"C compilation failed: {result.stderr}")
                return False
        except Exception as e:
            self.log(f"C compilation error: {e}")
            return False

    def test_c_bindings_execution(self):
        """Test C bindings execution"""
        try:
            result = subprocess.run(
                ["./test-c-bindings"], 
                cwd=self.test_dir, 
                capture_output=True, 
                text=True
            )

            if result.returncode == 0 and "All C binding tests passed!" in result.stdout:
                self.log("C bindings execution successful")
                return True
            else:
                self.log(f"C execution failed: {result.stderr}")
                return False
        except Exception as e:
            self.log(f"C execution error: {e}")
            return False

    def test_makefile_configuration(self):
        """Test Makefile configuration"""
        try:
            result = subprocess.run(
                ["make", "config"], 
                cwd=self.test_dir, 
                capture_output=True, 
                text=True
            )

            if result.returncode == 0 and "Atomspace Filesystem Build Configuration" in result.stdout:
                self.log("Makefile configuration working")
                return True
            else:
                self.log(f"Makefile config failed: {result.stderr}")
                return False
        except Exception as e:
            self.log(f"Makefile error: {e}")
            return False

    def test_scheme_modules_structure(self):
        """Test Scheme modules structure"""
        required_files = [
            "partition.scm",
            "implementation.scm", 
            "test.scm"
        ]

        for file in required_files:
            file_path = self.test_dir / file
            if not file_path.exists():
                self.log(f"Missing Scheme module: {file}")
                return False

        self.log("All required Scheme modules present")
        return True

    def test_scheme_module_syntax(self):
        """Test Scheme module syntax (basic validation)"""
        scheme_files = [
            "partition.scm",
            "implementation.scm",
            "test.scm"
        ]

        for file in scheme_files:
            file_path = self.test_dir / file
            try:
                with open(file_path, 'r') as f:
                    content = f.read()

                # Basic syntax checks
                if not content.strip():
                    self.log(f"Empty Scheme file: {file}")
                    return False

                # Check for balanced parentheses
                open_parens = content.count('(')
                close_parens = content.count(')')
                if open_parens != close_parens:
                    self.log(f"Unbalanced parentheses in {file}")
                    return False

                # Check for define-module declaration
                if "define-module" not in content:
                    self.log(f"Missing define-module in {file}")
                    return False

            except Exception as e:
                self.log(f"Error reading {file}: {e}")
                return False

        self.log("Scheme module syntax validation passed")
        return True

    def test_atomspace_filesystem_api(self):
        """Test atomspace filesystem API structure"""
        api_file = self.test_dir / "partition.scm"

        try:
            with open(api_file, 'r') as f:
                content = f.read()

            required_exports = [
                "make-atomspace-filesystem",
                "mount-atomspace-fs",
                "atomspace-filesystem?",
                "atomspace-fs-read",
                "atomspace-fs-write",
                "atomspace-fs-mkdir",
                "atomspace-fs-list",
                "atomspace-fs-stat"
            ]

            for export in required_exports:
                if export not in content:
                    self.log(f"Missing API function: {export}")
                    return False

            self.log("Atomspace filesystem API structure validated")
            return True

        except Exception as e:
            self.log(f"Error validating API: {e}")
            return False

    def test_cognitive_operations_interface(self):
        """Test cognitive operations interface"""
        impl_file = self.test_dir / "implementation.scm"

        try:
            with open(impl_file, 'r') as f:
                content = f.read()

            cognitive_features = [
                "atomspace-fs-query",
                "atomspace-fs-cognitive-operation",
                "atomspace-fs-parallel-op",
                "atomspace-fs-replicate",
                "atomspace-fs-namespace-bind"
            ]

            for feature in cognitive_features:
                if feature not in content:
                    self.log(f"Missing cognitive feature: {feature}")
                    return False

            self.log("Cognitive operations interface validated")
            return True

        except Exception as e:
            self.log(f"Error validating cognitive operations: {e}")
            return False

    def test_error_handling(self):
        """Test error handling implementation"""
        c_file = self.test_dir / "atomspace-fs-minimal.c"

        try:
            with open(c_file, 'r') as f:
                content = f.read()

            error_handling_patterns = [
                "if (!filesystem)",
                "if (!path)",
                "return EINVAL",
                "return ENOMEM",
                "atomspace_fs_strerror"
            ]

            for pattern in error_handling_patterns:
                if pattern not in content:
                    self.log(f"Missing error handling pattern: {pattern}")
                    return False

            self.log("Error handling implementation validated")
            return True

        except Exception as e:
            self.log(f"Error validating error handling: {e}")
            return False

    def test_performance_features(self):
        """Test performance and optimization features"""
        files_to_check = [
            ("implementation.scm", ["parallel-op", "performance-stats", "tensor-shape"]),
            ("atomspace-fs-minimal.c", ["cognitive_op", "get_stats", "verify_integration"])
        ]

        for file, features in files_to_check:
            file_path = self.test_dir / file
            try:
                with open(file_path, 'r') as f:
                    content = f.read()

                for feature in features:
                    if feature not in content:
                        self.log(f"Missing performance feature '{feature}' in {file}")
                        return False

            except Exception as e:
                self.log(f"Error checking performance features in {file}: {e}")
                return False

        self.log("Performance features validated")
        return True

    def test_integration_documentation(self):
        """Test integration documentation"""
        readme_file = self.test_dir / "README.md"

        if not readme_file.exists():
            self.log("Missing README.md documentation")
            return False

        try:
            with open(readme_file, 'r') as f:
                content = f.read()

            required_sections = [
                "## Overview",
                "## Features", 
                "## Architecture",
                "## Usage",
                "## Building",
                "## Integration"
            ]

            for section in required_sections:
                if section not in content:
                    self.log(f"Missing documentation section: {section}")
                    return False

            self.log("Integration documentation validated")
            return True

        except Exception as e:
            self.log(f"Error validating documentation: {e}")
            return False

    def test_skz_framework_compliance(self):
        """Test compliance with SKZ framework patterns"""
        # Check for SKZ integration patterns
        patterns_to_check = [
            ("implementation.scm", ["SKZ", "cognitive", "distributed"]),
            ("partition.scm", ["distributed-storage", "parallel-computing", "plan9-inferno"]),
            ("README.md", ["SKZ Integration", "Phase 3", "cognitive"])
        ]

        for file, patterns in patterns_to_check:
            file_path = self.test_dir / file
            try:
                with open(file_path, 'r') as f:
                    content = f.read().lower()

                for pattern in patterns:
                    if pattern.lower() not in content:
                        self.log(f"Missing SKZ pattern '{pattern}' in {file}")
                        return False

            except Exception as e:
                self.log(f"Error checking SKZ compliance in {file}: {e}")
                return False

        self.log("SKZ framework compliance validated")
        return True

    def run_all_tests(self):
        """Run all tests and report results"""
        self.log("Starting Atomspace Filesystem Operations Integration Test")
        self.log("=" * 60)

        tests = [
            ("C Bindings Compilation", self.test_c_bindings_compilation),
            ("C Bindings Execution", self.test_c_bindings_execution),
            ("Makefile Configuration", self.test_makefile_configuration),
            ("Scheme Modules Structure", self.test_scheme_modules_structure),
            ("Scheme Module Syntax", self.test_scheme_module_syntax),
            ("Atomspace Filesystem API", self.test_atomspace_filesystem_api),
            ("Cognitive Operations Interface", self.test_cognitive_operations_interface),
            ("Error Handling", self.test_error_handling),
            ("Performance Features", self.test_performance_features),
            ("Integration Documentation", self.test_integration_documentation),
            ("SKZ Framework Compliance", self.test_skz_framework_compliance),
        ]

        for test_name, test_func in tests:
            self.run_test(test_name, test_func)

        self.log("=" * 60)
        self.log(f"Test Results: {self.passed_tests}/{self.total_tests} tests passed")

        if self.passed_tests == self.total_tests:
            self.log("🎉 All atomspace filesystem tests passed!", "SUCCESS")
            return True
        else:
            self.log(f"❌ {self.total_tests - self.passed_tests} tests failed", "FAILURE")
            return False

def main():
    """Main test runner"""
    tester = AtomspaceFilesystemTester()
    success = tester.run_all_tests()

    # Create test report
    report = {
        "test_name": "Atomspace Filesystem Operations Integration Test",
        "total_tests": tester.total_tests,
        "passed_tests": tester.passed_tests,
        "success": success,
        "timestamp": subprocess.check_output(["date"], text=True).strip()
    }

    # Save test report
    report_file = Path(__file__).parent / "test-report.json"
    with open(report_file, 'w') as f:
        json.dump(report, f, indent=2)

    print(f"\nTest report saved to: {report_file}")

    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())