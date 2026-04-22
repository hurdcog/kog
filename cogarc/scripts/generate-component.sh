#!/bin/bash
#
# generate-component.sh - Generate a new OpenCog component from the archetype template
#
# Usage: generate-component.sh <component-name> <description> [options]
#
# Options:
#   --output-dir <dir>       Output directory (default: current directory)
#   --version <version>      Initial version (default: 1.0.0)
#   --author <name>          Author name (default: OpenCog Community)
#   --email <email>          Contact email (default: opencog@googlegroups.com)
#   --dependencies <list>    Comma-separated list of dependencies (default: cogutil)
#   --with-guile             Include Guile bindings
#   --with-python            Include Python bindings
#   --with-github-actions    Use GitHub Actions instead of CircleCI
#

set -e

# Script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TEMPLATE_DIR="$(dirname "$SCRIPT_DIR")/template"

# Default values
OUTPUT_DIR="."
VERSION="1.0.0"
AUTHOR="OpenCog Community"
EMAIL="opencog@googlegroups.com"
DEPENDENCIES="cogutil"
WITH_GUILE=false
WITH_PYTHON=false
WITH_GITHUB_ACTIONS=false
COGUTIL_MIN_VERSION="2.1.0"

# Parse arguments
if [ $# -lt 2 ]; then
    echo "Usage: $0 <component-name> <description> [options]"
    echo ""
    echo "Options:"
    echo "  --output-dir <dir>       Output directory (default: current directory)"
    echo "  --version <version>      Initial version (default: 1.0.0)"
    echo "  --author <name>          Author name (default: OpenCog Community)"
    echo "  --email <email>          Contact email (default: opencog@googlegroups.com)"
    echo "  --dependencies <list>    Comma-separated list of dependencies (default: cogutil)"
    echo "  --with-guile             Include Guile bindings"
    echo "  --with-python            Include Python bindings"
    echo "  --with-github-actions    Use GitHub Actions instead of CircleCI"
    exit 1
fi

COMPONENT_NAME="$1"
COMPONENT_DESCRIPTION="$2"
shift 2

# Parse options
while [ $# -gt 0 ]; do
    case "$1" in
        --output-dir)
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --version)
            VERSION="$2"
            shift 2
            ;;
        --author)
            AUTHOR="$2"
            shift 2
            ;;
        --email)
            EMAIL="$2"
            shift 2
            ;;
        --dependencies)
            DEPENDENCIES="$2"
            shift 2
            ;;
        --with-guile)
            WITH_GUILE=true
            shift
            ;;
        --with-python)
            WITH_PYTHON=true
            shift
            ;;
        --with-github-actions)
            WITH_GITHUB_ACTIONS=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Validate component name (lowercase with hyphens)
if ! [[ "$COMPONENT_NAME" =~ ^[a-z][a-z0-9-]*$ ]]; then
    echo "Error: Component name must be lowercase and contain only letters, numbers, and hyphens"
    exit 1
fi

# Derive variations of component name - use sed -E for portability
COMPONENT_NAME_UPPER=$(echo "$COMPONENT_NAME" | tr '[:lower:]' '[:upper:]' | tr '-' '_')
COMPONENT_NAME_CAMEL=$(echo "$COMPONENT_NAME" | sed -E 's/(^|-)([a-z])/\U\2/g')

# Parse version
VERSION_MAJOR=$(echo "$VERSION" | cut -d. -f1)
VERSION_MINOR=$(echo "$VERSION" | cut -d. -f2)
VERSION_PATCH=$(echo "$VERSION" | cut -d. -f3)

# Date for changelog
DATE=$(date -R 2>/dev/null || date)

# Output directory
COMPONENT_DIR="$OUTPUT_DIR/$COMPONENT_NAME"

echo "Generating OpenCog component: $COMPONENT_NAME"
echo "  Description: $COMPONENT_DESCRIPTION"
echo "  Output: $COMPONENT_DIR"
echo "  Version: $VERSION"
echo "  Author: $AUTHOR <$EMAIL>"
echo "  Dependencies: $DEPENDENCIES"
echo ""

# Create directory structure
mkdir -p "$COMPONENT_DIR"
cd "$COMPONENT_DIR"

# Copy template structure
echo "Creating directory structure..."
mkdir -p .circleci cmake debian doc/doxydoc examples lib opencog/$COMPONENT_NAME scripts tests/$COMPONENT_NAME

