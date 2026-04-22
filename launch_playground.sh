#!/bin/bash
set -e  # Exit on error
# Quick launcher for NeuroCog Playground

cd "$(dirname "$0")"

# Set Python path to include current directory
export PYTHONPATH="$(pwd):$PYTHONPATH"

# Run the playground
python3 cogself/neurocog_playground.py
