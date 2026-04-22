# Version Management Guide

This document describes the version management strategy for the AGI-OS/OCC repository.

## Version Standardization Policy

All components follow semantic versioning (MAJOR.MINOR.PATCH) with the following standardization:

### Version Categories

#### 1. Stable Components (Keep Current Versions)

These components have established version numbers that should be maintained:

| Component | Version | Type | Notes |
|-----------|---------|------|-------|
| cognumach | 1.8.0 | Fixed | GNU Mach base version |
| hurdcog | 0.9.0 | Fixed | GNU Hurd base version |
| cogutil | 2.3.1 | Dynamic | Read from version.cmake |
| atomspace | 5.0.4 | Dynamic | Read from version.cmake |
| cogserver | 3.3.0 | Fixed | Stable release |
| matrix | 1.3.0 | Fixed | Synchronized with storage |
| atomspace-storage | 1.3.0 | Fixed | Storage backend base |
| atomspace-rocks | 1.3.0 | Fixed | RocksDB backend |
| atomspace-pgres | 1.3.0 | Fixed | PostgreSQL backend |
| unify | 0.1.7 | Dynamic | Read from version.cmake |
| ure | 0.3.3 | Dynamic | Read from version.cmake |
| miner | 1.0.0 | Fixed | Release version |
| sensory | 0.4.0 | Fixed | Development version |
| asmoses | 3.4.0 | Fixed | MOSES version |

#### 2. Standardized to 1.0.0

These components were updated to 1.0.0 as part of the standardization effort:

| Component | Old Version | New Version | Reason |
|-----------|-------------|-------------|--------|
| atomspace-cog | 0.1.4 | 1.0.0 | Release milestone |
| pln | 0.1.1 | 1.0.0 | Release milestone |
| attention | 0.1.4 | 1.0.0 | Release milestone |
| learn | (none) | 1.0.0 | Added for consistency |
| agents | (none) | 1.0.0 | Added for consistency |
| coggml | 0.1.0 | 1.0.0 | Release milestone |
| cogself | 0.1.0 | 1.0.0 | Release milestone |
| atomspace-accelerator | 0.1.0 | 1.0.0 | Release milestone |
| agentic-chatbots | 0.1.0 | 1.0.0 | Release milestone |

## Version Declaration Methods

### Method 1: Fixed Version in CMakeLists.txt

For components with stable versions:

```cmake
PROJECT(component-name)

# Version information
SET(SEMANTIC_VERSION 1.0.0)
```

### Method 2: Dynamic Version from File

For components that read version from external files (cogutil, atomspace, unify, ure):

```cmake
PROJECT(component-name)

# Read version from version.cmake
FILE(READ "${CMAKE_SOURCE_DIR}/version.cmake" VERSION_FILE)
STRING(REGEX MATCH "SEMANTIC_VERSION ([0-9]+\\.[0-9]+\\.[0-9]+)" _ ${VERSION_FILE})
SET(SEMANTIC_VERSION ${CMAKE_MATCH_1})
```

### Method 3: PROJECT VERSION Parameter

For newer components using CMake 3.12+ features:

```cmake
PROJECT(component-name
    VERSION 1.0.0
    DESCRIPTION "Component description"
    LANGUAGES CXX
)

# Also set SEMANTIC_VERSION for compatibility
SET(SEMANTIC_VERSION 1.0.0)
```

## Version Checking

### Manual Version Check

Check all component versions:

```bash
cd /path/to/occ
python3 scripts/standardize-versions.py --report
```

### Automated Version Verification

Add to CI/CD pipeline:

```yaml
- name: Verify Version Consistency
  run: |
    python3 scripts/standardize-versions.py --report
    if [ $? -ne 0 ]; then
      echo "Version inconsistencies detected!"
      exit 1
    fi
```

## Version Update Workflow

When updating a component version:

1. **Update CMakeLists.txt**
   ```cmake
   SET(SEMANTIC_VERSION x.y.z)
   ```

2. **Update docs/BUILD_SEQUENCES.md**
   Update the version table for the component.

3. **Update CHANGELOG.md**
   Document the version change and what changed.

4. **Update guix.scm** (if applicable)
   Update package version in Guix package definition.

5. **Run Tests**
   ```bash
   make test
   ./synergy.sh
   ```

6. **Commit**
   ```bash
   git add <modified-files>
   git commit -m "Update <component> to version x.y.z"
   ```

## Synchronization Groups

Some components should maintain synchronized versions:

### Storage Backend Group (1.3.0)
- atomspace-storage
- atomspace-rocks
- atomspace-pgres
- matrix

When updating one, consider updating all in the group.

### Foundation Group (Dynamic)
- cogutil (2.3.1)
- atomspace (5.0.4)

These read versions from external files. Update version.cmake files.

### Reasoning Group (Dynamic)
- unify (0.1.7)
- ure (0.3.3)

These also use dynamic versioning.

### Cognitive Architecture Group (1.0.0)
- coggml
- cogself
- atomspace-accelerator
- agentic-chatbots

These were standardized together for the 1.0.0 release.

## Version Increment Guidelines

Follow semantic versioning principles:

### MAJOR version (x.0.0)
Increment when:
- Breaking API changes
- Major architecture changes
- Incompatible storage format changes

### MINOR version (0.x.0)
Increment when:
- New features added (backward compatible)
- Significant enhancements
- New algorithms or capabilities

### PATCH version (0.0.x)
Increment when:
- Bug fixes
- Performance improvements
- Documentation updates
- Minor enhancements

## Special Cases

### AGI-OS Layers

The three-layer architecture has specific versioning:

- **Layer 1 (Cognumach)**: Follows GNU Mach versioning (currently 1.8)
- **Layer 2 (HurdCog)**: Follows GNU Hurd versioning (currently 0.9)
- **Layer 3 (OCC)**: Independent versioning per component

### Autotools-Based Components

Cognumach and HurdCog use autotools and define versions in:
- `cognumach/version.m4`
- `hurdcog/configure.ac`

CMakeLists.txt wrappers should match these versions.

## Tools

### standardize-versions.py

Usage:
```bash
# Generate report
python3 scripts/standardize-versions.py --report

# Show what would be changed (dry run)
python3 scripts/standardize-versions.py

# Apply changes (interactive)
python3 scripts/standardize-versions.py --apply
```

### grep-versions.sh

Quick script to find all version declarations:

```bash
#!/bin/bash
find . -name "CMakeLists.txt" -exec grep -H "SEMANTIC_VERSION" {} \;
```

## Integration with Build System

### CMake

The root CMakeLists.txt reports all component versions in the build summary.

### Guix

The guix.scm file must be kept in sync with CMakeLists.txt versions.

### Makefile

The Makefile displays the project version (AGI-OS 1.0.0).

## Future Enhancements

### Planned Improvements

1. **Automated Version Bumping**
   - Script to bump versions across synchronized groups
   - Automated CHANGELOG generation

2. **Version Validation**
   - Pre-commit hook to verify version consistency
   - CI check for version synchronization

3. **Release Automation**
   - Automated tagging based on version updates
   - Release notes generation from CHANGELOG

4. **Version Pinning**
   - Lock file for reproducible builds
   - Version compatibility matrix

## References

- Semantic Versioning: https://semver.org/
- CMake PROJECT command: https://cmake.org/cmake/help/latest/command/project.html
- GNU Guix packaging: https://guix.gnu.org/manual/en/html_node/Defining-Packages.html

---

*Last Updated: 2025-12-06*  
*Document Version: 1.0.0*
