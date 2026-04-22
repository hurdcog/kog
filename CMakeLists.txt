#
# OpenCog Collection (OCC) - Root CMakeLists.txt
# Monorepo integration for cognitive synergy
#
# This CMakeLists.txt coordinates building the core OpenCog components
# in the optimal order for integration.
#

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)

PROJECT(opencog-collection)

# Set default build type
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE Release)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

MESSAGE(STATUS "AGI-OS Complete Stack build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "Three-layer architecture: Cognumach → HurdCog → OCC")

# Global compilation flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Options for selective building

# AGI-OS Layer 1: Cognitive Microkernel
OPTION(BUILD_COGNUMACH "Build Cognumach (GNU Mach) microkernel - Layer 1" OFF)

# AGI-OS Layer 2: Cognitive Operating System
OPTION(BUILD_HURDCOG "Build HurdCog (GNU Hurd + cognitive extensions) - Layer 2" OFF)

# Core OpenCog components
OPTION(BUILD_COGUTIL "Build CogUtil library" ON)
OPTION(BUILD_ATOMSPACE "Build AtomSpace hypergraph database" ON)  
OPTION(BUILD_COGSERVER "Build CogServer networking" ON)
OPTION(BUILD_MATRIX "Build Matrix sparse vector support" ON)
OPTION(BUILD_LEARN "Build symbolic learning" ON)
OPTION(BUILD_AGENTS "Build interactive agents" ON)
OPTION(BUILD_SENSORY "Build sensory dataflow system" ON)

# New cognitive architecture components
OPTION(BUILD_COGGML "Build CogGML self-aware microkernel" ON)
OPTION(BUILD_COGSELF "Build CogSelf AGI synergy framework" ON)
OPTION(BUILD_ATOMSPACE_ACCELERATOR "Build AtomSpace accelerator inference engine" ON)
OPTION(BUILD_AGENTIC_CHATBOTS "Build agentic chatbots integration" ON)
OPTION(BUILD_ENTELECHY "Build Ontogenetic Entelechy (Cognitive City / Civic Angel / AFI)" ON)

# Tensor Logic components
OPTION(BUILD_ATEN "Build ATen tensor library" ON)
OPTION(BUILD_ATENSPACE "Build ATenSpace (AtomSpace + Tensor embeddings)" ON)
OPTION(BUILD_TENSOR_LOGIC "Build Tensor Logic integration layer" ON)

# External integration packages
OPTION(BUILD_GNUCASH "Build Gnucash cognitive accounting integration" OFF)
OPTION(BUILD_KOBOLDCPP "Build KoboldCpp story/world modeling integration" OFF)
OPTION(BUILD_APHRODITE "Build Aphrodite Engine LLM inference integration" OFF)

# ============================================================================
# AGI-OS Build Sequence - Three Layer Architecture
# ============================================================================
# Layer 0: Build Tools (MIG - Mach Interface Generator)
# Layer 1: Cognumach (Microkernel)
# Layer 2: HurdCog (Operating System + Cognitive Extensions) 
# Layer 3: OCC (AGI Framework)
# ============================================================================

# Layer 0: Build Tools - MIG (Mach Interface Generator)
IF(BUILD_COGNUMACH OR BUILD_HURDCOG)
    MESSAGE(STATUS "Building Layer 0: MIG (Mach Interface Generator)...")
    MESSAGE(STATUS "  Location: build-tools/mig/")
    MESSAGE(STATUS "  Note: Centralized MIG for all Mach-based components")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build-tools/mig/Makefile.am")
        # MIG uses autotools, so we'll handle it separately
        MESSAGE(STATUS "  MIG found at build-tools/mig/ (autotools-based)")
    ELSE()
        MESSAGE(WARNING "  MIG not found at build-tools/mig/")
    ENDIF()
ENDIF()

# Layer 1: Cognumach Microkernel (autotools-based)
IF(BUILD_COGNUMACH)
    MESSAGE(STATUS "Building Layer 1: Cognumach Microkernel...")
    MESSAGE(STATUS "  Note: Requires autotools (autoconf, automake, libtool)")
    add_subdirectory(cognumach)
ENDIF()

# Layer 2: HurdCog Operating System (autotools-based + cognitive extensions)
IF(BUILD_HURDCOG)
    MESSAGE(STATUS "Building Layer 2: HurdCog Operating System...")
    MESSAGE(STATUS "  Note: Requires Cognumach and autotools")
    IF(BUILD_COGNUMACH)
        add_subdirectory(hurdcog)
    ELSE()
        MESSAGE(WARNING "HurdCog requires Cognumach. Skipping HurdCog build.")
    ENDIF()
ENDIF()

# Layer 3: OCC Components (CMake-based)
MESSAGE(STATUS "Building Layer 3: OpenCog Collection (OCC)...")

# Core components built in dependency order
IF(BUILD_COGUTIL)
    MESSAGE(STATUS "Building CogUtil...")
    add_subdirectory(cogutil)
    set(CogUtil_DIR "${CMAKE_BINARY_DIR}/cogutil/cmake" CACHE PATH "In-tree CogUtil package config" FORCE)
    list(APPEND CMAKE_PREFIX_PATH "${CogUtil_DIR}")

    # For in-tree monorepo builds: set CogUtil variables so downstream
    # components can find cmake modules and headers without installation
    set(COGUTIL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cogutil" CACHE PATH "CogUtil include dir" FORCE)
    set(COGUTIL_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cogutil" CACHE PATH "CogUtil data dir" FORCE)
    set(COGUTIL_FOUND 1 CACHE BOOL "CogUtil found" FORCE)
    set(COGUTIL_VERSION "2.1.0" CACHE STRING "CogUtil version" FORCE)
    # Make cogutil cmake modules available to all components
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cogutil/cmake")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/atomspace/cmake")
ENDIF()

# CogGML Microkernel - foundational for cognitive architecture
IF(BUILD_COGGML)
    MESSAGE(STATUS "Building CogGML Microkernel...")
    add_subdirectory(coggml)
ENDIF()

IF(BUILD_ATOMSPACE AND BUILD_COGUTIL)
    MESSAGE(STATUS "Building AtomSpace...")
    add_subdirectory(atomspace)
    set(AtomSpace_DIR "${CMAKE_BINARY_DIR}/atomspace/lib" CACHE PATH "In-tree AtomSpace package config" FORCE)
    list(APPEND CMAKE_PREFIX_PATH "${AtomSpace_DIR}")

    # For in-tree monorepo builds: set AtomSpace variables so downstream
    # components (storage, cogserver, etc.) can find headers and data
    set(ATOMSPACE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/atomspace" CACHE PATH "AtomSpace include dir" FORCE)
    set(ATOMSPACE_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/atomspace" CACHE PATH "AtomSpace data dir" FORCE)
    set(ATOMSPACE_FOUND 1 CACHE BOOL "AtomSpace found" FORCE)
    # Mark that we're doing an in-tree build so configs can skip target imports
    set(ATOMSPACE_IN_TREE_BUILD TRUE CACHE BOOL "AtomSpace is built in-tree" FORCE)
ENDIF()

# AtomSpace Accelerator - can build independently for cognitive architecture
IF(BUILD_ATOMSPACE_ACCELERATOR)
    MESSAGE(STATUS "Building AtomSpace Accelerator Inference Engine...")
    add_subdirectory(atomspace-accelerator)
ENDIF()

# ============================================================================
# Tensor Logic Components - Multi-Entity Network-Aware Tensor-Enhanced Synergy
# ============================================================================
# ATen: C++11 tensor library (foundational)
IF(BUILD_ATEN)
    MESSAGE(STATUS "Building ATen Tensor Library...")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/aten/CMakeLists.txt")
        add_subdirectory(aten)
    ELSEIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/aten/aten/CMakeLists.txt")
        add_subdirectory(aten/aten)
    ELSE()
        MESSAGE(WARNING "ATen CMakeLists.txt not found - skipping ATen build")
    ENDIF()
ENDIF()

# ATenSpace: Bridge symbolic AI (AtomSpace) with neural tensor embeddings
IF(BUILD_ATENSPACE AND BUILD_ATEN)
    MESSAGE(STATUS "Building ATenSpace (AtomSpace + Tensor Embeddings)...")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atenspace/CMakeLists.txt")
        add_subdirectory(atenspace)
    ELSEIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atenspace/aten/CMakeLists.txt")
        add_subdirectory(atenspace/aten)
    ELSE()
        MESSAGE(WARNING "ATenSpace CMakeLists.txt not found - skipping ATenSpace build")
    ENDIF()
ENDIF()

# Tensor Logic Integration Layer
IF(BUILD_TENSOR_LOGIC AND BUILD_ATENSPACE AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Tensor Logic Integration Layer...")
    MESSAGE(STATUS "  Features: Multi-Entity AtomSpace, Multi-Scale Analysis,")
    MESSAGE(STATUS "            Network-Aware DAS, ESN Reservoir Computing")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tensor-logic/CMakeLists.txt")
        add_subdirectory(tensor-logic)
    ELSE()
        MESSAGE(STATUS "  tensor-logic/ directory will be created for integration")
    ENDIF()
ENDIF()

# AtomSpace Storage - MUST be built before CogServer ⚠️ CRITICAL
OPTION(BUILD_ATOMSPACE_STORAGE "Build AtomSpace storage backends" ON)
IF(BUILD_ATOMSPACE_STORAGE AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building AtomSpace Storage (CRITICAL: required by CogServer)...")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-storage/CMakeLists.txt")
        add_subdirectory(atomspace-storage)
        # Set up in-tree AtomSpaceStorage for downstream components
        set(AtomSpaceStorage_DIR "${CMAKE_BINARY_DIR}/atomspace-storage/cmake" CACHE PATH "In-tree AtomSpaceStorage config" FORCE)
        list(APPEND CMAKE_PREFIX_PATH "${AtomSpaceStorage_DIR}")
        set(ATOMSPACE_STORAGE_FOUND 1 CACHE BOOL "AtomSpaceStorage found" FORCE)
        set(ATOMSPACE_STORAGE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-storage" CACHE PATH "AtomSpaceStorage include dir" FORCE)
    ELSE()
        MESSAGE(WARNING "atomspace-storage not found - CogServer build will fail!")
    ENDIF()
    # atomspace-rocks requires RocksDB - check before adding
    find_package(RocksDB QUIET)
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-rocks/CMakeLists.txt" AND ROCKSDB_FOUND)
        add_subdirectory(atomspace-rocks)
    ELSEIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-rocks/CMakeLists.txt")
        MESSAGE(STATUS "RocksDB not found - skipping atomspace-rocks build")
    ENDIF()
    # atomspace-cog needs CogServer - built after cogserver below
ENDIF()

IF(BUILD_COGSERVER AND BUILD_ATOMSPACE AND BUILD_ATOMSPACE_STORAGE)
    MESSAGE(STATUS "Building CogServer...")
    add_subdirectory(cogserver)
    # Set up in-tree CogServer for downstream components
    set(CogServer_DIR "${CMAKE_BINARY_DIR}/cogserver/lib" CACHE PATH "In-tree CogServer config" FORCE)
    list(APPEND CMAKE_PREFIX_PATH "${CogServer_DIR}")
    set(COGSERVER_FOUND 1 CACHE BOOL "CogServer found" FORCE)
ENDIF()

# atomspace-cog - needs CogServer and AtomSpaceStorage (built after both)
IF(BUILD_ATOMSPACE_STORAGE AND BUILD_COGSERVER AND BUILD_ATOMSPACE)
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-cog/CMakeLists.txt")
        MESSAGE(STATUS "Building AtomSpace-Cog...")
        add_subdirectory(atomspace-cog)
    ENDIF()
ENDIF()

IF(BUILD_MATRIX AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Matrix...")
    add_subdirectory(matrix)
ENDIF()

IF(BUILD_LEARN AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Learn...")
    add_subdirectory(learn)
ENDIF()

IF(BUILD_AGENTS AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Agents...")
    add_subdirectory(agents)
ENDIF()

IF(BUILD_SENSORY AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Sensory...")
    add_subdirectory(sensory)
ENDIF()

# Agentic Chatbots - can build independently for cognitive architecture
IF(BUILD_AGENTIC_CHATBOTS)
    MESSAGE(STATUS "Building Agentic Chatbots...")
    add_subdirectory(agentic-chatbots)
ENDIF()

# CogSelf AGI Framework - coordinates all cognitive components
IF(BUILD_COGSELF AND BUILD_COGGML)
    MESSAGE(STATUS "Building CogSelf AGI Cognitive Synergy Framework...")
    add_subdirectory(cogself)
ENDIF()

# Ontogenetic Entelechy - Reactor Core: arc-vortex + arc-helix + arc-halo => cyc-phoenix
IF(BUILD_ENTELECHY AND BUILD_COGSELF)
    MESSAGE(STATUS "Building Ontogenetic Entelechy (Cognitive City / Civic Angel / AFI)...")
    add_subdirectory(entelechy)
ENDIF()

# External integration packages
IF(BUILD_GNUCASH AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Gnucash cognitive accounting integration...")
    add_subdirectory(gnucash)
ENDIF()

IF(BUILD_KOBOLDCPP AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building KoboldCpp story/world modeling integration...")
    add_subdirectory(koboldcpp)
ENDIF()

IF(BUILD_APHRODITE AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building Aphrodite Engine LLM inference integration...")
    add_subdirectory(aphrodite-engine)
ENDIF()

# Integration layer
OPTION(BUILD_INTEGRATION_LAYER "Build integration bridges" ON)
IF(BUILD_INTEGRATION_LAYER)
    MESSAGE(STATUS "Building integration layer...")
    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/integration/CMakeLists.txt")
        add_subdirectory(integration)
    ENDIF()
ENDIF()

# Optional: Additional AtomSpace extensions
OPTION(BUILD_ATOMSPACE_EXTENSIONS "Build AtomSpace language extensions" OFF)
IF(BUILD_ATOMSPACE_EXTENSIONS AND BUILD_ATOMSPACE)
    add_subdirectory(atomspace-metta)
    add_subdirectory(atomspace-bridge)
ENDIF()

# Print summary
# ============================================================================
# AGI-OS Integration Components
# ============================================================================
OPTION(BUILD_AGI_OS_INTEGRATION "Build AGI-OS integration components" ON)
IF(BUILD_AGI_OS_INTEGRATION AND BUILD_HURDCOG AND BUILD_ATOMSPACE)
    MESSAGE(STATUS "Building AGI-OS integration components...")
    MESSAGE(STATUS "  - hurdcog-atomspace-bridge (kernel-level hypergraph)")
    MESSAGE(STATUS "  - cognumach-cognitive-scheduler (attention-aware scheduling)")
    MESSAGE(STATUS "  - hurdcog-occ-bridge (OS-framework bridge)")
    MESSAGE(STATUS "  Note: Build using opencog-debian packaging system")
ENDIF()

MESSAGE(STATUS "")
MESSAGE(STATUS "AGI-OS Configuration Summary:")
MESSAGE(STATUS "========================================")
MESSAGE(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
MESSAGE(STATUS "")
MESSAGE(STATUS "AGI-OS Stack:")
MESSAGE(STATUS "Layer 1 - Cognumach (Microkernel): ${BUILD_COGNUMACH}")
MESSAGE(STATUS "Layer 2 - HurdCog (OS + Cognitive): ${BUILD_HURDCOG}")
MESSAGE(STATUS "Layer 3 - OCC (AGI Framework): enabled")
MESSAGE(STATUS "")
MESSAGE(STATUS "Core Components:")
MESSAGE(STATUS "CogUtil: ${BUILD_COGUTIL}")
MESSAGE(STATUS "AtomSpace: ${BUILD_ATOMSPACE}")
MESSAGE(STATUS "CogServer: ${BUILD_COGSERVER}")
MESSAGE(STATUS "Matrix: ${BUILD_MATRIX}")
MESSAGE(STATUS "Learn: ${BUILD_LEARN}")
MESSAGE(STATUS "Agents: ${BUILD_AGENTS}")
MESSAGE(STATUS "Sensory: ${BUILD_SENSORY}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Cognitive Architecture Components:")
MESSAGE(STATUS "CogGML Microkernel (self-aware shards): ${BUILD_COGGML}")
MESSAGE(STATUS "CogSelf AGI Framework: ${BUILD_COGSELF}")
MESSAGE(STATUS "Ontogenetic Entelechy (Civic Angel / AFI): ${BUILD_ENTELECHY}")
MESSAGE(STATUS "AtomSpace Accelerator (inference engine): ${BUILD_ATOMSPACE_ACCELERATOR}")
MESSAGE(STATUS "Agentic Chatbots: ${BUILD_AGENTIC_CHATBOTS}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Tensor Logic Components:")
MESSAGE(STATUS "ATen Tensor Library: ${BUILD_ATEN}")
MESSAGE(STATUS "ATenSpace (AtomSpace + Tensors): ${BUILD_ATENSPACE}")
MESSAGE(STATUS "Tensor Logic Integration: ${BUILD_TENSOR_LOGIC}")
MESSAGE(STATUS "")
MESSAGE(STATUS "Extended Components:")
MESSAGE(STATUS "AtomSpace Storage: ${BUILD_ATOMSPACE_STORAGE}")
MESSAGE(STATUS "AtomSpace Extensions: ${BUILD_ATOMSPACE_EXTENSIONS}")
MESSAGE(STATUS "")
MESSAGE(STATUS "AGI-OS Integration:")
MESSAGE(STATUS "Integration Layer: ${BUILD_INTEGRATION_LAYER}")
MESSAGE(STATUS "AGI-OS Integration Components: ${BUILD_AGI_OS_INTEGRATION}")
MESSAGE(STATUS "")
MESSAGE(STATUS "External Integration Packages:")
MESSAGE(STATUS "Gnucash (cognitive accounting): ${BUILD_GNUCASH}")
MESSAGE(STATUS "KoboldCpp (story/world modeling): ${BUILD_KOBOLDCPP}")
MESSAGE(STATUS "Aphrodite Engine (LLM inference): ${BUILD_APHRODITE}")
MESSAGE(STATUS "")
MESSAGE(STATUS "========================================")
MESSAGE(STATUS "Cognitive Synergy Architecture:")
MESSAGE(STATUS "")
MESSAGE(STATUS "  ┌─────────────────────────────────────┐")
MESSAGE(STATUS "  │ Layer 3: OCC (OpenCog Collection)  │")
MESSAGE(STATUS "  │ AtomSpace, PLN, ECAN, URE, Learn   │")
MESSAGE(STATUS "  ├─────────────────────────────────────┤")
MESSAGE(STATUS "  │ Integration Bridges                 │")
MESSAGE(STATUS "  │ AtomSpace↔MachSpace, ECAN↔Scheduler│")
MESSAGE(STATUS "  ├─────────────────────────────────────┤")
MESSAGE(STATUS "  │ Layer 2: HurdCog (Cognitive OS)    │")
MESSAGE(STATUS "  │ MachSpace, CogKernel, Translators  │")
MESSAGE(STATUS "  ├─────────────────────────────────────┤")
MESSAGE(STATUS "  │ Layer 1: Cognumach (Cognitive μK)  │")
MESSAGE(STATUS "  │ Enhanced Mach, Cognitive Scheduler │")
MESSAGE(STATUS "  └─────────────────────────────────────┘")
MESSAGE(STATUS "")
MESSAGE(STATUS "For detailed build instructions, see:")
MESSAGE(STATUS "  - BUILD_DEPENDENCY_ORDER.md")
MESSAGE(STATUS "  - INTEGRATION_ANALYSIS.md")
MESSAGE(STATUS "  - opencog-debian/BUILD_ORDER_ENHANCED.md")
MESSAGE(STATUS "")