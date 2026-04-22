IF (WIN32 AND NOT UNIX)
	SET(WIN32_GETOPT_FILES getopt getopt_long)
ELSE (WIN32 AND NOT UNIX)
	SET(WIN32_GETOPT_FILES)
ENDIF (WIN32 AND NOT UNIX)

include(CheckSymbolExists)
include(CheckCSourceCompiles)

# The below are used to automatically find module load paths
ADD_DEFINITIONS(
   -DCMAKE_INSTALL_PREFIX="\\"${CMAKE_INSTALL_PREFIX}\\""
)

ADD_LIBRARY(cogutil SHARED
	algorithm.h
	backtrace-symbols.c
	Config.cc
	dorepeat.h
	exceptions.cc
	lazy_selector.cc
	lazy_random_selector.cc
	Logger.cc
	misc.cc
	mt19937ar.cc
	oc_assert.cc
	oc_omp.cc
	platform.cc
	random.h
	${WIN32_GETOPT_FILES}
)

# Windows-specific library linking
IF (WIN32)
	TARGET_LINK_LIBRARIES(cogutil
		psapi       # For GetProcessMemoryInfo
		kernel32    # For GlobalMemoryStatusEx, etc.
	)
ENDIF (WIN32)

IF (HAVE_BFD AND HAVE_IBERTY)
	check_symbol_exists(bfd_get_section_flags "bfd.h" HAVE_DECL_BFD_GET_SECTION_FLAGS)
	check_symbol_exists(bfd_section_flags "bfd.h" HAVE_DECL_BFD_SECTION_FLAGS)

	check_symbol_exists(bfd_get_section_vma "bfd.h" HAVE_DECL_BFD_GET_SECTION_VMA)
	check_symbol_exists(bfd_section_vma "bfd.h" HAVE_DECL_BFD_SECTION_VMA)

	if(HAVE_DECL_BFD_GET_SECTION_FLAGS)
		# This should be
		# add_compile_definitions(HAVE_DECL_BFD_GET_SECTION_FLAGS)
		# except that requires CMake version 3.12 which is not available
		# in Ubuntu 18.04 LTS -- FIXME someday...
		add_definitions(-DHAVE_DECL_BFD_GET_SECTION_FLAGS=1)
	endif()
	if(HAVE_DECL_BFD_SECTION_FLAGS)
		add_definitions(-DHAVE_DECL_BFD_SECTION_FLAGS=1)
	endif()
	if(HAVE_DECL_BFD_GET_SECTION_VMA)
		add_definitions(-DHAVE_DECL_BFD_GET_SECTION_VMA=1)
	endif()
	if(HAVE_DECL_BFD_SECTION_VMA)
		add_definitions(-DHAVE_DECL_BFD_SECTION_VMA=1)
	endif()

	# Check if bfd_section_size takes one or two arguments.
	set(CMAKE_REQUIRED_LIBRARIES ${BFD_LIBRARY})
	check_c_source_compiles(
		"#include <bfd.h>
		#include <malloc.h>
		int main(int argc, char** argv) {
			asection *sec = (asection*) malloc(sizeof(*sec));
			bfd_section_size(sec);
			free(sec);
			return 0;
		}"
		HAVE_1_ARG_BFD_SECTION_SIZE
	)
	if(HAVE_1_ARG_BFD_SECTION_SIZE)
		add_definitions(-DHAVE_1_ARG_BFD_SECTION_SIZE=1)
	else()
		add_definitions(-DHAVE_2_ARG_BFD_SECTION_SIZE=1)
	endif()

	# Check if bfd_section_vma takes one or two arguments.
	check_c_source_compiles(
		"#include <bfd.h>
		#include <malloc.h>
		int main(int argc, char** argv) {
			asection *sec = (asection *) malloc(sizeof(*sec));
			bfd_section_vma(sec);
			free(sec);
			return 0;
		}"
		HAVE_1_ARG_BFD_SECTION_VMA
	)
	if(HAVE_1_ARG_BFD_SECTION_VMA)
		add_definitions(-DHAVE_1_ARG_BFD_SECTION_VMA=1)
	else()
		add_definitions(-DHAVE_2_ARG_BFD_SECTION_VMA=1)
	endif()

	INCLUDE_DIRECTORIES(${IBERTY_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(cogutil
		${BFD_LIBRARY}
		iberty
	)
ENDIF (HAVE_BFD AND HAVE_IBERTY)

INSTALL(FILES
	algorithm.h
	async_buffer.h
	async_method_caller.h
	backtrace-symbols.h
	based_variant.h
	cogutil.h
	comprehension.h
	Config.h
	Counter.h
	concurrent_queue.h
	concurrent_set.h
	concurrent_stack.h
	digraph.h
	dorepeat.h
	empty_string.h
	exceptions.h
	functional.h
	hashing.h
	iostreamContainer.h
	jaccard_index.h
	KLD.h
	lazy_normal_selector.h
	lazy_random_selector.h
	lazy_selector.h
	log_prog_name.h
	Logger.h
	lru_cache.h
	macros.h
	MannWhitneyU.h
	misc.h
	mt19937ar.h
	numeric.h
	oc_assert.h
	oc_omp.h
	platform.h
	pool.h
	ranking.h
	RandGen.h
	random.h
	selection.h
	sigslot.h
	tree.h
	zipf.h
	DESTINATION "include/opencog/util"
)

IF (CYGWIN)
    INSTALL(TARGETS cogutil DESTINATION "bin/opencog")
    ADD_CUSTOM_COMMAND (
        TARGET cogutil 
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/opencog/util/cygcogutil.dll ${CMAKE_BINARY_DIR}/tests/util/ 
        MAIN_DEPENDENCY cogutil)
ELSE (CYGWIN) #Linux
    INSTALL(TARGETS cogutil
        EXPORT AtomSpaceTargets
        LIBRARY DESTINATION "lib${LIB_DIR_SUFFIX}/opencog")
ENDIF (CYGWIN)

# Install boost_ext headers
INSTALL(DIRECTORY boost_ext
	DESTINATION "include/opencog/util"
	FILES_MATCHING PATTERN "*.h"
)
