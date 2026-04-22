# HurdCog (GNU Hurd) - Layer 2: Cognitive Operating System
# CMake wrapper for autotools-based build system with cognitive extensions

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(hurdcog VERSION 0.9.0 LANGUAGES C)

MESSAGE(STATUS "Configuring HurdCog (GNU Hurd + Cognitive Extensions) ${PROJECT_VERSION}")
MESSAGE(STATUS "  This is an autotools-based project wrapped in CMake")

# Set version information
SET(HURDCOG_VERSION_MAJOR 0)
SET(HURDCOG_VERSION_MINOR 9)
SET(HURDCOG_VERSION_PATCH 0)
SET(SEMANTIC_VERSION "0.9.0")

# Configuration options
OPTION(HURDCOG_BUILD_DOCS "Build HurdCog documentation" OFF)
OPTION(HURDCOG_ENABLE_TESTS "Enable HurdCog tests" OFF)
OPTION(HURDCOG_ENABLE_COGNITIVE_KERNEL "Enable cognitive kernel extensions" ON)
OPTION(HURDCOG_ENABLE_MACHSPACE "Enable MachSpace distributed hypergraph" ON)
OPTION(HURDCOG_ENABLE_FUSION_REACTOR "Enable Cognitive Fusion Reactor" ON)

# Dependencies
MESSAGE(STATUS "HurdCog requires Cognumach (GNU Mach) to be built first")

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
SET(HURDCOG_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/autotools-build")
FILE(MAKE_DIRECTORY ${HURDCOG_BUILD_DIR})

# Configure flags for autotools
SET(CONFIGURE_FLAGS "")
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    LIST(APPEND CONFIGURE_FLAGS "--enable-debug")
ENDIF()

# Add cognitive extensions flags
IF(HURDCOG_ENABLE_COGNITIVE_KERNEL)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_COGNITIVE_KERNEL")
ENDIF()

# Custom target for configure
ADD_CUSTOM_TARGET(hurdcog-configure
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/configure
        --prefix=${CMAKE_INSTALL_PREFIX}
        ${CONFIGURE_FLAGS}
    WORKING_DIRECTORY ${HURDCOG_BUILD_DIR}
    COMMENT "Configuring HurdCog with autotools"
)

# Custom target for build
ADD_CUSTOM_TARGET(hurdcog-build ALL
    COMMAND $(MAKE) -j${CMAKE_BUILD_PARALLEL_LEVEL}
    WORKING_DIRECTORY ${HURDCOG_BUILD_DIR}
    DEPENDS hurdcog-configure
    COMMENT "Building HurdCog operating system"
)

# Build cognitive kernel components (Python/Scheme based)
IF(HURDCOG_ENABLE_COGNITIVE_KERNEL)
    MESSAGE(STATUS "Building cognitive kernel components...")
    
    # Check for Python dependencies
    FIND_PACKAGE(Python3 COMPONENTS Interpreter)
    IF(Python3_FOUND)
        # Install Python dependencies for cognitive kernel
        ADD_CUSTOM_TARGET(hurdcog-cogkernel-python
            COMMAND ${Python3_EXECUTABLE} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt --user || true
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Installing cognitive kernel Python dependencies"
        )
        ADD_DEPENDENCIES(hurdcog-build hurdcog-cogkernel-python)
    ENDIF()
    
    # Check for Guile (for Scheme-based cognitive modules)
    FIND_PROGRAM(GUILE_EXECUTABLE guile)
    IF(GUILE_EXECUTABLE)
        MESSAGE(STATUS "Found Guile: ${GUILE_EXECUTABLE}")
        # Cognitive modules will be installed with main build
    ENDIF()
ENDIF()

# Custom target for install
ADD_CUSTOM_TARGET(hurdcog-install
    COMMAND $(MAKE) install
    WORKING_DIRECTORY ${HURDCOG_BUILD_DIR}
    DEPENDS hurdcog-build
    COMMENT "Installing HurdCog"
)

# Integration with CMake install
INSTALL(CODE "
    EXECUTE_PROCESS(
        COMMAND make install
        WORKING_DIRECTORY ${HURDCOG_BUILD_DIR}
    )
")

# Install cognitive kernel components
IF(HURDCOG_ENABLE_COGNITIVE_KERNEL)
    INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cogkernel/
        DESTINATION ${CMAKE_INSTALL_PREFIX}/share/hurdcog/cogkernel
        PATTERN "*.pyc" EXCLUDE
        PATTERN "__pycache__" EXCLUDE
        PATTERN "*.o" EXCLUDE
    )
ENDIF()

# Add tests if enabled
IF(HURDCOG_ENABLE_TESTS)
    ENABLE_TESTING()
    ADD_CUSTOM_TARGET(hurdcog-check
        COMMAND $(MAKE) check
        WORKING_DIRECTORY ${HURDCOG_BUILD_DIR}
        DEPENDS hurdcog-build
        COMMENT "Running HurdCog tests"
    )
    
    IF(Python3_FOUND AND HURDCOG_ENABLE_COGNITIVE_KERNEL)
        # Add cognitive kernel tests
        ADD_TEST(NAME hurdcog-phase6-tests
            COMMAND ${Python3_EXECUTABLE} run-phase6-tests.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
    ENDIF()
ENDIF()

# Print configuration summary
MESSAGE(STATUS "")
MESSAGE(STATUS "HurdCog Configuration Summary:")
MESSAGE(STATUS "==============================")
MESSAGE(STATUS "Version: ${SEMANTIC_VERSION}")
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
MESSAGE(STATUS "Cognitive kernel: ${HURDCOG_ENABLE_COGNITIVE_KERNEL}")
MESSAGE(STATUS "MachSpace: ${HURDCOG_ENABLE_MACHSPACE}")
MESSAGE(STATUS "Fusion Reactor: ${HURDCOG_ENABLE_FUSION_REACTOR}")
MESSAGE(STATUS "Build docs: ${HURDCOG_BUILD_DOCS}")
MESSAGE(STATUS "Enable tests: ${HURDCOG_ENABLE_TESTS}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Note: HurdCog uses GNU autotools for core OS compilation.")
MESSAGE(STATUS "      Cognitive extensions are built with Python/Guile.")
MESSAGE(STATUS "      This CMake wrapper integrates it into the AGI-OS build.")
MESSAGE(STATUS "")
