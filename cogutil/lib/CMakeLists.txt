# cogutil/lib/CMakeLists.txt
# Library configuration and packaging files

# This directory contains build configuration files but no source code to compile
# The actual library source code is in opencog/util/

# Install packaging files if needed
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cogutil.spec")
    INSTALL(FILES
        cogutil.spec
        DESTINATION ${DATADIR}/cogutil
    )
ENDIF()

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/buildbot-master.cfg")
    INSTALL(FILES
        buildbot-master.cfg
        DESTINATION ${DATADIR}/cogutil
    )
ENDIF()
