#!/usr/bin/env python3
"""
OCC GitHub App - Main Application
Automates CI/CD, integration testing, and dependency management for AGI-OS
"""

import os
import hmac
import hashlib
import json
import logging
from datetime import datetime, timedelta
from typing import Optional, Dict, Any

from fastapi import FastAPI, Request, HTTPException, BackgroundTasks
from fastapi.responses import JSONResponse
import jwt
from github import Github, GithubIntegration

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
)
logger = logging.getLogger(__name__)

# Initialize FastAPI app
app = FastAPI(
    title="OCC AGI-OS Integration Bot",
    description="Automates CI/CD workflows, integration testing, and dependency management",
    version="1.0.0"
)

# Configuration
class Config:
    """Application configuration"""
    GITHUB_APP_ID = os.getenv("GITHUB_APP_ID")
    GITHUB_PRIVATE_KEY_PATH = os.getenv("GITHUB_PRIVATE_KEY_PATH", "private-key.pem")
    GITHUB_WEBHOOK_SECRET = os.getenv("GITHUB_WEBHOOK_SECRET")
    GITHUB_INSTALLATION_ID = os.getenv("GITHUB_INSTALLATION_ID")
    
    APP_HOST = os.getenv("APP_HOST", "0.0.0.0")
    APP_PORT = int(os.getenv("APP_PORT", "8080"))
    
    @classmethod
    def load_private_key(cls) -> str:
        """Load GitHub App private key"""
        with open(cls.GITHUB_PRIVATE_KEY_PATH, 'r') as f:
            return f.read()

config = Config()

# GitHub Integration
class GitHubAppClient:
    """GitHub App API client"""
    
    def __init__(self):
        self.app_id = config.GITHUB_APP_ID
        self.private_key = config.load_private_key()
        self.installation_id = config.GITHUB_INSTALLATION_ID
        
    def get_jwt(self) -> str:
        """Generate JWT for GitHub App authentication"""
        now = datetime.utcnow()
        payload = {
            'iat': now,
            'exp': now + timedelta(minutes=10),
            'iss': self.app_id
        }
        return jwt.encode(payload, self.private_key, algorithm='RS256')
    
    def get_installation_token(self) -> str:
        """Get installation access token"""
        integration = GithubIntegration(self.app_id, self.private_key)
        auth = integration.get_access_token(self.installation_id)
        return auth.token
    
    def get_client(self) -> Github:
        """Get authenticated GitHub client"""
        token = self.get_installation_token()
        return Github(token)

github_client = GitHubAppClient()

# Webhook signature verification
def verify_signature(payload_body: bytes, signature_header: str) -> bool:
    """Verify GitHub webhook signature"""
    if not signature_header:
        return False
    
    hash_object = hmac.new(
        config.GITHUB_WEBHOOK_SECRET.encode('utf-8'),
        msg=payload_body,
        digestmod=hashlib.sha256
    )
    expected_signature = "sha256=" + hash_object.hexdigest()
    
    return hmac.compare_digest(expected_signature, signature_header)

