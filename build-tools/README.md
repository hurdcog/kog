# OCC Build Tools

This directory contains centralized build tools used across the AGI-OS stack.

## MIG (Mach Interface Generator)

**Location**: `build-tools/mig/`

**Purpose**: MIG is the Mach Interface Generator, used to generate IPC (Inter-Process Communication) stubs for Mach-based systems.

**Used By**:
- `cognumach/` - GNU Mach microkernel (symlink: `cognumach/mig -> ../build-tools/mig`)
- `hurdcog/` - GNU Hurd + cognitive extensions (symlink: `hurdcog/mig -> ../build-tools/mig`)

**Build System**: Autotools (autoconf, automake, libtool)

### Building MIG

MIG uses the GNU Autotools build system:

```bash
cd build-tools/mig
./configure --prefix=/usr/local
make
sudo make install
```

### Architecture

MIG is centralized here to:
1. Avoid duplication between cognumach and hurdcog
2. Ensure consistent MIG version across all Mach-based components
3. Simplify maintenance and updates
4. Follow the layered AGI-OS architecture

## Integration with CMake

The root `CMakeLists.txt` references this directory for MIG when building Layer 0 (Build Tools):

```cmake
# Layer 0: Build Tools - MIG (Mach Interface Generator)
IF(BUILD_COGNUMACH OR BUILD_HURDCOG)
    MESSAGE(STATUS "Building Layer 0: MIG (Mach Interface Generator)...")
    MESSAGE(STATUS "  Location: build-tools/mig/")
    MESSAGE(STATUS "  Note: Centralized MIG for all Mach-based components")
    ...
ENDIF()
```

## Future Build Tools

This directory may contain additional build tools in the future:
- Cross-compilation toolchains
- Custom code generators
- Build orchestration scripts
- AGI-OS specific utilities

## Maintenance

When updating MIG:
1. Update the source in `build-tools/mig/`
2. The symlinks in `cognumach/mig` and `hurdcog/mig` will automatically point to the updated version
3. No need to update multiple copies

## References

- [GNU Mach](https://www.gnu.org/software/hurd/microkernel/mach/gnumach.html)
- [MIG Documentation](https://www.gnu.org/software/hurd/microkernel/mach/mig.html)
- [AGI-OS Architecture](../opencog-debian/AGI_OS_INTEGRATION.md)
