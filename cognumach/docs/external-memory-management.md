# External Memory Management (EMM)

EMM lets user-space pagers back memory objects, enabling sophisticated policies and distributed storage.

## Roles
- Default Memory Manager (DMM): system pager for anonymous memory; provides default backing.
- External Pager: user task serving a memory object (files, network-backed pages, etc.).
- VM: mediates page faults and enforces protections/attributes.

## Pager Lifecycle (Mermaid)
```mermaid
sequenceDiagram
  autonumber
  participant Client as Client Task
  participant VM as Mach VM
  participant Pager as External Pager

  Client->>VM: vm_map(object, offset)
  VM-->>Client: mapping established
  Client->>VM: page fault
  VM->>Pager: memory_object_data_request(off,len)
  Pager-->>VM: memory_object_data_provided(page)
  VM-->>Client: resume thread
```

## Pager Interfaces (PlantUML)
```plantuml
@startuml
interface memory_object {
  +init()
  +data_request(offset, length)
  +data_return(...)
  +terminate()
}

interface memory_object_control {
  +data_provided(...)
  +change_attributes(...)
  +supply_completed(...)
}

class ExternalPager
ExternalPager ..|> memory_object
ExternalPager --> memory_object_control : uses

note right of ExternalPager
  Pager implements memory_object and
  uses memory_object_control to reply
end note
@enduml
```

## Copy Strategy and Coherency
- Copy strategies influence how copy-on-write and replication are handled.
- Attribute negotiation occurs via object attribute calls.

## References
- External Memory Management: `https://www.gnu.org/software/hurd/gnumach-doc/External-Memory-Management.html`
- Memory Object Server: `https://www.gnu.org/software/hurd/gnumach-doc/Memory-Object-Server.html`
- Default Memory Manager: `https://www.gnu.org/software/hurd/gnumach-doc/Default-Memory-Manager.html`
