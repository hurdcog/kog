# Phase 1 Detailed Plan: Prototype 9P Server

**Date**: December 9, 2025  
**Version**: 1.0  
**Author**: Manus AI

## 1. Overview

This document provides a detailed task breakdown for the **Prototype 9P Server**, a key deliverable of Phase 1 of the Unified AGI-OS Roadmap. This server will act as the unified communication bridge for the AGI-OS, replacing all custom IPC mechanisms. The goal of this phase is to create a working prototype that exposes essential cognitive services as a file system.

**Estimated Timeline**: 7 working days.

## 2. Task Breakdown and Timeline

### Task 2.1: Set up 9P Library and Server Skeleton (2 Days)

*   **Goal**: To create a basic, running 9P server that can be mounted but serves no files.
*   **Implementation Steps**:
    1.  Choose and integrate a 9P library. `libixp` is a mature and suitable choice. Add it as a dependency to the new `hurdcog-p9-server` package.
    2.  Create a new directory, `integration/p9-server/`.
    3.  Create `main.c` for the server. This will initialize `libixp`, create a server instance, and start listening for connections on a specified port.
    4.  Implement the bare minimum 9P callbacks (e.g., `attach`, `walk`, `clunk`) to handle a client connecting and navigating an empty file system.
*   **Deliverable**: A running 9P server that can be mounted from the command line (e.g., `9p mount localhost:564 /mnt/cog`). The mount should succeed, and an `ls /mnt/cog` should show an empty directory.

### Task 2.2: Implement `/attention` File (2 Days)

*   **Goal**: To expose the system-wide attention values as a virtual file.
*   **Implementation Steps**:
    1.  Design the file system layout. We will have a single file, `/attention`, that represents the attention values for all tasks.
    2.  Implement the `walk` callback to recognize the path `/attention`.
    3.  Implement the `open` and `read` callbacks for this file. When read, the server will query the `cognitive-scheduler` (via a direct function call, as they are in the same kernel space) for the current attention values of all tasks and format them as a text string (e.g., `task_id: attention_value\n`).
    4.  Implement the `write` callback. When a string is written to `/attention` (e.g., `echo "123:0.9" > /mnt/cog/attention`), the server will parse the string and call a function in the scheduler to update the attention value for the specified task.
*   **Deliverable**: A user can `cat /mnt/cog/attention` to see the current attention state and `echo` to it to change a task's attention.

### Task 2.3: Implement `/ure/query` File (2 Days)

*   **Goal**: To expose the URE (Unified Rule Engine) as a virtual file for executing queries.
*   **Implementation Steps**:
    1.  Add a new file, `/ure/query`, to the virtual file system.
    2.  Implement the `write` callback for this file. When a query string (e.g., `(GetLink (ConceptNode \"foo\"))`) is written to it, the server will pass this string to the CogKernel's URE for execution.
    3.  The result of the query (which could be a set of atoms) will be stored temporarily.
    4.  Implement the `read` callback. When the file is read, the server will return the stored result, formatted as a string.
*   **Deliverable**: A user can `echo "(SomeQuery)" > /mnt/cog/ure/query` and then `cat /mnt/cog/ure/query` to get the result.

### Task 2.4: Finalize Packaging and Documentation (1 Day)

*   **Goal**: To package the prototype server as a Debian package and document its usage.
*   **Implementation Steps**:
    1.  Create the `debian/` directory for the `hurdcog-p9-server` package.
    2.  Build the final `.deb` package.
    3.  Write a `README.md` for the server, explaining its purpose, the files it exposes, and how to interact with them from the command line.
*   **Deliverable**: `hurdcog-p9-server.deb` and comprehensive documentation.

## 3. Code Specifications

### `p9-server/main.c` (Conceptual Snippet)

```c
#include <ixp.h>

// 9P Callbacks
void p9_attach(IxpReq *r) { /* ... */ }
void p9_walk(IxpReq *r) { /* ... */ }
void p9_read(IxpReq *r) {
    if (is_attention_file(r->fid)) {
        // 1. Get attention values from scheduler
        // 2. Format as string
        // 3. ixp_respond(r, formatted_string);
    } else if (is_ure_result_file(r->fid)) {
        // 1. Get stored URE result
        // 2. ixp_respond(r, result_string);
    }
}
void p9_write(IxpReq *r) {
    if (is_attention_file(r->fid)) {
        // 1. Parse r->data for task_id and value
        // 2. Call scheduler_update_attention(task_id, value)
        // 3. ixp_respond(r, NULL);
    } else if (is_ure_query_file(r->fid)) {
        // 1. Pass r->data to CogKernel URE
        // 2. Store the result
        // 3. ixp_respond(r, NULL);
    }
}

// ... other callbacks ...

Ixp9Srv p9_srv = {
    .attach = p9_attach,
    .walk = p9_walk,
    .read = p9_read,
    .write = p9_write,
    // ...
};

int main(int argc, char *argv[]) {
    // Initialize ixp server and start listening
    ixp_server_start(&p9_srv, "/path/to/socket");
    return 0;
}
```
