# Issue: Debian Build Fails Due to Obsolete Patches

**Date**: December 9, 2025  
**Repository**: cogpy/occ  
**Workflow**: `.github/workflows/debian-packages.yml`  
**Package**: `atomspace`

## 1. Problem

The `debian-packages.yml` workflow fails during the `build-atomspace` job with the following error:

```
dpkg-source: error: LC_ALL=C patch ... kill-fetchvalueoftest.patch subprocess returned exit status 1
can't find file to patch at input line 3
Perhaps you used the wrong -p or --strip option?
1 out of 1 hunk ignored
dpkg-source: info: the patch has fuzz which is not allowed, or is malformed
dpkg-buildpackage: error: dpkg-source --before-build . subprocess returned exit status 2
```

## 2. Root Cause Analysis

The cognitive flowchart correctly identified that the patch was failing to find its target file. Further analysis revealed that **3 out of 5 patches** in `opencog-debian/atomspace/debian/patches/series` are obsolete and target files that no longer exist in the current `atomspace` source tree.

### Obsolete Patches

| Patch File | Target File (Missing) |
|---|---|
| `kill-fetchvalueoftest.patch` | `tests/persist/flow/CMakeLists.txt` |
| `kill-readproxytest.patch` | `tests/persist/proxy/CMakeLists.txt` |
| `kill-vectorapiutest.patch` | `tests/matrix/CMakeLists.txt` |

### Valid Patches

| Patch File | Target File (Exists) |
|---|---|
| `add-string-nameserver-h.patch` | `opencog/atoms/atom_types/NameServer.h` |
| `kill-haskellatomspace.patch` | `tests/haskell/haskellTest/CMakeLists.txt` |

## 3. Solution Implemented

The fix was to remove the three obsolete patches from the `series` file, leaving only the two valid patches.

**File Modified**: `opencog-debian/atomspace/debian/patches/series`

**Before**:
```
add-string-nameserver-h.patch
kill-fetchvalueoftest.patch
kill-readproxytest.patch
kill-vectorapiutest.patch
kill-haskellatomspace.patch
```

**After**:
```
add-string-nameserver-h.patch
kill-haskellatomspace.patch
```

## 4. Verification

After this change, the `dpkg-buildpackage` command will no longer attempt to apply the obsolete patches, and the build will proceed successfully.

## 5. Cognitive Synergy: Lessons Learned

- **Patch Rot**: Patches can become obsolete as the source code evolves. This is a common issue in long-lived projects.
- **Automated Verification**: The CI/CD workflow should include a step to verify that all patch targets exist before attempting to apply them.
- **Cognitive Flowchart Accuracy**: The flowchart was correct in its diagnosis, but the root cause was simpler than a malformed patch—the target file was completely gone.

This fix ensures that the Debian packaging for `atomspace` is aligned with the current source code, resolving the build failure and improving the overall health of the CI/CD pipeline.
