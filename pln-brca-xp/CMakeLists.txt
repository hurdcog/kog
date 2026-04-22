cmake_minimum_required(VERSION 3.12)
project(pln_brca_xp VERSION 1.0.0 LANGUAGES CXX
        DESCRIPTION "PLN BRCA experiment")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/CMakeLists.txt")
    add_subdirectory(src)
endif()
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()
