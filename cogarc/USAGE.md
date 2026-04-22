# CogArc Usage Guide

## Quick Start

### Generate a New Component

The simplest way to create a new OpenCog component:

```bash
cd /path/to/occ
./cogarc/scripts/generate-component.sh my-utility "Utility functions for OpenCog"
```

This creates a `my-utility/` directory with the complete OpenCog component structure.

### Transform Existing Code

If you have existing C/C++ code to convert into an OpenCog component:

```bash
./cogarc/scripts/transform-to-opencog.py \
  --source /path/to/existing/code \
  --component-name my-utility \
  --description "Utility functions for OpenCog"
```

## Detailed Usage

### generate-component.sh

Generate a new OpenCog component from scratch.

**Basic Usage:**
```bash
./generate-component.sh <component-name> <description>
```

**With All Options:**
```bash
./generate-component.sh my-component "My component description" \
  --output-dir /path/to/output \
  --version 2.0.0 \
  --author "John Doe" \
  --email john@example.com \
  --dependencies cogutil,atomspace,ure \
  --with-guile \
  --with-python \
  --with-github-actions
```

**Options:**

| Option | Description | Default |
|--------|-------------|---------|
| `--output-dir <dir>` | Where to create the component | Current directory |
| `--version <version>` | Initial semantic version | 1.0.0 |
| `--author <name>` | Author name | OpenCog Community |
| `--email <email>` | Contact email | opencog@googlegroups.com |
| `--dependencies <list>` | Comma-separated dependencies | cogutil |
| `--with-guile` | Include Guile bindings | false |
| `--with-python` | Include Python bindings | false |
| `--with-github-actions` | Use GitHub Actions instead of CircleCI | false |

**Examples:**

1. **Minimal component:**
   ```bash
   ./generate-component.sh simple-utils "Simple utility functions"
   ```

2. **Component with AtomSpace dependency:**
   ```bash
   ./generate-component.sh atom-tools "AtomSpace manipulation tools" \
     --dependencies cogutil,atomspace
   ```

3. **Component with GitHub Actions:**
   ```bash
   ./generate-component.sh modern-component "Modern OpenCog component" \
     --with-github-actions \
     --author "Your Name" \
     --email you@example.com
   ```

4. **Full-featured component:**
   ```bash
   ./generate-component.sh advanced-reasoning "Advanced reasoning module" \
     --dependencies cogutil,atomspace,unify,ure \
     --with-guile \
     --with-python \
     --version 0.1.0
   ```

### transform-to-opencog.py

Transform existing code into an OpenCog component.

**Basic Usage:**
```bash
./transform-to-opencog.py \
  --source /path/to/code \
  --component-name component-name \
  --description "Component description"
```

**With All Options:**
```bash
./transform-to-opencog.py \
  --source /path/to/existing/code \
  --component-name my-component \
  --description "My transformed component" \
  --output-dir /path/to/output \
  --dependencies cogutil,atomspace \
  --version 1.5.0 \
  --author "John Doe" \
  --email john@example.com
```

**What It Does:**

1. Analyzes the source directory structure
2. Identifies source, header, test, and example files
3. Creates OpenCog component structure
4. Copies files to appropriate locations
5. Generates CMake configuration
6. Creates integration guide

**Examples:**

1. **Simple library transformation:**
   ```bash
   ./transform-to-opencog.py \
     --source ~/projects/my-library \
     --component-name my-library \
     --description "My useful library"
   ```

2. **Complex project transformation:**
   ```bash
   ./transform-to-opencog.py \
     --source ~/projects/advanced-ai \
     --component-name advanced-ai \
     --description "Advanced AI algorithms" \
     --dependencies cogutil,atomspace,pln \
     --version 2.0.0
   ```

## After Generation

### 1. Add Your Code

For new components:
```bash
cd my-component
# Add source files
cat > opencog/my-component/example.h << 'EOF'
#ifndef _OPENCOG_MY_COMPONENT_EXAMPLE_H
#define _OPENCOG_MY_COMPONENT_EXAMPLE_H

namespace opencog {
    void example_function();
}

#endif
EOF

cat > opencog/my-component/example.cc << 'EOF'
#include "example.h"
#include <iostream>

namespace opencog {
    void example_function() {
        std::cout << "Hello from my-component!" << std::endl;
    }
}
EOF
```

Update `opencog/my-component/CMakeLists.txt`:
```cmake
ADD_LIBRARY(my-component
    example.cc
)

TARGET_LINK_LIBRARIES(my-component
    ${COGUTIL_LIBRARIES}
)

INSTALL(FILES
    version.h
    example.h
    DESTINATION include/opencog/my-component
)
```

### 2. Build the Component

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### 3. Run Tests

```bash
cd build
make check
# or
ctest --output-on-failure
```

### 4. Install

```bash
sudo make install
```

### 5. Create a Package

