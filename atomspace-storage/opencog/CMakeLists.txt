
# The atom_types.h file is written to the build directory
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})

DECLARE_GUILE_CONFIG_TARGET(STORAGE_SCM_CONFIG "opencog as-storage-config" "ATOMSPACE_TEST")

ADD_SUBDIRECTORY (persist)

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/as-storage-config.scm STORAGE_SCM_CONFIG TRUE)

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/as-storage-config-installable.scm STORAGE_SCM_CONFIG FALSE)
INSTALL(FILES ${GUILE_BIN_DIR}/opencog/as-storage-config-installable.scm DESTINATION ${GUILE_SITE_DIR}/opencog RENAME as-storage-config.scm)
