# Guix Build Optimization Summary

## Date: December 8, 2025

## Executive Summary

The Guix build workflow has been **comprehensively optimized** to address the critical hash mismatch issue and improve overall build reliability. The primary issue was an outdated Guix channel causing hash mismatches for the `gnutls` dependency, which cascaded to prevent all OpenCog packages from building.

## Key Improvements

### 1. **Substitute Server Integration** ⭐ Most Important

**Problem**: Building everything from source was slow and prone to hash mismatches with outdated package definitions.

**Solution**: 
- Enabled official Guix substitute servers (ci.guix.gnu.org and bordeaux.guix.gnu.org)
- Pre-built binaries are now downloaded instead of building from source
- Significantly reduces build time and avoids compilation errors

**Implementation**:
```yaml
- name: Enable Guix Substitutes
  run: |
    sudo guix archive --authorize < /var/guix/profiles/per-user/root/current-guix/share/guix/ci.guix.gnu.org.pub
    sudo guix archive --authorize < /var/guix/profiles/per-user/root/current-guix/share/guix/bordeaux.guix.gnu.org.pub
```

### 2. **Improved Guix Channel Updates**

**Problem**: The `guix pull` step was failing silently, leaving the system with outdated package definitions that had incorrect hashes.

**Solution**:
- Added `--fallback` flag to build from source if substitutes fail
- Better error handling and logging
- Explicit PATH updates after pulling new Guix version
- Option to pin to specific Guix commit for reproducibility

**Implementation**:
```yaml
- name: Update Guix Channels
  run: |
    sudo guix pull --verbosity=1 --fallback || {
      echo "WARNING: guix pull failed, continuing with existing version"
      echo "This may cause hash mismatches with outdated package definitions"
    }
    # Update PATH to use newly pulled guix
    source /var/guix/profiles/per-user/root/current-guix/etc/profile
    export PATH="/var/guix/profiles/per-user/root/current-guix/bin:$PATH"
    echo "PATH=/var/guix/profiles/per-user/root/current-guix/bin:$PATH" >> $GITHUB_ENV
```

### 3. **Fallback Build Strategy**

**Problem**: If substitutes are unavailable or have hash mismatches, the build would fail completely.

**Solution**:
- Added `--fallback` flag to all build commands
- If pre-built binaries fail, Guix will automatically build from source
- Provides resilience against substitute server issues

**Implementation**:
```bash
sudo guix build \
  --file=guix.scm \
  --cores=2 \
  --keep-going \
  --fallback \
  --substitute-urls='https://ci.guix.gnu.org https://bordeaux.guix.gnu.org'
```

### 4. **Enhanced Dependency Installation**

**Problem**: Dependencies were being installed without fallback options, causing failures.

**Solution**:
- Added `--fallback` to dependency installation
- Made dependency installation step continue-on-error
- Dependencies will be pulled during main build if pre-installation fails

### 5. **Better Error Reporting**

**Problem**: Build failures didn't provide enough context for debugging.

**Solution**:
- Enhanced build logs with more diagnostic information
- Added Guix version and description to build output
- Improved build report with last 50 lines of log
- Better GitHub step summaries with emojis and formatting

### 6. **Workflow Input Options**

**Problem**: No flexibility to control build behavior.

**Solution**: Added workflow dispatch inputs:
- `guix_channel`: Pin to specific Guix commit
- `run_tests`: Enable/disable post-build testing
- `use_substitutes`: Control substitute usage

### 7. **Improved Caching**

**Problem**: Cache key wasn't versioned, could cause issues with incompatible cached data.

**Solution**:
- Added version suffix to cache key (`-v2`)
- Better restore-keys fallback pattern

## Technical Details

### Root Cause Analysis

The build was failing with:
```
sha256 hash mismatch for /gnu/store/61wwpsl8drzmggxy6xa5c2941n0fs7bb-gnutls-3.7.2.tar.xz:
  expected hash: 0li7mwjnm64mbxhacz0rpf6i9qd83f53fvbrx96alpqqk9d6qvk4
  actual hash:   16mb7g7s077pn4qgm3p8pd4wl85rrnav7ra2svn71g5g5xdsj4a8
```

This occurred because:
1. The Guix installation was using an old channel definition
2. The old definition had an outdated hash for gnutls-3.7.2
3. When trying to download, the actual file had a different hash
4. Guix security model rejects hash mismatches to prevent tampering

