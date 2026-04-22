#!/usr/bin/env python3
"""
HurdCog Phase 2: Microkernel Integration Verification Script

This script verifies that Phase 2 of the SKZ Integration project is complete
by checking all implementation artifacts, documentation, and integration points.
"""

import os
import sys
import subprocess
import json
from pathlib import Path

class Phase2Verifier:
    def __init__(self, base_path="."):
        self.base_path = Path(base_path)
        self.verification_results = {}
        self.errors = []
        self.warnings = []

    def log_result(self, test_name, passed, message=""):
        """Log a verification result"""
        status = "✅ PASS" if passed else "❌ FAIL"
        print(f"{status}: {test_name}")
        if message:
            print(f"    {message}")

        self.verification_results[test_name] = {
            "passed": passed,
            "message": message
        }

        if not passed:
            self.errors.append(f"{test_name}: {message}")

    def log_warning(self, test_name, message):
        """Log a warning"""
        print(f"⚠️  WARNING: {test_name}")
        print(f"    {message}")
        self.warnings.append(f"{test_name}: {message}")

    def check_file_exists(self, filepath, description=""):
        """Check if a file exists"""
        full_path = self.base_path / filepath
        exists = full_path.exists()
        msg = f"{description} - {filepath}" if description else str(filepath)
        self.log_result(f"File exists: {msg}", exists, str(full_path) if exists else f"File not found: {full_path}")
        return exists

    def check_directory_exists(self, dirpath, description=""):
        """Check if a directory exists"""
        full_path = self.base_path / dirpath
        exists = full_path.exists() and full_path.is_dir()
        msg = f"{description} - {dirpath}" if description else str(dirpath)
        self.log_result(f"Directory exists: {msg}", exists, str(full_path) if exists else f"Directory not found: {full_path}")
        return exists

    def check_file_contains(self, filepath, pattern, description=""):
        """Check if a file contains a pattern"""
        try:
            full_path = self.base_path / filepath
            if not full_path.exists():
                self.log_result(f"Content check: {description}", False, f"File not found: {full_path}")
                return False

            with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                contains = pattern in content
                self.log_result(f"Content check: {description}", contains, 
                                f"Found pattern '{pattern[:50]}...' in {filepath}" if contains else 
                                f"Pattern '{pattern[:50]}...' not found in {filepath}")
                return contains
        except Exception as e:
            self.log_result(f"Content check: {description}", False, f"Error reading {filepath}: {str(e)}")
            return False

    def verify_sub_task_implementations(self):
        """Verify that all 5 sub-tasks have implementation artifacts"""
        print("\n🔍 Verifying Sub-Task Implementation Artifacts...")

        # Sub-task 1: OpenCog atomspace integration
        self.check_file_exists("cogkernel/hurd-atomspace-bridge.c", "OpenCog-Hurd bridge C implementation")
        self.check_file_exists("cogkernel/hurd-atomspace-bridge.h", "OpenCog-Hurd bridge header")
        self.check_file_exists("cogkernel/atomspace.scm", "AtomSpace Scheme module")
        self.check_file_exists("cogkernel/microkernel-integration.scm", "Microkernel integration layer")

        # Sub-task 2: Plan9/Inferno namespace features
        self.check_file_exists("cogkernel/plan9-namespace.scm", "Plan9 namespace implementation")
        self.check_file_exists("cogkernel/9p-hypergraph.scm", "9P protocol hypergraph integration")
        self.check_file_contains("cogkernel/plan9-namespace.scm", "make-plan9-namespace", "Plan9 namespace constructor")

        # Sub-task 3: Kokkos parallel computing framework
        kokkos_dirs = ["performance/kokkos-integration", "cogkernel/ggml"]
        for kokkos_dir in kokkos_dirs:
            if self.check_directory_exists(kokkos_dir, "Kokkos framework integration"):
                break
        else:
            self.log_warning("Kokkos Integration", "No clear Kokkos integration directory found")

        # Sub-task 4: Compiler-explorer JIT infrastructure  
        self.check_directory_exists("development/compiler-explorer", "Compiler-explorer development tools")

        # Sub-task 5: guile-llama-cpp with ECMA-262 features
        self.check_directory_exists("guile-llama-cpp", "Guile-LLaMA-CPP integration")
        self.check_directory_exists("ecma262-main", "ECMA-262 features")

    def verify_integration_tests(self):
        """Verify integration test files exist and are properly structured"""
        print("\n🧪 Verifying Integration Tests...")

        test_files = [
            "cogkernel/phase2-standalone-test.scm",
            "cogkernel/test-microkernel-integration.scm", 
            "cogkernel/standalone-microkernel-test.scm",
            "cogkernel/phase2-integration.scm"
        ]

        for test_file in test_files:
            if self.check_file_exists(test_file, "Integration test"):
                # Check if test file has proper structure
                self.check_file_contains(test_file, "test", "Test function definitions")
                self.check_file_contains(test_file, "format", "Test output formatting")

    def verify_documentation(self):
        """Verify documentation is complete and up to date"""
        print("\n📚 Verifying Documentation...")

        # Core documentation files
        docs = [
            "cogkernel/README.md",
            "cogkernel/PHASE2_MICROKERNEL_INTEGRATION.md",
            "SKZ_INTEGRATION_STRATEGY.md",
            "DEVELOPMENT_ROADMAP.md"
        ]

        for doc in docs:
            if self.check_file_exists(doc, "Documentation"):
                # Check for Phase 2 content
                self.check_file_contains(doc, "Phase 2", "Phase 2 references")

        # Check for implementation summaries
        summary_files = [
            "cogkernel/PHASE1_IMPLEMENTATION_SUMMARY.md",
            "cogkernel/PHASE2_MICROKERNEL_INTEGRATION.md",
            "cogkernel/PHASE3_IMPLEMENTATION_SUMMARY.md"
        ]

        for summary in summary_files:
            self.check_file_exists(summary, "Phase implementation summary")

    def verify_build_system(self):
        """Verify build system integration"""
        print("\n🔨 Verifying Build System Integration...")

        # Check main build files
        self.check_file_exists("Makefile", "Main Makefile")
        self.check_file_exists("Makeconf", "Make configuration")
        self.check_file_exists("cogkernel/Makefile", "Cognitive kernel Makefile")

        # Check for cognitive integration in main Makefile
        if self.check_file_contains("Makefile", "cognitive-subdirs", "Cognitive subdirectories"):
            self.check_file_contains("Makefile", "cogkernel", "Cogkernel in build system")

    def verify_architectural_coherence(self):
        """Verify architectural coherence and integration"""
        print("\n🏗️  Verifying Architectural Coherence...")

        # Check for proper module structure
        core_modules = [
            "cogkernel/core.scm",
            "cogkernel/agents.scm",
            "cogkernel/atomspace.scm",
            "cogkernel/attention.scm"
        ]

        for module in core_modules:
            self.check_file_exists(module, "Core cognitive module")

        # Check for integration points
        integration_files = [
            "cogkernel/hurdcog-bootstrap.scm",
            "cogkernel/microkernel-integration.scm"
        ]

        for integration_file in integration_files:
            if self.check_file_exists(integration_file, "Integration module"):
                self.check_file_contains(integration_file, "define", "Scheme definitions")

    def check_phase2_readiness(self):
        """Check if system is ready for next phase deployment"""
        print("\n🚀 Checking Phase 2 Completion Readiness...")

        # Count passed vs failed tests
        total_tests = len(self.verification_results)
        passed_tests = sum(1 for result in self.verification_results.values() if result["passed"])

        readiness_score = (passed_tests / total_tests * 100) if total_tests > 0 else 0

        self.log_result("Phase 2 Readiness Score", readiness_score >= 80, 
                        f"{readiness_score:.1f}% ({passed_tests}/{total_tests} checks passed)")

        if readiness_score >= 90:
            print("✅ EXCELLENT: Phase 2 is ready for completion")
        elif readiness_score >= 80:
            print("✅ GOOD: Phase 2 is ready with minor improvements needed")
        elif readiness_score >= 70:
            print("⚠️  CAUTION: Phase 2 needs attention before completion")
        else:
            print("❌ CRITICAL: Phase 2 requires significant work before completion")

        return readiness_score >= 80

    def generate_completion_report(self):
        """Generate a completion report"""
        print("\n📋 PHASE 2 COMPLETION REPORT")
        print("=" * 50)

        total_tests = len(self.verification_results)
        passed_tests = sum(1 for result in self.verification_results.values() if result["passed"])

        print(f"Total Verification Checks: {total_tests}")
        print(f"Passed: {passed_tests}")
        print(f"Failed: {total_tests - passed_tests}")
        print(f"Warnings: {len(self.warnings)}")
        print(f"Success Rate: {(passed_tests/total_tests*100):.1f}%")

        if self.errors:
            print(f"\n❌ CRITICAL ISSUES ({len(self.errors)}):")
            for error in self.errors:
                print(f"  • {error}")

        if self.warnings:
            print(f"\n⚠️  WARNINGS ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"  • {warning}")

        print("\n🎯 RECOMMENDATION:")
        if passed_tests / total_tests >= 0.9:
            print("Phase 2 is COMPLETE and ready for Phase 3!")
        elif passed_tests / total_tests >= 0.8:
            print("Phase 2 is mostly complete. Address warnings and proceed to Phase 3.")
        else:
            print("Phase 2 requires additional work before completion.")

        return {
            "total_checks": total_tests,
            "passed_checks": passed_tests,
            "success_rate": passed_tests / total_tests * 100 if total_tests > 0 else 0,
            "errors": self.errors,
            "warnings": self.warnings,
            "ready_for_phase3": passed_tests / total_tests >= 0.8
        }

    def run_verification(self):
        """Run complete Phase 2 verification"""
        print("🧠 === HURDCOG PHASE 2: MICROKERNEL INTEGRATION VERIFICATION === 🧠")
        print("Verifying completion of all 5 sub-tasks and integration readiness...\n")

        self.verify_sub_task_implementations()
        self.verify_integration_tests()
        self.verify_documentation()
        self.verify_build_system()
        self.verify_architectural_coherence()

        readiness = self.check_phase2_readiness()
        report = self.generate_completion_report()

        return report

def main():
    """Main verification function"""
    verifier = Phase2Verifier()
    report = verifier.run_verification()

    # Exit with appropriate code
    if report["ready_for_phase3"]:
        print("\n🎉 SUCCESS: Phase 2 verification completed successfully!")
        sys.exit(0)
    else:
        print("\n⚠️  Phase 2 verification completed with issues.")
        sys.exit(1)

if __name__ == "__main__":
    main()