# GNU Guix OpenCog Packages

This directory contains GNU Guix package definitions for OpenCog components following the GNU Guix packaging standards.

## Package Definitions

The `opencog.scm` file provides the following package definitions:

### Core Packages

- **cogutil** (`cogutil`)
  - Low-level C++ programming utilities used by OpenCog components
  - Git repository: https://github.com/opencog/cogutil
  - License: AGPL3 or Apache 2.0

- **atomspace** (`atomspace`)  
  - OpenCog hypergraph database, query system and rule engine
  - Git repository: https://github.com/opencog/atomspace
  - License: AGPL3
  - Dependencies: cogutil, boost, gmp, guile-3.0, postgresql

- **cogserver** (`cogserver`)
  - OpenCog network and job server
  - Git repository: https://github.com/opencog/cogserver  
  - License: AGPL3
  - Dependencies: atomspace, cogutil, boost, gmp, guile-3.0

- **attention** (`attention`)
  - OpenCog attention allocation subsystem
  - Git repository: https://github.com/opencog/attention
  - License: AGPL3
  - Dependencies: atomspace, cogserver, cogutil, boost, gmp, guile-3.0

### Main Framework

- **opencog** (`opencog`)
  - Framework for integrated artificial intelligence
  - Git repository: https://github.com/opencog/opencog
  - License: AGPL3
  - Dependencies: All above components plus link-grammar and util-linux

### Research Tools

- **agi-bio** (`agi-bio`)
  - Genomic and proteomic data exploration and pattern mining
  - Git repository: https://github.com/opencog/agi-bio
  - License: AGPL3
  - Dependencies: atomspace, cogutil, gmp, guile-3.0

## Usage

### Building Individual Packages

```bash
# Build specific packages
guix build -L gnu/packages cogutil
guix build -L gnu/packages atomspace  
guix build -L gnu/packages cogserver
guix build -L gnu/packages attention
guix build -L gnu/packages opencog
guix build -L gnu/packages agi-bio
```

### Installing Packages

```bash
# Install individual components
guix install -L gnu/packages cogutil
guix install -L gnu/packages atomspace

# Install the complete framework
guix install -L gnu/packages opencog
```

### Creating a Development Environment

```bash
# Create a development shell with OpenCog packages
guix shell -L gnu/packages cogutil atomspace cogserver opencog
```

### Adding to Guix Channels

To use these packages as a Guix channel, add the following to your `~/.config/guix/channels.scm`:

```scheme
(cons* (channel
        (name 'opencog-collection)
        (url "https://github.com/cogpy/occ.git"))
       %default-channels)
```

Then update channels and install:

```bash
guix pull
guix install cogutil atomspace opencog
```

## Package Information

### Dependencies

All packages are built using the CMake build system and require:

- **Build tools**: cmake, pkg-config
- **Testing**: cxxtest  
- **Python**: python-minimal (for build scripts)
- **C++ libraries**: boost, gmp
- **Scheme**: guile-3.0

### Build Configuration

The packages are configured with:
- Guile 3.0 support enabled
- Tests enabled where available (some tests are disabled due to known failures)
- Proper installation paths for Guile modules

### License Information

Most packages are licensed under AGPL3, with cogutil offering a choice between AGPL3 and Apache 2.0.

## Contributing

When updating package definitions:

1. Update commit hashes and SHA256 checksums for new versions
2. Test builds locally before committing
3. Follow GNU Guix packaging guidelines
4. Update dependencies as needed
5. Maintain compatibility with current Guix and Guile versions

## Integration with Repository

These package definitions integrate with the OpenCog Collection repository structure:

- Used by the main `guix.scm` build file
- Referenced in the Guix manifest (`.guix/manifest.scm`)
- Part of the Guix channel definition (`.guix-channel`)
- Compatible with the development container setup