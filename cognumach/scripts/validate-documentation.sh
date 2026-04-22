#!/bin/bash
# Documentation Validation Script for GNU Mach Phase 3
# Validates documentation completeness and quality

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Default configuration
COMPLETENESS_CHECK=false
OUTPUT_DIR="$PROJECT_ROOT/test-results/documentation"
STRICT_MODE=false

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

usage() {
    cat << EOF
Usage: $0 [OPTIONS]

Validate documentation completeness and quality for GNU Mach.

Options:
  --completeness-check    Perform comprehensive completeness validation
  --strict               Use strict validation criteria
  --output-dir DIR       Output directory for validation results
  -h, --help            Show this help message

Examples:
  $0 --completeness-check
  $0 --completeness-check --strict
  $0 --completeness-check --output-dir=/tmp/doc-validation
EOF
}

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --completeness-check)
            COMPLETENESS_CHECK=true
            shift
            ;;
        --strict)
            STRICT_MODE=true
            shift
            ;;
        --output-dir=*)
            OUTPUT_DIR="${1#*=}"
            shift
            ;;
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            log_error "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Documentation requirements definition
declare -A REQUIRED_DOCS=(
    ["README"]="Project overview and basic instructions"
    ["CONTRIBUTING.md"]="Development contribution guidelines"
    ["NEXT_DEVELOPMENT_STEPS.md"]="Strategic development plan"
    ["DEVELOPMENT_PROGRESS_REPORT.md"]="Project progress assessment"
    ["build_instructions"]="How to build the system"
    ["testing_guide"]="How to run tests"
    ["performance_tuning"]="Performance optimization guide"
    ["production_deployment"]="Production deployment guide"
    ["debugging_guide"]="Advanced debugging scenarios"
)

declare -A OPTIONAL_DOCS=(
    ["AUTOTOOLS_MODERNIZATION.md"]="Build system modernization"
    ["PERFORMANCE_OPTIMIZATION_COMPLETE.md"]="Performance optimization details"
    ["SMP_IMPLEMENTATION_COMPLETE.md"]="SMP implementation guide"
    ["MACH5_RESEARCH_COMPLETE.md"]="Research implementation status"
    ["ROADMAP_IMPLEMENTATION_COMPLETE.md"]="Roadmap completion summary"
)

# Track validation results
declare -A validation_results
overall_doc_score=0
total_doc_checks=0

# Validate individual document
validate_document() {
    local doc_key="$1"
    local doc_description="$2"
    local is_required="$3"
    
    local doc_found=false
    local doc_quality="UNKNOWN"
    local doc_path=""
    
    # Search for document in multiple locations and formats
    local search_patterns=()
    case "$doc_key" in
        "README")
            search_patterns=("README" "README.md" "README.txt")
            ;;
        "build_instructions")
            search_patterns=("BUILD" "BUILD.md" "BUILDING" "BUILDING.md" "doc/building.md")
            ;;
        "testing_guide") 
            search_patterns=("TESTING" "TESTING.md" "doc/testing.md" "tests/README.md")
            ;;
        "performance_tuning")
            search_patterns=("PERFORMANCE" "PERFORMANCE.md" "doc/performance.md" "TUNING.md")
            ;;
        "production_deployment")
            search_patterns=("DEPLOYMENT" "DEPLOYMENT.md" "doc/deployment.md" "PRODUCTION.md")
            ;;
        "debugging_guide")
            search_patterns=("DEBUGGING" "DEBUGGING.md" "doc/debugging.md" "DEBUG.md")
            ;;
        *)
            search_patterns=("$doc_key")
            ;;
    esac
    
    # Search for document
    for pattern in "${search_patterns[@]}"; do
        if [[ -f "$PROJECT_ROOT/$pattern" ]]; then
            doc_found=true
            doc_path="$PROJECT_ROOT/$pattern"
            break
        fi
    done
    
    # Assess document quality if found
    if [[ "$doc_found" == "true" ]]; then
        local word_count
        word_count=$(wc -w < "$doc_path" 2>/dev/null || echo "0")
        local line_count
        line_count=$(wc -l < "$doc_path" 2>/dev/null || echo "0")
        
        if [[ $word_count -gt 500 && $line_count -gt 20 ]]; then
            doc_quality="HIGH"
            overall_doc_score=$((overall_doc_score + 3))
        elif [[ $word_count -gt 200 && $line_count -gt 10 ]]; then
            doc_quality="MEDIUM"
            overall_doc_score=$((overall_doc_score + 2))
        elif [[ $word_count -gt 50 ]]; then
            doc_quality="LOW"
            overall_doc_score=$((overall_doc_score + 1))
        else
            doc_quality="MINIMAL"
        fi
    fi
    
    total_doc_checks=$((total_doc_checks + 1))
    
    # Record result
    if [[ "$doc_found" == "true" ]]; then
        validation_results["$doc_key"]="FOUND:$doc_quality:$doc_path"
        
        if [[ "$is_required" == "true" ]]; then
            log_success "✅ Required: $doc_key ($doc_quality quality) - $doc_description"
        else
            log_info "📄 Optional: $doc_key ($doc_quality quality) - $doc_description"
        fi
    else
        validation_results["$doc_key"]="MISSING::$doc_description"
        
        if [[ "$is_required" == "true" ]]; then
            log_error "❌ Missing required: $doc_key - $doc_description"
        else
            log_warning "⚠️  Missing optional: $doc_key - $doc_description"
        fi
    fi
}

