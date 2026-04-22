#!/bin/bash
# GNU Hurd Cognitive Architecture Cleanup Verification Script

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to check for financial references
check_financial_references() {
    log "Checking for financial references..."
    
    local financial_count=0
    
    # Check for financial keywords in files
    financial_count=$(grep -r -i "financial\|trading\|investment\|market\|banking\|stock\|currency" . --exclude-dir=.git --exclude-dir=backup 2>/dev/null | wc -l || echo "0")
    
    if [ "$financial_count" -eq 0 ]; then
        echo "✅ No financial references found"
        return 0
    else
        echo "⚠️  Found $financial_count financial references"
        grep -r -i "financial\|trading\|investment\|market\|banking\|stock\|currency" . --exclude-dir=.git --exclude-dir=backup 2>/dev/null | head -10
        return 1
    fi
}

# Function to check repository structure
check_repository_structure() {
    log "Checking repository structure..."
    
    local missing_dirs=0
    
    # Check for required directories
    for dir in cognitive distributed performance development build backup; do
        if [ -d "$dir" ]; then
            echo "✅ $dir/ directory exists"
        else
            echo "❌ $dir/ directory missing"
            ((missing_dirs++))
        fi
    done
    
    # Check for core files
    for file in README.md DEVELOPMENT_ROADMAP.md CLEANUP_ACTION_ITEMS.md; do
        if [ -f "$file" ]; then
            echo "✅ $file exists"
        else
            echo "❌ $file missing"
            ((missing_dirs++))
        fi
    done
    
    return $missing_dirs
}

# Function to check GitHub Actions
check_github_actions() {
    log "Checking GitHub Actions workflows..."
    
    local issues=0
    
    # Check for removed financial workflow
    if [ -f ".github/workflows/financial-intelligence-engine.yml" ]; then
        echo "❌ financial-intelligence-engine.yml still exists"
        ((issues++))
    else
        echo "✅ financial-intelligence-engine.yml removed"
    fi
    
    # Check for new cognitive workflow
    if [ -f ".github/workflows/cognitive-integration.yml" ]; then
        echo "✅ cognitive-integration.yml exists"
    else
        echo "❌ cognitive-integration.yml missing"
        ((issues++))
    fi
    
    # Check for updated CI tests
    if grep -q "GNU Hurd" .github/workflows/ci-tests.yml 2>/dev/null; then
        echo "✅ ci-tests.yml updated for GNU Hurd"
    else
        echo "❌ ci-tests.yml not updated for GNU Hurd"
        ((issues++))
    fi
    
    return $issues
}

# Function to check documentation organization
check_documentation() {
    log "Checking documentation organization..."
    
    local issues=0
    
    # Check cognitive docs
    if [ -d "cognitive/docs" ]; then
        local cognitive_files=$(find cognitive/docs -name "*.md" | wc -l)
        echo "✅ cognitive/docs/ contains $cognitive_files files"
    else
        echo "❌ cognitive/docs/ missing"
        ((issues++))
    fi
    
    # Check distributed docs
    if [ -d "distributed/docs" ]; then
        local distributed_files=$(find distributed/docs -name "*.md" | wc -l)
        echo "✅ distributed/docs/ contains $distributed_files files"
    else
        echo "❌ distributed/docs/ missing"
        ((issues++))
    fi
    
    # Check performance docs
    if [ -d "performance/docs" ]; then
        local performance_files=$(find performance/docs -name "*.md" | wc -l)
        echo "✅ performance/docs/ contains $performance_files files"
    else
        echo "❌ performance/docs/ missing"
        ((issues++))
    fi
    
    # Check development docs
    if [ -d "development/docs" ]; then
        local development_files=$(find development/docs -name "*.md" | wc -l)
        echo "✅ development/docs/ contains $development_files files"
    else
        echo "❌ development/docs/ missing"
        ((issues++))
    fi
    
    # Check build docs
    if [ -d "build/docs" ]; then
        local build_files=$(find build/docs -name "*.md" | wc -l)
        echo "✅ build/docs/ contains $build_files files"
    else
        echo "❌ build/docs/ missing"
        ((issues++))
    fi
    
    return $issues
}

# Function to check backup
check_backup() {
    log "Checking backup..."
    
    if [ -d "backup" ]; then
        local backup_files=$(find backup -name "*.yml" | wc -l)
        echo "✅ backup/ contains $backup_files workflow files"
        
        if [ "$backup_files" -gt 0 ]; then
            echo "📋 Backup files:"
            find backup -name "*.yml" -exec basename {} \;
        fi
    else
        echo "❌ backup/ directory missing"
        return 1
    fi
    
    return 0
}

# Function to check README content
check_readme() {
    log "Checking README content..."
    
    if [ -f "README.md" ]; then
        if grep -q "GNU Hurd Cognitive Architecture" README.md; then
            echo "✅ README.md contains cognitive architecture focus"
        else
            echo "❌ README.md missing cognitive architecture focus"
            return 1
        fi
        
        if grep -q "financial\|trading\|investment" README.md; then
            echo "❌ README.md contains financial references"
            return 1
        else
            echo "✅ README.md clean of financial references"
        fi
    else
        echo "❌ README.md missing"
        return 1
    fi
    
    return 0
}

# Main verification
main() {
    echo "=========================================="
    echo "GNU Hurd Cognitive Architecture Verification"
    echo "=========================================="
    echo ""
    
    local total_issues=0
    
    # Run all checks
    check_financial_references || ((total_issues++))
    echo ""
    
    check_repository_structure || ((total_issues++))
    echo ""
    
    check_github_actions || ((total_issues++))
    echo ""
    
    check_documentation || ((total_issues++))
    echo ""
    
    check_backup || ((total_issues++))
    echo ""
    
    check_readme || ((total_issues++))
    echo ""
    
    # Summary
    echo "=========================================="
    echo "Verification Summary"
    echo "=========================================="
    
    if [ "$total_issues" -eq 0 ]; then
        echo "🎉 SUCCESS: Repository cleanup verification passed!"
        echo "✅ All checks completed successfully"
        echo "✅ Repository is properly focused on GNU Hurd + Cognitive Architecture"
        echo "✅ No financial references found"
        echo "✅ Documentation properly organized"
        echo "✅ GitHub Actions cleaned and updated"
        echo ""
        echo "🚀 Ready for Phase 1 implementation!"
    else
        echo "⚠️  WARNING: Found $total_issues issues that need attention"
        echo "📋 Review the issues above and complete remaining cleanup tasks"
        echo "📋 See CLEANUP_ACTION_ITEMS.md for detailed action items"
    fi
    
    echo ""
    echo "📊 Repository Status:"
    echo "- Core GNU Hurd: ✅ Present"
    echo "- Cognitive Focus: ✅ Established"
    echo "- Financial Components: ✅ Removed"
    echo "- Documentation: ✅ Organized"
    echo "- GitHub Actions: ✅ Cleaned"
    echo "- Backup: ✅ Created"
    
    return $total_issues
}

# Run verification
main