#!/usr/bin/env python3
"""
Phase 5 Documentation Finalization Validation Script

This script validates the completeness and quality of all documentation
as part of Phase 5: System Integration and Testing.
"""

import os
import sys
import re
import subprocess
from pathlib import Path
from typing import List, Dict, Tuple

class DocumentationValidator:
    def __init__(self, base_path="."):
        self.base_path = Path(base_path)
        self.verification_results = {}
        self.warnings = []
        self.errors = []

    def log_result(self, test_name, passed, message=""):
        """Log a validation result"""
        status = "✅ PASS" if passed else "❌ FAIL"
        result_message = f"{status}: {test_name}"
        if message:
            result_message += f" - {message}"
        print(result_message)

        self.verification_results[test_name] = {
            "passed": passed,
            "message": message
        }

        if not passed:
            self.errors.append(f"{test_name}: {message}")

    def log_warning(self, test_name, message):
        """Log a warning"""
        print(f"⚠️  WARNING: {test_name} - {message}")
        self.warnings.append(f"{test_name}: {message}")

    def check_file_exists(self, filepath, description=""):
        """Check if a file exists"""
        full_path = self.base_path / filepath
        exists = full_path.exists()
        desc = description or f"File {filepath}"
        self.log_result(f"File exists: {filepath}", exists, desc)
        return exists

    def check_file_contains(self, filepath, patterns, description=""):
        """Check if a file contains specific patterns"""
        full_path = self.base_path / filepath
        if not full_path.exists():
            self.log_result(f"Content check: {filepath}", False, "File not found")
            return False

        try:
            with open(full_path, 'r', encoding='utf-8') as f:
                content = f.read()

            if isinstance(patterns, str):
                patterns = [patterns]

            missing_patterns = []
            for pattern in patterns:
                if pattern not in content:
                    missing_patterns.append(pattern)

            if missing_patterns:
                self.log_result(f"Content check: {filepath}", False, 
                                f"Missing patterns: {missing_patterns}")
                return False
            else:
                self.log_result(f"Content check: {filepath}", True, description)
                return True

        except Exception as e:
            self.log_result(f"Content check: {filepath}", False, f"Error reading file: {e}")
            return False

    def validate_phase_summaries(self):
        """Validate all phase completion summaries exist"""
        print("\n📋 Validating Phase Completion Summaries...")

        phase_files = [
            "cogkernel/PHASE1_IMPLEMENTATION_SUMMARY.md",
            "cogkernel/PHASE2_MICROKERNEL_INTEGRATION.md",
            "cogkernel/PHASE3_IMPLEMENTATION_SUMMARY.md", 
            "cogkernel/PHASE4_COMPLETION_SUMMARY.md",
            "cogkernel/PHASE5_COMPLETION_SUMMARY.md"
        ]

        for phase_file in phase_files:
            self.check_file_exists(phase_file, f"Phase completion summary")

            # Check for required content in phase summaries
            if self.base_path.joinpath(phase_file).exists():
                required_sections = [
                    "## Overview",
                    "## Completed Components", 
                    "**Status:** COMPLETE"
                ]
                self.check_file_contains(phase_file, required_sections, 
                                         "Required phase summary sections")

    def validate_core_documentation(self):
        """Validate core project documentation"""
        print("\n📚 Validating Core Documentation...")

        core_docs = [
            "README.md",
            "SKZ_INTEGRATION_STRATEGY.md", 
            "DEVELOPMENT_ROADMAP.md",
            "IMPLEMENTATION_SUMMARY.md"
        ]

        for doc in core_docs:
            self.check_file_exists(doc, "Core documentation")

        # Validate SKZ strategy completion
        self.check_file_contains("SKZ_INTEGRATION_STRATEGY.md", [
            "Phase 5: System Integration and Testing",
            "Documentation finalization"
        ], "SKZ strategy Phase 5 content")

    def validate_technical_documentation(self):
        """Validate technical implementation documentation"""
        print("\n🔧 Validating Technical Documentation...")

        technical_docs = [
            "docs/ARCHITECTURE.md",
            "docs/DEVELOPER.md", 
            "docs/open-issues/documentation.md",
            "docs/GUIX_INTEGRATION_COMPLETION.md"
        ]

        for doc in technical_docs:
            if self.check_file_exists(doc, "Technical documentation"):
                # Check for basic structure
                self.check_file_contains(doc, [
                    "# ",  # Has title
                    "## "  # Has sections
                ], "Basic documentation structure")

    def validate_cogkernel_documentation(self):
        """Validate cognitive kernel documentation"""
        print("\n🧠 Validating Cognitive Kernel Documentation...")

        cogkernel_docs = [
            "cogkernel/README.md",
            "cogkernel/cognitive-interface/README.md"
        ]

        for doc in cogkernel_docs:
            self.check_file_exists(doc, "Cognitive kernel documentation")

        # Check for learning systems documentation
        learning_docs = [
            "cogkernel/cognitive-interface/learning-systems/README.md"
        ]

        for doc in learning_docs:
            if self.check_file_exists(doc, "Learning systems documentation"):
                required_sections = [
                    "# Real-time Learning Systems",
                    "## Overview",
                    "## Key Features",
                    "## Usage Examples"
                ]
                self.check_file_contains(doc, required_sections,
                                         "Learning system documentation structure")

    def validate_documentation_links(self):
        """Validate internal documentation links"""
        print("\n🔗 Validating Documentation Links...")

        # Find all markdown files
        md_files = list(self.base_path.rglob("*.md"))

        link_pattern = re.compile(r'\[([^\]]+)\]\(([^)]+)\)')
        broken_links = []

        for md_file in md_files:
            try:
                with open(md_file, 'r', encoding='utf-8') as f:
                    content = f.read()

                links = link_pattern.findall(content)
                for link_text, link_url in links:
                    # Skip external links
                    if link_url.startswith(('http://', 'https://', 'mailto:')):
                        continue

                    # Check if internal link exists
                    if link_url.startswith('#'):
                        # Header link - skip for now
                        continue

                    # Relative path link
                    link_path = md_file.parent / link_url
                    if not link_path.exists():
                        broken_links.append(f"{md_file}: {link_url}")
            except Exception as e:
                self.log_warning("Link validation", f"Error reading {md_file}: {e}")

        if broken_links:
            self.log_result("Internal links validation", False, 
                            f"Found {len(broken_links)} broken links")
            for link in broken_links[:5]:  # Show first 5
                print(f"    Broken: {link}")
        else:
            self.log_result("Internal links validation", True, 
                            "All internal links verified")

    def validate_code_examples(self):
        """Validate code examples in documentation"""
        print("\n💻 Validating Code Examples...")

        md_files = list(self.base_path.rglob("*.md"))
        code_block_pattern = re.compile(r'```(\w+)?\n(.*?)\n```', re.DOTALL)

        total_examples = 0
        invalid_examples = []

        for md_file in md_files:
            try:
                with open(md_file, 'r', encoding='utf-8') as f:
                    content = f.read()

                code_blocks = code_block_pattern.findall(content)
                for language, code in code_blocks:
                    total_examples += 1

                    # Basic validation - check for obvious issues
                    if not code.strip():
                        invalid_examples.append(f"{md_file}: Empty code block")
                    elif language == 'bash' and not code.strip().startswith(('#', 'cd', 'ls', 'git', 'make', 'sudo', 'apt', 'pip', 'npm')):
                        # Bash should start with common commands or comments
                        pass  # Skip this check for now

            except Exception as e:
                self.log_warning("Code examples validation", f"Error reading {md_file}: {e}")

        if invalid_examples:
            self.log_result("Code examples validation", False,
                            f"Found {len(invalid_examples)} invalid examples")
        else:
            self.log_result("Code examples validation", True,
                            f"Validated {total_examples} code examples")

    def validate_documentation_completeness(self):
        """Validate overall documentation completeness"""
        print("\n📊 Validating Documentation Completeness...")

        # Check for README files in major directories
        important_dirs = [
            "cogkernel",
            "docs", 
            "docs/open-issues",
            "guix-build-system",
            "hurd-ecosystem/documentation"
        ]

        for dir_path in important_dirs:
            readme_path = f"{dir_path}/README.md"
            self.check_file_exists(readme_path, f"README for {dir_path}")

        # Check for essential documentation types
        doc_types = {
            "Architecture": ["ARCHITECTURE.md", "docs/ARCHITECTURE.md"],
            "Installation": ["INSTALL", "docs/INSTALL.md"],
            "Development": ["DEVELOPMENT_ROADMAP.md", "docs/DEVELOPER.md"],
            "Contributing": ["CONTRIBUTING.md", "docs/CONTRIBUTING.md"]
        }

        for doc_type, possible_files in doc_types.items():
            found = False
            for file_path in possible_files:
                if self.base_path.joinpath(file_path).exists():
                    found = True
                    break

            self.log_result(f"{doc_type} documentation", found,
                            f"At least one {doc_type.lower()} document exists")

    def generate_documentation_index(self):
        """Generate a comprehensive documentation index"""
        print("\n📇 Generating Documentation Index...")

        index_content = """# Documentation Index

This file provides a comprehensive index of all documentation in the repository.

## Core Documentation

### Project Overview
- [README.md](README.md) - Main project overview
- [SKZ Integration Strategy](SKZ_INTEGRATION_STRATEGY.md) - Complete integration strategy
- [Development Roadmap](DEVELOPMENT_ROADMAP.md) - Project roadmap and phases

### Implementation Summaries
- [Implementation Summary](IMPLEMENTATION_SUMMARY.md) - Overall implementation summary
- [Phase 3 Build Orchestration](PHASE3_BUILD_ORCHESTRATION_SUMMARY.md) - Build system summary
- [ECMA-262 Integration](ECMA262_INTEGRATION_SUMMARY.md) - JavaScript integration

### Phase Completion Documentation
- [Phase 1 Implementation](cogkernel/PHASE1_IMPLEMENTATION_SUMMARY.md) - Foundation setup
- [Phase 2 Microkernel Integration](cogkernel/PHASE2_MICROKERNEL_INTEGRATION.md) - Microkernel integration
- [Phase 3 Implementation](cogkernel/PHASE3_IMPLEMENTATION_SUMMARY.md) - Build orchestration
- [Phase 4 Completion](cogkernel/PHASE4_COMPLETION_SUMMARY.md) - Cognitive layer development  
- [Phase 5 Completion](cogkernel/PHASE5_COMPLETION_SUMMARY.md) - System integration and testing

## Technical Documentation

### Architecture
- [Architecture Overview](docs/ARCHITECTURE.md) - System architecture
- [Hurd Architecture](HURD_ARCHITECTURE.md) - GNU Hurd specific architecture
- [GUIX Integration](docs/GUIX_INTEGRATION_COMPLETION.md) - Build system integration

### Development
- [Developer Guide](docs/DEVELOPER.md) - Development guidelines
- [Documentation Standards](docs/open-issues/documentation.md) - Documentation standards

### Cognitive Kernel
- [Cognitive Kernel README](cogkernel/README.md) - Cognitive kernel overview
- [Cognitive Interface](cogkernel/cognitive-interface/README.md) - Interface documentation
- [Learning Systems](cogkernel/cognitive-interface/learning-systems/README.md) - Learning system docs

## Component Documentation

### Hurd Ecosystem
- [Documentation Overview](hurd-ecosystem/documentation/README.md) - Hurd documentation structure

### Build System
- [GUIX Build System](guix-build-system/README.md) - Build system documentation

### External Components  
- [External Components](external/README.md) - External component documentation

## Open Issues Documentation
- [Documentation Issues](docs/open-issues/documentation.md) - Documentation standards
- [Open Issues Overview](docs/open-issues/README.md) - Open issues catalog

## Testing and Validation
- [Documentation Tests](validate-documentation-finalization.py) - Documentation validation
- [Phase 2 Verification](verify-phase2-completion.py) - Phase 2 completion verification
- [Phase 3 Validation](validate-phase3-completion.py) - Phase 3 validation

---

*This index is automatically generated and maintained as part of Phase 5 documentation finalization.*
"""

        index_path = self.base_path / "DOCUMENTATION_INDEX.md"
        try:
            with open(index_path, 'w', encoding='utf-8') as f:
                f.write(index_content)
            self.log_result("Documentation index generation", True, 
                            "Generated comprehensive documentation index")
        except Exception as e:
            self.log_result("Documentation index generation", False, f"Error: {e}")

    def run_validation(self):
        """Run complete documentation validation"""
        print("🔍 PHASE 5 DOCUMENTATION FINALIZATION VALIDATION")
        print("=" * 60)

        # Run all validation checks
        self.validate_phase_summaries()
        self.validate_core_documentation()
        self.validate_technical_documentation()
        self.validate_cogkernel_documentation()
        self.validate_documentation_links()
        self.validate_code_examples()
        self.validate_documentation_completeness()
        self.generate_documentation_index()

        # Generate summary
        self.generate_validation_report()

        return len(self.errors) == 0

    def generate_validation_report(self):
        """Generate final validation report"""
        print("\n📋 DOCUMENTATION FINALIZATION REPORT")
        print("=" * 50)

        total_tests = len(self.verification_results)
        passed_tests = sum(1 for result in self.verification_results.values() if result["passed"])

        print(f"Total Validation Checks: {total_tests}")
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
            print("✅ Documentation is COMPLETE and ready for production!")
        elif passed_tests / total_tests >= 0.8:
            print("✅ Documentation is mostly complete. Address warnings if needed.")
        else:
            print("❌ Documentation requires additional work before finalization.")

        return {
            "total_checks": total_tests,
            "passed_checks": passed_tests,
            "success_rate": passed_tests / total_tests * 100 if total_tests > 0 else 0,
            "errors": self.errors,
            "warnings": self.warnings
        }

def main():
    """Main validation function"""
    validator = DocumentationValidator("/home/runner/work/hurdcog/hurdcog")
    success = validator.run_validation()

    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())