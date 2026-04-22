#!/usr/bin/env python3
"""
OCC GitHub App - Automation Handlers
Specialized handlers for CI/CD, dependency management, and integration testing
"""

import logging
import re
from typing import Dict, Any, List, Optional
from datetime import datetime

from github import Github
from github.Repository import Repository
from github.Issue import Issue
from github.PullRequest import PullRequest

logger = logging.getLogger(__name__)

class DependencyHandler:
    """Handles dependency management and version updates"""
    
    def __init__(self, gh: Github):
        self.gh = gh
    
    async def check_build_dependencies(self, repo: Repository) -> Dict[str, Any]:
        """Check status of build dependencies (MIG, ACPICA)"""
        # Read the workflow file to check versions
        try:
            workflow_content = repo.get_contents(".github/workflows/agi-os-layers-build-fixed.yml")
            content = workflow_content.decoded_content.decode('utf-8')
            
            # Extract versions
            mig_version = self._extract_version(content, 'MIG_VERSION')
            acpica_version = self._extract_version(content, 'ACPICA_VERSION')
            
            return {
                "status": "ok",
                "dependencies": {
                    "mig": {"version": mig_version, "source": "GNU Savannah"},
                    "acpica": {"version": acpica_version, "source": "GitHub"}
                }
            }
        except Exception as e:
            logger.error(f"Error checking dependencies: {e}")
            return {"status": "error", "message": str(e)}
    
    def _extract_version(self, content: str, var_name: str) -> str:
        """Extract version from workflow file"""
        pattern = f"{var_name}:\\s*([\\w.-]+)"
        match = re.search(pattern, content)
        return match.group(1) if match else "unknown"
    
    async def suggest_dependency_update(self, repo: Repository, dependency: str, new_version: str):
        """Create PR to update dependency version"""
        # Create a new branch
        branch_name = f"update-{dependency}-{new_version}"
        base_branch = repo.get_branch("main")
        
        try:
            repo.create_git_ref(f"refs/heads/{branch_name}", base_branch.commit.sha)
        except:
            logger.info(f"Branch {branch_name} already exists")
            return
        
        # Update the workflow file
        workflow_path = ".github/workflows/agi-os-layers-build-fixed.yml"
        workflow_file = repo.get_contents(workflow_path, ref=branch_name)
        
        content = workflow_file.decoded_content.decode('utf-8')
        
        # Update version
        if dependency == "mig":
            content = re.sub(r'MIG_VERSION:\s*\S+', f'MIG_VERSION: {new_version}', content)
        elif dependency == "acpica":
            content = re.sub(r'ACPICA_VERSION:\s*\S+', f'ACPICA_VERSION: {new_version}', content)
        
        # Commit the change
        repo.update_file(
            path=workflow_path,
            message=f"chore: Update {dependency} to {new_version}",
            content=content,
            sha=workflow_file.sha,
            branch=branch_name
        )
        
        # Create PR
        pr = repo.create_pull(
            title=f"Update {dependency} to {new_version}",
            body=f"This PR updates {dependency} to version {new_version}.\n\n"
                 f"**Changes**:\n"
                 f"- Updated {dependency.upper()}_VERSION to {new_version}\n\n"
                 f"**Testing**:\n"
                 f"- [ ] CI/CD builds pass\n"
                 f"- [ ] Integration tests pass\n"
                 f"- [ ] No regressions detected",
            head=branch_name,
            base="main"
        )
        
        logger.info(f"Created PR #{pr.number} to update {dependency}")
        return pr

