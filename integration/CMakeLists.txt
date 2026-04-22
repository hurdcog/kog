CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
PROJECT(occ-integration VERSION 1.0.0)

MESSAGE(STATUS "")
MESSAGE(STATUS "==============================================")
MESSAGE(STATUS "Building OCC Integration Layer")
MESSAGE(STATUS "==============================================")

# Integration components
OPTION(BUILD_ATOMSPACE_MACHSPACE "Build AtomSpace-MachSpace bridge" ON)
OPTION(BUILD_COGNITIVE_SCHEDULER "Build cognitive scheduler" ON)
OPTION(BUILD_COGKERNEL_BRIDGE "Build CogKernel bridge" OFF)

# AtomSpace-MachSpace bridge
IF(BUILD_ATOMSPACE_MACHSPACE)
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-machspace/CMakeLists.txt")
        MESSAGE(STATUS "Building AtomSpace-MachSpace bridge...")
        add_subdirectory(atomspace-machspace)
    ELSE()
        MESSAGE(WARNING "AtomSpace-MachSpace bridge CMakeLists.txt not found")
    ENDIF()
ENDIF()

# Cognitive scheduler
IF(BUILD_COGNITIVE_SCHEDULER)
    MESSAGE(STATUS "Building cognitive scheduler...")
    
    # Build cognitive scheduler library
    ADD_LIBRARY(cognitive-scheduler STATIC
        cognitive-scheduler/cognitive_scheduler.c
    )
    
    TARGET_INCLUDE_DIRECTORIES(cognitive-scheduler PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/cognitive-scheduler
    )
    
    # Install library and headers
    INSTALL(TARGETS cognitive-scheduler
        ARCHIVE DESTINATION lib
    )
    
    INSTALL(FILES
        cognitive-scheduler/cognitive_scheduler.h
        DESTINATION include/cognumach
    )
ENDIF()

# CogKernel bridge (to be implemented)
IF(BUILD_COGKERNEL_BRIDGE)
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cogkernel-bridge/CMakeLists.txt")
        MESSAGE(STATUS "Building CogKernel bridge...")
        add_subdirectory(cogkernel-bridge)
    ELSE()
        MESSAGE(WARNING "CogKernel bridge not yet implemented")
    ENDIF()
ENDIF()

# Install integration bridge scripts
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../hurdcog-integration/atomspace-hurdcog-bridge.py")
    MESSAGE(STATUS "Installing AtomSpace-HurdCog bridge script...")
    INSTALL(
        FILES ${CMAKE_CURRENT_SOURCE_DIR}/../hurdcog-integration/atomspace-hurdcog-bridge.py
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    )
ENDIF()

# Check if we're building the full AGI-OS stack
IF(BUILD_COGNUMACH AND BUILD_HURDCOG)
    MESSAGE(STATUS "Full AGI-OS stack detected")
    SET(AGI_OS_FULL_STACK ON)
ELSE()
    MESSAGE(STATUS "Userspace-only OCC build")
    SET(AGI_OS_FULL_STACK OFF)
ENDIF()

MESSAGE(STATUS "")
MESSAGE(STATUS "Integration Layer Summary:")
MESSAGE(STATUS "  AGI-OS Full Stack: ${AGI_OS_FULL_STACK}")
MESSAGE(STATUS "  AtomSpace-MachSpace: ${BUILD_ATOMSPACE_MACHSPACE}")
MESSAGE(STATUS "  Cognitive Scheduler: ${BUILD_COGNITIVE_SCHEDULER}")
MESSAGE(STATUS "  CogKernel Bridge: ${BUILD_COGKERNEL_BRIDGE}")
MESSAGE(STATUS "==============================================")
MESSAGE(STATUS "")

MESSAGE(STATUS "Integration layer configuration complete")
