# AGI-OS Integration Packages: Full Implementation Guide

**Version**: 1.0  
**Date**: December 6, 2025  
**Status**: Implementation Guide

## 1. Introduction

This document provides a comprehensive, production-ready implementation plan for the source code of the three new AGI-OS integration packages: `cognumach-cognitive-scheduler`, `agi-os-monitoring`, and `agi-os-cognitive-init`. It synthesizes the technical specifications into a single, actionable guide for developers, complete with code examples, testing strategies, and project management milestones.

## 2. Project Goals and Milestones

The primary goal is to implement the source code for the three core integration packages that enable deep cognitive synergy across the AGI-OS stack. The project is divided into five major milestones over an estimated 16-week period.

| Milestone | Description | Duration | Key Deliverables |
|---|---|---|---|
| **M1** | `cognumach-cognitive-scheduler` | 5 Weeks | Kernel patch, user-space daemon, unit tests |
| **M2** | `agi-os-monitoring` | 5 Weeks | Data collectors, backend API, frontend dashboard |
| **M3** | `agi-os-cognitive-init` | 5 Weeks | Core init system, service definition schema, unit tests |
| **M4** | Integration Testing & Validation | 4 Weeks | Full test suite, system validation report, bug fixes |
| **M5** | Documentation & Final Release | 1 Week | Final source, updated docs, user guides |

## 3. Implementation Details and Code Examples

This section provides concrete implementation details and illustrative code examples for each package.

### 3.1. `cognumach-cognitive-scheduler`

**Objective**: To create a CPU scheduler that prioritizes tasks based on cognitive importance (attention value).

#### 3.1.1. Kernel `task` Struct Modification

**File**: `kernel/task.h`

```c
struct task {
    // ... existing fields ...
    unsigned int attention_value; /* Cognitive importance of the task */
    boolean_t is_cognitive;       /* Tag for cognitive workloads */
};
```

#### 3.1.2. Scheduler Logic Example

**File**: `kernel/sched_prim.c` (conceptual example)

```c
thread_t choose_thread_to_run(run_queue_t *rq) {
    thread_t best_thread = NULL;
    int max_score = -1;

    for (thread_t thread in rq->threads) {
        int score = 0;
        task_t task = thread->task;

        // Combine traditional priority with cognitive attention
        if (task->is_cognitive) {
            score = task->priority + (task->attention_value * COGNITIVE_WEIGHT);
        } else {
            score = task->priority;
        }

        if (score > max_score) {
            max_score = score;
            best_thread = thread;
        }
    }
    return best_thread;
}
```

#### 3.1.3. User-Space Daemon (`cognosched-daemon`)

**File**: `daemon/main.cpp` (conceptual example)

```cpp
#include "scheduler_api.h"
#include "learning_model.h"

int main() {
    LearningModel model;
    SchedulerApi api;

    while (true) {
        // Collect performance data from agi-os-monitoring
        SystemPerformance perf = get_system_performance();

        // Update the learning model
        model.update(perf);

        // Get the new optimal policy
        SchedulerPolicy new_policy = model.get_optimal_policy();

        // Push the new policy to the kernel
        api.set_scheduler_policy(new_policy);

        sleep(POLICY_UPDATE_INTERVAL);
    }
    return 0;
}
```

### 3.2. `agi-os-monitoring`

**Objective**: To provide a unified, real-time monitoring dashboard for the entire AGI-OS stack.

#### 3.2.1. Data Collector Example (`occ_collector.py`)

```python
import opencog.atomspace
import requests

API_ENDPOINT = "http://localhost:5001/api/v1/metrics"

class OccCollector:
    def __init__(self):
        self.atomspace = opencog.atomspace.AtomSpace()
        self.atomspace.subscribe(self.on_atom_event)

    def on_atom_event(self, event_type, atom):
        if event_type == 'attention_change':
            metric = {
                'source': 'occ',
                'type': 'attention',
                'atom_uuid': atom.uuid,
                'attention_value': atom.attention
            }
            requests.post(API_ENDPOINT, json=metric)

    def run(self):
        # Keep the script running to listen for events
        while True:
            time.sleep(1)
```

#### 3.2.2. Backend API Endpoint (`api_server.py`)

