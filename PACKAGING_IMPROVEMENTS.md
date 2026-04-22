# Packaging Workflow Improvements - OCC Repository

## Date: 2025-12-23

## Summary

This document outlines improvements and fixes applied to the packaging workflows for Chocolatey, Debian, and other package formats.

## 1. Chocolatey Package Improvements

### Current Status
- **Workflow:** `.github/workflows/chocolatey-package.yml`
- **Status:** Active, depends on successful Windows builds
- **Package Location:** `packaging/chocolatey/`

### Issues Identified
1. **Dependency on Windows builds** - Package creation requires successful Windows binary builds
2. **Version placeholder handling** - Install script has `__VERSION__` and `__CHECKSUM64__` placeholders
3. **Limited error handling** - Basic installation script without comprehensive checks

### Improvements Applied

#### Created Enhanced Install Script
**File:** `packaging/chocolatey/tools/chocolateyinstall-improved.ps1`

**Features:**
- Comprehensive error checking
- Verification of package structure
- Better user feedback with colored output
- Environment variable setup (OPENCOG_HOME)
- Start menu integration
- Detailed installation summary
- Graceful handling of missing components

#### Recommendations
1. **Update workflow** to use improved install script
2. **Add checksums** for package verification
3. **Test on clean Windows systems** before publishing
4. **Add package signing** for security

## 2. Debian Package Status

### Current Status
- **Workflow:** `.github/workflows/debian-package.yml`
- **Status:** Active, ready for testing
- **Package Structure:** Individual component packages in `opencog-debian/`

### Architecture
The repository has a comprehensive Debian packaging structure:
- Each component has its own `debian/` directory
- Proper dependency management
- Multi-distribution support (bookworm, bullseye, sid)

### Workflow Features
1. **Automated package building** on release or manual dispatch
2. **Lintian checks** for package quality
3. **Test installation** to verify package integrity
4. **Multi-distribution support**

### Issues and Fixes

#### Issue 1: Missing Root Debian Directory
**Problem:** Workflow expects `debian/` directory at repository root, but structure uses component-specific directories.

**Solution:** The workflow already handles this by:
- Creating debian directory during build
- Copying from `opencog-debian/debian/` if available
- Generating control, rules, changelog files dynamically

#### Issue 2: Build Dependencies
**Problem:** Some dependencies might not be available on all distributions.

**Status:** Properly handled in workflow with comprehensive dependency list.

### Testing Recommendations
1. **Test on Ubuntu 22.04 (Jammy)** - Most common LTS
2. **Test on Debian 12 (Bookworm)** - Latest stable
3. **Verify dependency resolution** on clean systems
4. **Check package installation** and removal

## 3. Aptitude Repository (APT)

### Current Status
- **Workflow:** `.github/workflows/apt-repository.yml.disabled`
- **Status:** Disabled (comprehensive implementation exists)

### Features (When Enabled)
- Self-hosted APT repository on GitHub Pages
- GPG package signing
- Multi-distribution support
- Automated repository metadata generation

### Recommendation
- **Keep disabled** until Windows builds are stable
- **Re-enable** after successful package testing
- **Update GPG keys** if needed

## 4. Winget Package

### Current Status
- **Workflow:** `.github/workflows/winget.yml`
- **Status:** Active
- **Package ID:** OpenCog.OpenCog

### Features
- Automated manifest generation
- Integration with Microsoft winget-pkgs repository
- Triggered on releases

### Requirements
- `WINGET_GITHUB_TOKEN` secret for PR submission
- Successful Windows builds with release artifacts

## 5. Package Testing Strategy

### Chocolatey Testing
```powershell
# Local testing
cd packaging\chocolatey
choco pack opencog.nuspec
choco install opencog -source="'.'" --force --debug

# Verify installation
Get-Command opencog
$env:OPENCOG_HOME

# Test uninstall
choco uninstall opencog
```

### Debian Testing
```bash
# Build package
dpkg-buildpackage -us -uc -b

# Install
sudo dpkg -i ../opencog_*.deb
sudo apt-get install -f

# Verify
dpkg -L opencog
opencog --version

# Remove
sudo apt-get remove opencog --purge
```

## 6. Packaging Workflow Dependencies

### Dependency Graph
```
Windows Builds (occ-win-build.yml, wincog.yml)
    ↓
Chocolatey Package (chocolatey-package.yml)
    ↓
Winget Package (winget.yml)

Debian Build (debian-package.yml)
    ↓
APT Repository (apt-repository.yml)
```

### Critical Path
1. **Windows builds must succeed** before packaging
2. **Chocolatey depends on** Windows build artifacts
3. **Debian builds are independent** but need proper dependencies
4. **APT repository depends on** Debian package builds

## 7. Version Management

### Version Sources
1. **cogutil.h** - `COGUTIL_VERSION_STRING` (authoritative)
2. **vcpkg.json** - `version-string`
3. **GitHub releases** - Tag format `vX.Y.Z`
4. **Package manifests** - Updated during build

