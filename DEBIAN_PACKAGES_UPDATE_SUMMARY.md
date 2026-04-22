# Debian Packages Workflow Update Summary

## Overview
Updated `debian-packages.yml` to match the comprehensive build sequence from `occ-build.yml`.

## New Components Added

### 1. **atomspace-storage** (Stage 3)
- **Purpose**: Storage API backend for AtomSpace
- **Dependencies**: Requires atomspace
- **Position**: After atomspace, before cogserver
- **Key packages**: librocksdb-dev not required here (only in atomspace-rocks)

### 2. **atomspace-rocks** (Stage 3b)
- **Purpose**: RocksDB storage backend for AtomSpace
- **Dependencies**: Requires atomspace-storage
- **Position**: After atomspace-storage
- **Key packages**: librocksdb-dev

### 3. **atomspace-cog** (Stage 6b)
- **Purpose**: Network storage backend via CogServer
- **Dependencies**: Requires cogserver
- **Position**: After cogserver
- **Key packages**: libasio-dev

### 4. **spacetime** (Stage 8)
- **Purpose**: Spatiotemporal representation system
- **Dependencies**: Requires cogserver
- **Position**: Parallel to attention, after cogserver
- **Key packages**: liboctomap-dev, libasio-dev

### 5. **learn** (Stage 9)
- **Purpose**: Language learning algorithms
- **Dependencies**: Requires cogserver
- **Position**: Parallel to attention, after cogserver
- **Key packages**: libasio-dev

### 6. **moses** (Stage 10)
- **Purpose**: Meta-Optimizing Semantic Evolutionary Search (foundation component)
- **Dependencies**: Requires only cogutil
- **Position**: Parallel to atomspace (independent)
- **Note**: Already existed in workflow but renumbered

## Updated Dependencies

### cogserver (Stage 6)
- **Before**: `needs: build-atomspace`
- **After**: `needs: build-atomspace-storage`
- **Reason**: CogServer requires atomspace-storage layer

### pln (Stage 13)
- **Before**: `needs: build-ure`
- **After**: `needs: [build-ure, build-spacetime]`
- **Reason**: PLN requires spatiotemporal reasoning capabilities

### Final Summary Job
- **Updated**: Added all new components to the `needs` list
- **Components**: build-atomspace-storage, build-atomspace-rocks, build-atomspace-cog, build-spacetime, build-learn

## Build Order Comparison

### Old Build Sequence (10 stages)
1. cogutil
2. atomspace
3. unify
4. ure
5. cogserver
6. attention
7. moses
8. asmoses
9. miner
10. pln

### New Build Sequence (13 stages)
1. cogutil
2. atomspace
3. **atomspace-storage** ← NEW
3b. **atomspace-rocks** ← NEW
4. unify
5. ure
6. cogserver
6b. **atomspace-cog** ← NEW
7. attention
8. **spacetime** ← NEW
9. **learn** ← NEW
10. moses
11. asmoses
12. miner
13. pln (now depends on spacetime)

## Components NOT Added

### matrix
- **Reason**: No debian packaging available in opencog-debian/
- **Status**: Component exists but without debian/ directory
- **Note**: Included in occ-build.yml but skipped for debian-packages.yml

## Installation Instructions Updated

The installation instructions in the summary now reflect the proper dependency order:
1. cogutil
2. atomspace
3. atomspace-storage
4. cogserver + atomspace-rocks + atomspace-cog
5. All remaining packages
6. Fix dependencies with apt-get

## File Statistics
- **Before**: 869 lines
- **After**: 1235 lines
- **Added**: 366 lines (~42% increase)
- **New jobs**: 5 complete build stages
- **YAML validation**: ✓ Valid

## Alignment with occ-build.yml

This update brings debian-packages.yml into alignment with the comprehensive build sequence in occ-build.yml, ensuring consistent build ordering across both workflows. The only intentional difference is the exclusion of `matrix` due to lack of debian packaging support.
