# Testing and Validation Strategy

## Overview

This document outlines the comprehensive testing and validation strategy for the OpenCog Collection (OCC) repository, covering Windows builds, packaging, Inferno kernel modules, and the overall AGI architecture.

## Testing Hierarchy

```
Level 1: Unit Tests (Component-level)
    ↓
Level 2: Integration Tests (Cross-component)
    ↓
Level 3: System Tests (End-to-end)
    ↓
Level 4: Performance Tests (Benchmarking)
    ↓
Level 5: Deployment Tests (Production-ready)
```

## Windows Build Validation

### Build Status Monitoring

**Current Status:**
- ✅ Windows compatibility fixes applied
- 🔄 OCC Windows Build workflow running
- 🔄 OCC Build - Windows Native (wincog) running
- ✅ Non-critical workflows disabled

**Validation Checklist:**

#### Stage 1: CogUtil Build
- [ ] CMake configuration succeeds
- [ ] All source files compile without errors
- [ ] No POSIX-specific errors (getcwd, sys/time.h)
- [ ] No snprintf macro conflicts
- [ ] M_PI constant properly defined
- [ ] windows_compat.h included correctly
- [ ] Unit tests build (if enabled)
- [ ] Installation completes successfully
- [ ] Artifacts uploaded to GitHub

#### Stage 2: AtomSpace Build
- [ ] CMake finds CogUtil dependency
- [ ] All source files compile without errors
- [ ] Links against CogUtil libraries
- [ ] Python bindings build (if enabled)
- [ ] Unit tests build (if enabled)
- [ ] Installation completes successfully
- [ ] Artifacts uploaded to GitHub

#### Stage 3: Moses Build
- [ ] CMake finds CogUtil dependency
- [ ] All source files compile without errors
- [ ] Links against CogUtil libraries
- [ ] Evolutionary algorithms compile
- [ ] Unit tests build (if enabled)
- [ ] Installation completes successfully
- [ ] Artifacts uploaded to GitHub

### Build Verification Commands

```powershell
# Verify CogUtil installation
Test-Path "C:\OpenCog\include\opencog\util\Config.h"
Test-Path "C:\OpenCog\lib\cogutil.lib"

# Verify AtomSpace installation
Test-Path "C:\OpenCog\include\opencog\atomspace\AtomSpace.h"
Test-Path "C:\OpenCog\lib\atomspace.lib"

# Verify Moses installation
Test-Path "C:\OpenCog\bin\moses.exe"
Test-Path "C:\OpenCog\lib\moses.lib"

# Check DLL dependencies
dumpbin /dependents "C:\OpenCog\bin\cogutil.dll"
dumpbin /dependents "C:\OpenCog\bin\atomspace.dll"
```

## Packaging Validation

### Chocolatey Package Testing

**Prerequisites:**
- Windows 10/11 (64-bit)
- Chocolatey installed
- Visual C++ Redistributable 2019+

**Test Steps:**

```powershell
# 1. Install package from local .nupkg
choco install opencog --source="." --force

# 2. Verify installation
Get-Command opencog -ErrorAction SilentlyContinue

# 3. Check installed files
Get-ChildItem "C:\ProgramData\chocolatey\lib\opencog"

# 4. Test basic functionality
# (Add specific OpenCog commands here)

# 5. Uninstall package
choco uninstall opencog

# 6. Verify clean uninstall
Test-Path "C:\ProgramData\chocolatey\lib\opencog"  # Should be False
```

**Validation Checklist:**
- [ ] Package installs without errors
- [ ] All binaries are in PATH
- [ ] DLL dependencies resolved
- [ ] Configuration files created
- [ ] Documentation accessible
- [ ] Package uninstalls cleanly
- [ ] No orphaned files after uninstall

### Debian Package Testing

**Prerequisites:**
- Debian Bookworm/Bullseye or Ubuntu 22.04/24.04
- Build dependencies installed

**Test Steps:**

