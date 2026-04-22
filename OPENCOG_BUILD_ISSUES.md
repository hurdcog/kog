# OpenCog Build Issues and Recommendations

## Summary

The OpenCog repository has several build issues that prevent successful compilation of the complete package. This document details the issues found and provides recommendations for resolution.

## Build Issues Identified

### 1. IRC Chatbot - Missing StringTokenizer.h

**Location**: `opencog/nlp/irc/CogitaConfig.cc:29`

**Error**:
```
fatal error: opencog/util/StringTokenizer.h: No such file or directory
```

**Analysis**:
- The IRC chatbot component references `opencog/util/StringTokenizer.h` from cogutil
- This header file does not exist in the current cogutil codebase
- The IRC chatbot appears to be legacy code that hasn't been updated

**Resolution**: Disabled IRC subdirectory in `opencog/nlp/CMakeLists.txt`

### 2. NLP WSD - Incorrect Include Path

**Location**: `opencog/nlp/wsd/ForeachWord.h:23`

**Error**:
```
fatal error: opencog/nlp/types/atom_types.h: No such file or directory
```

**Analysis**:
- The WSD (Word Sense Disambiguation) code includes `<opencog/nlp/types/atom_types.h>`
- The actual generated file is at `opencog/nlp/oc-types/atom_types.h`
- This is a mismatch between the generated file location and the include path
- The atom_types.h file IS being generated correctly during build
- The include path in the source code is incorrect or outdated

**Possible Solutions**:
1. Update all WSD source files to use correct include path: `<opencog/nlp/oc-types/atom_types.h>`
2. Create a symlink or copy during build: `types/atom_types.h` → `oc-types/atom_types.h`
3. Update CMake to add the correct include directory
4. Disable WSD component if not essential

### 3. Complex Dependency Chain

**Issue**: The opencog repository has a complex build dependency chain:
- neighbors (standalone)
- nlp (depends on atomspace)
  - irc (broken - missing StringTokenizer)
  - scm (working)
  - sentiment (working)
  - oc-types (working - generates atom_types.h)
  - wsd (broken - wrong include path)
  - aiml, chatbot, chatbot-psi, fuzzy, microplanning, relex2logic, sureal (require HAVE_NLP flag)
- eva (depends on atomspace + guile)
- openpsi (requires HAVE_OPENPSI flag)
- ghost (requires HAVE_GHOST flag)

## Recommended Approach

### Option 1: Minimal Core Package (RECOMMENDED)

Create a minimal `libopencog-core` package with only working components:

**Include**:
- neighbors (similarity/clustering)
- nlp/scm (Scheme bindings)
- nlp/sentiment (sentiment analysis)
- nlp/oc-types (atom type definitions)
- eva (emotional/expressive animation)

**Exclude**:
- nlp/irc (broken)
- nlp/wsd (broken)
- nlp/aiml, chatbot, etc. (require additional dependencies)

**Debian Package**: `libopencog-core` + `libopencog-core-dev`

### Option 2: Component-Specific Packages

Create separate packages for each working component:

1. **libopencog-neighbors** - Similarity and clustering
2. **libopencog-eva** - Emotional/Expressive Animation
3. **libopencog-ghost** - GHOST conversational AI (if it builds independently)
4. **libopencog-nlp-basic** - Basic NLP (scm, sentiment, oc-types only)

### Option 3: Fix All Issues (Time-Intensive)

1. Remove or update IRC chatbot code
2. Fix all WSD include paths
3. Test full NLP stack
4. Create comprehensive package

**Estimated Time**: 4-8 hours of debugging and testing

## GHOST Status

GHOST (Goal-Oriented Script-based Hierarchical Organizing System for Thought) is the most important component for conversational AI. It should be prioritized.

**Check**: Does GHOST build independently?
**Location**: `opencog/ghost/`
**Dependencies**: atomspace, guile, openpsi

**Recommendation**: Create a standalone `libghost` package if possible.

## Immediate Action Plan

### Phase 1: Minimal Working Package (NOW)

1. Disable broken NLP components (irc, wsd)
2. Build minimal opencog-core with:
   - neighbors
   - nlp/scm
   - nlp/sentiment
   - nlp/oc-types
   - eva
3. Test and package

### Phase 2: GHOST Package (NEXT)

1. Investigate GHOST dependencies
2. Build standalone GHOST package
3. Test conversational AI functionality

### Phase 3: Fix Remaining Issues (LATER)

1. Fix WSD include paths
2. Remove or update IRC code
3. Test full NLP stack
4. Create comprehensive packages

## Build Commands

### Disable Broken Components

Edit `opencog/nlp/CMakeLists.txt`:
```cmake
# ADD_SUBDIRECTORY (irc)  # Disabled: missing StringTokenizer
# ADD_SUBDIRECTORY (wsd)  # Disabled: wrong include path
```

### Build Minimal Package

```bash
cd /home/ubuntu/occ/opencog
dpkg-buildpackage -us -uc -b
```

## Files Modified

1. `/home/ubuntu/occ/opencog/opencog/nlp/CMakeLists.txt`
   - Commented out `ADD_SUBDIRECTORY (irc)`
   - Need to comment out `ADD_SUBDIRECTORY (wsd)`

2. `/home/ubuntu/occ/opencog/debian/rules`
   - Added `-DWITH_IRC=OFF` (didn't work - CMake flag not recognized)

## Conclusion

The OpenCog repository has legacy code and build system issues that prevent straightforward packaging. The recommended approach is to:

1. **Immediate**: Create minimal working package with core components
2. **Short-term**: Create standalone GHOST package for conversational AI
3. **Long-term**: Fix all build issues for comprehensive packaging

This pragmatic approach delivers working packages quickly while documenting issues for future resolution.

## Next Steps

1. Disable WSD component
2. Rebuild opencog-core
3. Test installation
4. Create GHOST-specific package
5. Document all working components
6. Commit to repository

---

**Date**: 2025-11-30  
**Status**: In Progress  
**Priority**: High - Blocking full OpenCog packaging
