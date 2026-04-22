#
# Gnucash OpenCog Integration CMakeLists.txt
# Cognitive accounting and attention management
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(gnucash-opencog)

# Find required dependencies
find_package(PkgConfig REQUIRED)
find_package(Boost REQUIRED)
find_package(Guile REQUIRED)

# Check for Gnucash development libraries
pkg_check_modules(GNUCASH REQUIRED gnucash)

# Set compilation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${GNUCASH_INCLUDE_DIRS})
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files for OpenCog adapter
set(GNUCASH_OPENCOG_SOURCES
    src/gnucash_atomspace_adapter.cpp
    src/cognitive_accounting.cpp
    src/attention_manager.cpp
    src/transaction_converter.cpp
)

# Create shared library for Gnucash-OpenCog integration
add_library(gnucash-opencog SHARED ${GNUCASH_OPENCOG_SOURCES})

target_link_libraries(gnucash-opencog
    ${GNUCASH_LIBRARIES}
    ${Boost_LIBRARIES}
    ${GUILE_LIBRARIES}
)

# Install library and headers
install(TARGETS gnucash-opencog
    LIBRARY DESTINATION lib/opencog
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/opencog/gnucash
    FILES_MATCHING PATTERN "*.h"
)

# Install Scheme modules
install(FILES scm/gnucash.scm
    DESTINATION share/guile/site/3.0/opencog
)

# Install service configuration
install(FILES services/gnucash-daemon.scm
    DESTINATION etc/shepherd/services
)

# Build tests if enabled
option(BUILD_GNUCASH_TESTS "Build Gnucash integration tests" OFF)
if(BUILD_GNUCASH_TESTS)
    add_subdirectory(tests)
endif()

message(STATUS "Gnucash OpenCog integration configured successfully")