```bash
# 1. Install package
sudo dpkg -i opencog_*.deb
sudo apt-get install -f  # Fix dependencies

# 2. Verify installation
dpkg -L opencog | head -20

# 3. Check library dependencies
ldd /usr/lib/libcogutil.so
ldd /usr/lib/libatomspace.so

# 4. Test basic functionality
# (Add specific OpenCog commands here)

# 5. Run lintian checks
lintian opencog_*.deb

# 6. Uninstall package
sudo apt-get remove opencog

# 7. Verify clean uninstall
dpkg -l | grep opencog  # Should be empty
```

**Validation Checklist:**
- [ ] Package installs without errors
- [ ] All dependencies satisfied
- [ ] Binaries executable
- [ ] Libraries properly linked
- [ ] Man pages installed
- [ ] Lintian checks pass
- [ ] Package uninstalls cleanly

## Inferno Kernel Module Validation

### AtomSpace Module Testing

**Test File:** `inferno-kern/atomspace/test_atomspace.b`

```limbo
implement TestAtomSpace;

include "sys.m";
include "atomspace.m";

TestAtomSpace: module {
    init: fn();
};

init()
{
    sys := load Sys Sys->PATH;
    atomspace := load AtomSpace AtomSpace->PATH;
    atomspace->init();
    
    # Test 1: Create node
    tv := atomspace->new_truth_value(0.8, 0.9);
    cat := space.create_node("ConceptNode", "cat", tv);
    assert(cat != nil, "Failed to create node");
    
    # Test 2: Create link
    mammal := space.create_node("ConceptNode", "mammal", tv);
    link := space.create_link("InheritanceLink", (cat, mammal), tv);
    assert(link != nil, "Failed to create link");
    
    # Test 3: Query by name
    results := space.get_atoms_by_name("cat");
    assert(len results == 1, "Failed to query by name");
    
    # Test 4: Update truth value
    new_tv := atomspace->new_truth_value(0.9, 0.95);
    cat.set_tv(new_tv);
    assert(cat.get_tv().strength == 0.9, "Failed to update TV");
    
    # Test 5: Incoming set
    assert(len cat.incoming == 1, "Incoming set incorrect");
    
    sys->print("All AtomSpace tests passed!\n");
}

assert(condition: int, message: string)
{
    if (!condition) {
        sys->print("ASSERTION FAILED: " + message + "\n");
        raise "fail:assertion";
    }
}
```

**Validation Checklist:**
- [ ] Module loads successfully
- [ ] Node creation works
- [ ] Link creation works
- [ ] Truth value updates work
- [ ] Attention value updates work
- [ ] Incoming/outgoing sets correct
- [ ] Indexing works (name, type)
- [ ] Memory management correct

### Attention Module Testing

**Test File:** `inferno-kern/attention/test_attention.b`

```limbo
implement TestAttention;

include "sys.m";
include "atomspace.m";
include "attention.m";

TestAttention: module {
    init: fn();
};

init()
{
    sys := load Sys Sys->PATH;
    atomspace := load AtomSpace AtomSpace->PATH;
    attention := load Attention Attention->PATH;
    
    atomspace->init();
    attention->init();
    
    # Test 1: Stimulate atom
    tv := atomspace->new_truth_value(0.8, 0.9);
    cat := space.create_node("ConceptNode", "cat", tv);
    attention->stimulate_atom(cat, 100);
    assert(cat.get_av().get_sti() == 100, "Stimulation failed");
    
    # Test 2: Attentional focus
    af := attention_bank.get_af();
    assert(af.contains(cat), "Atom not in AF");
    
    # Test 3: Attention spreading
    dog := space.create_node("ConceptNode", "dog", tv);
    link := space.create_link("InheritanceLink", (cat, dog), tv);
    attention->spread_attention(cat);
    assert(dog.get_av().get_sti() > 0, "Attention not spread");
    
    # Test 4: Cognitive scheduling
    next := attention->schedule_next();
    assert(next != nil, "Scheduling failed");
    
    # Test 5: Forgetting
    low_sti := space.create_node("ConceptNode", "forgotten", tv);
    attention->stimulate_atom(low_sti, -200);
    attention_bank.forget();
    assert(!af.contains(low_sti), "Forgetting failed");
    
    sys->print("All Attention tests passed!\n");
}
```

