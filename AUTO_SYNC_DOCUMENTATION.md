# Auto-Sync Repository Documentation

## Overview

The OCC repository implements an automated synchronization mechanism to keep multiple repository instances in sync. This ensures that changes pushed to the source repository (`o9nn/occ`) are automatically propagated to the target repository (`cogpy/occ`).

## Architecture

### Workflow Files

#### 1. Basic Auto-Sync (`auto-sync.yml`)
**Status:** Active  
**Purpose:** Simple, reliable synchronization after successful builds

**Features:**
- Triggers after successful Windows builds
- Daily scheduled sync at 00:00 UTC
- Manual dispatch option
- Force-with-lease push for safety
- Tag synchronization

**Triggers:**
- `workflow_run`: After "OCC Windows Build" or "OCC Build - Windows Native (wincog)" completes successfully
- `workflow_dispatch`: Manual trigger
- `schedule`: Daily at midnight UTC

#### 2. Enhanced Auto-Sync (`auto-sync-enhanced.yml`)
**Status:** Available (not yet active)  
**Purpose:** Advanced synchronization with conflict detection and reporting

**Features:**
- Comprehensive difference analysis
- Conflict detection and resolution
- Dry run mode
- Detailed sync reports
- Fast-forward detection
- Rollback capability
- GitHub Actions summary integration

**Additional Triggers:**
- `force_sync` input: Force sync even if builds failed
- `dry_run` input: Preview changes without pushing

## Authentication

### Git PAT (Personal Access Token)

**Secret Name:** `git_pat`  
**Alias:** `magoo`  
**Scope Required:**
- `repo` (Full control of private repositories)
- `workflow` (Update GitHub Action workflows)

**Usage in Workflows:**
```yaml
git remote add target https://${{ secrets.git_pat }}@github.com/${{ env.TARGET_REPO }}.git
```

### Security Considerations

1. **Token Rotation:** Rotate PAT every 90 days
2. **Minimal Scope:** Use only required permissions
3. **Audit Logs:** Monitor token usage in GitHub audit logs
4. **Secret Protection:** Never log or expose the token value

## Sync Process

### Basic Sync Flow

```
1. Trigger Event (build success, schedule, manual)
   ↓
2. Checkout source repository (o9nn/occ)
   ↓
3. Configure Git with bot credentials
   ↓
4. Add target remote with PAT authentication
   ↓
5. Fetch target repository state
   ↓
6. Check for differences
   ↓
7. Push changes if differences found
   ↓
8. Sync tags
   ↓
9. Report status
```

### Enhanced Sync Flow

```
1. Trigger Event
   ↓
2. Checkout source repository
   ↓
3. Configure Git
   ↓
4. Add target remote
   ↓
5. Fetch target repository
   ↓
6. Analyze differences
   ├─ Count commits ahead/behind
   ├─ Detect conflicts
   ├─ Check fast-forward capability
   └─ Generate change summary
   ↓
7. Generate sync report
   ↓
8. Sync changes (if no conflicts)
   ├─ Push commits
   └─ Push tags
   ↓
9. Handle conflicts (if any)
   ↓
10. Upload report artifact
    ↓
11. Create GitHub summary
```

## Configuration

### Environment Variables

```yaml
env:
  SOURCE_REPO: o9nn/occ        # Source repository
  TARGET_REPO: cogpy/occ       # Target repository
  SYNC_BRANCH: main            # Branch to sync
```

### Customization

To sync different repositories or branches:

1. Update environment variables in workflow file
2. Ensure PAT has access to both repositories
3. Test with dry run mode first

## Usage

### Manual Sync

#### Basic Workflow
```bash
# Via GitHub CLI
gh workflow run auto-sync.yml

# Via GitHub Web UI
# 1. Go to Actions tab
# 2. Select "Auto Sync Repository"
# 3. Click "Run workflow"
```

#### Enhanced Workflow
```bash
# Normal sync
gh workflow run auto-sync-enhanced.yml

# Dry run (preview only)
gh workflow run auto-sync-enhanced.yml -f dry_run=true

# Force sync (even if builds failed)
gh workflow run auto-sync-enhanced.yml -f force_sync=true
```

### Automatic Sync

Sync happens automatically:
- After successful Windows builds
- Daily at 00:00 UTC
- Can be triggered manually anytime

## Monitoring

### Check Sync Status

```bash
# List recent sync runs
gh run list --workflow=auto-sync.yml --limit 10

# View specific run
gh run view <run-id>

# View logs
gh run view <run-id> --log
```

### Sync Reports

Enhanced workflow generates detailed reports:
- Uploaded as workflow artifacts
- Retained for 30 days
- Contains commit-by-commit changes
- Includes conflict information

**Download Report:**
```bash
gh run download <run-id> --name sync-report-<run-number>
```

## Conflict Resolution

### Automatic Resolution

The enhanced workflow can handle:
- Fast-forward merges (no conflicts)
- Simple ahead-only scenarios

### Manual Resolution Required

When automatic resolution fails:

