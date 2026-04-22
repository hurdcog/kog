-- Network-Aware DAS: enabled
-- ESN Reservoir Computing: enabled
-- Tensor-Enhanced AtomSpace: enabled
-- ========================================
-- Building AtomSpace Storage (CRITICAL: required by CogServer)...
-- Build type: Release
-- CogUtil version 2.1.0 found.
-- Guile (3.0.9 >= 2.2.2) was found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter Development Development.Module Development.Embed 
-- Python 3.12.3 interpreter found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Could NOT find Cython (missing: CYTHON_EXECUTABLE) (Required is at least version "0.24.0")
-- Cython ( >= 0.24.0) was not found. Make sure CYTHON_EXECUTABLE is set.
-- Cython executable not found.
-- nosetests not found: needed for python tests
CMake Error at build/atomspace/lib/AtomSpaceConfig.cmake:27 (include):
  include could not find requested file:

    /usr/local/lib/cmake/AtomSpace/AtomSpaceTargets.cmake
Call Stack (most recent call first):
  atomspace-storage/CMakeLists.txt:114 (FIND_PACKAGE)


-- AtomSpace found.
CMake Error at atomspace/cmake/OpenCogGuile.cmake:356 (ADD_CUSTOM_TARGET):
  ADD_CUSTOM_TARGET cannot create target "SCM_CONFIG" because another target
  with the same name already exists.  The existing target is a custom target
  created in source directory "/home/azureuser/occ/atomspace/opencog".  See
  documentation for policy CMP0002 for more details.
Call Stack (most recent call first):
  atomspace-storage/opencog/CMakeLists.txt:5 (DECLARE_GUILE_CONFIG_TARGET)


-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   AtomSpace Storage  - AtomSpace persistence backend.
   Unit tests         - Unit tests.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen            - Code documentation.

-- Build type: Release
-- CogUtil version 2.1.0 found.
CMake Error at atomspace-rocks/CMakeLists.txt:77 (include):
  include could not find requested file:

    OpenCogGccOptions


CMake Error at atomspace-rocks/CMakeLists.txt:78 (include):
  include could not find requested file:

    OpenCogLibOptions


CMake Error at atomspace-rocks/CMakeLists.txt:79 (include):
  include could not find requested file:

    OpenCogInstallOptions


CMake Error at atomspace-rocks/CMakeLists.txt:80 (include):
  include could not find requested file:

    Summary


CMake Error at build/atomspace/lib/AtomSpaceConfig.cmake:27 (include):
  include could not find requested file:

    /usr/local/lib/cmake/AtomSpace/AtomSpaceTargets.cmake
Call Stack (most recent call first):
  atomspace-rocks/CMakeLists.txt:86 (FIND_PACKAGE)


-- AtomSpace found.
CMake Error at atomspace-rocks/CMakeLists.txt:95 (FIND_PACKAGE):
  Could not find a package configuration file provided by "AtomSpaceStorage"
  (requested version 4.1) with any of the following names:

    AtomSpaceStorageConfig.cmake
    atomspacestorage-config.cmake

  Add the installation prefix of "AtomSpaceStorage" to CMAKE_PREFIX_PATH or
  set "AtomSpaceStorage_DIR" to a directory containing one of the above
  files.  If "AtomSpaceStorage" provides a separate development package or
  SDK, be sure it has been installed.


-- Configuring incomplete, errors occurred!
azureuser@friendbot:~/occ/build$  cd /home/azureuser/occ && rm -rf build && mkdir build && cd build && cmake .. -DBUILD_ATOMSPACE_STORAGE=OFF -DBUILD_COGSERVER=OFF && cmake --build . -j"$(nproc)"
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- AGI-OS Complete Stack build type: Release
-- Three-layer architecture: Cognumach → HurdCog → OCC
-- Building Layer 3: OpenCog Collection (OCC)...
-- Building CogUtil...
-- Build type: Release
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Looking for C++ include cxxabi.h
-- Looking for C++ include cxxabi.h - found
-- Looking for execinfo.h
-- Looking for execinfo.h - found
-- Found GNU execinfo.h C++ backtrace functionality
-- Looking for include file bfd.h
-- Looking for include file bfd.h - found
-- Found libbfd: /usr/lib/x86_64-linux-gnu/libbfd.so
-- Binutils found.
-- Looking for include file libiberty.h
-- Looking for include file libiberty.h - found
-- Found libiberty: /usr/lib/x86_64-linux-gnu/libiberty.a
-- Libiberty found.
-- Looking for C++ include parallel/algorithm
-- Looking for C++ include parallel/algorithm - found
-- C++ library standardizes parallelism
-- Looking for bfd_get_section_flags
-- Looking for bfd_get_section_flags - not found
-- Looking for bfd_section_flags
-- Looking for bfd_section_flags - found
-- Looking for bfd_get_section_vma
-- Looking for bfd_get_section_vma - not found
-- Looking for bfd_section_vma
-- Looking for bfd_section_vma - found
-- Performing Test HAVE_1_ARG_BFD_SECTION_SIZE
-- Performing Test HAVE_1_ARG_BFD_SECTION_SIZE - Success
-- Performing Test HAVE_1_ARG_BFD_SECTION_VMA
-- Performing Test HAVE_1_ARG_BFD_SECTION_VMA - Success
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   StackPrint  - Pretty printing of stack traces.
   Unit tests  - Unit tests.
   Util        - General utility library.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen     - Code documentation.

