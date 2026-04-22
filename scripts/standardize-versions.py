#!/usr/bin/env python3
"""
Version Standardization Tool for AGI-OS/OCC

This script helps standardize version numbers across all CMakeLists.txt files
in the repository according to the version policy defined in BUILD_SEQUENCES.md
"""

import os
import re
from pathlib import Path
from typing import Dict, List, Tuple

# Version standardization policy
VERSION_POLICY = {
    # Components that should remain as-is (already standardized)
    'keep': {
        'cognumach': '1.8.0',
        'hurdcog': '0.9.0',
        'cogutil': 'dynamic',  # Reads from version.cmake
        'atomspace': 'dynamic',
        'cogserver': '3.3.0',
        'matrix': '1.3.0',
        'atomspace-storage': '1.3.0',
        'atomspace-rocks': '1.3.0',
        'atomspace-pgres': '1.3.0',
        'unify': 'dynamic',
        'ure': 'dynamic',
        'miner': '1.0.0',
        'sensory': '0.4.0',
        'asmoses': '3.4.0',
    },
    # Components that need version updates
    'update': {
        'atomspace-cog': '1.0.0',  # from 0.1.4
        'pln': '1.0.0',  # from 0.1.1
        'attention': '1.0.0',  # from 0.1.4
    },
    # Components that need version added
    'add': {
        'learn': '1.0.0',
        'agents': '1.0.0',
        'coggml': '1.0.0',
        'cogself': '1.0.0',
        'atomspace-accelerator': '1.0.0',
        'agentic-chatbots': '1.0.0',
    }
}


def find_cmake_files(root_dir: Path) -> Dict[str, Path]:
    """Find all CMakeLists.txt files for components."""
    components = {}
    
    # Core components to check
    component_names = [
        'cogutil', 'atomspace', 'cogserver', 'matrix', 'learn', 'agents', 'sensory',
        'attention', 'unify', 'ure', 'miner', 'pln', 'asmoses',
        'atomspace-storage', 'atomspace-rocks', 'atomspace-cog', 'atomspace-pgres',
        'coggml', 'cogself', 'atomspace-accelerator', 'agentic-chatbots',
        'cognumach', 'hurdcog'
    ]
    
    for comp in component_names:
        comp_path = root_dir / comp / 'CMakeLists.txt'
        if comp_path.exists():
            components[comp] = comp_path
    
    return components


def get_current_version(cmake_file: Path) -> str:
    """Extract current version from CMakeLists.txt."""
    with open(cmake_file, 'r', errors='ignore') as f:
        content = f.read()
    
    # Look for SEMANTIC_VERSION
    match = re.search(r'SET\(SEMANTIC_VERSION\s+([0-9.]+)', content, re.IGNORECASE)
    if match:
        return match.group(1)
    
    # Check for dynamic version
    if 'CMAKE_MATCH_1' in content:
        return 'dynamic'
    
    # Check for PROJECT version
    match = re.search(r'PROJECT\([^)]+VERSION\s+([0-9.]+)', content, re.IGNORECASE)
    if match:
        return match.group(1)
    
    return 'not specified'


def generate_version_report(root_dir: Path) -> None:
    """Generate a report of current versions and needed changes."""
    components = find_cmake_files(root_dir)
    
    print("=" * 80)
    print("VERSION STANDARDIZATION REPORT")
    print("=" * 80)
    print()
    
    print("Components to KEEP (already standardized):")
    print("-" * 80)
    for comp, expected_ver in VERSION_POLICY['keep'].items():
        if comp in components:
            current_ver = get_current_version(components[comp])
            status = "✓" if current_ver == expected_ver or expected_ver == 'dynamic' else "✗"
            print(f"  {status} {comp:30s} {current_ver:15s} (expected: {expected_ver})")
        else:
            print(f"  - {comp:30s} {'N/A':15s} (no CMakeLists.txt)")
    print()
    
    print("Components to UPDATE:")
    print("-" * 80)
    for comp, new_ver in VERSION_POLICY['update'].items():
        if comp in components:
            current_ver = get_current_version(components[comp])
            print(f"  • {comp:30s} {current_ver:15s} → {new_ver}")
        else:
            print(f"  • {comp:30s} {'N/A':15s} → {new_ver} (no CMakeLists.txt)")
    print()
    
    print("Components to ADD version:")
    print("-" * 80)
    for comp, new_ver in VERSION_POLICY['add'].items():
        if comp in components:
            current_ver = get_current_version(components[comp])
            print(f"  • {comp:30s} {current_ver:15s} → {new_ver}")
        else:
            print(f"  • {comp:30s} {'N/A':15s} → {new_ver} (no CMakeLists.txt)")
    print()
    
    print("=" * 80)


