# CogArc: OpenCog Component Archetype - Complete Index

## Quick Navigation

| Document | Purpose | Audience |
|----------|---------|----------|
| [README.md](README.md) | Overview and architecture | All users |
| [QUICKREF.md](QUICKREF.md) | Quick command reference | Developers needing fast answers |
| [USAGE.md](USAGE.md) | Detailed usage instructions | Developers creating components |
| [EXAMPLES.md](EXAMPLES.md) | Real-world examples | Developers learning the system |
| This file (INDEX.md) | Navigation and organization | All users |

## Documentation Structure

### 1. Getting Started

**New to CogArc?** Start here:
1. Read [README.md](README.md) - Understand the archetype pattern
2. Review [QUICKREF.md](QUICKREF.md) - Learn basic commands
3. Try [EXAMPLES.md](EXAMPLES.md) Example 1 - Generate your first component

**Quick Start (< 2 minutes):**
```bash
cd /path/to/occ
./cogarc/scripts/generate-component.sh my-component "My component description"
cd my-component
mkdir build && cd build && cmake .. && make
```

### 2. Core Documentation

#### README.md - Main Documentation
- **Section: Overview** - What is CogArc and why it exists
- **Section: Archetypal Structure** - The invariant OpenCog component structure
- **Section: Core Design Principles** - Seven key principles for OpenCog components
- **Section: Usage** - Basic generation commands
- **Section: Template Variables** - Available substitution variables
- **Section: Architecture Patterns** - CMake, version header, CircleCI patterns
- **Section: Advanced Features** - GitHub Actions, multi-language support

#### QUICKREF.md - Command Reference
- **Quick Commands** - Common one-liners
- **Command Options** - All available flags and parameters
- **After Generation** - Build, test, install, package
- **Directory Structure** - Where files go
- **Integration** - Adding to OCC monorepo
- **Troubleshooting** - Common problems and solutions

#### USAGE.md - Detailed Instructions
- **Section: Quick Start** - Getting started in 30 seconds
- **Section: Detailed Usage** - Full option documentation
- **Section: After Generation** - Detailed post-generation workflow
- **Section: Integration with OCC** - Monorepo integration steps
- **Section: Customization** - Modifying templates
- **Section: Best Practices** - Component design guidelines
- **Section: Troubleshooting** - Detailed problem solving

#### EXAMPLES.md - Real-World Examples
- **Example 1**: Simple utility component
- **Example 2**: AtomSpace extension
- **Example 3**: Advanced reasoning component
- **Example 4**: GitHub Actions workflow
- **Example 5**: Transform existing code
- **Example 6**: Language bindings
- **Example 7**: Full-featured research component
- **Integration patterns** and **troubleshooting examples**

### 3. Scripts and Tools

#### scripts/generate-component.sh
**Purpose:** Generate new OpenCog component from archetype template

**Usage:**
```bash
./generate-component.sh <name> <description> [options]
```

**Options:**
- `--output-dir <dir>` - Output location
- `--version <version>` - Initial version (default: 1.0.0)
- `--author <name>` - Author name
- `--email <email>` - Contact email
- `--dependencies <list>` - Comma-separated dependencies
- `--with-guile` - Include Guile bindings
- `--with-python` - Include Python bindings
- `--with-github-actions` - Use GitHub Actions CI

**See:** [USAGE.md](USAGE.md) for details

#### scripts/transform-to-opencog.py
**Purpose:** Transform existing code into OpenCog component

**Usage:**
```bash
./transform-to-opencog.py \
  --source /path/to/code \
  --component-name name \
  --description "description"
```

**Features:**
- Analyzes existing source structure
- Identifies source, header, test files
- Creates OpenCog structure
- Generates integration guide

**See:** [USAGE.md](USAGE.md) and [EXAMPLES.md](EXAMPLES.md) Example 5

#### scripts/test-generation.sh
**Purpose:** Test the component generation system

