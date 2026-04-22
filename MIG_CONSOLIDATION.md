# MIG (Mach Interface Generator) Consolidation

**Date**: December 12, 2025  
**Action**: Consolidated duplicate MIG implementations

## Changes Made

### Problem
MIG (Mach Interface Generator) existed in two locations:
- `cognumach/mig/` - Full implementation with 67 files including compiled binary
- `hurdcog/mig/` - Partial copy with 65 files, no compiled binary

This duplication caused:
- Build confusion and potential version conflicts
- Maintenance overhead
- Unclear canonical source

### Solution
1. Backed up `hurdcog/mig/` to `hurdcog/mig.backup/`
2. Replaced `hurdcog/mig/` with symbolic link to `../cognumach/mig/`
3. All references now point to single canonical MIG source in cognumach

### Rationale
- CognuMach MIG is more complete (67 vs 65 files)
- CognuMach MIG has compiled binary (migcom)
- MIG is fundamentally a Mach microkernel tool, belongs in cognumach
- Symlink maintains compatibility with existing build scripts

### Build System Impact
- HurdCog builds can still reference `hurdcog/mig/` via symlink
- No build script changes required
- Single source of truth for MIG maintenance
- Eliminates version drift between copies

## Verification
```bash
ls -la hurdcog/mig
# Output: lrwxrwxrwx 1 ubuntu ubuntu 16 Dec 11 23:38 hurdcog/mig -> ../cognumach/mig
```

## Rollback (if needed)
```bash
rm hurdcog/mig
mv hurdcog/mig.backup hurdcog/mig
```