# Check for build instructions in README or separate files
validate_build_instructions() {
    log_info "Validating build instructions..."
    
    local build_info_found=false
    local build_quality="NONE"
    
    # Check README for build instructions
    if [[ -f "$PROJECT_ROOT/README" ]]; then
        if grep -q -i "build\|compile\|make\|configure" "$PROJECT_ROOT/README"; then
            build_info_found=true
            
            # Check for comprehensive build info
            if grep -q -i "autoreconf\|autotools\|./configure" "$PROJECT_ROOT/README"; then
                build_quality="HIGH"
            elif grep -q -i "make" "$PROJECT_ROOT/README"; then
                build_quality="MEDIUM"
            else
                build_quality="LOW"
            fi
        fi
    fi
    
    # Check for separate build documentation
    for build_file in "BUILD" "BUILD.md" "BUILDING" "BUILDING.md"; do
        if [[ -f "$PROJECT_ROOT/$build_file" ]]; then
            build_info_found=true
            local word_count
            word_count=$(wc -w < "$PROJECT_ROOT/$build_file" 2>/dev/null || echo "0")
            
            if [[ $word_count -gt 300 ]]; then
                build_quality="HIGH"
            elif [[ $word_count -gt 100 ]]; then
                build_quality="MEDIUM"
            else
                build_quality="LOW"
            fi
            break
        fi
    done
    
    validation_results["build_instructions"]="$([[ "$build_info_found" == "true" ]] && echo "FOUND" || echo "MISSING"):$build_quality:"
    
    if [[ "$build_info_found" == "true" ]]; then
        log_success "✅ Build instructions found ($build_quality quality)"
        overall_doc_score=$((overall_doc_score + 2))
    else
        log_error "❌ Build instructions not found"
    fi
    
    total_doc_checks=$((total_doc_checks + 1))
}

