#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "cogself::cogself" for configuration "Release"
set_property(TARGET cogself::cogself APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(cogself::cogself PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcogself.so"
  IMPORTED_SONAME_RELEASE "libcogself.so"
  )

list(APPEND _cmake_import_check_targets cogself::cogself )
list(APPEND _cmake_import_check_files_for_cogself::cogself "${_IMPORT_PREFIX}/lib/libcogself.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
