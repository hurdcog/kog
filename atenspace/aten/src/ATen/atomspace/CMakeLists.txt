# ATenSpace - Tensor-based AtomSpace
cmake_minimum_required(VERSION 3.5)

# Find ATen
find_package(Torch REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${TORCH_INCLUDE_DIRS})

# Example executable
add_executable(atomspace_example example.cpp)
target_link_libraries(atomspace_example ${TORCH_LIBRARIES})
set_property(TARGET atomspace_example PROPERTY CXX_STANDARD 17)

# Advanced example executable
add_executable(atomspace_example_advanced example_advanced.cpp)
target_link_libraries(atomspace_example_advanced ${TORCH_LIBRARIES})
set_property(TARGET atomspace_example_advanced PROPERTY CXX_STANDARD 17)

# PLN example executable
add_executable(atomspace_example_pln example_pln.cpp)
target_link_libraries(atomspace_example_pln ${TORCH_LIBRARIES})
set_property(TARGET atomspace_example_pln PROPERTY CXX_STANDARD 17)

# ECAN example executable
add_executable(atomspace_example_ecan example_ecan.cpp)
target_link_libraries(atomspace_example_ecan ${TORCH_LIBRARIES})
set_property(TARGET atomspace_example_ecan PROPERTY CXX_STANDARD 17)

# Test executable
add_executable(atomspace_test test.cpp)
target_link_libraries(atomspace_test ${TORCH_LIBRARIES})
set_property(TARGET atomspace_test PROPERTY CXX_STANDARD 17)

# Advanced features test executable
add_executable(atomspace_test_advanced test_advanced.cpp)
target_link_libraries(atomspace_test_advanced ${TORCH_LIBRARIES})
set_property(TARGET atomspace_test_advanced PROPERTY CXX_STANDARD 17)

# PLN test executable
add_executable(atomspace_test_pln test_pln.cpp)
target_link_libraries(atomspace_test_pln ${TORCH_LIBRARIES})
set_property(TARGET atomspace_test_pln PROPERTY CXX_STANDARD 17)

# ECAN test executable
add_executable(atomspace_test_ecan test_ecan.cpp)
target_link_libraries(atomspace_test_ecan ${TORCH_LIBRARIES})
set_property(TARGET atomspace_test_ecan PROPERTY CXX_STANDARD 17)

# Installation
install(FILES 
    Atom.h 
    AtomSpace.h 
    ATenSpace.h
    TimeServer.h
    AttentionBank.h
    Serializer.h
    PatternMatcher.h
    TruthValue.h
    ForwardChainer.h
    BackwardChainer.h
    ECAN.h
    README.md
    DESTINATION include/ATen/atomspace
)
