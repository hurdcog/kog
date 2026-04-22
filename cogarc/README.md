# CogArc: OpenCog Component Archetype Template

## Overview

CogArc is a generative template system for creating OpenCog C/C++ components that conform to the standard OpenCog architectural patterns. This archetype was derived by analyzing the fundamental structure common to all core OpenCog repositories (cogutil, atomspace, cogserver, ure, pln, asmoses, etc.).

## Archetypal Structure

The invariant structure common to all OpenCog C/C++ components is:

```
component-name/
├── .circleci/
│   └── config.yml              # CircleCI continuous integration configuration
├── cmake/
│   ├── CMakeLists.txt          # CMake modules installation
│   ├── [Component]Config.cmake.in  # CMake package configuration template
│   ├── OpenCog*.cmake          # OpenCog-specific CMake macros
│   └── Find*.cmake             # CMake find modules for dependencies
├── debian/
│   ├── changelog               # Debian package changelog
│   ├── compat                  # Debian compatibility level
│   ├── control                 # Debian package metadata
│   ├── copyright               # Debian copyright information
│   ├── rules                   # Debian build rules
│   ├── lib[component].install  # Library installation manifest
│   └── lib[component]-dev.install  # Development files manifest
├── doc/                        # Optional: Documentation
│   ├── doxydoc/                # Doxygen configuration
│   └── README.*                # Platform-specific build instructions
├── examples/
│   └── [component-name]/       # Example code and usage demonstrations
├── lib/                        # Empty directory for CMake modules
├── opencog/
│   └── [component-name]/       # Main source code directory
│       ├── *.h                 # Header files
│       ├── *.cc                # Implementation files
│       └── CMakeLists.txt      # Build configuration for this module
├── scripts/
│   └── [component-name]/       # Utility scripts
├── tests/
│   └── [component-name]/       # Unit tests
│       ├── *UTest.cxxtest      # CxxTest unit tests
│       └── CMakeLists.txt      # Test build configuration
├── .gitignore                  # Git ignore patterns
├── AUTHORS                     # List of contributors
├── CMakeLists.txt              # Root CMake configuration
├── LICENSE                     # License file (typically AGPL-3.0)
├── README.md                   # Component documentation
└── [HACKING]                   # Optional: Development guidelines
```

## Core Design Principles

### 1. CMake Build System
- Minimum CMake version: 3.12
- Support for multiple build types: Release, Debug, Coverage, Profile, RelWithDebInfo
- Semantic versioning extracted from header files
- Dependency detection via `FIND_PACKAGE()`
- Modular build with conditional compilation options

### 2. Dependency Management
- **CogUtil** is always the first dependency (provides base utilities)
- **AtomSpace** is typically the second dependency (knowledge representation)
- Components use `FIND_PACKAGE(CONFIG)` for OpenCog dependencies
- Support for both installed packages and in-tree builds

### 3. Source Code Organization
- All component code lives in `opencog/[component-name]/`
- Public headers installed to system include paths
- Version information in dedicated header file
- Clear separation between library code and tests

### 4. Testing Infrastructure
- CxxTest framework for C++ unit tests
- Tests mirror the source directory structure
- `make check` or `make test` to run tests
- Optional: Python tests for bindings

### 5. Language Bindings
- **Guile Scheme**: Primary scripting interface
- **Python**: Via Cython bindings
- Optional: Haskell, OCaml support

### 6. Continuous Integration
- CircleCI configuration in `.circleci/config.yml`
- Docker-based builds using opencog-deps image
- Automated testing and artifact caching

### 7. Debian Packaging
- Full debian/ directory for package creation
- Separate packages for runtime and development files
- Follows Debian policy standards

## Usage

### Quick Start

Generate a new OpenCog component:

```bash
./cogarc/scripts/generate-component.sh my-component "My Component Description"
```

This creates a new directory `my-component/` with the complete archetypal structure.

### Transform Existing Code

Transform an existing project into an OpenCog component:

```bash
./cogarc/scripts/transform-to-opencog.py \
  --source /path/to/existing/project \
  --component-name my-component \
  --description "My Component Description" \
  --dependencies cogutil,atomspace
```