### Solution Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Guix Build Workflow                       │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  1. Install Guix                                             │
│     ├─ Official installer (with fallback)                    │
│     └─ Manual installation if needed                         │
│                                                               │
│  2. Enable Substitutes ⭐                                    │
│     ├─ Authorize ci.guix.gnu.org                            │
│     └─ Authorize bordeaux.guix.gnu.org                      │
│                                                               │
│  3. Update Channels (with --fallback)                        │
│     ├─ Pull latest package definitions                       │
│     └─ Update PATH to use new Guix                          │
│                                                               │
│  4. Install Dependencies (with --fallback)                   │
│     ├─ guile                                                 │
│     └─ sparsehash                                            │
│                                                               │
│  5. Build Package (with --fallback + substitutes)            │
│     ├─ Try substitutes first (fast)                         │
│     ├─ Fall back to source build if needed                  │
│     └─ Keep going on non-critical failures                  │
│                                                               │
│  6. Generate Reports                                         │
│     ├─ Build log                                             │
│     ├─ Build report                                          │
│     └─ GitHub step summary                                   │
│                                                               │
└─────────────────────────────────────────────────────────────┘
```

## Expected Outcomes

### Before Optimization
- ❌ Build fails at dependency download stage
- ❌ Hash mismatch errors for gnutls
- ❌ No OpenCog packages built
- ❌ No functional artifacts produced
- ⏱️ Build time: ~5-10 minutes (before failure)

### After Optimization
- ✅ Substitutes downloaded successfully
- ✅ Hash mismatches resolved via updated channels
- ✅ All OpenCog packages build successfully
- ✅ Functional artifacts in /gnu/store
- ⏱️ Build time: ~15-30 minutes (with substitutes)
- ⏱️ Build time: ~60-120 minutes (if building from source)

## Build Components

The following OpenCog components should now build successfully:

### Core Components
1. **CogUtil** - Base utilities and configuration system
2. **CogGML** - Self-aware microkernel for cognitive processing
3. **AtomSpace** - Hypergraph database and knowledge representation
4. **AtomSpace Accelerator** - High-performance inference engine
5. **CogServer** - Networking and inter-process communication
6. **Matrix** - Sparse matrix and graph processing
7. **Learn** - Symbolic learning algorithms and pattern mining
8. **Agents** - Interactive cognitive agents framework
9. **Sensory** - Dataflow system for external world interaction
10. **Agentic Chatbots** - Conversational AI integration
11. **CogSelf** - AGI cognitive synergy framework

### Build Order
The CMakeLists.txt ensures proper dependency order:
```
CogUtil → CogGML → AtomSpace → [AtomSpace Accelerator, CogServer, Matrix, Learn, Agents, Sensory, Agentic Chatbots, CogSelf]
```

## Testing the Optimization

### Local Testing
```bash
# Clone the repository
git clone https://github.com/cogpy/occ.git
cd occ

# Ensure you have Guix installed
guix --version

# Update Guix channels
guix pull

# Build with optimized settings
guix build -f guix.scm --fallback --substitute-urls='https://ci.guix.gnu.org https://bordeaux.guix.gnu.org'
```

### CI Testing
The workflow will automatically run on:
- Push to `main` or `master` branches
- Pull requests to `main` or `master` branches
- Manual workflow dispatch

### Manual Workflow Dispatch
You can trigger the workflow manually with custom options:
1. Go to Actions tab in GitHub
2. Select "Guix Build - Reproducible Package"
3. Click "Run workflow"
4. Configure options:
   - Guix channel commit (optional)
   - Run tests (true/false)
   - Use substitutes (true/false)

## Monitoring and Validation

### Success Indicators
1. ✅ "Enable Guix Substitutes" step completes successfully
2. ✅ "Update Guix Channels" step completes (may show warnings but should not fail)
3. ✅ "Build OpenCog Collection Package" step completes with status=success
4. ✅ Build artifacts uploaded (guix-build.log, guix-build-report.md)
5. ✅ Store path populated with binaries, libraries, and includes

### Failure Indicators
1. ❌ Hash mismatch errors in build log
2. ❌ "cannot build derivation" errors
3. ❌ Build status: failed
4. ❌ No store path in output

### Log Analysis
Check the build log for:
- "downloading from https://ci.guix.gnu.org" - indicates substitutes are working
- "building /gnu/store/..." - indicates building from source (slower but acceptable)
- "hash mismatch" - indicates the optimization didn't fully resolve the issue

## Rollback Plan

If the optimized workflow causes issues:

1. **Revert to previous workflow**:
   ```bash
   git checkout HEAD~1 .github/workflows/guix-build.yml
   git commit -m "Revert guix-build.yml to previous version"
   git push
   ```

2. **Disable substitutes**:
   - Set `use_substitutes: false` in workflow dispatch
   - Or remove substitute-related steps

3. **Pin to specific Guix commit**:
   - Use workflow dispatch with known-good commit
   - Or hardcode in workflow file

## Future Improvements

### Short-term
1. Add automated testing of built packages
2. Create binary cache for faster builds
3. Add build time metrics and tracking

### Medium-term
1. Set up custom substitute server for OpenCog packages
2. Implement multi-architecture builds (x86_64, aarch64)
3. Add integration tests for cognitive synergy

### Long-term
1. Create Guix channel for OpenCog packages
2. Upstream OpenCog packages to official Guix repository
3. Implement continuous deployment pipeline

## Conclusion

The optimized Guix build workflow addresses the critical hash mismatch issue through:
1. **Substitute server integration** - Downloads pre-built binaries
2. **Improved channel updates** - Ensures latest package definitions
3. **Fallback strategy** - Builds from source if substitutes fail
4. **Better error handling** - Provides clear diagnostics

These changes should result in a **fully functional build** that produces all OpenCog packages without errors or mock placeholders.

## Next Steps

1. **Commit and push the changes**:
   ```bash
   git add .github/workflows/guix-build.yml
   git commit -m "Optimize Guix build workflow: fix hash mismatches and enable substitutes"
   git push origin main
   ```

2. **Monitor the build**:
   - Watch the GitHub Actions run
   - Check for successful completion
   - Verify artifacts are produced

3. **Validate the build**:
   - Download the build artifacts
   - Check the store path contents
   - Run any available tests

4. **Document the success**:
   - Update README with build status badge
   - Document the build process
   - Share results with the team

---

**Optimization completed by**: Manus AI Agent  
**Date**: December 8, 2025  
**Repository**: https://github.com/cogpy/occ  
**Status**: Ready for testing ✅
