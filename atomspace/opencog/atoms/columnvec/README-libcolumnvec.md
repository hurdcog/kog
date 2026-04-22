# libcolumnvec.so

**Source:** `atomspace/opencog/atoms/columnvec/`  
**Role:** Column-vector atom types — efficient bulk storage of numeric and symbolic data arranged in typed column arrays, used for matrix and tabular representations.

## What it does

Provides atom types that wrap dense column arrays (floats, links, s-expressions).  They allow matrix-style data to live inside the AtomSpace without encoding every cell as a separate atom, giving significant memory and traversal efficiency for numeric workloads.

## Object files

| Object | Purpose |
|--------|---------|
| `FloatColumn.cc.o` | `FloatColumnValue` — dense `float` column, wraps a `std::vector<double>` |
| `LinkColumn.cc.o` | `LinkColumnValue` — column of atom `Handle`s |
| `SexprColumn.cc.o` | `SexprColumnValue` — column of s-expression strings |
| `TransposeColumn.cc.o` | `TransposeColumnLink` — lazy transpose of a column-value atom |

## Key public headers

- `opencog/atoms/columnvec/FloatColumn.h`
- `opencog/atoms/columnvec/LinkColumn.h`