# Validate testing documentation
validate_testing_guide() {
    log_info "Validating testing documentation..."
    
    local testing_info_found=false
    local testing_quality="NONE"
    
    # Check for testing documentation
    for test_file in "TESTING" "TESTING.md" "tests/README.md" "doc/testing.md"; do
        if [[ -f "$PROJECT_ROOT/$test_file" ]]; then
            testing_info_found=true
            local word_count
            word_count=$(wc -w < "$PROJECT_ROOT/$test_file" 2>/dev/null || echo "0")
            
            if [[ $word_count -gt 400 ]]; then
                testing_quality="HIGH"
            elif [[ $word_count -gt 150 ]]; then
                testing_quality="MEDIUM"
            else
                testing_quality="LOW"
            fi
            break
        fi
    done
    
    # Check if README mentions testing
    if [[ "$testing_info_found" == "false" && -f "$PROJECT_ROOT/README" ]]; then
        if grep -q -i "test\|make check\|make run-" "$PROJECT_ROOT/README"; then
            testing_info_found=true
            testing_quality="LOW"
        fi
    fi
    
    validation_results["testing_guide"]="$([[ "$testing_info_found" == "true" ]] && echo "FOUND" || echo "MISSING"):$testing_quality:"
    
    if [[ "$testing_info_found" == "true" ]]; then
        log_success "✅ Testing documentation found ($testing_quality quality)"
        overall_doc_score=$((overall_doc_score + 2))
    else
        log_warning "⚠️  Testing documentation not found"
    fi
    
    total_doc_checks=$((total_doc_checks + 1))
}

