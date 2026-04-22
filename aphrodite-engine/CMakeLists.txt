#
# Aphrodite Engine OpenCog Integration CMakeLists.txt
# High-performance LLM inference for AtomSpace reasoning
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(aphrodite-engine-opencog)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Check for CUDA (optional but recommended)
find_package(CUDA QUIET)
if(CUDA_FOUND)
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 17)
    add_definitions(-DUSE_CUDA)
    message(STATUS "CUDA support enabled")
endif()

# Python package requirements check
execute_process(
    COMMAND ${Python3_EXECUTABLE} -c "import torch; print(torch.__version__)"
    RESULT_VARIABLE TORCH_CHECK
    OUTPUT_QUIET
    ERROR_QUIET
)

if(NOT TORCH_CHECK EQUAL 0)
    message(WARNING "PyTorch not found. Please install: pip install torch")
endif()

# Include directories
include_directories(${Python3_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Aphrodite OpenCog adapter sources
set(APHRODITE_OPENCOG_SOURCES
    src/aphrodite_bridge.cpp
    src/atomspace_llm_adapter.cpp
    src/cognitive_inference.cpp
    src/prompt_generator.cpp
    src/response_parser.cpp
    src/batch_processor.cpp
)

# Create shared library for Aphrodite-OpenCog integration
add_library(aphrodite-opencog SHARED ${APHRODITE_OPENCOG_SOURCES})

target_link_libraries(aphrodite-opencog
    ${Python3_LIBRARIES}
    ${Boost_LIBRARIES}
)

# Add CUDA support if available
if(CUDA_FOUND)
    target_link_libraries(aphrodite-opencog ${CUDA_LIBRARIES})
    target_include_directories(aphrodite-opencog PRIVATE ${CUDA_INCLUDE_DIRS})
endif()

# Install library and binaries
install(TARGETS aphrodite-opencog
    LIBRARY DESTINATION lib/opencog
    RUNTIME DESTINATION bin
)

# Install headers
install(DIRECTORY include/
    DESTINATION include/opencog/aphrodite
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

# Install Python modules
install(FILES python/opencog_aphrodite.py
    DESTINATION lib/python3.10/site-packages
)

# Install Scheme modules
install(FILES scm/aphrodite.scm
    DESTINATION share/guile/site/3.0/opencog
)

# Install service startup script
install(PROGRAMS scripts/aphrodite-opencog
    DESTINATION bin
)

# Install Python requirements for runtime
install(FILES requirements.txt
    DESTINATION share/opencog/aphrodite
)

# Install configuration templates
install(FILES 
    config/server.yaml
    config/model_config.json
    DESTINATION etc/opencog/aphrodite
)

# Create model cache directory
install(DIRECTORY DESTINATION var/cache/opencog/models)

# Install Shepherd service definition
install(FILES services/aphrodite-daemon.scm
    DESTINATION etc/shepherd/services
)

# Build tests if enabled
option(BUILD_APHRODITE_TESTS "Build Aphrodite integration tests" OFF)
if(BUILD_APHRODITE_TESTS)
    add_subdirectory(tests)
endif()

# Custom target for Python dependencies
add_custom_target(install-python-deps
    COMMAND ${Python3_EXECUTABLE} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
    COMMENT "Installing Python dependencies for Aphrodite Engine"
)

# Configuration summary
message(STATUS "Aphrodite Engine OpenCog integration configured:")
message(STATUS "  CUDA support: ${CUDA_FOUND}")
message(STATUS "  Python version: ${Python3_VERSION}")
message(STATUS "  PyTorch check: ${TORCH_CHECK}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")

if(CUDA_FOUND)
    message(STATUS "  CUDA version: ${CUDA_VERSION}")
else()
    message(STATUS "  Note: Install CUDA for GPU acceleration")
endif()