#!/bin/bash
# MIG wrapper that fixes 64-bit issues automatically
# This script calls the real MIG, then fixes the generated files

# Use the migcom executable directly
REAL_MIG="/usr/local/bin/mig"

if [ ! -f "$REAL_MIG" ]; then
    echo "Error: Could not find MIG executable at $REAL_MIG" >&2
    exit 1
fi

# Store all original arguments
ORIG_ARGS=("$@")

# Parse arguments to find output files
USER_FILE=""
SERVER_FILE=""
HEADER_FILE=""

i=0
while [ $i -lt ${#ORIG_ARGS[@]} ]; do
    case "${ORIG_ARGS[$i]}" in
        -user)
            i=$((i + 1))
            USER_FILE="${ORIG_ARGS[$i]}"
            ;;
        -server)  
            i=$((i + 1))
            SERVER_FILE="${ORIG_ARGS[$i]}"
            ;;
        -header)
            i=$((i + 1))
            HEADER_FILE="${ORIG_ARGS[$i]}"
            ;;
    esac
    i=$((i + 1))
done

# Run the real MIG with all original arguments
"$REAL_MIG" "${ORIG_ARGS[@]}"
MIG_EXIT_CODE=$?

# If MIG succeeded, fix the generated files
if [ $MIG_EXIT_CODE -eq 0 ]; then
    SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
    FIX_SCRIPT="$SCRIPT_DIR/fix-mig-64bit.sh"
    
    if [ -f "$FIX_SCRIPT" ]; then
        [ -n "$USER_FILE" ] && [ -f "$USER_FILE" ] && "$FIX_SCRIPT" "$USER_FILE"
        [ -n "$SERVER_FILE" ] && [ -f "$SERVER_FILE" ] && "$FIX_SCRIPT" "$SERVER_FILE"
        [ -n "$HEADER_FILE" ] && [ -f "$HEADER_FILE" ] && "$FIX_SCRIPT" "$HEADER_FILE"
    fi
fi

exit $MIG_EXIT_CODE