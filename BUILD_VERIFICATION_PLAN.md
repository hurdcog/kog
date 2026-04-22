# Build Verification and Testing Plan

**Date:** 2024-12-24  
**Repository:** o9nn/occ  
**Purpose:** Verify all packages build without errors after optimizations

## Overview

This document outlines the verification plan for ensuring all OpenCog Collection (OCC) components build successfully on Windows and package correctly for various distribution platforms.

## Build Verification Status

### ✅ Completed Optimizations

1. **Enhanced Windows Build Workflow**
   - File: `.github/workflows/occ-win-build-enhanced.yml`
   - Features:
     - Retry logic with exponential backoff (5 attempts)
     - Network timeout handling
     - Increased parallelism (4 jobs)
     - Enhanced error reporting
   - Status: Ready for testing

2. **Disabled Non-Critical Workflows**
   - `auto-sync-enhanced.yml`: Disabled scheduled and workflow_run triggers
   - `auto-sync-cogpy.yml`: Disabled scheduled runs
   - `debian-package.yml`: Disabled push/PR triggers
   - Status: Configured

3. **Fixed Packaging Workflows**
   - Chocolatey: Updated to consolidate artifacts from multiple builds
   - Debian: Enhanced to build all components with proper dependencies
   - Status: Ready for testing

4. **Implemented Auto-Sync**
   - File: `.github/workflows/auto-sync-on-success.yml`
   - Triggers: After successful Windows builds only
   - Authentication: git_pat
   - Features: Conflict detection, dry-run mode, detailed reporting
   - Status: Ready for testing

## Component Build Order

### Windows Builds

```
Stage 1: Foundation
└─ CogUtil (cogutil)
   └─ Dependencies: Boost, GSL, vcpkg packages

Stage 2: Core (parallel, depends on CogUtil)
├─ AtomSpace (atomspace)
│  └─ Dependencies: CogUtil, Guile, PostgreSQL
└─ Moses (moses)
   └─ Dependencies: CogUtil, Boost

Stage 3: Services (depends on AtomSpace)
├─ CogServer (cogserver)
│  └─ Dependencies: AtomSpace
├─ Attention (attention)
│  └─ Dependencies: AtomSpace
└─ URE (ure)
   └─ Dependencies: AtomSpace

Stage 4: Advanced (depends on URE)
├─ PLN (pln)
│  └─ Dependencies: URE, AtomSpace
└─ Miner (miner)
   └─ Dependencies: URE, AtomSpace
```

### Debian/Ubuntu Builds

Same dependency order as Windows, but using:
- apt packages for system dependencies
- CMake for build system
- debhelper for packaging

### Chocolatey Packaging

Depends on:
- Successful Windows build artifacts
- All DLLs and executables from install directory
- vcredist140 runtime dependency

## Verification Steps

### Phase 1: Windows Build Verification

#### Step 1.1: Trigger Enhanced Windows Build
```bash
# Via GitHub CLI
gh workflow run occ-win-build-enhanced.yml --ref main

# Or via web interface:
# Actions → OCC Windows Build (Enhanced with Retry) → Run workflow
```

#### Step 1.2: Monitor Build Progress
```bash
# Watch the latest run
gh run watch

# Or check status
gh run list --workflow=occ-win-build-enhanced.yml --limit 1
```

#### Step 1.3: Verify Build Artifacts
Expected artifacts:
- `cogutil-build-windows-enhanced`
- `atomspace-build-windows-enhanced`
- `moses-build-windows-enhanced`

Each should contain:
- `/install/bin/*.exe` - Executables
- `/install/lib/*.lib` - Static libraries
- `/install/bin/*.dll` - Dynamic libraries
- `/install/include/**/*.h` - Header files

#### Step 1.4: Check Build Logs
Look for:
- ✅ All vcpkg dependencies installed
- ✅ CMake configuration successful
- ✅ Compilation completed without errors
- ✅ Installation to artifact directory successful

### Phase 2: Packaging Verification

#### Step 2.1: Chocolatey Package Build
```bash
# Should trigger automatically after successful Windows build
# Or manually:
gh workflow run chocolatey-package.yml --ref main
```

Verify:
- Package created: `opencog-occ.{version}.nupkg`
- Installation scripts present
- All binaries included
- Dependencies declared correctly

#### Step 2.2: Debian Package Build
```bash
# Manually trigger (disabled for push/PR)
gh workflow run debian-package.yml --ref main -f version=1.0.0 -f distributions=jammy,noble
```

Verify:
- Packages created: `opencog-occ_{version}.deb`, `opencog-occ-dev_{version}.deb`
- All components built in correct order
- Dependencies resolved
- Lintian checks pass

### Phase 3: Auto-Sync Verification

#### Step 3.1: Verify git_pat Secret
```bash
# Check if secret is configured (via web interface)
# Settings → Secrets and variables → Actions → Repository secrets
# Should see: git_pat
```

