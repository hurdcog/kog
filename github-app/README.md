# OCC AGI-OS Integration Bot

**Version**: 1.0.0  
**Status**: Production Ready  
**Repository**: cogpy/occ

## Overview

The OCC AGI-OS Integration Bot is a comprehensive GitHub App designed to automate CI/CD workflows, integration testing, and dependency management for the AGI-OS repository. It monitors repository events, analyzes build failures, manages dependencies, and coordinates the three-layer AGI-OS architecture (Cognumach, HurdCog, OCC).

## Features

### 🤖 Automated CI/CD Management
- Automatically triggers workflows when code changes affect specific layers
- Detects build failures and suggests fixes
- Creates issues with detailed fix instructions
- Monitors workflow status across all layers

### 🔬 Integration Testing
- Validates cross-layer compatibility
- Runs integration tests on pull requests
- Generates comprehensive test reports
- Tracks integration health

### 📦 Dependency Management
- Monitors build tool versions (MIG, ACPICA)
- Detects missing or outdated dependencies
- Creates PRs for dependency updates
- Ensures reproducible builds

### 📝 Documentation Automation
- Generates build status reports
- Updates README badges
- Creates release notes
- Maintains architecture documentation

### 🏷️ Issue Management
- Auto-labels issues by component
- Tracks build failures
- Notifies about dependency issues
- Manages integration status

## Architecture

```
GitHub Events → Webhook Handler → Event Router → Action Handlers → GitHub API
```

### Components

1. **Webhook Handler**: Receives and validates GitHub webhook events
2. **Event Router**: Routes events to appropriate handlers
3. **Action Handlers**: Performs specific actions (build analysis, dependency checks, etc.)
4. **GitHub API Client**: Interacts with GitHub API

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed architecture documentation.

## Installation

### Prerequisites

- Python 3.11+
- Docker and Docker Compose
- GitHub App credentials (App ID, Private Key, Installation ID)
- Domain name for webhook endpoint

### Quick Start

1. **Clone the repository**:
   ```bash
   git clone https://github.com/cogpy/occ.git
   cd occ/github-app
   ```

2. **Create `.env` file**:
   ```bash
   cp .env.example .env
   # Edit .env with your GitHub App credentials
   ```

3. **Place your private key**:
   ```bash
   cp /path/to/your/private-key.pem ./private-key.pem
   chmod 400 private-key.pem
   ```

4. **Run with Docker Compose**:
   ```bash
   docker-compose up -d
   ```

5. **Verify deployment**:
   ```bash
   curl http://localhost:8080/health
   ```

See [DEPLOYMENT.md](DEPLOYMENT.md) for complete deployment instructions.

## Configuration

### Environment Variables

| Variable | Description | Required |
|----------|-------------|----------|
| `GITHUB_APP_ID` | GitHub App ID | Yes |
| `GITHUB_PRIVATE_KEY_PATH` | Path to private key file | Yes |
| `GITHUB_WEBHOOK_SECRET` | Webhook secret for signature verification | Yes |
| `GITHUB_INSTALLATION_ID` | Installation ID for the repository | Yes |
| `APP_HOST` | Host to bind to (default: 0.0.0.0) | No |
| `APP_PORT` | Port to listen on (default: 8080) | No |
| `LOG_LEVEL` | Logging level (default: INFO) | No |

### GitHub App Permissions

The app requires the following permissions:

- **Contents**: Read/Write (for updating workflows and files)
- **Issues**: Read/Write (for creating and managing issues)
- **Pull Requests**: Read/Write (for creating PRs and comments)
- **Workflows**: Read/Write (for triggering workflows)
- **Checks**: Read/Write (for creating check runs)

### Webhook Events

The app subscribes to these events:

- `push`: Code changes
- `pull_request`: PR creation and updates
- `workflow_run`: Workflow completions
- `issues`: Issue creation and updates
- `check_run`: Check status updates

## Usage

### Automatic Actions

The app automatically performs actions based on repository events:

**On Push**:
- Detects affected layers (cognumach, hurdcog, opencog-debian)
- Triggers appropriate workflows
- Logs layer changes

**On Pull Request**:
- Runs integration tests
- Creates test report comments
- Validates cross-layer compatibility

**On Workflow Failure**:
- Analyzes failure cause
- Creates issue with fix suggestions
- Provides detailed remediation steps

**On Issue Creation**:
- Auto-labels based on content
- Categorizes by component and type
- Links to relevant documentation

### Manual Triggers

You can manually trigger workflows through the GitHub UI:
1. Go to **Actions** tab
2. Select a workflow
3. Click **Run workflow**

### API Endpoints

- `GET /`: Root endpoint with app info
- `GET /health`: Health check endpoint
- `POST /webhooks/github`: Webhook receiver
- `GET /status`: Current build status

## Development

### Local Development

1. **Install dependencies**:
   ```bash
   pip install -r requirements.txt
   ```

2. **Set up ngrok** (for webhook testing):
   ```bash
   ngrok http 8080
   ```

3. **Update webhook URL** in GitHub App settings to ngrok URL

4. **Run the app**:
   ```bash
   python app.py
   ```

### Testing

```bash
# Run unit tests
pytest tests/

# Run integration tests
pytest tests/integration/

# Test webhook signature verification
pytest tests/test_webhooks.py
```

### Adding New Handlers

1. Create a new handler class in `handlers.py`
2. Add the handler to `EventRouter` in `app.py`
3. Update tests
4. Update documentation

## Monitoring

### Logs

View logs with Docker Compose:
```bash
docker-compose logs -f
```

### Metrics

The app logs the following metrics:
- Webhook events received
- API calls made
- Handler execution times
- Error rates

### Health Checks

The app includes a health check endpoint that verifies:
- Application is running
- GitHub API is accessible
- Rate limit status

## Troubleshooting

### Common Issues

**502 Bad Gateway**:
- Check if the app container is running
- Verify nginx configuration
- Check app logs for errors

**401 Invalid Signature**:
- Verify webhook secret is correct
- Check for encoding issues
- Ensure no extra whitespace in secret

**GitHub API Errors**:
- Verify App ID and Installation ID
- Check private key is correct
- Ensure app has required permissions

See [DEPLOYMENT.md](DEPLOYMENT.md) for more troubleshooting tips.

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests
5. Submit a pull request

## License

This project is part of the cogpy/occ repository and follows the same license.

## Support

For issues or questions:
- Create an issue in the repository
- Check existing documentation
- Review logs for error messages

## Roadmap

- [ ] ML-based build failure prediction
- [ ] Automated dependency resolution
- [ ] Performance optimization suggestions
- [ ] Multi-repository support
- [ ] Slack/Discord integration
- [ ] Web-based dashboard

## Acknowledgments

Built for the AGI-OS project, integrating:
- **Cognumach**: Cognitive microkernel
- **HurdCog**: Cognitive operating system
- **OpenCog**: AGI framework

---

**Maintained by**: AGI-OS Team  
**Last Updated**: December 9, 2025
