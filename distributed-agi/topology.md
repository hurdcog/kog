# Distributed AGI Cognitive Network Topology

## Overview

The distributed cognitive network organises cognitive nodes into functional
clusters connected by the 9P-Cog protocol. Each cluster specialises in a
different aspect of cognition while sharing a distributed AtomSpace.

## Cluster Architecture

```
                    ┌─────────────────────────────────┐
                    │  Central Coordination Node      │
                    │  (CogNumach Core)               │
                    └──────────────┬──────────────────┘
                                   │
            ┌──────────────────────┼──────────────────────┐
            │                      │                       │
   ┌────────▼──────┐    ┌──────────▼────────┐   ┌─────────▼─────────┐
   │  Perception   │    │    Reasoning      │   │    Learning       │
   │  Cluster      │    │    Cluster        │   │    Cluster        │
   └───────┬───────┘    └────────┬──────────┘   └─────────┬─────────┘
           │                     │                          │
   ┌───────┴────┐      ┌────────┴────────┐      ┌─────────┴──────────┐
   │            │      │       │         │      │          │          │
 Vision      Audio    PLN     URE      Temporal  MOSES   Miner    Neural
   Node       Node    Node    Node      Node     Node    Node      Node
           │                     │                          │
           └─────────────────────┴──────────────────────────┘
                                 │
                    ┌────────────▼────────────┐
                    │     Action Cluster      │
                    │  (Motor, Speech, Plan)  │
                    └─────────────────────────┘
```

## Node Addresses

| Node          | Network Address           | Port | Path              |
|---------------|---------------------------|------|-------------------|
| Coordination  | tcp!cognumach!564         | 564  | /cog              |
| Perception    | tcp!percept-cluster!564   | 564  | /net/cognitive/perception |
| Vision        | tcp!vision-node!564       | 564  | /net/cognitive/perception/vision |
| Audio         | tcp!audio-node!564        | 564  | /net/cognitive/perception/audio  |
| Reasoning     | tcp!reason-cluster!564    | 564  | /net/cognitive/reasoning |
| PLN           | tcp!pln-node!564          | 564  | /net/cognitive/reasoning/pln     |
| URE           | tcp!ure-node!564          | 564  | /net/cognitive/reasoning/ure     |
| Learning      | tcp!learn-cluster!564     | 564  | /net/cognitive/learning  |
| MOSES         | tcp!moses-node!564        | 564  | /net/cognitive/learning/moses    |
| Action        | tcp!action-cluster!564    | 564  | /net/cognitive/action    |

## Atom Distribution

Atoms are distributed according to the following rules (see `partitioning.c`):

| Atom Type         | Primary Node    | Replicated? |
|-------------------|-----------------|-------------|
| PerceptionNode    | Perception      | If STI ≥ 500 |
| ConceptNode       | Reasoning       | If STI ≥ 500 |
| PredicateNode     | Reasoning       | If STI ≥ 500 |
| ProcedureNode     | Action          | If STI ≥ 500 |
| ActionNode        | Action          | If STI ≥ 500 |
| EpisodeNode       | Memory          | If STI ≥ 500 |
| InheritanceLink   | Reasoning       | If STI ≥ 500 |
| EvaluationLink    | Reasoning       | If STI ≥ 500 |

## Synchronization

Nodes use the Lamport clock-based eventual consistency protocol
implemented in `synchronization.c`:

1. Each node maintains a logical clock.
2. On every atom update, the clock is incremented and a sync record queued.
3. Updates are gossiped to 2–3 peers on each cycle.
4. Merge conflicts are resolved using the Lamport ordering (higher timestamp wins).

## Namespace Binding

```sh
# On each node, bind the distributed namespace
bind /net/cognitive/perception /cog/distributed/perception
bind /net/cognitive/reasoning  /cog/distributed/reasoning
bind /net/cognitive/learning   /cog/distributed/learning
bind /net/cognitive/action     /cog/distributed/action

# Mount remote reasoning cluster
mount -A tcp!reason-cluster!564 /n/reasoning
```

## Failure Handling

- **Node failure**: Surviving nodes detect via heartbeat timeout (5s).
- **Partition**: Split-brain prevented by requiring quorum for updates to
  replicated atoms.
- **Recovery**: On reconnect, nodes exchange sync queues and apply
  missing updates in Lamport order.
