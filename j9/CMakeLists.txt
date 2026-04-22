cmake_minimum_required(VERSION 3.12)
project(j9_system VERSION 1.0.0 LANGUAGES C
        DESCRIPTION "j9: J-Surface Elementary Differentials (orgalhost topology)")

set(CMAKE_C_STANDARD 99)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# j9 is primarily Limbo-based; this CMakeLists.txt documents the component.

message(STATUS "  j9: J-Surface Differentials (orgalhost) configured")
message(STATUS "    Limbo source: j9system.b")
message(STATUS "    dis-files:          j9/dis-files/")
message(STATUS "    compute-gradients:  j9/compute-gradients/")
message(STATUS "    topology-nets:      j9/topology-nets/")