1. **Clone both repositories:**
   ```bash
   git clone https://github.com/o9nn/occ.git occ-source
   git clone https://github.com/cogpy/occ.git occ-target
   ```

2. **Add source as remote in target:**
   ```bash
   cd occ-target
   git remote add source https://github.com/o9nn/occ.git
   git fetch source
   ```

3. **Merge changes:**
   ```bash
   git merge source/main
   ```

4. **Resolve conflicts:**
   ```bash
   # Edit conflicting files
   git add <resolved-files>
   git commit
   ```

5. **Push to target:**
   ```bash
   git push origin main
   ```

### Force Sync (Use with Caution)

To overwrite target with source:

```bash
# Via enhanced workflow
gh workflow run auto-sync-enhanced.yml -f force_sync=true

# Or manually
cd occ-source
git remote add target https://${git_pat}@github.com/cogpy/occ.git
git push target main:main --force
```

⚠️ **Warning:** Force push will overwrite target history. Use only when necessary.

## Troubleshooting

### Common Issues

#### Issue: "Sync failed - no changes detected"
**Cause:** Repositories already in sync  
**Solution:** No action needed, this is normal

#### Issue: "Cannot fast-forward"
**Cause:** Target has commits not in source  
**Solution:** Use manual conflict resolution or force sync

#### Issue: "Authentication failed"
**Cause:** Invalid or expired PAT  
**Solution:** Update `git_pat` secret with valid token

#### Issue: "Target branch not found"
**Cause:** Branch doesn't exist in target  
**Solution:** Create branch in target repository first

#### Issue: "Workflow not triggering"
**Cause:** Build didn't complete successfully  
**Solution:** Fix build errors or use manual trigger

### Debug Mode

Enable debug logging:

```yaml
# Add to workflow file
env:
  ACTIONS_STEP_DEBUG: true
  ACTIONS_RUNNER_DEBUG: true
```

## Best Practices

### 1. Regular Monitoring
- Check sync status weekly
- Review sync reports for unexpected changes
- Monitor for conflict patterns

### 2. Conflict Prevention
- Avoid making changes directly in target repository
- Use source repository as single source of truth
- Document any manual interventions

### 3. Testing
- Use dry run mode before important syncs
- Test workflow changes in a fork first
- Verify sync after major repository changes

### 4. Security
- Rotate PAT regularly
- Audit sync logs for unauthorized access
- Use branch protection rules on target

## Advanced Configuration

### Bidirectional Sync

To sync changes in both directions:

```yaml
# Add reverse sync job
reverse-sync:
  runs-on: ubuntu-latest
  if: github.event_name == 'schedule'
  steps:
    # Similar steps but swap SOURCE_REPO and TARGET_REPO
```

### Selective Sync

To sync only specific paths:

```yaml
- name: Sync specific directories
  run: |
    git checkout target/main -- specific/directory
    git push target main:main
```

### Multi-Branch Sync

To sync multiple branches:

```yaml
strategy:
  matrix:
    branch: [main, develop, release]
```

## Integration with CI/CD

### Sync After Successful Builds

```yaml
on:
  workflow_run:
    workflows: ["Build", "Test", "Package"]
    types: [completed]
    branches: [main]
```

### Sync Before Deployment

```yaml
deploy:
  needs: sync-repository
  runs-on: ubuntu-latest
  steps:
    # Deployment steps
```

## Metrics and Analytics

### Track Sync Performance

- Sync frequency
- Success rate
- Conflict rate
- Average sync time
- Data transferred

### Generate Reports

```bash
# Sync history
gh run list --workflow=auto-sync.yml --json conclusion,createdAt,databaseId

# Success rate
gh run list --workflow=auto-sync.yml --json conclusion | \
  jq '[.[] | .conclusion] | group_by(.) | map({status: .[0], count: length})'
```

## Future Enhancements

### Planned Features
1. **Webhook Integration** - Real-time sync on push
2. **Multi-Target Sync** - Sync to multiple repositories
3. **Selective File Sync** - Sync specific files or directories
4. **Conflict Resolution Bot** - Automated conflict resolution
5. **Sync Analytics Dashboard** - Visual sync metrics
6. **Rollback Automation** - One-click rollback on sync failure

### Experimental Features
1. **AI-Assisted Merge** - Use AI to resolve conflicts
2. **Predictive Sync** - Predict and prevent conflicts
3. **Sync Optimization** - Reduce sync time and bandwidth

## Support and Contribution

### Getting Help
- **GitHub Issues:** https://github.com/cogpy/occ/issues
- **Label:** `sync`, `automation`
- **Documentation:** This file

### Contributing
To improve the sync mechanism:
1. Fork the repository
2. Create a feature branch
3. Test thoroughly
4. Submit pull request
5. Update documentation

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Git Documentation](https://git-scm.com/doc)
- [GitHub Personal Access Tokens](https://docs.github.com/en/authentication/keeping-your-account-and-data-secure/creating-a-personal-access-token)

---

**Last Updated:** 2025-12-23  
**Maintainer:** OCC Build Team  
**Version:** 2.0
