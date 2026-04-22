#
# Master @COMPONENT_NAME_CAMEL@ CMake file.
#
# General organization:
# -- check for different compilers, OS'es
# -- search for various required & optional libraries/tools
# -- decide what to build based on above results.
# -- configure various config files.
# -- print pretty summary
#

include(cmake/Summary.cmake)

# CMake 3.12 is required for modern features
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(@COMPONENT_NAME@)

# ----------------------------------------------------------
# User-modifiable options. Feel free to change these!
#
# Uncomment to be in Release mode [default].
# SET(CMAKE_BUILD_TYPE Release)

# Uncomment to build in debug mode.
# SET(CMAKE_BUILD_TYPE Debug)

# Uncomment to be in coverage testing mode.
# SET(CMAKE_BUILD_TYPE Coverage)

# Uncomment to build in profile mode.
# SET(CMAKE_BUILD_TYPE Profile)

# Uncomment to build in release mode with debug information.
# SET(CMAKE_BUILD_TYPE RelWithDebInfo)

# Default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

ADD_DEFINITIONS(-DPROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                -DPROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")

# Add the 'cmake' and 'lib' dirs to cmake's module search path
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/")

# ===============================================================
# Get @COMPONENT_NAME_CAMEL@ version from header file
# NOTE: This is the official semantic-version, derived from
# a version-control independent means of declaring versioning.
#
FILE(READ "${CMAKE_CURRENT_SOURCE_DIR}/opencog/@COMPONENT_NAME@/version.h" _VERSION_H_CONTENTS)
STRING(REGEX MATCH
	"#define @COMPONENT_NAME_UPPER@_VERSION_STRING \"([0-9]+[.0-9]+[.0-9]+)\""
	_ "${_VERSION_H_CONTENTS}")
SET(SEMANTIC_VERSION ${CMAKE_MATCH_1})

MESSAGE(STATUS "@COMPONENT_NAME_CAMEL@ version: ${SEMANTIC_VERSION}")

# ===============================================================
# Detect different compilers and OS'es, tweak flags as necessary.

include(cmake/OpenCogGccOptions.cmake)

# The default case for non-profile builds is to use shared libraries.
IF (CMAKE_BUILD_TYPE STREQUAL "Profile")
	SET(BUILD_SHARED_LIBS OFF)
ELSE (CMAKE_BUILD_TYPE STREQUAL "Profile")
	SET(BUILD_SHARED_LIBS ON)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "Profile")

# ===============================================================
# Check for existence of various required, optional packages.
# Listed in alphabetical order, more or less.
# CogUtil must come first, because it supplies various FindXXX macros.

# Cogutil - REQUIRED
FIND_PACKAGE(CogUtil CONFIG)
IF (COGUTIL_FOUND)
	MESSAGE(STATUS "CogUtil version ${COGUTIL_VERSION} found.")
	ADD_DEFINITIONS(-DHAVE_COGUTIL)
	SET(HAVE_COGUTIL 1)
ELSEIF(TARGET cogutil)
	# CogUtil is being built as part of the same project
	MESSAGE(STATUS "CogUtil being built in-tree as subdirectory.")
	ADD_DEFINITIONS(-DHAVE_COGUTIL)
	SET(HAVE_COGUTIL 1)
	SET(COGUTIL_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/cogutil")
	SET(COGUTIL_DATA_DIR "${CMAKE_SOURCE_DIR}/cogutil")
	SET(COGUTIL_LIBRARIES cogutil)
ELSE ()
	MESSAGE(FATAL_ERROR "CogUtil missing: it is needed!")
ENDIF ()

# Add cogutil's cmake directory to module path
IF(COGUTIL_FOUND)
	list(APPEND CMAKE_MODULE_PATH "${COGUTIL_DATA_DIR}/cmake/")
ENDIF()

# @DEPENDENCIES_FIND_PACKAGE@
# Additional dependencies will be inserted here by the generator

# ===============================================================
# Optional packages

# Boost - commonly used in OpenCog
FIND_PACKAGE(Boost 1.46 COMPONENTS system thread)
IF(Boost_FOUND)
	MESSAGE(STATUS "Boost version ${Boost_VERSION} found.")
	SET(HAVE_BOOST 1)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

# Guile - for Scheme scripting interface
FIND_PACKAGE(Guile 3.0.0)
IF(GUILE_FOUND)
	ADD_DEFINITIONS(-DHAVE_GUILE)
	SET(HAVE_GUILE 1)
	INCLUDE_DIRECTORIES(${GUILE_INCLUDE_DIRS})
ENDIF(GUILE_FOUND)

# Python - for Python bindings
FIND_PACKAGE(PythonLibs)
IF(PYTHONLIBS_FOUND)
	SET(HAVE_PYTHON 1)
	INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_DIRS})
	MESSAGE(STATUS "Python ${PYTHONLIBS_VERSION_STRING} found.")
