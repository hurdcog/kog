# Packaging Workflows Guide - OCC Repository

**Date:** December 23, 2025  
**Repository:** https://github.com/o9nn/occ  
**Purpose:** Guide for re-enabling and using packaging workflows

---

## Overview

The OCC repository includes comprehensive packaging workflows for multiple distribution systems:

1. **Chocolatey** - Windows package manager
2. **Debian/APT** - Debian-based Linux distributions
3. **Winget** - Windows Package Manager (Microsoft Store)

These workflows have been **temporarily disabled** to focus on fixing the core Windows build pipeline. This document provides guidance for re-enabling and using them once the Windows builds are stable.

---

## Chocolatey Package Workflow

### Status: 🔴 **Disabled** (Fixed version ready)

**File:** `.github/workflows/chocolatey-package.yml.disabled`  
**Fixed Version:** `.github/workflows/chocolatey-package-fixed.yml`

### Features

- ✅ Automatically triggers after successful Windows builds
- ✅ Creates `.nupkg` package from build artifacts
- ✅ Tests package installation locally
- ✅ Uploads to GitHub releases
- ✅ Optional: Publishes to Chocolatey.org (requires API key)

### Improvements in Fixed Version

1. **Artifact Integration**
   - Downloads artifacts from `occ-win-build.yml` workflow
   - Supports CogUtil, AtomSpace, and Moses components
   - Handles missing artifacts gracefully

2. **Dynamic Versioning**
   - Release tags: Uses tag version (e.g., `v1.0.0` → `1.0.0`)
   - Manual dispatch: Uses input version
   - Automatic: Generates version from date + commit hash

3. **Package Structure**
   - Creates proper Chocolatey package structure
   - Includes install/uninstall scripts
   - Adds binaries to system PATH
   - Professional nuspec metadata

4. **Testing**
   - Local installation test before upload
   - Verification of package contents
   - Error handling with continue-on-error

### Re-enable Instructions

```bash
cd /home/ubuntu/occ/.github/workflows

# Option 1: Use fixed version
mv chocolatey-package-fixed.yml chocolatey-package.yml

# Option 2: Re-enable original (not recommended)
mv chocolatey-package.yml.disabled chocolatey-package.yml
```

### Trigger Methods

1. **Automatic:** After successful `occ-win-build.yml` completion
2. **Release:** When a GitHub release is published
3. **Manual:** Via workflow dispatch with custom version

### Usage Example

```powershell
# After package is built and uploaded
choco install opencog --source='https://github.com/o9nn/occ/releases'

# Or from local package
choco install opencog --source='packaging/chocolatey'
```

### Requirements

- ✅ Successful Windows build artifacts
- ⚠️ Optional: `CHOCOLATEY_API_KEY` secret for publishing to Chocolatey.org

---

## Debian Package Workflow

### Status: 🔴 **Disabled** (Original version available)

**File:** `.github/workflows/debian-package.yml.disabled`

### Features

- ✅ Builds `.deb` packages for Debian/Ubuntu
- ✅ Supports multiple distributions (Bookworm, Bullseye, Sid)
- ✅ Creates both runtime and development packages
- ✅ Runs Lintian checks for package quality
- ✅ Tests installation locally
- ✅ Uploads to GitHub releases

### Package Contents

**Runtime Package (`opencog`):**
- CogUtil libraries
- AtomSpace libraries
- Moses binaries
- Python bindings
- Guile bindings

**Development Package (`opencog-dev`):**
- Header files
- CMake configuration files
- Static libraries (if built)

### Build Dependencies

The workflow automatically installs:
- Build tools (cmake, gcc, g++)
- Boost libraries (filesystem, program-options, system, thread, etc.)
- Python 3 development files
- Guile 3.0 development files
- CxxTest for testing

### Re-enable Instructions

```bash
cd /home/ubuntu/occ/.github/workflows

# Re-enable Debian packaging
mv debian-package.yml.disabled debian-package.yml
```

### Trigger Methods

1. **Release:** When a GitHub release is published
2. **Manual:** Via workflow dispatch with custom version and distribution

### Usage Example

```bash
# After package is built and uploaded
wget https://github.com/o9nn/occ/releases/download/v1.0.0/opencog_1.0.0-1_amd64.deb
sudo dpkg -i opencog_1.0.0-1_amd64.deb
sudo apt-get install -f  # Install dependencies

# Or add to APT repository (requires repository setup)
sudo add-apt-repository ppa:opencog/opencog
sudo apt-get update
sudo apt-get install opencog
```

### Potential Issues

1. **Dependency Versions**
   - Boost library versions may differ between distributions
   - Guile 3.0 may not be available on older distributions
   - **Solution:** Test on each target distribution

2. **Build Time**
   - Full compilation can take 30-60 minutes
   - **Solution:** Consider using build artifacts from Windows builds (cross-compile)

3. **Package Size**
   - Full package can be 100+ MB
   - **Solution:** Split into multiple packages (core, bindings, tools)

---

## Winget Package Workflow

### Status: ✅ **Active**

**File:** `.github/workflows/winget.yml`

### Features

