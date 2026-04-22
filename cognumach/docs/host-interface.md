# Host Interface

Operations for system-wide control and information, exposed via host ports.

## Host Operations
- host_information: CPU/memory stats, scheduling info.
- host_get_time / host_set_time.
- host_reboot.
- host-ports discovery and capabilities.

## Example Interactions (Mermaid)
```mermaid
sequenceDiagram
  participant T as Task
  participant K as Mach Kernel

  T->>K: host_get_host_priv_port()
  K-->>T: host_priv_port
  T->>K: host_get_time(host_priv_port)
  K-->>T: timebase
  T->>K: host_reboot(host_priv_port)
```

## Components (PlantUML)
```plantuml
@startuml
actor Admin
component "Host Interface" as Host
node "System" as Sys

Admin --> Host : info/time/reboot
Host --> Sys : control paths
@enduml
```

## References
- Host Interface: `https://www.gnu.org/software/hurd/gnumach-doc/Host-Interface.html`
- Host Ports/Info/Time/Reboot: `https://www.gnu.org/software/hurd/gnumach-doc/Host-Ports.html`, `https://www.gnu.org/software/hurd/gnumach-doc/Host-Information.html`, `https://www.gnu.org/software/hurd/gnumach-doc/Host-Time.html`, `https://www.gnu.org/software/hurd/gnumach-doc/Host-Reboot.html`
