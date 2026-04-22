# OCC GitHub App Deployment Guide

**Version**: 1.0  
**Date**: December 9, 2025  
**Purpose**: Guide for deploying and managing the OCC GitHub App

## Overview

This guide provides step-by-step instructions for deploying the OCC GitHub App to a production environment. The app is designed to be deployed as a containerized application.

## Prerequisites

1. **GitHub Account**: With admin access to the `cogpy/occ` repository.
2. **Server**: A server or cloud instance to host the application (e.g., AWS EC2, DigitalOcean Droplet).
3. **Domain Name**: A domain name for the webhook endpoint (e.g., `occ-app.your-domain.com`).
4. **Docker**: Docker and Docker Compose installed on the server.
5. **GitHub App**: A GitHub App created with the settings from `app.yml`.

## Step 1: Create GitHub App

1. Go to **Settings** → **Developer settings** → **GitHub Apps**.
2. Click **New GitHub App**.
3. Fill in the details from `app.yml`:
   - **App name**: OCC AGI-OS Integration Bot
   - **Homepage URL**: Your app's homepage
   - **Webhook URL**: `https://your-app-domain.com/webhooks/github`
   - **Webhook secret**: Generate a strong secret and save it.
4. **Permissions**:
   - Set the permissions as specified in `app.yml` (Contents, Issues, PRs, Workflows, Checks).
5. **Events**:
   - Subscribe to the events listed in `app.yml` (Push, Pull Request, Workflow Run, etc.).
6. Click **Create GitHub App**.
7. **Generate a private key**: Download the private key and save it as `private-key.pem`.
8. **Install the app**: Install the app on the `cogpy/occ` repository.
9. **Get App ID and Installation ID**:
   - App ID is on the app's settings page.
   - Installation ID is in the URL after installing the app.

## Step 2: Server Setup

1. **Install Docker and Docker Compose**:
   ```bash
   sudo apt-get update
   sudo apt-get install -y docker.io docker-compose
   ```

2. **Create Project Directory**:
   ```bash
   mkdir -p /opt/occ-github-app
   cd /opt/occ-github-app
   ```

3. **Copy Application Files**:
   - `app.py`
   - `handlers.py`
   - `requirements.txt`
   - `Dockerfile`
   - `docker-compose.yml`
   - `private-key.pem`

4. **Create `.env` file**:
   ```bash
   # GitHub App Configuration
   GITHUB_APP_ID=your-app-id
   GITHUB_PRIVATE_KEY_PATH=/app/private-key.pem
   GITHUB_WEBHOOK_SECRET=your-webhook-secret
   GITHUB_INSTALLATION_ID=your-installation-id

   # Application Configuration
   APP_ENV=production
   APP_HOST=0.0.0.0
   APP_PORT=8080
   LOG_LEVEL=INFO
   ```

## Step 3: Docker Configuration

### `Dockerfile`
```Dockerfile
# Use official Python image
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Copy requirements and install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application files
COPY . .

# Expose port
EXPOSE 8080

# Run application
CMD ["uvicorn", "app:app", "--host", "0.0.0.0", "--port", "8080"]
```

### `docker-compose.yml`
```yaml
version: '3.8'

services:
  app:
    build: .
    container_name: occ-github-app
    restart: always
    env_file:
      - .env
    volumes:
      - ./private-key.pem:/app/private-key.pem:ro
    ports:
      - "127.0.0.1:8080:8080"
    networks:
      - occ-app-net

networks:
  occ-app-net:
    driver: bridge
```

## Step 4: Deploy with Nginx

1. **Install Nginx**:
   ```bash
   sudo apt-get install -y nginx
   ```

2. **Configure Nginx**:
   Create `/etc/nginx/sites-available/occ-app`:
   ```nginx
   server {
       listen 80;
       server_name your-app-domain.com;

       location / {
           return 301 https://$host$request_uri;
       }
   }

   server {
       listen 443 ssl;
       server_name your-app-domain.com;

       # SSL configuration (Let's Encrypt)
       ssl_certificate /etc/letsencrypt/live/your-app-domain.com/fullchain.pem;
       ssl_certificate_key /etc/letsencrypt/live/your-app-domain.com/privkey.pem;

       location / {
           proxy_pass http://127.0.0.1:8080;
           proxy_set_header Host $host;
           proxy_set_header X-Real-IP $remote_addr;
           proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
           proxy_set_header X-Forwarded-Proto $scheme;
       }
   }
   ```

3. **Enable Site and Get SSL Certificate**:
   ```bash
   sudo ln -s /etc/nginx/sites-available/occ-app /etc/nginx/sites-enabled/
   sudo apt-get install -y certbot python3-certbot-nginx
   sudo certbot --nginx -d your-app-domain.com
   sudo systemctl restart nginx
   ```

## Step 5: Run the Application

```bash
cd /opt/occ-github-app
docker-compose up -d
```

## Step 6: Verification

1. **Check Docker logs**:
   ```bash
docker-compose logs -f
   ```

2. **Check health endpoint**:
   ```bash
   curl https://your-app-domain.com/health
   ```

3. **Trigger a webhook**:
   - Push a change to the `cogpy/occ` repository.
   - Check the app logs for the webhook event.

## Maintenance

### Updating the App
```bash
cd /opt/occ-github-app
git pull origin main
docker-compose build
docker-compose up -d
```

### Viewing Logs
```bash
docker-compose logs -f
```

### Backups
- Back up the `.env` file and `private-key.pem`.
- If using a database, set up regular backups.

## Troubleshooting

### 502 Bad Gateway
- Check if the app container is running (`docker-compose ps`).
- Check the app logs for errors (`docker-compose logs`).

### 401 Invalid Signature
- Verify `GITHUB_WEBHOOK_SECRET` is correct.
- Check for extra whitespace or encoding issues.

### GitHub API Errors
- Verify `GITHUB_APP_ID` and `GITHUB_INSTALLATION_ID`.
- Ensure `private-key.pem` is correct and has the right permissions.
- Check GitHub App permissions.

## Security Best Practices

- **Firewall**: Restrict access to necessary ports (80, 443).
- **Private Key**: Set file permissions to `400` (`chmod 400 private-key.pem`).
- **Secrets**: Use a secrets management tool (e.g., Vault, AWS Secrets Manager) for production.
- **Updates**: Keep the server and dependencies updated.
