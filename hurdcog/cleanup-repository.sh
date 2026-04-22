#!/bin/bash
# GNU Hurd Cognitive Architecture Repository Cleanup Script
# This script helps reorganize the repository to focus on core mission

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Function to log with timestamp
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1"
}

# Function to handle errors
error_exit() {
    log "ERROR: $1"
    exit 1
}

# Function to backup before deletion
backup_file() {
    local file="$1"
    local backup_dir="backup/$(date '+%Y%m%d_%H%M%S')"
    
    if [ -f "$file" ]; then
        mkdir -p "$backup_dir"
        cp "$file" "$backup_dir/"
        log "Backed up $file to $backup_dir/"
    fi
}

log "Starting GNU Hurd Cognitive Architecture repository cleanup..."

# Create backup directory
mkdir -p backup
log "Created backup directory: backup/"

# Phase 1: Remove Financial Intelligence Components
log "Phase 1: Removing financial intelligence components..."

# Remove financial GitHub Actions workflows
if [ -f ".github/workflows/financial-intelligence-engine.yml" ]; then
    backup_file ".github/workflows/financial-intelligence-engine.yml"
    rm ".github/workflows/financial-intelligence-engine.yml"
    log "Removed financial-intelligence-engine.yml"
fi

# Clean cogsplit.yml (remove financial parts, keep cognitive parts)
if [ -f ".github/workflows/cogsplit.yml" ]; then
    backup_file ".github/workflows/cogsplit.yml"
    log "Backed up cogsplit.yml - needs manual review to extract cognitive components"
fi

# Fix ci-tests.yml (remove GnuCash references)
if [ -f ".github/workflows/ci-tests.yml" ]; then
    backup_file ".github/workflows/ci-tests.yml"
    log "Backed up ci-tests.yml - needs manual review to replace GnuCash with Hurd-specific tests"
fi

# Phase 2: Remove ElizaOS Components
log "Phase 2: Identifying ElizaOS components for separation..."

# Find ElizaOS related files
find . -name "*.md" -exec grep -l "ElizaOS\|eliza" {} \; 2>/dev/null | while read file; do
    log "Found ElizaOS reference in: $file"
done

# Phase 3: Clean Documentation
log "Phase 3: Cleaning documentation..."

# Remove financial references from documentation
find docs/ -name "*.md" -exec grep -l "financial\|trading\|investment\|market" {} \; 2>/dev/null | while read file; do
    log "Found financial content in: $file"
done

# Phase 4: Create Clean Structure
log "Phase 4: Creating clean repository structure..."

# Create new directory structure
mkdir -p {cognitive,distributed,performance,development,build}

# Move cognitive-related documentation
if [ -d "docs" ]; then
    mkdir -p cognitive/docs
    # Move OpenCog, cognitive analysis files
    find docs/ -name "*OpenCog*" -o -name "*cognitive*" -o -name "*AtomSpace*" | while read file; do
        if [ -f "$file" ]; then
            cp "$file" "cognitive/docs/"
            log "Moved cognitive doc: $file"
        fi
    done
fi

# Move distributed systems documentation
if [ -d "docs" ]; then
    mkdir -p distributed/docs
    # Move Plan9, Inferno files
    find docs/ -name "*Plan9*" -o -name "*Inferno*" -o -name "*distributed*" | while read file; do
        if [ -f "$file" ]; then
            cp "$file" "distributed/docs/"
            log "Moved distributed doc: $file"
        fi
    done
fi

# Move performance documentation
if [ -d "docs" ]; then
    mkdir -p performance/docs
    # Move Kokkos files
    find docs/ -name "*Kokkos*" -o -name "*performance*" | while read file; do
        if [ -f "$file" ]; then
            cp "$file" "performance/docs/"
            log "Moved performance doc: $file"
        fi
    done
fi

# Move development tools documentation
if [ -d "docs" ]; then
    mkdir -p development/docs
    # Move Compiler Explorer, Theia files
    find docs/ -name "*Compiler*" -o -name "*Theia*" -o -name "*development*" | while read file; do
        if [ -f "$file" ]; then
            cp "$file" "development/docs/"
            log "Moved development doc: $file"
        fi
    done
