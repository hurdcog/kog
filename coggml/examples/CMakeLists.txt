# CogGML Examples

cmake_minimum_required(VERSION 3.12)
project(coggml-examples CXX)

# Find the CogGML library
find_package(coggml QUIET)

if(NOT coggml_FOUND)
    # If not installed, use the local build
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/coggml)
endif()

# Communication demo example
add_executable(communication_demo communication_demo.cpp)
target_link_libraries(communication_demo PRIVATE coggml)

message(STATUS "CogGML examples configured")
