# CI/CD Workflow Status

Last updated: Sat Jan 24 23:48:26 UTC 2026

## Active Workflows (12)

### Windows Build Workflows
1. **occ-win-build.yml** - Full Windows build using vcpkg with GitHub Actions binary caching
   - Status: ✅ Active
   - Purpose: Builds cogutil → atomspace → moses (parallel) → cogserver
   - First build: ~2 hours, subsequent: ~5-10 minutes
   - Uses prebuilt vcpkg packages from vcpkg-prebuild.yml

2. **occ-win-build-fast.yml** - Fast Windows build using vendored dependencies
   - Status: ✅ Active
   - Purpose: Quick builds with pre-vendored dependencies
   - Build time: ~5-10 minutes (no vcpkg rebuild)
   - Requires vendor-dependencies.yml to run first

3. **wincogpre.yml** - Windows cogutil pre-build
   - Status: ✅ Active
   - Purpose: Pre-build cogutil for Windows

### Dependency Management
4. **vcpkg-prebuild.yml** - Tiered vcpkg dependency pre-building
   - Status: ✅ Active
   - Purpose: Pre-build vcpkg dependencies in tiers
   - Tier 1: Quick packages (Boost headers) ~5-10 min
   - Tier 2: Medium packages (Boost compiled) ~15-20 min
   - Tier 3: Heavy packages (gRPC, RocksDB, Protobuf) ~100 min
   - Runs: Weekly or on vcpkg.json changes

5. **vendor-builds.yml** - Smart incremental vendoring
   - Status: ✅ Active
   - Purpose: Calculates source hashes and rebuilds changed components
   - Commits prebuilt binaries to repository

6. **vendor-dependencies.yml** - Vendor vcpkg dependencies
   - Status: ✅ Active
   - Purpose: Vendors vcpkg dependencies for fast builds

7. **heavy-deps-build.yml** - Heavy dependencies build
   - Status: ✅ Active
   - Purpose: Builds heavy dependencies (protobuf, grpc, rocksdb)

### Application Builds
8. **electron-app-build.yml** - Electron desktop app build
   - Status: ✅ Active
   - Purpose: Builds desktop app for Windows/Linux
   - Triggered after successful Windows build
   - Creates installers (.exe, .msi, .AppImage, .deb)

### Package Distribution
9. **chocolatey-package.yml** - Chocolatey Windows package
   - Status: ✅ Active
   - Purpose: Creates and publishes Chocolatey packages

10. **winget.yml** - Windows Package Manager
    - Status: ✅ Active
    - Purpose: Windows Package Manager integration

### Automation
11. **auto-sync-runner.yml** - Auto-sync runner
    - Status: ✅ Active
    - Purpose: Automated synchronization tasks

12. **android-ci.yml** - Android CI
    - Status: ✅ Active
    - Purpose: Android platform builds and tests

## Temporarily Disabled Workflows (30+)

### Build Workflows (.temp_disabled)
- `ci-build-all.yml.temp_disabled` - Complete CI build
- `occ-build.yml.temp_disabled` - Main OCC build
- `guix-build.yml.temp_disabled` - GNU Guix builds
- `debian-packages.yml.temp_disabled` - Debian packaging
- `occ-win-build-complete.yml.temp_disabled` - Complete Windows build
- `occ-win-build-enhanced.yml.temp_disabled` - Enhanced Windows build
- `integration-tests.yml.temp_disabled` - Integration testing
- `cognumach-ci.yml.temp_disabled` - Cognumach CI

### Auto-sync Workflows (.temp_disabled)
- `auto-sync-cogpy.yml.temp_disabled` - CogPy auto-sync
- `auto-sync-enhanced.yml.temp_disabled` - Enhanced auto-sync
- `auto-sync-on-success.yml.temp_disabled` - Success-triggered auto-sync
- `close-issue.yml.temp_disabled` - Auto-close issues