# Function to substitute variables in a file - consolidated for efficiency
substitute_variables() {
    local file="$1"
    # Use awk for safe multi-variable substitution
    awk -v cn="$COMPONENT_NAME" \
        -v cnu="$COMPONENT_NAME_UPPER" \
        -v cnc="$COMPONENT_NAME_CAMEL" \
        -v desc="$COMPONENT_DESCRIPTION" \
        -v ver="$VERSION" \
        -v vma="$VERSION_MAJOR" \
        -v vmi="$VERSION_MINOR" \
        -v vpa="$VERSION_PATCH" \
        -v auth="$AUTHOR" \
        -v email="$EMAIL" \
        -v year="$(date +%Y)" \
        -v date="$DATE" \
        -v cumin="$COGUTIL_MIN_VERSION" '
    {
        gsub(/@COMPONENT_NAME@/, cn)
        gsub(/@COMPONENT_NAME_UPPER@/, cnu)
        gsub(/@COMPONENT_NAME_CAMEL@/, cnc)
        gsub(/@COMPONENT_DESCRIPTION@/, desc)
        gsub(/@VERSION@/, ver)
        gsub(/@VERSION_MAJOR@/, vma)
        gsub(/@VERSION_MINOR@/, vmi)
        gsub(/@VERSION_PATCH@/, vpa)
        gsub(/@AUTHOR@/, auth)
        gsub(/@EMAIL@/, email)
        gsub(/@YEAR@/, year)
        gsub(/@DATE@/, date)
        gsub(/@COGUTIL_MIN_VERSION@/, cumin)
        print
    }' "$file" > "$file.tmp" && mv "$file.tmp" "$file"
}

# Copy and process template files
echo "Processing templates..."

# CMakeLists.txt
cp "$TEMPLATE_DIR/CMakeLists.txt" .
substitute_variables CMakeLists.txt

# Process dependencies for CMakeLists.txt
DEPENDENCIES_FIND_PACKAGE=""
DEBIAN_BUILD_DEPENDS=""
IFS=',' read -ra DEPS <<< "$DEPENDENCIES"
for dep in "${DEPS[@]}"; do
    dep=$(echo "$dep" | xargs) # trim whitespace
    if [ "$dep" != "cogutil" ]; then
        dep_upper=$(echo "$dep" | tr '[:lower:]' '[:upper:]' | tr '-' '_')
        
        # Handle special case conversions for known OpenCog components
        case "$dep" in
            atomspace)
                dep_camel="AtomSpace"
                ;;
            cogserver)
                dep_camel="CogServer"
                ;;
            cogutil)
                dep_camel="CogUtil"
                ;;
            *)
                # Generic CamelCase conversion - use sed -E for portability
                dep_camel=$(echo "$dep" | sed -E 's/(^|-)([a-z])/\U\2/g')
                ;;
        esac
        
        DEPENDENCIES_FIND_PACKAGE="${DEPENDENCIES_FIND_PACKAGE}
