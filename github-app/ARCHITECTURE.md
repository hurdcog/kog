# OCC GitHub App Architecture

**Version**: 1.0  
**Date**: December 9, 2025  
**Purpose**: Automate AGI-OS integration, CI/CD workflows, and repository management

## Overview

The OCC GitHub App is a comprehensive automation system designed specifically for the AGI-OS repository (cogpy/occ). It monitors repository events, automates build processes, manages dependencies, and coordinates the three-layer AGI-OS architecture (Cognumach, HurdCog, OCC).

## Core Features

### 1. CI/CD Automation
- **Workflow Management**: Automatically trigger and monitor CI/CD workflows
- **Build Dependency Resolution**: Detect and fix missing dependencies (MIG, ACPICA)
- **Layer Coordination**: Ensure proper build order (Cognumach → HurdCog → OCC)
- **Artifact Management**: Track and validate build artifacts across layers

### 2. Integration Testing
- **Cross-Layer Testing**: Validate integration between Cognumach, HurdCog, and OCC
- **Regression Detection**: Identify breaking changes across layers
- **Performance Monitoring**: Track build times and resource usage
- **Test Report Generation**: Create comprehensive test reports

### 3. Dependency Management
- **Automatic Updates**: Monitor and update build tool versions (MIG, ACPICA)
- **Vulnerability Scanning**: Check for security issues in dependencies
- **Version Pinning**: Maintain reproducible builds with version locks
- **Compatibility Checking**: Verify tool compatibility across layers

### 4. Documentation Automation
- **Build Report Generation**: Auto-generate build status reports
- **Architecture Diagrams**: Update architecture documentation
- **Change Logs**: Maintain automatic change logs
- **API Documentation**: Generate API docs from code

### 5. Issue Management
- **Auto-Labeling**: Automatically label issues by component (cognumach/hurdcog/occ)
- **Build Failure Tracking**: Create issues for CI/CD failures
- **Dependency Alerts**: Notify about outdated or vulnerable dependencies
- **Integration Status**: Track integration health across layers

## Architecture Components

### 1. Webhook Handler
```
GitHub Events → Webhook Handler → Event Router → Action Handlers
```

**Supported Events**:
- `push`: Code changes to main branches
- `pull_request`: PR creation, updates, and merges
- `workflow_run`: CI/CD workflow completions
- `issues`: Issue creation and updates
- `check_run`: Build check status updates
- `release`: Release creation and publishing

### 2. Event Router

