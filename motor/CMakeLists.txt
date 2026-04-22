#
# Empty, for now.
# Required so that build infrastructure doesn't break.

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(motor-atomese)

SET(SEMANTIC_VERSION 0.0.1)


# Bogus install target
# include(GNUInstallDirs)
INSTALL (FILES
	README.md
	# DESTINATION ${CMAKE_INSTALL_DOCDIR}
	DESTINATION "share/doc/opencog/motor"
)
