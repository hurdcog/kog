
DECLARE_GUILE_CONFIG_TARGET(SCM_CONFIG "opencog cheminfo-config" "CHEMINFO_TEST")

# Build C++ code for Cheminformatics atom-types.
ADD_SUBDIRECTORY (types)

IF (HAVE_CYTHON)
	ADD_SUBDIRECTORY (cython)
ENDIF (HAVE_CYTHON)

# Bogus boilerplate for unit-testing. This should be automated.
WRITE_GUILE_CONFIG(
	${GUILE_BIN_DIR}/opencog/cheminfo-config.scm
	SCM_CONFIG TRUE
)

WRITE_GUILE_CONFIG(
	${GUILE_BIN_DIR}/opencog/cheminfo-config-installable.scm
	SCM_CONFIG FALSE
)

INSTALL(
	FILES ${GUILE_BIN_DIR}/opencog/cheminfo-config-installable.scm
	DESTINATION ${GUILE_SITE_DIR}/opencog
	RENAME cheminfo-config.scm
)

ADD_GUILE_MODULE (FILES
	cheminformatics.scm
	MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog/cheminformatics"
)
