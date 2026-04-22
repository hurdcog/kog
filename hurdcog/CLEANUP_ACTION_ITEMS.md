# Repository Cleanup Action Items

## Immediate Actions Required

### 1. Remove Financial Components ❌
- [ ] Delete `.github/workflows/financial-intelligence-engine.yml`
- [ ] Clean `.github/workflows/cogsplit.yml` (extract cognitive parts, remove financial)
- [ ] Fix `.github/workflows/ci-tests.yml` (replace GnuCash with Hurd-specific tests)
- [ ] Remove all financial references from documentation

### 2. Separate ElizaOS Components ⚠️
- [ ] Create separate repository for ElizaOS components
- [ ] Move ElizaOS-related files to new repository
- [ ] Update documentation to remove ElizaOS references
- [ ] Clean up any remaining ElizaOS dependencies

### 3. Reorganize Documentation 📚
- [ ] Move cognitive docs to `cognitive/docs/`
- [ ] Move distributed system docs to `distributed/docs/`
- [ ] Move performance docs to `performance/docs/`
- [ ] Move development docs to `development/docs/`
- [ ] Move build system docs to `build/docs/`
- [ ] Update all documentation links

### 4. Clean GitHub Actions 🔧
- [ ] Remove financial intelligence engine workflow
- [ ] Clean cogsplit workflow (keep cognitive, remove financial)
- [ ] Fix ci-tests workflow (Hurd-specific tests)
- [ ] Update workflow documentation

### 5. Update Core Files 📝
- [ ] Replace README.md with README_CLEAN.md
- [ ] Update DEVELOPMENT_ROADMAP.md
- [ ] Clean up any remaining financial references
- [ ] Update project description and tags

## Verification Checklist

### Before Committing Changes
- [ ] No financial/trading references remain
- [ ] No ElizaOS components in main repository
- [ ] All documentation properly organized
- [ ] GitHub Actions workflows cleaned
- [ ] Build system functional
- [ ] Tests pass
- [ ] Documentation links updated

### After Cleanup
- [ ] Repository focuses on GNU Hurd + Cognitive Architecture
- [ ] Clear separation of concerns
- [ ] Proper documentation structure
- [ ] Clean development workflow
- [ ] Community guidelines updated

## Notes

- All removed files are backed up in `backup/` directory
- Review backup before permanent deletion
- Consider creating separate repositories for removed components
- Update all documentation to reflect new structure