### Version Update Process
1. Update `cogutil/opencog/util/cogutil.h`
2. Update `vcpkg.json`
3. Create Git tag: `git tag -a vX.Y.Z -m "Release X.Y.Z"`
4. Push tag: `git push origin vX.Y.Z`
5. GitHub Actions automatically builds and packages

## 8. Package Distribution Channels

### Official Channels
1. **GitHub Releases** - Primary distribution
2. **Chocolatey.org** - Windows package manager
3. **Winget** - Microsoft package manager
4. **APT Repository** - Debian/Ubuntu (self-hosted)

### Future Channels
1. **Snap Store** - Universal Linux packages
2. **Flatpak** - Sandboxed Linux packages
3. **Homebrew** - macOS package manager
4. **Docker Hub** - Container images
5. **Conda** - Python-focused distribution

## 9. Quality Assurance

### Pre-Release Checklist
- [ ] All Windows builds passing
- [ ] All Linux builds passing
- [ ] Chocolatey package builds successfully
- [ ] Debian package builds successfully
- [ ] Test installation on clean Windows system
- [ ] Test installation on clean Ubuntu system
- [ ] Version numbers consistent across all files
- [ ] Documentation updated
- [ ] CHANGELOG.md updated

### Post-Release Verification
- [ ] Chocolatey package installs correctly
- [ ] Debian package installs correctly
- [ ] Winget manifest accepted
- [ ] All dependencies resolve correctly
- [ ] Uninstallation works properly
- [ ] No file conflicts

## 10. Troubleshooting Guide

### Common Issues

#### Chocolatey: "Package not found"
**Cause:** Package not yet published or local source not specified  
**Solution:** Use `-source="'.'"` for local packages

#### Chocolatey: "PATH not updated"
**Cause:** Environment variables not refreshed  
**Solution:** Open new terminal or run `refreshenv`

#### Debian: "Dependency problems"
**Cause:** Missing or conflicting dependencies  
**Solution:** Run `sudo apt-get install -f`

#### Debian: "Package architecture mismatch"
**Cause:** Wrong architecture package  
**Solution:** Verify system architecture with `dpkg --print-architecture`

#### Winget: "Manifest validation failed"
**Cause:** Invalid YAML syntax or missing required fields  
**Solution:** Validate manifest with `winget validate --manifest .`

## 11. Security Considerations

### Package Signing
- **Chocolatey:** Supports package signing (not yet implemented)
- **Debian:** GPG signing available (configured in apt-repository workflow)
- **Winget:** Microsoft validates and signs

### Recommendations
1. **Implement package signing** for Chocolatey
2. **Enable GPG signing** for Debian packages
3. **Verify checksums** for all downloads
4. **Use HTTPS** for all package sources
5. **Scan packages** for vulnerabilities before release

## 12. Monitoring and Maintenance

### Metrics to Track
- Package download counts
- Installation success rate
- User-reported issues
- Build failure rate
- Package size trends

### Maintenance Tasks
- **Weekly:** Check for dependency updates
- **Monthly:** Review package quality metrics
- **Quarterly:** Update package metadata
- **Annually:** Review and update packaging strategy

## 13. Next Steps

### Immediate Actions
1. ✅ Fix Windows build errors (completed)
2. ✅ Update vcpkg configurations (completed)
3. ⏳ Test Windows builds (in progress)
4. ⏳ Verify Chocolatey package creation
5. ⏳ Test Debian package builds

### Short-term Goals (1-2 weeks)
1. Publish first Chocolatey package
2. Submit Winget manifest
3. Test Debian packages on multiple distributions
4. Enable APT repository

### Long-term Goals (1-3 months)
1. Implement package signing
2. Add Snap and Flatpak support
3. Create Docker images
4. Set up automated package testing
5. Implement delta updates

## 14. Documentation Updates Needed

### User Documentation
- [ ] Installation guide for Chocolatey
- [ ] Installation guide for Debian/Ubuntu
- [ ] Installation guide for Winget
- [ ] Troubleshooting guide
- [ ] Uninstallation instructions

### Developer Documentation
- [ ] Packaging workflow architecture
- [ ] How to add new components
- [ ] Release process documentation
- [ ] Package testing procedures
- [ ] Dependency management guide

## 15. Contact and Support

### For Package Issues
- **GitHub Issues:** https://github.com/cogpy/occ/issues
- **Label:** `packaging`

### For Build Issues
- **GitHub Issues:** https://github.com/cogpy/occ/issues
- **Label:** `build`

### For General Support
- **Help Portal:** https://help.manus.im
- **Wiki:** https://wiki.opencog.org/

---

**Status:** Packaging infrastructure ready, awaiting successful Windows builds  
**Next Review:** After first successful Windows build completion  
**Last Updated:** 2025-12-23
