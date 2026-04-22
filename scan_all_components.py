#!/usr/bin/env python3
"""Scan all OCC components for Windows compatibility issues and generate unified report."""

import os
import sys
import json
from pathlib import Path
sys.path.insert(0, 'tools')
from scan_windows_issues_v2 import WindowsCompatibilityScanner

COMPONENTS = [
    'atomspace',
    'cogutil', 
    'cogserver',
    'atomspace-storage',
    'atomspace-rocks',
    'atomspace-cog',
    'unify',
    'ure',
    'miner',
    'attention',
    'asmoses',
    'matrix',
    'spacetime',
    'pln',
    'learn',
    'moses',
]

def main():
    root = Path('.')
    all_issues = {}
    stats = {}
    
    print("=" * 80)
    print("Scanning all OCC components for Windows compatibility issues")
    print("=" * 80)
    print()
    
    for component in COMPONENTS:
        comp_path = root / component
        if not comp_path.exists():
            print(f"⚠️  {component}: Not found, skipping")
            continue
            
        scanner = WindowsCompatibilityScanner(str(comp_path))
        scanner.scan_directory(comp_path)
        
        # Prefix paths with component name
        for filepath, issues in scanner.issues.items():
            all_issues[f"{component}/{filepath}"] = dict(issues)
        
        file_count = len(scanner.issues)
        posix_count = sum(len(v['posix_headers']) for v in scanner.issues.values())
        alt_ops_count = sum(len(v['alt_operators']) for v in scanner.issues.values())
        total = posix_count + alt_ops_count
        
        stats[component] = {
            'files': file_count,
            'posix_headers': posix_count,
            'alt_operators': alt_ops_count,
            'total': total
        }
        
        if total > 0:
            print(f"🔴 {component}: {total} issues ({file_count} files)")
        else:
            print(f"✅ {component}: Clean")
    
    print()
    print("=" * 80)
    print("Summary")
    print("=" * 80)
    
    total_files = sum(s['files'] for s in stats.values())
    total_posix = sum(s['posix_headers'] for s in stats.values())
    total_alt_ops = sum(s['alt_operators'] for s in stats.values())
    total_issues = sum(s['total'] for s in stats.values())
    
    print(f"Total files with issues: {total_files}")
    print(f"POSIX headers to fix: {total_posix}")
    print(f"Alternative operators to fix: {total_alt_ops}")
    print(f"Total issues: {total_issues}")
    print()
    
    # Save unified report
    with open('all_windows_issues.json', 'w') as f:
        json.dump(all_issues, f, indent=2)
    
    print("✅ Saved all_windows_issues.json")
    print()
    
    return len(all_issues) > 0

if __name__ == '__main__':
    has_issues = main()
    sys.exit(0 if has_issues else 0)
