# GNU Mach Architecture Overview

This overview connects the major subsystems of the GNU Mach microkernel: tasks/threads, IPC/ports, virtual memory, scheduling/processor sets, device interface, host interface, bootstrap, and debugging.

- Microkernel responsibilities: IPC, VM, scheduling, basic devices, timing, and host control.
- User space responsibilities: servers (e.g., filesystems, network stacks) and applications built atop IPC.

## High-Level System Map
```mermaid
graph TB
  subgraph User_Space[User Space]
    A[Tasks (clients)]
    S[Servers (Hurd translators, pagers, etc.)]
  end

  subgraph Kernel[GNU Mach Microkernel]
    IPC[IPC: Ports & Messages]
    VM[Virtual Memory]
    SCHED[Scheduler]
    DEV[Device Interface]
    HOST[Host Interface]
    KDB[Kernel Debugger]
  end

  A -- mach_msg() over send rights --> IPC
  S -- mach_msg() over receive rights --> IPC

  IPC --> VM
  IPC --> DEV
  SCHED --> A
  SCHED --> S

  DEV ---|device ports| A
  DEV ---|device ports| S
  VM ---|memory objects| S
  HOST --- A
  HOST --- S

  BOOT[Boot/Bootstrap] --> Kernel
  Kernel --> BT[Bootstrap Task & Ports]
```

## Component Relationships (PlantUML)
```plantuml
@startuml
skinparam componentStyle rectangle

package "User Space" {
  [Client Tasks] as Clients
  [Servers] as Servers
}

component "GNU Mach Microkernel" as Kernel
component "IPC (Ports/Messages)" as IPC
component "Virtual Memory" as VM
component "Scheduler" as Sched
component "Device Interface" as Dev
component "Host Interface" as Host
component "Kernel Debugger" as Kdb

Clients --> IPC : mach_msg(send)
Servers --> IPC : mach_msg(recv)
IPC --> Kernel
Kernel -- VM
Kernel -- Sched
Kernel -- Dev
Kernel -- Host
Kernel -- Kdb

Dev .. Clients : device_* operations
Dev .. Servers : device_* operations
VM .. Servers : memory_object_* callbacks
Sched .. Clients
Sched .. Servers
@enduml
```

## Notes
- Ports mediate all capabilities: a port right (send/receive/send-once) is required to communicate or control a resource.
- VM integrates with external pagers via memory objects and the default memory manager.
- Scheduling is policy-driven and can be scoped to processor sets.
- Devices are accessed through device ports; implementations may reside in-kernel or be proxied to user-space drivers, depending on configuration.

## References
- Overview: `https://www.gnu.org/software/hurd/gnumach-doc/Overview.html`
- Features: `https://www.gnu.org/software/hurd/gnumach-doc/Features.html`
- Major Concepts: `https://www.gnu.org/software/hurd/gnumach-doc/Major-Concepts.html`
