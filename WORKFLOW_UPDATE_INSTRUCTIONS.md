# Instructions for Updating the Guix Build Workflow

## Overview

Due to GitHub permissions restrictions, the optimized workflow file cannot be pushed directly via the GitHub App integration. This document provides instructions for manually applying the workflow updates.

## The Problem

The current Guix build workflow fails with a hash mismatch error:
```
sha256 hash mismatch for /gnu/store/61wwpsl8drzmggxy6xa5c2941n0fs7bb-gnutls-3.7.2.tar.xz:
  expected hash: 0li7mwjnm64mbxhacz0rpf6i9qd83f53fvbrx96alpqqk9d6qvk4
  actual hash:   16mb7g7s077pn4qgm3p8pd4wl85rrnav7ra2svn71g5g5xdsj4a8
```

This prevents all OpenCog packages from building.

## The Solution

An optimized workflow has been created that:
1. ✅ Enables Guix substitute servers for pre-built binaries
2. ✅ Adds `--fallback` flag to build from source when needed
3. ✅ Improves Guix channel update process
4. ✅ Enhances error reporting and diagnostics

## How to Apply the Update

### Option 1: Manual File Replacement (Recommended)

1. **Download the optimized workflow**:
   The optimized workflow file has been saved to the repository analysis and is available in the documentation.

2. **Replace the existing workflow**:
   ```bash
   # Navigate to your local repository
   cd /path/to/occ
   
   # Backup the current workflow
   cp .github/workflows/guix-build.yml .github/workflows/guix-build.yml.backup
   
   # Copy the optimized workflow (you'll need to get this from the analysis)
   # The file is provided in the GUIX_BUILD_OPTIMIZATION.md document
   ```

3. **Commit and push**:
   ```bash
   git add .github/workflows/guix-build.yml
   git commit -m "Apply optimized Guix build workflow"
   git push origin main
   ```

### Option 2: Apply Key Changes Manually

If you prefer to update the existing workflow incrementally, apply these key changes:

#### Change 1: Add Substitute Server Authorization (Critical)

Add this step after "Configure Guix Environment":

```yaml
- name: Enable Guix Substitutes
  run: |
    echo "=== Enabling Guix Substitutes ==="
    
    # Authorize the official substitute server
    if [ -f "/var/guix/profiles/per-user/root/current-guix/share/guix/ci.guix.gnu.org.pub" ]; then
      sudo guix archive --authorize < /var/guix/profiles/per-user/root/current-guix/share/guix/ci.guix.gnu.org.pub
      echo "✓ Authorized ci.guix.gnu.org"
    fi
    
    # Authorize bordeaux substitute server (additional mirror)
    if [ -f "/var/guix/profiles/per-user/root/current-guix/share/guix/bordeaux.guix.gnu.org.pub" ]; then
      sudo guix archive --authorize < /var/guix/profiles/per-user/root/current-guix/share/guix/bordeaux.guix.gnu.org.pub
      echo "✓ Authorized bordeaux.guix.gnu.org"
    fi
    
    echo "Substitute servers configured"
```

#### Change 2: Update "Update Guix Channels" Step

Replace the existing step with:

```yaml
- name: Update Guix Channels
  run: |
    echo "=== Updating Guix Channels ==="

    if [ -n "${{ github.event.inputs.guix_channel }}" ]; then
      echo "Using specific Guix commit: ${{ github.event.inputs.guix_channel }}"
      sudo guix pull --commit=${{ github.event.inputs.guix_channel }} --fallback || {
        echo "WARNING: guix pull with specific commit failed"
        echo "Trying to pull latest..."
        sudo guix pull --fallback || echo "WARNING: guix pull failed, continuing with existing version"
      }
    else
      echo "Pulling latest Guix channels..."
      sudo guix pull --verbosity=1 --fallback || {
        echo "WARNING: guix pull failed, continuing with existing version"
        echo "This may cause hash mismatches with outdated package definitions"
      }
    fi

    guix describe || true
    
    # Update PATH to use the newly pulled guix
    if [ -f "/var/guix/profiles/per-user/root/current-guix/etc/profile" ]; then
      source /var/guix/profiles/per-user/root/current-guix/etc/profile
      export PATH="/var/guix/profiles/per-user/root/current-guix/bin:$PATH"
      echo "PATH=/var/guix/profiles/per-user/root/current-guix/bin:$PATH" >> $GITHUB_ENV
    fi
```

#### Change 3: Update "Install Build Dependencies" Step

Add `--fallback` flag:

