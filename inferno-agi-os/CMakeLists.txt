# CMakeLists.txt for Inferno AGI OS Kernel Modules
# Cross-platform build system for cognitive kernel services
#
# This builds the complete OpenCog Inferno AGI OS:
#   - Kernel modules: AtomSpace, Reasoning, Attention, Cognitive 9P
#   - PLN Inference Engine with full truth value formulas
#   - MOSES Evolutionary Learning Engine
#   - Cognitive IPC Channels
#   - Pattern Matching Engine
#   - Distributed AtomSpace
#   - Cognitive filesystems: atoms, reasoning, attention, learning, perception, action
#   - Userspace client libraries
#   - Comprehensive test suite

cmake_minimum_required(VERSION 3.16)
project(InfernoAGIOS VERSION 0.3.0 LANGUAGES C CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_KERNEL_MODULES "Build kernel-level modules" ON)
option(BUILD_FILESYSTEMS "Build cognitive filesystem modules" ON)
option(BUILD_USERSPACE "Build userspace libraries" ON)
option(BUILD_TESTS "Build test suite" ON)
option(ENABLE_DISTRIBUTED "Enable distributed cognitive features" ON)
option(BUILD_PLN_ENGINE "Build PLN inference engine" ON)
option(BUILD_MOSES_ENGINE "Build MOSES evolutionary learning" ON)
option(BUILD_IPC "Build cognitive IPC channels" ON)
option(BUILD_PATTERN_MATCHER "Build pattern matching engine" ON)

# Platform detection
if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_definitions(-DPLATFORM_WINDOWS -D_WIN32_WINNT=0x0A00)
    # Suppress MSVC warnings for standard C functions
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    add_definitions(-DPLATFORM_LINUX)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
    add_definitions(-DPLATFORM_MACOS)
endif()

# Find dependencies
find_package(Threads REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/atomspace
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/reasoning
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/attention
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/cognitive9p
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/pattern
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/distributed
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/learning
    ${CMAKE_CURRENT_SOURCE_DIR}/fs
    ${CMAKE_CURRENT_SOURCE_DIR}/../cogutil/opencog/util
)

# ============================================================
# Kernel-compatible source files (portable C implementation)
# ============================================================
set(KERNEL_SOURCES
    kernel/atomspace/atomspace_portable.c
    kernel/reasoning/reasoning_portable.c
    kernel/attention/attention_portable.c
    kernel/cognitive9p/cognitive9p_portable.c
)

# PLN Inference Engine
if(BUILD_PLN_ENGINE)
    list(APPEND KERNEL_SOURCES kernel/reasoning/pln_engine.c)
endif()

# MOSES Evolutionary Learning
if(BUILD_MOSES_ENGINE)
    list(APPEND KERNEL_SOURCES kernel/learning/moses_engine.c)
endif()

# Cognitive IPC Channels
if(BUILD_IPC)
    list(APPEND KERNEL_SOURCES kernel/ipc/cognitive_channel.c)
endif()

# Pattern Matching Engine
if(BUILD_PATTERN_MATCHER)
    list(APPEND KERNEL_SOURCES kernel/pattern/pattern_matcher.c)
endif()

# Distributed AtomSpace
if(ENABLE_DISTRIBUTED)
    list(APPEND KERNEL_SOURCES kernel/distributed/distributed_atomspace.c)
endif()

# ============================================================
# Cognitive filesystem sources
# ============================================================
set(FILESYSTEM_SOURCES
    fs/atoms/atomfs.c
    fs/reasoning/reasonfs.c
    fs/attention/attnfs.c
    fs/learning/learnfs.c
    fs/perception/perceptfs.c
    fs/action/actionfs.c
)

# ============================================================
# Userspace library sources
# ============================================================
set(USERSPACE_SOURCES
    userspace/cogfs_client.c
    userspace/atomspace_client.c
    userspace/reasoning_client.c
)

# ============================================================
# Build targets
# ============================================================

# Kernel library
if(BUILD_KERNEL_MODULES)
    add_library(inferno_agi_kernel STATIC ${KERNEL_SOURCES})
    target_link_libraries(inferno_agi_kernel Threads::Threads)
    
    if(WIN32)
        target_link_libraries(inferno_agi_kernel ws2_32)
    else()
        target_link_libraries(inferno_agi_kernel m)
    endif()
endif()

# Cognitive filesystem library
if(BUILD_FILESYSTEMS)
    add_library(inferno_agi_cogfs STATIC ${FILESYSTEM_SOURCES})
    target_link_libraries(inferno_agi_cogfs Threads::Threads)
    
    if(WIN32)
        target_link_libraries(inferno_agi_cogfs ws2_32)
    endif()
endif()

