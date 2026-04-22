#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "coggml::coggml" for configuration "Release"
set_property(TARGET coggml::coggml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(coggml::coggml PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libcoggml.so"
  IMPORTED_SONAME_RELEASE "libcoggml.so"
  )

list(APPEND _cmake_import_check_targets coggml::coggml )
list(APPEND _cmake_import_check_files_for_coggml::coggml "${_IMPORT_PREFIX}/lib/libcoggml.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
