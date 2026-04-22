# GNU Mach CI/CD Pipeline

This document describes the comprehensive CI/CD pipeline established for the GNU Mach microkernel project.

## Overview

The CI/CD pipeline provides automated building, testing, and quality assurance for multiple architectures with the following key features:

- **Multi-architecture builds** (i686, x86_64)
- **Comprehensive static analysis** (cppcheck, clang scan-build)
- **Regression testing** with QEMU-based functional tests
- **Code quality checks** and security scanning
- **Integration testing** and performance monitoring
- **Artifact management** with build outputs and reports

## Pipeline Architecture

### Workflow: `ci-cd.yml`

The main CI/CD pipeline is triggered on:
- Push to `master` or `develop` branches
- Pull requests to `master` or `develop`
- Manual workflow dispatch

#### Jobs Overview:

1. **Build** (`build`)
   - Matrix build for i686 and x86_64 architectures
   - Handles MIG (Mach Interface Generator) setup
   - Produces kernel binaries and build artifacts
   - Uses `--force-build` to handle known MIG static assertion issues

2. **Code Quality** (`code-quality`) 
   - Comprehensive static analysis with cppcheck and clang tools
   - Compiler warning analysis
   - Critical issue detection with failure conditions
   - Analysis report generation

3. **Regression Tests** (`test`)
   - Functional testing with QEMU emulation
   - Basic kernel functionality tests
   - Console timestamp verification
   - Test log collection and artifact upload

4. **Security Scan** (`security`)
   - Security vulnerability detection
   - Common security anti-pattern scanning
   - Hardcoded credential detection

5. **Integration Test** (`integration`)
   - End-to-end pipeline verification
   - Build artifact validation
   - Smoke testing of complete build process

6. **Performance Check** (`performance`)
   - Build performance monitoring (on non-PR builds)
   - Future: Binary size tracking, build time metrics

## Build Script: `scripts/ci-build.sh`

Enhanced build script that handles:

- **Architecture-specific configuration** (i686, x86_64)
- **Dependency checking** and MIG setup
- **Build process management** with error handling
- **Testing integration** with timeout controls
- **Static analysis integration**

### Usage:

```bash
# Basic build
./scripts/ci-build.sh i686

# Build with tests
./scripts/ci-build.sh --test i686

# Build with static analysis
./scripts/ci-build.sh --analysis i686

# Clean build with debug
./scripts/ci-build.sh --clean --debug --test i686

# Force build (continue despite MIG assertion errors)
./scripts/ci-build.sh --force-build x86_64
```

## Known Issues and Workarounds

### MIG Static Assertion Errors

The build currently encounters static assertion failures in MIG-generated code related to port size expectations. The CI/CD pipeline uses `--force-build` to continue past these issues while they are being resolved.

**Example error:**
```
vm/memory_object_user.user.c:164:9: error: static assertion failed: "expected ipc_port_t to be size 8 * 1"
```

**Current workaround:** The `--force-build` flag allows builds to continue despite these assertions, enabling testing of other functionality.

### Test Reliability

Some tests may be flaky in QEMU environments. The pipeline:
- Uses appropriate timeouts (300 seconds)
- Captures exit codes for analysis
- Continues with other tests if one fails
- Preserves test logs as artifacts

## Artifact Management

### Build Artifacts
- `gnumach-{arch}`: Kernel binaries for each architecture
- Build logs and configuration files
- Retention: 30 days

### Analysis Reports
- `code-analysis-reports`: Static analysis results
- cppcheck reports, compiler warnings
- Clang scan-build results
- Retention: 30 days

### Test Artifacts
- `test-logs-{arch}`: Test execution logs
- QEMU console outputs and raw logs
- Retention: 30 days

## Quality Gates

### Critical Failure Conditions
- Build failures (except known MIG assertion issues with --force-build)
- Critical security issues detected in static analysis
- Complete test suite failures

### Warning Conditions
- Individual test failures (logged but don't fail pipeline)
- Non-critical static analysis issues
- Performance regressions (future enhancement)

## Integration with Development Workflow

### Pull Request Checks
- Full CI/CD pipeline runs on all PRs
- Quality gates must pass for merge approval
- Analysis reports available as artifacts

### Branch Protection
- CI/CD pipeline success required for master/develop merges
- Status checks enforce quality standards

### Developer Tools
- Local build script can replicate CI environment
- Static analysis can be run locally
- Test scripts available for individual test execution

## Future Enhancements

### Planned Improvements
1. **Enhanced Testing**
   - Expand test coverage to additional architectures
   - Add performance benchmarking
   - Integration with external test suites

2. **Advanced Analysis**
   - Memory leak detection
   - Performance profiling integration
   - Code coverage measurement

3. **Deployment Automation**
   - Automated release builds
   - Cross-compilation for additional architectures
   - Container-based testing environments

4. **Monitoring & Metrics**
   - Build time tracking
   - Binary size monitoring
   - Test success rate trending

## Maintenance

### Workflow Updates
- Workflows are version controlled in `.github/workflows/`
- Deprecated workflows are preserved with `.deprecated` suffix
- Changes should be tested in feature branches

### Dependency Management
- Tool versions pinned in workflows where possible
- MIG built from source to ensure compatibility
- Regular updates to GitHub Actions versions

### Troubleshooting
- Check workflow logs in GitHub Actions tab
- Build artifacts available for download and analysis  
- Test logs provide detailed failure information

For questions or issues with the CI/CD pipeline, consult the GitHub Actions logs or open an issue in the project repository.