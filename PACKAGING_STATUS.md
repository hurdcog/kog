# OpenCog Collection Packaging Status

## Overview

This document tracks the status of packaging workflows for distributing OpenCog Collection (OCC) across multiple platforms and package managers.

## Package Managers

### Windows

#### Chocolatey
- **Status:** ✅ Workflow configured, awaiting stable Windows builds
- **Workflow:** `.github/workflows/chocolatey-package.yml`
- **Package Name:** `opencog`
- **Trigger:** Release or manual dispatch
- **Dependencies:** 
  - vcredist2019 (Visual C++ Redistributable)
  - cmake (>= 3.20.0)
- **Components Included:**
  - cogutil - Core utilities
  - atomspace - Hypergraph knowledge representation
  - atomspace-storage - Persistence backends
  - unify - Unification framework
  - ure - Unified Rule Engine
  - cogserver - Network server
  - pln - Probabilistic Logic Networks
  - moses - Evolutionary program learning
- **Installation:** `choco install opencog`
- **Notes:** Currently depends on successful Windows builds from wincog.yml

#### Winget (Windows Package Manager)
- **Status:** ✅ Workflow configured
- **Workflow:** `.github/workflows/winget.yml`
- **Package ID:** `OpenCog.OpenCog`
- **Trigger:** Release
- **Notes:** Automatically submits to winget-pkgs repository

### Linux

#### Debian/Ubuntu (APT)
- **Status:** ✅ Workflow created
- **Workflow:** `.github/workflows/debian-package.yml`
- **Package Name:** `opencog`, `opencog-dev`
- **Supported Distributions:**
  - Debian Bookworm (12)
  - Debian Bullseye (11)
  - Debian Sid (unstable)
  - Ubuntu 22.04 LTS (Jammy)
  - Ubuntu 24.04 LTS (Noble)
- **Architecture:** amd64, arm64 (planned)
- **Build System:** debhelper + CMake
- **Dependencies:**
  - libboost-filesystem
  - libboost-program-options
  - libboost-system
  - libboost-thread
  - python3
  - guile-3.0
- **Installation:** `sudo apt install ./opencog_*.deb`
- **Repository:** PPA planned for future
- **Notes:** Builds from source using debian/ directory

#### Arch Linux (AUR)
- **Status:** 🔄 Planned
- **Package Name:** `opencog-git`
- **Build System:** PKGBUILD
- **Notes:** Community-maintained AUR package

#### Fedora/RHEL (RPM)
- **Status:** 🔄 Planned
- **Package Name:** `opencog`
- **Build System:** rpmbuild + spec file
- **Notes:** Requires RPM spec file creation

### Cross-Platform

#### Nix
- **Status:** ✅ Existing support
- **Location:** `opencog-nix/`
- **Package Name:** `opencog`
- **Installation:** `nix-env -iA nixpkgs.opencog`
- **Notes:** Declarative package management

#### Guix
- **Status:** ✅ Existing support
- **Location:** `guix/` directory
- **Package Name:** `opencog`
- **Installation:** `guix install opencog`
- **Notes:** Functional package management

#### Docker
- **Status:** 🔄 Planned
- **Image Name:** `opencog/opencog`
- **Registry:** Docker Hub
- **Tags:** `latest`, `dev`, version tags
- **Notes:** Containerized deployment

#### Flatpak
- **Status:** 🔄 Planned
- **App ID:** `org.opencog.OpenCog`
- **Runtime:** `org.freedesktop.Platform`
- **Notes:** Sandboxed desktop application

## Build Matrix

| Platform | Package Manager | Status | Workflow | Artifacts |
|----------|----------------|--------|----------|-----------|
| Windows 10/11 | Chocolatey | ✅ Ready | chocolatey-package.yml | .nupkg |
| Windows 10/11 | Winget | ✅ Ready | winget.yml | manifest |
| Debian 12 | APT | ✅ Ready | debian-package.yml | .deb |
| Debian 11 | APT | ✅ Ready | debian-package.yml | .deb |
| Ubuntu 22.04 | APT | ✅ Ready | debian-package.yml | .deb |
| Ubuntu 24.04 | APT | ✅ Ready | debian-package.yml | .deb |
| NixOS | Nix | ✅ Existing | - | Nix derivation |
| Guix System | Guix | ✅ Existing | - | Guix package |
| Arch Linux | AUR | 🔄 Planned | - | PKGBUILD |
| Fedora | DNF | 🔄 Planned | - | .rpm |
| Docker | Docker Hub | 🔄 Planned | - | Container image |
| Flatpak | Flathub | 🔄 Planned | - | .flatpak |