**Usage:**
```bash
./test-generation.sh
```

**Tests:**
1. Basic component generation
2. Variable substitution
3. Dependency handling
4. GitHub Actions workflow
5. Custom version and author

### 4. Templates

All templates are in `template/` directory:

#### Root Files
- `CMakeLists.txt` - Root build configuration with full dependency management
- `README.md` - Documentation template with placeholders
- `.gitignore` - Standard ignore patterns

#### Configuration Directories
- `.circleci/config.yml` - CircleCI continuous integration
- `.github/workflows/build.yml` - GitHub Actions workflow
- `cmake/` - CMake modules (Summary.cmake, OpenCogGccOptions.cmake, Config.cmake.in)
- `debian/` - Debian packaging (control, rules, copyright, changelog, install files)

#### Source Structure
- `opencog/version.h` - Version header template
- Component-specific CMakeLists.txt templates

### 5. Archetypal Patterns

#### Directory Pattern (Invariant Structure)
```
component-name/
├── .circleci/              # CI/CD: CircleCI configuration
├── .github/                # CI/CD: GitHub Actions (optional)
├── cmake/                  # Build: CMake modules
├── debian/                 # Packaging: Debian files
├── doc/                    # Documentation (optional)
├── examples/               # Usage examples
├── lib/                    # Additional libraries (usually empty)
├── opencog/[component]/    # Source code
├── scripts/                # Utility scripts
├── tests/[component]/      # Unit tests
├── CMakeLists.txt          # Root build config
├── LICENSE                 # AGPL-3.0
├── README.md               # Documentation
└── AUTHORS                 # Contributors
```

#### CMake Pattern
- Minimum version: 3.12
- Build types: Release, Debug, Coverage, Profile, RelWithDebInfo
- Version from header file
- Dependency detection via FIND_PACKAGE
- Modular build with subdirectories

#### Dependency Pattern (Build Order)
1. **CogUtil** (always first) - Base utilities
2. **AtomSpace** (if needed) - Knowledge representation
3. **Other components** (as needed) - Additional functionality

#### Version Pattern
- Semantic versioning: MAJOR.MINOR.PATCH
- Defined in `opencog/[component]/version.h`
- Used by CMake for package configuration
- Format: `#define COMPONENT_VERSION_STRING "X.Y.Z"`

### 6. Use Cases

#### Use Case 1: New Utility Component
**Scenario:** Create new standalone utilities

**Steps:**
1. Generate component: `./generate-component.sh util-name "description"`
2. Add source code to `opencog/util-name/`
3. Build and test

**See:** [EXAMPLES.md](EXAMPLES.md) Example 1

#### Use Case 2: AtomSpace Extension
**Scenario:** Extend AtomSpace functionality

**Steps:**
1. Generate with AtomSpace dependency
2. Implement using AtomSpace API
3. Add pattern matching or graph operations

**See:** [EXAMPLES.md](EXAMPLES.md) Example 2

#### Use Case 3: Reasoning Module
**Scenario:** Implement new reasoning algorithm

**Steps:**
1. Generate with cogutil, atomspace, ure dependencies
2. Implement reasoning rules
3. Integrate with URE or PLN

**See:** [EXAMPLES.md](EXAMPLES.md) Example 3

#### Use Case 4: Legacy Code Migration
**Scenario:** Convert existing C++ library to OpenCog component

**Steps:**
1. Use `transform-to-opencog.py`
2. Review `INTEGRATION_GUIDE.md`
3. Update dependencies and build

**See:** [EXAMPLES.md](EXAMPLES.md) Example 5

#### Use Case 5: Research Project
**Scenario:** Create component for research with publications

**Steps:**
1. Generate full-featured component
2. Add Guile/Python bindings
3. Include comprehensive examples
4. Document in README

**See:** [EXAMPLES.md](EXAMPLES.md) Example 7

### 7. Integration Workflows

#### Standalone Component
```
Generate → Develop → Build → Test → Package → Publish
```

