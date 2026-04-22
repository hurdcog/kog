# OCC Repository Analysis Report

## Date: 2025-12-16

## Repository Structure Analysis

### Current State
- Repository: https://github.com/o9nn/occ
- Components: OpenCog Collection with cognumach and hurdcog integration
- Build systems: CMake (OCC), Autotools (cognumach/hurdcog)

## Issues Identified

### 1. Build Tools Organization
**Issue**: CMakeLists.txt references `build-tools/mig/` but directory doesn't exist
**Current State**: 
- MIG exists in `cognumach/mig/`
- Symlink in `hurdcog/mig -> ../cognumach/mig`
- CMakeLists.txt expects `build-tools/mig/`

**Impact**: Medium - Build configuration mismatch
**Fix Required**: Create build-tools directory structure and move MIG to centralized location

### 2. CMakeLists.txt Build Dependency Order
**Issue**: atomspace-storage not explicitly built before cogserver in CMakeLists.txt
**Current State**:
- Line 119-122: cogserver built conditionally
- Line 182-187: atomspace-storage in optional section
**Impact**: High - Build order violation
**Fix Required**: Move atomspace-storage to mandatory build before cogserver

### 3. Windows Build Configuration
**Issue**: build-windows.ps1 lacks component-specific build options
**Current State**: Generic CMake build without OCC-specific flags
**Impact**: Medium - Windows builds may not include all components
**Fix Required**: Add OCC build options to Windows build script

### 4. Chocolatey Package Configuration
**Issue**: Missing actual build artifacts in package
**Current State**: 
- nuspec references tools/** but no build integration
- Install script expects pre-built ZIP from releases
**Impact**: High - Chocolatey package cannot be built from source
**Fix Required**: Add build step to Chocolatey workflow

### 5. Debian Packaging - Missing Components
**Issue**: Not all components have debian packaging
**Missing Packages**:
- atomspace-pgres (structure exists but incomplete)
- hurdcog (mentioned in BUILD_ORDER.md but incomplete)
- cognumach-cognitive-scheduler
- Several AGI-OS bridge components

**Impact**: High - Incomplete packaging infrastructure
**Fix Required**: Complete debian packaging for all components

### 6. MIG Build Tool Duplication
**Issue**: MIG exists in multiple locations
**Locations**:
- cognumach/mig/ (primary)
- hurdcog/mig (symlink to cognumach/mig)
- hurdcog/mig.backup/ (backup copy)

**Impact**: Low - Potential confusion but symlink works
**Fix Required**: Centralize to build-tools/mig/ as per CMakeLists.txt design

## Recommendations

### Priority 1: Critical Build Fixes
1. Fix atomspace-storage build order in CMakeLists.txt
2. Create build-tools/mig/ centralized location
3. Update all MIG references to use centralized location

### Priority 2: Windows Build Enhancement
1. Update build-windows.ps1 with OCC build options
2. Add Chocolatey package build integration
3. Create Windows build artifact generation

### Priority 3: Debian Packaging Completion
1. Complete atomspace-pgres debian package
2. Create hurdcog debian package
3. Add missing AGI-OS component packages
4. Verify build dependency order in all packages

### Priority 4: Integration Improvements
1. Create unified build orchestration script
2. Add integration tests for cognumach/hurdcog/occ
3. Document AGI-OS architecture clearly

## Component Dependency Graph

```
Layer 0: Build Tools
  └── build-tools/mig/ (Mach Interface Generator)

Layer 1: Cognumach (Microkernel)
  └── cognumach/ (depends on: mig)

Layer 2: HurdCog (Operating System)
  └── hurdcog/ (depends on: cognumach, mig)

Layer 3: OCC (AGI Framework)
  ├── cogutil (foundation)
  ├── atomspace (depends on: cogutil)
  ├── atomspace-storage (depends on: atomspace) ← MUST BUILD BEFORE COGSERVER
  ├── cogserver (depends on: atomspace, atomspace-storage)
  ├── ure (depends on: atomspace)
  ├── pln (depends on: atomspace, ure)
  └── ... (other components)
```

## Next Steps

1. Implement fixes in priority order
2. Test builds on Windows and Linux
3. Validate Chocolatey package generation
4. Complete debian packaging infrastructure
5. Commit and push changes to repository