## Packaging Workflow Architecture

### Windows Build → Package Flow
```
1. Code Push/Release
   ↓
2. wincog.yml (Windows Build)
   ├─→ Build cogutil
   ├─→ Build atomspace
   ├─→ Build moses
   └─→ Upload artifacts
   ↓
3. chocolatey-package.yml
   ├─→ Download artifacts
   ├─→ Create .nupkg
   ├─→ Test installation
   └─→ Publish to Chocolatey
   ↓
4. winget.yml
   ├─→ Generate manifest
   └─→ Submit to winget-pkgs
```

### Linux Build → Package Flow
```
1. Code Push/Release
   ↓
2. debian-package.yml
   ├─→ Install build deps
   ├─→ Build from source
   ├─→ Create .deb package
   ├─→ Run lintian checks
   └─→ Upload artifacts
   ↓
3. PPA Upload (future)
   └─→ Publish to Launchpad
```

## Package Testing

### Automated Tests
- **Windows:** Chocolatey test install in CI
- **Debian:** dpkg install test in CI
- **Lintian:** Debian package quality checks

### Manual Testing Checklist
- [ ] Package installs without errors
- [ ] All binaries are executable
- [ ] Libraries are properly linked
- [ ] Python bindings work
- [ ] Guile bindings work (Linux only)
- [ ] Configuration files in correct locations
- [ ] Documentation installed
- [ ] Package uninstalls cleanly

## Distribution Channels

### Official Channels
- **GitHub Releases:** Primary distribution for all platforms
- **Chocolatey Community:** Windows packages
- **Winget Community:** Windows packages
- **Debian PPA:** Ubuntu/Debian packages (planned)

### Community Channels
- **AUR:** Arch Linux user repository
- **Docker Hub:** Container images
- **Flathub:** Flatpak applications

## Version Management

### Version Scheme
- Format: `MAJOR.MINOR.PATCH` (Semantic Versioning)
- Example: `1.0.0`, `1.1.0`, `1.1.1`

### Release Process
1. Tag release in Git: `git tag -a v1.0.0 -m "Release 1.0.0"`
2. Push tag: `git push origin v1.0.0`
3. GitHub Actions automatically:
   - Builds Windows binaries
   - Creates Chocolatey package
   - Creates Debian package
   - Uploads to GitHub Releases
4. Manual steps:
   - Publish to Chocolatey (if API key configured)
   - Submit to Winget
   - Update AUR package
   - Build Docker images

## Dependencies

### Windows
- Visual C++ Redistributable 2019+
- CMake 3.20+
- Boost 1.71+
- Python 3.8+

### Linux
- GCC 9+ or Clang 10+
- CMake 3.12+
- Boost 1.71+
- Python 3.8+
- Guile 3.0+
- GMP library

## Future Enhancements

### Short-term
- [ ] Enable Chocolatey publishing after Windows builds stable
- [ ] Test Debian packages on all supported distributions
- [ ] Create Ubuntu PPA for easier installation
- [ ] Add ARM64 support for Debian packages

### Medium-term
- [ ] Create Arch Linux AUR package
- [ ] Create Fedora RPM package
- [ ] Build Docker images for containerized deployment
- [ ] Create Flatpak for desktop application

### Long-term
- [ ] Set up package signing for security
- [ ] Create automated package testing infrastructure
- [ ] Support macOS via Homebrew
- [ ] Support BSD systems
- [ ] Create snap packages for Ubuntu

## Package Maintainers

- **Windows (Chocolatey/Winget):** OpenCog Build Bot
- **Debian/Ubuntu:** OpenCog Build Bot
- **Nix:** Community maintained
- **Guix:** Community maintained
- **AUR:** Community maintained (planned)

## Support

For packaging issues, please:
1. Check existing issues: https://github.com/cogpy/occ/issues
2. Create new issue with `packaging` label
3. Include package manager, OS version, and error logs

## References

- Chocolatey Documentation: https://docs.chocolatey.org/
- Winget Documentation: https://learn.microsoft.com/en-us/windows/package-manager/
- Debian Packaging Guide: https://www.debian.org/doc/manuals/maint-guide/
- Nix Manual: https://nixos.org/manual/nix/stable/
- Guix Manual: https://guix.gnu.org/manual/
