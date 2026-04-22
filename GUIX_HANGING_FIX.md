# Guix Build Hanging Issue - Fixed

**Date:** 2025-10-22 (Updated: 2025-10-24)  
**Status:** ✅ FIXED (Updated with complete removal)

## Problem

The Guix build workflow was hanging indefinitely during the channel update phase with output like:

```
building CA certificate bundle...
listing Emacs sub-directories...
building fonts directory...
building directory of Info manuals...
building profile with 1 package...
hint: Consider installing the `glibc-locales' package and defining
`GUIX_LOCPATH', along these lines:
     guix install glibc-locales
     export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
See the "Application Setup" section in the manual, for more info.
Updating channel 'guix' from Git repository at 'https://git.savannah.gnu.org/git/guix.git'...
```

The process would hang at the "Updating channel..." step and never complete, causing CI builds to time out after the workflow's 60-minute limit.

## Root Cause

The issue occurred in the "Update Guix channels and profile" step of `.github/workflows/guix-build.yml`:

1. **Network Operations Without Timeout**: The `guix pull` command fetches updates from the remote Git repository at `https://git.savannah.gnu.org/git/guix.git`. This network operation had no timeout mechanism, allowing it to hang indefinitely if:
   - The network connection was slow or unstable
   - The Git server was experiencing issues
   - The download was interrupted but not properly terminated

2. **No Fallback Mechanism**: The workflow treated `guix pull` as a critical operation, but it's actually optional for most builds since the fresh Guix installation already includes the necessary modules.

3. **Resource Intensive Operations**: Without limiting parallel jobs, `guix pull` could consume excessive resources, further slowing down the process.

## Solution History

### Initial Fix (2025-10-22)
Added timeout protection and graceful degradation to prevent indefinite hanging:
- 5-minute timeout on `glibc-locales` installation
- 10-minute timeout on `guix pull`
- 5-minute timeout on `guix package -u`

### Final Fix (2025-10-24) - Complete Removal
**Even with timeouts, the update step was still causing issues and preventing the main build from running.**

The entire "Update Guix channels and profile" step has been **completely removed** from `.github/workflows/guix-build.yml`.

#### What was removed:
- `guix install glibc-locales` (with 5-minute timeout)
- `guix pull --max-jobs=2 --fallback` (with 10-minute timeout)
- `guix package -u` (with 5-minute timeout)
- All profile sourcing and GUIX_LOCPATH setup related to updates

#### Why this works:
- The fresh Guix installation already includes all necessary modules for the build
- The workflow proceeds directly from "Setup Guix environment" to "Verify Guix files"
- No network-dependent update operations that can hang or timeout
- Faster CI builds since no time is spent on updates

## Impact

These changes ensure that:

1. **No Hanging**: The problematic update step is completely removed
2. **Faster Builds**: No time wasted on network-dependent update operations
3. **Reliable CI**: Workflow always completes successfully without network-related failures
4. **Acceptable Builds**: The fresh Guix installation includes all necessary modules for building

## Testing

To verify the fix works:

1. The workflow should complete within the 60-minute timeout
2. No update operations should occur - workflow goes directly to verification and build
3. The build steps should execute using the fresh Guix installation
4. No network-related hanging or timeout messages

## Alternative Approaches Considered

1. **Add timeouts to update operations** (Previous approach): Still caused issues with timeouts
2. **Use substitutes only**: Faster, but may not always be available
3. **Cache Guix installation**: Complex to implement in GitHub Actions
4. **Longer timeouts**: Would still risk hanging, just for longer
5. **Complete removal** (Current approach): Most reliable, uses fresh installation which already has needed modules

The chosen approach prioritizes reliability (will never hang) and focuses on getting the main build working first.

## Related Files

- `.github/workflows/guix-build.yml` - Main workflow file (modified)
- `GUIX_BUILD_FIXES.md` - Previous fixes documentation
- `GUIX_CI_MODULE_FIX.md` - Module availability fixes
- `.guix/README.md` - Guix usage documentation

## Future Improvements

Potential enhancements (if channel updates are needed later):

1. **Cache mechanism**: Cache Guix channels between runs (complex to implement, may have limited benefit)
2. **Conditional updates**: Only update on schedule, not every build (adds workflow complexity)
3. **Separate workflow**: Have a dedicated workflow for updating Guix, separate from builds (better isolation but more maintenance)
4. **Network diagnostics**: Check connectivity before attempting updates (adds overhead)

For now, the focus is on getting the main build working reliably. These improvements can be considered if channel updates become necessary in the future, but each adds complexity that must be weighed against the benefit.

## Verification

The workflow should now show a clean progression:

```bash
# Setup steps
Install GNU Guix non-interactively (SSR safe)
Setup Guix environment
# Then directly to:
Verify Guix files
Build with Guix (dry-run)
Build with Guix (actual build - may be slow)
```

No update-related output or timeout warnings should appear.

The key success criterion is that the workflow **completes successfully** and the main build runs without hanging.

## Summary

✅ **Problem**: `guix pull` and update operations hanging/timing out, preventing main build  
✅ **Solution**: Completely removed the "Update Guix channels and profile" step from `.github/workflows/guix-build.yml`  
✅ **Impact**: Workflow can no longer hang on updates  
✅ **Trade-off**: No channel updates, but fresh installation has all needed modules  
✅ **Result**: Reliable CI builds that focus on the main build process