-- Building CogGML Microkernel...
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE  
-- CogGML Microkernel configuration complete
-- Building AtomSpace...
-- Build type: Release
-- CogUtil version 2.1.0 found.
-- CxxTest found.
-- Could NOT find Folly (missing: FOLLY_LIBRARIES FOLLY_INCLUDE_DIR) 
-- Folly missing: provides more efficient std::set replacement.
-- Could NOT find SparseHash (missing: SPARSEHASH_INCLUDE_DIR) 
-- SparseHash missing: provides more efficient std::set replacement.
-- Guile (3.0.9 >= 2.2.2) was found.
-- Looking for secure_getenv
-- Looking for secure_getenv - not found
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter Development Development.Module Development.Embed 
-- Python 3.12.3 interpreter found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Could NOT find Cython (missing: CYTHON_EXECUTABLE) (Required is at least version "0.24.0")
-- Cython ( >= 0.24.0) was not found. Make sure CYTHON_EXECUTABLE is set.
-- Cython executable not found.
-- nosetests not found: needed for python tests
-- Could NOT find OCaml. Please specify CMAKE_OCaml_EXECUTABLE. (missing: CMAKE_OCaml_VERSION CMAKE_OCaml_EXECUTABLE CMAKE_OCaml_FIND CMAKE_OCaml_LEX CMAKE_OCaml_YACC) 
-- Stack was not found. Haskell bindings will not be built.
-- Valgrind Prefix: 
-- Could NOT find VALGRIND (missing: VALGRIND_INCLUDE_DIR VALGRIND_PROGRAM) 
-- VALGRIND missing: needed for thread debugging.
-- Compile time defs are: 
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   Scheme bindings   - Scheme (guile) bindings.
   Unit tests        - Unit tests.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen           - Code documentation.
   Haskell bindings  - Haskell bindings.
   OCaml bindings    - OCaML bindings.
   Python bindings   - Python (cython) bindings.
   Python tests      - Python bindings nose tests.
   SparseHash        - Replacement for std::set.

-- Building AtomSpace Accelerator Inference Engine...
-- AtomSpace Accelerator Inference Engine configuration complete
-- Building ATen Tensor Library...
-- Building ATenSpace (AtomSpace + Tensor Embeddings)...
-- Building Tensor Logic Integration Layer...
--   Features: Multi-Entity AtomSpace, Multi-Scale Analysis,
--             Network-Aware DAS, ESN Reservoir Computing
-- 
-- Tensor Logic Configuration:
-- ========================================
-- Multi-Entity AtomSpace: enabled
-- Multi-Scale Analysis: enabled
-- Network-Aware DAS: enabled
-- ESN Reservoir Computing: enabled
-- Tensor-Enhanced AtomSpace: enabled
-- ========================================
-- Building Matrix...
-- Build type: Release
-- CogUtil version 2.1.0 found.
CMake Error at matrix/CMakeLists.txt:71 (include):
  include could not find requested file:

    OpenCogGccOptions


CMake Error at matrix/CMakeLists.txt:72 (include):
  include could not find requested file:

    OpenCogLibOptions


CMake Error at matrix/CMakeLists.txt:73 (include):
  include could not find requested file:

    OpenCogInstallOptions


CMake Error at matrix/CMakeLists.txt:74 (include):
  include could not find requested file:

    Summary


CMake Error at build/atomspace/lib/AtomSpaceConfig.cmake:27 (include):
  include could not find requested file:

    /usr/local/lib/cmake/AtomSpace/AtomSpaceTargets.cmake
Call Stack (most recent call first):
  matrix/CMakeLists.txt:80 (FIND_PACKAGE)


-- AtomSpace found.
CMake Warning at matrix/CMakeLists.txt:92 (FIND_PACKAGE):
  By not providing "FindCxxtest.cmake" in CMAKE_MODULE_PATH this project has
  asked CMake to find a package configuration file provided by "Cxxtest", but
  CMake did not find one.

  Could not find a package configuration file provided by "Cxxtest" with any
  of the following names:

    CxxtestConfig.cmake
    cxxtest-config.cmake

  Add the installation prefix of "Cxxtest" to CMAKE_PREFIX_PATH or set
  "Cxxtest_DIR" to a directory containing one of the above files.  If
  "Cxxtest" provides a separate development package or SDK, be sure it has
  been installed.


-- CxxTest missing: needed for unit tests.
CMake Warning at matrix/CMakeLists.txt:99 (FIND_PACKAGE):
  By not providing "FindAtomSpacePgres.cmake" in CMAKE_MODULE_PATH this
  project has asked CMake to find a package configuration file provided by
  "AtomSpacePgres", but CMake did not find one.

  Could not find a package configuration file provided by "AtomSpacePgres"
  with any of the following names:

    AtomSpacePgresConfig.cmake
    atomspacepgres-config.cmake

  Add the installation prefix of "AtomSpacePgres" to CMAKE_PREFIX_PATH or set
  "AtomSpacePgres_DIR" to a directory containing one of the above files.  If
  "AtomSpacePgres" provides a separate development package or SDK, be sure it
  has been installed.


-- AtomSpace Postgres missing: needed for unit tests.
CMake Error at matrix/CMakeLists.txt:111 (include):
  include could not find requested file:

    OpenCogFindGuile


CMake Error at matrix/CMakeLists.txt:120 (INCLUDE):
  INCLUDE could not find requested file:

    OpenCogMacros


