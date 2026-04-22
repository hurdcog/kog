cmake_minimum_required(VERSION 3.12)
project(distributed_agi VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "Distributed AtomSpace coordination for cognitive clusters")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(distributed_agi STATIC
    partitioning.c
    replication.c
    synchronization.c
)

target_include_directories(distributed_agi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Simple test executable
add_executable(distributed_agi_test
    partitioning.c
    replication.c
    synchronization.c
)

target_compile_definitions(distributed_agi_test PRIVATE STANDALONE_TEST=1)

message(STATUS "  distributed-agi: partition/replication/sync configured")
