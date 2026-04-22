# Workflow Enhancements for guix-build.yml

**Note:** Due to GitHub App permissions restrictions, the workflow file cannot be automatically updated. These enhancements should be manually applied to `.github/workflows/guix-build.yml`.

---

## Recommended Enhancements

### 1. Increase Timeout

**Current:** `timeout-minutes: 60`  
**Recommended:** `timeout-minutes: 120`

**Reason:** Full OpenCog component builds may require more than 60 minutes, especially on first run when all dependencies need to be downloaded and compiled.

---

### 2. Add Locale Installation

Add this step after "Setup Guix environment":

```yaml
- name: Install locales
  run: |
    export PATH="/var/guix/profiles/per-user/$(whoami)/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    
    echo "Installing glibc-locales to avoid warnings..."
    guix install glibc-locales || true
```

**Reason:** Eliminates locale-related warnings that clutter the build output.

---

### 3. Enhance Build Verbosity

**Current:** `--verbosity=1`  
**Recommended:** `--verbosity=2`

**Change in "Build with Guix (actual build)" step:**

```yaml
guix build -f guix.scm --verbosity=2 --no-grafts
```

**Reason:** Provides more detailed information for debugging build issues.

---

### 4. Add Keep-Going Flag

**Change in "Build with Guix (actual build)" step:**

```yaml
guix build -f guix.scm --verbosity=2 --no-grafts --keep-going
```

**Reason:** Continues building other components even if one fails, providing more comprehensive error information.

---

### 5. Improve Error Handling

Replace the current error handling in "Build with Guix (actual build)":

```yaml
- name: Build with Guix (actual build)
  run: |
    export PATH="/var/guix/profiles/per-user/$(whoami)/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    export GUIX_PACKAGE_PATH="$GITHUB_WORKSPACE"
    
    echo "Attempting actual build..."
    guix build -f guix.scm --verbosity=2 --no-grafts --keep-going || {
      BUILD_EXIT_CODE=$?
      echo "Build failed with exit code: $BUILD_EXIT_CODE"
      echo "Checking build logs for details..."
      
      # Show last build log if available
      LAST_LOG=$(guix build -f guix.scm --log-file 2>/dev/null || echo "")
      if [ -n "$LAST_LOG" ]; then
        echo "Build log location: $LAST_LOG"
        if [ -f "$LAST_LOG" ]; then
          echo "=== Last 100 lines of build log ==="
          tail -100 "$LAST_LOG"
        fi
      fi
      
      exit $BUILD_EXIT_CODE
    }
```

**Reason:** Provides detailed error information and build logs when failures occur, instead of silently exiting with success.

---

### 6. Add Build Verification Step

Add this new step after "Build with Guix (actual build)":

```yaml
- name: Verify build output
  if: success()
  run: |
    export PATH="/var/guix/profiles/per-user/$(whoami)/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    
    echo "Build completed successfully!"
    echo "Checking installed files..."
    
    # Get the store path of the built package
    STORE_PATH=$(guix build -f guix.scm --no-grafts)
    echo "Package installed at: $STORE_PATH"
    
    # List installed files
    echo "=== Installed files ==="
    find "$STORE_PATH" -type f | head -50
    
    # Verify key components
    echo "=== Verifying key components ==="
    [ -d "$STORE_PATH/lib" ] && echo "✓ Libraries installed" || echo "✗ No libraries found"
    [ -d "$STORE_PATH/bin" ] && echo "✓ Binaries installed" || echo "✗ No binaries found"
    [ -d "$STORE_PATH/share" ] && echo "✓ Shared files installed" || echo "✗ No shared files found"
```

**Reason:** Confirms that the build actually produced the expected output files.

---

### 7. Add Installation Test Step

Add this new step after "Verify build output":

```yaml
- name: Test installation
  if: success()
  run: |
    export PATH="/var/guix/profiles/per-user/$(whoami)/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    
    echo "Testing package installation..."
    
    # Create a test profile and install the package
    TEST_PROFILE="$HOME/test-profile"
    guix package -p "$TEST_PROFILE" -f guix.scm
    
    echo "Package successfully installed to test profile!"
    echo "Profile contents:"
    ls -la "$TEST_PROFILE"
```

**Reason:** Validates that the package can be installed into a user profile, ensuring deployment readiness.

---

## Complete Enhanced Workflow File

A complete enhanced workflow file has been saved to: `.github/workflows/guix-build.yml.enhanced`

To apply the enhancements:

1. **Option A - Manual Edit:**
   - Open `.github/workflows/guix-build.yml` in your editor
   - Apply the changes listed above
   - Commit and push directly to main branch

2. **Option B - Replace File:**
   ```bash
   cp .github/workflows/guix-build.yml.enhanced .github/workflows/guix-build.yml
   git add .github/workflows/guix-build.yml
   git commit -m "Enhance Guix build workflow with better error handling and verification"
   git push origin main
   ```

---

## Why These Enhancements Matter

### Current Workflow Issues
1. **Silent Failures:** The current workflow exits with success even when builds fail
2. **Limited Debugging:** Low verbosity makes it hard to diagnose issues
3. **No Verification:** No confirmation that built artifacts are correct
4. **Timeout Risk:** 60 minutes may be insufficient for full builds

### After Enhancements
1. **Proper Error Reporting:** Build failures are clearly reported with logs
2. **Better Debugging:** Increased verbosity and log output aid troubleshooting
3. **Build Verification:** Confirms libraries, binaries, and shared files are installed
4. **Installation Testing:** Validates deployment readiness
5. **Sufficient Time:** 120-minute timeout accommodates full builds

---

## Testing the Enhanced Workflow

After applying the enhancements:

1. **Trigger a build:**
   - Push a commit to main branch
   - Or manually trigger the workflow from GitHub Actions tab

2. **Monitor the workflow:**
   - Watch for all steps to complete successfully
   - Verify that "Verify build output" shows installed files
   - Confirm "Test installation" creates a test profile

3. **Check for issues:**
   - If build fails, review the detailed error logs
   - Build log location will be displayed
   - Last 100 lines of build log will be shown

---

## Priority

**HIGH PRIORITY:** These enhancements should be applied as soon as possible to ensure:
- Reliable detection of build failures
- Comprehensive error reporting
- Verification of successful builds
- Full functional implementation without mock placeholders

The current workflow's silent failure mode (exit 0 on build failure) masks real issues and prevents proper validation of the build system.

---

**Created:** November 4, 2025  
**Status:** Pending Manual Application  
**Reason:** GitHub App lacks `workflows` permission
