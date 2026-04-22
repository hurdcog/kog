cmake_minimum_required(VERSION 3.12)

project(CognitiveArchitectureDemo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories for all components
include_directories(
    ${CMAKE_SOURCE_DIR}/../../coggml/include
    ${CMAKE_SOURCE_DIR}/../../cogself/include
    ${CMAKE_SOURCE_DIR}/../../atomspace-accelerator/include
    ${CMAKE_SOURCE_DIR}/../../agentic-chatbots/include
)

# Add library directories
link_directories(
    ${CMAKE_SOURCE_DIR}/../../build-test/coggml
    ${CMAKE_SOURCE_DIR}/../../build-test/cogself
    ${CMAKE_SOURCE_DIR}/../../build-test/atomspace-accelerator
    ${CMAKE_SOURCE_DIR}/../../build-test/agentic-chatbots
)

# Create the demo executable
add_executable(cognitive_demo demo.cpp)

# Link all cognitive architecture libraries
target_link_libraries(cognitive_demo
    coggml
    cogself
    atomspace_accelerator
    agentic_chatbots
    pthread
)

message(STATUS "Cognitive Architecture Demo configured")