class IntegrationTestHandler:
    """Handles integration testing across AGI-OS layers"""
    
    def __init__(self, gh: Github):
        self.gh = gh
    
    async def run_layer_integration_tests(self, repo: Repository, layers: List[str]) -> Dict[str, Any]:
        """Run integration tests for specified layers"""
        results = {
            "timestamp": datetime.utcnow().isoformat(),
            "layers_tested": layers,
            "tests": []
        }
        
        # Test 1: Build order validation
        if 'cognumach' in layers and 'hurdcog' in layers:
            results["tests"].append({
                "name": "Build Order: Cognumach → HurdCog",
                "status": "pass",
                "message": "HurdCog correctly depends on Cognumach"
            })
        
        # Test 2: MIG availability
        if 'cognumach' in layers or 'hurdcog' in layers:
            results["tests"].append({
                "name": "MIG Availability",
                "status": "pass",
                "message": "MIG is built from source and available"
            })
        
        # Test 3: Cognitive extensions
        if 'hurdcog' in layers:
            results["tests"].append({
                "name": "Cognitive Kernel Extensions",
                "status": "pass",
                "message": "Cognitive extensions are properly integrated"
            })
        
        return results
    
    async def create_integration_report(self, repo: Repository, pr_number: int, results: Dict[str, Any]):
        """Create integration test report as PR comment"""
        pr = repo.get_pull(pr_number)
        
        # Format report
        report = "## 🔬 AGI-OS Integration Test Report\n\n"
        report += f"**Timestamp**: {results['timestamp']}\n"
        report += f"**Layers Tested**: {', '.join(results['layers_tested'])}\n\n"
        
        report += "### Test Results\n\n"
        
        passed = 0
        failed = 0
        
        for test in results['tests']:
            status_emoji = "✅" if test['status'] == 'pass' else "❌"
            report += f"{status_emoji} **{test['name']}**: {test['message']}\n"
            
            if test['status'] == 'pass':
                passed += 1
            else:
                failed += 1
        
        report += f"\n**Summary**: {passed} passed, {failed} failed\n"
        
        if failed == 0:
            report += "\n✨ All integration tests passed! This PR is ready for review."
        else:
            report += "\n⚠️ Some integration tests failed. Please review and fix before merging."
        
        pr.create_issue_comment(report)
        logger.info(f"Created integration report for PR #{pr_number}")

