# `agi-os-cognitive-init`: Technical Specification

**Version**: 1.0  
**Date**: December 6, 2025  
**Status**: Implementation Specification

## 1. Introduction

This document provides the detailed technical specification for the `agi-os-cognitive-init` package. This package implements an intelligent and adaptive initialization system for the AGI-OS, replacing the traditional `init` process. It is responsible for bootstrapping the entire operating system with cognitive awareness, orchestrating service startup based on both traditional and cognitive dependencies.

## 2. Goals and Core Features

The primary goal is to create a dynamic, dependency-aware init system that understands the cognitive state of the AGI-OS.

- **Cognitive-Aware Service Orchestration**: Start, stop, and manage system services based on the presence of specific cognitive states (e.g., the existence of a `ConceptNode` in the AtomSpace).
- **Initial Knowledge Base Loading**: Load and validate the initial knowledge base required for the system to begin cognitive operations.
- **Hardware-Adaptive Configuration**: Profile the underlying hardware at boot time and dynamically configure system parameters for optimal performance.
- **Resilience and Fault Tolerance**: Gracefully handle service failures and ensure the system can boot into a safe, minimal state.

## 3. Architecture and Components

The `agi-os-cognitive-init` system is a Python-based application designed to run as PID 1. It is event-driven, reacting to both system signals and changes in the AtomSpace.

### 3.1. Core Components (Language: Python)

- **Init Core (`cognitive_init.py`)**: The main entry point that runs as PID 1. It is responsible for parsing configuration, setting up the main event loop, and managing the overall boot process.

- **Service Manager (`service_manager.py`)**: A central class that manages the lifecycle of all system services. It builds a dependency graph from service definition files and orchestrates the startup sequence.

- **Knowledge Loader (`knowledge_loader.py`)**: A module responsible for loading the initial knowledge base from Atomese files into the AtomSpace. It performs validation and integrity checks.

- **Hardware Profiler (`hardware_profiler.py`)**: A module that runs at the beginning of the boot process to gather information about the hardware (CPU, memory, GPU) and populate a `HardwareNode` in the AtomSpace.

- **Dependency Resolver (`dependency_resolver.py`)**: A component that resolves both traditional (service-based) and cognitive (AtomSpace-based) dependencies for each service.

- **AtomSpace Monitor (`atomspace_monitor.py`)**: A dedicated thread that subscribes to the AtomSpace event stream and triggers service state changes when cognitive dependencies are met.

### 3.2. Directory Structure

```
/usr/sbin/cognitive-init
/usr/lib/python3/dist-packages/cognitive_init/
    __init__.py
    main.py
    service_manager.py
    knowledge_loader.py
    hardware_profiler.py
    dependency_resolver.py
    atomspace_monitor.py
    config.py
/etc/cognitive-init/
    init.yml
    services.d/
        00-core.yml
        10-hurdcog.yml
        20-occ.yml
/var/lib/cognitive-init/knowledge/
    initial_kb.scm
```

## 4. Configuration

The system is configured through a set of YAML files located in `/etc/cognitive-init/`.

### 4.1. Main Configuration (`init.yml`)

```yaml
knowledge_base_path: /var/lib/cognitive-init/knowledge/initial_kb.scm
service_definitions_path: /etc/cognitive-init/services.d/
log_level: INFO
```

### 4.2. Service Definitions (`services.d/*.yml`)

Services are defined in YAML files. The files are read in lexicographical order.

```yaml
services:
  - name: cogserver
    command: /usr/bin/cogserver --config /etc/cogserver.conf
    depends_on:
      - service: atomspace-daemon
      - cognitive: "(Evaluation (PredicateNode \"AtomSpaceReady\") (True))"
    restart_policy: always

  - name: atomspace-daemon
    command: /usr/bin/atomspace-daemon
    depends_on: []
    restart_policy: on-failure
```

- **`name`**: The unique name of the service.
- **`command`**: The command to execute to start the service.
- **`depends_on`**: A list of dependencies.
  - **`service`**: A traditional dependency on another service name.
  - **`cognitive`**: A cognitive dependency specified as an Atomese expression. The service will only start when this expression evaluates to `True` in the AtomSpace.
- **`restart_policy`**: `always`, `on-failure`, or `never`.

## 5. Detailed Workflow

1.  **Kernel Boot**: The Linux kernel boots and executes `/usr/sbin/cognitive-init` as PID 1.
2.  **Initialization**: `cognitive_init` reads its main configuration file.
3.  **Hardware Profiling**: The `HardwareProfiler` runs, gathers system information, and creates a temporary in-memory AtomSpace to store this information in a `HardwareNode`.
4.  **Knowledge Loading**: The `KnowledgeLoader` starts the `atomspace-daemon` as a prerequisite, then loads the initial knowledge base from the specified file. The hardware information is merged into the main AtomSpace.
5.  **Service Graph Construction**: The `ServiceManager` parses all service definition files and builds a directed acyclic graph (DAG) of dependencies using the `DependencyResolver`.
6.  **AtomSpace Monitoring**: The `AtomSpaceMonitor` starts a new thread, connects to the AtomSpace, and subscribes to events. It maintains a list of all pending cognitive dependencies.
7.  **Service Startup**: The `ServiceManager` begins traversing the dependency graph.
    - For services with no dependencies, it forks and executes the service command.
    - For services with dependencies, it waits. Traditional dependencies are met when the required service is running. Cognitive dependencies are met when the `AtomSpaceMonitor` fires an event indicating the Atomese expression is `True`.
8.  **Event Loop**: The main `cognitive_init` process enters its main event loop, where it:
    - Reaps child processes (zombies).
    - Handles signals (e.g., `SIGTERM` for shutdown).
    - Processes events from the `AtomSpaceMonitor` to start new services.
    - Restarts failed services according to their `restart_policy`.

## 6. APIs and Data Structures

### 6.1. `Service` Class

```python
class Service:
    name: str
    command: str
    state: Literal["stopped", "starting", "running", "failed"]
    pid: int
    dependencies: List[Dependency]
    restart_policy: str
```

### 6.2. `Dependency` Class (Abstract)

```python
class Dependency(ABC):
    @abstractmethod
    def is_met(self, system_state) -> bool:
        pass
```

### 6.3. `ServiceDependency` and `CognitiveDependency`

```python
class ServiceDependency(Dependency):
    service_name: str

class CognitiveDependency(Dependency):
    atomese_expression: str
```

## 7. Testing Strategy

- **Unit Tests**: Each Python module will have comprehensive unit tests. The `DependencyResolver` will be tested with complex dependency graphs, including circular dependencies (which should raise an error).
- **Integration Tests**: A suite of tests will be run in a containerized environment (e.g., Docker). These tests will involve:
  - Booting a minimal system with `cognitive-init` as PID 1.
  - Using a mock AtomSpace to simulate cognitive events and verify that services start correctly.
  - Testing service failure and restart policies.
- **System Tests**: Full AGI-OS boot tests will be performed in a VM to ensure the entire system comes up correctly.

## 8. Conclusion

This specification outlines a robust and flexible cognitive init system that is a cornerstone of the AGI-OS. By integrating service management with the cognitive state of the system, `agi-os-cognitive-init` enables a new level of dynamic and intelligent system behavior, behavior.