### Package Workflows (.temp_disabled)
- `chocolatey-package-fixed.yml.temp_disabled` - Fixed Chocolatey package
- `debian-package.yml.temp_disabled` - Debian package build

### Permanently Disabled Workflows (.disabled)

#### AGI-OS Workflows
- `agi-os-debian-build.yml.disabled`
- `agi-os-deploy.yml.disabled`
- `agi-os-guix-build.yml.disabled`
- `agi-os-integration-test.yml.disabled`
- `agi-os-layers-build-fixed.yml.disabled`
- `agi-os-layers-build.yml.disabled`
- `agi-os-unified-build.yml.disabled`

#### Build System Workflows
- `occ-complete-stack-4core.yml.disabled`
- `occ-complete-stack-96core.yml.disabled`
- `occ-complete-stack.yml.disabled`
- `occ-evo-11-opencog.yml.disabled`
- `occ-evo.yml.disabled`

#### CI/CD Workflows
- `cogci.yml.disabled` - Self-healing CI
- `cognitive-integration-tests.yml.disabled`
- `hurdcog-ci.yml.disabled`
- `oc.yml.disabled`
- `ocall.yml.disabled`
- `oci.yml.disabled`
- `ocog9.yml.disabled`

#### Quality Checks
- `python-lint.yml.disabled` - Flake8 linting
- `python-type-check.yml.disabled` - Pyright type checking
- `python-check-requirements.yml.disabled`

#### Package Management
- `apt-repository.yml.disabled`
- `debian-package.yml.disabled`
- `build-cmake-pkg.yml.disabled`
- `build-linux-cross.yml.disabled`
- `chocolatey-package.yml.disabled`

#### Other Workflows
- `docker.yml.disabled` - Docker builds
- `wincog.yml.disabled` - Windows cognitive builds
- `bench.yml.disabled` - Benchmarking
- `release.yml.disabled` - Release automation
- `server.yml.disabled` - Server deployment
- `generate-enhanced-triadic-issues.yml.disabled`
- `generate-triadic-issues.yml.disabled`
- `gguf-publish.yml.disabled`
- `guix1.yml.disabled`
- `st.yml.disabled`
- `stw.yml.disabled`

## Workflow Strategy

### Current Focus
The repository is currently focused on:
1. **Windows builds** with vcpkg dependency management
2. **Vendored dependencies** for fast, reproducible builds
3. **Desktop application** distribution via Electron
4. **Package distribution** via Chocolatey and WinGet

### Disabled Workflow Rationale
Many workflows are disabled because they:
- Require dependencies not available in CI environment (Guile 3.0+)
- Are superseded by newer workflows (e.g., vendor-builds.yml)
- Were experimental and no longer maintained
- Have been consolidated into unified workflows

### Re-enabling Workflows

To re-enable a workflow:
1. Verify dependencies are available
2. Update workflow configuration if needed
3. Rename from `.disabled` or `.temp_disabled` to `.yml`
4. Test in a feature branch first
5. Monitor first few runs for issues

## Recommendations

### Immediate Actions
1. **Review .temp_disabled workflows** - Determine if they should be re-enabled or permanently disabled
2. **Clean up .disabled workflows** - Archive or remove obsolete workflows
3. **Update workflow documentation** - Keep this file synchronized with actual workflow status

### Medium-term Actions
1. **Add dependency availability checks** - Workflows should gracefully handle missing dependencies
2. **Implement workflow health monitoring** - Track success rates and build times
3. **Create workflow testing framework** - Test workflow changes before merging

### Long-term Actions
1. **Consolidate similar workflows** - Reduce redundancy
2. **Improve caching strategy** - Minimize build times
3. **Add workflow documentation** - Each workflow should have clear purpose and usage

## See Also
- [Workflow Changes](WORKFLOW_CHANGES.md) - Recent workflow modifications
- [Build Status](BUILD_STATUS.md) - Current build status
- [CI/CD README](.github/workflows/README.md) - Workflow documentation
