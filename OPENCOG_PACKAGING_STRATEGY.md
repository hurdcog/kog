# OpenCog Component Packaging Strategy

## Repository Structure Analysis

The OCC repository contains the following major components:

### Core Libraries (Already Packaged)
1. **cogutil** - Foundation utility library ✅
   - Location: `/cogutil/`
   - Package: `libcogutil`, `libcogutil-dev`
   - Status: Packaged and tested

2. **atomspace** - Hypergraph database ✅
   - Location: `/atomspace/`
   - Package: `libatomspace`, `libatomspace-dev`
   - Status: Packaging complete, build in progress

### Application Components (To Be Packaged)

3. **opencog** - Main framework with multiple subsystems
   - Location: `/opencog/`
   - Contains:
     - **GHOST** - Conversational AI (`/opencog/opencog/ghost/`)
     - **NLP** - Natural language processing (`/opencog/opencog/nlp/`)
     - **EVA** - Emotional/expressive animation (`/opencog/opencog/eva/`)
     - **OpenPsi** - Motivational system (`/opencog/opencog/openpsi/`)
     - **Neighbors** - Similarity/clustering (`/opencog/opencog/neighbors/`)

4. **pln** - Probabilistic Logic Networks
   - Location: `/pln/`
   - Contains:
     - **PLN Core** - Reasoning engine (`/pln/opencog/pln/`)
     - **TorchPLN** - PyTorch integration (`/pln/opencog/torchpln/`)

### Missing Components

5. **ECAN** - Economic Attention Networks
   - Status: Not found in current repository structure
   - May be integrated into atomspace or separate repository

## Packaging Approach

### Option 1: Monolithic Package (Recommended for Initial Release)

Create a single **libopencog** package that includes:
- GHOST conversational system
- NLP tools
- OpenPsi motivational system
- EVA animation
- All opencog framework components

**Advantages**:
- Simpler dependency management
- Easier for users to install
- Single version number
- Reduced packaging complexity

**Disadvantages**:
- Larger package size
- Users install components they may not need

### Option 2: Component-Based Packages (Future Enhancement)

Create separate packages:
- `libopencog-core` - Core framework
- `libopencog-ghost` - GHOST conversational AI
- `libopencog-nlp` - Natural language processing
- `libopencog-eva` - Emotional animation
- `libopencog-openpsi` - Motivational system
- `libpln` - Probabilistic reasoning
- `libpln-torch` - PyTorch integration

**Advantages**:
- Modular installation
- Smaller individual packages
- Users install only what they need

**Disadvantages**:
- Complex dependency chains
- More packages to maintain
- Version synchronization challenges

## Recommended Packaging Plan

### Phase 1: Core Packages (Current)
- ✅ `libcogutil` + `libcogutil-dev`
- ✅ `libatomspace` + `libatomspace-dev`

### Phase 2: Framework Package
- `libopencog` + `libopencog-dev`
  - Includes: GHOST, NLP, EVA, OpenPsi, Neighbors
  - Dependencies: libatomspace, libcogutil
  - Size estimate: ~5-10 MB

### Phase 3: Reasoning Package
- `libpln` + `libpln-dev`
  - Includes: PLN core reasoning engine
  - Dependencies: libatomspace, libcogutil
  - Size estimate: ~2-3 MB

### Phase 4: Optional Components
- `libpln-torch` + `libpln-torch-dev`
  - PyTorch integration for PLN
  - Dependencies: libpln, python3-torch
  - Size estimate: ~1-2 MB

## Package Dependencies

```
libcogutil (foundation)
    ↓
libatomspace (hypergraph)
    ↓
libopencog (framework: GHOST, NLP, EVA, OpenPsi)
    ↓
libpln (reasoning)
    ↓
libpln-torch (optional: PyTorch integration)
```

## Build Order

1. cogutil → atomspace → opencog → pln
2. Each component must be installed before building the next
3. Development packages required for building dependent components

## Installation Scenarios

### Scenario 1: Full OpenCog Stack
```bash
sudo apt-get install libopencog-dev libpln-dev
# Automatically installs: libatomspace, libcogutil
```

### Scenario 2: Minimal AtomSpace Only
```bash
sudo apt-get install libatomspace-dev
# Automatically installs: libcogutil
```

### Scenario 3: GHOST Conversational AI
```bash
sudo apt-get install libopencog-dev
# Includes GHOST + all framework components
```

## ECAN Status

**Economic Attention Networks (ECAN)** is not found as a separate component in the current repository structure. Possible locations:

1. **Integrated into AtomSpace**: Attention allocation may be built into the core atomspace
2. **Separate Repository**: May exist in a different cogpy repository
3. **Deprecated**: May have been replaced by newer attention mechanisms

**Recommendation**: Research ECAN status and either:
- Document its integration into atomspace
- Create separate package if found in another repository
- Note as deprecated if no longer maintained

## Next Steps

1. ✅ Complete atomspace package build
2. Create `libopencog` debian packaging
3. Create `libpln` debian packaging
4. Test installation chain
5. Create meta-package `opencog-full` for complete installation
6. Document component relationships

## Meta-Package Strategy

Create convenience meta-packages:

### opencog-minimal
- libcogutil
- libatomspace
- Minimal working AtomSpace system

### opencog-standard
- opencog-minimal
- libopencog (GHOST, NLP, OpenPsi)
- Standard cognitive architecture

### opencog-full
- opencog-standard
- libpln (reasoning)
- libpln-torch (PyTorch integration)
- Complete OpenCog stack

## Multi-Architecture Support

All packages should support:
- **amd64** (x86_64) - Primary architecture ✅
- **arm64** (aarch64) - ARM 64-bit (Raspberry Pi 4, Apple Silicon)
- **armhf** (armv7l) - ARM 32-bit (Raspberry Pi 3)

Build commands:
```bash
# AMD64 (default)
dpkg-buildpackage -us -uc -b

# ARM64 (cross-compile)
dpkg-buildpackage -us -uc -b -aarm64

# ARMHF (cross-compile)
dpkg-buildpackage -us -uc -b -aarmhf
```

## PPA Setup (Future)

Create official OpenCog PPA:
```bash
sudo add-apt-repository ppa:opencog/opencog
sudo apt-get update
sudo apt-get install opencog-full
```

## HurdCog Integration

All packages designed for compatibility with:
- **Ubuntu 22.04/24.04** - Standard Linux
- **Debian** - Debian-based systems
- **HurdCog** - Modified GNU Hurd with cognitive primitives

HurdCog-specific features (when available):
- Mach port integration
- Translator support
- Kernel-level cognitive primitives

---

**Status**: Strategy document complete
**Next Action**: Build atomspace packages, then proceed with libopencog packaging