# Check documentation consistency
validate_consistency() {
    log_info "Validating documentation consistency..."
    
    local consistency_issues=0
    
    # Check for broken internal links (simple check)
    if [[ -f "$PROJECT_ROOT/README" ]]; then
        while IFS= read -r line; do
            # Look for markdown links [text](url)
            if echo "$line" | grep -q '\[.*\](.*)'  ; then
                # Extract the URL part
                local link=$(echo "$line" | sed -n 's/.*\[\([^]]*\)\](\([^)]*\)).*/\2/p')
                if [[ -n "$link" && "$link" != http* && "$link" != "#"* ]]; then
                    if [[ ! -f "$PROJECT_ROOT/$link" ]]; then
                        log_warning "Broken link in README: $link"
                        ((consistency_issues++))
                    fi
                fi
            fi
        done < "$PROJECT_ROOT/README"
    fi
    
    # Check for version consistency
    local version_mentions=()
    for doc_file in README CONTRIBUTING.md NEXT_DEVELOPMENT_STEPS.md; do
        if [[ -f "$PROJECT_ROOT/$doc_file" ]]; then
            while IFS= read -r line; do
                if [[ "$line" =~ [vV]ersion.*[0-9]+\.[0-9]+ ]]; then
                    version_mentions+=("$doc_file: $line")
                fi
            done < "$PROJECT_ROOT/$doc_file"
        fi
    done
    
    if [[ ${#version_mentions[@]} -gt 1 ]]; then
        log_info "📋 Version mentions found - verify consistency:"
        for mention in "${version_mentions[@]}"; do
            echo "    $mention"
        done
    fi
    
    if [[ $consistency_issues -eq 0 ]]; then
        log_success "✅ Documentation consistency check passed"
        overall_doc_score=$((overall_doc_score + 1))
    else
        log_warning "⚠️  Found $consistency_issues consistency issues"
    fi
    
    total_doc_checks=$((total_doc_checks + 1))
}

# Generate documentation validation report
generate_documentation_report() {
    local report_file="$OUTPUT_DIR/documentation_validation_report_$(date +%Y%m%d_%H%M%S).txt"
    
    {
        echo "GNU Mach Documentation Validation Report"
        echo "========================================"
        echo "Date: $(date)"
        echo "Completeness check: $COMPLETENESS_CHECK"
        echo "Strict mode: $STRICT_MODE"
        echo ""
        
        echo "Documentation Assessment:"
        echo "========================"
        
        local found_count=0
        local missing_required=0
        local missing_optional=0
        
        for doc in "${!validation_results[@]}"; do
            local result="${validation_results[$doc]}"
            local status="${result%%:*}"
            local quality="${result#*:}"; quality="${quality%%:*}"
            local description="${result##*:}"
            
            if [[ "$status" == "FOUND" ]]; then
                ((found_count++))
                echo "  ✅ $doc: $status ($quality quality)"
            else
                echo "  ❌ $doc: $status - $description"
                
                # Check if it's a required document
                if [[ -n "${REQUIRED_DOCS[$doc]:-}" ]]; then
                    ((missing_required++))
                else
                    ((missing_optional++))
                fi
            fi
        done
        
        echo ""
        echo "Statistics:"
        echo "==========="
        echo "  Total documents checked: $total_doc_checks"
        echo "  Documents found: $found_count"
        echo "  Missing required: $missing_required"  
        echo "  Missing optional: $missing_optional"
        echo "  Overall documentation score: $overall_doc_score/$((total_doc_checks * 3))"
        
        local score_percentage=$((overall_doc_score * 100 / (total_doc_checks * 3)))
        echo "  Documentation completeness: $score_percentage%"
        
        echo ""
        echo "Quality Assessment:"
        echo "=================="
        
        if [[ $score_percentage -ge 80 ]]; then
            echo "  Overall Quality: EXCELLENT"
            echo "  Status: Ready for production"
        elif [[ $score_percentage -ge 60 ]]; then
            echo "  Overall Quality: GOOD"
            echo "  Status: Minor improvements recommended"
        elif [[ $score_percentage -ge 40 ]]; then
            echo "  Overall Quality: FAIR"
            echo "  Status: Significant improvements needed"
        else
            echo "  Overall Quality: POOR"
            echo "  Status: Major documentation work required"
        fi
        
        if [[ "$COMPLETENESS_CHECK" == "true" ]]; then
            echo ""
            echo "Completeness Recommendations:"
            echo "============================"
            
            if [[ $missing_required -gt 0 ]]; then
                echo "  HIGH PRIORITY:"
                for doc in "${!REQUIRED_DOCS[@]}"; do
                    if [[ "${validation_results[$doc]}" == MISSING* ]]; then
                        echo "    - Create $doc: ${REQUIRED_DOCS[$doc]}"
                    fi
                done
            fi
            
            if [[ $missing_optional -gt 0 ]]; then
                echo "  MEDIUM PRIORITY:"
                for doc in "${!OPTIONAL_DOCS[@]}"; do
                    if [[ "${validation_results[$doc]}" == MISSING* ]]; then
                        echo "    - Consider $doc: ${OPTIONAL_DOCS[$doc]}"
                    fi
                done
            fi
        fi
        
    } > "$report_file"
    
    log_info "Documentation validation report written to: $report_file"
}

# Main execution
main() {
    log_info "GNU Mach Documentation Validation"
    log_info "Completeness check: $COMPLETENESS_CHECK"
    log_info "Strict mode: $STRICT_MODE"
    log_info "Output directory: $OUTPUT_DIR"
    
    # Create output directory
    mkdir -p "$OUTPUT_DIR"
    
    # Validate required documents
    log_info "Validating required documentation..."
    for doc in "${!REQUIRED_DOCS[@]}"; do
        validate_document "$doc" "${REQUIRED_DOCS[$doc]}" "true"
    done
    
    # Validate optional documents if doing completeness check
    if [[ "$COMPLETENESS_CHECK" == "true" ]]; then
        log_info "Validating optional documentation..."
        for doc in "${!OPTIONAL_DOCS[@]}"; do
            validate_document "$doc" "${OPTIONAL_DOCS[$doc]}" "false"  
        done
    fi
    
    # Special validation for critical documentation
    validate_build_instructions
    validate_testing_guide
    
    # Check consistency
    validate_consistency
    
    # Generate report
    generate_documentation_report
    
    # Determine final result
    local score_percentage=$((overall_doc_score * 100 / (total_doc_checks * 3)))
    
    if [[ "$STRICT_MODE" == "true" ]]; then
        local min_score=80
    else
        local min_score=60
    fi
    
    if [[ $score_percentage -ge $min_score ]]; then
        log_success "Documentation validation PASSED ($score_percentage%)"
        log_success "Documentation is ready for production release"
        exit 0
    else
        log_error "Documentation validation FAILED ($score_percentage%)"
        log_error "Documentation improvements required before release"
        exit 1
    fi
}

# Run main function
main "$@"