# Scheduling and Processor Sets

GNU Mach supports multiple policies and processor set partitioning for scalability and isolation.

## Scheduling Policies
- Time-sharing (TS): dynamic priorities.
- Round-robin (RR): fixed timeslice rotation.
- First-in, first-out (FIFO): run-to-block with fixed priority.
- Hand-off scheduling: transfer CPU directly on IPC rendezvous in special cases.

## Processor Sets (Mermaid)
```mermaid
graph LR
  subgraph Host
    H[host] -->|manages| PS1[processor_set: A]
    H --> PS2[processor_set: B]
    PS1 --> P1[CPU 0]
    PS1 --> P2[CPU 1]
    PS2 --> P3[CPU 2]
  end

  T1[Task/Threads] -->|assigned to| PS1
  T2[Task/Threads] -->|assigned to| PS2
```

## Components and Control (PlantUML)
```plantuml
@startuml
component "Scheduler" as S
component "Policy: TS" as Pts
component "Policy: RR" as Prr
component "Policy: FIFO" as Pfifo
node "Processor Set A" as PSA
node "Processor Set B" as PSB
node "CPU0" as CPU0
node "CPU1" as CPU1
node "CPU2" as CPU2

S -- Pts
S -- Prr
S -- Pfifo
PSA -- CPU0
PSA -- CPU1
PSB -- CPU2
@enduml
```

## References
- Scheduling: `https://www.gnu.org/software/hurd/gnumach-doc/Scheduling.html`
- Hand-Off Scheduling: `https://www.gnu.org/software/hurd/gnumach-doc/Hand_002dOff-Scheduling.html`
- Scheduling Policy: `https://www.gnu.org/software/hurd/gnumach-doc/Scheduling-Policy.html`
- Processor Sets: `https://www.gnu.org/software/hurd/gnumach-doc/Processors-and-Processor-Sets.html`
- Processor/Set Interfaces: `https://www.gnu.org/software/hurd/gnumach-doc/Processor-Set-Interface.html`, `https://www.gnu.org/software/hurd/gnumach-doc/Processor-Interface.html`
