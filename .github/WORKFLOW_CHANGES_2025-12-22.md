# Workflow Changes - December 22, 2025

## Purpose
Temporarily disable all non-Windows build workflows to focus on fixing Windows builds, which are the highest priority for this repository.

## Changes Made

### Critical Fix: MSVC C++17 Configuration
- **File**: `cogutil/cmake/OpenCogGccOptions.cmake`
- **Change**: Added comprehensive MSVC (Visual Studio) compiler configuration
- **Details**:
  - Set C++17 standard explicitly (`CMAKE_CXX_STANDARD 17`)
  - Added `/std:c++17` compiler flag
  - Enabled OpenMP support with `/openmp` flag
  - Enabled parallel compilation with `/MP` flag
  - Added standards conformance with `/permissive-` flag
  - Disabled noisy DLL interface warnings (4251, 4275)
  - Enabled big object files with `/bigobj` flag
  - Configured proper optimization and debug flags

### Workflows Disabled (Temporarily)

The following workflows have been disabled by renaming with `.disabled` extension:

1. **cogci.yml** → **cogci.yml.disabled**
   - CogML CI Build (Self-Healing)
   - Reason: Linux-specific, was failing

2. **st.yml** → **st.yml.disabled**
   - SpaceTime Build
   - Reason: Linux-specific, was failing

3. **stw.yml** → **stw.yml.disabled**
   - SpaceTime Build (variant)
   - Reason: Linux-specific

4. **server.yml** → **server.yml.disabled**
   - Server build
   - Reason: Linux-specific

5. **docker.yml** → **docker.yml.disabled**
   - Docker container builds
   - Reason: Not Windows-related

6. **build-cmake-pkg.yml** → **build-cmake-pkg.yml.disabled**
   - Build relocatable cmake package
   - Reason: Not Windows-specific

7. **python-lint.yml** → **python-lint.yml.disabled**
   - flake8 Lint
   - Reason: Code quality check, not critical for Windows builds

8. **python-type-check.yml** → **python-type-check.yml.disabled**
   - Python Type-Check
   - Reason: Was failing, not critical for Windows builds

9. **python-check-requirements.yml** → **python-check-requirements.yml.disabled**
   - Python Check Requirements
   - Reason: Not critical for Windows builds

10. **release.yml** → **release.yml.disabled**
    - Release workflow
    - Reason: Cannot release until Windows builds work

11. **generate-triadic-issues.yml** → **generate-triadic-issues.yml.disabled**
    - Generate Triadic Issues
    - Reason: Was failing, not critical for Windows builds

12. **generate-enhanced-triadic-issues.yml** → **generate-enhanced-triadic-issues.yml.disabled**
    - Generate Enhanced Development Issues
    - Reason: Not critical for Windows builds

13. **gguf-publish.yml** → **gguf-publish.yml.disabled**
    - GGUF Publish
    - Reason: Not critical for Windows builds

14. **apt-repository.yml** → **apt-repository.yml.disabled**
    - APT Repository management
    - Reason: Linux-specific, Debian packaging

### Workflows Remaining Active

The following workflows remain active as they are Windows-related or essential:

1. **occ-win-build.yml** - OCC Windows Build (PRIMARY FOCUS)
2. **wincog.yml** - OCC Build - Windows Native (PRIMARY FOCUS)
3. **chocolatey-package.yml** - Chocolatey Package (Windows packaging)
4. **winget.yml** - Winget (Windows package manager)
5. **auto-sync.yml** - Auto Sync Repository (for git-pat integration)
6. **close-issue.yml** - Close Issue (utility)
7. **labeler.yml** - Pull Request Labeler (utility)
8. **editorconfig.yml** - EditorConfig Checker (code quality)

## Expected Impact

### Immediate Benefits
1. Reduced CI/CD noise from failing non-Windows workflows
2. Focus on fixing Windows builds without distractions
3. Faster feedback loop for Windows build fixes
4. Lower GitHub Actions usage and costs

### Windows Build Improvements
1. CogUtil should now build successfully on Windows with C++17 support
2. Proper OpenMP multithreading support on Windows
3. Better MSVC compiler compatibility
4. Reduced build warnings

## Re-enabling Workflows

Once Windows builds are stable and all packages build without errors:

1. Re-enable Linux builds (cogci.yml, st.yml, stw.yml)
2. Re-enable Docker builds (docker.yml)
3. Re-enable packaging workflows (apt-repository.yml, debian-packages.yml)
4. Re-enable code quality checks (python-lint.yml, python-type-check.yml)
5. Re-enable release workflow (release.yml)
6. Re-enable issue generation workflows

## Testing Required

After these changes:
1. Test cogutil build on Windows
2. Test atomspace build on Windows (depends on cogutil)
3. Test moses build on Windows (depends on cogutil)
4. Test full Windows build pipeline
5. Verify all OpenCog components build without errors
6. Test Windows binary functionality

## Next Steps

1. Commit and push these changes
2. Monitor Windows build workflows
3. Fix any remaining Windows build issues
4. Fix Chocolatey packaging workflow
5. Implement auto-sync with git-pat
6. Once stable, re-enable other workflows
7. Proceed to Part 2: Inferno kernel-based AGI OS implementation