CMake Error at matrix/CMakeLists.txt:121 (INCLUDE):
  INCLUDE could not find requested file:

    OpenCogGuile


-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
CMake Error at cogutil/cmake/Summary.cmake:9 (if):
  if given arguments:

    "6" "GREATER"

  Unknown arguments specified
Call Stack (most recent call first):
  matrix/CMakeLists.txt:242 (SUMMARY_ADD)


-- Configuring incomplete, errors occurred!
azureuser@friendbot:~/occ/build$  cd /home/azureuser/occ && rm -rf build && mkdir build && cd build && cmake .. -DBUILD_ATOMSPACE_STORAGE=OFF -DBUILD_COGSERVER=OFF -DBUILD_MATRIX=OFF -DBUILD_LEARN=OFF -DBUILD_AGENTS=OFF -DBUILD_SENSORY=OFF -DBUILD_COGSELF=OFF -DBUILD_AGENTIC_CHATBOTS=OFF -DBUILD_ENTELECHY=OFF -DBUILD_INTEGRATION_LAYER=OFF && cmake --build . -j"$(nproc)"
-- The C compiler identification is GNU 13.3.0
-- The CXX compiler identification is GNU 13.3.0
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: /usr/bin/cc - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: /usr/bin/c++ - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- AGI-OS Complete Stack build type: Release
-- Three-layer architecture: Cognumach → HurdCog → OCC
-- Building Layer 3: OpenCog Collection (OCC)...
-- Building CogUtil...
-- Build type: Release
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Looking for C++ include cxxabi.h
-- Looking for C++ include cxxabi.h - found
-- Looking for execinfo.h
-- Looking for execinfo.h - found
-- Found GNU execinfo.h C++ backtrace functionality
-- Looking for include file bfd.h
-- Looking for include file bfd.h - found
-- Found libbfd: /usr/lib/x86_64-linux-gnu/libbfd.so
-- Binutils found.
-- Looking for include file libiberty.h
-- Looking for include file libiberty.h - found
-- Found libiberty: /usr/lib/x86_64-linux-gnu/libiberty.a
-- Libiberty found.
-- Looking for C++ include parallel/algorithm
-- Looking for C++ include parallel/algorithm - found
-- C++ library standardizes parallelism
-- Looking for bfd_get_section_flags
-- Looking for bfd_get_section_flags - not found
-- Looking for bfd_section_flags
-- Looking for bfd_section_flags - found
-- Looking for bfd_get_section_vma
-- Looking for bfd_get_section_vma - not found
-- Looking for bfd_section_vma
-- Looking for bfd_section_vma - found
-- Performing Test HAVE_1_ARG_BFD_SECTION_SIZE
-- Performing Test HAVE_1_ARG_BFD_SECTION_SIZE - Success
-- Performing Test HAVE_1_ARG_BFD_SECTION_VMA
-- Performing Test HAVE_1_ARG_BFD_SECTION_VMA - Success
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   StackPrint  - Pretty printing of stack traces.
   Unit tests  - Unit tests.
   Util        - General utility library.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen     - Code documentation.

-- Building CogGML Microkernel...
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD
-- Performing Test CMAKE_HAVE_LIBC_PTHREAD - Success
-- Found Threads: TRUE  
-- CogGML Microkernel configuration complete
-- Building AtomSpace...
-- Build type: Release
-- CogUtil version 2.1.0 found.
-- CxxTest found.
-- Could NOT find Folly (missing: FOLLY_LIBRARIES FOLLY_INCLUDE_DIR) 
-- Folly missing: provides more efficient std::set replacement.
-- Could NOT find SparseHash (missing: SPARSEHASH_INCLUDE_DIR) 
-- SparseHash missing: provides more efficient std::set replacement.
-- Guile (3.0.9 >= 2.2.2) was found.
-- Looking for secure_getenv
-- Looking for secure_getenv - not found
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter Development Development.Module Development.Embed 
-- Python 3.12.3 interpreter found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Could NOT find Cython (missing: CYTHON_EXECUTABLE) (Required is at least version "0.24.0")
-- Cython ( >= 0.24.0) was not found. Make sure CYTHON_EXECUTABLE is set.
-- Cython executable not found.
-- nosetests not found: needed for python tests
-- Could NOT find OCaml. Please specify CMAKE_OCaml_EXECUTABLE. (missing: CMAKE_OCaml_VERSION CMAKE_OCaml_EXECUTABLE CMAKE_OCaml_FIND CMAKE_OCaml_LEX CMAKE_OCaml_YACC) 
-- Stack was not found. Haskell bindings will not be built.
-- Valgrind Prefix: 
-- Could NOT find VALGRIND (missing: VALGRIND_INCLUDE_DIR VALGRIND_PROGRAM) 
-- VALGRIND missing: needed for thread debugging.
-- Compile time defs are: 
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   Scheme bindings   - Scheme (guile) bindings.
   Unit tests        - Unit tests.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen           - Code documentation.
   Haskell bindings  - Haskell bindings.
   OCaml bindings    - OCaML bindings.
   Python bindings   - Python (cython) bindings.
   Python tests      - Python bindings nose tests.
   SparseHash        - Replacement for std::set.

