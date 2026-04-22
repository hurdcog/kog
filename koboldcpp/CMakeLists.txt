#
# KoboldCpp OpenCog Integration CMakeLists.txt
# Story/world modeling and GGUF inference
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(koboldcpp-opencog)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Check for OpenBLAS (for GGML acceleration)
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENBLAS openblas)

# Set compilation flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
if(OPENBLAS_FOUND)
    add_definitions(-DGGML_USE_OPENBLAS)
endif()

# Include directories
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# KoboldCpp OpenCog adapter sources
set(KOBOLDCPP_OPENCOG_SOURCES
    src/koboldcpp_bridge.cpp
    src/gguf_atomspace_adapter.cpp
    src/story_generator.cpp
    src/world_modeler.cpp
    src/narrative_processor.cpp
)

# Create shared library for KoboldCpp-OpenCog integration
add_library(koboldcpp-opencog SHARED ${KOBOLDCPP_OPENCOG_SOURCES})

target_link_libraries(koboldcpp-opencog
    ${Python3_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Link OpenBLAS if available for acceleration
if(OPENBLAS_FOUND)
    target_link_libraries(koboldcpp-opencog ${OPENBLAS_LIBRARIES})
    target_include_directories(koboldcpp-opencog PRIVATE ${OPENBLAS_INCLUDE_DIRS})
endif()

# Build KoboldCpp engine with OpenCog modifications
set(KOBOLDCPP_SOURCES
    src/koboldcpp/koboldcpp.cpp
    src/koboldcpp/ggml.c
    src/koboldcpp/llama.cpp
    src/koboldcpp/opencog_extensions.cpp
)

add_executable(koboldcpp-engine ${KOBOLDCPP_SOURCES})
target_link_libraries(koboldcpp-engine koboldcpp-opencog)

# Install binaries and libraries
install(TARGETS koboldcpp-opencog koboldcpp-engine
    LIBRARY DESTINATION lib/opencog
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/
    DESTINATION include/opencog/koboldcpp
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install Python modules
install(FILES python/opencog_koboldcpp.py
    DESTINATION lib/python3.10/site-packages
)

# Install Scheme modules
install(FILES scm/koboldcpp.scm
    DESTINATION share/guile/site/3.0/opencog
)

# Install service wrapper script
install(PROGRAMS scripts/koboldcpp-opencog
    DESTINATION bin
)

# Install models directory structure
install(DIRECTORY DESTINATION var/lib/opencog/models)

# Build tests if enabled
option(BUILD_KOBOLDCPP_TESTS "Build KoboldCpp integration tests" OFF)
if(BUILD_KOBOLDCPP_TESTS)
    add_subdirectory(tests)
endif()

# Configuration summary
message(STATUS "KoboldCpp OpenCog integration configured:")
message(STATUS "  OpenBLAS acceleration: ${OPENBLAS_FOUND}")
message(STATUS "  Python support: ${Python3_FOUND}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")