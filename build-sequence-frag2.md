[ 36%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/UniqueLink.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/UnorderedLink.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Variables.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/VariableList.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/VariableSet.cc.o
[ 39%] Linking CXX shared library libatomcore.so
during IPA pass: static-var
lto1: internal compiler error: Bus error
0x1013b0e internal_error(char const*, ...)
        ???:0
0x15de552 lto_obj_append_data(void const*, unsigned long, void*)
        ???:0
0x15dfa2b lto_output()
        ???:0
0x7a757f ipa_write_optimization_summaries(lto_symtab_encoder_d*)
        ???:0
0x157ed11 lto_main()
        ???:0
Please submit a full bug report, with preprocessed source (by using -freport-bug).
Please include the complete backtrace with any bug report.
See <file:///usr/share/doc/gcc-13/README.Bugs> for instructions.
lto-wrapper: fatal error: /usr/bin/c++ returned 1 exit status
compilation terminated.
/usr/bin/ld: error: lto-wrapper failed
collect2: error: ld returned 1 exit status
make[2]: *** [atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/build.make:629: atomspace/opencog/atoms/core/libatomcore.so] Error 1
make[1]: *** [CMakeFiles/Makefile2:4033: atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/all] Error 2
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -n "flto" /home/azureuser/occ/cogutil/cmake/OpenCogGccOptions.cmake 2>/dev/null
32:             # -flto is good for performance, but wow is it slow to link...
34:             SET(CMAKE_C_FLAGS_RELEASE "-O3 -g -flto=auto")
azureuser@friendbot:~/build$  grep -n "flto" /home/azureuser/occ/cogutil/cmake/OpenCogGccOptions.cmake
32:             # -flto is good for performance, but wow is it slow to link...
azureuser@friendbot:~/build$  grep -n "CMAKE_CXX_FLAGS_RELEASE" /home/azureuser/occ/cogutil/cmake/OpenCogGccOptions.cmake 2>/dev/null
63:     SET(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
112:    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -5
--   - opencog-debian/BUILD_ORDER_ENHANCED.md
-- 
-- Configuring done (6.4s)
-- Generating done (4.9s)
-- Build files have been written to: /home/azureuser/build
azureuser@friendbot:~/build$  wc -l /tmp/build_output.txt 2>/dev/null && tail -100 /tmp/build_output.txt
46 /tmp/build_output.txt
[  0%] Built target coggml
[  0%] Building C object cogutil/opencog/util/CMakeFiles/cogutil.dir/backtrace-symbols.c.o
[  0%] Built target SCM_CONFIG
[  0%] Built target opencog_atom_types
[  0%] Built target atomspace_accelerator
[  2%] Built target tensor-logic
[  2%] Built target STORAGE_SCM_CONFIG
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Config.cc.o
[  4%] Built target storage_types
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  4%] Built target COGSERVER_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  4%] Building CXX object cogserver/examples/module/CMakeFiles/example_module.dir/ExampleModule.cc.o
[  4%] Linking CXX shared library libexample_module.so
[  4%] Built target example_module
[  4%] Built target COG_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/exceptions.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_random_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Logger.cc.o
[  7%] Built target persist_cog_atom_types
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/misc.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/mt19937ar.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_assert.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_omp.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/platform.cc.o
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Linking CXX shared library libcogutil.so
[  9%] Built target cogutil
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[ 14%] Built target agentic_chatbots
[ 17%] Built target cogself
[ 17%] Built target afi
[ 17%] Built target cognitive-scheduler
[ 17%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 17%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ConsoleSocket.cc.o
azureuser@friendbot:~/build$  while [ ! -f /tmp/build_done ]; do if ! pgrep -f "make -j" > /dev/null 2>&1; then touch /tmp/build_done; fi; sleep 5; done && tail -120 /tmp/build_output.txt

[  4%] Built target storage_types
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  4%] Built target COGSERVER_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  4%] Building CXX object cogserver/examples/module/CMakeFiles/example_module.dir/ExampleModule.cc.o
[  4%] Linking CXX shared library libexample_module.so
[  4%] Built target example_module
[  4%] Built target COG_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/exceptions.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_random_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Logger.cc.o
[  7%] Built target persist_cog_atom_types
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/misc.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/mt19937ar.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_assert.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_omp.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/platform.cc.o
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Linking CXX shared library libcogutil.so
[  9%] Built target cogutil
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[ 14%] Built target agentic_chatbots
[ 17%] Built target cogself
[ 17%] Built target afi
[ 17%] Built target cognitive-scheduler
[ 17%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 17%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ConsoleSocket.cc.o
[ 24%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/GenericShell.cc.o
[ 29%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/NameServer.cc.o
[ 29%] Linking CXX shared library libatom_types.so
[ 29%] Built target atom_types
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/NetworkServer.cc.o
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ServerSocket.cc.o
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/WebSocket.cc.o
[ 29%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog_persist_cog-types
[ 29%] Building CXX object sensory/opencog/sensory-v0/types/CMakeFiles/sensory-v0-types.dir/sensory_v0_types_init.cc.o
[ 29%] Linking CXX shared library libsensory-v0-types.so
[ 29%] Built target sensory-v0-types
[ 29%] Building CXX object sensory/opencog/sensory/types/CMakeFiles/sensory-types.dir/sensory_types_init.cc.o
[ 29%] Linking CXX shared library libsensory-types.so
[ 29%] Built target sensory-types
[ 31%] Built target entelechy
[ 31%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/Value.cc.o
[ 31%] Linking CXX shared library libnetwork.so
[ 31%] Built target network
[ 31%] Building CXX object atomspace-storage/opencog/persist/storage/CMakeFiles/storage-types.dir/storage_types_init.cc.o
[ 31%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/BoolValue.cc.o
[ 34%] Linking CXX shared library libstorage-types.so
[ 34%] Built target storage-types
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ContainerValue.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FloatValue.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FormulaStream.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FutureStream.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/LinkValue.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/QueueValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/RandomStream.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/SectionValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/StringValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/UnisetValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ValueFactory.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/VoidValue.cc.o
[ 41%] Linking CXX shared library libvalue.so
[ 41%] Built target value
[ 41%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/FormulaTruthValue.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/CountTruthValue.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/SimpleTruthValue.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/TruthValue.cc.o
[ 43%] Linking CXX shared library libtruthvalue.so
[ 43%] Built target truthvalue
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Atom.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/ClassServer.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Handle.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Link.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Node.cc.o
[ 43%] Linking CXX shared library libatombase.so
[ 43%] Built target atombase
[ 43%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/ForeignAST.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/AbsentLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/SexprAST.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Checkers.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/CondLink.cc.o
[ 46%] Linking CXX shared library libforeign.so
[ 46%] Built target foreign
[ 46%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedPredicateNode.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Context.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedSchemaNode.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryManager.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DefineLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryRunner.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DeleteLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/DLScheme.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FindUtils.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/SCMRunner.cc.o
[ 48%] Linking CXX shared library libgrounded.so
[ 48%] Built target grounded
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeVariables.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FunctionLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/GrantLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/LambdaLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/NumberNode.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PrenexLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PresentLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PutLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Quotation.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RandomChoice.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Replacement.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RewriteLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/ScopeLink.cc.o
azureuser@friendbot:~/build$ 
azureuser@friendbot:~/build$  wc -l /tmp/build_output.txt && tail -120 /tmp/build_output.txt
128 /tmp/build_output.txt
[  4%] Built target storage_types
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  4%] Built target COGSERVER_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  4%] Building CXX object cogserver/examples/module/CMakeFiles/example_module.dir/ExampleModule.cc.o
[  4%] Linking CXX shared library libexample_module.so
[  4%] Built target example_module
[  4%] Built target COG_SCM_CONFIG
[  4%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/exceptions.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/lazy_random_selector.cc.o
[  4%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/Logger.cc.o
[  7%] Built target persist_cog_atom_types
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/misc.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/mt19937ar.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_assert.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/oc_omp.cc.o
[  7%] Building CXX object cogutil/opencog/util/CMakeFiles/cogutil.dir/platform.cc.o
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Linking CXX shared library libcogutil.so
[  9%] Built target cogutil
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[ 14%] Built target agentic_chatbots
[ 17%] Built target cogself
[ 17%] Built target afi
[ 17%] Built target cognitive-scheduler
[ 17%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/atom_types_init.cc.o
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 17%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 17%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ConsoleSocket.cc.o
[ 24%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/GenericShell.cc.o
[ 29%] Building CXX object atomspace/opencog/atoms/atom_types/CMakeFiles/atom_types.dir/NameServer.cc.o
[ 29%] Linking CXX shared library libatom_types.so
[ 29%] Built target atom_types
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/NetworkServer.cc.o
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ServerSocket.cc.o
[ 29%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/WebSocket.cc.o
[ 29%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog_persist_cog-types
[ 29%] Building CXX object sensory/opencog/sensory-v0/types/CMakeFiles/sensory-v0-types.dir/sensory_v0_types_init.cc.o
[ 29%] Linking CXX shared library libsensory-v0-types.so
[ 29%] Built target sensory-v0-types
[ 29%] Building CXX object sensory/opencog/sensory/types/CMakeFiles/sensory-types.dir/sensory_types_init.cc.o
[ 29%] Linking CXX shared library libsensory-types.so
[ 29%] Built target sensory-types
[ 31%] Built target entelechy
[ 31%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/Value.cc.o
[ 31%] Linking CXX shared library libnetwork.so
[ 31%] Built target network
[ 31%] Building CXX object atomspace-storage/opencog/persist/storage/CMakeFiles/storage-types.dir/storage_types_init.cc.o
[ 31%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/BoolValue.cc.o
[ 34%] Linking CXX shared library libstorage-types.so
[ 34%] Built target storage-types
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ContainerValue.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FloatValue.cc.o
[ 34%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FormulaStream.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/FutureStream.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/LinkValue.cc.o
[ 36%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/QueueValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/RandomStream.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/SectionValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/StringValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/UnisetValue.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/ValueFactory.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/value/CMakeFiles/value.dir/VoidValue.cc.o
[ 41%] Linking CXX shared library libvalue.so
[ 41%] Built target value
[ 41%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/FormulaTruthValue.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/CountTruthValue.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/SimpleTruthValue.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/truthvalue/CMakeFiles/truthvalue.dir/TruthValue.cc.o
[ 43%] Linking CXX shared library libtruthvalue.so
[ 43%] Built target truthvalue
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Atom.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/ClassServer.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Handle.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Link.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/base/CMakeFiles/atombase.dir/Node.cc.o
[ 43%] Linking CXX shared library libatombase.so
[ 43%] Built target atombase
[ 43%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/ForeignAST.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/AbsentLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/foreign/CMakeFiles/foreign.dir/SexprAST.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Checkers.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/CondLink.cc.o
[ 46%] Linking CXX shared library libforeign.so
[ 46%] Built target foreign
[ 46%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedPredicateNode.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Context.cc.o
[ 46%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/GroundedSchemaNode.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryManager.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DefineLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/LibraryRunner.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DeleteLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/DLScheme.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FindUtils.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/SCMRunner.cc.o
[ 48%] Linking CXX shared library libgrounded.so
[ 48%] Built target grounded
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeVariables.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FunctionLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/GrantLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/LambdaLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/NumberNode.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PrenexLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PresentLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PutLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Quotation.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RandomChoice.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Replacement.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RewriteLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/ScopeLink.cc.o
azureuser@friendbot:~/build$  pgrep -af "make|c++" | head -5
4 [kworker/R-rcu_gp]
5 [kworker/R-sync_wq]
6 [kworker/R-kvfree_rcu_reclaim]
13 [kworker/R-mm_percpu_wq]
14 [rcu_tasks_rude_kthread]
azureuser@friendbot:~/build$  tail -20 /tmp/build_output.txt
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/DeleteLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/DLScheme.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FindUtils.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/grounded/CMakeFiles/grounded.dir/SCMRunner.cc.o
[ 48%] Linking CXX shared library libgrounded.so
[ 48%] Built target grounded
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeLink.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FreeVariables.cc.o
[ 48%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/FunctionLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/GrantLink.cc.o
[ 51%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/LambdaLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/NumberNode.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PrenexLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PresentLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/PutLink.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Quotation.cc.o
[ 53%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RandomChoice.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/Replacement.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/RewriteLink.cc.o
[ 56%] Building CXX object atomspace/opencog/atoms/core/CMakeFiles/atomcore.dir/ScopeLink.cc.o
azureuser@friendbot:~/build$  dmesg 2>/dev/null | grep -i "oom\|killed" | tail -5
azureuser@friendbot:~/build$  sudo dmesg 2>/dev/null | grep -i "oom\|killed\|out of memory" | tail -10
azureuser@friendbot:~/build$  free -h && echo "---" && cat /proc/meminfo | head -5
               total        used        free      shared  buff/cache   available
Mem:           7.8Gi       2.1Gi       1.0Gi       4.0Mi       5.0Gi       5.7Gi
Swap:             0B          0B          0B
---
MemTotal:        8133952 kB
MemFree:         1040024 kB
MemAvailable:    5935336 kB
Buffers:          155084 kB
Cached:          4679556 kB
azureuser@friendbot:~/build$  cd /home/azureuser/build && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|Linking|^\[" | tail -80
[ 39%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/LinkSignatureLink.cc.o
[ 39%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ExecuteThreadedLink.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/NumberOfLink.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ParallelLink.cc.o
[ 41%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/PromiseLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/PureExecLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SetTVLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/SleepLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SetValueLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/parallel/CMakeFiles/parallel.dir/ThreadJoinLink.cc.o
[ 43%] Building CXX object atomspace/opencog/atoms/flow/CMakeFiles/atomflow.dir/SizeOfLink.cc.o
[ 43%] Linking CXX shared library libparallel.so
[ 43%] Built target parallel
[ 46%] Linking CXX shared library libclearbox.so
[ 48%] Built target clearbox
[ 48%] Linking CXX shared library libjoin.so
[ 51%] Built target join
[ 51%] Linking CXX shared library librule.so
[ 53%] Built target rule
[ 53%] Linking CXX shared library libatomflow.so
[ 56%] Built target atomflow
[ 56%] Linking CXX shared library libexecution.so
[ 58%] Built target execution
[ 58%] Linking CXX shared library libquery-engine.so
[ 60%] Built target query-engine
[ 60%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternLink.cc.o
[ 60%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/PatternUtils.cc.o
[ 60%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/Pattern.cc.o
[ 60%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/QueryLink.cc.o
[ 60%] Building CXX object atomspace/opencog/atoms/pattern/CMakeFiles/pattern.dir/SatisfactionLink.cc.o
[ 60%] Linking CXX shared library libpattern.so
[ 63%] Built target pattern
[ 63%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/AtomTable.cc.o
[ 63%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/AtomSpace.cc.o
[ 63%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/Frame.cc.o
[ 63%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/Transient.cc.o
[ 63%] Building CXX object atomspace/opencog/atomspace/CMakeFiles/atomspace.dir/TypeIndex.cc.o
[ 63%] Linking CXX shared library libatomspace.so
[ 63%] Built target atomspace
[ 63%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeEval.cc.o
[ 63%] Building CXX object atomspace-storage/opencog/persist/csv/CMakeFiles/csv.dir/table_read.cc.o
[ 63%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeModule.cc.o
[ 63%] Linking CXX shared library libcsv.so
[ 63%] Built target csv
[ 63%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemePrimitive.cc.o
[ 63%] Building CXX object atomspace-storage/opencog/persist/sexpr/CMakeFiles/sexpr.dir/AtomSexpr.cc.o
[ 63%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmob.cc.o
[ 63%] Building CXX object atomspace-storage/opencog/persist/sexpr/CMakeFiles/sexpr.dir/FrameSexpr.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobAtom.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/sexpr/CMakeFiles/sexpr.dir/ValueSexpr.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobAS.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobGC.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobNew.cc.o
[ 65%] Linking CXX shared library libsexpr.so
[ 65%] Built target sexpr
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobPrint.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/json/CMakeFiles/json.dir/DecodeJson.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobTV.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobValue.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/json/CMakeFiles/json.dir/EncodeJson.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/json/CMakeFiles/json.dir/JSCommands.cc.o
[ 65%] Building CXX object atomspace/opencog/guile/CMakeFiles/smob.dir/SchemeSmobLogger.cc.o
[ 65%] Linking CXX shared library libsmob.so
[ 65%] Built target smob
[ 65%] Building CXX object atomspace-storage/opencog/persist/json/CMakeFiles/json.dir/JsonEval.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/prolog/CMakeFiles/datalog.dir/DecodeProlog.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/json/CMakeFiles/json.dir/McpPlugAtomSpace.cc.o
[ 65%] Building CXX object atomspace-storage/opencog/persist/prolog/CMakeFiles/datalog.dir/EncodeProlog.cc.o
[ 65%] Linking CXX shared library libjson.so
[ 65%] Built target json
[ 65%] Building CXX object atomspace-storage/opencog/persist/tlb/CMakeFiles/tlb.dir/TLB.cc.o
[ 68%] Linking CXX shared library libdatalog.so
[ 68%] Built target datalog
[ 68%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/CogServer.cc.o
/home/azureuser/occ/cogserver/opencog/cogserver/shell/McpEval.h:32:10: fatal error: opencog/persist/json/McpPlugin.h: No such file or directory
make[2]: *** [cogserver/opencog/cogserver/server/CMakeFiles/server.dir/build.make:76: cogserver/opencog/cogserver/server/CMakeFiles/server.dir/CogServer.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:12366: cogserver/opencog/cogserver/server/CMakeFiles/server.dir/all] Error 2
[ 70%] Linking CXX shared library libtlb.so
[ 70%] Built target tlb
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -n "INCLUDE_DIRECTORIES\|include_directories" /home/azureuser/occ/cogserver/CMakeLists.txt 2>/dev/null
127:    INCLUDE_DIRECTORIES(${JSONCPP_INCLUDE_DIRS})
166:INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}
azureuser@friendbot:~/build$  grep -n "ATOMSPACE_STORAGE_INCLUDE\|AtomSpaceStorage" /home/azureuser/occ/cogserver/CMakeLists.txt 2>/dev/null
95:FIND_PACKAGE(AtomSpaceStorage CONFIG REQUIRED)
azureuser@friendbot:~/build$  grep "INCLUDE_DIR\|INCLUDE_DIRECTORIES" /home/azureuser/occ/atomspace-storage/cmake/AtomSpaceStorageConfig.cmake.in 2>/dev/null
    set(ATOMSPACE_STORAGE_INCLUDE_DIR "@CMAKE_INSTALL_PREFIX@/include/")
azureuser@friendbot:~/build$  grep -n "atomspace-storage\|ATOMSPACE_STORAGE" /home/azureuser/occ/CMakeLists.txt | head -15
188:OPTION(BUILD_ATOMSPACE_STORAGE "Build AtomSpace storage backends" ON)
189:IF(BUILD_ATOMSPACE_STORAGE AND BUILD_ATOMSPACE)
191:    IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/atomspace-storage/CMakeLists.txt")
192:        add_subdirectory(atomspace-storage)
194:        set(AtomSpaceStorage_DIR "${CMAKE_BINARY_DIR}/atomspace-storage/cmake" CACHE PATH "In-tree AtomSpaceStorage config" FORCE)
196:        set(ATOMSPACE_STORAGE_FOUND 1 CACHE BOOL "AtomSpaceStorage found" FORCE)
198:        MESSAGE(WARNING "atomspace-storage not found - CogServer build will fail!")
210:IF(BUILD_COGSERVER AND BUILD_ATOMSPACE AND BUILD_ATOMSPACE_STORAGE)
220:IF(BUILD_ATOMSPACE_STORAGE AND BUILD_COGSERVER AND BUILD_ATOMSPACE)
342:MESSAGE(STATUS "AtomSpace Storage: ${BUILD_ATOMSPACE_STORAGE}")
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -5 && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -80
--   - opencog-debian/BUILD_ORDER_ENHANCED.md
-- 
-- Configuring done (2.9s)
-- Generating done (1.8s)
-- Build files have been written to: /home/azureuser/build
[  7%] Built target STORAGE_SCM_CONFIG
[  7%] Built target storage_types
[  7%] Built target COGSERVER_SCM_CONFIG
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  7%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ConsoleSocket.cc.o
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  7%] Building CXX object cogserver/examples/module/CMakeFiles/example_module.dir/ExampleModule.cc.o
[  7%] Linking CXX shared library libexample_module.so
[  7%] Built target example_module
[  7%] Built target COG_SCM_CONFIG
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  7%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/GenericShell.cc.o
[  7%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/NetworkServer.cc.o
[  9%] Built target persist_cog_atom_types
[  9%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/ServerSocket.cc.o
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  9%] Building CXX object cogserver/opencog/network/CMakeFiles/network.dir/WebSocket.cc.o
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 14%] Built target agentic_chatbots
[ 17%] Built target cogself
[ 17%] Built target afi
[ 17%] Built target cognitive-scheduler
[ 19%] Built target atom_types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 21%] Built target value
[ 24%] Built target storage-types
[ 24%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 24%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog_persist_cog-types
[ 24%] Built target sensory-v0-types
[ 24%] Built target sensory-types
[ 26%] Built target entelechy
[ 26%] Built target truthvalue
[ 26%] Built target atombase
[ 36%] Built target atomcore
[ 36%] Built target foreign
[ 39%] Built target grounded
[ 41%] Built target join
[ 43%] Built target rule
[ 43%] Built target columnvec
[ 48%] Built target atomflow
[ 51%] Built target parallel
[ 56%] Built target clearbox
[ 58%] Built target execution
[ 60%] Built target query-engine
[ 63%] Built target pattern
[ 63%] Built target atomspace
[ 65%] Built target smob
[ 65%] Built target csv
[ 65%] Built target sexpr
[ 65%] Built target json
[ 68%] Built target datalog
[ 70%] Built target tlb
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/LookatLink.cc.o
[ 73%] Linking CXX shared library libnetwork.so
[ 73%] Built target network
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/OpenLink.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory/CMakeFiles/sensory.dir/ReadStream.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/OutputStream.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory/CMakeFiles/sensory.dir/SensoryNode.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/SensoryNode.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory/CMakeFiles/sensory.dir/StreamNode.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/StreamEqualLink.cc.o
[ 73%] Building CXX object sensory/opencog/atoms/sensory/CMakeFiles/sensory.dir/TextStreamNode.cc.o
[ 73%] Linking CXX shared library libsensory.so
[ 73%] Building CXX object sensory/opencog/atoms/sensory-v0/CMakeFiles/sensory-v0.dir/WriteLink.cc.o
[ 73%] Built target sensory
[ 75%] Building CXX object integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o
/home/azureuser/occ/atomspace/opencog/atomspace/AtomSpace.h:28:10: fatal error: opencog/util/async_method_caller.h: No such file or directory
make[2]: *** [integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/build.make:76: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:15729: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/all] Error 2
[ 75%] Linking CXX shared library libsensory-v0.so
[ 75%] Built target sensory-v0
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -n "INCLUDE_DIRECTORIES\|include_directories" /home/azureuser/occ/integration/atomspace-machspace/CMakeLists.txt 2>/dev/null
8:INCLUDE_DIRECTORIES(
azureuser@friendbot:~/build$  ls /home/azureuser/occ/integration/
CMakeLists.txt  README.md  atomspace-machspace  build-integration-layer.sh  cognitive-scheduler  hurdcog-bridge
azureuser@friendbot:~/build$  grep -rn "INCLUDE_DIRECTORIES" /home/azureuser/occ/integration/*/CMakeLists.txt 2>/dev/null
8:INCLUDE_DIRECTORIES(
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -3 && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -80
-- Configuring done (2.9s)
-- Generating done (2.4s)
-- Build files have been written to: /home/azureuser/build
[  4%] Built target atomspace_accelerator
[  7%] Built target tensor-logic
[  7%] Built target STORAGE_SCM_CONFIG
[  7%] Built target storage_types
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  7%] Built target COGSERVER_SCM_CONFIG
[  7%] Built target network
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  7%] Built target example_module
[  7%] Built target COG_SCM_CONFIG
[  9%] Built target persist_cog_atom_types
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 14%] Built target agentic_chatbots
[ 14%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 14%] Built target afi
[ 17%] Built target cogself
[ 17%] Built target cognitive-scheduler
[ 19%] Built target atom_types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 19%] Built target sensory-v0-types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog_persist_cog-types
[ 19%] Built target sensory-types
[ 21%] Built target entelechy
[ 24%] Built target value
[ 26%] Built target storage-types
[ 26%] Built target truthvalue
[ 26%] Built target atombase
[ 26%] Built target foreign
[ 29%] Built target grounded
[ 39%] Built target atomcore
[ 39%] Built target columnvec
[ 41%] Built target parallel
[ 46%] Built target atomflow
[ 51%] Built target clearbox
[ 53%] Built target join
[ 56%] Built target rule
[ 58%] Built target execution
[ 60%] Built target query-engine
[ 63%] Built target pattern
[ 63%] Built target atomspace
[ 63%] Built target csv
[ 65%] Built target smob
[ 65%] Built target sexpr
[ 68%] Built target datalog
[ 68%] Built target json
[ 68%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/CogServer.cc.o
[ 70%] Built target tlb
[ 70%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/MCPServer.cc.o
[ 73%] Built target sensory-v0
[ 73%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/ModuleManager.cc.o
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:28:2: error: #else after #else
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:24: error: the conditional began here
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:140:5: error: ‘dlerror’ was not declared in this scope; did you mean ‘perror’?
  140 |     dlerror();
      |     perror
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:153:45: error: ‘RTLD_LAZY’ was not declared in this scope
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:153:57: error: ‘RTLD_GLOBAL’ was not declared in this scope
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:153:24: error: ‘dlopen’ was not declared in this scope; did you mean ‘popen’?
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:169:31: error: ‘dlsym’ was not declared in this scope
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:293:5: error: ‘dlerror’ was not declared in this scope; did you mean ‘perror’?
  293 |     dlerror(); // Reset error state.
      |     perror
/home/azureuser/occ/cogserver/opencog/cogserver/server/ModuleManager.cc:294:9: error: ‘dlclose’ was not declared in this scope; did you mean ‘pclose’?
make[2]: *** [cogserver/opencog/cogserver/server/CMakeFiles/server.dir/build.make:104: cogserver/opencog/cogserver/server/CMakeFiles/server.dir/ModuleManager.cc.o] Error 1
[ 73%] Built target sensory
[ 75%] Building CXX object integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.h:14:10: fatal error: opencog/persist/api/BackingStore.h: No such file or directory
make[2]: *** [integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/build.make:76: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:15729: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/all] Error 2
make[1]: *** [CMakeFiles/Makefile2:12366: cogserver/opencog/cogserver/server/CMakeFiles/server.dir/all] Error 2
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -3 && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -80
-- Configuring done (2.9s)
-- Generating done (1.8s)
-- Build files have been written to: /home/azureuser/build
[  0%] Built target coggml
[  4%] Built target cogutil
[  4%] Built target SCM_CONFIG
[  4%] Built target opencog_atom_types
[  4%] Built target atomspace_accelerator
[  7%] Built target tensor-logic
[  7%] Built target storage_types
[  7%] Built target STORAGE_SCM_CONFIG
[  7%] Built target COGSERVER_SCM_CONFIG
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_csv
[  7%] Built target network
[  7%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_cogserver_opencog_cogserver_scm
[  7%] Built target example_module
[  7%] Built target COG_SCM_CONFIG
[  9%] Built target persist_cog_atom_types
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_fake
[  9%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_matrix_opencog_matrix
[  9%] Built target SCM_V0_CONFIG
[  9%] Built target SENSORY_SCM_CONFIG
[  9%] Built target sensory_v0_atom_types
[ 12%] Built target sensory_atom_types
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory-v0
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_learn_scm
[ 12%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_sensory_opencog_sensory
[ 14%] Built target agentic_chatbots
[ 17%] Built target cogself
[ 17%] Built target afi
[ 17%] Built target cognitive-scheduler
[ 19%] Built target atom_types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_storage
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_atoms_atom_types
[ 19%] Built target sensory-v0-types
[ 19%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-cog_opencog_persist_cog-types
[ 19%] Built target sensory-types
[ 21%] Built target entelechy
[ 24%] Built target storage-types
[ 26%] Built target value
[ 26%] Built target truthvalue
[ 26%] Built target atombase
[ 26%] Built target foreign
[ 29%] Built target grounded
[ 39%] Built target atomcore
[ 39%] Built target columnvec
[ 41%] Built target parallel
[ 46%] Built target atomflow
[ 48%] Built target join
[ 53%] Built target clearbox
[ 56%] Built target rule
[ 58%] Built target execution
[ 60%] Built target query-engine
[ 63%] Built target pattern
[ 63%] Built target atomspace
[ 63%] Built target csv
[ 63%] Built target sexpr
[ 65%] Built target smob
[ 68%] Built target datalog
[ 68%] Built target json
[ 70%] Built target tlb
[ 70%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/ModuleManager.cc.o
[ 73%] Built target sensory-v0
[ 73%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/Request.cc.o
[ 73%] Built target sensory
[ 73%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/RequestManager.cc.o
[ 73%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/ServerConsole.cc.o
[ 75%] Building CXX object integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o
[ 78%] Building CXX object cogserver/opencog/cogserver/server/CMakeFiles/server.dir/WebServer.cc.o
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.h:65:53: error: ‘AttentionValuePtr’ does not name a type
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.h:66:5: error: ‘AttentionValuePtr’ does not name a type
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.cc:32:12: error: ‘MACHSPACE_STORAGE_NODE’ was not declared in this scope
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.cc:196:71: error: ‘AttentionValuePtr’ does not name a type
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.cc:204:31: error: base operand of ‘->’ is not a pointer
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.cc:208:1: error: ‘AttentionValuePtr’ does not name a type
/home/azureuser/occ/integration/atomspace-machspace/MachSpaceStorageNode.cc:303:50: error: could not convert ‘std::make_shared(_Args&& ...) [with _Tp = opencog::MachSpaceStorageNode; _Args = {const __cxx11::basic_string<char, char_traits<char>, allocator<char> >&}; typename enable_if<(! is_array< <template-parameter-1-1> >::value), _Tp>::type = opencog::MachSpaceStorageNode]()’ from ‘std::shared_ptr<opencog::MachSpaceStorageNode>’ to ‘opencog::Handle’
make[2]: *** [integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/build.make:76: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/MachSpaceStorageNode.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:15729: integration/atomspace-machspace/CMakeFiles/atomspace-machspace.dir/all] Error 2
[ 78%] Linking CXX shared library libserver.so
[ 78%] Built target server
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -n "atomspace-machspace\|integration" /home/azureuser/occ/CMakeLists.txt | head -15
3:# Monorepo integration for cognitive synergy
6:# in the optimal order for integration.
47:OPTION(BUILD_AGENTIC_CHATBOTS "Build agentic chatbots integration" ON)
53:OPTION(BUILD_TENSOR_LOGIC "Build Tensor Logic integration layer" ON)
55:# External integration packages
56:OPTION(BUILD_GNUCASH "Build Gnucash cognitive accounting integration" OFF)
57:OPTION(BUILD_KOBOLDCPP "Build KoboldCpp story/world modeling integration" OFF)
58:OPTION(BUILD_APHRODITE "Build Aphrodite Engine LLM inference integration" OFF)
183:        MESSAGE(STATUS "  tensor-logic/ directory will be created for integration")
266:# External integration packages
268:    MESSAGE(STATUS "Building Gnucash cognitive accounting integration...")
273:    MESSAGE(STATUS "Building KoboldCpp story/world modeling integration...")
278:    MESSAGE(STATUS "Building Aphrodite Engine LLM inference integration...")
283:OPTION(BUILD_INTEGRATION_LAYER "Build integration bridges" ON)
285:    MESSAGE(STATUS "Building integration layer...")
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release -DBUILD_ATOMSPACE_MACHSPACE=OFF 2>&1 | tail -3 && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -80
-- Configuring done (2.9s)
-- Generating done (1.8s)
-- Build files have been written to: /home/azureuser/build
[ 84%] Built target top-shell
[ 84%] Building CXX object sensory/opencog/atoms/filedir-v0/CMakeFiles/sensory-v0-filedir.dir/TextFileStream.cc.o
[ 87%] Building CXX object sensory/opencog/atoms/irc-v0/CMakeFiles/sensory-v0-irc.dir/IRC.cc.o
[ 87%] Building CXX object sensory/opencog/atoms/irc-v0/CMakeFiles/sensory-v0-irc.dir/IRChatStream.cc.o
[ 87%] Linking CXX shared library libsensory-v0-filedir.so
[ 87%] Built target sensory-v0-filedir
[ 87%] Building CXX object sensory/opencog/atoms/terminal-v0/CMakeFiles/sensory-v0-terminal.dir/TerminalStream.cc.o
[ 87%] Linking CXX shared library libsensory-v0-irc.so
[ 87%] Built target sensory-v0-irc
[ 89%] Building CXX object sensory/opencog/atoms/filedir/CMakeFiles/sensory-filedir.dir/FileSysNode.cc.o
[ 89%] Linking CXX shared library libsensory-v0-terminal.so
[ 89%] Built target sensory-v0-terminal
[ 89%] Building CXX object sensory/opencog/atoms/filedir/CMakeFiles/sensory-filedir.dir/TextFileNode.cc.o
[ 89%] Building CXX object sensory/opencog/atoms/irc/CMakeFiles/sensory-irc.dir/IRC.cc.o
[ 89%] Building CXX object sensory/opencog/atoms/irc/CMakeFiles/sensory-irc.dir/IRChatNode.cc.o
[ 89%] Linking CXX shared library libsensory-filedir.so
[ 89%] Built target sensory-filedir
[ 89%] Building CXX object sensory/opencog/atoms/terminal/CMakeFiles/sensory-terminal.dir/TerminalNode.cc.o
[ 89%] Linking CXX shared library libsensory-terminal.so
[ 89%] Built target sensory-terminal
[ 89%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_guile_modules
[ 89%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_api
[ 89%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/CachingProxy.cc.o
[ 89%] Linking CXX shared library libsensory-irc.so
[ 89%] Built target sensory-irc
[ 89%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/DynamicDataProxy.cc.o
[ 89%] Building CXX object atomspace-storage/opencog/persist/flow/CMakeFiles/persist-flow.dir/FetchValueOfLink.cc.o
[ 89%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/NullProxy.cc.o
[ 89%] Building CXX object atomspace-storage/opencog/persist/flow/CMakeFiles/persist-flow.dir/StoreValueOfLink.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/ProxyNode.cc.o
[ 92%] Linking CXX shared library libpersist-flow.so
[ 92%] Built target persist-flow
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/ReadThruProxy.cc.o
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_tlb
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/ReadWriteProxy.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/SequentialReadProxy.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/WriteBufferProxy.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/proxy/CMakeFiles/persist-proxy.dir/WriteThruProxy.cc.o
[ 92%] Linking CXX shared library libpersist-proxy.so
[ 92%] Built target persist-proxy
[ 92%] Building CXX object atomspace-storage/opencog/persist/sexcom/CMakeFiles/sexcom.dir/Dispatcher.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/sexcom/CMakeFiles/sexcom.dir/Commands.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/sexcom/CMakeFiles/sexcom.dir/SexprEval.cc.o
[ 92%] Linking CXX shared library libsexcom.so
[ 92%] Built target sexcom
[ 92%] Building CXX object atomspace-storage/opencog/persist/file/CMakeFiles/fast_load_scm.dir/fast_load.cc.o
[ 92%] Linking CXX shared library libfast_load_scm.so
[ 92%] Built target fast_load_scm
[ 92%] Building CXX object atomspace-storage/opencog/persist/file/CMakeFiles/persist-file.dir/PersistFileSCM.cc.o
[ 92%] Building CXX object atomspace-storage/opencog/persist/file/CMakeFiles/persist-file.dir/FileStorage.cc.o
[ 92%] Linking CXX shared library libpersist-file.so
[ 92%] Built target persist-file
[ 94%] Building CXX object cogserver/opencog/cogserver/shell/CMakeFiles/sexpr-shell.dir/SexprShell.cc.o
[ 94%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_file
[ 94%] Building CXX object cogserver/opencog/cogserver/shell/CMakeFiles/json-shell.dir/JsonShell.cc.o
[ 94%] Building CXX object cogserver/opencog/cogserver/shell/CMakeFiles/json-shell.dir/JsonShellModule.cc.o
[ 94%] Building CXX object cogserver/opencog/cogserver/shell/CMakeFiles/sexpr-shell.dir/SexprShellModule.cc.o
[ 97%] Linking CXX shared library libjson-shell.so
[ 97%] Linking CXX shared library libsexpr-shell.so
[ 97%] Built target json-shell
[ 97%] Built target sexpr-shell
[ 97%] Building CXX object atomspace-cog/opencog/persist/cog-types/CMakeFiles/persist-cog-types.dir/atom_types_init.cc.o
[100%] Linking CXX shared library libpersist-cog-types.so
[100%] Built target persist-cog-types
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogIO.cc.o
[100%] Building CXX object atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/CogSimpleStorage.cc.o
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-simple/CogSimpleStorage.cc:39:2: error: #else after #else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-simple/CogSimpleStorage.cc:35: error: the conditional began here
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-simple/CogSimpleStorage.cc:52:2: error: #else after #else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-simple/CogSimpleStorage.cc:48: error: the conditional began here
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-simple/CogSimpleStorage.cc:65:10: fatal error: opencog/persist/cog-types/atom_types.h: No such file or directory
make[2]: *** [atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/build.make:76: atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/CogSimpleStorage.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:13128: atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/all] Error 2
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogPersistSCM.cc.o
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogIO.cc:30:10: fatal error: opencog/persist/sexpr/Sexpr.h: No such file or directory
make[2]: *** [atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/build.make:76: atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogIO.cc.o] Error 1
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogPersistSCM.cc:29:10: fatal error: opencog/persist/api/StorageNode.h: No such file or directory
make[2]: *** [atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/build.make:90: atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogPersistSCM.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:13187: atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/all] Error 2
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -n "INCLUDE_DIRECTORIES.*SOURCE_DIR\|INCLUDE_DIRECTORIES.*INCLUDE_DIR" /home/azureuser/occ/atomspace-cog/CMakeLists.txt 2>/dev/null
151:INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}
azureuser@friendbot:~/build$  cd /home/azureuser/build && cmake /home/azureuser/occ -DCMAKE_BUILD_TYPE=Release -DBUILD_ATOMSPACE_MACHSPACE=OFF 2>&1 | tail -3 && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -40
-- Configuring done (2.8s)
-- Generating done (1.7s)
-- Build files have been written to: /home/azureuser/build
[ 84%] Built target persist-proxy
[ 84%] Built target persist-flow
[ 84%] Built target guile-uuid
[ 84%] Built target guile-cogserver
[ 84%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_scm
[ 84%] Built target cogserver
[ 87%] Built target builtinreqs
[ 87%] Built target scheme-shell
[ 87%] Built target top-shell
[ 87%] Built target sensory-v0-filedir
[ 89%] Built target sensory-v0-irc
[ 89%] Built target sensory-v0-terminal
[ 89%] Built target sensory-irc
[ 92%] Built target sensory-filedir
[ 92%] Built target sensory-terminal
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_api
[ 92%] Built target sexcom
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_guile_modules
[ 92%] Built target fast_load_scm
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_tlb
[ 92%] Built target persist-file
[ 94%] Built target sexpr-shell
[ 94%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_file
[ 94%] Building CXX object atomspace-cog/opencog/persist/cog-types/CMakeFiles/persist-cog-types.dir/atom_types_init.cc.o
[ 97%] Built target json-shell
[100%] Linking CXX shared library libpersist-cog-types.so
[100%] Built target persist-cog-types
[100%] Building CXX object atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/CogSimpleStorage.cc.o
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogIO.cc.o
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogChannel.cc:40:2: error: #else after #else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogChannel.cc:36: error: the conditional began here
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogChannel.cc:53:2: error: #else after #else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogChannel.cc:49: error: the conditional began here
make[2]: *** [atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/build.make:76: atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogIO.cc.o] Error 1
make[1]: *** [CMakeFiles/Makefile2:13187: atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/all] Error 2
[100%] Building CXX object atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/CogSimpleIO.cc.o
[100%] Building CXX object atomspace-cog/opencog/persist/cog-simple/CMakeFiles/persist-cog-simple.dir/CogSimplePersistSCM.cc.o
[100%] Linking CXX shared library libpersist-cog-simple.so
[100%] Built target persist-cog-simple
make: *** [Makefile:156: all] Error 2
azureuser@friendbot:~/build$  grep -rn "#else after #else\|#else$" /home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/*.cc 2>/dev/null | head -10
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogChannel.cc:36:#else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogStorage.cc:35:#else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogStorage.cc:36:#else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogStorage.cc:40:#else
/home/azureuser/occ/atomspace-cog/opencog/persist/cog-storage/CogStorage.cc:43:#else
azureuser@friendbot:~/build$  cd /home/azureuser/build && make -j$(nproc) 2>&1 | grep -E "error|Error|FAILED|Built target|^\[.*%\]" | tail -30
[ 84%] Built target persist-flow
[ 84%] Built target guile-uuid
[ 84%] Built target guile-cogserver
[ 84%] Built target cogserver
[ 87%] Built target builtinreqs
[ 87%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_scm
[ 87%] Built target scheme-shell
[ 87%] Built target top-shell
[ 87%] Built target sensory-v0-filedir
[ 89%] Built target sensory-v0-irc
[ 89%] Built target sensory-v0-terminal
[ 92%] Built target sensory-filedir
[ 92%] Built target sensory-irc
[ 92%] Built target sensory-terminal
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_api
[ 92%] Built target sexcom
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace_opencog_guile_modules
[ 92%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_tlb
[ 92%] Built target fast_load_scm
[ 92%] Built target persist-file
[ 94%] Built target sexpr-shell
[ 94%] Built target COPY_TO_LOAD_PATH_IN_BUILD_DIR_FROM__home_azureuser_occ_atomspace-storage_opencog_persist_file
[ 97%] Built target persist-cog-types
[100%] Built target json-shell
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogIO.cc.o
[100%] Built target persist-cog-simple
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogPersistSCM.cc.o
[100%] Building CXX object atomspace-cog/opencog/persist/cog-storage/CMakeFiles/persist-cog.dir/CogStorage.cc.o
[100%] Linking CXX shared library libpersist-cog.so
[100%] Built target persist-cog
azureuser@friendbot:~/build$  find /home/azureuser/build -name "*.so" -type f 2>/dev/null | wc -l && echo "---Libraries built---" && find /home/azureuser/build -name "*.so" -type f 2>/dev/null | xargs -I{} basename {} | sort
65
---Libraries built---
libafi.so
libagentic_chatbots.so
libatom_types.so
libatombase.so
libatomcore.so
libatomflow.so
libatomspace.so
libatomspace_accelerator.so
libbuiltinreqs.so
libclearbox.so
libcoggml.so
libcogself.so
libcogutil.so
libcolumnvec.so
libcsv-table.so
libcsv.so
libdatalog.so
libentelechy.so
libexample_module.so
libexec.so
libexecution.so
libfast_load_scm.so
libforeign.so
libgrounded.so
libguile-cogserver.so
libjoin.so
libjson-shell.so
libjson.so
liblogger.so
libnetwork.so
libparallel.so
libpattern.so
libpersist-cog-simple.so
libpersist-cog-types.so
libpersist-cog.so
libpersist-file.so
libpersist-flow.so
libpersist-proxy.so
libpersist.so
libquery-engine.so
librandgen.so
librule.so
libscheme-shell.so
libsensory-filedir.so
libsensory-irc.so
libsensory-terminal.so
libsensory-types.so
libsensory-v0-filedir.so
libsensory-v0-irc.so
libsensory-v0-terminal.so
libsensory-v0-types.so
libsensory-v0.so
libsensory.so
libserver.so
libsexcom.so
libsexpr-shell.so
libsexpr.so
libsmob.so
libstorage-types.so
libtensor-logic.so
libtlb.so
libtop-shell.so
libtruthvalue.so
libtype-utils.so
libvalue.so
azureuser@friendbot:~/build$ 