-- Building AtomSpace Accelerator Inference Engine...
-- AtomSpace Accelerator Inference Engine configuration complete
-- Building ATen Tensor Library...
-- Building ATenSpace (AtomSpace + Tensor Embeddings)...
-- Building Tensor Logic Integration Layer...
--   Features: Multi-Entity AtomSpace, Multi-Scale Analysis,
--             Network-Aware DAS, ESN Reservoir Computing
-- 
-- Tensor Logic Configuration:
-- ========================================
-- Multi-Entity AtomSpace: enabled
-- Multi-Scale Analysis: enabled
-- Network-Aware DAS: enabled
-- ESN Reservoir Computing: enabled
-- Tensor-Enhanced AtomSpace: enabled
-- ========================================
-- 
-- AGI-OS Configuration Summary:
-- ========================================
-- Build type: Release
-- 
-- AGI-OS Stack:
-- Layer 1 - Cognumach (Microkernel): OFF
-- Layer 2 - HurdCog (OS + Cognitive): OFF
-- Layer 3 - OCC (AGI Framework): enabled
-- 
-- Core Components:
-- CogUtil: ON
-- AtomSpace: ON
-- CogServer: OFF
-- Matrix: OFF
-- Learn: OFF
-- Agents: OFF
-- Sensory: OFF
-- 
-- Cognitive Architecture Components:
-- CogGML Microkernel (self-aware shards): ON
-- CogSelf AGI Framework: OFF
-- Ontogenetic Entelechy (Civic Angel / AFI): OFF
-- AtomSpace Accelerator (inference engine): ON
-- Agentic Chatbots: OFF
-- 
-- Tensor Logic Components:
-- ATen Tensor Library: ON
-- ATenSpace (AtomSpace + Tensors): ON
-- Tensor Logic Integration: ON
-- 
-- Extended Components:
-- AtomSpace Storage: OFF
-- AtomSpace Extensions: OFF
-- 
-- AGI-OS Integration:
-- Integration Layer: OFF
-- AGI-OS Integration Components: ON
-- 
-- External Integration Packages:
-- Gnucash (cognitive accounting): OFF
-- KoboldCpp (story/world modeling): OFF
-- Aphrodite Engine (LLM inference): OFF
-- 
-- ========================================
-- Cognitive Synergy Architecture:
-- 
--   ┌─────────────────────────────────────┐
--   │ Layer 3: OCC (OpenCog Collection)  │
--   │ AtomSpace, PLN, ECAN, URE, Learn   │
--   ├─────────────────────────────────────┤
--   │ Integration Bridges                 │
--   │ AtomSpace↔MachSpace, ECAN↔Scheduler│
--   ├─────────────────────────────────────┤
--   │ Layer 2: HurdCog (Cognitive OS)    │
--   │ MachSpace, CogKernel, Translators  │
--   ├─────────────────────────────────────┤
--   │ Layer 1: Cognumach (Cognitive μK)  │
--   │ Enhanced Mach, Cognitive Scheduler │
--   └─────────────────────────────────────┘
-- 
-- For detailed build instructions, see:
--   - BUILD_DEPENDENCY_ORDER.md
--   - INTEGRATION_ANALYSIS.md
--   - opencog-debian/BUILD_ORDER_ENHANCED.md
-- 
-- Configuring done (3.3s)
-- Generating done (0.8s)
-- Build files have been written to: /home/azureuser/occ/build
[  3%] Building CXX object coggml/CMakeFiles/coggml.dir/src/microkernel.cpp.o
[  6%] Building C object cogutil/opencog/util/CMakeFiles/cogutil.dir/backtrace-symbols.c.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Config.cc.o
[  6%] Building CXX object coggml/CMakeFiles/coggml.dir/src/cognitive_shard.cpp.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/exceptions.cc.o
[  6%] Building CXX object coggml/CMakeFiles/coggml.dir/src/self_awareness.cpp.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_selector.cc.o
[  6%] Building CXX object coggml/CMakeFiles/coggml.dir/src/shard_coordinator.cpp.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_random_selector.cc.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Logger.cc.o
[  6%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/misc.cc.o
[  6%] Building CXX object coggml/CMakeFiles/coggml.dir/src/shard_message.cpp.o
[  9%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/mt19937ar.cc.o
[  9%] Building CXX object coggml/CMakeFiles/coggml.dir/src/distributed_coordinator.cpp.o
[  9%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_assert.cc.o
[  9%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_omp.cc.o
[  9%] Linking CXX shared library libcoggml.so
[  9%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/platform.cc.o
[  9%] Built target coggml
[  9%] Built target SCM_CONFIG
[ 12%] Generating C++ Atom Type bindings from atom_types.script
[ 12%] Linking CXX shared library libcogutil.so
[ 12%] Generating Scheme Atom Type bindings from atom_types.script
[ 12%] Generating Python Atom Type bindings from atom_types.script
[ 12%] Built target opencog_atom_types
[ 12%] Building CXX object atomspace-accelerator/CMakeFiles/atomspace_accelerator.dir/src/inference_engine.cpp.o
[ 12%] Building CXX object atomspace-accelerator/CMakeFiles/atomspace_accelerator.dir/src/accelerator.cpp.o
[ 12%] Building CXX object atomspace-accelerator/CMakeFiles/atomspace_accelerator.dir/src/query_optimizer.cpp.o
[ 12%] Built target cogutil
[ 12%] Building CXX object atomspace-accelerator/CMakeFiles/atomspace_accelerator.dir/src/pattern_miner.cpp.o
[ 12%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/TensorLogic.cpp.o
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp: In member function ‘void opencog::tensor_logic::TensorLogic::recordFlow(const FlowTensor&)’:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:340:34: error: cannot convert ‘const opencog::tensor_logic::TensorLogic::FlowTensor’ to ‘const opencog::tensor_logic::NetworkAwareDAS::FlowRecord&’
  340 |         network_das_->recordFlow(flow);
      |                                  ^~~~
      |                                  |
      |                                  const opencog::tensor_logic::TensorLogic::FlowTensor
In file included from /home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:10:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/NetworkAwareDAS.h:41:39: note:   initializing argument 1 of ‘void opencog::tensor_logic::NetworkAwareDAS::recordFlow(const FlowRecord&)’
   41 |     void recordFlow(const FlowRecord& flow);
      |                     ~~~~~~~~~~~~~~~~~~^~~~
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp: In member function ‘std::vector<float> opencog::tensor_logic::TensorLogic::computeTensorTruthValue(const std::vector<float>&)’:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:536:32: error: no matching function for call to ‘min(float, __gnu_cxx::__enable_if<true, double>::__type)’
  536 |     float confidence = std::min(1.0f, magnitude / std::sqrt(tensor.size()));
      |                        ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/13/bits/stl_uninitialized.h:63,
                 from /usr/include/c++/13/memory:69,
                 from /home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.h:18,
                 from /home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:7:
/usr/include/c++/13/bits/stl_algobase.h:233:5: note: candidate: ‘template<class _Tp> constexpr const _Tp& std::min(const _Tp&, const _Tp&)’
  233 |     min(const _Tp& __a, const _Tp& __b)
      |     ^~~
/usr/include/c++/13/bits/stl_algobase.h:233:5: note:   template argument deduction/substitution failed:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:536:32: note:   deduced conflicting types for parameter ‘const _Tp’ (‘float’ and ‘__gnu_cxx::__enable_if<true, double>::__type’ {aka ‘double’})
  536 |     float confidence = std::min(1.0f, magnitude / std::sqrt(tensor.size()));
      |                        ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/bits/stl_algobase.h:281:5: note: candidate: ‘template<class _Tp, class _Compare> constexpr const _Tp& std::min(const _Tp&, const _Tp&, _Compare)’
  281 |     min(const _Tp& __a, const _Tp& __b, _Compare __comp)
      |     ^~~
/usr/include/c++/13/bits/stl_algobase.h:281:5: note:   template argument deduction/substitution failed:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:536:32: note:   deduced conflicting types for parameter ‘const _Tp’ (‘float’ and ‘__gnu_cxx::__enable_if<true, double>::__type’ {aka ‘double’})
  536 |     float confidence = std::min(1.0f, magnitude / std::sqrt(tensor.size()));
      |                        ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
In file included from /usr/include/c++/13/functional:67,
                 from /home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.h:22:
/usr/include/c++/13/bits/stl_algo.h:5775:5: note: candidate: ‘template<class _Tp> constexpr _Tp std::min(initializer_list<_Tp>)’
 5775 |     min(initializer_list<_Tp> __l)
      |     ^~~
/usr/include/c++/13/bits/stl_algo.h:5775:5: note:   template argument deduction/substitution failed:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:536:32: note:   mismatched types ‘std::initializer_list<_Tp>’ and ‘float’
  536 |     float confidence = std::min(1.0f, magnitude / std::sqrt(tensor.size()));
      |                        ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/usr/include/c++/13/bits/stl_algo.h:5785:5: note: candidate: ‘template<class _Tp, class _Compare> constexpr _Tp std::min(initializer_list<_Tp>, _Compare)’
 5785 |     min(initializer_list<_Tp> __l, _Compare __comp)
      |     ^~~
/usr/include/c++/13/bits/stl_algo.h:5785:5: note:   template argument deduction/substitution failed:
/home/azureuser/occ/tensor-logic/opencog/tensor-logic/TensorLogic.cpp:536:32: note:   mismatched types ‘std::initializer_list<_Tp>’ and ‘float’
  536 |     float confidence = std::min(1.0f, magnitude / std::sqrt(tensor.size()));
      |                        ~~~~~~~~^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
gmake[2]: *** [tensor-logic/CMakeFiles/tensor-logic.dir/build.make:76: tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/TensorLogic.cpp.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:9528: tensor-logic/CMakeFiles/tensor-logic.dir/all] Error 2
gmake[1]: *** Waiting for unfinished jobs....
[ 12%] Linking CXX shared library libatomspace_accelerator.so
[ 12%] Built target atomspace_accelerator
gmake: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/occ/build$  cd /home/azureuser/occ/build && cmake --build . -j"$(nproc)"
[  3%] Built target coggml
[  9%] Built target cogutil
[  9%] Built target SCM_CONFIG
[ 12%] Built target opencog_atom_types
[ 12%] Built target atomspace_accelerator
[ 12%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/TensorLogic.cpp.o
[ 12%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o
In file included from /home/azureuser/occ/build/atomspace/opencog/atoms/atom_types/atom_types.definitions:2,
                 from /home/azureuser/occ/atomspace/opencog/atoms/atom_types/atom_types_init.cc:23:
/home/azureuser/occ/atomspace/opencog/atoms/atom_types/NameServer.h:33:10: fatal error: opencog/util/sigslot.h: No such file or directory
   33 | #include <opencog/util/sigslot.h>
      |          ^~~~~~~~~~~~~~~~~~~~~~~~
compilation terminated.
gmake[2]: *** [atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/build.make:76: atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:2320: atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/all] Error 2
gmake[1]: *** Waiting for unfinished jobs....
[ 12%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/MultiEntityAtomSpace.cpp.o
[ 12%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/MultiScaleAnalysis.cpp.o
[ 15%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/NetworkAwareDAS.cpp.o
[ 15%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/ESNReservoir.cpp.o
[ 15%] Building CXX object tensor-logic/CMakeFiles/tensor-logic.dir/opencog/tensor-logic/TensorAtomSpace.cpp.o
[ 15%] Linking CXX shared library libtensor-logic.so
[ 15%] Built target tensor-logic
gmake: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/occ/build$  cd /home/azureuser/occ/build && cmake --build . -j"$(nproc)"
-- AGI-OS Complete Stack build type: Release
-- Three-layer architecture: Cognumach → HurdCog → OCC
-- Building Layer 3: OpenCog Collection (OCC)...
-- Building CogUtil...
-- Build type: Release
-- Found GNU execinfo.h C++ backtrace functionality
-- Found libbfd: /usr/lib/x86_64-linux-gnu/libbfd.so
-- Binutils found.
-- Found libiberty: /usr/lib/x86_64-linux-gnu/libiberty.a
-- Libiberty found.
-- C++ library standardizes parallelism
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   StackPrint  - Pretty printing of stack traces.
   Unit tests  - Unit tests.
   Util        - General utility library.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen     - Code documentation.

-- Building CogGML Microkernel...
-- CogGML Microkernel configuration complete
-- Building AtomSpace...
-- Build type: Release
-- CogUtil being built in-tree as subdirectory.
-- CxxTest found.
-- Could NOT find Folly (missing: FOLLY_LIBRARIES FOLLY_INCLUDE_DIR) 
-- Folly missing: provides more efficient std::set replacement.
-- Could NOT find SparseHash (missing: SPARSEHASH_INCLUDE_DIR) 
-- SparseHash missing: provides more efficient std::set replacement.
-- Guile (3.0.9 >= 2.2.2) was found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter Development Development.Module Development.Embed 
-- Python 3.12.3 interpreter found.
-- Found Python3: /usr/bin/python3 (found version "3.12.3") found components: Interpreter 
-- Could NOT find Cython (missing: CYTHON_EXECUTABLE) (Required is at least version "0.24.0")
-- Cython ( >= 0.24.0) was not found. Make sure CYTHON_EXECUTABLE is set.
-- Cython executable not found.
-- nosetests not found: needed for python tests
-- Could NOT find OCaml. Please specify CMAKE_OCaml_EXECUTABLE. (missing: CMAKE_OCaml_VERSION CMAKE_OCaml_EXECUTABLE CMAKE_OCaml_FIND CMAKE_OCaml_LEX CMAKE_OCaml_YACC) 
-- Stack was not found. Haskell bindings will not be built.
-- Valgrind Prefix: 
-- Could NOT find VALGRIND (missing: VALGRIND_INCLUDE_DIR VALGRIND_PROGRAM) 
-- VALGRIND missing: needed for thread debugging.
-- Compile time defs are: 
-- Could NOT find Doxygen (missing: DOXYGEN_EXECUTABLE) 
-- Doxygen not found, you won't be able to generate API documentation.

Building for Ubuntu 24.04.3 LTS

The following components will be built:
-----------------------------------------------
   Scheme bindings   - Scheme (guile) bindings.
   Unit tests        - Unit tests.

The following components WILL NOT be built:
-----------------------------------------------
   Doxygen           - Code documentation.
   Haskell bindings  - Haskell bindings.
   OCaml bindings    - OCaML bindings.
   Python bindings   - Python (cython) bindings.
   Python tests      - Python bindings nose tests.
   SparseHash        - Replacement for std::set.

-- Building AtomSpace Accelerator Inference Engine...
-- AtomSpace Accelerator Inference Engine configuration complete
-- Building ATen Tensor Library...
-- Building ATenSpace (AtomSpace + Tensor Embeddings)...
-- Building Tensor Logic Integration Layer...
--   Features: Multi-Entity AtomSpace, Multi-Scale Analysis,
--             Network-Aware DAS, ESN Reservoir Computing
-- 
-- Tensor Logic Configuration:
-- ========================================
-- Multi-Entity AtomSpace: enabled
-- Multi-Scale Analysis: enabled
-- Network-Aware DAS: enabled
-- ESN Reservoir Computing: enabled
-- Tensor-Enhanced AtomSpace: enabled
-- ========================================
-- 
-- AGI-OS Configuration Summary:
-- ========================================
-- Build type: Release
-- 
-- AGI-OS Stack:
-- Layer 1 - Cognumach (Microkernel): OFF
-- Layer 2 - HurdCog (OS + Cognitive): OFF
-- Layer 3 - OCC (AGI Framework): enabled
-- 
-- Core Components:
-- CogUtil: ON
-- AtomSpace: ON
-- CogServer: OFF
-- Matrix: OFF
-- Learn: OFF
-- Agents: OFF
-- Sensory: OFF
-- 
-- Cognitive Architecture Components:
-- CogGML Microkernel (self-aware shards): ON
-- CogSelf AGI Framework: OFF
-- Ontogenetic Entelechy (Civic Angel / AFI): OFF
-- AtomSpace Accelerator (inference engine): ON
-- Agentic Chatbots: OFF
-- 
-- Tensor Logic Components:
-- ATen Tensor Library: ON
-- ATenSpace (AtomSpace + Tensors): ON
-- Tensor Logic Integration: ON
-- 
-- Extended Components:
-- AtomSpace Storage: OFF
-- AtomSpace Extensions: OFF
-- 
-- AGI-OS Integration:
-- Integration Layer: OFF
-- AGI-OS Integration Components: ON
-- 
-- External Integration Packages:
-- Gnucash (cognitive accounting): OFF
-- KoboldCpp (story/world modeling): OFF
-- Aphrodite Engine (LLM inference): OFF
-- 
-- ========================================
-- Cognitive Synergy Architecture:
-- 
--   ┌─────────────────────────────────────┐
--   │ Layer 3: OCC (OpenCog Collection)  │
--   │ AtomSpace, PLN, ECAN, URE, Learn   │
--   ├─────────────────────────────────────┤
--   │ Integration Bridges                 │
--   │ AtomSpace↔MachSpace, ECAN↔Scheduler│
--   ├─────────────────────────────────────┤
--   │ Layer 2: HurdCog (Cognitive OS)    │
--   │ MachSpace, CogKernel, Translators  │
--   ├─────────────────────────────────────┤
--   │ Layer 1: Cognumach (Cognitive μK)  │
--   │ Enhanced Mach, Cognitive Scheduler │
--   └─────────────────────────────────────┘
-- 
-- For detailed build instructions, see:
--   - BUILD_DEPENDENCY_ORDER.md
--   - INTEGRATION_ANALYSIS.md
--   - opencog-debian/BUILD_ORDER_ENHANCED.md
-- 
-- Configuring done (0.7s)
-- Generating done (0.7s)
-- Build files have been written to: /home/azureuser/occ/build
[  6%] Built target cogutil
[  9%] Built target coggml
[  9%] Built target SCM_CONFIG
[ 12%] Built target opencog_atom_types
[ 12%] Built target atomspace_accelerator
[ 15%] Built target tensor-logic
[ 15%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o
[ 15%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 15%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/NameServer.cc.o
[ 15%] Linking CXX shared library libatom_types.so
[ 15%] Built target atom_types
[ 15%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/Value.cc.o
[ 15%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/BoolValue.cc.o
[ 15%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ContainerValue.cc.o
[ 15%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FloatValue.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FormulaStream.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FutureStream.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/LinkValue.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/QueueValue.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/RandomStream.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/SectionValue.cc.o
[ 18%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/StringValue.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/UnisetValue.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ValueFactory.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/VoidValue.cc.o
[ 21%] Linking CXX shared library libvalue.so
[ 21%] Built target value
[ 21%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/CountTruthValue.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/FormulaTruthValue.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/SimpleTruthValue.cc.o
[ 21%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/TruthValue.cc.o
[ 25%] Linking CXX shared library libtruthvalue.so
[ 25%] Built target truthvalue
[ 25%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/ClassServer.cc.o
[ 25%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Atom.cc.o
[ 25%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Handle.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Link.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Node.cc.o
[ 28%] Linking CXX shared library libatombase.so
[ 28%] Built target atombase
[ 28%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/ForeignAST.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/AbsentLink.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/SexprAST.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Checkers.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/CondLink.cc.o
[ 28%] Linking CXX shared library libforeign.so
[ 28%] Built target foreign
[ 28%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Context.cc.o
[ 28%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedPredicateNode.cc.o
[ 31%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DefineLink.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedSchemaNode.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DeleteLink.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryManager.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FindUtils.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryRunner.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/DLScheme.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeLink.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/SCMRunner.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeVariables.cc.o
[ 34%] Linking CXX shared library libgrounded.so
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FunctionLink.cc.o
[ 34%] Built target grounded
[ 34%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/GrantLink.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/LambdaLink.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/NumberNode.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PrenexLink.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PresentLink.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PutLink.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Quotation.cc.o
[ 37%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RandomChoice.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Replacement.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RewriteLink.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/ScopeLink.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/StateLink.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TimeLink.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypeChoice.cc.o
[ 40%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypedAtomLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypedVariableLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypeIntersectionLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypeNode.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/TypeUtils.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/UniqueLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/UnorderedLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Variables.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/VariableList.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/VariableSet.cc.o
[ 46%] Linking CXX shared library libatomcore.so
[ 46%] Built target atomcore
[ 46%] Building CXX object atomspace/opencog/atoms/columnvec/CMakeFiles/columnvec.dir/FloatColumn.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/CollectionOfLink.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/columnvec/CMakeFiles/columnvec.dir/LinkColumn.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/ConcatenateLink.cc.o
[ 50%] Building CXX object atomspace/opencog/atoms/columnvec/CMakeFiles/columnvec.dir/SexprColumn.cc.o
[ 50%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/FilterLink.cc.o
[ 50%] Building CXX object atomspace/opencog/atoms/columnvec/CMakeFiles/columnvec.dir/TransposeColumn.cc.o
[ 50%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/FormulaPredicateLink.cc.o
[ 50%] Linking CXX shared library libcolumnvec.so
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/IncomingOfLink.cc.o
[ 53%] Built target columnvec
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/IncrementValueLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/DefinedProcedureNode.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/LinkSignatureLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/DontExecLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ExecuteThreadedLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/NumberOfLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/PromiseLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ParallelLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SetTVLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/PureExecLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SetValueLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/SleepLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SizeOfLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ThreadJoinLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SplitLink.cc.o
[ 59%] Linking CXX shared library libparallel.so
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/StreamValueOfLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/StringOfLink.cc.o
[ 59%] Built target parallel
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/TruthValueOfLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/AccumulateLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/TypeOfLink.cc.o
[ 59%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/ValueOfLink.cc.o
[ 62%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/ArithmeticLink.cc.o
[ 62%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/BoolOpLink.cc.o
[ 65%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/ValueShimLink.cc.o
[ 65%] Linking CXX shared library libatomflow.so
[ 65%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/DecimateLink.cc.o
[ 65%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/DivideLink.cc.o
[ 65%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/ElementOfLink.cc.o
[ 65%] Built target atomflow
[ 65%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/FoldLink.cc.o
[ 68%] Building CXX object atomspace/opencog/atoms/join/CMakeFiles/join.dir/JoinLink.cc.o
[ 68%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/ImpulseLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/MaxLink.cc.o
[ 71%] Linking CXX shared library libjoin.so
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/MinLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/MinusLink.cc.o
[ 71%] Built target join
[ 71%] Building CXX object atomspace/opencog/atoms/rule/CMakeFiles/rule.dir/ConclusionOfLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/NumericFunctionLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/rule/CMakeFiles/rule.dir/PremiseOfLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/PlusLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/rule/CMakeFiles/rule.dir/RuleLink.cc.o
[ 71%] Building CXX object atomspace/opencog/atoms/reduct/CMakeFiles/clearbox.dir/TimesLink.cc.o
[ 75%] Building CXX object atomspace/opencog/atoms/rule/CMakeFiles/rule.dir/VardeclOfLink.cc.o
[ 75%] Linking CXX shared library libclearbox.so
[ 75%] Linking CXX shared library librule.so
[ 75%] Built target rule
[ 75%] Built target clearbox
[ 75%] Building CXX object atomspace/opencog/atoms/execution/CMakeFiles/execution.dir/EvaluationLink.cc.o
[ 75%] Building CXX object atomspace/opencog/atoms/execution/CMakeFiles/execution.dir/Force.cc.o
[ 78%] Building CXX object atomspace/opencog/atoms/execution/CMakeFiles/execution.dir/ExecutionOutputLink.cc.o
[ 78%] Building CXX object atomspace/opencog/atoms/execution/CMakeFiles/execution.dir/Instantiator.cc.o
[ 78%] Linking CXX shared library libexecution.so
[ 78%] Built target execution
[ 78%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/InitiateSearchMixin.cc.o
[ 78%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/ContinuationMixin.cc.o
[ 78%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/NextSearchMixin.cc.o
[ 78%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/PatternMatchEngine.cc.o
[ 81%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/Recognizer.cc.o
[ 81%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/RewriteMixin.cc.o
[ 81%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/Satisfier.cc.o
[ 81%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/SatisfyMixin.cc.o
[ 81%] Building CXX object atomspace/opencog/query/CMakeFiles/query-engine.dir/TermMatchMixin.cc.o
[ 81%] Linking CXX shared library libquery-engine.so
[ 81%] Built target query-engine
[ 81%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/BindLink.cc.o
[ 81%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/DualLink.cc.o
[ 81%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/GetLink.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/MeetLink.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternJit.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternLink.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternTerm.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternUtils.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/Pattern.cc.o
[ 84%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/QueryLink.cc.o
[ 87%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/SatisfactionLink.cc.o
[ 87%] Linking CXX shared library libpattern.so
[ 87%] Built target pattern
[ 87%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/AtomSpace.cc.o
[ 87%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/AtomTable.cc.o
[ 87%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/Frame.cc.o
[ 87%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/Transient.cc.o
[ 87%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/TypeIndex.cc.o
[ 90%] Linking CXX shared library libatomspace.so
[ 90%] Built target atomspace
[ 90%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeEval.cc.o
[ 90%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeModule.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemePrimitive.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmob.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobAtom.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobAS.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobGC.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobNew.cc.o
[ 93%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobPrint.cc.o
[ 96%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobTV.cc.o
[ 96%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobValue.cc.o
[ 96%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobLogger.cc.o
[ 96%] Linking CXX shared library libsmob.so
[ 96%] Built target smob
[ 96%] Building CXX object atomspace/opencog/guile/modules/CMakeFiles/exec.dir/ExecSCM.cc.o
[ 96%] Building CXX object atomspace/opencog/guile/modules/CMakeFiles/logger.dir/LoggerSCM.cc.o
[ 96%] Linking CXX shared library libexec.so
[ 96%] Linking CXX shared library liblogger.so
[ 96%] Built target exec
[ 96%] Building CXX object atomspace/opencog/guile/modules/CMakeFiles/randgen.dir/RandGenSCM.cc.o
[ 96%] Built target logger
[ 96%] Building CXX object atomspace/opencog/guile/modules/CMakeFiles/type-utils.dir/TypeUtilsSCM.cc.o
[100%] Linking CXX shared library librandgen.so
[100%] Linking CXX shared library libtype-utils.so
[100%] Built target randgen
[100%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_scm
[100%] Built target type-utils
[100%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_guile_modules
azureuser@friendbot:~/occ/build$ https://github.com/l3utterfly?page=2&tab=repositorieshttps://github.com/l3utterfly?page=2&tab=repositories
