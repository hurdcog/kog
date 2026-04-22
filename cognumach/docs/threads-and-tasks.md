# Threads and Tasks

Tasks encapsulate resources (address space, ports). Threads execute within tasks. Many thread and task operations are exposed via Mach interfaces.

## Structure (Mermaid Class Diagram)
```mermaid
classDiagram
  class Task {
    +address_space
    +special_ports: control, name, notif, exception
  }
  class Thread {
    +state: running/blocked
    +priority
    +policy
    +special_ports: thread_self, thread_reply
  }
  Task "1" o-- "*" Thread : contains
```

## Interfaces
- Task: creation/termination, info, special ports, execution control.
- Thread: creation/termination, info, settings, execution control, exceptions.
- Exceptions: delivered to task/thread exception ports.

## Exception Delivery (PlantUML Sequence)
```plantuml
@startuml
actor FaultingThread as FT
participant Kernel as Mach
participant TaskExPort as "Task Exception Port"

FT -> Kernel: fault/trap/syscall exception
Kernel -> TaskExPort: exception(message with state)
TaskExPort --> Kernel: reply (disposition)
Kernel -> FT: resume per disposition
@enduml
```

## Scheduling Hooks
- Thread priority and policy determine dispatch; see scheduling document.
- Hand-off scheduling can transfer CPU directly on IPC handoff in specific cases.

## References
- Threads and Tasks: `https://www.gnu.org/software/hurd/gnumach-doc/Threads-and-Tasks.html`
- Thread Interface: `https://www.gnu.org/software/hurd/gnumach-doc/Thread-Interface.html`
- Task Interface: `https://www.gnu.org/software/hurd/gnumach-doc/Task-Interface.html`
- Exceptions: `https://www.gnu.org/software/hurd/gnumach-doc/Exceptions.html`
