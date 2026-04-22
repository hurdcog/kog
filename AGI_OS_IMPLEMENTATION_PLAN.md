# AGI-OS Integration Packages: Source Code Implementation Plan

**Version**: 1.0  
**Date**: December 6, 2025  
**Status**: Planning Document

## 1. Introduction

This document provides a comprehensive plan for implementing the source code for the three new AGI-OS integration packages: `cognumach-cognitive-scheduler`, `agi-os-monitoring`, and `agi-os-cognitive-init`. The plan is based on the detailed architecture defined in `IMPLEMENTATION_ARCHITECTURE.md` and is designed to guide the development, testing, and release of these critical components.

## 2. Project Goals and Objectives

The primary goal of this project is to transition the AGI-OS from a collection of components with packaging infrastructure to a fully functional, cognitive operating system. This will be achieved by implementing the source code for the three core integration packages that enable deep cognitive synergy across the system stack.

**Objectives**:
- To implement a cognitive-aware scheduler in the Cognumach microkernel.
- To develop a unified, real-time monitoring system for the entire AGI-OS.
- To create an intelligent initialization system that bootstraps the AGI-OS with cognitive awareness.
- To ensure all new components are fully tested and integrated with the existing infrastructure.

## 3. Overall Project Milestones

The project is divided into five major milestones, with an estimated total duration of 16 weeks.

| Milestone | Description | Estimated Duration | Deliverables |
|---|---|---|---|
| **M1** | `cognumach-cognitive-scheduler` Implementation | 5 Weeks | - Source code for kernel modifications<br>- Source code for user-space daemon<br>- Unit tests | 
| **M2** | `agi-os-monitoring` Implementation | 5 Weeks | - Source code for data collectors<br>- Source code for backend and frontend<br>- Unit tests | 
| **M3** | `agi-os-cognitive-init` Implementation | 5 Weeks | - Source code for init system<br>- YAML service definition schema<br>- Unit tests | 
| **M4** | Integration Testing and System Validation | 4 Weeks | - Integration test suite<br>- System validation report<br>- Bug fixes and performance tuning | 
| **M5** | Documentation and Final Release | 1 Week | - Final source code release<br>- Updated developer documentation<br>- User guides | 

## 4. Detailed Implementation Plan

This section breaks down the implementation of each package into detailed tasks.

### 4.1. Milestone 1: `cognumach-cognitive-scheduler` (Weeks 1-5)

| Task ID | Task Description | Language | Estimated Effort | Deliverable |
|---|---|---|---|---|
| 1.1 | Modify Cognumach `task` struct and `sched_prim.c` to include `attention_value`. | C | 5 days | Patched kernel source |
| 1.2 | Implement Mach IPC interface for secure attention value updates. | C | 4 days | Kernel IPC code |
| 1.3 | Implement the cognitive workload classifier heuristic within the kernel. | C | 3 days | Kernel classifier code |
| 1.4 | Implement the `cognumach_set_sched_policy` syscall and policy engine. | C | 3 days | Kernel syscall code |
| 1.5 | Develop the `cognosched-daemon` for learning-based policy management. | C++ | 5 days | Daemon source code |
| 1.6 | Develop the `libsched-api.so` shared library for user-space hints. | C++ | 2 days | Library source code |
| 1.7 | Develop comprehensive unit tests for all kernel and user-space components. | C/C++ | 3 days | Unit test suite |

### 4.2. Milestone 2: `agi-os-monitoring` (Weeks 4-8)

| Task ID | Task Description | Language | Estimated Effort | Deliverable |
|---|---|---|---|---|
| 2.1 | Develop the Kernel Collector and `/dev/cognostats` character device. | C/Python | 5 days | Kernel driver and Python collector |
| 2.2 | Develop the HurdCog Collector to monitor translators and MachSpace. | Python | 4 days | Collector source code |
| 2.3 | Develop the OCC Collector to monitor the AtomSpace event stream. | Python | 4 days | Collector source code |
| 2.4 | Implement the Aggregator Service and time-series database storage. | Python | 4 days | Aggregator source code |
| 2.5 | Develop the Flask backend API server and WebSocket endpoint. | Python | 4 days | API server source code |
| 2.6 | Develop the JavaScript frontend dashboard with interactive visualizations. | JavaScript | 5 days | Frontend source code |
| 2.7 | Develop unit tests for all collectors, services, and API endpoints. | Python/JS | 4 days | Unit test suite |

