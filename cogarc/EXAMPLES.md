# CogArc Examples

This document demonstrates real-world usage of the CogArc component generation system.

## Example 1: Simple Utility Component

Generate a basic utility component with minimal dependencies:

```bash
./cogarc/scripts/generate-component.sh simple-utils "Simple utility functions"
```

**Generated Structure:**
```
simple-utils/
├── .circleci/config.yml
├── .github/workflows/
├── cmake/
│   ├── CMakeLists.txt
│   ├── OpenCogGccOptions.cmake
│   ├── SimpleUtilsConfig.cmake.in
│   └── Summary.cmake
├── debian/
│   ├── changelog
│   ├── compat
│   ├── control
│   ├── copyright
│   ├── libsimple-utils-dev.install
│   ├── libsimple-utils.install
│   └── rules
├── examples/
│   └── CMakeLists.txt
├── opencog/
│   ├── CMakeLists.txt
│   └── simple-utils/
│       ├── CMakeLists.txt
│       └── version.h
├── scripts/
├── tests/
│   ├── CMakeLists.txt
│   └── simple-utils/
│       └── CMakeLists.txt
├── .gitignore
├── AUTHORS
├── CMakeLists.txt
├── LICENSE
└── README.md
```

**Add functionality:**

```bash
cd simple-utils

# Add header file
cat > opencog/simple-utils/string_utils.h << 'EOF'
#ifndef _OPENCOG_SIMPLE_UTILS_STRING_UTILS_H
#define _OPENCOG_SIMPLE_UTILS_STRING_UTILS_H

#include <string>

namespace opencog {
namespace simple_utils {

// Convert string to uppercase
std::string to_upper(const std::string& str);

// Convert string to lowercase  
std::string to_lower(const std::string& str);

} // namespace simple_utils
} // namespace opencog

#endif
EOF

# Add implementation
cat > opencog/simple-utils/string_utils.cc << 'EOF'
#include "string_utils.h"
#include <algorithm>
#include <cctype>

namespace opencog {
namespace simple_utils {

std::string to_upper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::toupper(c); });
    return result;
}

std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return result;
}

} // namespace simple_utils
} // namespace opencog
EOF

# Update CMakeLists.txt
cat > opencog/simple-utils/CMakeLists.txt << 'EOF'
ADD_LIBRARY(simple-utils
    string_utils.cc
)

TARGET_LINK_LIBRARIES(simple-utils
    ${COGUTIL_LIBRARIES}
)

INSTALL(TARGETS simple-utils
    EXPORT SimpleUtilsTargets
    LIBRARY DESTINATION lib${LIB_DIR_SUFFIX}/simple-utils
    ARCHIVE DESTINATION lib${LIB_DIR_SUFFIX}/simple-utils
)

INSTALL(FILES
    version.h
    string_utils.h
    DESTINATION include/opencog/simple-utils
)
EOF
```

**Build and test:**
```bash
mkdir build && cd build
cmake .. && make -j$(nproc)
sudo make install
```

## Example 2: AtomSpace Extension Component

Generate a component that extends AtomSpace functionality:

```bash
./cogarc/scripts/generate-component.sh atom-analyzer "AtomSpace analysis tools" \
  --dependencies cogutil,atomspace
```

**Key CMakeLists.txt changes automatically applied:**
```cmake
# AtomSpace dependency added automatically
FIND_PACKAGE(AtomSpace CONFIG)
IF (ATOMSPACE_FOUND)
    MESSAGE(STATUS "AtomSpace version ${ATOMSPACE_VERSION} found.")
    ADD_DEFINITIONS(-DHAVE_ATOMSPACE)
    SET(HAVE_ATOMSPACE 1)
ELSE ()
    MESSAGE(FATAL_ERROR "AtomSpace missing: it is needed!")
ENDIF ()
```

## Example 3: Advanced Reasoning Component

Generate a component with multiple dependencies:

```bash
./cogarc/scripts/generate-component.sh advanced-reasoner "Advanced reasoning module" \
  --dependencies cogutil,atomspace,unify,ure \
  --version 0.5.0 \
  --author "Research Team" \
  --email research@example.com
```

**Debian packaging automatically configured:**
```debian
Build-Depends: debhelper (>= 13),
               cmake (>= 3.12),
               libcogutil-dev (>= 2.1.0),
               libatomspace-dev,
               libunify-dev,
               libure-dev,
```

## Example 4: Modern Component with GitHub Actions

