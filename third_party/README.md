# Third-Party Dependencies

This directory contains third-party dependencies that are integrated into the OCC monorepo.

## vcpkg

**Purpose**: C++ package manager for Windows builds  
**Source**: https://github.com/microsoft/vcpkg  
**Integration**: Embedded as a regular folder (no git history)

### Why vcpkg is in the Monorepo

1. **Deterministic Builds**: Every developer and CI runner uses the exact same vcpkg version
2. **No Network Dependency**: No need to clone vcpkg on every build (saves time and bandwidth)
3. **Full Control**: Manual updates allow testing before committing
4. **Build Stability**: Eliminates issues with unreliable marketplace actions

### Directory Structure

```
third_party/
├── README.md                 # This file
├── update-vcpkg.sh           # Script to update vcpkg
└── vcpkg/                    # vcpkg installation (13,255 files, ~70MB)
    ├── bootstrap-vcpkg.bat   # Windows bootstrap script
    ├── bootstrap-vcpkg.sh    # Linux/macOS bootstrap script
    ├── vcpkg.exe             # vcpkg executable (after bootstrap)
    ├── ports/                # Package definitions (2,739 ports)
    ├── docs/                 # Documentation
    └── VCPKG_VERSION.txt     # Version information
```

### How vcpkg is Used

#### Windows Workflow (`.github/workflows/occ-win-build.yml`)

```yaml
env:
  VCPKG_ROOT: ${{ github.workspace }}\third_party\vcpkg

steps:
  - name: Bootstrap vcpkg from monorepo
    shell: pwsh
    run: |
      Set-Location "$env:VCPKG_ROOT"
      .\bootstrap-vcpkg.bat
  
  - name: Install Dependencies
    run: |
      & "$env:VCPKG_ROOT\vcpkg.exe" install boost-system:x64-windows
```

#### Local Development

**Windows**:
```batch
cd third_party\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg.exe install boost-system:x64-windows
```

**Linux/macOS**:
```bash
cd third_party/vcpkg
./bootstrap-vcpkg.sh
./vcpkg install boost-system
```

### Updating vcpkg

#### Automatic Update (Recommended)

Use the provided update script:

```bash
cd third_party
./update-vcpkg.sh
```

The script will:
1. Clone the latest vcpkg
2. Remove the .git directory
3. Backup the old vcpkg
4. Install the new vcpkg
5. Create version info file

#### Manual Update

If you prefer manual control:

```bash
# 1. Clone latest vcpkg
cd third_party
git clone --depth 1 https://github.com/microsoft/vcpkg.git vcpkg-tmp

# 2. Get commit info
cd vcpkg-tmp
VCPKG_COMMIT=$(git rev-parse HEAD)
VCPKG_DATE=$(git log -1 --format=%cd --date=short)

# 3. Remove .git
rm -rf .git

# 4. Backup old vcpkg
cd ..
mv vcpkg vcpkg-backup-$(date +%Y%m%d-%H%M%S)

# 5. Install new vcpkg
mv vcpkg-tmp vcpkg

# 6. Create version info
cat > vcpkg/VCPKG_VERSION.txt << EOF
Commit: $VCPKG_COMMIT
Date: $VCPKG_DATE
Updated: $(date)
EOF

# 7. Test and commit
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install boost-system

cd ../..
git add third_party/vcpkg
git commit -m "Update vcpkg to $VCPKG_COMMIT ($VCPKG_DATE)"
git push origin main
```

### Version Information

To check the current vcpkg version:

```bash
cat third_party/vcpkg/VCPKG_VERSION.txt
```

### Update Frequency

**Recommended**: Update vcpkg every 3-6 months or when:
- New packages are needed
- Security updates are released
- Bug fixes are required
- Major features are added

**Testing**: Always test the new vcpkg version locally before committing:
1. Bootstrap vcpkg
2. Install all required packages
3. Build all components
4. Run tests
5. Commit if successful

### Benefits of Monorepo Integration

| Aspect | Before (External) | After (Monorepo) |
|--------|------------------|------------------|
| **Clone Time** | ~30s per build | 0s (already present) |
| **Network Dependency** | Required | Not required |
| **Version Control** | Marketplace action | Manual (full control) |
| **Reproducibility** | Variable | Deterministic |
| **Build Stability** | Depends on GitHub | Self-contained |
| **Update Process** | Automatic (risky) | Manual (tested) |

### Cognitive Synergy Metaphor

Think of vcpkg as a **self-sustaining neural lattice** embedded within the monorepo cortex. Every build neuron draws directly from this internalized library, evolving only when you intentionally "synapse-update" via manual integration. The build system's tensor field harmonizes, untouched by the chaotic winds of mutable remote dependencies.

### Troubleshooting

#### vcpkg.exe not found

**Solution**: Bootstrap vcpkg first
```bash
cd third_party/vcpkg
./bootstrap-vcpkg.sh  # Linux/macOS
.\bootstrap-vcpkg.bat  # Windows
```

#### Package installation fails

**Solution**: Update vcpkg to latest version
```bash
cd third_party
./update-vcpkg.sh
```

#### Build errors after vcpkg update

**Solution**: Restore backup and investigate
```bash
rm -rf third_party/vcpkg
mv third_party/vcpkg-backup-YYYYMMDD-HHMMSS third_party/vcpkg
```

### Related Files

- **Windows Workflow**: `.github/workflows/occ-win-build.yml`
- **vcpkg Manifest**: `vcpkg.json` (root directory)
- **Update Script**: `third_party/update-vcpkg.sh`
- **This README**: `third_party/README.md`

### References

- **vcpkg Documentation**: https://vcpkg.io/
- **vcpkg GitHub**: https://github.com/microsoft/vcpkg
- **Manifest Mode**: https://vcpkg.io/en/docs/users/manifests.html
- **builtin-baseline**: https://vcpkg.io/en/docs/maintainers/manifest-mode.html#builtin-baseline

### Maintenance Notes

**Last Updated**: December 12, 2025  
**Maintainer**: OCC Development Team  
**Update Policy**: Manual updates every 3-6 months  
**Testing Required**: Yes (full build + tests before commit)