```python
from flask import Flask, request, jsonify
from flask_socketio import SocketIO

app = Flask(__name__)
socketio = SocketIO(app)

@app.route('/api/v1/metrics', methods=['POST'])
def receive_metric():
    metric = request.get_json()
    
    # 1. Store the metric in the time-series database
    store_metric_in_db(metric)
    
    # 2. Emit the metric to all connected WebSocket clients
    socketio.emit('new_metric', metric, namespace='/live')
    
    return jsonify({'status': 'success'}), 201
```

### 3.3. `agi-os-cognitive-init`

**Objective**: To create a dynamic, dependency-aware init system that understands the cognitive state of the AGI-OS.

#### 3.3.1. Service Definition Example (`/etc/cognitive-init/services.d/occ.yml`)

```yaml
services:
  - name: pln-engine
    command: /usr/bin/pln-engine --mode background
    depends_on:
      - service: cogserver
      - cognitive: "(SatisfactionLink (ConceptNode \"SystemReady\") (FloatValue 1.0))"
    restart_policy: on-failure
```

#### 3.3.2. Service Manager Logic (`service_manager.py`)

```python
class ServiceManager:
    def __init__(self, service_definitions):
        self.services = self.parse_definitions(service_definitions)
        self.dependency_graph = self.build_graph(self.services)

    def attempt_to_start_services(self, system_state):
        for service in self.services:
            if service.state == 'stopped' and self.dependencies_met(service, system_state):
                self.start_service(service)

    def dependencies_met(self, service, system_state):
        for dep in service.dependencies:
            if not dep.is_met(system_state):
                return False
        return True

    def start_service(self, service):
        print(f"Starting service: {service.name}")
        # Fork and exec the service command
        # ...
```

## 4. Testing and Validation Strategy

A multi-layered testing strategy will be employed to ensure the correctness and stability of the new components.

### 4.1. Unit Testing

- **Frameworks**: `cxxtest` for C++, Python's `unittest` and `pytest` for Python code.
- **Coverage**: Aim for >80% code coverage for all new code.
- **Focus**:
    - **Cognitive Scheduler**: Test scheduling logic with various attention values and priorities. Test the policy update mechanism.
    - **Monitoring**: Test each data collector in isolation. Test the API endpoints and WebSocket emissions.
    - **Cognitive Init**: Test the dependency resolver with complex graphs (including cycles). Test the service startup and restart logic.

### 4.2. Integration Testing

- **Environment**: Docker containers simulating a multi-node AGI-OS environment.
- **Focus**:
    - **Attention Propagation**: Verify that attention value changes in the AtomSpace correctly propagate down to the kernel scheduler and influence task priorities.
    - **Monitoring Data Flow**: Ensure that events from all three layers (kernel, HurdCog, OCC) are correctly collected, aggregated, and displayed on the dashboard.
    - **Cognitive Dependencies**: Create test cases where services are started based on the appearance of specific atoms or patterns in the AtomSpace.

### 4.3. System Testing

- **Environment**: A full AGI-OS installation in a virtual machine (QEMU/KVM).
- **Focus**:
    - **Full Boot Test**: Boot the entire AGI-OS from scratch using `agi-os-cognitive-init` and verify that all services start in the correct order and the system reaches a stable, operational state.
    - **Workload Simulation**: Run a variety of cognitive workloads (e.g., PLN inference, pattern mining) and monitor system performance and stability using the `agi-os-monitoring` dashboard.
    - **Fault Tolerance**: Manually kill critical services and verify that `agi-os-cognitive-init` restarts them correctly according to their policies.

## 5. Getting Started for Developers

1.  **Set up the Build Environment**: Ensure you have a clean Ubuntu 22.04 environment with all the build dependencies listed in the `debian/control` files installed. Pay special attention to the `mig` dependency for Cognumach.
2.  **Clone the Repository**: `git clone --recursive https://github.com/cogpy/occ.git`
3.  **Implement a Feature**: Pick a task from the detailed milestone plan (e.g., Task 1.1: Modify `task` struct).
4.  **Write Unit Tests**: Create a new unit test file that validates the functionality you've added.
5.  **Build and Test Locally**: Run the build scripts and the unit tests to ensure your changes haven't introduced regressions.
6.  **Submit a Pull Request**: Push your changes to a feature branch and open a pull request for review.

## 6. Conclusion

This implementation guide provides a clear, actionable plan for developing the three new AGI-OS integration packages. By following the detailed specifications, code examples, and testing strategies outlined in this document, the development team can efficiently and effectively deliver the core components required to bring the AGI-OS to life.