Generate a component using GitHub Actions instead of CircleCI:

```bash
./cogarc/scripts/generate-component.sh modern-component "Modern OpenCog component" \
  --with-github-actions \
  --author "Your Name" \
  --email you@example.com
```

**Generated workflow (`.github/workflows/build.yml`):**
```yaml
name: Build and Test ModernComponent

on:
  push:
    branches: [ main, master, develop ]
  pull_request:
    branches: [ main, master, develop ]

jobs:
  build-ubuntu:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake cxxtest libboost-dev libcogutil-dev
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    - name: Build
      run: cmake --build build -j$(nproc)
    - name: Run tests
      run: cd build && ctest --output-on-failure
```

## Example 5: Transform Existing Code

Transform an existing C++ library into an OpenCog component:

Assume you have an existing library at `/home/user/projects/my-algorithm`:

```
my-algorithm/
├── src/
│   ├── algorithm.h
│   ├── algorithm.cpp
│   └── utils.cpp
├── tests/
│   └── test_algorithm.cpp
└── CMakeLists.txt (old format)
```

**Transform it:**
```bash
./cogarc/scripts/transform-to-opencog.py \
  --source /home/user/projects/my-algorithm \
  --component-name my-algorithm \
  --description "Advanced machine learning algorithm" \
  --dependencies cogutil,atomspace \
  --version 2.0.0 \
  --author "Original Author" \
  --email author@example.com
```

**What happens:**
1. Analyzes source structure
2. Creates OpenCog directory layout
3. Copies source files to `opencog/my-algorithm/`
4. Copies test files to `tests/my-algorithm/`
5. Generates OpenCog-compliant CMakeLists.txt
6. Creates debian packaging
7. Generates INTEGRATION_GUIDE.md

**Output:**
```
my-algorithm/
├── .circleci/config.yml
├── cmake/
├── debian/
├── opencog/my-algorithm/
│   ├── algorithm.h           # Copied from src/
│   ├── algorithm.cpp         # Copied from src/
│   ├── utils.cpp             # Copied from src/
│   ├── version.h             # Generated
│   └── CMakeLists.txt        # Generated
├── tests/my-algorithm/
│   ├── test_algorithm.cpp    # Copied from tests/
│   └── CMakeLists.txt        # Generated
├── CMakeLists.txt            # Generated (OpenCog format)
├── INTEGRATION_GUIDE.md      # Step-by-step integration instructions
└── README.md                 # Generated
```

## Example 6: Component with Language Bindings

Generate a component with Guile and Python bindings:

```bash
./cogarc/scripts/generate-component.sh polyglot-utils "Utilities with language bindings" \
  --with-guile \
  --with-python
```

**Then add bindings manually:**

```bash
cd polyglot-utils

# Add Guile bindings directory
mkdir -p opencog/polyglot-utils/guile

# Add Python bindings directory
mkdir -p opencog/polyglot-utils/cython

# Update CMakeLists.txt to build bindings
# (bindings implementation left as exercise)
```

## Example 7: Full-Featured Research Component

A complete example for a research project:

```bash
./cogarc/scripts/generate-component.sh neural-symbolic "Neural-symbolic integration framework" \
  --dependencies cogutil,atomspace,unify,ure,pln \
  --version 0.1.0 \
  --author "Research Lab" \
  --email lab@university.edu \
  --with-guile \
  --with-python \
  --with-github-actions
```

**Add research code:**

```bash
cd neural-symbolic

# Add main implementation
cat > opencog/neural-symbolic/integration.h << 'EOF'
#ifndef _OPENCOG_NEURAL_SYMBOLIC_INTEGRATION_H
#define _OPENCOG_NEURAL_SYMBOLIC_INTEGRATION_H

#include <opencog/atomspace/AtomSpace.h>

namespace opencog {
namespace neural_symbolic {

class Integration {
public:
    Integration(AtomSpacePtr as);
    
    // Neural network to symbolic conversion
    Handle neural_to_symbolic(const std::vector<float>& activations);
    
    // Symbolic to neural network conversion
    std::vector<float> symbolic_to_neural(Handle h);
    
private:
    AtomSpacePtr _atomspace;
};

} // namespace neural_symbolic
} // namespace opencog

#endif
EOF

# Add tests
cat > tests/neural-symbolic/IntegrationUTest.cxxtest << 'EOF'
#include <opencog/neural-symbolic/integration.h>
#include <cxxtest/TestSuite.h>

using namespace opencog;
using namespace opencog::neural_symbolic;

class IntegrationUTest : public CxxTest::TestSuite {
public:
    void testBasic() {
        AtomSpacePtr as = createAtomSpace();
        Integration integrator(as);
        
        // Test basic conversion
        std::vector<float> activations = {0.5, 0.8, 0.2};
        Handle h = integrator.neural_to_symbolic(activations);
        
        TS_ASSERT(h != Handle::UNDEFINED);
    }
};
EOF
```

