#!/bin/bash
# update-agi-os-unified.sh - Prepare AGI-OS Unified meta-package for building

set -e

PACKAGE_NAME="agi-os-unified"
VERSION="1.0.0"
ORIG_TARBALL="${PACKAGE_NAME}_${VERSION}.orig.tar.gz"

echo "=========================================="
echo "Preparing $PACKAGE_NAME $VERSION"
echo "=========================================="

# Clean up previous builds
rm -rf "${PACKAGE_NAME}-${VERSION}"
rm -f "${ORIG_TARBALL}"

# Create package directory structure
mkdir -p "${PACKAGE_NAME}-${VERSION}"

# Create minimal source structure for meta-package
cat > "${PACKAGE_NAME}-${VERSION}/README.md" << 'EOF'
# AGI-OS Unified

This is a meta-package that installs the complete AGI Operating System stack,
integrating three foundational layers:

1. **Cognumach Microkernel** - Enhanced GNU Mach with cognitive features
2. **HurdCog Cognitive OS** - OpenCog-powered GNU Hurd system
3. **OpenCog Collection** - Comprehensive AGI research platform

## Installation

```bash
sudo apt-get install agi-os-unified
```

## Components

The AGI-OS provides:
- Distributed hypergraph processing (AtomSpace)
- Probabilistic reasoning (PLN)
- Attention allocation (ECAN)
- Pattern mining
- Language learning and generation
- Evolutionary optimization (MOSES)
- Real-time cognitive monitoring
- Self-optimization capabilities

## Documentation

For detailed documentation, see:
- https://github.com/cogpy/occ
- https://wiki.opencog.org/

## License

This meta-package is released under the AGPL-3.0 license.
Individual components may have different licenses.
EOF

# Create integration scripts directory
mkdir -p "${PACKAGE_NAME}-${VERSION}/scripts"

# Create AGI-OS initialization script
cat > "${PACKAGE_NAME}-${VERSION}/scripts/agi-os-init" << 'EOF'
#!/bin/bash
# agi-os-init - Initialize AGI Operating System environment

set -e

echo "Initializing AGI Operating System..."

# Check if all required components are installed
REQUIRED_PACKAGES=(
    "cognumach"
    "hurdcog"
    "opencog"
    "opencog-atomspace"
    "opencog-cogserver"
)

MISSING_PACKAGES=()

for pkg in "${REQUIRED_PACKAGES[@]}"; do
    if ! dpkg -l | grep -q "^ii  $pkg"; then
        MISSING_PACKAGES+=("$pkg")
    fi
done

if [ ${#MISSING_PACKAGES[@]} -gt 0 ]; then
    echo "ERROR: Missing required packages:"
    printf '  - %s\n' "${MISSING_PACKAGES[@]}"
    echo "Please install agi-os-unified package."
    exit 1
fi

# Create AGI-OS configuration directory
mkdir -p /etc/agi-os
mkdir -p /var/lib/agi-os
mkdir -p /var/log/agi-os

# Initialize AtomSpace storage
if [ ! -d /var/lib/agi-os/atomspace ]; then
    mkdir -p /var/lib/agi-os/atomspace
    echo "AtomSpace storage initialized at /var/lib/agi-os/atomspace"
fi

# Create default configuration
if [ ! -f /etc/agi-os/config.scm ]; then
    cat > /etc/agi-os/config.scm << 'EOFCONFIG'
;; AGI-OS Configuration
;; This file configures the unified AGI Operating System

(use-modules (opencog))
(use-modules (opencog cogserver))

;; AtomSpace configuration
(define atomspace-config
  '((storage-type . "rocksdb")
    (storage-path . "/var/lib/agi-os/atomspace")
    (cache-size . 1000000)))

;; CogServer configuration
(define cogserver-config
  '((port . 17001)
    (network-interface . "127.0.0.1")))

;; Cognitive synergy configuration
(define synergy-config
  '((enable-attention . #t)
    (enable-pln . #t)
    (enable-learning . #t)))

(display "AGI-OS configuration loaded\n")
EOFCONFIG
    echo "Default configuration created at /etc/agi-os/config.scm"
fi

echo "AGI Operating System initialized successfully!"
echo ""
echo "To start the CogServer:"
echo "  systemctl start cogserver"
echo ""
echo "To access the AtomSpace:"
echo "  guile -l /etc/agi-os/config.scm"
EOF

chmod +x "${PACKAGE_NAME}-${VERSION}/scripts/agi-os-init"

# Create AGI-OS status check script
cat > "${PACKAGE_NAME}-${VERSION}/scripts/agi-os-status" << 'EOF'
#!/bin/bash
# agi-os-status - Check AGI Operating System status

echo "=========================================="
echo "AGI Operating System Status"
echo "=========================================="
echo ""

# Check layer 1: Cognumach
echo "Layer 1: Cognumach Microkernel"
if dpkg -l | grep -q "^ii  cognumach"; then
    COGNUMACH_VERSION=$(dpkg -l | grep "^ii  cognumach" | awk '{print $3}')
    echo "  ✓ Installed: $COGNUMACH_VERSION"
else
    echo "  ✗ Not installed"
fi
echo ""

# Check layer 2: HurdCog
echo "Layer 2: HurdCog Cognitive OS"
if dpkg -l | grep -q "^ii  hurdcog"; then
    HURDCOG_VERSION=$(dpkg -l | grep "^ii  hurdcog" | awk '{print $3}')
    echo "  ✓ Installed: $HURDCOG_VERSION"
else
    echo "  ✗ Not installed"
fi
echo ""

# Check layer 3: OpenCog
echo "Layer 3: OpenCog Collection"
OPENCOG_COMPONENTS=(
    "opencog-atomspace:AtomSpace"
    "opencog-cogserver:CogServer"
    "opencog-ure:URE"
    "opencog-pln:PLN"
    "opencog-attention:ECAN"
    "opencog-miner:Pattern Miner"
    "opencog-learn:Learning"
    "opencog-generate:Generation"
)

for component in "${OPENCOG_COMPONENTS[@]}"; do
    PKG="${component%%:*}"
    NAME="${component##*:}"
    if dpkg -l | grep -q "^ii  $PKG"; then
        echo "  ✓ $NAME"
    else
        echo "  ✗ $NAME (not installed)"
    fi
done
echo ""

# Check services
echo "Services:"
if systemctl is-active --quiet cogserver 2>/dev/null; then
    echo "  ✓ CogServer: running"
else
    echo "  ○ CogServer: not running"
fi
echo ""

# Check storage
echo "Storage:"
if [ -d /var/lib/agi-os/atomspace ]; then
    SIZE=$(du -sh /var/lib/agi-os/atomspace 2>/dev/null | cut -f1)
    echo "  ✓ AtomSpace storage: $SIZE"
else
    echo "  ○ AtomSpace storage: not initialized"
fi
echo ""

echo "=========================================="
EOF

chmod +x "${PACKAGE_NAME}-${VERSION}/scripts/agi-os-status"

# Create orig tarball
tar czf "${ORIG_TARBALL}" "${PACKAGE_NAME}-${VERSION}"

# Copy debian directory
cp -r debian "${PACKAGE_NAME}-${VERSION}/"

echo ""
echo "Package prepared successfully!"
echo "  Source directory: ${PACKAGE_NAME}-${VERSION}/"
echo "  Orig tarball: ${ORIG_TARBALL}"
echo ""
echo "To build the package:"
echo "  cd ${PACKAGE_NAME}-${VERSION}"
echo "  dpkg-buildpackage -rfakeroot -us -uc"