```bash
cd build
cpack -G DEB  # Debian package
# or
cpack -G RPM  # RPM package
```

## Integration with OCC Monorepo

To add your component to the OCC monorepo:

### 1. Copy Component

```bash
cp -r my-component /path/to/occ/
```

### 2. Update Root CMakeLists.txt

Edit `/path/to/occ/CMakeLists.txt`:

```cmake
# Add your component
OPTION(BUILD_MY_COMPONENT "Build My Component" ON)

IF(BUILD_MY_COMPONENT)
    ADD_SUBDIRECTORY(my-component)
ENDIF()
```

### 3. Update CI Workflow

Edit `.github/workflows/ocall.yml` or similar:

```yaml
- name: Build My Component
  run: |
    cd my-component
    mkdir build
    cd build
    cmake ..
    make -j$(nproc)
    make check
```

### 4. Test Integration

```bash
cd /path/to/occ
mkdir build
cd build
cmake -DBUILD_MY_COMPONENT=ON ..
make -j$(nproc)
```

## Customization

### Modify Templates

Templates are in `cogarc/template/`. Edit them to change generated structure:

- `CMakeLists.txt` - Root CMake configuration
- `README.md` - Documentation template
- `opencog/version.h` - Version header template
- `.circleci/config.yml` - CircleCI configuration
- `.github/workflows/build.yml` - GitHub Actions workflow
- `debian/*` - Debian packaging templates

After modifying templates, regenerate components to use new templates.

### Add Custom CMake Modules

Add custom CMake modules to `cogarc/template/cmake/`:

```bash
cat > cogarc/template/cmake/FindMyLibrary.cmake << 'EOF'
# Find MyLibrary
FIND_PATH(MYLIBRARY_INCLUDE_DIR mylibrary.h)
FIND_LIBRARY(MYLIBRARY_LIBRARY NAMES mylibrary)
EOF
```

### Extend Generation Script

The generation script is in `cogarc/scripts/generate-component.sh`. You can:

- Add new command-line options
- Generate additional files
- Customize substitution logic

## Best Practices

### Component Naming

- Use lowercase with hyphens: `my-component`
- Be descriptive but concise
- Follow OpenCog naming conventions

### Versioning

- Follow semantic versioning: MAJOR.MINOR.PATCH
- Start at 0.1.0 for initial development
- Increment appropriately for changes

### Dependencies

- Minimize dependencies
- Always depend on cogutil
- Document why each dependency is needed

### Documentation

- Write clear README.md
- Add inline code comments
- Provide usage examples
- Document public API

### Testing

- Write unit tests for all functionality
- Aim for high test coverage
- Test edge cases
- Use CxxTest framework

### CI/CD

- Set up continuous integration early
- Run tests on every commit
- Generate test reports
- Monitor test failures

## Troubleshooting

### Build Failures

**Problem:** CMake can't find CogUtil

**Solution:**
```bash
# Install CogUtil first
cd /path/to/occ/cogutil
mkdir build && cd build
cmake .. && make && sudo make install
```

**Problem:** Source files not found

**Solution:** Check `opencog/COMPONENT/CMakeLists.txt` lists all source files.

### Test Failures

**Problem:** Tests won't build

**Solution:** Ensure CxxTest is installed:
```bash
sudo apt-get install cxxtest
```

**Problem:** Tests fail at runtime

**Solution:** Check test has proper includes and links to component library.

### Packaging Issues

**Problem:** Debian package build fails

**Solution:** Review `debian/control` for correct dependencies.

### CircleCI Issues

**Problem:** CircleCI build fails

**Solution:** 
1. Check `.circleci/config.yml` syntax
2. Ensure Docker image has dependencies
3. Review CircleCI build logs

## Advanced Topics

### Multi-Language Bindings

Add Guile bindings:
```bash
mkdir -p opencog/my-component/guile
# Add Guile wrapper code
```

Add Python bindings:
```bash
mkdir -p opencog/my-component/cython
# Add Cython wrapper code
```

### Custom Build Types

Add custom build type in CMakeLists.txt:
```cmake
IF (CMAKE_BUILD_TYPE STREQUAL "Custom")
    ADD_COMPILE_OPTIONS(-O2 -g -DCUSTOM_BUILD)
ENDIF()
```

### Integration Testing

Create integration tests:
```bash
mkdir tests/integration
# Add integration test code
```

## Support and Resources

- **CogArc README:** `cogarc/README.md`
- **OpenCog Wiki:** https://wiki.opencog.org/
- **Mailing List:** opencog@googlegroups.com
- **GitHub Issues:** https://github.com/opencog/occ/issues
- **IRC:** #opencog on Libera.Chat

## Contributing

To improve CogArc:

1. Analyze more OpenCog components
2. Identify missing patterns
3. Update templates
4. Test with various component types
5. Submit pull request

See `CONTRIBUTING.md` in the main repository.
