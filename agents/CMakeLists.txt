#
# Atomese Agents - Minimal CMake configuration
#
# This project is in early prototyping stage and contains
# primarily Scheme (.scm) files and examples.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(agents)

# Version information
SET(SEMANTIC_VERSION 1.0.0)

MESSAGE(STATUS "Configuring Atomese Agents...")

# Install Scheme files if available
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/scm")
    FILE(GLOB_RECURSE SCM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/scm/*.scm")
    IF(SCM_FILES)
        INSTALL(FILES ${SCM_FILES}
                DESTINATION "share/opencog/scm/agents")
    ENDIF()
ENDIF()

# Install examples if available
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
    FILE(GLOB_RECURSE EXAMPLE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.scm")
    IF(EXAMPLE_FILES)
        INSTALL(FILES ${EXAMPLE_FILES}
                DESTINATION "share/opencog/examples/agents")
    ENDIF()
ENDIF()

# Install documentation
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    INSTALL(FILES README.md
            DESTINATION "share/doc/opencog/agents")
ENDIF()

MESSAGE(STATUS "Atomese Agents configured (Scheme files only)")
