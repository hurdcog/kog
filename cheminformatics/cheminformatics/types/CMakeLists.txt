
OPENCOG_ADD_ATOM_TYPES(atom_types.script
	atom_types.h
	atom_types.definitions
	atom_types.inheritance
	cheminformatics_types.scm
	cheminformatics_types.pyx
)
ADD_CUSTOM_TARGET(cheminformatics_atom_types DEPENDS atom_types.h)

# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

ADD_LIBRARY (cheminformatics-types SHARED
	CheminformaticsTypes.cc
)

# Without this, parallel make will race and crap up the generated files.
ADD_DEPENDENCIES(cheminformatics-types cheminformatics_atom_types)

TARGET_LINK_LIBRARIES(cheminformatics-types
	${ATOMSPACE_LIBRARIES}
)

INSTALL (TARGETS cheminformatics-types
	LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog"
)

ADD_GUILE_EXTENSION(SCM_CONFIG cheminformatics-types "opencog-ext-path-cheminfo")

INSTALL (FILES
	${CMAKE_CURRENT_BINARY_DIR}/atom_types.h
	DESTINATION "include/opencog/cheminformatics/types"
)

INSTALL (FILES
	${CMAKE_CURRENT_BINARY_DIR}/cheminformatics_types.scm
	DESTINATION "${GUILE_SITE_DIR}/opencog/cheminformatics/types"
)
