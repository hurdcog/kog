# How to Apply Workflow Improvements

This directory contains the improved Guix build workflow that could not be automatically applied due to GitHub App permissions restrictions.

## Files in This Directory

- **`guix-build-original.yml`**: The current workflow from `.github/workflows/guix-build.yml`
- **`guix-build-improved.yml`**: The improved workflow with all enhancements
- **`APPLY_WORKFLOW_IMPROVEMENTS.md`**: This file

## Why Manual Application is Required

GitHub Apps (like the one used by Manus) require special `workflows` permission to modify workflow files in `.github/workflows/`. This is a security measure to prevent automated systems from modifying CI/CD pipelines without explicit authorization.

## Improvements in the New Workflow

The improved workflow includes:

1. **Two-Stage Build Process**
   - Fast syntax validation stage (10 minutes)
   - Full build stage (120 minutes) that only runs if validation passes

2. **Guix Store Caching**
   - Caches `/gnu/store` and `/var/guix` between runs
   - Significantly speeds up subsequent builds

3. **Increased Timeout**
   - Changed from 60 to 120 minutes for full builds
   - Prevents timeout failures on complex builds

4. **Artifact Preservation**
   - Uploads build outputs for download
   - Retains artifacts for 7 days

5. **Better Error Reporting**
   - Build status summary in GitHub Actions UI
   - Clear indication of validation vs. build failures

6. **Guile Installation**
   - Installs Guile in validation stage
   - Ensures syntax tests can run properly

## How to Apply

### Option 1: Manual Copy (Recommended)

1. **Review the differences**:
   ```bash
   diff workflow_improvements/guix-build-original.yml workflow_improvements/guix-build-improved.yml
   ```

2. **Copy the improved workflow**:
   ```bash
   cp workflow_improvements/guix-build-improved.yml .github/workflows/guix-build.yml
   ```

3. **Commit and push**:
   ```bash
   git add .github/workflows/guix-build.yml
   git commit -m "feat: Apply improved Guix build workflow

   - Add two-stage build process (validation + build)
   - Implement Guix store caching
   - Increase timeout to 120 minutes
   - Add artifact preservation
   - Improve error reporting
   - Fix Guile dependency for validation"
   git push origin main
   ```

### Option 2: GitHub Web Interface

1. Navigate to `.github/workflows/guix-build.yml` in the GitHub web interface
2. Click "Edit this file"
3. Copy the contents of `workflow_improvements/guix-build-improved.yml`
4. Paste into the editor, replacing all content
5. Commit with message: "feat: Apply improved Guix build workflow"

### Option 3: Pull Request

1. Create a new branch:
   ```bash
   git checkout -b improve-guix-workflow
   ```

2. Apply the changes:
   ```bash
   cp workflow_improvements/guix-build-improved.yml .github/workflows/guix-build.yml
   git add .github/workflows/guix-build.yml
   git commit -m "feat: Apply improved Guix build workflow"
   git push origin improve-guix-workflow
   ```

3. Create a pull request on GitHub
4. Review and merge

## Testing the New Workflow

After applying the changes:

1. **Trigger the workflow**:
   - Push a commit to `main` branch
   - Or manually trigger via GitHub Actions UI (workflow_dispatch)

2. **Monitor the validation stage**:
   - Should complete in ~5-10 minutes
   - Validates all Guix files with Guile

3. **Monitor the build stage**:
   - Only runs if validation passes
   - May take 60-120 minutes on first run
   - Subsequent runs should be faster due to caching

4. **Check artifacts**:
   - If build succeeds, artifacts will be available for download
   - Look for "guix-build-output" in the workflow run

## Rollback Instructions

If you need to revert to the original workflow:

```bash
cp workflow_improvements/guix-build-original.yml .github/workflows/guix-build.yml
git add .github/workflows/guix-build.yml
git commit -m "revert: Restore original Guix build workflow"
git push origin main
```

## Support

If you encounter issues:

1. Check the workflow run logs in GitHub Actions
2. Verify Guile is installed in the validation stage
3. Check that Guix daemon starts successfully
4. Review cache hit/miss status
5. Examine build artifacts if available

## Additional Resources

- [GitHub Actions Caching](https://docs.github.com/en/actions/using-workflows/caching-dependencies-to-speed-up-workflows)
- [Guix Manual](https://guix.gnu.org/manual/)
- [Workflow Analysis](../guix_workflow_analysis.md)
- [Cognitive Synergy Enhancements](../COGNITIVE_SYNERGY_ENHANCEMENTS.md)