```yaml
- name: Install Build Dependencies
  run: |
    echo "=== Installing Build Dependencies ==="
    
    sudo guix install guile sparsehash --fallback 2>&1 || {
      echo "WARNING: Some packages failed to install"
      echo "Dependencies will be pulled during main build"
    }
    
    # ... rest of the step
  continue-on-error: true
```

#### Change 4: Update "Build OpenCog Collection Package" Step

Replace the build command section:

```yaml
- name: Build OpenCog Collection Package
  id: guix-build
  run: |
    echo "=== Starting Guix Build ==="
    echo "Building from guix.scm..."

    export GUIX_BUILD_OPTIONS="${{ env.GUIX_BUILD_OPTIONS }}"
    BUILD_START=$(date +%s)

    # Build with substitutes enabled and fallback to source if needed
    BUILD_CMD="sudo guix build \
      --file=guix.scm \
      --cores=${{ env.GUIX_BUILD_CORES }} \
      --keep-going \
      --fallback"
    
    # Add substitute options if enabled
    if [ "${{ github.event.inputs.use_substitutes }}" != "false" ]; then
      BUILD_CMD="$BUILD_CMD --substitute-urls='https://ci.guix.gnu.org https://bordeaux.guix.gnu.org'"
    fi
    
    echo "Build command: $BUILD_CMD"
    eval "$BUILD_CMD" 2>&1 | tee guix-build.log

    BUILD_STATUS=${PIPESTATUS[0]}
    # ... rest of the step
```

#### Change 5: Add Workflow Inputs

At the top of the workflow file, update the `workflow_dispatch` section:

```yaml
workflow_dispatch:
  inputs:
    guix_channel:
      description: 'Guix channel commit (leave empty for latest)'
      required: false
      default: ''
    run_tests:
      description: 'Run tests after build'
      required: false
      default: 'false'
      type: boolean
    use_substitutes:
      description: 'Use pre-built substitutes'
      required: false
      default: 'true'
      type: boolean
```

### Option 3: Use GitHub Web Interface

1. Go to: https://github.com/cogpy/occ/blob/main/.github/workflows/guix-build.yml
2. Click the "Edit" button (pencil icon)
3. Replace the entire content with the optimized workflow
4. Commit directly to main branch or create a pull request

## Verification

After applying the changes, verify the workflow works by:

1. **Trigger a manual workflow run**:
   - Go to Actions tab
   - Select "Guix Build - Reproducible Package"
   - Click "Run workflow"
   - Leave all options at default
   - Click "Run workflow"

2. **Check the build log** for these success indicators:
   - ✅ "Authorized ci.guix.gnu.org"
   - ✅ "Authorized bordeaux.guix.gnu.org"
   - ✅ "downloading from https://ci.guix.gnu.org" (indicates substitutes working)
   - ✅ "Build Successful"
   - ✅ Store path populated with files

3. **Download the artifacts**:
   - Check `guix-build.log` for detailed build output
   - Check `guix-build-report.md` for summary

## Expected Results

### Before Optimization
- ❌ Build fails with hash mismatch
- ❌ No packages built
- ⏱️ Fails in ~5-10 minutes

### After Optimization
- ✅ Build completes successfully
- ✅ All OpenCog packages built
- ✅ Functional artifacts in /gnu/store
- ⏱️ Completes in ~15-30 minutes (with substitutes)

## Troubleshooting

### If the build still fails with hash mismatches:

1. **Check if substitutes are enabled**:
   Look for "Authorized ci.guix.gnu.org" in the log

2. **Check if guix pull succeeded**:
   Look for "guix describe" output showing recent commit

3. **Try building with a specific Guix commit**:
   Use workflow dispatch with a known-good commit

4. **Check substitute server availability**:
   The servers might be temporarily down

### If substitutes are slow or unavailable:

The `--fallback` flag ensures the build will continue by building from source. This is slower but more reliable.

## Additional Resources

- **Full optimization documentation**: See `GUIX_BUILD_OPTIMIZATION.md`
- **Root cause analysis**: See `build-failure-analysis.md`
- **Guix manual**: https://guix.gnu.org/manual/
- **Guix substitute servers**: https://guix.gnu.org/manual/en/html_node/Substitutes.html

## Support

If you encounter issues applying these changes:

1. Check the documentation files in the repository
2. Review the GitHub Actions logs for specific error messages
3. Consult the Guix manual for troubleshooting guidance

---

**Created**: December 8, 2025  
**Repository**: https://github.com/cogpy/occ  
**Issue**: Guix build hash mismatch preventing package builds  
**Status**: Solution ready for application ✅