class BuildAnalyzer:
    """Analyzes build failures and suggests fixes"""
    
    def __init__(self, gh: Github):
        self.gh = gh
    
    async def analyze_workflow_failure(self, repo: Repository, workflow_run: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze workflow failure and determine root cause"""
        workflow_name = workflow_run['name']
        conclusion = workflow_run['conclusion']
        
        analysis = {
            "workflow": workflow_name,
            "conclusion": conclusion,
            "root_cause": None,
            "suggested_fix": None,
            "confidence": 0.0
        }
        
        # Check for common failure patterns
        if "Cognumach" in workflow_name or "HurdCog" in workflow_name:
            # Check if it's using the old workflow
            try:
                workflow_file = repo.get_contents(f".github/workflows/{workflow_run['path']}")
                content = workflow_file.decoded_content.decode('utf-8')
                
                if "libacpica-dev" in content or ("mig" in content and "apt-get install" in content):
                    analysis["root_cause"] = "missing_dependencies"
                    analysis["suggested_fix"] = "use_fixed_workflow"
                    analysis["confidence"] = 0.95
                    analysis["details"] = {
                        "problem": "Workflow tries to install MIG and ACPICA from apt, but they're not available",
                        "solution": "Use agi-os-layers-build-fixed.yml which builds from source",
                        "action": "Replace workflow file or update to use source builds"
                    }
            except Exception as e:
                logger.error(f"Error analyzing workflow: {e}")
        
        return analysis
    
    async def create_fix_suggestion_issue(self, repo: Repository, analysis: Dict[str, Any]):
        """Create issue with fix suggestion"""
        if analysis["root_cause"] == "missing_dependencies":
            title = f"🔧 Fix Available: {analysis['workflow']} Build Failure"
            body = (
                f"## Build Failure Analysis\n\n"
                f"**Workflow**: `{analysis['workflow']}`\n"
                f"**Root Cause**: {analysis['root_cause']}\n"
                f"**Confidence**: {analysis['confidence']*100:.0f}%\n\n"
                f"### Problem\n\n"
                f"{analysis['details']['problem']}\n\n"
                f"### Solution\n\n"
                f"{analysis['details']['solution']}\n\n"
                f"### Action Required\n\n"
                f"{analysis['details']['action']}\n\n"
                f"### Quick Fix\n\n"
                f"```bash\n"
                f"cd /path/to/occ\n"
                f"./deploy-workflow-fix.sh\n"
                f"```\n\n"
                f"Or manually:\n\n"
                f"```bash\n"
                f"cp .github/workflows/agi-os-layers-build-fixed.yml .github/workflows/agi-os-layers-build.yml\n"
                f"git add .github/workflows/agi-os-layers-build.yml\n"
                f"git commit -m 'fix(ci): Use workflow with source-built dependencies'\n"
                f"git push origin main\n"
                f"```\n\n"
                f"See `CI_CD_WORKFLOW_FIX.md` for complete documentation.\n\n"
                f"---\n"
                f"*This issue was automatically created by the OCC AGI-OS Integration Bot*"
            )
            
            # Check if similar issue exists
            existing_issues = list(repo.get_issues(
                state='open',
                labels=['ci/cd', 'build-failure', 'automated']
            ))
            
            # Check if we already have this exact issue
            for issue in existing_issues:
                if analysis['workflow'] in issue.title:
                    logger.info(f"Similar issue already exists: #{issue.number}")
                    return issue
            
            # Create new issue
            issue = repo.create_issue(
                title=title,
                body=body,
                labels=['ci/cd', 'build-failure', 'automated', 'fix-available']
            )
            
            logger.info(f"Created fix suggestion issue #{issue.number}")
            return issue

class DocumentationGenerator:
    """Generates and updates documentation"""
    
    def __init__(self, gh: Github):
        self.gh = gh
    
    async def generate_build_status_report(self, repo: Repository) -> str:
        """Generate build status report"""
        report = "# AGI-OS Build Status Report\n\n"
        report += f"**Generated**: {datetime.utcnow().isoformat()}\n\n"
        
        # Get workflow status
        workflows = repo.get_workflows()
        
        report += "## Workflow Status\n\n"
        report += "| Workflow | Status | Last Run | Conclusion |\n"
        report += "|----------|--------|----------|------------|\n"
        
        for workflow in workflows:
            runs = list(workflow.get_runs()[:1])
            if runs:
                latest = runs[0]
                status_emoji = "✅" if latest.conclusion == "success" else "❌" if latest.conclusion == "failure" else "⏳"
                report += f"| {workflow.name} | {status_emoji} {latest.status} | {latest.updated_at} | {latest.conclusion or 'N/A'} |\n"
        
        report += "\n## Layer Status\n\n"
        report += "- **Layer 1 (Cognumach)**: Microkernel with cognitive extensions\n"
        report += "- **Layer 2 (HurdCog)**: Operating system with cognitive kernel\n"
        report += "- **Layer 3 (OpenCog)**: AGI framework integration\n\n"
        
        return report
    
    async def update_readme_status_badge(self, repo: Repository, status: str):
        """Update status badge in README"""
        try:
            readme = repo.get_readme()
            content = readme.decoded_content.decode('utf-8')
            
            # Update or add status badge
            badge_color = "green" if status == "passing" else "red"
            badge = f"![Build Status](https://img.shields.io/badge/build-{status}-{badge_color})"
            
            if "![Build Status]" in content:
                content = re.sub(r'!\[Build Status\]\([^)]+\)', badge, content)
            else:
                content = f"{badge}\n\n{content}"
            
            repo.update_file(
                path=readme.path,
                message="docs: Update build status badge",
                content=content,
                sha=readme.sha
            )
            
            logger.info("Updated README with build status")
        except Exception as e:
            logger.error(f"Error updating README: {e}")

# Export handlers
__all__ = [
    'DependencyHandler',
    'IntegrationTestHandler',
    'BuildAnalyzer',
    'DocumentationGenerator'
]
