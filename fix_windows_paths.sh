#!/bin/bash
# Fix Windows path compatibility by renaming files with colons

set -e

echo "Fixing Windows-incompatible file paths..."

# Find all files with colons and rename them
find language-learning -name "*:*" -type f | while read -r file; do
    dir=$(dirname "$file")
    base=$(basename "$file")
    # Replace colons with underscores
    newbase=$(echo "$base" | tr ':' '_')
    newfile="$dir/$newbase"
    
    if [ "$file" != "$newfile" ]; then
        echo "Renaming: $file -> $newfile"
        mv "$file" "$newfile"
    fi
done

# Find all directories with colons and rename them
find language-learning -name "*:*" -type d -depth | while read -r dir; do
    parent=$(dirname "$dir")
    base=$(basename "$dir")
    # Replace colons with underscores
    newbase=$(echo "$base" | tr ':' '_')
    newdir="$parent/$newbase"
    
    if [ "$dir" != "$newdir" ]; then
        echo "Renaming directory: $dir -> $newdir"
        mv "$dir" "$newdir"
    fi
done

echo "Windows path compatibility fixes complete!"
