# AGI-OS Integration Guide: OCC + HurdCog + Cognumach

## 1. Introduction

This guide provides a comprehensive overview of the integrated AGI-OS, a vertically integrated stack combining the **OpenCog Collection (OCC)**, **HurdCog**, and **Cognumach**. This system represents the world's first complete AGI operating system, from the microkernel to the cognitive framework, designed for research and development in artificial general intelligence.

### 1.1. System Components

-   **Cognumach**: An enhanced version of the GNU Mach microkernel, providing the low-level foundation for the OS.
-   **HurdCog**: A cognitive operating system based on GNU Hurd, with integrated OpenCog components for learning, reasoning, and adaptation.
-   **OCC (OpenCog Collection)**: A comprehensive AGI research platform with a rich set of tools for cognitive synergy.

### 1.2. Architecture Overview

The integrated system follows a three-layer architecture:

```
┌─────────────────────────────────────────────────────────┐
│                    Layer 3: OCC                         │
│  (Cognitive Architecture & AGI Research Platform)       │
└─────────────────────────────────────────────────────────┘
                          ↕
┌─────────────────────────────────────────────────────────┐
│                  Layer 2: HurdCog                       │
│  (Cognitive Operating System Services)                  │
└─────────────────────────────────────────────────────────┘
                          ↕
┌─────────────────────────────────────────────────────────┐
│                 Layer 1: Cognumach                      │
│  (Enhanced Microkernel Foundation)                      │
└─────────────────────────────────────────────────────────┘
```

## 2. Build and Installation

### 2.1. Prerequisites

Before building the integrated system, you must install the required dependencies. A setup script is provided for this purpose.

```bash
# From the agi-os-integration directory
make dev-setup
```

This will install `build-essential`, `gcc-multilib`, `guile-3.0`, `cmake`, and other necessary tools.

### 2.2. Building with GNU Guix (Recommended)

The recommended method for building the AGI-OS is using GNU Guix, which provides a reproducible build environment. The entire stack can be built with a single command:

```bash
# From the agi-os-integration directory
make guix-build
```

This will use the `occ-hurdcog-unified.scm` file to build all three components and link them together.

### 2.3. Direct Build (for Development)

For development purposes, you can build the components directly without Guix. The `Makefile` provides a `quickstart` target that automates this process:

```bash
# From the agi-os-integration directory
make quickstart
```

This will build each component in the correct order and run the integration tests.

### 2.4. Installation

Once the system is built, you can install it to your system using the `install` target. This requires `sudo` and GNU Guix.

```bash
# From the agi-os-integration directory
make install
```

This will install the AGI-OS to `/usr/local` and create the necessary environment profiles.

## 3. Testing the Integration

A comprehensive test suite is provided to validate the integration. You can run the tests using the `test` target in the `Makefile`.

```bash
# From the agi-os-integration directory
make test
```

This will run a series of tests to ensure that all components are correctly integrated and communicating with each other.

### 3.1. Test Suite Overview

The test suite covers the following areas:

-   **Cognumach Headers**: Verifies that the cognitive headers for AtomSpace IPC and VM have been correctly installed.
-   **HurdCog Integration Layer**: Checks for the presence of the `machspace-bridge.scm` module.
-   **OCC Integration Layer**: Checks for the presence of the `atomspace-hurdcog-bridge.py` script.
-   **Python Bridge Functionality**: Executes the Python bridge to ensure it can connect to the (simulated) HurdCog kernel.
-   **Guile Bridge Functionality**: Verifies that the Guile interpreter is functional and can load the necessary modules.

## 4. Developer's Guide

### 4.1. Working with the Integrated Stack

Once the AGI-OS is installed, you can start developing with it. The environment profile in `/etc/profile.d/agi-os.sh` sets up the necessary environment variables.

-   `$AGI_OS_ROOT`: The root directory of the integrated system.
-   `$HURDCOG_ROOT`: The root directory of the HurdCog installation.
-   `$COGNUMACH_ROOT`: The root directory of the Cognumach installation.

### 4.2. Starting the System

You can start the HurdCog Master Control Dashboard with the following command:

```bash
/home/ubuntu/hurdcog/cogkernel/start-dashboard.sh
```

This will launch the dashboard, which you can access at `http://localhost:8080/dashboard`.

### 4.3. Modifying Components

If you modify any of the components, you will need to rebuild the system. Use the `make guix-build` or `make direct-build` command to do so.

-   **Cognumach Changes**: If you modify the microkernel, you will need to rebuild `cognumach` and then the entire stack.
-   **HurdCog Changes**: If you modify the HurdCog cognitive kernel, you will need to rebuild `hurdcog` and `occ-hurdcog-unified`.
-   **OCC Changes**: If you modify the OCC framework, you will need to rebuild `occ-hurdcog-unified`.

## 5. Conclusion

This integrated AGI-OS provides a powerful and unique platform for research and development in artificial general intelligence. By combining a state-of-the-art microkernel, a cognitive operating system, and a comprehensive AGI framework, it opens up new possibilities for building intelligent systems.
