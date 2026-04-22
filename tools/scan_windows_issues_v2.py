#!/usr/bin/env python3
"""
Windows Compatibility Scanner v2 for OpenCog Components

Improved version that:
- Skips comments (// and /* */)
- Skips string literals
- Skips license headers
- Only reports actual code issues
"""

import os
import re
import sys
from pathlib import Path
from collections import defaultdict
import json

class WindowsCompatibilityScanner:
    def __init__(self, root_dir):
        self.root_dir = Path(root_dir)
        self.issues = defaultdict(lambda: defaultdict(list))
        
        # POSIX headers that need Windows equivalents
        self.posix_headers = {
            'unistd.h': ['io.h', 'process.h'],
            'strings.h': ['string.h'],
            'sys/time.h': ['winsock2.h'],
            'sys/socket.h': ['winsock2.h'],
            'sys/select.h': ['winsock2.h'],
            'netinet/in.h': ['winsock2.h'],
            'arpa/inet.h': ['winsock2.h'],
            'netdb.h': ['winsock2.h'],
            'dirent.h': ['<filesystem> (C++17)'],
            'pwd.h': ['Windows API'],
            'grp.h': ['Windows API'],
            'sys/wait.h': ['Windows API'],
            'sys/mman.h': ['Windows API'],
            'dlfcn.h': ['Windows.h (LoadLibrary)'],
        }
        
        # C++ alternative operators
        self.alt_operators = {
            'not': '!',
            'and': '&&',
            'or': '||',
        }
        
    def remove_comments_and_strings(self, content):
        """Remove comments and string literals from code."""
        # Remove multi-line comments
        content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
        # Remove single-line comments
        content = re.sub(r'//.*?$', '', content, flags=re.MULTILINE)
        # Remove string literals
        content = re.sub(r'"(?:[^"\\]|\\.)*"', '""', content)
        content = re.sub(r"'(?:[^'\\]|\\.)*'", "''", content)
        return content
        
    def is_in_license_header(self, line_num):
        """Check if line is likely in license header (first 30 lines)."""
        return line_num <= 30
        
    def scan_file(self, filepath):
        """Scan a single file for Windows compatibility issues."""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                lines = content.split('\n')
                
            rel_path = filepath.relative_to(self.root_dir)
            
            # Remove comments and strings for operator checking
            clean_content = self.remove_comments_and_strings(content)
            clean_lines = clean_content.split('\n')
            
            # Check for POSIX headers (always check, even in headers)
            for line_num, line in enumerate(lines, 1):
                include_match = re.match(r'^\s*#include\s*[<"]([^>"]+)[>"]', line)
                if include_match:
                    header = include_match.group(1)
                    if header in self.posix_headers:
                        self.issues[str(rel_path)]['posix_headers'].append({
                            'line': line_num,
                            'header': header,
                            'windows_equiv': self.posix_headers[header],
                            'code': line.strip()
                        })
            
            # Check for alternative operators (only in clean code)
            for line_num, (orig_line, clean_line) in enumerate(zip(lines, clean_lines), 1):
                # Skip license headers
                if self.is_in_license_header(line_num):
                    continue
                    
                # Skip empty lines
                if not clean_line.strip():
                    continue
                
                for alt_op, std_op in self.alt_operators.items():
                    # Pattern: word boundary, operator, word boundary or (
                    patterns = [
                        r'\b' + re.escape(alt_op) + r'\s+\w',  # "not test"
                        r'\b' + re.escape(alt_op) + r'\s*\(',  # "not("
                        r'\(\s*' + re.escape(alt_op) + r'\s+', # "(not "
                        r'\s+' + re.escape(alt_op) + r'\s+',   # " not "
                    ]
                    
                    for pattern in patterns:
                        if re.search(pattern, clean_line):
                            # Additional filtering
                            if alt_op == 'not':
                                # Skip if part of "cannot", "not_", etc.
                                if re.search(r'\w' + re.escape(alt_op), clean_line):
                                    continue
                                if re.search(re.escape(alt_op) + r'_', clean_line):
                                    continue
                            
                            if alt_op == 'or':
                                # Skip "for", "error", "or_"
                                if re.search(r'for\s*\(', clean_line):
                                    continue
                                if 'error' in clean_line.lower():
                                    continue
                                if re.search(re.escape(alt_op) + r'_', clean_line):
                                    continue
                            
                            if alt_op == 'and':
                                # Skip "and_", "command"
                                if re.search(re.escape(alt_op) + r'_', clean_line):
                                    continue
                            
                            self.issues[str(rel_path)]['alt_operators'].append({
                                'line': line_num,
                                'operator': alt_op,
                                'replacement': std_op,
                                'code': orig_line.strip()
                            })
                            break  # Only report once per line
                        
        except Exception as e:
            print(f"Error scanning {filepath}: {e}", file=sys.stderr)
    
    def scan_directory(self, directory):
        """Recursively scan directory for C/C++ files."""
        extensions = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hh', '.hpp', '.hxx'}
        
        for filepath in directory.rglob('*'):
            if filepath.suffix in extensions:
                # Skip test files and examples
                if '/tests/' in str(filepath) or '/examples/' in str(filepath):
                    continue
                self.scan_file(filepath)
    
    def generate_report(self):
        """Generate human-readable report."""
        report = []
        report.append("=" * 80)
        report.append("Windows Compatibility Issues Report (Code Only)")
        report.append("=" * 80)
        report.append("")
        
        total_files = len(self.issues)
        total_posix_headers = sum(len(v['posix_headers']) for v in self.issues.values())
        total_alt_operators = sum(len(v['alt_operators']) for v in self.issues.values())
        
        report.append(f"Summary:")
        report.append(f"  Files with issues: {total_files}")
        report.append(f"  POSIX headers: {total_posix_headers}")
        report.append(f"  Alternative operators: {total_alt_operators}")
        report.append(f"  Total issues: {total_posix_headers + total_alt_operators}")
        report.append("")
        
        if total_files == 0:
            report.append("✅ No Windows compatibility issues found!")
            return "\n".join(report)
        
        report.append("=" * 80)
        report.append("")
        
        for filepath in sorted(self.issues.keys()):
            file_issues = self.issues[filepath]
            
            if not any(file_issues.values()):
                continue
                
            report.append(f"File: {filepath}")
            report.append("-" * 80)
            
            # POSIX headers
            if file_issues['posix_headers']:
                report.append(f"  POSIX Headers ({len(file_issues['posix_headers'])}):")
                for issue in file_issues['posix_headers']:
                    report.append(f"    Line {issue['line']}: {issue['header']}")
                    report.append(f"      → {', '.join(issue['windows_equiv'])}")
                report.append("")
            
            # Alternative operators
            if file_issues['alt_operators']:
                report.append(f"  Alternative Operators ({len(file_issues['alt_operators'])}):")
                # Group by operator type
                by_operator = defaultdict(list)
                for issue in file_issues['alt_operators']:
                    by_operator[issue['operator']].append(issue)
                
                for op in sorted(by_operator.keys()):
                    issues = by_operator[op]
                    report.append(f"    '{op}' → '{self.alt_operators[op]}' ({len(issues)} occurrences)")
                    for issue in issues[:5]:  # Show first 5
                        report.append(f"      Line {issue['line']}: {issue['code'][:60]}")
                    if len(issues) > 5:
                        report.append(f"      ... and {len(issues) - 5} more")
                report.append("")
            
            report.append("")
        
        return "\n".join(report)
    
    def get_statistics(self):
        """Get statistics about issues found."""
        stats = {
            'total_files': len(self.issues),
            'total_posix_headers': sum(len(v['posix_headers']) for v in self.issues.values()),
            'total_alt_operators': sum(len(v['alt_operators']) for v in self.issues.values()),
            'files_by_component': defaultdict(int),
        }
        
        for filepath in self.issues.keys():
            component = filepath.split('/')[0] if '/' in filepath else 'root'
            stats['files_by_component'][component] += 1
        
        return stats

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 scan_windows_issues_v2.py <directory>")
        sys.exit(1)
    
    root_dir = sys.argv[1]
    
    if not os.path.isdir(root_dir):
        print(f"Error: {root_dir} is not a directory")
        sys.exit(1)
    
    print(f"Scanning {root_dir} for Windows compatibility issues...")
    print()
    
    scanner = WindowsCompatibilityScanner(root_dir)
    scanner.scan_directory(Path(root_dir))
    
    # Generate and print report
    report = scanner.generate_report()
    print(report)
    
    # Save reports
    with open('windows_issues_report.txt', 'w') as f:
        f.write(report)
    
    with open('windows_issues_report.json', 'w') as f:
        f.write(json.dumps(dict(scanner.issues), indent=2))
    
    stats = scanner.get_statistics()
    
    print()
    print("=" * 80)
    print("Reports saved:")
    print("  - windows_issues_report.txt")
    print("  - windows_issues_report.json")
    print()
    
    if stats['files_by_component']:
        print("Issues by component:")
        for component, count in sorted(stats['files_by_component'].items()):
            print(f"  {component}: {count} files")
    
    total = stats['total_posix_headers'] + stats['total_alt_operators']
    print(f"\nTotal issues to fix: {total}")
    
    if total > 0:
        print(f"\n⚠️  Found {total} Windows compatibility issues that need fixing")
        print("   Run the auto-fixer to apply fixes automatically")
    else:
        print("\n✅ No issues found!")

if __name__ == '__main__':
    main()