- Lightweight wrapper for Windows Package Manager
- Integrates with Microsoft Store ecosystem
- Automatic updates via Winget repository

### Current Configuration

The workflow is minimal (52 lines) and appears to be a placeholder or wrapper. It may need enhancement to fully integrate with Winget.

### Recommended Enhancements

1. **Manifest Generation**
   - Create Winget manifest files (`.yaml`)
   - Include version, publisher, license information
   - Define installer URLs and checksums

2. **Submission Process**
   - Automate PR creation to `microsoft/winget-pkgs`
   - Include validation checks
   - Handle version updates

3. **Testing**
   - Test installation via `winget install opencog`
   - Verify uninstallation
   - Check for conflicts

### Example Winget Manifest Structure

```yaml
# opencog.yaml
PackageIdentifier: OpenCog.OpenCog
PackageVersion: 1.0.0
PackageName: OpenCog AGI Framework
Publisher: OpenCog Foundation
License: AGPL-3.0
ShortDescription: Framework for Artificial General Intelligence
Installers:
  - Architecture: x64
    InstallerType: msi
    InstallerUrl: https://github.com/o9nn/occ/releases/download/v1.0.0/opencog-1.0.0-x64.msi
    InstallerSha256: <hash>
```

---

## Integration Strategy

### Phase 1: Re-enable After Windows Build Success

**Condition:** `occ-win-build.yml` completes successfully

**Actions:**
1. ✅ Re-enable Chocolatey workflow (use fixed version)
2. ⏸️ Keep Debian workflow disabled (Linux builds need separate testing)
3. ✅ Keep Winget workflow active (already enabled)

**Expected Result:**
- Chocolatey packages built automatically after each successful Windows build
- Artifacts available for download and testing

### Phase 2: Test and Validate Chocolatey Packages

**Actions:**
1. Download Chocolatey package from artifacts
2. Test local installation on Windows
3. Verify all components are included
4. Check PATH configuration
5. Test uninstallation

**Success Criteria:**
- Package installs without errors
- Binaries are accessible from command line
- Uninstallation removes all files

### Phase 3: Re-enable Debian Packaging

**Condition:** Windows builds stable, Chocolatey packages working

**Actions:**
1. Re-enable Debian workflow
2. Test build on Ubuntu 22.04 (Jammy)
3. Test build on Debian 12 (Bookworm)
4. Verify package installation
5. Check for dependency issues

**Expected Result:**
- `.deb` packages build successfully
- Packages install on target distributions
- All dependencies resolved correctly

### Phase 4: Enhance Winget Integration

**Actions:**
1. Create Winget manifest files
2. Automate manifest generation from releases
3. Set up PR automation to `microsoft/winget-pkgs`
4. Test Winget installation

**Expected Result:**
- Users can install via `winget install opencog`
- Automatic updates via Winget

---

## Packaging Workflow Dependencies

```
┌─────────────────────────────────────────────────────────┐
│                  occ-win-build.yml                      │
│  (Primary Windows Build - CogUtil, AtomSpace, Moses)    │
└────────────────────┬────────────────────────────────────┘
                     │
                     ├─────────────────┐
                     │                 │
                     ▼                 ▼
        ┌────────────────────┐  ┌──────────────────┐
        │  Chocolatey        │  │  Winget          │
        │  Package           │  │  Package         │
        │  (Windows)         │  │  (Windows)       │
        └────────────────────┘  └──────────────────┘
                     
                     
┌─────────────────────────────────────────────────────────┐
│              Separate Linux Build                       │
│         (Not yet implemented)                           │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
        ┌────────────────────┐
        │  Debian/APT        │
        │  Package           │
        │  (Linux)           │
        └────────────────────┘
```

---

## Artifact Flow

### Windows Build Artifacts

```
occ-win-build.yml
    ├── cogutil-build-windows
    │   ├── bin/*.dll
    │   ├── lib/*.lib
    │   └── include/*.h
    ├── atomspace-build-windows
    │   ├── bin/*.dll
    │   ├── lib/*.lib
    │   └── include/*.h
    └── moses-build-windows
        ├── bin/*.exe
        ├── bin/*.dll
        └── lib/*.lib

                ↓

chocolatey-package.yml
    └── opencog.1.0.0.nupkg
        └── tools/opencog/
            ├── bin/
            ├── lib/
            └── include/
```

### Debian Build Artifacts

```
debian-package.yml
    ├── opencog_1.0.0-1_amd64.deb
    ├── opencog-dev_1.0.0-1_amd64.deb
    ├── opencog_1.0.0-1_amd64.buildinfo
    └── opencog_1.0.0-1_amd64.changes
```

---

## Testing Checklist

### Chocolatey Package Testing

- [ ] Package builds without errors
- [ ] Package size is reasonable (< 500 MB)
- [ ] Install script runs successfully
- [ ] Binaries are added to PATH
- [ ] DLLs are accessible
- [ ] Uninstall script removes all files
- [ ] No registry pollution after uninstall
- [ ] Works on Windows 10 and Windows 11
- [ ] Works with both PowerShell and CMD

