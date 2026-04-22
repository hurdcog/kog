---
title: "Integrating Kokkos into Your Project - Kokkos documentation"
source: "https://kokkos.org/kokkos-core-wiki/get-started/integrating-kokkos-into-your-cmake-project.html"
author:
published:
created: 2025-08-03
description:
tags:
  - "clippings"
---
## Integrating Kokkos into Your Project

This document describes how to integrate the Kokkos library into your CMake project.

Kokkos provides the `Kokkos::kokkos` target, which simplifies the process by automatically handling necessary include directories, link libraries, compiler options, and other usage requirements.

Here are several integration methods, each with its own advantages:

## 2\. Embedded Kokkos via add\_subdirectory() and Git Submodules

This method embeds the Kokkos source code directly into your project. It’s useful when you need very tight control over the Kokkos version or when you can’t install Kokkos separately.

1. Add Kokkos as a [Git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules):

```sh
MyProject> git submodule add -b 4.5.01 https://github.com/kokkos/kokkos.git tpls/kokkos
MyProject> git commit -m 'Adding Kokkos v4.5.1 as a submodule'
```

`tpls/kokkos/` should now contain the full Kokkos source tree.

1. Use `add_subdirectory()` in your CMakeLists.txt:

```cmake
add_subdirectory(tpls/kokkos)
# ...
target_link_libraries(MyTarget PRIVATE Kokkos::kokkos)
```

## 3\. Embedded Kokkos via FetchContent

[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) simplifies the process of downloading and including Kokkos as a dependency during the CMake configuration stage.

```cmake
include(FetchContent)
FetchContent_Declare(
    Kokkos
    URL      https://github.com/kokkos/kokkos/releases/download/4.5.01/kokkos-4.5.01.tar.gz
    URL_HASH SHA256=52d003ffbbe05f30c89966e4009c017efb1662b02b2b73190670d3418719564c
)
FetchContent_MakeAvailable(Kokkos)
# ...
target_link_libraries(MyTarget PRIVATE Kokkos::kokkos)
```

- `URL_HASH` is highly recommended for verifying the integrity of the downloaded archive. You can find the SHA256 checksums for Kokkos releases in the `kokkos-X.Y.Z-SHA-256.txt` file on the [Kokkos releases page](https://github.com/kokkos/kokkos/releases).

## 4\. Supporting Both External and Embedded Kokkos

This approach allows your project to use either an external Kokkos installation or an embedded version, providing flexibility for different build environments.

```cmake
find_package(Kokkos CONFIG) # Try to find Kokkos externally
if(Kokkos_FOUND)
    message(STATUS "Found Kokkos: ${Kokkos_DIR} (version \"${Kokkos_VERSION}\")")
else()
    message(STATUS "Kokkos not found externally. Fetching via FetchContent.")
    include(FetchContent)
    FetchContent_Declare(
        Kokkos
        URL https://github.com/kokkos/kokkos/archive/refs/tags/4.4.01.tar.gz
    )
    FetchContent_MakeAvailable(Kokkos)
endif()
# ...
target_link_libraries(MyTarget PRIVATE Kokkos::kokkos)
```

Controlling the Kokkos integration:

- [CMAKE\_DISABLE\_FIND\_PACKAGE\_Kokkos](https://cmake.org/cmake/help/latest/variable/CMAKE_DISABLE_FIND_PACKAGE_PackageName.html): Set this variable to `TRUE` to force the use of the embedded Kokkos, even if an external installation is found.
- [CMAKE\_REQUIRE\_FIND\_PACKAGE\_Kokkos](https://cmake.org/cmake/help/latest/variable/CMAKE_REQUIRE_FIND_PACKAGE_PackageName.html): Set this variable to `TRUE` to require an external Kokkos installation. The build will fail if Kokkos is not found.
- `Kokkos_ROOT`: Use this variable to specify the directory where CMake should search for Kokkos when using `find_package()`.

For example:

```sh
cmake -DCMAKE_REQUIRE_FIND_PACKAGE_Kokkos=ON -DKokkos_ROOT=/path/to/kokkos/install/dir
```

or

```sh
cmake -DCMAKE_DISABLE_FIND_PACKAGE_Kokkos=ON
```