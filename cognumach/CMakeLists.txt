# Cognumach (GNU Mach) - Layer 1: Cognitive Microkernel
# CMake wrapper for autotools-based build system

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(cognumach VERSION 1.8.0 LANGUAGES C ASM)

MESSAGE(STATUS "Configuring Cognumach (GNU Mach) ${PROJECT_VERSION}")
MESSAGE(STATUS "  This is an autotools-based project wrapped in CMake")

# Set version information
SET(COGNUMACH_VERSION_MAJOR 1)
SET(COGNUMACH_VERSION_MINOR 8)
SET(COGNUMACH_VERSION_PATCH 0)
SET(SEMANTIC_VERSION "1.8.0")

# Configuration options
OPTION(COGNUMACH_BUILD_DOCS "Build Cognumach documentation" OFF)
OPTION(COGNUMACH_ENABLE_TESTS "Enable Cognumach tests" OFF)
OPTION(COGNUMACH_ENABLE_64BIT "Enable 64-bit port (experimental)" OFF)

# Detect if configure script exists
IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/configure")
    MESSAGE(STATUS "Running autoreconf to generate configure script...")
    EXECUTE_PROCESS(
        COMMAND autoreconf -fi
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE AUTORECONF_RESULT
    )
    IF(NOT AUTORECONF_RESULT EQUAL 0)
        MESSAGE(WARNING "autoreconf failed. You may need to run it manually.")
    ENDIF()
ENDIF()

# Build directory for autotools
SET(COGNUMACH_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/autotools-build")
FILE(MAKE_DIRECTORY ${COGNUMACH_BUILD_DIR})

# Configure flags for autotools
SET(CONFIGURE_FLAGS "")
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    LIST(APPEND CONFIGURE_FLAGS "--enable-debug")
ENDIF()

IF(COGNUMACH_ENABLE_64BIT)
    LIST(APPEND CONFIGURE_FLAGS "--enable-64bit")
ENDIF()

# Custom target for configure
ADD_CUSTOM_TARGET(cognumach-configure
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure
        --prefix=${CMAKE_INSTALL_PREFIX}
        ${CONFIGURE_FLAGS}
    WORKING_DIRECTORY ${COGNUMACH_BUILD_DIR}
    COMMENT "Configuring Cognumach with autotools"
)

# Custom target for build
ADD_CUSTOM_TARGET(cognumach-build ALL
    COMMAND $(MAKE) -j${CMAKE_BUILD_PARALLEL_LEVEL}
    WORKING_DIRECTORY ${COGNUMACH_BUILD_DIR}
    DEPENDS cognumach-configure
    COMMENT "Building Cognumach microkernel"
)

# Custom target for install
ADD_CUSTOM_TARGET(cognumach-install
    COMMAND $(MAKE) install
    WORKING_DIRECTORY ${COGNUMACH_BUILD_DIR}
    DEPENDS cognumach-build
    COMMENT "Installing Cognumach"
)

# Integration with CMake install
INSTALL(CODE "
    EXECUTE_PROCESS(
        COMMAND make install
        WORKING_DIRECTORY ${COGNUMACH_BUILD_DIR}
    )
")

# Add tests if enabled
IF(COGNUMACH_ENABLE_TESTS)
    ENABLE_TESTING()
    ADD_CUSTOM_TARGET(cognumach-check
        COMMAND $(MAKE) check
        WORKING_DIRECTORY ${COGNUMACH_BUILD_DIR}
        DEPENDS cognumach-build
        COMMENT "Running Cognumach tests"
    )
ENDIF()

# Print configuration summary
MESSAGE(STATUS "")
MESSAGE(STATUS "Cognumach Configuration Summary:")
MESSAGE(STATUS "================================")
MESSAGE(STATUS "Version: ${SEMANTIC_VERSION}")
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "64-bit support: ${COGNUMACH_ENABLE_64BIT}")
MESSAGE(STATUS "Build docs: ${COGNUMACH_BUILD_DOCS}")
MESSAGE(STATUS "Enable tests: ${COGNUMACH_ENABLE_TESTS}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Note: Cognumach uses GNU autotools for actual compilation.")
MESSAGE(STATUS "      This CMake wrapper integrates it into the AGI-OS build.")
MESSAGE(STATUS "")
