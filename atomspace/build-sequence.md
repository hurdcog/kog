# OCC Core Build Sequence

**Platform:** Ubuntu 24.04.3 LTS  
**Compiler:** GCC 13.3.0 (C/C++)  
**CMake:** 3.28.3  
**Build type:** Release  
**Command:**
```bash
cmake .. \
  -DBUILD_ATOMSPACE_STORAGE=OFF \
  -DBUILD_COGSERVER=OFF \
  -DBUILD_MATRIX=OFF \
  -DBUILD_LEARN=OFF \
  -DBUILD_AGENTS=OFF \
  -DBUILD_SENSORY=OFF \
  -DBUILD_COGSELF=OFF \
  -DBUILD_AGENTIC_CHATBOTS=OFF \
  -DBUILD_ENTELECHY=OFF \
  -DBUILD_INTEGRATION_LAYER=OFF
cmake --build . -j$(nproc)
```

---

## Prerequisites installed

```
build-essential  cmake  git
guile-3.0  guile-3.0-dev
python3  python3-dev
libboost-all-dev
cxxtest
binutils-dev  libiberty-dev
libasio-dev
```

---

## Configure phase — components enabled

| Layer | Component | Status |
|-------|-----------|--------|
| Layer 3 | **CogUtil** 2.1.0 | ✅ found |
| Layer 3 | **CogGML** Microkernel | ✅ configured |
| Layer 3 | **AtomSpace** | ✅ configured |
| Layer 3 | **AtomSpace Accelerator** | ✅ configured |
| Layer 3 | **ATen** Tensor Library | ✅ configured |
| Layer 3 | **ATenSpace** | ✅ configured |
| Layer 3 | **Tensor Logic** | ✅ configured |

Optional bindings not built (packages absent):
- Doxygen — documentation
- Cython / nosetests — Python bindings & tests
- OCaml / Stack — OCaml & Haskell bindings
- Valgrind — thread debugging
- Folly / SparseHash — optional performance replacements

---

## Build phase — shared libraries produced (in link order)

```
[  9%] libcoggml.so
[  9%] libcogutil.so
[ 12%] libatom_types.so       (opencog_atom_types generated sources)
[ 12%] libatomspace_accelerator.so
[ 12%] libtensor-logic.so
[ 15%] libatombase.so
[ 18%] libvalue.so
[ 21%] libtruthvalue.so
[ 25%] libatom_types.so       (final link)
[ 28%] libatomcore.so
[ 31%] libcolumnvec.so
[ 34%] libatomflow.so
[ 37%] libparallel.so
[ 40%] libjoin.so
[ 43%] libforeign.so
[ 46%] libgrounded.so
[ 50%] libclearbox.so
[ 53%] librule.so
[ 56%] libexecution.so
[ 59%] libquery-engine.so
[ 62%] libpattern.so
[ 65%] libatomspace.so
[ 68%] libsmob.so             (Guile Scheme bindings)
[ 71%] libexec.so
[ 74%] liblogger.so
[ 78%] librandgen.so
[ 81%] libtype-utils.so
[100%] SCM load-path copies
```

**Exit code: 0 — build successful.**

---

## Known deferred components (standalone-path CMake fixes pending)

| Component | Blocker |
|-----------|---------|
| AtomSpace Storage | Requires installed `AtomSpaceTargets.cmake` |
| atomspace-rocks | Requires `AtomSpaceStorage` package config |
| atomspace-cog | Requires `AtomSpaceStorage` package config |
| CogServer | Depends on AtomSpace Storage |
| Matrix / Learn / Agents / Sensory | Use standalone helper-module paths |
| CogSelf / Entelechy | Depend on CogServer |

Fix strategy: add `list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cogutil/cmake")` and
`find_package` fallbacks to those subproject `CMakeLists.txt` files (same pattern applied to
`atomspace` and `atomspace-storage` in this session).
