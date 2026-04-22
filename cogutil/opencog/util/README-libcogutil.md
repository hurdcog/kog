# libcogutil.so

**Source:** `cogutil/opencog/util/`  
**Role:** Foundation utility library — the lowest-level dependency in the entire OCC stack.  Every other component links against it.

## What it does

Provides cross-cutting infrastructure: logging, configuration loading, assertion/exception handling, thread-safety primitives, platform portability shims, and random-number utilities.  All higher-level OpenCog libraries call into this one for their runtime needs.

## Object files

| Object | Purpose |
|--------|---------|
| `backtrace-symbols.c.o` | Stack-trace symbol resolution (BFD/libiberty backend) |
| `Config.cc.o` | `Config` singleton — loads and queries `opencog.conf` key-value settings |
| `exceptions.cc.o` | `RuntimeException`, `AssertionException`, and the OC exception hierarchy |
| `lazy_selector.cc.o` | `LazySelector` — deferred random selection over a shrinking set |
| `lazy_random_selector.cc.o` | `LazyRandomSelector` — weighted variant of `LazySelector` |
| `Logger.cc.o` | `Logger` singleton — thread-safe levelled logging to file/stdout/syslog |
| `misc.cc.o` | Miscellaneous helpers: string trim/split, file-path utilities |
| `mt19937ar.cc.o` | Portable Mersenne-Twister (MT19937) PRNG |
| `oc_assert.cc.o` | `OC_ASSERT` / `OC_ASSERT_THROW` macros and their runtime handlers |
| `oc_omp.cc.o` | OpenMP thread-count helpers and parallel-STL detection |
| `platform.cc.o` | OS/compiler portability: `gettimeofday` fallback, path separators, etc. |

## Key public headers

- `opencog/util/Config.h`
- `opencog/util/Logger.h`
- `opencog/util/exceptions.h`
- `opencog/util/oc_assert.h`
- `opencog/util/sigslot.h` — `SigSlot<T…>` lightweight signal/slot
- `opencog/util/platform.h`