#### Step 3.2: Test Auto-Sync (Dry Run)
```bash
# Manual trigger with dry run
gh workflow run auto-sync-on-success.yml --ref main -f dry_run=true
```

Expected output:
- Analysis of differences between o9nn/occ and cogpy/occ
- List of commits to sync
- No actual push (dry run)

#### Step 3.3: Test Auto-Sync (Real)
After successful Windows build, auto-sync should trigger automatically.

Verify:
- Workflow triggered after Windows build completion
- Changes pushed to cogpy/occ
- Sync report generated
- No conflicts detected

## Expected Results

### Success Criteria

#### Windows Builds
- [ ] CogUtil builds without errors
- [ ] AtomSpace builds without errors
- [ ] Moses builds without errors
- [ ] All artifacts uploaded successfully
- [ ] Build time < 2 hours per component
- [ ] Retry logic handles transient network failures

#### Packaging
- [ ] Chocolatey package created successfully
- [ ] Chocolatey package installs without errors
- [ ] Debian packages created for all distributions
- [ ] Debian packages install without dependency errors
- [ ] All binaries and libraries included

#### Auto-Sync
- [ ] Sync triggers after successful Windows build
- [ ] Changes pushed to cogpy/occ
- [ ] No conflicts during sync
- [ ] Sync report generated and uploaded
- [ ] git_pat authentication works correctly

### Known Issues and Workarounds

#### Issue 1: vcpkg Network Timeouts (504 errors)
**Status:** Addressed with retry logic

**Workaround:**
- Enhanced workflow includes 5 retry attempts
- Exponential backoff with jitter
- Clears buildtrees between retries

**If still failing:**
- Re-run the workflow manually
- Check vcpkg GitHub status
- Consider pinning vcpkg to stable commit

#### Issue 2: Missing Components
**Status:** Some components may not have directories

**Workaround:**
- Debian and Chocolatey workflows check for directory existence
- Only build components that are present
- Core components (cogutil, atomspace, moses) are required

#### Issue 3: Dependency Resolution
**Status:** Complex dependency chains

**Workaround:**
- Build order strictly enforced
- Artifacts passed between jobs
- CMAKE_PREFIX_PATH set correctly

## Testing Checklist

### Pre-Push Testing
- [x] Enhanced Windows workflow syntax validated
- [x] Chocolatey workflow updated for artifact consolidation
- [x] Debian workflow enhanced for all components
- [x] Auto-sync workflow configured with git_pat
- [x] Non-critical workflows disabled

### Post-Push Testing
- [ ] Trigger enhanced Windows build manually
- [ ] Monitor build progress and logs
- [ ] Verify all artifacts created
- [ ] Test Chocolatey packaging
- [ ] Test Debian packaging
- [ ] Verify auto-sync triggers correctly
- [ ] Check cogpy/occ receives updates

### Integration Testing
- [ ] Full build → package → sync pipeline
- [ ] Verify Windows binaries are functional
- [ ] Test Chocolatey installation on clean Windows
- [ ] Test Debian installation on Ubuntu Jammy/Noble
- [ ] Verify all OpenCog components present

## Rollback Plan

If builds fail after changes:

1. **Revert workflow changes:**
   ```bash
   git revert <commit-hash>
   git push origin main
   ```

2. **Use previous working workflows:**
   - `occ-win-build.yml` (original)
   - `occ-win-build-complete.yml` (complete stack)

3. **Disable problematic workflows:**
   - Add `if: false` to job conditions
   - Or delete workflow files temporarily

## Next Steps After Verification

Once all builds pass:

1. **Enable Additional Components:**
   - Add cogserver, attention, ure, pln, miner to enhanced workflow
   - Update packaging to include all components

2. **Optimize Build Times:**
   - Increase parallelism further
   - Use more aggressive caching
   - Consider self-hosted runners

3. **Implement Continuous Deployment:**
   - Auto-publish Chocolatey packages
   - Auto-publish Debian packages to PPA
   - Create GitHub releases with binaries

4. **Part 2: Inferno Kernel-Based AGI Architecture**
   - Design cognitive kernel services
   - Implement distributed AGI operating system
   - Integrate with existing OpenCog components

## Monitoring and Maintenance

### Daily Checks
- Monitor GitHub Actions status
- Check for failed builds
- Review sync reports
- Monitor cogpy/occ sync status

### Weekly Checks
- Review build times and optimize
- Update dependencies if needed
- Check for vcpkg updates
- Review and close stale issues

### Monthly Checks
- Full integration testing
- Performance benchmarking
- Security updates
- Documentation updates

## Conclusion

This verification plan ensures that all optimizations are tested systematically and that the build pipeline is robust and reliable. The enhanced workflows address the primary issue (network timeouts) while improving overall build quality and automation.

**Current Status:** Ready for Phase 1 testing (Windows builds)

**Next Action:** Push changes and trigger enhanced Windows build workflow
