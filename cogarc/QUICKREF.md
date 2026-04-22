# CogArc Quick Reference

## Generate New Component (5 seconds)

```bash
cd /path/to/occ
./cogarc/scripts/generate-component.sh my-component "My component description"
```

## Transform Existing Code (30 seconds)

```bash
./cogarc/scripts/transform-to-opencog.py \
  --source /path/to/existing/code \
  --component-name my-component \
  --description "My component"
```

## Common Commands

### Basic Component
```bash
./cogarc/scripts/generate-component.sh simple-tool "Simple utility tool"
```

### Component with Dependencies
```bash
./cogarc/scripts/generate-component.sh my-tool "My tool" \
  --dependencies cogutil,atomspace
```

### Component with GitHub Actions
```bash
./cogarc/scripts/generate-component.sh modern-tool "Modern tool" \
  --with-github-actions
```

### Full Options
```bash
./cogarc/scripts/generate-component.sh my-component "Description" \
  --output-dir /output/path \
  --version 2.0.0 \
  --author "Your Name" \
  --email you@example.com \
  --dependencies cogutil,atomspace,ure \
  --with-guile \
  --with-python \
  --with-github-actions
```

## After Generation

### Build
```bash
cd my-component
mkdir build && cd build
cmake .. && make -j$(nproc)
```

### Test
```bash
cd build
make check
```

### Install
```bash
sudo make install
```

### Package
```bash
cd build
cpack -G DEB  # Debian package
```

## Directory Structure Generated

```
my-component/
├── .circleci/config.yml       # CI configuration
├── .github/workflows/         # GitHub Actions (optional)
├── cmake/                     # CMake modules
├── debian/                    # Debian packaging
├── examples/                  # Usage examples
├── opencog/my-component/      # Source code goes here
│   ├── version.h              # Version information
│   └── CMakeLists.txt         # Build config
├── tests/my-component/        # Tests go here
├── CMakeLists.txt             # Root build config
├── README.md                  # Documentation
├── LICENSE                    # AGPL-3.0
└── AUTHORS                    # Contributors list
```

## File Locations

### Add Source Code
- Headers: `opencog/my-component/*.h`
- Implementation: `opencog/my-component/*.cc`

### Add Tests
- Tests: `tests/my-component/*UTest.cxxtest`

### Add Examples
- Examples: `examples/my-component/`

## Archetypal Pattern Reference

### Core Dependencies (in order)
1. CogUtil (always required)
2. AtomSpace (if using knowledge representation)
3. Other OpenCog components as needed

### Standard Directories
- `.circleci/` - Continuous integration
- `cmake/` - CMake modules and configuration
- `debian/` - Debian packaging files
- `doc/` - Documentation (optional)
- `examples/` - Usage examples
- `lib/` - Additional libraries (usually empty)
- `opencog/[component]/` - Main source code
- `scripts/` - Utility scripts
- `tests/[component]/` - Unit tests

### Required Files
- `CMakeLists.txt` - Root build configuration
- `opencog/[component]/CMakeLists.txt` - Component build config
- `opencog/[component]/version.h` - Version information
- `README.md` - Project documentation
- `LICENSE` - Software license (AGPL-3.0)
- `AUTHORS` - List of contributors
- `.gitignore` - Git ignore patterns

### Optional Files
- `HACKING` - Development guidelines
- `doc/` - Additional documentation
- `debian/` - Debian packaging (for distribution)

## Integration with OCC

### Add to Monorepo
1. Copy component directory to OCC root
2. Update root `CMakeLists.txt`:
   ```cmake
   OPTION(BUILD_MY_COMPONENT "Build My Component" ON)
   IF(BUILD_MY_COMPONENT)
       ADD_SUBDIRECTORY(my-component)
   ENDIF()
   ```
3. Update CI workflows
4. Build and test integration

## Troubleshooting

### Can't find CogUtil
```bash
cd /path/to/occ/cogutil
mkdir build && cd build
cmake .. && make && sudo make install
```

### Build fails
- Check all dependencies are installed
- Review CMakeLists.txt for correct configuration
- Check compiler version (requires C++17)

### Tests fail
- Ensure test files are in `tests/[component]/`
- Check test CMakeLists.txt links libraries correctly
- Verify CxxTest is installed

## Quick Links

- **Main README:** `cogarc/README.md`
- **Detailed Usage:** `cogarc/USAGE.md`
- **Templates:** `cogarc/template/`
- **Scripts:** `cogarc/scripts/`
- **Test Suite:** `cogarc/scripts/test-generation.sh`

## OpenCog Resources

- Wiki: https://wiki.opencog.org/
- GitHub: https://github.com/opencog
- Mailing List: opencog@googlegroups.com
- IRC: #opencog on Libera.Chat

## Semantic Versioning

Format: MAJOR.MINOR.PATCH

- **MAJOR**: Incompatible API changes
- **MINOR**: Added functionality (backward compatible)
- **PATCH**: Bug fixes (backward compatible)

Start new components at `1.0.0` or `0.1.0` for pre-release.