### Debian Package Testing

- [ ] Package builds on Ubuntu 22.04
- [ ] Package builds on Debian 12
- [ ] Lintian checks pass (or warnings are acceptable)
- [ ] Package installs without errors
- [ ] All dependencies are resolved
- [ ] Binaries are in PATH
- [ ] Libraries are in LD_LIBRARY_PATH
- [ ] Python bindings work
- [ ] Guile bindings work (if included)
- [ ] Uninstall removes all files
- [ ] No conflicts with system libraries

### Winget Package Testing

- [ ] Manifest validates successfully
- [ ] Package appears in Winget search
- [ ] Installation via `winget install` works
- [ ] Upgrade via `winget upgrade` works
- [ ] Uninstallation via `winget uninstall` works
- [ ] No conflicts with Chocolatey installation

---

## Troubleshooting

### Chocolatey Package Issues

**Problem:** Package is empty or missing files

**Solution:**
- Check that Windows build artifacts are available
- Verify artifact download step succeeded
- Check artifact retention period (default: 1 day)

**Problem:** Installation fails with PATH errors

**Solution:**
- Verify `Install-ChocolateyPath` is called correctly
- Check that bin directory exists
- Test with administrator privileges

### Debian Package Issues

**Problem:** Build fails with missing dependencies

**Solution:**
- Update `debian/control` with correct dependency versions
- Check distribution-specific package names
- Use `apt-cache search` to find correct package names

**Problem:** Lintian errors

**Solution:**
- Review Lintian output for specific issues
- Update `debian/copyright` for license compliance
- Fix file permissions in package

### Winget Package Issues

**Problem:** Manifest validation fails

**Solution:**
- Use Winget manifest validator
- Check YAML syntax
- Verify installer URL is accessible
- Ensure SHA256 hash is correct

---

## Future Enhancements

### 1. Electron Desktop App Packaging

**Goal:** Package OpenCog components into an Electron desktop application

**Requirements:**
- Electron build system
- Native module compilation (node-gyp)
- Windows, macOS, and Linux installers
- Auto-update mechanism

**Workflow Structure:**
```yaml
name: Build Electron App
on: [push, release]
jobs:
  build-electron:
    strategy:
      matrix:
        os: [windows-latest, ubuntu-latest, macos-latest]
    steps:
      - Build native modules
      - Package Electron app
      - Create installers (NSIS, DMG, DEB)
      - Upload artifacts
```

### 2. APT Repository Setup

**Goal:** Host Debian packages in a proper APT repository

**Requirements:**
- Repository hosting (GitHub Pages, S3, or dedicated server)
- GPG key for package signing
- Repository metadata generation (Packages, Release files)
- Automatic updates on new releases

**Benefits:**
- Users can install via `apt-get install opencog`
- Automatic updates via system package manager
- Dependency resolution handled by APT

### 3. Snap Package

**Goal:** Create Snap package for universal Linux distribution

**Benefits:**
- Works on all Linux distributions
- Sandboxed execution
- Automatic updates
- Easy installation via `snap install opencog`

### 4. Docker Images

**Goal:** Provide Docker images for easy deployment

**Benefits:**
- Consistent environment across platforms
- Easy deployment to cloud services
- Development environment setup
- CI/CD integration

---

## Commit Strategy

### After Windows Builds Succeed

```bash
# Stage 1: Re-enable Chocolatey with fixes
git add .github/workflows/chocolatey-package.yml
git commit -m "Re-enable Chocolatey packaging with Windows build integration"

# Stage 2: Test and iterate
# (Make fixes based on test results)

# Stage 3: Re-enable Debian packaging
git add .github/workflows/debian-package.yml
git commit -m "Re-enable Debian packaging workflow"

# Stage 4: Enhance Winget
git add .github/workflows/winget.yml
git commit -m "Enhance Winget packaging with manifest generation"
```

---

## Success Metrics

### Chocolatey Package

- ✅ Package builds automatically after Windows builds
- ✅ Package size < 500 MB
- ✅ Installation time < 5 minutes
- ✅ 100% of components included
- ✅ Zero installation errors on Windows 10/11

### Debian Package

- ✅ Package builds on Ubuntu 22.04 and Debian 12
- ✅ Lintian warnings < 5
- ✅ Installation time < 2 minutes
- ✅ All dependencies auto-resolved
- ✅ Zero installation errors

### Winget Package

- ✅ Package available in Winget repository
- ✅ Installation via `winget install` works
- ✅ Automatic updates enabled
- ✅ Zero conflicts with other packages

---

## Conclusion

The packaging workflows are well-structured and ready to be re-enabled once the Windows builds are stable. The fixed Chocolatey workflow includes significant improvements for artifact integration and testing. Debian packaging is ready for testing on Linux builds. Winget integration needs enhancement but has a solid foundation.

**Recommended Priority:**
1. **High:** Re-enable Chocolatey after first successful Windows build
2. **Medium:** Test and validate Chocolatey packages
3. **Medium:** Re-enable Debian packaging
4. **Low:** Enhance Winget integration
5. **Future:** Electron app and APT repository setup
