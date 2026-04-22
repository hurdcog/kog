# Windows Build Guide for OpenCog Collection (OCC)

## Overview

This guide provides comprehensive instructions for building the OpenCog Collection on Windows using vcpkg for dependency management and CMake for building.

## Prerequisites

### Required Software

1. **Visual Studio 2022** (Community, Professional, or Enterprise)
   - Install "Desktop development with C++" workload
   - Include CMake tools for Windows
   - Include C++ CMake tools for Linux (optional, for WSL support)

2. **Git for Windows**
   - Download from: https://git-scm.com/download/win
   - Enable Git Bash during installation

3. **CMake** (version 3.20 or higher)
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

4. **vcpkg** (Microsoft C++ Package Manager)
   ```powershell
   git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
   cd C:\vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```

## Quick Start

### Option 1: Using vcpkg Manifest Mode (Recommended)

The repository includes `vcpkg.json` which automatically manages dependencies:

```powershell
# Clone the repository
git clone https://github.com/cogpy/occ.git
cd occ

# Configure with CMake (vcpkg will automatically install dependencies)
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Release

# Install (optional)
cmake --install build --prefix C:/OpenCog
```

### Option 2: Manual Dependency Installation

If you prefer to install dependencies manually:

```powershell
# Install core dependencies
vcpkg install boost:x64-windows
vcpkg install rocksdb:x64-windows
vcpkg install protobuf:x64-windows
vcpkg install grpc:x64-windows
vcpkg install catch2:x64-windows
vcpkg install spdlog:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install yaml-cpp:x64-windows

# Build the project
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## Building Individual Components

### Core Components Build Order

The OpenCog Collection consists of multiple components that must be built in dependency order:

#### 1. CogUtil (Foundation Library)

```powershell
cd cogutil
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
cmake --install build --prefix C:/OpenCog
```

#### 2. AtomSpace (Knowledge Representation)

```powershell
cd atomspace
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCogUtil_DIR=C:/OpenCog/lib/cmake/CogUtil
cmake --build build --config Release
cmake --install build --prefix C:/OpenCog
```

#### 3. URE (Unified Rule Engine)

```powershell
cd ure
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCogUtil_DIR=C:/OpenCog/lib/cmake/CogUtil `
  -DAtomSpace_DIR=C:/OpenCog/lib/cmake/AtomSpace
cmake --build build --config Release
cmake --install build --prefix C:/OpenCog
```

#### 4. CogServer (Network Server)

```powershell
cd cogserver
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCogUtil_DIR=C:/OpenCog/lib/cmake/CogUtil `
  -DAtomSpace_DIR=C:/OpenCog/lib/cmake/AtomSpace
cmake --build build --config Release
cmake --install build --prefix C:/OpenCog
```

#### 5. PLN (Probabilistic Logic Networks)

```powershell
cd pln
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCogUtil_DIR=C:/OpenCog/lib/cmake/CogUtil `
  -DAtomSpace_DIR=C:/OpenCog/lib/cmake/AtomSpace `
  -DURE_DIR=C:/OpenCog/lib/cmake/URE
cmake --build build --config Release
cmake --install build --prefix C:/OpenCog
```

## Build Configuration Options

### CMake Options

Common CMake configuration options:

```powershell
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Release `
  -DBUILD_SHARED_LIBS=OFF `
  -DBUILD_TESTING=ON `
  -DCMAKE_INSTALL_PREFIX=C:/OpenCog
```

### Available Options

- `BUILD_SHARED_LIBS`: Build shared libraries (ON/OFF, default: OFF on Windows)
- `BUILD_TESTING`: Build unit tests (ON/OFF, default: ON)
- `WITH_GUILE`: Enable Guile Scheme bindings (OFF on Windows, requires WSL)
- `WITH_PYTHON`: Enable Python bindings (ON/OFF, default: ON if Python found)
- `CMAKE_INSTALL_PREFIX`: Installation directory (default: C:/Program Files/OpenCog)

## Troubleshooting

### Common Issues

#### Issue 1: vcpkg Integration Not Found

**Error**: `Could not find vcpkg toolchain file`

**Solution**:
```powershell
# Ensure vcpkg is integrated
cd C:\vcpkg
.\vcpkg integrate install

# Or specify the toolchain file explicitly
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

#### Issue 2: Boost Not Found

**Error**: `Could not find Boost`

**Solution**:
```powershell
# Install Boost with required features
vcpkg install boost-system:x64-windows boost-filesystem:x64-windows boost-thread:x64-windows
```

#### Issue 3: Long Path Issues

**Error**: `The system cannot find the path specified`

**Solution**: Enable long path support in Windows:
```powershell
# Run as Administrator
New-ItemProperty -Path "HKLM:\SYSTEM\CurrentControlSet\Control\FileSystem" `
  -Name "LongPathsEnabled" -Value 1 -PropertyType DWORD -Force
```