ENDIF(PYTHONLIBS_FOUND)

# Cython - for Python bindings
FIND_PACKAGE(Cython 0.23.0)
IF(CYTHON_FOUND)
	SET(HAVE_CYTHON 1)
	MESSAGE(STATUS "Cython ${CYTHON_VERSION} found.")
ENDIF(CYTHON_FOUND)

# CxxTest - for unit testing
FIND_PACKAGE(CxxTest)
IF(CXXTEST_FOUND)
	MESSAGE(STATUS "CxxTest found.")
ELSE(CXXTEST_FOUND)
	MESSAGE(STATUS "CxxTest not found. Unit tests will not be built.")
ENDIF(CXXTEST_FOUND)

# Doxygen - for documentation generation
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
	MESSAGE(STATUS "Doxygen found: ${DOXYGEN_EXECUTABLE}")
ENDIF(DOXYGEN_FOUND)

# ===============================================================
# Include configuration

# Include configuration summary
INCLUDE(cmake/Summary.cmake)

# Set default include paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# ===============================================================
# Decide what to build based on package availability

# Basic C++ library always built
ADD_SUBDIRECTORY(opencog)

# Examples (optional)
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt")
	ADD_SUBDIRECTORY(examples)
ENDIF()

# Tests (if CxxTest found)
IF(CXXTEST_FOUND)
	ADD_SUBDIRECTORY(tests)
	ENABLE_TESTING()
ENDIF(CXXTEST_FOUND)

# Scripts
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scripts/CMakeLists.txt")
	ADD_SUBDIRECTORY(scripts)
ENDIF()

# ===============================================================
# Packaging

# Set package metadata
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "@COMPONENT_DESCRIPTION@")
SET(CPACK_PACKAGE_NAME "@COMPONENT_NAME@")
SET(CPACK_PACKAGE_VENDOR "OpenCog Community")
SET(CPACK_PACKAGE_VERSION "${SEMANTIC_VERSION}")
SET(CPACK_GENERATOR "DEB")

SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "@AUTHOR@ <@EMAIL@>")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libcogutil-dev (>= @COGUTIL_MIN_VERSION@)")
SET(CPACK_DEBIAN_PACKAGE_SECTION "science")
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://opencog.org")

INCLUDE(CPack)

# ===============================================================
# Show summary of what was found and what will be built

SUMMARY_ADD("@COMPONENT_NAME_CAMEL@ version" "${SEMANTIC_VERSION}")
SUMMARY_ADD("Build type" "${CMAKE_BUILD_TYPE}")
SUMMARY_ADD("CogUtil" "${COGUTIL_VERSION}")
SUMMARY_ADD("Guile" "${GUILE_VERSION}")
SUMMARY_ADD("Python" "${PYTHONLIBS_VERSION_STRING}")
SUMMARY_ADD("Cython" "${CYTHON_VERSION}")
SUMMARY_ADD("Boost" "${Boost_VERSION}")
SUMMARY_ADD("Build tests" "${CXXTEST_FOUND}")
SUMMARY_ADD("Doxygen" "${DOXYGEN_FOUND}")

SUMMARY_SHOW()
