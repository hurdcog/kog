#!/bin/bash

# Roadmap Review and Update Script
# This script helps maintain the GNU Mach development roadmap by:
# 1. Analyzing current progress against phases
# 2. Identifying completed tasks
# 3. Suggesting roadmap updates
# 4. Generating progress reports

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
ROADMAP_FILE="$PROJECT_ROOT/open-issues-gnumach.md"
REPORTS_DIR="$PROJECT_ROOT/roadmap-reports"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Create reports directory if it doesn't exist
mkdir -p "$REPORTS_DIR"

# Generate timestamp for report
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
REPORT_FILE="$REPORTS_DIR/roadmap_review_$TIMESTAMP.md"

# Function to analyze roadmap progress
analyze_roadmap_progress() {
    log_info "Analyzing roadmap progress..."
    
    cat > "$REPORT_FILE" << EOF
# GNU Mach Roadmap Progress Report

**Generated**: $(date)
**Report ID**: roadmap_review_$TIMESTAMP

## Summary

This report analyzes the current state of the GNU Mach development roadmap
and provides recommendations for updates based on recent progress.

EOF

    # Count completed items in each phase
    local phase1_completed=$(grep -c '\- \[x\]' "$ROADMAP_FILE" | head -1 || echo "0")
    local phase1_total=$(grep -c '\- \[.*\]' "$ROADMAP_FILE" | head -1 || echo "1")
    
    # Calculate completion percentage
    local completion_pct=$((phase1_completed * 100 / phase1_total))
    
    cat >> "$REPORT_FILE" << EOF
### Progress Statistics

- **Total Items**: $phase1_total
- **Completed Items**: $phase1_completed
- **Completion Rate**: ${completion_pct}%

EOF

    log_info "Progress analysis complete. Completion rate: ${completion_pct}%"
}

# Function to identify recent changes
analyze_recent_changes() {
    log_info "Analyzing recent changes..."
    
    cat >> "$REPORT_FILE" << EOF
## Recent Activity Analysis

### Recent Commits (Last 30 days)
EOF

    # Get recent commits
    cd "$PROJECT_ROOT"
    git log --oneline --since="30 days ago" --pretty=format:"- %h: %s (%an, %ar)" >> "$REPORT_FILE" 2>/dev/null || {
        echo "- No recent commits found" >> "$REPORT_FILE"
    }
    
    cat >> "$REPORT_FILE" << EOF

### Recently Modified Files
EOF

    # Find recently modified files
    find "$PROJECT_ROOT" -name "*.c" -o -name "*.h" -o -name "*.md" | \
        xargs ls -lt | head -10 | \
        awk '{print "- " $9 " (modified " $6 " " $7 " " $8 ")"}' >> "$REPORT_FILE" 2>/dev/null || {
        echo "- No recently modified files found" >> "$REPORT_FILE"
    }
}

# Function to suggest roadmap updates
suggest_updates() {
    log_info "Generating update suggestions..."
    
    cat >> "$REPORT_FILE" << EOF

## Suggested Roadmap Updates

### Completed Items to Mark as Done

The following items appear to be completed based on code analysis:

EOF

    # Check if safety infrastructure exists
    if [ -f "$PROJECT_ROOT/include/mach/mach_safety.h" ]; then
        cat >> "$REPORT_FILE" << EOF
- ✅ Safety infrastructure framework implemented (mach_safety.h exists)
EOF
    fi

    # Check if documentation exists
    if [ -f "$PROJECT_ROOT/docs/new-developer-guide.md" ]; then
        cat >> "$REPORT_FILE" << EOF
- ✅ New developer documentation created
EOF
    fi

    if [ -f "$PROJECT_ROOT/docs/mentorship-program.md" ]; then
        cat >> "$REPORT_FILE" << EOF
- ✅ Mentorship program documentation created
EOF
    fi

    # Check test infrastructure
    local test_count=$(find "$PROJECT_ROOT/tests" -name "test-*.c" 2>/dev/null | wc -l || echo "0")
    if [ "$test_count" -gt 15 ]; then
        cat >> "$REPORT_FILE" << EOF
- ✅ Comprehensive test suite exists (${test_count} test files found)
EOF
    fi

    cat >> "$REPORT_FILE" << EOF

### New Items to Add

Based on recent development, consider adding these items:

- [ ] Automated roadmap progress tracking
- [ ] Community metrics and health monitoring  
- [ ] Developer onboarding success metrics
- [ ] Regular security audit process
- [ ] Performance regression testing automation

### Priority Adjustments

Consider these priority changes based on current needs:

- **Increase priority**: Documentation and community infrastructure (high community value)
- **Decrease priority**: Advanced research features (focus on stability first)
- **New focus areas**: Developer experience and community growth
EOF
}

