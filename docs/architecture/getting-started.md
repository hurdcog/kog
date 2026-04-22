# Getting Started with the OpenCog Collection

This guide will walk you through setting up your development environment for the OpenCog Collection (OCC). We strongly recommend using the provided devcontainer, which will create a reproducible and isolated environment with all the necessary dependencies.

## Prerequisites

- [Docker](https://docs.docker.com/get-docker/)
- [Visual Studio Code](https://code.visualstudio.com/)
- [Remote - Containers extension for VS Code](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Setup

1.  **Clone the repository with submodules:**

    ```bash
    git clone --recurse-submodules https://github.com/Kaw-Aii/occ.git
    cd occ
    ```

2.  **Open the repository in VS Code:**

    ```bash
    code .
    ```

3.  **Reopen in Container:**

    -   When you open the repository in VS Code, you should see a notification in the bottom-right corner asking if you want to "Reopen in Container". Click this button.
    -   If you don't see the notification, open the Command Palette (`Ctrl+Shift+P` or `Cmd+Shift+P`) and search for "Remote-Containers: Reopen in Container".

4.  **Wait for the container to build.** This may take some time on the first run as it downloads and installs all the dependencies.

## Your First Build

Once the devcontainer is up and running, you can build the entire OpenCog Collection with a single command:

```bash
guix build -f guix.scm
```

This will build all the components of the OCC and install them into the Guix store. You can then run the various components and experiment with the system.

## What's Next?

-   Learn more about the [architecture of the OCC](architecture.md).
-   Explore the principles of [cognitive synergy](cognitive-synergy.md) that guide our work.
-   Start [contributing](CONTRIBUTING.md) to the project!