#### Issue 4: Permission Denied During Build

**Solution**: Run Visual Studio or PowerShell as Administrator

#### Issue 5: Missing DLL at Runtime

**Solution**: Add vcpkg installed bin directory to PATH:
```powershell
$env:PATH += ";C:\vcpkg\installed\x64-windows\bin"
```

## Advanced Configuration

### Using Ninja Build System

For faster builds, use Ninja instead of MSBuild:

```powershell
# Install Ninja via vcpkg
vcpkg install ninja:x64-windows

# Configure with Ninja
cmake -B build -S . `
  -G Ninja `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build
```

### Cross-Compilation for ARM64

```powershell
# Install ARM64 dependencies
vcpkg install boost:arm64-windows

# Configure for ARM64
cmake -B build-arm64 -S . `
  -A ARM64 `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build-arm64 --config Release
```

### Building with Clang on Windows

```powershell
# Install LLVM/Clang
vcpkg install llvm:x64-windows

# Configure with Clang
cmake -B build -S . `
  -T ClangCL `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build
cmake --build build --config Release
```

## Development Workflow

### Setting Up Development Environment

1. **Install Visual Studio Code** (optional but recommended)
   ```powershell
   winget install Microsoft.VisualStudioCode
   ```

2. **Install VS Code Extensions**:
   - C/C++ Extension Pack
   - CMake Tools
   - CMake Language Support

3. **Configure VS Code Settings** (`.vscode/settings.json`):
   ```json
   {
     "cmake.configureSettings": {
       "CMAKE_TOOLCHAIN_FILE": "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
     },
     "cmake.buildDirectory": "${workspaceFolder}/build",
     "cmake.installPrefix": "C:/OpenCog"
   }
   ```

### Running Tests

```powershell
# Build with tests enabled
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DBUILD_TESTING=ON

cmake --build build --config Release

# Run tests
cd build
ctest -C Release --output-on-failure
```

### Debugging

```powershell
# Build in Debug mode
cmake -B build-debug -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_BUILD_TYPE=Debug

cmake --build build-debug --config Debug

# Debug with Visual Studio
devenv build-debug\OpenCog.sln
```

## Integration with WSL (Windows Subsystem for Linux)

For components that require Linux-specific features (like Guile), you can use WSL:

### Setup WSL2

```powershell
# Install WSL2
wsl --install -d Ubuntu-22.04

# Enter WSL
wsl
```

### Build in WSL

```bash
# Inside WSL
cd /mnt/c/path/to/occ

# Follow Linux build instructions
sudo apt-get update
sudo apt-get install -y build-essential cmake libboost-all-dev guile-3.0-dev

# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Performance Optimization

### Compiler Optimization Flags

```powershell
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCMAKE_CXX_FLAGS="/O2 /GL /arch:AVX2" `
  -DCMAKE_BUILD_TYPE=Release
```

### Parallel Build

```powershell
# Use all available cores
cmake --build build --config Release --parallel

# Or specify number of cores
cmake --build build --config Release --parallel 8
```

## Packaging for Distribution

### Creating Windows Installer

```powershell
# Install NSIS
vcpkg install nsis:x64-windows

# Configure with CPack
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake `
  -DCPACK_GENERATOR=NSIS

# Build
cmake --build build --config Release

# Create installer
cd build
cpack -C Release
```

## Environment Variables

Set these environment variables for easier development:

```powershell
# Add to PowerShell profile ($PROFILE)
$env:VCPKG_ROOT = "C:\vcpkg"
$env:OPENCOG_HOME = "C:\OpenCog"
$env:PATH += ";$env:OPENCOG_HOME\bin;$env:VCPKG_ROOT\installed\x64-windows\bin"
```

## Additional Resources

- **OpenCog Wiki**: https://wiki.opencog.org/
- **vcpkg Documentation**: https://vcpkg.io/
- **CMake Documentation**: https://cmake.org/documentation/
- **Visual Studio Documentation**: https://docs.microsoft.com/visualstudio/

## Support

For issues specific to Windows builds:
1. Check the GitHub Issues: https://github.com/cogpy/occ/issues
2. OpenCog Mailing List: https://groups.google.com/g/opencog
3. OpenCog Discord: https://discord.gg/opencog

## License

This build system and documentation are part of the OpenCog Collection and are licensed under AGPL-3.0.
