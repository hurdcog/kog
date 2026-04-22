#
# Ontogenetic Entelechy — CMakeLists.txt
#
# Reactor Core: arc-vortex + arc-helix + arc-halo => cyc-phoenix
#
# Builds two libraries:
#   - libentelechy: Cloninger, interoceptive, developmental, narrative, social, civic_angel, adapter
#   - libafi: Active Free-energy Inference engine, districts
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(entelechy VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# =========================================================================
# AFI Library (Active Free-energy Inference)
# =========================================================================
set(AFI_SOURCES
    src/afi/types.cpp
    src/afi/active_inference.cpp
    src/afi/district.cpp
)

set(AFI_HEADERS
    include/opencog/afi/types.hpp
    include/opencog/afi/active_inference.hpp
    include/opencog/afi/district.hpp
)

add_library(afi SHARED ${AFI_SOURCES})
target_include_directories(afi PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# =========================================================================
# Entelechy Library
# =========================================================================
set(ENTELECHY_SOURCES
    src/entelechy/types.cpp
    src/entelechy/cloninger.cpp
    src/entelechy/interoceptive.cpp
    src/entelechy/developmental.cpp
    src/entelechy/narrative.cpp
    src/entelechy/social.cpp
    src/entelechy/civic_angel.cpp
    src/entelechy/entelechy_adapter.cpp
)

set(ENTELECHY_HEADERS
    include/opencog/entelechy/types.hpp
    include/opencog/entelechy/cloninger.hpp
    include/opencog/entelechy/interoceptive.hpp
    include/opencog/entelechy/developmental.hpp
    include/opencog/entelechy/narrative.hpp
    include/opencog/entelechy/social.hpp
    include/opencog/entelechy/civic_angel.hpp
    include/opencog/entelechy/entelechy_adapter.hpp
)

add_library(entelechy SHARED ${ENTELECHY_SOURCES})
target_include_directories(entelechy PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(entelechy PUBLIC afi)

# =========================================================================
# Installation
# =========================================================================
install(TARGETS afi entelechy
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/opencog/afi
    DESTINATION include/opencog
    FILES_MATCHING PATTERN "*.hpp"
)

install(DIRECTORY include/opencog/entelechy
    DESTINATION include/opencog
    FILES_MATCHING PATTERN "*.hpp"
)

# =========================================================================
# Status
# =========================================================================
message(STATUS "")
message(STATUS "Ontogenetic Entelechy — Reactor Core")
message(STATUS "  arc-vortex + arc-helix + arc-halo => cyc-phoenix")
message(STATUS "  AFI sources:       ${AFI_SOURCES}")
message(STATUS "  Entelechy sources: ${ENTELECHY_SOURCES}")
message(STATUS "")