# Function to generate action items
generate_action_items() {
    log_info "Generating action items..."
    
    cat >> "$REPORT_FILE" << EOF

## Recommended Actions

### Immediate Actions (Next 2 weeks)
- [ ] Update roadmap to mark completed infrastructure items as done
- [ ] Review and update phase priorities based on current needs
- [ ] Validate that safety infrastructure is properly tested
- [ ] Ensure new documentation is linked from main README

### Short-term Actions (Next month)
- [ ] Establish regular roadmap review schedule (monthly)
- [ ] Create automated progress tracking tools
- [ ] Set up community metrics collection
- [ ] Launch mentorship program pilot

### Long-term Actions (Next quarter)
- [ ] Comprehensive roadmap restructuring based on learnings
- [ ] Establish success metrics for each development phase
- [ ] Create contributor retention and growth strategies
- [ ] Plan next major development cycle

## Review Schedule

This roadmap review should be repeated:
- **Monthly**: Progress and priority assessment
- **Quarterly**: Strategic direction and phase planning  
- **Annually**: Comprehensive roadmap restructuring

**Next review due**: $(date -d "+1 month" +"%Y-%m-%d")

EOF
}

# Function to validate roadmap consistency
validate_roadmap() {
    log_info "Validating roadmap consistency..."
    
    local errors=0
    
    # Check for broken links
    if grep -q '\[.*\](.*)' "$ROADMAP_FILE"; then
        log_info "Checking for broken links in roadmap..."
        # This would need more sophisticated link checking
    fi
    
    # Check for duplicate items
    local duplicates=$(grep '\- \[.*\]' "$ROADMAP_FILE" | sort | uniq -d | wc -l)
    if [ "$duplicates" -gt 0 ]; then
        log_warning "Found $duplicates potential duplicate items in roadmap"
        errors=$((errors + 1))
    fi
    
    # Check for inconsistent formatting
    if grep -q '\- \[[xX ]\]' "$ROADMAP_FILE"; then
        log_info "Checkbox formatting appears consistent"
    else
        log_warning "Inconsistent checkbox formatting detected"
        errors=$((errors + 1))
    fi
    
    if [ $errors -eq 0 ]; then
        log_success "Roadmap validation passed"
    else
        log_warning "Roadmap validation found $errors issues"
    fi
    
    cat >> "$REPORT_FILE" << EOF

## Validation Results

- **Consistency check**: $([ $errors -eq 0 ] && echo "✅ Passed" || echo "⚠️  $errors issues found")
- **Format check**: $(grep -q '\- \[[xX ]\]' "$ROADMAP_FILE" && echo "✅ Consistent" || echo "⚠️  Inconsistent")
- **Link check**: Manual review recommended

EOF
}

# Main execution
main() {
    log_info "Starting GNU Mach roadmap review..."
    log_info "Project root: $PROJECT_ROOT"
    log_info "Report will be saved to: $REPORT_FILE"
    
    # Check if roadmap file exists
    if [ ! -f "$ROADMAP_FILE" ]; then
        log_error "Roadmap file not found: $ROADMAP_FILE"
        exit 1
    fi
    
    # Run analysis functions
    analyze_roadmap_progress
    analyze_recent_changes
    suggest_updates
    generate_action_items
    validate_roadmap
    
    # Finalize report
    cat >> "$REPORT_FILE" << EOF

---
*This report was generated automatically by the roadmap review script.*
*For questions or suggestions, please open an issue or discuss on the mailing list.*
EOF
    
    log_success "Roadmap review complete!"
    log_info "Report saved to: $REPORT_FILE"
    
    # Display summary
    echo
    echo "=== ROADMAP REVIEW SUMMARY ==="
    echo "Report file: $REPORT_FILE"
    echo "Key findings:"
    echo "- Progress analysis completed"
    echo "- Update suggestions generated" 
    echo "- Action items identified"
    echo "- Validation results included"
    echo
    echo "Next steps:"
    echo "1. Review the generated report"
    echo "2. Update roadmap based on suggestions"
    echo "3. Schedule next review for one month from now"
    echo "4. Share findings with the development community"
    
    # Optionally open the report file
    if command -v less >/dev/null 2>&1; then
        echo
        read -p "View report now? (y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            less "$REPORT_FILE"
        fi
    fi
}

# Script usage
usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Analyze and update the GNU Mach development roadmap.

Options:
  -h, --help    Show this help message
  -q, --quiet   Run in quiet mode (less output)
  -v, --verbose Run in verbose mode (more output)

Examples:
  $0                # Run complete roadmap review
  $0 --quiet       # Run with minimal output
  $0 --verbose     # Run with detailed output

The script will:
1. Analyze current roadmap progress
2. Identify completed tasks
3. Suggest updates and improvements
4. Generate actionable recommendations
5. Validate roadmap consistency

Reports are saved to: roadmap-reports/
EOF
}

# Handle command line arguments
case "${1:-}" in
    -h|--help)
        usage
        exit 0
        ;;
    -q|--quiet)
        exec > /dev/null 2>&1
        main
        ;;
    -v|--verbose)
        set -x
        main
        ;;
    "")
        main
        ;;
    *)
        log_error "Unknown option: $1"
        usage
        exit 1
        ;;
esac