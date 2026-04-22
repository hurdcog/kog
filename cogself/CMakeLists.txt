#
# CogSelf - AGI Cognitive Synergy Framework
# Part of the OpenCog Collection (OCC) cognitive architecture
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(cogself
    VERSION 1.0.0
    DESCRIPTION "CogSelf AGI Cognitive Synergy Framework"
    LANGUAGES CXX
)

# Version information
SET(SEMANTIC_VERSION 1.0.0)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find dependencies
find_package(Threads REQUIRED)

# CogSelf library source files
set(COGSELF_SOURCES
    src/cogself.cpp
    src/synergy_manager.cpp
    src/agi_goal_tracker.cpp
    src/cogprime_identity.cpp
    src/distributed_identity.cpp
    src/unified_self_model.cpp
)

# CogSelf library headers
set(COGSELF_HEADERS
    include/cogself/cogself.h
    include/cogself/synergy_manager.h
    include/cogself/agi_goal_tracker.h
    include/cogself/cogprime_identity.h
    include/cogself/distributed_identity.h
    include/cogself/unified_self_model.h
)

# Build CogSelf shared library
add_library(cogself SHARED ${COGSELF_SOURCES})

target_include_directories(cogself
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(cogself
    PUBLIC
        Threads::Threads
)

# Installation
install(TARGETS cogself
    EXPORT cogself-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/cogself
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Export targets
install(EXPORT cogself-targets
    FILE cogself-targets.cmake
    NAMESPACE cogself::
    DESTINATION lib/cmake/cogself
)

MESSAGE(STATUS "CogSelf AGI Synergy Framework configuration complete")