# Userspace library
if(BUILD_USERSPACE)
    add_library(inferno_agi_user SHARED ${USERSPACE_SOURCES})
    target_link_libraries(inferno_agi_user Threads::Threads)
    
    if(BUILD_KERNEL_MODULES)
        target_link_libraries(inferno_agi_user inferno_agi_kernel)
    endif()
    if(BUILD_FILESYSTEMS)
        target_link_libraries(inferno_agi_user inferno_agi_cogfs)
    endif()
    
    if(WIN32)
        target_link_libraries(inferno_agi_user ws2_32)
    else()
        target_link_libraries(inferno_agi_user m)
    endif()
endif()

# ============================================================
# Test suite
# ============================================================
if(BUILD_TESTS)
    enable_testing()
    
    # Core tests
    add_executable(test_atomspace kernel/test/test_atomspace.c)
    target_link_libraries(test_atomspace inferno_agi_kernel)
    if(NOT WIN32)
        target_link_libraries(test_atomspace m)
    endif()
    add_test(NAME AtomSpaceTest COMMAND test_atomspace)
    
    add_executable(test_reasoning kernel/test/test_reasoning.c)
    target_link_libraries(test_reasoning inferno_agi_kernel)
    if(NOT WIN32)
        target_link_libraries(test_reasoning m)
    endif()
    add_test(NAME ReasoningTest COMMAND test_reasoning)
    
    add_executable(test_cognitive9p kernel/test/test_cognitive9p.c)
    target_link_libraries(test_cognitive9p inferno_agi_kernel)
    if(NOT WIN32)
        target_link_libraries(test_cognitive9p m)
    endif()
    add_test(NAME Cognitive9PTest COMMAND test_cognitive9p)
    
    # PLN Engine tests
    if(BUILD_PLN_ENGINE)
        add_executable(test_pln_engine kernel/test/test_pln_engine.c)
        target_link_libraries(test_pln_engine inferno_agi_kernel)
        if(NOT WIN32)
            target_link_libraries(test_pln_engine m)
        endif()
        add_test(NAME PLNEngineTest COMMAND test_pln_engine)
    endif()
    
    # MOSES Engine tests
    if(BUILD_MOSES_ENGINE)
        add_executable(test_moses_engine kernel/test/test_moses_engine.c)
        target_link_libraries(test_moses_engine inferno_agi_kernel)
        if(NOT WIN32)
            target_link_libraries(test_moses_engine m)
        endif()
        add_test(NAME MOSESEngineTest COMMAND test_moses_engine)
    endif()
    
    # IPC & Distributed tests
    if(BUILD_IPC AND ENABLE_DISTRIBUTED)
        add_executable(test_cognitive_ipc kernel/test/test_cognitive_ipc.c)
        target_link_libraries(test_cognitive_ipc inferno_agi_kernel)
        if(NOT WIN32)
            target_link_libraries(test_cognitive_ipc m)
        endif()
        add_test(NAME CognitiveIPCTest COMMAND test_cognitive_ipc)
    endif()
    
    # Integration tests
    add_executable(test_integration kernel/test/test_integration.c)
    target_link_libraries(test_integration inferno_agi_kernel)
    if(NOT WIN32)
        target_link_libraries(test_integration m)
    endif()
    add_test(NAME IntegrationTest COMMAND test_integration)
endif()

# ============================================================
# Installation
# ============================================================
install(TARGETS inferno_agi_kernel
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

if(BUILD_FILESYSTEMS)
    install(TARGETS inferno_agi_cogfs
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
    )
endif()

if(BUILD_USERSPACE)
    install(TARGETS inferno_agi_user
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()

install(FILES
    kernel/atomspace/atomspace.h
    kernel/reasoning/reasoning.h
    kernel/reasoning/pln_engine.h
    kernel/attention/attention.h
    kernel/cognitive9p/cognitive9p.h
    kernel/ipc/cognitive_channel.h
    kernel/pattern/pattern_matcher.h
    kernel/distributed/distributed_atomspace.h
    kernel/learning/moses_engine.h
    fs/atoms/atomfs.h
    DESTINATION include/inferno-agi
)

# ============================================================
# Configuration summary
# ============================================================
message(STATUS "")
message(STATUS "Inferno AGI OS Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Build kernel modules: ${BUILD_KERNEL_MODULES}")
message(STATUS "  Build filesystems: ${BUILD_FILESYSTEMS}")
message(STATUS "  Build userspace: ${BUILD_USERSPACE}")
message(STATUS "  Build tests: ${BUILD_TESTS}")
message(STATUS "  PLN Engine: ${BUILD_PLN_ENGINE}")
message(STATUS "  MOSES Engine: ${BUILD_MOSES_ENGINE}")
message(STATUS "  Cognitive IPC: ${BUILD_IPC}")
message(STATUS "  Pattern Matcher: ${BUILD_PATTERN_MATCHER}")
message(STATUS "  Distributed features: ${ENABLE_DISTRIBUTED}")
message(STATUS "")
