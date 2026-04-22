# Phase 1 Detailed Plan: Cognumach Symplectic Scheduler

**Date**: December 9, 2025  
**Version**: 1.0  
**Author**: Manus AI

## 1. Overview

This document provides a detailed task breakdown for the **Cognumach Symplectic Scheduler**, a key deliverable of Phase 1 of the Unified AGI-OS Roadmap. This scheduler replaces the previously planned heuristic-based model with a mathematically rigorous implementation based on Hamiltonian mechanics.

**Estimated Timeline**: 7 working days.

## 2. Task Breakdown and Timeline

### Task 1.1: Finalize Hamiltonian Design and Data Structures (1 Day)

*   **Goal**: To create a formal specification for the scheduler's core data structures and mathematical model.
*   **Implementation Steps**:
    1.  **Define Phase Space**: Formally define the `(q, p)` phase space. `q_i` will be task progress (e.g., a `uint64_t` counter) and `p_i` will be task momentum/priority (e.g., a `float`).
    2.  **Define Hamiltonian**: Formally define the Hamiltonian function `H(q, p)` in C code, including the kinetic energy term `p^2/2m` and the potential energy term `V(q)` derived from attention values and the Jacobian norm.
    3.  **Update Kernel Data Structures**: Modify the `cognitive_task_info` struct in the Cognumach kernel to include fields for `q`, `p`, `mass` (derived from the Jacobian), `sti`, and `lti`.
    4.  **Extend IPC Message**: Define the new IPC message format that will be sent from the user-space daemon to the kernel, including fields for `task_id`, `sti`, `lti`, and `jacobian_norm`.
*   **Deliverable**: A new specification document, `cognitive_scheduler_v2_spec.md`, detailing the above.

### Task 1.2: Implement Symplectic Integrator (2 Days)

*   **Goal**: To implement the core leapfrog symplectic integrator in a standalone, testable C module.
*   **Implementation Steps**:
    1.  Create a new file, `symplectic.c`, in the `integration/cognitive-scheduler/` directory.
    2.  Implement the leapfrog integration algorithm as a function: `void leapfrog_step(State* state, float dt, Hamiltonian* H)`.
    3.  Create a corresponding `symplectic.h` header file.
    4.  Write a unit test, `test_symplectic.c`, that uses a simple, known Hamiltonian (e.g., a simple harmonic oscillator) and verifies that the integrator conserves energy over a large number of steps.
*   **Deliverable**: `symplectic.c`, `symplectic.h`, and a passing `test_symplectic.c`.

### Task 1.3: Integrate Integrator into Scheduler (2 Days)

*   **Goal**: To replace the old heuristic-based priority calculation with the new symplectic integrator.
*   **Implementation Steps**:
    1.  Modify `cognitive_scheduler.c` to include `symplectic.h`.
    2.  In the main scheduling function, instead of calculating a new priority directly, construct the `State` and `Hamiltonian` objects from the current task data.
    3.  Call the `leapfrog_step()` function to get the new momentum `p` for each task.
    4.  Apply the new momentum values as the actual Mach task priorities.
    5.  Implement the logic to receive the extended IPC message and update the `sti`, `lti`, and `mass` values for each task.
*   **Deliverable**: Modified `cognitive_scheduler.c` that uses the symplectic integrator.

### Task 1.4: Implement Integration Test (1 Day)

*   **Goal**: To create an end-to-end test that verifies the correct behavior of the new scheduler.
*   **Implementation Steps**:
    1.  Create a new test script, `test_integration_scheduler_v2.py`.
    2.  The script will spawn several user-space processes with different base priorities.
    3.  It will then send a series of IPC messages to the scheduler, giving one of the processes a high attention value.
    4.  The test will monitor the CPU usage of all processes over time.
    5.  **Success Criteria**: The process with the high attention value should consistently receive a larger share of the CPU time. The total energy of the system (calculated by the test script based on the known state) should remain constant within a small tolerance.
*   **Deliverable**: A passing `test_integration_scheduler_v2.py`.

### Task 1.5: Finalize Packaging and Documentation (1 Day)

*   **Goal**: To package the new scheduler as a Debian package and document its usage.
*   **Implementation Steps**:
    1.  Update the `debian/` directory for the `cognumach-cognitive-scheduler` package to include the new files.
    2.  Build the final `.deb` package.
    3.  Write a `README.md` for the scheduler, explaining the new Hamiltonian-based design, how to configure it (e.g., via `sysctl`), and how to interpret its behavior.
*   **Deliverable**: `cognumach-cognitive-scheduler.deb` and comprehensive documentation.

## 3. Code Specifications

### `symplectic.h`

```c
#ifndef SYMPLECTIC_H
#define SYMPLECTIC_H

// Represents the state of the entire system
typedef struct {
    int num_tasks;
    float* q; // Array of positions
    float* p; // Array of momenta
    float* m; // Array of masses
} State;

// Represents the Hamiltonian of the system
typedef struct {
    // Function pointer to calculate the gradient of the potential energy
    void (*grad_V)(State* state, float* grad);
} Hamiltonian;

// Performs one step of the leapfrog integration
void leapfrog_step(State* state, float dt, Hamiltonian* H);

#endif // SYMPLECTIC_H
```

### `cognitive_scheduler.c` (Conceptual Snippet)

```c
// Main scheduling loop
void cognitive_schedule() {
    // 1. Lock the task list
    // 2. Populate the State object from the cognitive_task_info structs
    State current_state;
    populate_state(&current_state, &task_list);

    // 3. Define the Hamiltonian for this step
    Hamiltonian H;
    H.grad_V = &calculate_potential_gradient; // This function uses sti/lti

    // 4. Evolve the system
    leapfrog_step(&current_state, SCHEDULER_TICK_INTERVAL, &H);

    // 5. Apply the new momenta as priorities
    apply_priorities(&current_state, &task_list);

    // 6. Unlock the task list
}

// This function is called when an IPC message is received
void update_task_potential(int task_id, float sti, float lti, float jacobian_norm) {
    // Update the sti, lti, and mass values for the given task
}
```
