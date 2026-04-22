# FindLinkGrammar.cmake
# Find the Link Grammar library
#
# This module defines:
#  LINK_GRAMMAR_FOUND - system has Link Grammar
#  LINK_GRAMMAR_INCLUDE_DIRS - the Link Grammar include directories
#  LINK_GRAMMAR_LIBRARIES - Link Grammar libraries
#  LINK_GRAMMAR_VERSION - Link Grammar version

find_path(LINK_GRAMMAR_INCLUDE_DIR
    NAMES link-grammar/link-includes.h
    PATHS
        /usr/include
        /usr/local/include
)

find_library(LINK_GRAMMAR_LIBRARY
    NAMES link-grammar
    PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/x86_64-linux-gnu
        /usr/lib/aarch64-linux-gnu
)

# Try to find version
if(LINK_GRAMMAR_INCLUDE_DIR AND EXISTS "${LINK_GRAMMAR_INCLUDE_DIR}/link-grammar/link-features.h")
    file(STRINGS "${LINK_GRAMMAR_INCLUDE_DIR}/link-grammar/link-features.h" 
         LINK_GRAMMAR_VERSION_LINE 
         REGEX "^#define[ \t]+VERSION[ \t]+\"[0-9]+\\.[0-9]+\\.[0-9]+\"")
    
    if(LINK_GRAMMAR_VERSION_LINE)
        string(REGEX REPLACE "^#define[ \t]+VERSION[ \t]+\"([0-9]+\\.[0-9]+\\.[0-9]+)\".*" "\\1" 
               LINK_GRAMMAR_VERSION "${LINK_GRAMMAR_VERSION_LINE}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LinkGrammar
    REQUIRED_VARS LINK_GRAMMAR_LIBRARY LINK_GRAMMAR_INCLUDE_DIR
    VERSION_VAR LINK_GRAMMAR_VERSION
)

if(LINK_GRAMMAR_FOUND)
    set(LINK_GRAMMAR_LIBRARIES ${LINK_GRAMMAR_LIBRARY})
    set(LINK_GRAMMAR_INCLUDE_DIRS ${LINK_GRAMMAR_INCLUDE_DIR})
    
    if(NOT TARGET LinkGrammar::LinkGrammar)
        add_library(LinkGrammar::LinkGrammar UNKNOWN IMPORTED)
        set_target_properties(LinkGrammar::LinkGrammar PROPERTIES
            IMPORTED_LOCATION "${LINK_GRAMMAR_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LINK_GRAMMAR_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(
    LINK_GRAMMAR_INCLUDE_DIR
    LINK_GRAMMAR_LIBRARY
)