# $dep_camel
FIND_PACKAGE($dep_camel CONFIG)
IF (${dep_upper}_FOUND)
	MESSAGE(STATUS \"$dep_camel version \${${dep_upper}_VERSION} found.\")
	ADD_DEFINITIONS(-DHAVE_${dep_upper})
	SET(HAVE_${dep_upper} 1)
ELSE ()
	MESSAGE(FATAL_ERROR \"$dep_camel missing: it is needed!\")
ENDIF ()
"
        DEBIAN_BUILD_DEPENDS="${DEBIAN_BUILD_DEPENDS}               lib${dep}-dev,\n"
    fi
done

# Use a temporary marker to avoid sed issues with special characters
if [ -n "$DEPENDENCIES_FIND_PACKAGE" ]; then
    awk -v deps="$DEPENDENCIES_FIND_PACKAGE" '
        /# @DEPENDENCIES_FIND_PACKAGE@/ {print deps; next}
        {print}
    ' CMakeLists.txt > CMakeLists.txt.tmp && mv CMakeLists.txt.tmp CMakeLists.txt
fi

# README.md
cp "$TEMPLATE_DIR/README.md" .
substitute_variables README.md

# Fill in placeholder sections - using awk for safe substitution
awk '
    /@COMPONENT_BADGE@/ {
        print "[![CircleCI](https://circleci.com/gh/opencog/'"$COMPONENT_NAME"'.svg?style=svg)](https://circleci.com/gh/opencog/'"$COMPONENT_NAME"')"
        next
    }
    /@COMPONENT_OVERVIEW@/ {
        print "This component provides '"$COMPONENT_DESCRIPTION"' for OpenCog."
        next
    }
    /@REQUIRED_DEPENDENCIES@|@OPTIONAL_DEPENDENCIES@|@USAGE_EXAMPLES@|@ARCHITECTURE_NOTES@|@ACKNOWLEDGMENTS@/ {
        # Remove these placeholders
        next
    }
    /@REFERENCES@/ {
        print "- OpenCog Wiki: https://wiki.opencog.org/"
        next
    }
    {print}
' README.md > README.md.tmp && mv README.md.tmp README.md

# Version header
cp "$TEMPLATE_DIR/opencog/version.h" "opencog/$COMPONENT_NAME/version.h"
substitute_variables "opencog/$COMPONENT_NAME/version.h"

# CircleCI config
if [ "$WITH_GITHUB_ACTIONS" = false ]; then
    cp "$TEMPLATE_DIR/.circleci/config.yml" .circleci/
    substitute_variables .circleci/config.yml
else
    # GitHub Actions workflow
    mkdir -p .github/workflows
    cat > .github/workflows/build.yml << 'EOF'
name: Build and Test

on:
  push:
    branches: [ main, master ]
  pull_request:
    branches: [ main, master ]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y build-essential cmake cxxtest \
          libboost-dev libcogutil-dev
    
    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: cmake --build build -j$(nproc)
    
    - name: Test
      run: cd build && ctest --output-on-failure
EOF
    substitute_variables .github/workflows/build.yml
fi

# Debian packaging
for file in changelog compat control copyright rules; do
    cp "$TEMPLATE_DIR/debian/$file" debian/
    substitute_variables "debian/$file"
done

# Install files with substituted names
sed "s/@COMPONENT_NAME@/$COMPONENT_NAME/g" "$TEMPLATE_DIR/debian/lib@COMPONENT_NAME@.install" > "debian/lib$COMPONENT_NAME.install"
sed "s/@COMPONENT_NAME@/$COMPONENT_NAME/g" "$TEMPLATE_DIR/debian/lib@COMPONENT_NAME@-dev.install" > "debian/lib$COMPONENT_NAME-dev.install"
# Update debian/control with dependencies
if [ -n "$DEBIAN_BUILD_DEPENDS" ]; then
    awk -v deps="$DEBIAN_BUILD_DEPENDS" '
        /@DEBIAN_BUILD_DEPENDS@/ {print deps; next}
        {print}
    ' debian/control > debian/control.tmp && mv debian/control.tmp debian/control
else
    sed -i.bak '/@DEBIAN_BUILD_DEPENDS@/d' debian/control && rm -f debian/control.bak
fi

chmod +x debian/rules

# LICENSE (truncated AGPL-3.0)
cat > LICENSE << 'EOF'
                    GNU AFFERO GENERAL PUBLIC LICENSE
                       Version 3, 19 November 2007

 Copyright (C) 2007 Free Software Foundation, Inc. <https://fsf.org/>
 Everyone is permitted to copy and distribute verbatim copies
 of this license document, but changing it is not allowed.

                            Preamble

  The GNU Affero General Public License is a free, copyleft license for
software and other kinds of works, specifically designed to ensure
cooperation with the community in the case of network server software.

[Full AGPL-3.0 license text - see https://www.gnu.org/licenses/agpl-3.0.txt]
EOF

# AUTHORS
cat > AUTHORS << EOF
$AUTHOR <$EMAIL>

This is a list of contributors to $COMPONENT_NAME_CAMEL.
Please add your name here when you contribute.
EOF

# .gitignore
cat > .gitignore << 'EOF'
# Build directories
build/
build-*/
*.kdev4
.kdev4/

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
CTestTestfile.cmake
Makefile

# Compiled files
*.o
*.so
*.a
*.la
*.lo
*.pyc
*.pyo
__pycache__/

# IDE files
.vscode/
.idea/
*.swp
*.swo
*~
.DS_Store

# Documentation
doc/html/
doc/latex/
doxygen_sqlite3.db

# Test results
Testing/
tests/Testing/

# Coverage
*.gcov
*.gcda
*.gcno
coverage/
lcov.info

# Debian
debian/files
debian/*.debhelper
debian/*.log
debian/*.substvars
debian/tmp/

# Package files
*.deb
*.rpm
*.tar.gz
*.zip
EOF

# opencog CMakeLists.txt
cat > opencog/CMakeLists.txt << EOF
# Source files for $COMPONENT_NAME_CAMEL

ADD_SUBDIRECTORY($COMPONENT_NAME)
EOF

# opencog/component CMakeLists.txt
cat > "opencog/$COMPONENT_NAME/CMakeLists.txt" << EOF
# $COMPONENT_NAME_CAMEL library source files

ADD_LIBRARY($COMPONENT_NAME
	# Add your source files here
	# example.cc
)

TARGET_LINK_LIBRARIES($COMPONENT_NAME
	\${COGUTIL_LIBRARIES}
)

INSTALL(TARGETS $COMPONENT_NAME
	EXPORT ${COMPONENT_NAME_CAMEL}Targets
	LIBRARY DESTINATION lib\${LIB_DIR_SUFFIX}/$COMPONENT_NAME
	ARCHIVE DESTINATION lib\${LIB_DIR_SUFFIX}/$COMPONENT_NAME
)

INSTALL(FILES
	version.h
	# Add your header files here
	# example.h
	DESTINATION include/opencog/$COMPONENT_NAME
)
EOF

# tests CMakeLists.txt
cat > tests/CMakeLists.txt << EOF
# Tests for $COMPONENT_NAME_CAMEL

ENABLE_TESTING()

ADD_SUBDIRECTORY($COMPONENT_NAME)
EOF

cat > "tests/$COMPONENT_NAME/CMakeLists.txt" << EOF
# Unit tests for $COMPONENT_NAME_CAMEL

# Example test
# ADD_CXXTEST(ExampleUTest)
# TARGET_LINK_LIBRARIES(ExampleUTest $COMPONENT_NAME)
EOF

# examples CMakeLists.txt
cat > examples/CMakeLists.txt << EOF
# Examples for $COMPONENT_NAME_CAMEL

# ADD_SUBDIRECTORY(basic)
EOF

# Summary script
cat > cmake/Summary.cmake << 'EOF'
# Summary display functions

MACRO(SUMMARY_ADD _name _value)
	LIST(APPEND _summary "${_name}###${_value}")
ENDMACRO()

MACRO(SUMMARY_SHOW)
	MESSAGE("")
	MESSAGE("===============================================================================")
	MESSAGE("                          Configuration Summary")
	MESSAGE("===============================================================================")
	FOREACH(_item ${_summary})
		STRING(REPLACE "###" ";" _item_list ${_item})
		LIST(GET _item_list 0 _name)
		LIST(GET _item_list 1 _value)
		STRING(LENGTH "${_name}" _name_length)
		MATH(EXPR _padding "40 - ${_name_length}")
		STRING(REPEAT " " ${_padding} _spaces)
		MESSAGE("  ${_name}${_spaces}: ${_value}")
	ENDFOREACH()
	MESSAGE("===============================================================================")
	MESSAGE("")
ENDMACRO()
EOF

# OpenCogGccOptions.cmake (simplified version)
cat > cmake/OpenCogGccOptions.cmake << 'EOF'
# Compiler options for GCC/Clang

IF (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# C++17 standard
	SET(CMAKE_CXX_STANDARD 17)
	SET(CMAKE_CXX_STANDARD_REQUIRED ON)
	
	# Warning flags
	ADD_COMPILE_OPTIONS(-Wall -Wextra -Wno-unused-parameter)
	
	# Position independent code
	SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
	
	# Debug flags
	IF (CMAKE_BUILD_TYPE STREQUAL "Debug")
		ADD_COMPILE_OPTIONS(-g -O0)
	ENDIF()
	
	# Release flags
	IF (CMAKE_BUILD_TYPE STREQUAL "Release")
		ADD_COMPILE_OPTIONS(-O3 -DNDEBUG)
	ENDIF()
	
	# Coverage flags
	IF (CMAKE_BUILD_TYPE STREQUAL "Coverage")
		ADD_COMPILE_OPTIONS(-g -O0 --coverage)
		ADD_LINK_OPTIONS(--coverage)
	ENDIF()
ENDIF()
EOF

# Config file for find_package
cat > "cmake/${COMPONENT_NAME_CAMEL}Config.cmake.in" << EOF
# ${COMPONENT_NAME_CAMEL}Config.cmake
# This file helps CMake find ${COMPONENT_NAME_CAMEL}

@PACKAGE_INIT@

include("\${CMAKE_CURRENT_LIST_DIR}/${COMPONENT_NAME_CAMEL}Targets.cmake")

set(${COMPONENT_NAME_UPPER}_LIBRARIES $COMPONENT_NAME)
set(${COMPONENT_NAME_UPPER}_INCLUDE_DIR "@PACKAGE_INCLUDE_INSTALL_DIR@")
set(${COMPONENT_NAME_UPPER}_DATA_DIR "@PACKAGE_DATA_INSTALL_DIR@")
set(${COMPONENT_NAME_UPPER}_VERSION "@PACKAGE_VERSION@")

check_required_components(${COMPONENT_NAME_CAMEL})
EOF

# cmake/CMakeLists.txt
cat > cmake/CMakeLists.txt << EOF
# Install CMake modules

INSTALL(FILES
	OpenCogGccOptions.cmake
	Summary.cmake
	DESTINATION lib/cmake/${COMPONENT_NAME_CAMEL}
)
EOF

echo ""
echo "✓ Component '$COMPONENT_NAME' generated successfully!"
echo ""
echo "Next steps:"
echo "  1. cd $COMPONENT_NAME"
echo "  2. Add your source code to opencog/$COMPONENT_NAME/"
echo "  3. Add your tests to tests/$COMPONENT_NAME/"
echo "  4. Add examples to examples/"
echo "  5. Build: mkdir build && cd build && cmake .. && make"
echo "  6. Test: make check"
echo ""
echo "For more information, see the README.md file."