fi

# Move build system documentation
if [ -d "docs" ]; then
    mkdir -p build/docs
    # Move Guix, Mes files
    find docs/ -name "*Guix*" -o -name "*Mes*" -o -name "*build*" | while read file; do
        if [ -f "$file" ]; then
            cp "$file" "build/docs/"
            log "Moved build doc: $file"
        fi
    done
fi

# Phase 5: Create Clean README
log "Phase 5: Creating clean README..."

cat > README_CLEAN.md << 'EOF'
# GNU Hurd Cognitive Architecture

**Project:** GNU Hurd Cognitive Microkernel Operating System  
**Version:** 2.0 - Clean Architecture Focus  
**Status:** Reorganization Phase

## Overview

This repository represents the world's first cognitive microkernel operating system, integrating GNU Hurd's modular architecture with advanced cognitive computing frameworks. The project aims to solve 350+ open GNU Hurd issues through intelligent, self-optimizing system components.

## Core Vision

Transform GNU Hurd from a traditional microkernel OS into a cognitive operating system that can:
- **Self-diagnose** and **self-heal** system issues
- **Optimize performance** through machine learning
- **Adapt** to changing workloads and hardware
- **Learn** from system behavior patterns
- **Coordinate** distributed resources intelligently

## Repository Structure

```
├── cognitive/          # OpenCog, AI frameworks, cognitive components
├── distributed/        # Plan9, Inferno, distributed systems
├── performance/        # Kokkos, optimization, high-performance computing
├── development/        # Compiler Explorer, Theia, development tools
├── build/             # GNU Guix, GNU Mes, build systems
├── hurd-ecosystem/    # Core GNU Hurd components
├── docs/              # General documentation
└── backup/            # Backup of removed components
```

## Framework Integration

### Core Operating System
- **GNU Hurd**: Microkernel-based operating system foundation
- **GNU Mach**: Microkernel providing core system services
- **MIG**: Interface generator for IPC

### Cognitive Architecture
- **OpenCog**: Artificial General Intelligence framework
- **AtomSpace**: Hypergraph database for knowledge representation
- **CogServer**: Distributed cognitive processing

### Distributed Systems
- **Plan9**: Distributed operating system with 9P protocol
- **Inferno**: Virtual machine with Limbo programming language
- **SingularityNET**: Distributed AI marketplace

### Performance & Development
- **Kokkos**: Performance portability programming ecosystem
- **Compiler Explorer**: Interactive compilation analysis
- **Theia**: Custom development environment framework

### Build & Package Management
- **GNU Guix**: Declarative, transactional package management
- **GNU Mes**: Scheme interpreter and C compiler for bootstrapping

## Development Phases

### Phase 1: Foundation & Cleanup (Months 1-3)
- Repository cleanup and reorganization
- Core GNU Hurd integration
- Development environment setup

### Phase 2: Cognitive & Distributed Infrastructure (Months 4-6)
- Plan9 and Inferno integration
- OpenCog cognitive architecture
- GNU Guix build system

### Phase 3: Advanced Features & Optimization (Months 7-9)
- SingularityNET distributed AI services
- Kokkos performance optimization
- Compiler Explorer development tools

### Phase 4: Development Environment & Community (Months 10-12)
- Theia IDE framework
- AI model ecosystem
- Community development tools

## Getting Started

### Prerequisites
- GCC configured for i686-gnu target
- GNU Make and Autotools
- Git for development

### Building
```bash
# Configure the build
./configure --host=i686-gnu

# Build the complete system
make

# Install (requires proper Hurd environment)
make install
```

## Contributing

### Development Areas
- **Performance**: IPC optimization, memory management
- **Hardware Support**: Device drivers, architecture ports
- **Security**: Capability system enhancements
- **Testing**: Automated testing framework
- **Documentation**: Improved developer guides

### Getting Started
1. Read the [Development Roadmap](DEVELOPMENT_ROADMAP.md)
2. Choose a component to work on
3. Follow existing code style and conventions
4. Add tests for new functionality
5. Update documentation as needed

## Resources

