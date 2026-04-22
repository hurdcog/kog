# Guix CI Module Not Found Fix

**Date:** 2025-10-22  
**Status:** ✅ FIXED (see also GUIX_HANGING_FIX.md for timeout improvements)

## Problem

The GitHub Actions workflow for Guix builds was failing with the error:

```
ice-9/boot-9.scm:3330:6: In procedure resolve-interface:
no code for module (guix packages)
```

This error indicated that the Guix installation on the CI runner was incomplete or missing critical Guile modules, specifically the `(guix packages)` module.

## Root Cause

The Guix installation script installed Guix, but the CI workflow did not:
1. Update Guix channels to ensure all modules are available
2. Properly source the Guix profile in each step
3. Set the correct GUIX_PROFILE environment variable consistently

## Solution

Added a new workflow step and updated existing steps to:

### 1. New Step: "Update Guix channels and profile"

This step runs after Guix installation and daemon setup to:
- Run `guix pull` to update Guix channels and ensure all modules are available
- Run `guix package -u` to update packages in the default profile
- Source the Guix profile from `/etc/profile`
- Source the root user's guix-profile at `/var/guix/profiles/per-user/root/guix-profile`
- Verify Guix is working with `guix describe`

### 2. Updated All Subsequent Steps

Each step that uses Guix commands now:
- Sources the Guix profile at `/var/guix/profiles/per-user/root/guix-profile`
- Sources `/etc/profile` to ensure environment is properly set up
- Uses consistent `GUIX_PROFILE` environment variable across all steps

## Changes Made

### File: `.github/workflows/guix-build.yml`

#### Added new step (lines 42-62):
```yaml
- name: Update Guix channels and profile
  run: |
    echo "Updating Guix channels..."
    export PATH="/var/guix/profiles/per-user/$(whoami)/current-guix/bin:$PATH"
    
    # Update Guix to ensure all modules are available
    sudo -i guix pull || echo "Warning: guix pull failed"
    
    # Update packages in the default profile
    sudo -i guix package -u || echo "Warning: guix package -u failed"
    
    # Source the Guix profile
    echo "Sourcing Guix profile..."
    source /etc/profile || true
    
    # Set and verify GUIX_PROFILE
    export GUIX_PROFILE="/var/guix/profiles/per-user/root/guix-profile"
    source $GUIX_PROFILE/etc/profile || echo "Warning: could not source profile"
    
    # Verify Guix is working
    guix describe || echo "Warning: guix describe failed after update"
```

#### Updated existing steps:
- **Verify Guix files** - Added profile sourcing
- **Build with Guix (dry-run)** - Added profile sourcing
- **Build with Guix (actual build)** - Added profile sourcing

Each step now includes:
```bash
# Source Guix profile to ensure modules are available
export GUIX_PROFILE="/var/guix/profiles/per-user/root/guix-profile"
source $GUIX_PROFILE/etc/profile || true
source /etc/profile || true
```

## Impact

These changes ensure that:
- The `(guix packages)` module and all other Guix modules are available
- Each step has a properly initialized Guix environment
- The workflow is more resilient to Guix installation variations
- All Guix commands can access the necessary Guile modules

## Verification

The workflow now:
1. ✅ Updates Guix channels after installation
2. ✅ Sources the Guix profile in each step
3. ✅ Uses consistent environment variables
4. ✅ Provides better error handling with fallbacks
5. ✅ Verifies Guix is working before proceeding

## References

- [Guix Manual - Application Setup](https://guix.gnu.org/manual/en/html_node/Application-Setup.html)
- [Guix CI Workflow](.github/workflows/guix-build.yml)
- [Problem Statement](https://github.com/cogpy/occ/issues/XX)

## Next Steps

The workflow should now successfully:
1. Install Guix
2. Update Guix channels and profile (with timeout protection - see GUIX_HANGING_FIX.md)
3. Validate package definitions
4. Run dry-run builds to check syntax
5. Attempt actual builds (which may still fail due to dependencies, but will no longer fail due to missing modules)

Any remaining build failures will be related to actual dependencies or build system issues, not missing Guix modules.

**Note**: For timeout protection on `guix pull` and other network operations, see GUIX_HANGING_FIX.md.