Routes events to appropriate handlers based on:
- Event type
- Affected paths (cognumach/*, hurdcog/*, opencog-debian/*)
- Branch name
- User permissions

### 3. Action Handlers

**Build Handler**:
- Monitors workflow runs
- Detects build failures
- Suggests fixes (e.g., missing dependencies)
- Triggers retry workflows

**Integration Handler**:
- Validates cross-layer compatibility
- Runs integration tests
- Updates integration status
- Creates integration reports

**Dependency Handler**:
- Checks for dependency updates
- Creates PRs for version bumps
- Validates dependency compatibility
- Manages build tool versions

**Documentation Handler**:
- Generates build reports
- Updates README files
- Creates release notes
- Maintains architecture docs

### 4. State Manager

Maintains application state:
- Build status per layer
- Dependency versions
- Integration test results
- Workflow run history

### 5. GitHub API Client

Interacts with GitHub API:
- Create/update issues
- Trigger workflows
- Manage pull requests
- Update commit statuses
- Create releases

## Data Flow

```
1. GitHub Event
   ↓
2. Webhook Receiver (validates signature)
   ↓
3. Event Parser (extracts relevant data)
   ↓
4. Event Router (determines handler)
   ↓
5. Action Handler (performs actions)
   ↓
6. GitHub API Client (updates GitHub)
   ↓
7. State Manager (records results)
```

## Security

### Authentication
- **GitHub App JWT**: For API authentication
- **Installation Access Token**: For repository-specific operations
- **Webhook Secret**: For webhook signature verification

### Permissions Required
- **Contents**: Read/Write (for creating files, updating workflows)
- **Issues**: Read/Write (for creating and managing issues)
- **Pull Requests**: Read/Write (for creating and reviewing PRs)
- **Workflows**: Read/Write (for triggering and managing workflows)
- **Checks**: Read/Write (for creating check runs)
- **Metadata**: Read (for repository information)

### Security Best Practices
- Validate all webhook signatures
- Use short-lived installation tokens
- Encrypt sensitive data at rest
- Log all API operations
- Rate limit handling

## Deployment Architecture

```
┌─────────────────────────────────────────┐
│         GitHub (cogpy/occ)              │
│  ┌─────────────────────────────────┐   │
│  │  Webhooks                        │   │
│  └──────────────┬──────────────────┘   │
└─────────────────┼──────────────────────┘
                  │ HTTPS
                  ↓
┌─────────────────────────────────────────┐
│         OCC GitHub App Server           │
│  ┌─────────────────────────────────┐   │
│  │  Webhook Handler                 │   │
│  │  - Signature Verification        │   │
│  │  - Event Parsing                 │   │
│  └──────────────┬──────────────────┘   │
│                 ↓                       │
│  ┌─────────────────────────────────┐   │
│  │  Event Router                    │   │
│  └──────────────┬──────────────────┘   │
│                 ↓                       │
│  ┌─────────────────────────────────┐   │
│  │  Action Handlers                 │   │
│  │  - Build Handler                 │   │
│  │  - Integration Handler           │   │
│  │  - Dependency Handler            │   │
│  │  - Documentation Handler         │   │
│  └──────────────┬──────────────────┘   │
│                 ↓                       │
│  ┌─────────────────────────────────┐   │
│  │  GitHub API Client               │   │
│  └──────────────┬──────────────────┘   │
└─────────────────┼──────────────────────┘
                  │ GitHub API
                  ↓
┌─────────────────────────────────────────┐
│         GitHub API                      │
│  - Create Issues                        │
│  - Trigger Workflows                    │
│  - Update Statuses                      │
│  - Manage PRs                           │
└─────────────────────────────────────────┘
```

## Technology Stack

### Backend
- **Language**: Python 3.11+
- **Framework**: FastAPI (async web framework)
- **GitHub Integration**: PyGithub / github3.py
- **Authentication**: PyJWT (for GitHub App JWT)
- **Webhook Handling**: HMAC signature verification

### Storage
- **State Storage**: SQLite (for development) / PostgreSQL (for production)
- **Cache**: Redis (for rate limiting and temporary data)
- **Logs**: Structured logging to files/stdout

### Deployment
- **Container**: Docker
- **Orchestration**: Docker Compose / Kubernetes
- **Reverse Proxy**: Nginx (for webhook endpoint)
- **SSL/TLS**: Let's Encrypt (for HTTPS)

## Configuration

### Environment Variables
```bash
# GitHub App Configuration
GITHUB_APP_ID=123456
GITHUB_APP_PRIVATE_KEY_PATH=/path/to/private-key.pem
GITHUB_WEBHOOK_SECRET=your-webhook-secret
GITHUB_INSTALLATION_ID=987654

# Application Configuration
APP_ENV=production
APP_HOST=0.0.0.0
APP_PORT=8080
LOG_LEVEL=INFO

# Database Configuration
DATABASE_URL=postgresql://user:pass@localhost/occ_app
REDIS_URL=redis://localhost:6379/0
```

### App Manifest
```yaml
name: OCC AGI-OS Integration Bot
description: Automates CI/CD, integration testing, and dependency management
url: https://github.com/cogpy/occ
permissions:
  contents: write
  issues: write
  pull_requests: write
  workflows: write
  checks: write
events:
  - push
  - pull_request
  - workflow_run
  - issues
  - check_run
```

## API Endpoints

### Webhook Endpoint
```
POST /webhooks/github
- Receives GitHub webhook events
- Validates signature
- Routes to appropriate handler
```

### Health Check
```
GET /health
- Returns application health status
- Checks database connectivity
- Validates GitHub API access
```

### Status Dashboard
```
GET /status
- Shows current build status
- Displays integration health
- Lists recent events
```

## Monitoring and Observability

### Metrics
- Webhook events received
- API calls made
- Handler execution times
- Error rates
- Build success/failure rates

### Logging
- Structured JSON logs
- Request/response logging
- Error tracking with stack traces
- Audit trail for all actions

### Alerting
- Build failures
- API rate limit warnings
- Webhook delivery failures
- Integration test failures

## Future Enhancements

1. **ML-Based Build Prediction**: Predict build failures before they occur
2. **Automated Dependency Resolution**: Automatically fix dependency conflicts
3. **Performance Optimization**: Suggest build optimizations based on historical data
4. **Multi-Repository Support**: Extend to manage multiple AGI-OS repositories
5. **Slack/Discord Integration**: Send notifications to team channels
6. **Dashboard UI**: Web-based dashboard for monitoring and management

## Development Workflow

1. **Local Development**: Run app locally with ngrok for webhook testing
2. **Testing**: Unit tests, integration tests, webhook simulation
3. **Staging**: Deploy to staging environment with test repository
4. **Production**: Deploy to production with monitoring and alerts

## Maintenance

### Regular Tasks
- Update dependencies monthly
- Review and optimize API usage
- Clean up old logs and state data
- Update documentation

### Monitoring
- Check error logs daily
- Review API rate limit usage
- Monitor webhook delivery success
- Track build success rates

## Conclusion

The OCC GitHub App provides comprehensive automation for the AGI-OS repository, reducing manual work and improving build reliability. Its modular architecture allows for easy extension and maintenance, while its robust security and monitoring ensure reliable operation in production.
