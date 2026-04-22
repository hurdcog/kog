#!/bin/bash

# Cognitive Fusion Reactor - Dashboard Startup Script
# Starts the Master Control Dashboard server

set -e

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SERVER_SCRIPT="fusion-reactor-server.py"
DASHBOARD_FILE="fusion-reactor-dashboard.html"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Banner
echo "=============================================================================="
echo -e "${GREEN}🧬 Cognitive Fusion Reactor - Master Control Dashboard${NC}"
echo "=============================================================================="
echo ""

# Check if Python 3 is installed
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}Error: Python 3 is not installed${NC}"
    echo "Please install Python 3.7 or higher to run the dashboard"
    exit 1
fi

# Display Python version
PYTHON_VERSION=$(python3 --version 2>&1)
echo -e "${BLUE}Using: ${PYTHON_VERSION}${NC}"
echo ""

# Check if server script exists
if [ ! -f "$SCRIPT_DIR/$SERVER_SCRIPT" ]; then
    echo -e "${RED}Error: Server script not found: $SERVER_SCRIPT${NC}"
    exit 1
fi

# Check if dashboard HTML exists
if [ ! -f "$SCRIPT_DIR/$DASHBOARD_FILE" ]; then
    echo -e "${RED}Error: Dashboard file not found: $DASHBOARD_FILE${NC}"
    exit 1
fi

# Check if port 8080 is available
if lsof -Pi :8080 -sTCP:LISTEN -t >/dev/null 2>&1; then
    echo -e "${YELLOW}Warning: Port 8080 is already in use${NC}"
    echo "Another process is using port 8080. The dashboard may not start correctly."
    echo ""
    read -p "Do you want to continue anyway? (y/N) " -n 1 -r
    echo ""
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Startup cancelled."
        exit 1
    fi
fi

# Change to script directory
cd "$SCRIPT_DIR"

echo -e "${GREEN}Starting Cognitive Fusion Reactor Dashboard Server...${NC}"
echo ""
echo "Dashboard will be available at:"
echo -e "  ${BLUE}http://localhost:8080/dashboard${NC}"
echo ""
echo "API endpoints will be available at:"
echo -e "  ${BLUE}http://localhost:8080/api/${NC}"
echo ""
echo -e "${YELLOW}Press Ctrl+C to stop the server${NC}"
echo "=============================================================================="
echo ""

# Start the server
python3 "$SERVER_SCRIPT"