- **Project Website**: <http://www.gnu.org/software/hurd/>
- **Development Roadmap**: [DEVELOPMENT_ROADMAP.md](DEVELOPMENT_ROADMAP.md)
- **Architecture Overview**: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Bug Reports**: <bug-hurd@gnu.org>
- **Help**: <help-hurd@gnu.org>
- **IRC**: #hurd on libera.chat

## License

The GNU Hurd is free software. All components are covered by the GNU General Public License. See [COPYING](COPYING) for details.

---

*The future of operating systems is cognitive, and it starts with GNU Hurd.*
EOF

log "Created clean README: README_CLEAN.md"

# Phase 6: Create Action Items List
log "Phase 6: Creating action items list..."

cat > CLEANUP_ACTION_ITEMS.md << 'EOF'
# Repository Cleanup Action Items

## Immediate Actions Required

### 1. Remove Financial Components ❌
- [ ] Delete `.github/workflows/financial-intelligence-engine.yml`
- [ ] Clean `.github/workflows/cogsplit.yml` (extract cognitive parts, remove financial)
- [ ] Fix `.github/workflows/ci-tests.yml` (replace GnuCash with Hurd-specific tests)
- [ ] Remove all financial references from documentation

### 2. Separate ElizaOS Components ⚠️
- [ ] Create separate repository for ElizaOS components
- [ ] Move ElizaOS-related files to new repository
- [ ] Update documentation to remove ElizaOS references
- [ ] Clean up any remaining ElizaOS dependencies

### 3. Reorganize Documentation 📚
- [ ] Move cognitive docs to `cognitive/docs/`
- [ ] Move distributed system docs to `distributed/docs/`
- [ ] Move performance docs to `performance/docs/`
- [ ] Move development docs to `development/docs/`
- [ ] Move build system docs to `build/docs/`
- [ ] Update all documentation links

### 4. Clean GitHub Actions 🔧
- [ ] Remove financial intelligence engine workflow
- [ ] Clean cogsplit workflow (keep cognitive, remove financial)
- [ ] Fix ci-tests workflow (Hurd-specific tests)
- [ ] Update workflow documentation

### 5. Update Core Files 📝
- [ ] Replace README.md with README_CLEAN.md
- [ ] Update DEVELOPMENT_ROADMAP.md
- [ ] Clean up any remaining financial references
- [ ] Update project description and tags

## Verification Checklist

### Before Committing Changes
- [ ] No financial/trading references remain
- [ ] No ElizaOS components in main repository
- [ ] All documentation properly organized
- [ ] GitHub Actions workflows cleaned
- [ ] Build system functional
- [ ] Tests pass
- [ ] Documentation links updated

### After Cleanup
- [ ] Repository focuses on GNU Hurd + Cognitive Architecture
- [ ] Clear separation of concerns
- [ ] Proper documentation structure
- [ ] Clean development workflow
- [ ] Community guidelines updated

## Notes

- All removed files are backed up in `backup/` directory
- Review backup before permanent deletion
- Consider creating separate repositories for removed components
- Update all documentation to reflect new structure
EOF

log "Created action items: CLEANUP_ACTION_ITEMS.md"

# Phase 7: Summary
log "Phase 7: Cleanup summary..."

echo ""
echo "=========================================="
echo "GNU Hurd Cognitive Architecture Cleanup"
echo "=========================================="
echo ""
echo "✅ Backup created: backup/"
echo "✅ Clean README created: README_CLEAN.md"
echo "✅ Action items created: CLEANUP_ACTION_ITEMS.md"
echo "✅ Directory structure created:"
echo "   - cognitive/"
echo "   - distributed/"
echo "   - performance/"
echo "   - development/"
echo "   - build/"
echo ""
echo "📋 Next Steps:"
echo "1. Review CLEANUP_ACTION_ITEMS.md"
echo "2. Manually remove financial components"
echo "3. Separate ElizaOS components"
echo "4. Update documentation structure"
echo "5. Test build system"
echo ""
echo "🎯 Goal: Focus on GNU Hurd + Cognitive Architecture"
echo ""

log "Cleanup script completed successfully!"
log "Review CLEANUP_ACTION_ITEMS.md for next steps"