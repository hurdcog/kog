#!/bin/bash
set -e

PACKAGE_NAME="hurdcog-occ-bridge"
VERSION="1.0.0"
OCC_ROOT="../.."

echo "Preparing $PACKAGE_NAME source package..."

# Clean previous builds
rm -rf ${PACKAGE_NAME}-${VERSION}
rm -f ${PACKAGE_NAME}_${VERSION}.orig.tar.gz

# Create source directory
mkdir -p ${PACKAGE_NAME}-${VERSION}

# Copy Python bridge
echo "  Copying Python bridge..."
cp ${OCC_ROOT}/hurdcog-integration/atomspace-hurdcog-bridge.py ${PACKAGE_NAME}-${VERSION}/

# Copy Scheme interfaces
echo "  Copying Scheme interfaces..."
cp ${OCC_ROOT}/cognumach.scm ${PACKAGE_NAME}-${VERSION}/
cp ${OCC_ROOT}/hurdcog.scm ${PACKAGE_NAME}-${VERSION}/
cp ${OCC_ROOT}/occ-hurdcog-unified.scm ${PACKAGE_NAME}-${VERSION}/

# Copy AGI-OS synergy framework
echo "  Copying AGI-OS synergy framework..."
cp ${OCC_ROOT}/agi-os-synergy.scm ${PACKAGE_NAME}-${VERSION}/

# Create README
echo "  Creating README..."
cat > ${PACKAGE_NAME}-${VERSION}/README.md << 'EOF'
# HurdCog-OCC Integration Bridge

Integration layer connecting OpenCog Collection, HurdCog, and Cognumach for three-layer cognitive synergy.

## Overview

The HurdCog-OCC Bridge enables bidirectional communication and cognitive synergy between:
- **Layer 0**: Cognumach microkernel
- **Layer 1**: HurdCog cognitive operating system
- **Layer 2**: OpenCog Collection

## Components

### Python Bridge (atomspace-hurdcog-bridge.py)
Bidirectional communication between OpenCog AtomSpace and HurdCog MachSpace:
```python
from hurdcog import AtomSpaceHurdCogBridge

bridge = AtomSpaceHurdCogBridge()
bridge.connect()
bridge.send_atom(atom_id, atom_type, atom_data)
bridge.sync_atomspace()
```

### Scheme Interfaces

#### cognumach.scm
Microkernel cognitive primitives for Layer 0 integration.

#### hurdcog.scm
OS-level cognitive operations for Layer 1 integration.

#### occ-hurdcog-unified.scm
Unified integration framework coordinating all three layers.

### AGI-OS Synergy Framework (agi-os-synergy.scm)

Complete cognitive synergy implementation:
```scheme
(use-modules (agi-os synergy))

;; Initialize three-layer architecture
(agi-os-init)

;; Execute cognitive fusion
(agi-os-cognitive-fusion)

;; Check system status
(agi-os-layer-status)
```

## Cognitive Synergy Bridges

### Attention Bridge
Links ECAN (Economic Attention Networks) to system resource allocation.
High-importance atoms receive more OS-level resources.

### Learning Bridge
Mines patterns from system behavior and applies them to OS configuration.
Enables continuous OS adaptation through cognitive learning.

### Reasoning Bridge
Enables PLN (Probabilistic Logic Networks) inference at kernel level.
Kernel decisions informed by cognitive reasoning processes.

## Cognitive Fusion Reactor

Four-phase cross-layer cognitive process:
1. **Knowledge Gathering**: Collect from all three layers
2. **Knowledge Integration**: Unified AtomSpace representation
3. **Cross-Layer Reasoning**: PLN inference across layers
4. **Insight Application**: Update all layers with insights

## Usage

### Initialize AGI-OS
```bash
sudo agi-os-init
```

### Check Status
```bash
agi-os-status
```

### Python Integration
```python
from hurdcog import AtomSpaceHurdCogBridge
bridge = AtomSpaceHurdCogBridge()
bridge.connect()
bridge.sync_atomspace()
```

### Scheme Integration
```scheme
(use-modules (agi-os synergy))
(agi-os-init)
(machspace-sync)
```

## Documentation

See `/usr/share/doc/hurdcog-occ-bridge/` for complete documentation.

## License

AGPL-3.0+
EOF

# Copy debian directory
echo "  Copying debian packaging files..."
cp -r debian ${PACKAGE_NAME}-${VERSION}/

# Create source tarball
echo "  Creating source tarball..."
tar czf ${PACKAGE_NAME}_${VERSION}.orig.tar.gz ${PACKAGE_NAME}-${VERSION}

echo "✓ Source package prepared: ${PACKAGE_NAME}_${VERSION}.orig.tar.gz"
echo ""
echo "To build the package:"
echo "  cd ${PACKAGE_NAME}-${VERSION}"
echo "  dpkg-buildpackage -rfakeroot -us -uc"
