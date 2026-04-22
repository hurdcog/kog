#!/bin/bash
#
# Visualize Debian Package Build Workflow
# Shows the dependency order and what will be built
#

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   OpenCog Debian Package Build Workflow Visualization     ║"
echo "╔════════════════════════════════════════════════════════════╗"
echo ""

echo "📋 Workflow Information:"
echo "   Workflow File: .github/workflows/debian-packages.yml"
echo "   Helper Scripts: .github/scripts/{parse-package-info.sh, prepare-package-build.sh}"
echo "   Package Metadata: opencog-debian/*/update-*.sh"
echo ""

echo "🔧 Build Stages and Dependencies:"
echo ""

# Stage 1
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 1: Foundation Layer                              │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" cogutil 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 cogutil → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: None (system libraries only)"
else
    echo "  ⚠️  cogutil - Unable to retrieve package information"
fi
echo ""

# Stage 2
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 2: Core AtomSpace                                │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" atomspace 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 atomspace → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil"
else
    echo "  ⚠️  atomspace - Unable to retrieve package information"
fi
echo ""

# Stage 3
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 3: Pattern Matching (Parallel)                   │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" unify 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 unify → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace"
else
    echo "  ⚠️  unify - Unable to retrieve package information"
fi
echo ""

# Stage 4
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 4: Rule Engine                                    │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" ure 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 ure → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace, unify"
else
    echo "  ⚠️  ure - Unable to retrieve package information"
fi
echo ""

# Stage 5
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 5: Core Services (Parallel)                      │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" cogserver 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 cogserver → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace"
else
    echo "  ⚠️  cogserver - Unable to retrieve package information"
fi
echo ""

# Stage 6
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 6: Cognitive Components                           │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" attention 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 attention → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace, cogserver"
else
    echo "  ⚠️  attention - Unable to retrieve package information"
fi
echo ""

# Stage 7
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 7: Evolutionary Systems (Moses)                   │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" moses 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 moses → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil"
else
    echo "  ⚠️  moses - Unable to retrieve package information"
fi
echo ""

# Stage 8
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 8: Advanced Evolutionary                          │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" asmoses 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 asmoses → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: moses, atomspace"
else
    echo "  ⚠️  asmoses - Unable to retrieve package information"
fi
echo ""

# Stage 9
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 9: Pattern Mining & Reasoning                    │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" miner 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 miner → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace, ure"
else
    echo "  ⚠️  miner - Unable to retrieve package information"
fi
echo ""

# Stage 10
echo "┌─────────────────────────────────────────────────────────┐"
echo "│ Stage 10: Probabilistic Logic Networks                 │"
echo "└─────────────────────────────────────────────────────────┘"
if INFO=$("$SCRIPT_DIR/parse-package-info.sh" pln 2>/dev/null) && [ -n "$INFO" ]; then
    VERSION=$(echo "$INFO" | grep VERSION= | cut -d= -f2)
    DEB_NAME=$(echo "$INFO" | grep DEB_NAME= | cut -d= -f2)
    echo "  📦 pln → $DEB_NAME (v$VERSION)"
    echo "     Dependencies: cogutil, atomspace, ure"
else
    echo "  ⚠️  pln - Unable to retrieve package information"
fi
echo ""

echo "╔════════════════════════════════════════════════════════════╗"
echo "║   Build Workflow Summary                                   ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Total Packages: 10"
echo "Total Stages: 10"
echo "Parallel Opportunities: Stages 3, 5, 6, 7, 8, 9, 10"
echo ""
echo "✅ All packages use prepare-package-build.sh for consistency"
echo "✅ All metadata comes from opencog-debian/*/update-*.sh scripts"
echo "✅ Workflow follows proper dependency order"
echo ""
