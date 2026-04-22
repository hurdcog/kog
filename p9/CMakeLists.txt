cmake_minimum_required(VERSION 3.12)
project(p9_system VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "p9: P-System Nested Scopes (globalhost execution contexts)")

set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# p9 is primarily Limbo-based; this CMakeLists.txt documents the component.

message(STATUS "  p9: P-System Nested Scopes (globalhost) configured")
message(STATUS "    Limbo source: p9system.b")
message(STATUS "    m-files:            p9/m-files/")
message(STATUS "    execution-contexts: p9/execution-contexts/")
message(STATUS "    thread-pools:       p9/thread-pools/")
