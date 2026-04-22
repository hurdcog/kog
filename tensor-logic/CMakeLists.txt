#
# Tensor Logic - Multi-Entity Network-Aware Tensor-Enhanced Cognitive Synergy
#
# This component integrates ATen tensors with AtomSpace for:
# - Multi-Entity AtomSpace: Tensor representations across cognitive entities
# - Multi-Scale Analysis: Time-series tensors at different granularities
# - Network-Aware DAS: Flow tensors capturing distributed movement patterns
# - ESN Reservoir Computing: Echo State Networks for temporal processing
# - Tensor-Enhanced AtomSpace: Mathematical operations on atomspace structures
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(tensor-logic VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find dependencies
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../aten
    ${CMAKE_CURRENT_SOURCE_DIR}/../atenspace
    ${CMAKE_CURRENT_SOURCE_DIR}/../atomspace/opencog
    ${CMAKE_CURRENT_SOURCE_DIR}/../cogutil/opencog
)

# Source files
set(TENSOR_LOGIC_SOURCES
    opencog/tensor-logic/TensorLogic.cpp
    opencog/tensor-logic/MultiEntityAtomSpace.cpp
    opencog/tensor-logic/MultiScaleAnalysis.cpp
    opencog/tensor-logic/NetworkAwareDAS.cpp
    opencog/tensor-logic/ESNReservoir.cpp
    opencog/tensor-logic/TensorAtomSpace.cpp
)

# Header files
set(TENSOR_LOGIC_HEADERS
    opencog/tensor-logic/TensorLogic.h
    opencog/tensor-logic/MultiEntityAtomSpace.h
    opencog/tensor-logic/MultiScaleAnalysis.h
    opencog/tensor-logic/NetworkAwareDAS.h
    opencog/tensor-logic/ESNReservoir.h
    opencog/tensor-logic/TensorAtomSpace.h
)

# Create library
add_library(tensor-logic SHARED ${TENSOR_LOGIC_SOURCES})

target_include_directories(tensor-logic PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(tensor-logic
    Threads::Threads
)

# Install targets
install(TARGETS tensor-logic
    EXPORT tensor-logic-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${TENSOR_LOGIC_HEADERS}
    DESTINATION include/opencog/tensor-logic
)

# Export configuration
install(EXPORT tensor-logic-targets
    FILE tensor-logic-config.cmake
    NAMESPACE opencog::
    DESTINATION lib/cmake/tensor-logic
)

MESSAGE(STATUS "")
MESSAGE(STATUS "Tensor Logic Configuration:")
MESSAGE(STATUS "========================================")
MESSAGE(STATUS "Multi-Entity AtomSpace: enabled")
MESSAGE(STATUS "Multi-Scale Analysis: enabled")
MESSAGE(STATUS "Network-Aware DAS: enabled")
MESSAGE(STATUS "ESN Reservoir Computing: enabled")
MESSAGE(STATUS "Tensor-Enhanced AtomSpace: enabled")
MESSAGE(STATUS "========================================")
