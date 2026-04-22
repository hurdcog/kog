# @COMPONENT_NAME_CAMEL@
@COMPONENT_BADGE@

@COMPONENT_DESCRIPTION@

This is an OpenCog component following the standard OpenCog architecture patterns.

## Overview

@COMPONENT_OVERVIEW@

## Main Project Site

The main OpenCog project site is at http://opencog.org

## Prerequisites

To build @COMPONENT_NAME_CAMEL@, the following packages are required:

### Required Dependencies

###### cmake
> Build management tool; v3.12 or higher recommended.
> http://www.cmake.org/ | `apt-get install cmake`

###### cogutil
> OpenCog common utilities
> https://github.com/opencog/cogutil | Must be installed first

@REQUIRED_DEPENDENCIES@

### Optional Dependencies

###### cxxtest
> Unit test framework
> https://cxxtest.com/ | `apt-get install cxxtest`

###### guile
> Scheme scripting language (version 3.0 or higher)
> https://www.gnu.org/software/guile/ | `apt-get install guile-3.0-dev`

###### python3
> Python language bindings
> https://www.python.org/ | `apt-get install python3-dev cython3`

###### boost
> C++ utility library
> https://www.boost.org/ | `apt-get install libboost-dev`

###### doxygen
> Documentation generator
> http://www.doxygen.org/ | `apt-get install doxygen`

@OPTIONAL_DEPENDENCIES@

## Building @COMPONENT_NAME_CAMEL@

### Quick Build

Perform the following steps at the shell prompt:

```bash
cd @COMPONENT_NAME@
mkdir build
cd build
cmake ..
make -j$(nproc)
```

Libraries will be built into subdirectories within build, mirroring the structure of the source directory root.

### Install

After building, install the library:

```bash
sudo make install
```

By default, this will install to `/usr/local`. To specify a different installation prefix:

```bash
cmake -DCMAKE_INSTALL_PREFIX=/your/prefix ..
```

### Build Types

You can specify the build type during configuration:

- **Release** (default, optimized): `cmake -DCMAKE_BUILD_TYPE=Release ..`
- **Debug** (debugging symbols): `cmake -DCMAKE_BUILD_TYPE=Debug ..`
- **RelWithDebInfo** (optimized with debug info): `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..`
- **Coverage** (code coverage): `cmake -DCMAKE_BUILD_TYPE=Coverage ..`
- **Profile** (profiling): `cmake -DCMAKE_BUILD_TYPE=Profile ..`

## Testing

### Unit Tests

To build and run the unit tests, from the `./build` directory:

```bash
make check
```

Or using CTest:

```bash
make test
```

Run tests in parallel:

```bash
make test ARGS=-j4
```

Run specific tests:

```bash
ctest -R TestName
```

### Code Coverage

To generate code coverage reports:

```bash
mkdir build-coverage
cd build-coverage
cmake -DCMAKE_BUILD_TYPE=Coverage ..
make -j$(nproc)
make check
# Coverage report will be generated
```

## Usage

### C++ API

```cpp
#include <opencog/@COMPONENT_NAME@/example.h>

// Your code here
```

### Scheme API

If Guile bindings are built:

```scheme
(use-modules (opencog) (opencog @COMPONENT_NAME@))

; Your Scheme code here
```

### Python API

If Python bindings are built:

```python
from opencog.@COMPONENT_NAME@ import *

# Your Python code here
```

## Examples

Example code can be found in the `examples/` directory.

@USAGE_EXAMPLES@

## Documentation

### API Documentation

Generate API documentation with Doxygen:

```bash
cd build
make doxygen
```

The generated documentation will be in `build/doc/html/`.

### Additional Resources

- [OpenCog Wiki](https://wiki.opencog.org/)
- [AtomSpace documentation](https://wiki.opencog.org/w/AtomSpace)
- [Component-specific documentation](doc/)

## Architecture

@ARCHITECTURE_NOTES@

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Workflow

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Ensure all tests pass
6. Submit a pull request

### Code Style

- Follow existing code style in the project
- Use consistent indentation (tabs or spaces as per project convention)
- Write clear, descriptive commit messages
- Document public APIs with comments

## Continuous Integration

This project uses CircleCI for continuous integration. The CI pipeline:

- Builds the project on every commit
- Runs all unit tests
- Checks code style (if configured)
- Generates test reports

## Debian Packaging

Debian packages can be built:

```bash
cd build
cpack -G DEB
```

This generates:
- `@COMPONENT_NAME@_@VERSION@_amd64.deb` - Runtime library
- `@COMPONENT_NAME@-dev_@VERSION@_amd64.deb` - Development headers

## Docker

A Docker container is available for easy deployment:

```bash
docker pull opencog/@COMPONENT_NAME@
docker run -it opencog/@COMPONENT_NAME@
```

## License

This project is licensed under the GNU Affero General Public License v3.0 - see the [LICENSE](LICENSE) file for details.

## Authors

See [AUTHORS](AUTHORS) file for the list of contributors.

## Acknowledgments

@ACKNOWLEDGMENTS@

## References

@REFERENCES@

## Support

For questions, issues, or contributions:

- GitHub Issues: https://github.com/opencog/@COMPONENT_NAME@/issues
- Mailing List: opencog@googlegroups.com
- IRC: #opencog on Libera.Chat
- Forum: https://groups.google.com/g/opencog

## Version History

See [CHANGELOG.md](CHANGELOG.md) for version history and release notes.
