# Comprehensive GitHub Actions Workflow Issues Analysis

## Date: December 12, 2025

## Executive Summary

Multiple critical build failures have been identified across the GitHub Actions workflows. The most impactful issues are:
1. Missing build dependencies causing Debian package build failures
2. Build order dependency violations in AGI-OS workflows
3. Configuration errors in OC workflow
4. Guix installation reliability issues

## Critical Issues by Priority

### PRIORITY 1: Debian Package Build Failures

#### Issue 1.1: Missing cython3 dependency in cogserver build job
- **Workflow**: `.github/workflows/debian-packages.yml`
- **Job**: `build-cogserver` (lines 362-435)
- **Error**: `dpkg-checkbuilddeps: error: Unmet build dependencies: cython3`
- **Root Cause**: The cogserver job installs many prerequisites but omits `cython3`, which is required in the debian/control file
- **Impact**: HIGH - Prevents cogserver package from building, blocking downstream packages (attention)
- **Fix**: Add `cython3` to the prerequisites installation step at line 398

```yaml
# Current (line 374-399):
- name: Install Prerequisites
  run: |
    sudo apt-get update
    sudo apt-get install -y \
      build-essential \
      debhelper \
      dpkg-dev \
      cmake \
      g++ \
      guile-3.0-dev \
      libboost-dev \
      libboost-date-time-dev \
      libboost-filesystem-dev \
      libboost-program-options-dev \
      libboost-regex-dev \
      libboost-serialization-dev \
      libboost-system-dev \
      libboost-thread-dev \
      binutils-dev \
      libiberty-dev \
      libasio-dev \
      cxxtest \
      python3-dev \
      python3-nose \
      valgrind \
      patchelf

# Should be (add cython3):
- name: Install Prerequisites
  run: |
    sudo apt-get update
    sudo apt-get install -y \
      build-essential \
      debhelper \
      dpkg-dev \
      cmake \
      g++ \
      guile-3.0-dev \
      libboost-dev \
      libboost-date-time-dev \
      libboost-filesystem-dev \
      libboost-program-options-dev \
      libboost-regex-dev \
      libboost-serialization-dev \
      libboost-system-dev \
      libboost-thread-dev \
      binutils-dev \
      libiberty-dev \
      libasio-dev \
      cxxtest \
      cython3 \
      python3-dev \
      python3-nose \
      valgrind \
      patchelf
```

### PRIORITY 2: AGI-OS Build Failures

#### Issue 2.1: Build order dependency violations
- **Workflow**: `.github/workflows/agi-os-complete-stack.yml`
- **Jobs**: `Build cogserver`, `Build attention`
- **Errors**:
  - cogserver: `CMake Error at CMakeLists.txt:89 (FIND_PACKAGE)` - Cannot find required package
  - attention: `CMake Error at CMakeLists.txt:97 (MESSAGE): CogServer missing: it is needed!`
- **Root Cause**: Build jobs are running in parallel without proper dependency sequencing
- **Impact**: HIGH - Components that depend on cogserver fail because cogserver hasn't been built/installed yet
- **Fix**: Ensure proper job dependencies with `needs:` directives and install artifacts correctly

### PRIORITY 3: OC Workflow Configuration Issues

#### Issue 3.1: Git safe directory path incorrect
- **Workflow**: `.github/workflows/oc.yml` (line 45)
- **Current**: `git config --global --add safe.directory /__w/pycog0/pycog0`
- **Error**: References wrong path `/__w/pycog0/pycog0` instead of `/__w/occ/occ`
- **Root Cause**: Copy-paste error from another repository
- **Impact**: MEDIUM - May cause git operations to fail with dubious ownership errors
- **Fix**: Update to correct path

```yaml
# Current (line 44-45):
- name: Trust working directory for Git
  run: git config --global --add safe.directory /__w/pycog0/pycog0

# Should be:
- name: Trust working directory for Git
  run: git config --global --add safe.directory /__w/occ/occ
```

#### Issue 3.2: PostgreSQL role configuration
- **Workflow**: `.github/workflows/oc.yml`
- **Error**: `FATAL: role "root" does not exist` (repeated)
- **Root Cause**: The workflow runs in a container as root, but PostgreSQL service doesn't have the root role properly configured
- **Impact**: MEDIUM - Database connectivity tests fail
- **Fix**: The PostgreSQL service is configured correctly with `POSTGRES_USER: root`, but the container is trying to use `sudo` which is unnecessary when already running as root

```yaml
# Current (line 54-55):
- name: Verify PostgreSQL Connection
  run: |
    # Install PostgreSQL client
    sudo apt-get install -y postgresql-client

# Should be (remove sudo, already running as root):
- name: Verify PostgreSQL Connection
  run: |
    # Install PostgreSQL client
    apt-get update && apt-get install -y postgresql-client
```

### PRIORITY 4: Guix Build Workflow Reliability

#### Issue 4.1: Network reliability with git.savannah.gnu.org
- **Workflow**: `.github/workflows/guix-build.yml`
- **Error**: `curl: (28) Failed to connect to git.savannah.gnu.org port 443 after 134834 ms`
- **Root Cause**: The Guix installation script is downloaded from `git.savannah.gnu.org`, which has known reliability issues
- **Impact**: MEDIUM - Workflow cannot proceed past installation, but this is a known infrastructure issue
- **Fix**: Add fallback mirror sources and improve retry logic

## Packaging Issues

### Debian/Aptitude Packaging
- **Status**: Configuration files exist in `opencog-debian/` directory
- **Issues**: Build failures prevent package generation
- **Fix**: Once build workflows are fixed, packages will generate correctly

### Chocolatey Packaging
- **Status**: No Chocolatey packaging configuration found in repository
- **Recommendation**: Create `.nuspec` files and packaging scripts if Windows support is needed

### Other Package Managers
- **Guix**: Package definitions exist and pass validation
- **Nix**: Configuration exists in `opencog-nix/` directory
- **Docker**: Dockerfile exists but may need updates

## Immediate Action Plan

1. **Fix debian-packages.yml** (15 minutes)
   - Add cython3 to cogserver prerequisites
   - Test the fix

2. **Fix oc.yml** (10 minutes)
   - Update git safe directory path
   - Remove unnecessary sudo commands in container

3. **Review AGI-OS workflows** (30 minutes)
   - Check all AGI-OS workflow files for dependency order
   - Add proper `needs:` directives
   - Ensure artifacts are installed before dependent builds

4. **Test fixes** (variable)
   - Commit changes to a test branch
   - Monitor workflow runs
   - Iterate on any remaining issues

5. **Sync to main repository** (5 minutes)
   - Push fixes to main branch
   - Update documentation

## Files to Modify

1. `.github/workflows/debian-packages.yml` - Add cython3 dependency
2. `.github/workflows/oc.yml` - Fix git path and remove sudo
3. `.github/workflows/agi-os-complete-stack.yml` - Review dependencies
4. `.github/workflows/agi-os-unified-build.yml` - Review dependencies
5. `.github/workflows/guix-build.yml` - Add mirror fallbacks (optional)

## Success Criteria

- [ ] Debian package build workflow completes successfully
- [ ] All OpenCog packages build without errors
- [ ] AGI-OS workflows complete full stack builds
- [ ] OC workflow runs without PostgreSQL or git errors
- [ ] No mock placeholders or incomplete implementations in builds