### 4.3. Milestone 3: `agi-os-cognitive-init` (Weeks 7-11)

| Task ID | Task Description | Language | Estimated Effort | Deliverable |
|---|---|---|---|---|
| 3.1 | Implement the `cognitive_init.py` core process to run as PID 1. | Python | 5 days | Core init source code |
| 3.2 | Develop the Service Manager with cognitive dependency graph logic. | Python | 5 days | Service manager code |
| 3.3 | Implement the Knowledge Loader for loading and validating Atomese files. | Python | 4 days | Knowledge loader code |
| 3.4 | Develop the Hardware Profiler module for dynamic configuration. | Python | 3 days | Profiler source code |
| 3.5 | Finalize the YAML service definition schema and create example files. | YAML | 2 days | Schema documentation |
| 3.6 | Develop unit tests for all components, including dependency resolution. | Python | 4 days | Unit test suite |

## 5. Project Timeline (Gantt Chart)

```
+-------------------------------------+---+---+---+---+---+---+---+---+---+----+----+----+----+----+----+----+
| Milestone / Week                    | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 |
+-------------------------------------+---+---+---+---+---+---+---+---+---+----+----+----+----+----+----+----+
| M1: cognumach-cognitive-scheduler   |███|███|███|███|███|   |   |   |   |    |    |    |    |    |    |    |
| M2: agi-os-monitoring               |   |   |   |███|███|███|███|███|   |    |    |    |    |    |    |    |
| M3: agi-os-cognitive-init           |   |   |   |   |   |   |███|███|███|███ |███ |    |    |    |    |    |
| M4: Integration Testing             |   |   |   |   |   |   |   |   |   |    |    |████|████|████|████|    |
| M5: Documentation & Release         |   |   |   |   |   |   |   |   |   |    |    |    |    |    |    |████|
+-------------------------------------+---+---+---+---+---+---+---+---+---+----+----+----+----+----+----+----+
```

## 6. Testing and Validation Strategy

- **Unit Testing**: Each function and class will be tested in isolation. The `cxxtest` framework will be used for C++ components, and Python's `unittest` framework will be used for Python components.
- **Integration Testing (Milestone 4)**: A dedicated test suite will be developed to validate the interactions between the new packages and the existing AGI-OS components. This will include testing the flow of attention values from the AtomSpace down to the kernel scheduler and the flow of monitoring data back up to the dashboard.
- **System Testing (Milestone 4)**: The entire AGI-OS will be booted and run under various cognitive workloads to validate system stability, performance, and correctness. The `synergy_validate_agi_os.sh` script will be extended to include tests for the new source code functionalities.

## 7. Deliverables

Upon completion of this project, the following deliverables will be provided:

1.  **Source Code**: Complete, commented, and version-controlled source code for all three integration packages.
2.  **Unit Test Suites**: Comprehensive unit tests for all new components.
3.  **Integration Test Suite**: A new test suite for validating system-wide cognitive synergy.
4.  **Updated Debian Packages**: The existing Debian packages will be updated to include the newly implemented source code.
5.  **Developer Documentation**: Detailed documentation for the APIs and architecture of the new components.
6.  **User Guides**: Basic user guides for the `agi-os-monitoring` dashboard and for configuring the `agi-os-cognitive-init` system.

## 8. Conclusion

This implementation plan provides a clear and structured path to completing the source code for the new AGI-OS integration packages. By following this plan, we can ensure a high-quality, well-tested, and on-time delivery of the components required to realize the full vision of a cognitive operating system.
