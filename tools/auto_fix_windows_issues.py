#!/usr/bin/env python3
"""
Automated Windows Compatibility Fixer

Reads JSON report from scanner and automatically fixes:
1. POSIX headers → Windows equivalents (with conditional compilation)
2. Alternative operators → Standard operators
"""

import os
import re
import sys
import json
import shutil
from pathlib import Path
from collections import defaultdict

class WindowsCompatibilityFixer:
    def __init__(self, root_dir, json_report_path):
        self.root_dir = Path(root_dir)
        self.json_report_path = json_report_path
        self.issues = {}
        self.fixes_applied = defaultdict(lambda: defaultdict(int))
        self.files_modified = set()
        
        # Load issues from JSON report
        with open(json_report_path, 'r') as f:
            self.issues = json.load(f)
    
    def fix_posix_headers(self, filepath, issues):
        """Fix POSIX header includes with Windows conditional compilation."""
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()
        
        modified = False
        i = 0
        while i < len(lines):
            line = lines[i]
            
            # Check if this line has a POSIX header issue
            for issue in issues:
                if issue['line'] == i + 1:
                    header = issue['header']
                    
                    # Generate Windows-specific replacement
                    if header == 'unistd.h':
                        replacement = [
                            '#ifdef _WIN32\n',
                            '#include <io.h>\n',
                            '#include <process.h>\n',
                            '#else\n',
                            line,
                            '#endif\n'
                        ]
                    elif header == 'strings.h':
                        replacement = [
                            '#ifdef _WIN32\n',
                            '#include <string.h>\n',
                            '#else\n',
                            line,
                            '#endif\n'
                        ]
                    elif header in ['sys/socket.h', 'sys/select.h', 'netinet/in.h', 'arpa/inet.h', 'netdb.h', 'sys/time.h']:
                        replacement = [
                            '#ifdef _WIN32\n',
                            '#include <winsock2.h>\n',
                            '#else\n',
                            line,
                            '#endif\n'
                        ]
                    elif header == 'dlfcn.h':
                        replacement = [
                            '#ifdef _WIN32\n',
                            '#include <windows.h>\n',
                            '#else\n',
                            line,
                            '#endif\n'
                        ]
                    else:
                        # For other headers, just add conditional
                        replacement = [
                            '#ifndef _WIN32\n',
                            line,
                            '#endif\n'
                        ]
                    
                    lines[i:i+1] = replacement
                    modified = True
                    self.fixes_applied[str(filepath)]['posix_headers'] += 1
                    break
            
            i += 1
        
        if modified:
            with open(filepath, 'w', encoding='utf-8') as f:
                f.writelines(lines)
            self.files_modified.add(str(filepath))
        
        return modified
    
    def fix_alternative_operators(self, filepath, issues):
        """Fix alternative operators by replacing with standard operators."""
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
        
        original_content = content
        
        # Group issues by line for efficient processing
        issues_by_line = defaultdict(list)
        for issue in issues:
            issues_by_line[issue['line']].append(issue)
        
        lines = content.split('\n')
        modified = False
        
        for line_num, line_issues in issues_by_line.items():
            if line_num > len(lines):
                continue
            
            line_idx = line_num - 1
            original_line = lines[line_idx]
            modified_line = original_line
            
            # Sort by operator to ensure consistent replacement
            for issue in sorted(line_issues, key=lambda x: x['operator']):
                operator = issue['operator']
                replacement = issue['replacement']
                
                # Replace whole word only
                pattern = r'\b' + re.escape(operator) + r'\b'
                modified_line = re.sub(pattern, replacement, modified_line)
            
            if modified_line != original_line:
                lines[line_idx] = modified_line
                modified = True
                self.fixes_applied[str(filepath)]['alt_operators'] += len(line_issues)
        
        if modified:
            content = '\n'.join(lines)
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(content)
            self.files_modified.add(str(filepath))
        
        return modified
    
    def fix_file(self, rel_filepath):
        """Fix all issues in a single file."""
        filepath = self.root_dir / rel_filepath
        
        if not filepath.exists():
            print(f"Warning: File not found: {filepath}")
            return False
        
        file_issues = self.issues.get(rel_filepath, {})
        
        if not file_issues:
            return False
        
        # Create backup
        backup_path = str(filepath) + '.backup'
        shutil.copy2(filepath, backup_path)
        
        try:
            # Fix POSIX headers first
            if file_issues.get('posix_headers'):
                self.fix_posix_headers(filepath, file_issues['posix_headers'])
            
            # Fix alternative operators
            if file_issues.get('alt_operators'):
                self.fix_alternative_operators(filepath, file_issues['alt_operators'])
            
            # Remove backup if successful
            os.remove(backup_path)
            return True
            
        except Exception as e:
            print(f"Error fixing {filepath}: {e}")
            # Restore from backup
            shutil.copy2(backup_path, filepath)
            os.remove(backup_path)
            return False
    
    def fix_all(self):
        """Fix all files with issues."""
        total_files = len(self.issues)
        print(f"Fixing {total_files} files...")
        print()
        
        for i, rel_filepath in enumerate(sorted(self.issues.keys()), 1):
            print(f"[{i}/{total_files}] Fixing {rel_filepath}...", end=' ')
            
            if self.fix_file(rel_filepath):
                print("✓")
            else:
                print("✗")
        
        print()
        print("=" * 80)
        print("Fix Summary")
        print("=" * 80)
        print(f"Files modified: {len(self.files_modified)}")
        
        total_posix = sum(v['posix_headers'] for v in self.fixes_applied.values())
        total_alt_ops = sum(v['alt_operators'] for v in self.fixes_applied.values())
        
        print(f"POSIX headers fixed: {total_posix}")
        print(f"Alternative operators fixed: {total_alt_ops}")
        print(f"Total fixes: {total_posix + total_alt_ops}")
        print()
    
    def generate_report(self):
        """Generate report of fixes applied."""
        report = []
        report.append("=" * 80)
        report.append("Windows Compatibility Fixes Applied")
        report.append("=" * 80)
        report.append("")
        report.append(f"Files modified: {len(self.files_modified)}")
        report.append("")
        
        total_posix = sum(v['posix_headers'] for v in self.fixes_applied.values())
        total_alt_ops = sum(v['alt_operators'] for v in self.fixes_applied.values())
        
        report.append(f"Fixes by type:")
        report.append(f"  POSIX headers: {total_posix}")
        report.append(f"  Alternative operators: {total_alt_ops}")
        report.append(f"  Total: {total_posix + total_alt_ops}")
        report.append("")
        report.append("=" * 80)
        report.append("")
        
        # Group by component
        by_component = defaultdict(lambda: {'files': 0, 'posix': 0, 'alt_ops': 0})
        
        for filepath, fixes in self.fixes_applied.items():
            component = filepath.split('/')[0] if '/' in filepath else 'root'
            by_component[component]['files'] += 1
            by_component[component]['posix'] += fixes['posix_headers']
            by_component[component]['alt_ops'] += fixes['alt_operators']
        
        report.append("Fixes by component:")
        report.append("")
        
        for component in sorted(by_component.keys()):
            stats = by_component[component]
            total = stats['posix'] + stats['alt_ops']
            report.append(f"  {component}:")
            report.append(f"    Files: {stats['files']}")
            report.append(f"    POSIX headers: {stats['posix']}")
            report.append(f"    Alternative operators: {stats['alt_ops']}")
            report.append(f"    Total: {total}")
            report.append("")
        
        return "\n".join(report)

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 auto_fix_windows_issues.py <root_directory> <json_report>")
        sys.exit(1)
    
    root_dir = sys.argv[1]
    json_report = sys.argv[2]
    
    if not os.path.isdir(root_dir):
        print(f"Error: {root_dir} is not a directory")
        sys.exit(1)
    
    if not os.path.isfile(json_report):
        print(f"Error: {json_report} is not a file")
        sys.exit(1)
    
    print("=" * 80)
    print("Automated Windows Compatibility Fixer")
    print("=" * 80)
    print()
    print(f"Root directory: {root_dir}")
    print(f"JSON report: {json_report}")
    print()
    
    fixer = WindowsCompatibilityFixer(root_dir, json_report)
    fixer.fix_all()
    
    # Generate and save report
    report = fixer.generate_report()
    print(report)
    
    with open('windows_fixes_applied.txt', 'w') as f:
        f.write(report)
    
    print("=" * 80)
    print("Report saved: windows_fixes_applied.txt")
    print("=" * 80)

if __name__ == '__main__':
    main()
