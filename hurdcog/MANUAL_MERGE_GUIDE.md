# Manual Merge Guide: Checkout via Command Line

This guide provides step-by-step instructions for manually merging the `clone-me` branch into `master` when automatic merge cannot be performed due to conflicts.

## Overview

The `clone-me` branch contains significant automation and workflow enhancements for the Hurd project, including:
- New GitHub Actions workflows
- Python scripts for issue generation and roadmap creation
- Enhanced repository cloning capabilities
- Improved documentation structure

Due to the extensive changes, automatic merge conflicts occur in several files that require manual resolution.

## 🤖 Automated Merge Resolution

**NEW**: We now provide an automated workflow to resolve merge conflicts automatically!

### Auto-Resolve Workflow
- **Workflow**: `.github/workflows/auto-resolve-merge-conflicts.yml`
- **Triggers**: Manual dispatch, weekly schedule (Mondays 10 AM UTC)
- **Features**:
  - Automatically attempts to merge and resolve conflicts
  - Creates pull requests for successful merges
  - Creates GitHub issues for failed merges with detailed logs
  - Supports dry-run mode for testing

### Triggering the Automated Workflow
1. Go to the **Actions** tab in the GitHub repository
2. Select **"Auto-Resolve Merge Conflicts"** workflow
3. Click **"Run workflow"** and configure options:
   - **Target branch**: Branch to merge into clone-me (default: master)
   - **Create PR**: Whether to create a PR after successful merge
   - **Dry run**: Test mode without making actual changes

If the automated workflow fails, it will create a GitHub issue with detailed information and fallback to this manual guide.

## Prerequisites

- Git installed and configured
- Local clone of the repository
- Understanding of merge conflict resolution
- Text editor for resolving conflicts

## Step-by-Step Instructions

### Step 1: Clone the repository or update your local repository

```bash
git pull origin master
```

If you don't have the repository cloned yet:

```bash
git clone https://github.com/Unicorn-Dynamics/9nu.git
cd 9nu
```

### Step 2: Switch to the head branch of the pull request

```bash
git checkout clone-me
```

If the branch doesn't exist locally, create it from the remote:

```bash
git fetch origin clone-me:clone-me
git checkout clone-me
```

### Step 3: Merge the base branch into the head branch

```bash
git merge master --allow-unrelated-histories
```

**Note**: The `--allow-unrelated-histories` flag is required because the branches have diverged significantly.

### Step 4: Resolve the conflicts

The merge will fail with conflicts in these files:
- `.github/scripts/create_roadmap.py`
- `.github/scripts/generate_issues.py`
- `.github/scripts/requirements.txt`
- `.github/scripts/test_structure.py`
- `.github/workflows/generate-open-issues.yml`
- `Makefile`
- `clone-repos.sh`
- `external/README.md`

#### Conflict Resolution Strategy

For each conflicted file, you'll see conflict markers like:
```
<<<<<<< HEAD
# Content from clone-me branch
=======
# Content from master branch
>>>>>>> master
```

**Recommended Resolution Approach:**

1. **For Python scripts** (`.github/scripts/*.py`): 
   - Keep the version from `clone-me` branch (above `=======`)
   - These are new automation features that should be preserved

2. **For workflow files** (`.github/workflows/*.yml`):
   - Keep the version from `clone-me` branch
   - These are new GitHub Actions workflows

3. **For build files** (`Makefile`):
   - Carefully merge both versions, preserving existing functionality while adding new features
   - This requires understanding the specific changes in each branch

4. **For shell scripts** (`clone-repos.sh`):
   - Keep the enhanced version from `clone-me` branch
   - This includes improved error handling and security features

5. **For documentation** (`external/README.md`):
   - Merge both versions, combining documentation from both branches
   - Preserve all useful information

#### Manual Resolution Process

For each conflicted file:

1. Open the file in your text editor
2. Locate the conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`)
3. Choose the appropriate content based on the strategy above
4. Remove the conflict markers
5. Save the file
6. Mark the conflict as resolved: `git add <filename>`

#### Example Resolution

For a Python script conflict:
```bash
# Edit the file to resolve conflicts
nano .github/scripts/create_roadmap.py

# Remove conflict markers and keep the clone-me version
# Save and exit

# Mark as resolved
git add .github/scripts/create_roadmap.py
```

### Step 5: Commit the merge resolution

After resolving all conflicts:

```bash
# Verify all conflicts are resolved
git status

# Commit the merge
git commit -m "Merge master into clone-me - resolve conflicts

- Resolved conflicts in GitHub workflows and scripts
- Preserved automation enhancements from clone-me branch
- Merged documentation from both branches
- Updated build configuration"
```

### Step 6: Push the changes

```bash
git push -u origin clone-me
```

## Automated Conflict Resolution Script

For convenience, you can use the provided helper script:

```bash
./resolve-clone-me-conflicts.sh
```

This script will automatically resolve the common conflicts using the recommended strategy.

## Verification

After completing the merge:

1. **Test the build system:**
   ```bash
   make clean
   make
   ```

2. **Verify GitHub workflows:**
   - Check that workflow files are valid YAML
   - Ensure Python scripts run without syntax errors

3. **Test repository cloning:**
   ```bash
   ./clone-repos.sh
   ```

4. **Review the changes:**
   ```bash
   git log --oneline -10
   git diff HEAD~1
   ```

## Troubleshooting

### Common Issues

1. **"refusing to merge unrelated histories"**
   - Solution: Use `--allow-unrelated-histories` flag

2. **"pathspec 'clone-me' did not match any file(s)"**
   - Solution: Fetch the branch first: `git fetch origin clone-me:clone-me`

3. **Too many conflicts to resolve manually**
   - Solution: Use the automated resolution script
   - Alternative: Reset and apply changes incrementally

### Getting Help

- See [GitHub's merge conflict documentation](https://docs.github.com/pull-requests/collaborating-with-pull-requests/addressing-merge-conflicts/resolving-a-merge-conflict-using-the-command-line)
- Review the specific changes in each branch: `git log --oneline master..clone-me`
- Check the original PR discussion for context

## Important Notes

- Always backup your work before starting the merge process
- The merge adds significant new functionality - review all changes carefully
- Test thoroughly after merging to ensure everything works correctly
- The resulting merge will be large due to the extensive automation additions

## Related Documentation

- [Repository Cloning Guide](clone-repos.sh)
- [Development Pathways](DEVELOPMENT_PATHWAYS.md)
- [GitHub Workflows Documentation](.github/README.md)