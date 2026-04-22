# Quick Fix Guide: CI/CD Build Failure

**Problem**: Build fails with `E: Unable to locate package libacpica-dev`  
**Solution**: Replace workflow with version that builds dependencies from source  
**Time to Fix**: 2 minutes  

## Option 1: Automated Deployment (Recommended)

```bash
cd /path/to/occ
./deploy-workflow-fix.sh
```

This script will:
1. Backup the old workflow
2. Deploy the fixed workflow
3. Commit and push the changes
4. Provide next steps

## Option 2: Manual Deployment

### Step 1: Replace the workflow file

```bash
cd /path/to/occ
cp .github/workflows/agi-os-layers-build-fixed.yml .github/workflows/agi-os-layers-build.yml
```

### Step 2: Commit and push

```bash
git add .github/workflows/agi-os-layers-build.yml
git commit -m "fix(ci): Build MIG and ACPICA from source"
git push origin main
```

## Option 3: GitHub Web UI

1. Go to https://github.com/cogpy/occ
2. Navigate to `.github/workflows/agi-os-layers-build.yml`
3. Click the pencil icon (Edit)
4. Replace entire content with the content from `agi-os-layers-build-fixed.yml`
5. Commit directly to main branch

## Option 4: Create Pull Request

If you don't have direct push access:

```bash
cd /path/to/occ
git checkout -b fix/ci-workflow-dependencies
cp .github/workflows/agi-os-layers-build-fixed.yml .github/workflows/agi-os-layers-build.yml
git add .github/workflows/agi-os-layers-build.yml
git commit -m "fix(ci): Build MIG and ACPICA from source"
git push origin fix/ci-workflow-dependencies
```

Then create a PR on GitHub.

## What Gets Fixed

**Before** (Current State):
```
- name: Install Cognumach Dependencies
  run: |
    sudo apt-get install -y libacpica-dev mig
    # ❌ FAILS: Packages not found in Ubuntu 22.04
```

**After** (Fixed State):
```
jobs:
  build-mig:
    # ✅ Builds MIG from source
  
  build-acpica:
    # ✅ Builds ACPICA from source
  
  build-cognumach:
    needs: [build-mig, build-acpica]
    # ✅ Uses the built tools
```

## Verification

After deploying, verify the fix:

1. Go to https://github.com/cogpy/occ/actions
2. Find the latest workflow run
3. Check that these jobs appear and succeed:
   - ✅ Build MIG from Source
   - ✅ Build ACPICA from Source
   - ✅ Layer 1: Cognumach Microkernel
   - ✅ Layer 2: HurdCog Operating System
   - ✅ AGI-OS Integration Summary

## Expected Build Times

| Run Type | Duration |
|---|---|
| First run (no cache) | 15-28 minutes |
| Subsequent runs (cached) | 10-20 minutes |

The first run takes longer because it builds MIG and ACPICA from source, but subsequent runs are fast because the built tools are cached.

## Troubleshooting

### If the script fails with "Permission denied"

```bash
chmod +x deploy-workflow-fix.sh
./deploy-workflow-fix.sh
```

### If push is rejected

```bash
git pull --rebase origin main
git push origin main
```

### If you see "refusing to allow a GitHub App"

This means you're using a GitHub App token without workflow permissions. Use Option 3 (Web UI) or Option 4 (Pull Request) instead.

## Need Help?

If the fix doesn't work:
1. Check the GitHub Actions logs for the specific error
2. Verify that `agi-os-layers-build-fixed.yml` exists in your repository
3. Ensure you have push access to the main branch
4. Try Option 3 (Web UI) if automated methods fail

## Files Involved

- `.github/workflows/agi-os-layers-build.yml` - The workflow file to replace
- `.github/workflows/agi-os-layers-build-fixed.yml` - The corrected workflow
- `CI_CD_WORKFLOW_FIX.md` - Detailed technical documentation
- `deploy-workflow-fix.sh` - Automated deployment script
- `QUICK_FIX_GUIDE.md` - This file

## Summary

The fix is simple: replace one file. The corrected workflow builds MIG and ACPICA from source instead of trying to install them from Ubuntu's package repositories (where they don't exist). This makes the build portable across all Ubuntu versions and gives us full control over the tool versions.