# Event Handlers
class EventRouter:
    """Routes webhook events to appropriate handlers"""
    
    def __init__(self):
        self.handlers = {
            'push': self.handle_push,
            'pull_request': self.handle_pull_request,
            'workflow_run': self.handle_workflow_run,
            'issues': self.handle_issues,
            'check_run': self.handle_check_run,
        }
    
    async def route(self, event_type: str, payload: Dict[str, Any]):
        """Route event to appropriate handler"""
        handler = self.handlers.get(event_type)
        if handler:
            try:
                await handler(payload)
            except Exception as e:
                logger.error(f"Error handling {event_type}: {e}", exc_info=True)
        else:
            logger.info(f"No handler for event type: {event_type}")
    
    async def handle_push(self, payload: Dict[str, Any]):
        """Handle push events"""
        repo_name = payload['repository']['full_name']
        ref = payload['ref']
        commits = payload['commits']
        
        logger.info(f"Push to {repo_name} on {ref} with {len(commits)} commits")
        
        # Check if push affects cognumach, hurdcog, or opencog-debian
        affected_paths = set()
        for commit in commits:
            affected_paths.update(commit.get('added', []))
            affected_paths.update(commit.get('modified', []))
        
        layers_affected = []
        if any(p.startswith('cognumach/') for p in affected_paths):
            layers_affected.append('cognumach')
        if any(p.startswith('hurdcog/') for p in affected_paths):
            layers_affected.append('hurdcog')
        if any(p.startswith('opencog-debian/') for p in affected_paths):
            layers_affected.append('opencog-debian')
        
        if layers_affected:
            logger.info(f"Affected layers: {', '.join(layers_affected)}")
            # Trigger appropriate workflows
            await self.trigger_layer_builds(repo_name, layers_affected)
    
    async def handle_pull_request(self, payload: Dict[str, Any]):
        """Handle pull request events"""
        action = payload['action']
        pr_number = payload['pull_request']['number']
        repo_name = payload['repository']['full_name']
        
        logger.info(f"PR #{pr_number} {action} in {repo_name}")
        
        if action in ['opened', 'synchronize']:
            # Run integration tests
            await self.run_integration_tests(repo_name, pr_number)
    
    async def handle_workflow_run(self, payload: Dict[str, Any]):
        """Handle workflow run events"""
        workflow_name = payload['workflow_run']['name']
        conclusion = payload['workflow_run']['conclusion']
        repo_name = payload['repository']['full_name']
        
        logger.info(f"Workflow '{workflow_name}' completed with {conclusion}")
        
        if conclusion == 'failure':
            # Analyze failure and suggest fixes
            await self.analyze_build_failure(repo_name, payload['workflow_run'])
    
    async def handle_issues(self, payload: Dict[str, Any]):
        """Handle issue events"""
        action = payload['action']
        issue_number = payload['issue']['number']
        repo_name = payload['repository']['full_name']
        
        logger.info(f"Issue #{issue_number} {action} in {repo_name}")
        
        if action == 'opened':
            # Auto-label based on content
            await self.auto_label_issue(repo_name, issue_number, payload['issue'])
    
    async def handle_check_run(self, payload: Dict[str, Any]):
        """Handle check run events"""
        check_name = payload['check_run']['name']
        conclusion = payload['check_run']['conclusion']
        
        logger.info(f"Check '{check_name}' completed with {conclusion}")
    
    async def trigger_layer_builds(self, repo_name: str, layers: list):
        """Trigger builds for affected layers"""
        gh = github_client.get_client()
        repo = gh.get_repo(repo_name)
        
        # Trigger the fixed workflow
        workflow = repo.get_workflow("agi-os-layers-build-fixed.yml")
        workflow.create_dispatch(ref='main')
        
        logger.info(f"Triggered workflow for layers: {', '.join(layers)}")
    
    async def run_integration_tests(self, repo_name: str, pr_number: int):
        """Run integration tests for a PR"""
        gh = github_client.get_client()
        repo = gh.get_repo(repo_name)
        pr = repo.get_pull(pr_number)
        
        # Create a comment on the PR
        pr.create_issue_comment(
            "🤖 Running AGI-OS integration tests...\n\n"
            "This will verify compatibility across all three layers:\n"
            "- Layer 1: Cognumach Microkernel\n"
            "- Layer 2: HurdCog Operating System\n"
            "- Layer 3: OpenCog Framework"
        )
        
        logger.info(f"Started integration tests for PR #{pr_number}")
    
    async def analyze_build_failure(self, repo_name: str, workflow_run: Dict[str, Any]):
        """Analyze build failure and suggest fixes"""
        gh = github_client.get_client()
        repo = gh.get_repo(repo_name)
        
        # Check if it's the missing dependency error
        workflow_name = workflow_run['name']
        if 'Cognumach' in workflow_name or 'HurdCog' in workflow_name:
            # Create an issue suggesting the fix
            issue_title = f"CI/CD Build Failure: {workflow_name}"
            issue_body = (
                f"## Build Failure Detected\n\n"
                f"The workflow `{workflow_name}` has failed.\n\n"
                f"**Possible cause**: Missing build dependencies (MIG, ACPICA)\n\n"
                f"**Suggested fix**: Use the corrected workflow that builds dependencies from source:\n"
                f"- File: `.github/workflows/agi-os-layers-build-fixed.yml`\n"
                f"- This workflow builds MIG and ACPICA from source\n\n"
                f"**Workflow Run**: {workflow_run['html_url']}\n\n"
                f"See `CI_CD_WORKFLOW_FIX.md` for details."
            )
            
            # Check if similar issue already exists
            existing_issues = repo.get_issues(state='open', labels=['ci/cd', 'build-failure'])
            if not any(issue.title == issue_title for issue in existing_issues):
                issue = repo.create_issue(
                    title=issue_title,
                    body=issue_body,
                    labels=['ci/cd', 'build-failure', 'automated']
                )
                logger.info(f"Created issue #{issue.number} for build failure")
    
    async def auto_label_issue(self, repo_name: str, issue_number: int, issue_data: Dict[str, Any]):
        """Automatically label issues based on content"""
        gh = github_client.get_client()
        repo = gh.get_repo(repo_name)
        issue = repo.get_issue(issue_number)
        
        title = issue_data['title'].lower()
        body = issue_data.get('body', '').lower()
        content = title + ' ' + body
        
        labels = []
        
        # Component labels
        if 'cognumach' in content or 'microkernel' in content:
            labels.append('layer-1-cognumach')
        if 'hurdcog' in content or 'hurd' in content:
            labels.append('layer-2-hurdcog')
        if 'opencog' in content or 'atomspace' in content:
            labels.append('layer-3-opencog')
        
        # Type labels
        if 'bug' in content or 'error' in content or 'fail' in content:
            labels.append('bug')
        if 'feature' in content or 'enhancement' in content:
            labels.append('enhancement')
        if 'ci' in content or 'workflow' in content or 'build' in content:
            labels.append('ci/cd')
        if 'dependency' in content or 'mig' in content or 'acpica' in content:
            labels.append('dependencies')
        
        if labels:
            issue.add_to_labels(*labels)
            logger.info(f"Added labels to issue #{issue_number}: {', '.join(labels)}")

