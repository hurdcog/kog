# Windows Build Analysis & Fix Strategy

## Executive Summary

The OCC (OpenCog Collection) Windows builds are failing due to POSIX-specific code that is incompatible with the Windows/MSVC toolchain. This document provides a comprehensive analysis and fix strategy.

## Current Build Status

**Failed Workflows:**
- `OCC Build - Windows Native (wincog)` - Run #20447059327
- `OCC Windows Build` - Run #20447059324
- `Build Chocolatey Package` - Disabled (depends on Windows builds)

**Build Stage:** Stage 1 - CogUtil (Foundation) - **FAILING**

## Critical Build Errors

### 1. Config.cc - getcwd() Not Found
**Error:** `error C3861: 'getcwd': identifier not found`
**Location:** `cogutil/opencog/util/Config.cc:148`
**Cause:** POSIX `getcwd()` not available on Windows
**Fix:** Use `_getcwd()` on Windows with conditional compilation

### 2. Logger.cc - Missing sys/time.h
**Error:** `error C1083: Cannot open include file: 'sys/time.h'`
**Location:** `cogutil/opencog/util/Logger.cc:69`
**Cause:** POSIX header not available on Windows
**Fix:** Use Windows time headers (`<time.h>`, `<sys/timeb.h>`)

### 3. snprintf Macro Conflict
**Error:** `error C1189: #error: Macro definition of snprintf conflicts with Standard Library`
**Location:** `cogutil/build/opencog/util/cogutil.vcxproj`
**Cause:** Legacy snprintf macro conflicts with C++11 standard library
**Fix:** Remove snprintf macro definitions, use standard library function

### 4. M_PI Constant Not Defined
**Error:** `error C3861: 'M_PI': identifier not found`
**Location:** `cogutil/opencog/util/numeric.h:412`
**Cause:** Math constants not defined by default in MSVC
**Fix:** Define `_USE_MATH_DEFINES` before including `<cmath>`

## Dependency Analysis

### Build Order (Dependency Graph)
```
cogutil (Foundation)
  ├─→ atomspace (Hypergraph Database)
  │     ├─→ cogserver (Network Server)
  │     ├─→ attention (Attention Allocation)
  │     ├─→ unify (Unification)
  │     ├─→ ure (Unified Rule Engine)
  │     └─→ pln (Probabilistic Logic Networks)
  └─→ moses (Meta-Optimizing Semantic Evolutionary Search)
```

**Critical Path:** CogUtil must build successfully before any other component can proceed.

## GitHub Actions Workflow Analysis

### Windows Build Workflows (KEEP ENABLED)
1. **occ-win-build.yml** (376 lines)
   - Builds: cogutil, atomspace, moses
   - Uses: vcpkg, Visual Studio 2022, MSBuild
   - Status: FAILING at cogutil stage

2. **wincog.yml** (2467 lines)
   - Comprehensive Windows-native build
   - Builds all OpenCog components
   - Status: FAILING at cogutil stage

3. **winget.yml** (52 lines)
   - Windows Package Manager integration
   - Status: Depends on successful builds

### Non-Windows Workflows (DISABLE TEMPORARILY)
1. **chocolatey-package.yml** (251 lines)
   - Status: Already disabled (`if: false` on line 22)
   - Depends on: Windows builds

2. **editorconfig.yml** (29 lines)
   - EditorConfig validation
   - Not Windows-specific
   - **Action:** Disable temporarily

3. **labeler.yml** (17 lines)
   - PR labeling automation
   - Not Windows-specific
   - **Action:** Disable temporarily

4. **close-issue.yml** (28 lines)
   - Stale issue management
   - Not Windows-specific
   - **Action:** Keep enabled (maintenance)

5. **auto-sync.yml** (83 lines)
   - Syncs o9nn/occ → cogpy/occ
   - Triggers after successful Windows builds
   - **Action:** Keep enabled (already conditional)

## Packaging Workflows Status

### Chocolatey (Windows)
- **File:** `.github/workflows/chocolatey-package.yml`
- **Status:** Disabled, waiting for stable Windows builds
- **Issues:** 
  - Depends on non-existent `build-windows.ps1` script
  - References missing `dist/` directory
  - Needs to consume artifacts from wincog.yml

### Debian/Aptitude
- **Location:** `opencog-debian/` directory
- **Status:** No GitHub Actions workflow found
- **Issue:** Missing automation for Debian package builds

### Winget (Windows Package Manager)
- **File:** `.github/workflows/winget.yml`
- **Status:** Present but not tested
- **Depends on:** Successful Windows builds + releases

## Fix Strategy

### Phase 1: Fix CogUtil Windows Compatibility (PRIORITY 1)
1. Fix `Config.cc` - Add Windows getcwd support
2. Fix `Logger.cc` - Replace sys/time.h with Windows headers
3. Fix snprintf conflicts - Remove legacy macros
4. Fix `numeric.h` - Add _USE_MATH_DEFINES
5. Create comprehensive Windows compatibility header

### Phase 2: Disable Non-Critical Workflows
1. Disable `editorconfig.yml` temporarily
2. Disable `labeler.yml` temporarily
3. Keep `close-issue.yml` enabled
4. Keep `auto-sync.yml` enabled (conditional)

### Phase 3: Fix Packaging Workflows
1. Update `chocolatey-package.yml` to consume wincog artifacts
2. Create Debian packaging workflow
3. Test winget workflow after successful builds

### Phase 4: Enable Auto-Sync with git-pat
1. Verify `secrets.git_pat` is configured
2. Test auto-sync after successful Windows build
3. Document sync process

## Recommended File Structure

```
occ/
├── .github/
│   └── workflows/
│       ├── occ-win-build.yml          [KEEP - Primary Windows build]
│       ├── wincog.yml                  [KEEP - Comprehensive Windows build]
│       ├── winget.yml                  [KEEP - Windows package manager]
│       ├── auto-sync.yml               [KEEP - Conditional sync]
│       ├── close-issue.yml             [KEEP - Maintenance]
│       ├── editorconfig.yml            [DISABLE TEMPORARILY]
│       ├── labeler.yml                 [DISABLE TEMPORARILY]
│       └── chocolatey-package.yml      [FIX - Update artifact consumption]
├── cogutil/
│   ├── cmake/
│   │   └── WindowsCompat.cmake        [CREATE - Windows compatibility]
│   └── opencog/
│       └── util/
│           ├── windows_compat.h       [CREATE - Windows compatibility header]
│           ├── Config.cc              [FIX - getcwd]
│           ├── Logger.cc              [FIX - sys/time.h]
│           └── numeric.h              [FIX - M_PI]
└── packaging/
    ├── chocolatey/                     [FIX - Artifact consumption]
    └── debian/                         [CREATE - Debian workflow]
```

## Next Steps

1. **Immediate:** Fix CogUtil Windows compatibility issues
2. **Immediate:** Disable non-Windows workflows temporarily
3. **Short-term:** Update Chocolatey packaging to use wincog artifacts
4. **Short-term:** Create Debian packaging workflow
5. **Medium-term:** Test and validate all Windows builds
6. **Medium-term:** Re-enable disabled workflows after Windows builds stable
7. **Long-term:** Implement Inferno kernel-based AGI architecture

## Success Criteria

- [ ] CogUtil builds successfully on Windows
- [ ] AtomSpace builds successfully on Windows
- [ ] Moses builds successfully on Windows
- [ ] All Windows workflows pass without errors
- [ ] Chocolatey package builds successfully
- [ ] Auto-sync triggers after successful builds
- [ ] Debian packaging workflow created and functional