### Customize Template

Edit the template files in `cogarc/template/` to customize the generated structure.

## Template Variables

The following variables are substituted during component generation:

| Variable | Description | Example |
|----------|-------------|---------|
| `@COMPONENT_NAME@` | Component name (lowercase) | `my-component` |
| `@COMPONENT_NAME_UPPER@` | Component name (uppercase) | `MY_COMPONENT` |
| `@COMPONENT_NAME_CAMEL@` | Component name (CamelCase) | `MyComponent` |
| `@COMPONENT_DESCRIPTION@` | Brief description | `My Component Description` |
| `@VERSION@` | Semantic version | `1.0.0` |
| `@AUTHOR@` | Author name | `OpenCog Community` |
| `@EMAIL@` | Contact email | `opencog@googlegroups.com` |
| `@DEPENDENCIES@` | Comma-separated dependencies | `cogutil,atomspace` |
| `@YEAR@` | Current year | `2024` |

## Architecture Patterns

### CMakeLists.txt Root Pattern

```cmake
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(@COMPONENT_NAME@)

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
    SET(CMAKE_BUILD_TYPE Release)
ENDIF ()

# Add cmake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/")

# Version from header file
FILE(READ "${CMAKE_CURRENT_SOURCE_DIR}/opencog/@COMPONENT_NAME@/version.h" _VERSION_H)
STRING(REGEX MATCH "#define @COMPONENT_NAME_UPPER@_VERSION_STRING \"([0-9.]+)\"" _ "${_VERSION_H}")
SET(SEMANTIC_VERSION ${CMAKE_MATCH_1})

# Find dependencies
FIND_PACKAGE(CogUtil CONFIG REQUIRED)
# Add more dependencies as needed

# Build subdirectories
ADD_SUBDIRECTORY(opencog)
ADD_SUBDIRECTORY(tests)
```

### Version Header Pattern

```cpp
#ifndef _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H
#define _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H

#define @COMPONENT_NAME_UPPER@_VERSION_STRING "1.0.0"

#endif // _OPENCOG_@COMPONENT_NAME_UPPER@_VERSION_H
```

### CircleCI Pattern

```yaml
version: 2.0

jobs:
  @COMPONENT_NAME@:
    docker:
      - image: $CIRCLE_PROJECT_USERNAME/opencog-deps
        user: root
    working_directory: /ws/@COMPONENT_NAME@
    steps:
      - checkout
      - run:
          name: CMake Configure
          command: mkdir build && cd build && cmake ..
      - run:
          name: Build
          command: cd build && make
      - run:
          name: Run tests
          command: cd build && make check
```

## Advanced Features

### GitHub Actions Integration

Modern components may prefer GitHub Actions over CircleCI. A template is provided:

```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y build-essential cmake libboost-all-dev
      - name: Configure
        run: cmake -B build
      - name: Build
        run: cmake --build build
      - name: Test
        run: cd build && ctest
```

### Multi-Language Support

Components can provide bindings for multiple languages:

- **Guile**: Add `guile/` subdirectory in `opencog/[component]/`
- **Python**: Add `cython/` subdirectory for Cython wrappers
- **Haskell**: Add `haskell/` subdirectory
- **OCaml**: Add `ocaml/` subdirectory

## Examples

See the `examples/` directory for sample generated components:

- `examples/simple-component/` - Minimal component with basic utilities
- `examples/atomspace-component/` - Component that extends AtomSpace
- `examples/full-featured/` - Component with all optional features

## Contributing

To improve the archetype template:

1. Analyze additional OpenCog components
2. Identify common patterns
3. Update templates in `cogarc/template/`
4. Update generation scripts
5. Submit pull request

## References

- [OpenCog Wiki](https://wiki.opencog.org/)
- [AtomSpace](https://github.com/opencog/atomspace)
- [CogUtil](https://github.com/opencog/cogutil)
- [Building OpenCog](https://wiki.opencog.org/w/Building_OpenCog)

## License

This template system is released under the AGPL-3.0 license, consistent with OpenCog components.
