#!/bin/bash
echo "Stopping..."
# kill old session if exists
tmux kill-session -t george > /dev/null 2>&1
rfcomm release 0
rfcomm release 1
