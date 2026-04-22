CYTHON_ADD_MODULE_PYX(cheminformatics
	cheminformatics-types
)

INCLUDE_DIRECTORIES(
	${Python3_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
)

ADD_LIBRARY(cheminformatics
	SHARED
	cheminformatics.cpp
)

TARGET_LINK_LIBRARIES(cheminformatics
	cheminformatics-types
	${ATOMSPACE_LIBRARIES}
	${Python3_LIBRARIES}
)

SET_TARGET_PROPERTIES(cheminformatics PROPERTIES
	PREFIX ""
	OUTPUT_NAME cheminformatics)

### install the modules ###
INSTALL(TARGETS
	cheminformatics
	DESTINATION "${PYTHON_DEST}")