**Validation Checklist:**
- [ ] Module loads successfully
- [ ] Stimulation works
- [ ] Attentional focus management works
- [ ] Attention spreading works
- [ ] Hebbian learning works
- [ ] Cognitive scheduling works
- [ ] Forgetting mechanism works
- [ ] Importance updating works

## Integration Testing

### Cross-Module Integration

**Test Scenarios:**

#### Scenario 1: AtomSpace + Attention
```limbo
# Create atoms
cat := space.create_node("ConceptNode", "cat", tv);
mammal := space.create_node("ConceptNode", "mammal", tv);
link := space.create_link("InheritanceLink", (cat, mammal), tv);

# Stimulate and spread
attention->stimulate_atom(cat, 100);
attention->spread_attention(cat);

# Verify attention spread to linked atoms
assert(mammal.get_av().get_sti() > 0);
```

#### Scenario 2: Distributed Cognition
```bash
# Node 1: Start AtomSpace server
mount -A tcp!localhost!9999 /cog/atomspace

# Node 2: Connect to remote AtomSpace
mount -A tcp!node1!9999 /n/remote/atomspace

# Create atom on remote node
echo "ConceptNode cat" > /n/remote/atomspace

# Verify atom created
cat /n/remote/atomspace/query
```

#### Scenario 3: Neural-Symbolic Integration
```limbo
# Create symbolic atoms
cat := space.create_node("ConceptNode", "cat", tv);

# Convert to tensor
tensor := tensor_cognition->atom_to_tensor(cat);

# Perform neural operations
embedding := neural_network->forward(tensor);

# Convert back to symbolic
new_atoms := tensor_cognition->tensor_to_atom(embedding);
```

**Validation Checklist:**
- [ ] AtomSpace + Attention integration works
- [ ] Distributed cognition via 9P works
- [ ] Neural-symbolic integration works
- [ ] Cross-language interop works
- [ ] Error handling correct
- [ ] Performance acceptable

## Performance Testing

### Benchmarks

#### AtomSpace Performance

```limbo
# Benchmark 1: Node creation
start := sys->millisec();
for (i := 0; i < 10000; i++) {
    space.create_node("ConceptNode", sprint("node%d", i), tv);
}
elapsed := sys->millisec() - start;
sys->print(sprint("Created 10000 nodes in %d ms\n", elapsed));

# Benchmark 2: Link creation
start = sys->millisec();
for (i := 0; i < 10000; i++) {
    space.create_link("InheritanceLink", (node1, node2), tv);
}
elapsed = sys->millisec() - start;
sys->print(sprint("Created 10000 links in %d ms\n", elapsed));

# Benchmark 3: Query performance
start = sys->millisec();
for (i := 0; i < 1000; i++) {
    space.get_atoms_by_name("cat");
}
elapsed = sys->millisec() - start;
sys->print(sprint("1000 queries in %d ms\n", elapsed));
```

**Performance Targets:**
- Node creation: < 0.1 ms per node
- Link creation: < 0.2 ms per link
- Query by name: < 0.05 ms per query
- Attention update: < 0.01 ms per update
- Memory usage: < 100 bytes per atom

#### Attention Performance

```limbo
# Benchmark: Attention spreading
start := sys->millisec();
for (i := 0; i < 1000; i++) {
    attention->spread_attention(source);
}
elapsed := sys->millisec() - start;
sys->print(sprint("1000 attention spreads in %d ms\n", elapsed));
```

**Performance Targets:**
- Attention stimulation: < 0.01 ms
- Attention spreading: < 0.1 ms per spread
- AF update: < 1 ms
- Cognitive scheduling: < 0.05 ms