## Integration with OCC Monorepo

After generating a component, integrate it into the OCC monorepo:

```bash
# 1. Copy component to OCC
cp -r my-component /path/to/occ/

# 2. Update root CMakeLists.txt
echo "
# My Component
OPTION(BUILD_MY_COMPONENT \"Build My Component\" ON)
IF(BUILD_MY_COMPONENT)
    ADD_SUBDIRECTORY(my-component)
ENDIF()
" >> /path/to/occ/CMakeLists.txt

# 3. Update CI workflow
# Edit .github/workflows/ocall.yml to add build step

# 4. Build entire stack
cd /path/to/occ
mkdir build && cd build
cmake -DBUILD_MY_COMPONENT=ON .. && make -j$(nproc)
```

## Creating Test Suite for Generated Component

Add comprehensive tests:

```bash
cd my-component/tests/my-component/

# Add unit test
cat > BasicUTest.cxxtest << 'EOF'
#include <cxxtest/TestSuite.h>
#include <opencog/my-component/version.h>

class BasicUTest : public CxxTest::TestSuite {
public:
    void testVersion() {
        TS_ASSERT_DIFFERS(MY_COMPONENT_VERSION_STRING, "");
    }
};
EOF

# Update CMakeLists.txt
echo "
ADD_CXXTEST(BasicUTest)
TARGET_LINK_LIBRARIES(BasicUTest my-component)
" >> CMakeLists.txt

# Run tests
cd ../../build
make check
```

## Building Debian Packages

Generate and build Debian packages:

```bash
cd my-component/build
cpack -G DEB

# Install generated packages
sudo dpkg -i libmy-component_1.0.0_amd64.deb
sudo dpkg -i libmy-component-dev_1.0.0_amd64.deb
```

## Continuous Integration Setup

### CircleCI Setup:
1. Push component to GitHub
2. Enable project in CircleCI
3. CircleCI will use `.circleci/config.yml`

### GitHub Actions Setup:
1. Component already has `.github/workflows/build.yml`
2. Push to GitHub
3. Actions run automatically on push/PR

## Tips and Best Practices

1. **Start Simple**: Begin with basic component, add features incrementally
2. **Test Early**: Add tests as you develop functionality
3. **Document**: Update README.md with actual usage examples
4. **Version Properly**: Follow semantic versioning
5. **Dependencies**: Minimize dependencies, only add what's needed
6. **Build Often**: Test builds frequently during development
7. **CI/CD**: Set up continuous integration early

## Common Patterns

### Pattern 1: Utility Library
- Minimal dependencies (just cogutil)
- Pure C++ utility functions
- No AtomSpace interaction

### Pattern 2: AtomSpace Extension
- Depends on cogutil + atomspace
- Extends AtomSpace functionality
- May use pattern matcher

### Pattern 3: Reasoning Module
- Depends on cogutil + atomspace + ure/pln
- Implements reasoning algorithms
- Uses rule engine

### Pattern 4: Integration Component
- Multiple dependencies
- Bridges different systems
- May have external dependencies

## Troubleshooting Examples

### Problem: Build fails with "CogUtil not found"
```bash
# Solution: Install CogUtil first
cd /path/to/occ/cogutil
mkdir build && cd build
cmake .. && make && sudo make install
```

### Problem: Tests won't compile
```bash
# Solution: Check CxxTest is installed
sudo apt-get install cxxtest

# Reconfigure
cd build
cmake .. && make
```

### Problem: Package install conflicts
```bash
# Solution: Remove old version first
sudo dpkg -r libmy-component libmy-component-dev

# Then install new version
sudo dpkg -i *.deb
```

## Reference Generated Component

See `/tmp/cogarc-examples/example-utility/` for a reference implementation generated by the test suite.

---

For more examples and patterns, see the OpenCog component repositories:
- https://github.com/opencog/cogutil
- https://github.com/opencog/atomspace
- https://github.com/opencog/ure
- https://github.com/opencog/pln
