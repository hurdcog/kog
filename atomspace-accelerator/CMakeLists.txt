#
# AtomSpace Accelerator Inference Engine
# Part of the OpenCog Collection (OCC) cognitive architecture
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(atomspace-accelerator
    VERSION 1.0.0
    DESCRIPTION "AtomSpace Accelerator Inference Engine"
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

# AtomSpace Accelerator library source files
set(ATOMSPACE_ACCELERATOR_SOURCES
    src/inference_engine.cpp
    src/accelerator.cpp
    src/query_optimizer.cpp
    src/pattern_miner.cpp
)

# AtomSpace Accelerator library headers
set(ATOMSPACE_ACCELERATOR_HEADERS
    include/atomspace_accelerator/inference_engine.h
    include/atomspace_accelerator/accelerator.h
    include/atomspace_accelerator/query_optimizer.h
    include/atomspace_accelerator/pattern_miner.h
)

# Build AtomSpace Accelerator shared library
add_library(atomspace_accelerator SHARED ${ATOMSPACE_ACCELERATOR_SOURCES})

target_include_directories(atomspace_accelerator
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(atomspace_accelerator
    PUBLIC
        Threads::Threads
)

# Installation
install(TARGETS atomspace_accelerator
    EXPORT atomspace_accelerator-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/atomspace_accelerator
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

MESSAGE(STATUS "AtomSpace Accelerator Inference Engine configuration complete")
