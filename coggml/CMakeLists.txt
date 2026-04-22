#
# CogGML Microkernel - Self-Aware Cognitive Shards
# Part of the OpenCog Collection (OCC) cognitive architecture
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(coggml
    VERSION 1.0.0
    DESCRIPTION "CogGML Self-Aware Microkernel for Cognitive Shards"
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

# CogGML Microkernel library source files
set(COGGML_SOURCES
    src/microkernel.cpp
    src/cognitive_shard.cpp
    src/self_awareness.cpp
    src/shard_coordinator.cpp
    src/shard_message.cpp
    src/distributed_coordinator.cpp
)

# CogGML Microkernel library headers
set(COGGML_HEADERS
    include/coggml/microkernel.h
    include/coggml/cognitive_shard.h
    include/coggml/self_awareness.h
    include/coggml/shard_coordinator.h
    include/coggml/shard_message.h
    include/coggml/distributed_coordinator.h
)

# Build CogGML shared library
add_library(coggml SHARED ${COGGML_SOURCES})

target_include_directories(coggml
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(coggml
    PUBLIC
        Threads::Threads
)

# Installation
install(TARGETS coggml
    EXPORT coggml-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/coggml
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Export targets
install(EXPORT coggml-targets
    FILE coggml-targets.cmake
    NAMESPACE coggml::
    DESTINATION lib/cmake/coggml
)

# Package configuration
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/coggml-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/coggml-config.cmake
    INSTALL_DESTINATION lib/cmake/coggml
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/coggml-config.cmake
    DESTINATION lib/cmake/coggml
)

MESSAGE(STATUS "CogGML Microkernel configuration complete")