#### OCC Monorepo Integration
```
Generate → Copy to OCC → Update CMakeLists.txt → Update CI → Build → Test
```

#### Research Publication
```
Generate → Implement → Document → Test → Benchmark → Publish → Package
```

### 8. Common Tasks

| Task | Command/Location |
|------|------------------|
| Generate component | `./scripts/generate-component.sh` |
| Transform existing code | `./scripts/transform-to-opencog.py` |
| Test generation | `./scripts/test-generation.sh` |
| Modify templates | Edit files in `template/` |
| Add CMake module | Add to `template/cmake/` |
| Customize README | Edit `template/README.md` |
| Change CI config | Edit `template/.circleci/config.yml` |
| Update packaging | Edit files in `template/debian/` |

### 9. Reference Information

#### Component Naming Conventions
- **Format:** lowercase-with-hyphens
- **Examples:** `simple-utils`, `atom-analyzer`, `neural-symbolic`
- **Avoid:** CamelCase, underscores, spaces

#### Version Numbering
- **Format:** MAJOR.MINOR.PATCH (semantic versioning)
- **Initial:** 0.1.0 (pre-release) or 1.0.0 (stable)
- **Increment:** Follow semver rules

#### Dependency Specification
- **Format:** Comma-separated, lowercase
- **Example:** `cogutil,atomspace,ure`
- **Order:** CogUtil first, then others

#### File Naming
- **Headers:** `snake_case.h`
- **Implementation:** `snake_case.cc` or `snake_case.cpp`
- **Tests:** `ComponentUTest.cxxtest`

### 10. Troubleshooting Index

#### Build Issues
- **CogUtil not found** → Install CogUtil first
- **CMake fails** → Check CMakeLists.txt syntax
- **Linker errors** → Verify library dependencies

#### Test Issues
- **CxxTest not found** → Install cxxtest package
- **Tests don't build** → Check test CMakeLists.txt
- **Tests fail** → Review test logic and dependencies

#### Packaging Issues
- **Debian build fails** → Check debian/control dependencies
- **Install paths wrong** → Review CMake install directives
- **Package conflicts** → Remove old versions first

**See:** [USAGE.md](USAGE.md) Troubleshooting section for details

### 11. External Resources

- **OpenCog Wiki:** https://wiki.opencog.org/
- **AtomSpace:** https://github.com/opencog/atomspace
- **CogUtil:** https://github.com/opencog/cogutil
- **CMake Documentation:** https://cmake.org/documentation/
- **Semantic Versioning:** https://semver.org/
- **Mailing List:** opencog@googlegroups.com

### 12. Contributing to CogArc

Want to improve the archetype system?

1. Analyze more OpenCog components
2. Identify missing patterns
3. Update templates in `template/`
4. Test with `scripts/test-generation.sh`
5. Submit pull request

**See:** Main OCC repository CONTRIBUTING.md

---

## Quick Command Reference

### Most Common Commands

```bash
# Generate new component
./cogarc/scripts/generate-component.sh my-component "Description"

# Generate with dependencies
./cogarc/scripts/generate-component.sh my-component "Description" \
  --dependencies cogutil,atomspace

# Transform existing code
./cogarc/scripts/transform-to-opencog.py \
  --source /path/to/code \
  --component-name my-component \
  --description "Description"

# Test generation system
./cogarc/scripts/test-generation.sh

# Build generated component
cd my-component && mkdir build && cd build && cmake .. && make

# Run tests
cd build && make check
```

## Documentation Quick Links

- **Start Here:** [README.md](README.md)
- **Quick Reference:** [QUICKREF.md](QUICKREF.md)
- **Detailed Guide:** [USAGE.md](USAGE.md)
- **Examples:** [EXAMPLES.md](EXAMPLES.md)
- **This Index:** INDEX.md

---

**CogArc Version:** 1.0.0
**Last Updated:** 2024
**Maintainer:** OpenCog Community