def update_version_in_cmake(cmake_file: Path, new_version: str, component: str) -> bool:
    """Update version in CMakeLists.txt file."""
    with open(cmake_file, 'r', errors='ignore') as f:
        content = f.read()
    
    original_content = content
    
    # Check if version already exists
    if re.search(r'SET\(SEMANTIC_VERSION\s+', content, re.IGNORECASE):
        # Update existing version
        content = re.sub(
            r'(SET\(SEMANTIC_VERSION\s+)[0-9.]+',
            f'\\g<1>{new_version}',
            content,
            flags=re.IGNORECASE
        )
    else:
        # Add version after PROJECT line
        project_match = re.search(r'(PROJECT\([^)]+\))', content, re.IGNORECASE)
        if project_match:
            # Insert after PROJECT
            insert_pos = project_match.end()
            version_line = f'\n\n# Version information\nSET(SEMANTIC_VERSION {new_version})'
            content = content[:insert_pos] + version_line + content[insert_pos:]
        else:
            # Add at the beginning after CMAKE_MINIMUM_REQUIRED
            cmake_min_match = re.search(r'(CMAKE_MINIMUM_REQUIRED[^\n]+)', content, re.IGNORECASE)
            if cmake_min_match:
                insert_pos = cmake_min_match.end()
                version_line = f'\n\n# Version information\nSET(SEMANTIC_VERSION {new_version})'
                content = content[:insert_pos] + version_line + content[insert_pos:]
    
    # Only write if changed
    if content != original_content:
        with open(cmake_file, 'w') as f:
            f.write(content)
        return True
    
    return False


def apply_version_updates(root_dir: Path, dry_run: bool = True) -> None:
    """Apply version updates according to policy."""
    components = find_cmake_files(root_dir)
    
    print("=" * 80)
    if dry_run:
        print("DRY RUN - No changes will be made")
    else:
        print("APPLYING VERSION UPDATES")
    print("=" * 80)
    print()
    
    changes_made = []
    
    # Update components
    for comp, new_ver in VERSION_POLICY['update'].items():
        if comp in components:
            if not dry_run:
                changed = update_version_in_cmake(components[comp], new_ver, comp)
                if changed:
                    changes_made.append(comp)
                    print(f"  ✓ Updated {comp} to version {new_ver}")
                else:
                    print(f"  - {comp} already at version {new_ver}")
            else:
                print(f"  [DRY RUN] Would update {comp} to version {new_ver}")
    
    # Add versions to components
    for comp, new_ver in VERSION_POLICY['add'].items():
        if comp in components:
            if not dry_run:
                changed = update_version_in_cmake(components[comp], new_ver, comp)
                if changed:
                    changes_made.append(comp)
                    print(f"  ✓ Added version {new_ver} to {comp}")
                else:
                    print(f"  - {comp} already has version")
            else:
                print(f"  [DRY RUN] Would add version {new_ver} to {comp}")
    
    print()
    if not dry_run and changes_made:
        print(f"Successfully updated {len(changes_made)} components:")
        for comp in changes_made:
            print(f"  - {comp}")
    print("=" * 80)


def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description='Standardize version numbers across AGI-OS components'
    )
    parser.add_argument(
        '--report',
        action='store_true',
        help='Generate version report only'
    )
    parser.add_argument(
        '--apply',
        action='store_true',
        help='Apply version updates (default is dry-run)'
    )
    parser.add_argument(
        '--root',
        type=Path,
        default=Path.cwd(),
        help='Root directory of the repository'
    )
    
    args = parser.parse_args()
    
    if args.report:
        generate_version_report(args.root)
    elif args.apply:
        generate_version_report(args.root)
        print()
        response = input("Apply these changes? [y/N]: ")
        if response.lower() == 'y':
            apply_version_updates(args.root, dry_run=False)
        else:
            print("Cancelled.")
    else:
        # Default: dry run
        generate_version_report(args.root)
        print()
        apply_version_updates(args.root, dry_run=True)
        print()
        print("To apply these changes, run with --apply flag")


if __name__ == '__main__':
    main()
