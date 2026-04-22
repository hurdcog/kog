CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(atomspace-machspace VERSION 1.0.0 LANGUAGES CXX)

# Find AtomSpace
FIND_PACKAGE(AtomSpace 5.0.3 REQUIRED)

# Include directories
INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${COGUTIL_INCLUDE_DIR}
    ${ATOMSPACE_INCLUDE_DIR}
    ${ATOMSPACE_STORAGE_INCLUDE_DIR}
)

# MachSpace storage backend library
ADD_LIBRARY(atomspace-machspace SHARED
    MachSpaceStorageNode.cc
)

TARGET_LINK_LIBRARIES(atomspace-machspace
    ${ATOMSPACE_LIBRARIES}
)

# Set library properties
SET_TARGET_PROPERTIES(atomspace-machspace PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Installation
INSTALL(TARGETS atomspace-machspace
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

INSTALL(FILES
    MachSpaceStorageNode.h
    DESTINATION include/opencog/persist/machspace
)

# Build information
MESSAGE(STATUS "Building AtomSpace MachSpace storage backend")
MESSAGE(STATUS "  Version: ${PROJECT_VERSION}")
MESSAGE(STATUS "  AtomSpace: ${ATOMSPACE_INCLUDE_DIR}")
