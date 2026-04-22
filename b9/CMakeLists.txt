cmake_minimum_required(VERSION 3.12)
project(b9_system VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "b9: B-Series Rooted Trees (localhost connection patterns)")

set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# b9 is primarily Limbo-based; this CMakeLists.txt documents the component
# and provides any C support code needed for integration with C++ components.

message(STATUS "  b9: B-Series Rooted Trees (localhost) configured")
message(STATUS "    Limbo source: b9system.b")
message(STATUS "    b-files:           b9/b-files/")
message(STATUS "    connection-patterns: b9/connection-patterns/")
message(STATUS "    terminal-nodes:    b9/terminal-nodes/")