event_router = EventRouter()

# API Endpoints
@app.get("/")
async def root():
    """Root endpoint"""
    return {
        "name": "OCC AGI-OS Integration Bot",
        "version": "1.0.0",
        "status": "running"
    }

@app.get("/health")
async def health_check():
    """Health check endpoint"""
    try:
        # Verify GitHub API access
        gh = github_client.get_client()
        rate_limit = gh.get_rate_limit()
        
        return {
            "status": "healthy",
            "github_api": "connected",
            "rate_limit_remaining": rate_limit.core.remaining,
            "timestamp": datetime.utcnow().isoformat()
        }
    except Exception as e:
        logger.error(f"Health check failed: {e}")
        return JSONResponse(
            status_code=503,
            content={"status": "unhealthy", "error": str(e)}
        )

@app.post("/webhooks/github")
async def github_webhook(request: Request, background_tasks: BackgroundTasks):
    """GitHub webhook endpoint"""
    # Get headers
    signature = request.headers.get('X-Hub-Signature-256')
    event_type = request.headers.get('X-GitHub-Event')
    delivery_id = request.headers.get('X-GitHub-Delivery')
    
    # Get payload
    payload_body = await request.body()
    
    # Verify signature
    if not verify_signature(payload_body, signature):
        logger.warning(f"Invalid signature for delivery {delivery_id}")
        raise HTTPException(status_code=401, detail="Invalid signature")
    
    # Parse payload
    try:
        payload = json.loads(payload_body)
    except json.JSONDecodeError:
        raise HTTPException(status_code=400, detail="Invalid JSON payload")
    
    logger.info(f"Received {event_type} event (delivery: {delivery_id})")
    
    # Route event to handler (in background)
    background_tasks.add_task(event_router.route, event_type, payload)
    
    return {"status": "accepted", "event": event_type, "delivery_id": delivery_id}

@app.get("/status")
async def get_status():
    """Get current status"""
    gh = github_client.get_client()
    repo = gh.get_repo("cogpy/occ")
    
    # Get latest workflow runs
    workflows = repo.get_workflows()
    workflow_status = []
    
    for workflow in workflows:
        runs = workflow.get_runs()
        latest_run = runs[0] if runs.totalCount > 0 else None
        
        workflow_status.append({
            "name": workflow.name,
            "status": latest_run.status if latest_run else "unknown",
            "conclusion": latest_run.conclusion if latest_run else None,
            "updated_at": latest_run.updated_at.isoformat() if latest_run else None
        })
    
    return {
        "repository": "cogpy/occ",
        "workflows": workflow_status,
        "timestamp": datetime.utcnow().isoformat()
    }

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(
        app,
        host=config.APP_HOST,
        port=config.APP_PORT,
        log_level="info"
    )
