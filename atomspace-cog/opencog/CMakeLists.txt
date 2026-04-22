
DECLARE_GUILE_CONFIG_TARGET(COG_SCM_CONFIG "opencog cog-config" "OPENCOG_TEST")

ADD_SUBDIRECTORY (persist)

ADD_GUILE_MODULE (FILES
   scm/persist-cog.scm
   scm/persist-cog-simple.scm
   MODULE_DESTINATION "${GUILE_SITE_DIR}/opencog"
)

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/cog-config.scm COG_SCM_CONFIG TRUE)

WRITE_GUILE_CONFIG(${GUILE_BIN_DIR}/opencog/cog-config-installable.scm COG_SCM_CONFIG FALSE)
INSTALL(FILES ${GUILE_BIN_DIR}/opencog/cog-config-installable.scm DESTINATION ${GUILE_SITE_DIR}/opencog RENAME cog-config.scm)
