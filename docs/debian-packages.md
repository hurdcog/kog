# Debian Package Building

## Overview

The OpenCog Collection provides automated Debian package building through GitHub Actions. This allows easy distribution and installation of OpenCog components on Debian and Ubuntu-based systems.

## GitHub Action Workflow

The repository includes a GitHub Action workflow (`.github/workflows/debian-packages.yml`) that automatically builds Debian packages for all major OpenCog components.

### Build Commands

The workflow executes the following build sequence:

```bash
# Prerequisites
sudo apt-get install -y debhelper dpkg-dev cmake g++ \
  guile-3.0-dev libboost-all-dev binutils-dev libiberty-dev

# Build cogutil
cd cogutil
dpkg-buildpackage -us -uc -b

# Build atomspace (requires cogutil installed)
cd atomspace
dpkg-buildpackage -us -uc -b

# Build unify (requires cogutil, atomspace)
cd unify
dpkg-buildpackage -us -uc -b

# Build ure (requires cogutil, atomspace, unify)
cd ure
dpkg-buildpackage -us -uc -b

# Build cogserver (requires cogutil, atomspace)
cd cogserver
dpkg-buildpackage -us -uc -b

# Build attention (requires cogutil, atomspace, cogserver)
cd attention
dpkg-buildpackage -us -uc -b

# Build moses (requires cogutil)
cd moses
dpkg-buildpackage -us -uc -b

# Build asmoses (requires moses, atomspace)
cd asmoses
dpkg-buildpackage -us -uc -b

# Build miner (requires cogutil, atomspace, ure)
cd miner
dpkg-buildpackage -us -uc -b

# Build pln (requires cogutil, atomspace, ure)
cd pln
dpkg-buildpackage -us -uc -b
```

### Components Built

The workflow builds packages for the following components in dependency order:

1. **cogutil** - Foundation utilities library
2. **atomspace** - Hypergraph knowledge representation
3. **unify** - Unification algorithms
4. **ure** - Unified Rule Engine
5. **cogserver** - Network shell and server
6. **attention** - ECAN attention allocation
7. **moses** - Evolutionary optimization
8. **asmoses** - MOSES with AtomSpace integration
9. **miner** - Pattern mining
10. **pln** - Probabilistic Logic Networks

### Triggering Builds

The workflow runs automatically on:
- Push to `main` or `master` branches
- Pull requests to `main` or `master` branches

You can also trigger it manually:
1. Go to the "Actions" tab in GitHub
2. Select "Build Debian Packages"
3. Click "Run workflow"

### Downloading Packages

After a successful workflow run:

1. Go to the workflow run page
2. Scroll to the "Artifacts" section
3. Download the desired packages:
   - Individual component packages (e.g., `cogutil-debian-packages`)
   - Or download `all-debian-packages` for everything

### Installing Packages

Once downloaded, install in dependency order:

```bash
# Extract the artifact zip
unzip all-debian-packages.zip

# Install in order
sudo dpkg -i *cogutil*.deb
sudo dpkg -i *atomspace*.deb
sudo dpkg -i *.deb

# Fix any dependency issues
sudo apt-get install -f -y
```

## Manual Building

You can also build packages manually following the same process used by the workflow.

### Prerequisites

Install build dependencies:

```bash
sudo apt-get update
sudo apt-get install -y \
  debhelper \
  dpkg-dev \
  cmake \
  g++ \
  guile-3.0-dev \
  libboost-all-dev \
  binutils-dev \
  libiberty-dev \
  cxxtest \
  doxygen \
  graphviz \
  python3-dev \
  patchelf
```

### Build Process

For each component:

1. Copy the debian packaging files:
   ```bash
   cp -r opencog-debian/cogutil/debian cogutil/
   ```

2. Build the package:
   ```bash
   cd cogutil
   dpkg-buildpackage -us -uc -b
   ```

3. Install the package:
   ```bash
   sudo dpkg -i ../*.deb
   sudo apt-get install -f -y
   ```

4. Repeat for other components in dependency order

### Example Script

A reference script that displays the build commands is available:

```bash
./scripts/build-debian-packages-example.sh
```

**Note:** This script only displays the build commands for reference; it does not execute them. Use it to understand the build sequence and commands.

## Package Structure

### Runtime Packages

- `libcogutil` - Core utilities
- `libatomspace` - Hypergraph database
- `opencog-unify` - Unification
- `opencog-ure` - Rule engine
- `opencog-cogserver` - Server
- `opencog-attention` - Attention allocation
- `opencog-moses` - MOSES
- `opencog-asmoses` - ASMOSES
- `opencog-miner` - Pattern mining
- `opencog-pln` - PLN reasoning

### Development Packages

- `libcogutil-dev` - CogUtil headers and build files
- `libatomspace-dev` - AtomSpace headers and build files
- Additional `-dev` packages for other components

## Debian Packaging Files

Debian packaging metadata is maintained in the `opencog-debian/` directory:

```
opencog-debian/
├── cogutil/debian/
├── atomspace/debian/
├── unify/debian/
├── ure/debian/
├── cogserver/debian/
├── attention/debian/
├── moses/debian/
├── asmoses/debian/
├── miner/debian/
└── pln/debian/
```

Each directory contains:
- `control` - Package metadata and dependencies
- `rules` - Build rules
- `changelog` - Version history
- `compat` - Debhelper compatibility level
- `copyright` - License information
- `*.install` - File installation lists

## Troubleshooting

### Build Failures

If a build fails:

1. Check that the `debian/` directory exists in `opencog-debian/<component>/`
2. Verify all dependencies are installed
3. Ensure previous dependencies are built and installed
4. Check build logs in GitHub Actions for specific errors

### Dependency Issues

If installation fails:

```bash
# Fix broken dependencies
sudo apt-get install -f -y

# Or manually install missing dependencies
sudo apt-get install <missing-package>
```

### Missing debian/ Files

If you get "No debian/ directory found" warnings:

1. Ensure the component has packaging files in `opencog-debian/`
2. Check that the workflow correctly copies the files
3. Verify file permissions

## Documentation

For more details, see:
- [Debian Packages Workflow README](.github/workflows/DEBIAN_PACKAGES_README.md)
- [OpenCog Debian README](opencog-debian/README.md)
- [Debian Packaging Summary](DEBIAN_PACKAGING_SUMMARY.md)

## Contributing

To add Debian packaging for a new component:

1. Create `opencog-debian/<component>/debian/` directory
2. Add required Debian packaging files (control, rules, changelog, etc.)
3. Update the workflow to include the new component
4. Test the build locally before submitting a PR

## License

The Debian packaging files and workflows follow the same license as the OpenCog project (AGPL-3.0).
