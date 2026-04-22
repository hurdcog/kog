#!/bin/bash
set -e

PACKAGE_NAME="hurdcog-machspace"
VERSION="1.0.0"
HURDCOG_ROOT="../../hurdcog"

echo "Preparing $PACKAGE_NAME source package..."

# Clean previous builds
rm -rf ${PACKAGE_NAME}-${VERSION}
rm -f ${PACKAGE_NAME}_${VERSION}.orig.tar.gz

# Create source directory
mkdir -p ${PACKAGE_NAME}-${VERSION}

# Copy MachSpace core files from hurdcog/cogkernel
echo "  Copying MachSpace core files..."
cp ${HURDCOG_ROOT}/cogkernel/machspace.scm ${PACKAGE_NAME}-${VERSION}/
cp ${HURDCOG_ROOT}/cogkernel/cognitive-grip.scm ${PACKAGE_NAME}-${VERSION}/

# Copy Mach integration bridge
echo "  Copying Mach integration bridge..."
mkdir -p ${PACKAGE_NAME}-${VERSION}/mach-integration
cp ${HURDCOG_ROOT}/cogkernel/mach-integration/machspace-bridge.scm ${PACKAGE_NAME}-${VERSION}/mach-integration/

# Create README
echo "  Creating README..."
cat > ${PACKAGE_NAME}-${VERSION}/README.md << 'EOF'
# HurdCog MachSpace

MachSpace provides OS-level AtomSpace integration for the HurdCog cognitive operating system.

## Overview

MachSpace is a distributed hypergraph memory system that extends OpenCog's AtomSpace with Mach microkernel-specific features, enabling cognitive capabilities at the operating system level.

## Key Components

### machspace.scm
Distributed hypergraph memory implementation with:
- Mach port integration
- Hurd server/translator integration
- Cognitive IPC routing
- Bidirectional AtomSpace synchronization

### cognitive-grip.scm
The 5-finger cognitive grip mechanism:
1. **Thumb** (Universal Grip): AtomSpace integration
2. **Index** (Identity Pointing): Unique signatures
3. **Middle** (Coherence Strength): PLN validation
4. **Ring** (Trust Binding): Capability rings
5. **Pinky** (Resource Tracking): ECAN allocation

### mach-integration/machspace-bridge.scm
Low-level bridge between MachSpace and Mach IPC primitives.

## Usage

```scheme
(use-modules (hurdcog machspace))
(use-modules (hurdcog cognitive-grip))

;; Initialize MachSpace
(machspace-init)

;; Create cognitive grip on object
(define grip (cognitive-grip my-object))

;; Synchronize with AtomSpace
(machspace-sync)
```

## Integration

MachSpace is the foundational layer for HurdCog's cognitive architecture:
- Layer 0: Cognumach microkernel
- Layer 1: HurdCog OS + **MachSpace** ← You are here
- Layer 2: OpenCog Collection

## Documentation

See `/usr/share/doc/hurdcog-machspace/` for complete documentation.

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
