# AGI-OS GitHub Actions Workflows

This directory contains comprehensive CI/CD workflows for building, testing, and deploying the AGI Operating System.

## Workflows Overview

### 1. AGI-OS Debian Package Build (`agi-os-debian-build.yml`)

**Purpose**: Build all Debian packages for the AGI-OS stack.

**Triggers**:
- Push to `main` or `develop` branches (when relevant files change)
- Pull requests to `main` or `develop`
- Manual workflow dispatch with stage selection

**Stages**:
- **Stage 0**: Cognumach Microkernel (optional)
- **Stage 1**: Foundation (cogutil)
- **Stage 2**: AtomSpace core
- **Stage 3**: HurdCog components (machspace, cogkernel-core, main)
- **Stage 4**: OCC components (cogserver, ure, attention, pln, etc.)
- **Stage 5**: Integration layer (occ-bridge, unified meta-package)

**Outputs**:
- Individual package artifacts for each component
- Complete package collection: `agi-os-debian-packages-complete`
- Package index (Packages.gz)

**Usage**:
```bash
# Trigger manually with specific stage
gh workflow run agi-os-debian-build.yml -f stage=stage2

# Trigger all stages
gh workflow run agi-os-debian-build.yml -f stage=all

# Skip tests
gh workflow run agi-os-debian-build.yml -f skip_tests=true
```

**Key Features**:
- Parallel builds for independent components
- Automatic dependency installation
- Package validation before build
- Installation testing after build
- Comprehensive build summary

---

### 2. AGI-OS Guix Build (`agi-os-guix-build.yml`)

**Purpose**: Build AGI-OS components using GNU Guix for reproducible builds.

**Triggers**:
- Push to `main` or `develop` (when .scm files change)
- Pull requests
- Manual workflow dispatch with target selection

**Build Targets**:
- `cognumach`: Microkernel only
- `hurdcog`: Operating system only
- `unified`: Complete AGI-OS stack
- `all`: All targets

**Outputs**:
- Build logs for each target
- Guix store items (if successful)

**Usage**:
```bash
# Build unified stack
gh workflow run agi-os-guix-build.yml -f build_target=unified

# Build all components
gh workflow run agi-os-guix-build.yml -f build_target=all
```

**Key Features**:
- Automatic Guix installation
- Scheme syntax validation
- Development environment testing
- Fallback builds with detailed logging

**Notes**:
- Guix builds may fail in CI due to cross-compilation requirements
- Local builds are recommended for full Guix functionality
- Builds are reproducible across different machines

---

### 3. AGI-OS Integration Testing (`agi-os-integration-test.yml`)

**Purpose**: Comprehensive integration testing of all AGI-OS components.

**Triggers**:
- Push to any branch
- Pull requests
- Manual workflow dispatch
- Daily schedule (2 AM UTC)

**Test Suites**:
1. **Cognitive Synergy Test**: Full synergy_agi_os.sh execution
2. **Cognitive Integration Test**: Module functionality testing
3. **Repository Structure Test**: Directory and file validation
4. **Component Integration Test**: Cognumach, HurdCog, OCC validation
5. **Documentation Test**: Completeness and quality checks
6. **Build Dependency Test**: Circular dependency detection

**Outputs**:
- Test logs for each suite
- Dependency order report
- Integration test summary

**Usage**:
```bash
# Run all tests
gh workflow run agi-os-integration-test.yml

# Runs automatically on push/PR
```

**Key Features**:
- 6 independent test suites
- Parallel execution
- Detailed reporting
- Daily automated testing
- Comprehensive summary with pass/fail counts

**Expected Results**:
- Most tests should pass (4-6 out of 6)
- Some tests may be skipped if dependencies are unavailable
- Cognitive synergy tests demonstrate functional integration

---

### 4. AGI-OS Deploy (`agi-os-deploy.yml`)

**Purpose**: Deploy AGI-OS releases to package repositories and documentation sites.

**Triggers**:
- Release published on GitHub
- Manual workflow dispatch with target selection

**Deployment Targets**:
- `staging`: Test deployment
- `production`: Production release
- `test`: Development testing

**Jobs**:
1. **Prepare Release**: Create release artifacts and tarball
2. **Deploy Packages**: Set up Debian package repository
3. **Deploy Documentation**: Prepare documentation site
4. **Create Release Notes**: Generate comprehensive release notes

**Outputs**:
- Release tarball: `agi-os-VERSION.tar.gz`
- Debian package repository structure
- Documentation site
- Release notes

**Usage**:
```bash
# Deploy to staging
gh workflow run agi-os-deploy.yml -f deploy_target=staging

# Deploy to production
gh workflow run agi-os-deploy.yml -f deploy_target=production -f version=v1.0.0

# Automatic deployment on release
gh release create v1.0.0 --generate-notes
```

**Key Features**:
- Automatic version detection
- Complete release manifest
- Installation script generation
- Package repository creation
- Documentation deployment
- Release notes generation

**Artifacts**:
- `agi-os-release-VERSION`: Complete release tarball
- `agi-os-debian-repository`: Package repository
- `agi-os-documentation-site`: Documentation files
- `release-notes`: Generated release notes

---

## Workflow Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   AGI-OS CI/CD Pipeline                  │
└─────────────────────────────────────────────────────────┘
                            │
                ┌───────────┴───────────┐
                │                       │
        ┌───────▼────────┐    ┌────────▼────────┐
        │  Debian Build  │    │   Guix Build    │
        │   (5 stages)   │    │  (3 targets)    │
        └───────┬────────┘    └────────┬────────┘
                │                      │
                └──────────┬───────────┘
                           │
                  ┌────────▼─────────┐
                  │ Integration Test │
                  │   (6 suites)     │
                  └────────┬─────────┘
                           │
                  ┌────────▼─────────┐
                  │   Deployment     │
                  │  (4 targets)     │
                  └──────────────────┘
