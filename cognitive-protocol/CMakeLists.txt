cmake_minimum_required(VERSION 3.12)
project(cognitive_protocol VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "9P-Cog: Styx-based cognitive protocol extensions")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Protocol implementation (platform-agnostic C)
add_library(styx_cog OBJECT
    styx-cog.c
)

target_include_directories(styx_cog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install headers
install(FILES styx-cog.h
    DESTINATION include/cognitive-protocol
    OPTIONAL
)

message(STATUS "  cognitive-protocol: 9P-Cog Styx extensions configured")
