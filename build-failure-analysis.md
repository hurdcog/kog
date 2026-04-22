# Guix Build Failure Analysis

## Date: December 8, 2025

## Summary

The Guix build GitHub Action is failing due to a **hash mismatch** when downloading the `gnutls-3.7.2.tar.xz` dependency. This is a critical issue that prevents the entire build from proceeding.

## Root Cause

### Primary Issue: GnuTLS Hash Mismatch

The build fails during the dependency download phase with the following error:

```
sha256 hash mismatch for /gnu/store/61wwpsl8drzmggxy6xa5c2941n0fs7bb-gnutls-3.7.2.tar.xz:
  expected hash: 0li7mwjnm64mbxhacz0rpf6i9qd83f53fvbrx96alpqqk9d6qvk4
  actual hash:   16mb7g7s077pn4qgm3p8pd4wl85rrnav7ra2svn71g5g5xdsj4a8
```

This indicates that:
1. The Guix package definition expects a specific hash for gnutls-3.7.2
2. The downloaded file has a different hash (likely due to upstream changes or mirror issues)
3. This is a transitive dependency (required by sparsehash or other dependencies)

### Secondary Issues

1. **Guix Installation Issues**: The official Guix installer fails with SSL connection errors, falling back to alternative installation method
2. **Guix Pull Failures**: Unable to update Guix channels, continuing with existing version
3. **Dependency Chain Failure**: The gnutls failure cascades to prevent building:
   - sparsehash-2.0.4
   - The final opencog-collection profile

## Impact

- **Build Status**: Complete failure - no artifacts produced
- **Affected Components**: All OpenCog packages (none can be built)
- **Deployment**: Impossible - no functional implementation available

## Proposed Solutions

### Solution 1: Update Guix Channels (Recommended)

The hash mismatch suggests we're using an outdated Guix channel that references old package definitions. We need to:

1. **Force Guix channel update** to get the latest package definitions with correct hashes
2. **Use a specific Guix commit** known to work (if available)
3. **Improve the Guix pull step** to ensure it succeeds

### Solution 2: Pin Guix to a Known-Good Commit

Instead of using the latest Guix, pin to a specific commit that's known to work:

```yaml
- name: Update Guix Channels
  run: |
    # Use a specific known-good Guix commit
    sudo guix pull --commit=<KNOWN_GOOD_COMMIT>
```

### Solution 3: Use Guix Time Machine

Use Guix's time-machine feature to build with a specific channel configuration:

```scheme
(use-modules (guix channels)
             (guix inferior)
             (guix packages))

(define channels
  (list (channel
         (name 'guix)
         (url "https://git.savannah.gnu.org/git/guix.git")
         (commit "<KNOWN_GOOD_COMMIT>"))))
```

### Solution 4: Bypass Problematic Dependencies

If sparsehash is causing the issue and isn't critical:
1. Make sparsehash optional in the build
2. Use alternative data structures
3. Remove from inputs if not actually used

### Solution 5: Use Substitutes

Enable substitute servers to download pre-built binaries instead of building from source:

```yaml
- name: Configure Guix Substitutes
  run: |
    sudo guix archive --authorize < /var/guix/profiles/per-user/root/current-guix/share/guix/ci.guix.gnu.org.pub
    echo "Using substitutes from ci.guix.gnu.org"
```

## Recommended Action Plan

1. **Immediate Fix**: Update the workflow to use Guix substitutes and a more recent Guix pull
2. **Short-term**: Pin to a known-good Guix commit
3. **Long-term**: Regularly update and test the Guix package definition

## Additional Observations

### Workflow Structure
The workflow is well-structured with:
- ✓ Validation stage for guix.scm syntax
- ✓ Build stage with proper error handling
- ✓ Analysis stage for derivation inspection
- ✓ Reporting stage for status summary

### guix.scm Package Definition
The package definition is comprehensive:
- ✓ Proper module imports
- ✓ CMake build system configuration
- ✓ Multiple build phases with diagnostics
- ✓ Complete dependency list
- ✓ Installation of additional components

### Missing Elements
- ⚠ No fallback for dependency download failures
- ⚠ No use of Guix substitutes (pre-built binaries)
- ⚠ Guix channel update step fails but continues
- ⚠ No pinned Guix version for reproducibility