### Scalability Testing

**Test Configurations:**

| Test | Atoms | Links | Memory | Time |
|------|-------|-------|--------|------|
| Small | 1K | 2K | < 10 MB | < 1s |
| Medium | 10K | 20K | < 100 MB | < 10s |
| Large | 100K | 200K | < 1 GB | < 100s |
| XLarge | 1M | 2M | < 10 GB | < 1000s |

**Validation Checklist:**
- [ ] Small scale: All operations fast
- [ ] Medium scale: Performance acceptable
- [ ] Large scale: No memory leaks
- [ ] XLarge scale: Graceful degradation
- [ ] Distributed: Linear scaling

## Deployment Testing

### Production Readiness

**Checklist:**

#### Security
- [ ] No buffer overflows
- [ ] No memory leaks
- [ ] Input validation
- [ ] Access control
- [ ] Secure communication (TLS)
- [ ] Audit logging

#### Reliability
- [ ] Error handling complete
- [ ] Graceful degradation
- [ ] Automatic recovery
- [ ] Data persistence
- [ ] Backup/restore
- [ ] High availability

#### Maintainability
- [ ] Code documentation
- [ ] API documentation
- [ ] Deployment guide
- [ ] Troubleshooting guide
- [ ] Monitoring/alerting
- [ ] Logging infrastructure

#### Compatibility
- [ ] Windows 10/11 support
- [ ] Linux (Debian, Ubuntu) support
- [ ] Inferno OS support
- [ ] Cross-platform builds
- [ ] Version compatibility
- [ ] Upgrade path

## Continuous Integration

### GitHub Actions Workflows

**Monitored Workflows:**
- `occ-win-build.yml`: Windows builds
- `wincog.yml`: Comprehensive Windows builds
- `debian-package.yml`: Debian packaging
- `chocolatey-package.yml`: Chocolatey packaging
- `auto-sync.yml`: Repository synchronization

**Success Criteria:**
- [ ] All Windows builds pass
- [ ] All packaging workflows pass
- [ ] Auto-sync triggers correctly
- [ ] Artifacts uploaded successfully
- [ ] No security vulnerabilities
- [ ] Code quality checks pass

### Automated Testing

```yaml
# .github/workflows/test.yml
name: Automated Testing

on: [push, pull_request]

jobs:
  test-atomspace:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Test AtomSpace
        run: limbo inferno-kern/atomspace/test_atomspace.b
  
  test-attention:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Test Attention
        run: limbo inferno-kern/attention/test_attention.b
```

## Reporting

### Test Report Template

```markdown
# Test Report: [Date]

## Summary
- Total tests: X
- Passed: Y
- Failed: Z
- Success rate: Y/X %

## Windows Builds
- CogUtil: [PASS/FAIL]
- AtomSpace: [PASS/FAIL]
- Moses: [PASS/FAIL]

## Packaging
- Chocolatey: [PASS/FAIL]
- Debian: [PASS/FAIL]

## Inferno Modules
- AtomSpace: [PASS/FAIL]
- Attention: [PASS/FAIL]

## Performance
- Node creation: X ms
- Link creation: Y ms
- Query performance: Z ms

## Issues
1. [Issue description]
2. [Issue description]

## Recommendations
1. [Recommendation]
2. [Recommendation]
```

## Next Steps

1. ✅ Monitor Windows build completion
2. 🔄 Validate build artifacts
3. 🔄 Test Chocolatey package
4. 🔄 Test Debian package
5. 🔄 Test Inferno kernel modules
6. 🔄 Run performance benchmarks
7. 🔄 Generate test report
8. 🔄 Deploy to production

## References

- GitHub Actions: https://docs.github.com/en/actions
- Chocolatey Testing: https://docs.chocolatey.org/en-us/community-repository/moderation/package-validator
- Debian Testing: https://www.debian.org/doc/manuals/developers-reference/pkgs.html#testing
- Inferno Testing: http://www.vitanuova.com/inferno/papers/
