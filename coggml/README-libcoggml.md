# libcoggml.so

**Source:** `coggml/src/`  
**Role:** CogGML self-aware microkernel — a lightweight cognitive sharding layer that sits between raw hardware resources and the AtomSpace-level AGI components.

## What it does

Implements a *cognitive microkernel* model where cognition is decomposed into isolated, versioned **shards**.  Each shard encapsulates a slice of cognitive state; a coordinator distributes shards across execution units and manages message-passing between them.  The self-awareness module tracks runtime health of each shard and feeds diagnostics back to the coordinator.

## Object files

| Object | Purpose |
|--------|---------|
| `microkernel.cpp.o` | `CogMicrokernel` — entry point; initialises the shard runtime and exposes the public C API |
| `cognitive_shard.cpp.o` | `CognitiveShard` — unit of isolated cognitive state; lifecycle (create/suspend/resume/destroy) |
| `self_awareness.cpp.o` | `SelfAwareness` — monitors shard health metrics and raises alerts |
| `shard_coordinator.cpp.o` | `ShardCoordinator` — placement, scheduling and load-balancing of shards |
| `shard_message.cpp.o` | `ShardMessage` — typed message envelopes for inter-shard communication |
| `distributed_coordinator.cpp.o` | `DistributedCoordinator` — multi-node extension of the shard coordinator |

## Key public headers

- `coggml/include/microkernel.h`
- `coggml/include/cognitive_shard.h`
- `coggml/include/shard_coordinator.h`