```

## Workflow Dependencies

```
validate-packaging
    │
    ├─→ stage0-cognumach ──┐
    ├─→ stage1-foundation ─┼─→ stage2-atomspace ──┐
    │                      │                       │
    │                      └─→ stage3-hurdcog ────┼─→ stage5-integration
    │                                              │
    └─────────────────────────→ stage4-occ ───────┘
                                    │
                                    ▼
                            collect-packages
                                    │
                                    ▼
                            test-installation
                                    │
                                    ▼
                            integration-test
                                    │
                                    ▼
                                deployment
```

## Common Operations

### Trigger a Full Build

```bash
# Debian packages
gh workflow run agi-os-debian-build.yml -f stage=all

# Guix packages
gh workflow run agi-os-guix-build.yml -f build_target=all
```

### Run Integration Tests

```bash
# Manual trigger
gh workflow run agi-os-integration-test.yml

# Tests run automatically on push/PR
```

### Create a Release

```bash
# Create release (triggers deployment)
gh release create v1.0.0 \
  --title "AGI-OS v1.0.0" \
  --notes "Complete AGI Operating System release" \
  --generate-notes

# Or deploy manually
gh workflow run agi-os-deploy.yml \
  -f deploy_target=production \
  -f version=v1.0.0
```

### Download Build Artifacts

```bash
# List recent workflow runs
gh run list --workflow=agi-os-debian-build.yml

# Download artifacts from specific run
gh run download RUN_ID

# Download specific artifact
gh run download RUN_ID --name agi-os-debian-packages-complete
```

## Workflow Status Badges

Add these to your README.md:

```markdown
[![Debian Build](https://github.com/cogpy/occ/actions/workflows/agi-os-debian-build.yml/badge.svg)](https://github.com/cogpy/occ/actions/workflows/agi-os-debian-build.yml)

[![Guix Build](https://github.com/cogpy/occ/actions/workflows/agi-os-guix-build.yml/badge.svg)](https://github.com/cogpy/occ/actions/workflows/agi-os-guix-build.yml)

[![Integration Tests](https://github.com/cogpy/occ/actions/workflows/agi-os-integration-test.yml/badge.svg)](https://github.com/cogpy/occ/actions/workflows/agi-os-integration-test.yml)

[![Deploy](https://github.com/cogpy/occ/actions/workflows/agi-os-deploy.yml/badge.svg)](https://github.com/cogpy/occ/actions/workflows/agi-os-deploy.yml)
```

## Secrets Configuration

Some workflows may require secrets for deployment:

```bash
# Set repository secrets
gh secret set DEPLOY_SSH_KEY < ~/.ssh/deploy_key
gh secret set PACKAGE_SERVER_HOST --body "packages.agi-os.org"
gh secret set PACKAGE_SERVER_USER --body "deploy"
```

## Troubleshooting

### Debian Build Fails

1. Check package validation output
2. Verify update scripts are executable
3. Check for missing build dependencies
4. Review build logs in artifacts

### Guix Build Fails

1. Expected for cross-compilation targets
2. Check Scheme syntax validation
3. Verify Guix installation succeeded
4. Review build logs for specific errors

### Integration Tests Fail

1. Check which specific test suite failed
2. Review test logs in artifacts
3. Some tests may be skipped (expected)
4. Verify repository structure is complete

### Deployment Issues

1. Verify release was created properly
2. Check artifact availability
3. Review deployment target configuration
4. Verify secrets are configured (if needed)

## Performance Optimization

### Caching

Workflows use GitHub Actions caching for:
- APT package cache
- Guix store items
- Build artifacts

### Parallel Execution

- Stage 4 (OCC components) runs in parallel matrix
- Independent test suites run in parallel
- Artifact uploads happen asynchronously

### Resource Limits

- Workflow timeout: 60 minutes (default)
- Artifact retention: 30-90 days
- Maximum parallel jobs: 20 (GitHub default)

## Maintenance

### Regular Updates

- Update Ubuntu version: Change `runs-on: ubuntu-XX.04`
- Update dependencies: Modify apt-get install commands
- Update Guix version: Modify Guix installation script

### Adding New Packages

1. Add to appropriate stage in Debian build workflow
2. Update dependency matrix if needed
3. Add to integration tests
4. Update documentation

### Modifying Build Order

1. Edit stage dependencies in Debian build workflow
2. Update dependency resolution script
3. Test with workflow dispatch
4. Verify integration tests pass

## Best Practices

1. **Always test locally first**: Run scripts locally before pushing
2. **Use workflow dispatch**: Test workflows manually before merging
3. **Check artifacts**: Download and verify build outputs
4. **Monitor test results**: Review integration test summaries
5. **Keep workflows updated**: Regular maintenance and updates
6. **Document changes**: Update this README when modifying workflows

## Support

For issues with workflows:
1. Check workflow run logs
2. Review this documentation
3. Open an issue on GitHub
4. Check GitHub Actions status page

## References

- [GitHub Actions Documentation](https://docs.github.com/en/actions)
- [Debian Packaging Guide](https://www.debian.org/doc/manuals/maint-guide/)
- [GNU Guix Manual](https://guix.gnu.org/manual/)
- [AGI-OS Integration Architecture](../../AGI_OS_INTEGRATION_ARCHITECTURE.md)

---

**Last Updated**: 2025-12-06  
**Workflows Version**: 1.0  
**Status**: Production Ready ✅
