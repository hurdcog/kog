# OpenCog Inferno AGI OS Architecture

## Overview

OpenCog Inferno is a pure kernel-based distributed AGI operating system. It implements artificial general intelligence as a fundamental operating system service, following the Inferno/Plan 9 philosophy where everything is a file and all resources are accessible through the 9P protocol.

**Version**: 0.3.0  
**License**: AGPL-3.0  
**Build System**: CMake 3.16+  
**Platforms**: Windows, Linux, macOS (portable C implementation)

## Design Principles

1. **Intelligence as a Kernel Service**: Cognition is not an application; it is a core OS function.
2. **Everything is a File**: All cognitive structures (atoms, links, truth values, attention values) are exposed as files in the cognitive namespace.
3. **Distributed by Default**: Knowledge, reasoning, and attention are natively distributed across nodes using 9P/Styx.
4. **Zero-Copy Cognitive Operations**: Kernel-level integration eliminates user-kernel boundary crossing overhead.
5. **Economic Attention**: Limited computational resources are allocated using economic principles (ECAN).

## Architecture Layers

```
┌─────────────────────────────────────────────────────────────────────┐
│                   Cognitive Applications (Limbo)                     │
│  cognitive_demo.b  hello_cognition.b  simple_inference.b             │
├─────────────────────────────────────────────────────────────────────┤
│                 Limbo Cognitive Libraries                             │
│  atomspace.m  pln.m  attention.m  cognet.m                           │
├─────────────────────────────────────────────────────────────────────┤
│                        Userspace Clients                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐               │
│  │ atomspace_    │  │ cogfs_       │  │ reasoning_   │               │
│  │ client        │  │ client       │  │ client       │               │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘               │
├─────────┼──────────────────┼──────────────────┼──────────────────────┤
│         │     Cognitive 9P Filesystem         │                      │
│  ┌──────┴─────────────────────────────────────┴───────────────────┐  │
│  │  /cognitive/                                                    │  │
│  │  ├── atomspace/ (atoms, queries, patterns, stats, count)        │  │
│  │  ├── reasoning/ (pln/rules/proofs/beliefs, ure/forward/back)    │  │
│  │  ├── attention/ (focus, importance, urgency, allocation)        │  │
│  │  ├── memory/    (working, episodic, semantic, procedural)       │  │
│  │  ├── learning/  (supervised, unsupervised, reinforcement, meta) │  │
│  │  ├── perception/ (vision, audio, text, sensors)                 │  │
│  │  └── action/    (motor, speech, commands)                       │  │
│  └────────────────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────────────┤
│                     Kernel Cognitive Services                        │
│                                                                      │
│  ┌─────────────┐  ┌──────────────┐  ┌───────────────┐               │
│  │  AtomSpace   │  │  PLN Engine  │  │ MOSES Engine  │               │
│  │  Hypergraph  │  │  Inference   │  │ Evolutionary  │               │
│  │  Database    │  │  Engine      │  │ Learning      │               │
│  └──────┬──────┘  └──────┬───────┘  └───────┬───────┘               │
│         │                │                    │                       │
│  ┌──────┴──────┐  ┌──────┴───────┐  ┌───────┴───────┐               │
│  │  Pattern    │  │  Attention   │  │  Cognitive    │               │
│  │  Matcher    │  │  Allocation  │  │  IPC Channels │               │
│  │  Engine     │  │  (ECAN)      │  │               │               │
│  └─────────────┘  └──────────────┘  └───────────────┘               │
│                                                                      │
│  ┌────────────────────────────────────────────────────────────────┐  │
│  │              Distributed AtomSpace Layer                        │  │
│  │  Node Discovery │ Load Balancing │ Sync │ Remote Queries        │  │
│  └────────────────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────────────┤
│              Dis Virtual Machine / Native Platform                    │
├─────────────────────────────────────────────────────────────────────┤
│                       Hardware                                       │
└─────────────────────────────────────────────────────────────────────┘
```

## Kernel Modules

### 1. AtomSpace Hypergraph Database

**Files**: `kernel/atomspace/atomspace_portable.c`, `kernel/atomspace/atomspace.h`

The AtomSpace is the central knowledge representation layer. It implements a typed
hypergraph database where nodes represent concepts, predicates, and variables, while
links represent relationships between atoms.

**Key Features**:
- Hash table-based atom storage (65,536 buckets)
- Thread-safe operations with platform-abstracted locking (CRITICAL_SECTION on Windows, pthread_mutex on POSIX)
- Support for 14+ atom types (ConceptNode, PredicateNode, VariableNode, NumberNode, InheritanceLink, SimilarityLink, EvaluationLink, etc.)
- Truth Values (strength, confidence, count) on every atom
- Attention Values (STI, LTI, VLTI) for importance-based resource allocation
- Incoming set tracking for reverse link traversal
- Reference counting for memory management

**API**:
```c
AtomSpace* atomspace_create(void);
void atomspace_destroy(AtomSpace *as);
uint32_t atomspace_add_node(AtomSpace *as, uint16_t type, const char *name);
uint32_t atomspace_add_link(AtomSpace *as, uint16_t type, uint32_t *outgoing, uint32_t arity);
void atomspace_remove_atom(AtomSpace *as, uint32_t atom_id);
Atom* atomspace_get_atom(AtomSpace *as, uint32_t atom_id);
void atomspace_set_tv(AtomSpace *as, uint32_t atom_id, TruthValue tv);
TruthValue atomspace_get_tv(AtomSpace *as, uint32_t atom_id);
```

### 2. PLN Inference Engine

**Files**: `kernel/reasoning/pln_engine.c`, `kernel/reasoning/pln_engine.h`

The Probabilistic Logic Networks (PLN) engine implements formal probabilistic inference
over the AtomSpace. It supports five core inference rules, each with mathematically
grounded truth value formulas.

**Inference Rules**:

| Rule | Description | TV Formula |
|------|-------------|------------|
| **Deduction** | A→B, B→C ⊢ A→C | sAC = sAB·sBC + (1-sAB)·(sC-sB·sBC)/(1-sB) |
| **Modus Ponens** | A, A→B ⊢ B | sB = sA·sAB + (1-sA)·sB·(1-sAB) |
| **Induction** | A→B, A→C ⊢ B→C | sBC = sAB·sAC + (1-sAB)·sC |
| **Abduction** | A→C, B→C ⊢ A→B | sAB = sAC·sBC + (1-sAC)·sB |
| **Revision** | TV1, TV2 ⊢ TV_merged | s = (s1·c1+s2·c2)/(c1+c2), c = (c1+c2)/(c1+c2+1) |

**Key Features**:
- Forward chaining with configurable max depth (default: 20)
- Rule priority-based selection
- Confidence threshold filtering
- Per-rule statistics tracking
- Extensible rule registration system

**API**:
```c
PLNEngine* pln_engine_create(AtomSpace *as);
void pln_engine_destroy(PLNEngine *engine);
int pln_engine_register_default_rules(PLNEngine *engine);
int pln_engine_infer(PLNEngine *engine, uint32_t *premises,
                     uint32_t premise_count, PLNResult *results,
                     uint32_t max_results);
int pln_engine_forward_chain(PLNEngine *engine, uint32_t *seeds,
                             uint32_t seed_count, uint32_t max_steps,
                             PLNResult *results, uint32_t max_results);
```

### 3. MOSES Evolutionary Learning Engine

**Files**: `kernel/learning/moses_engine.c`, `kernel/learning/moses_engine.h`

The Meta-Optimizing Semantic Evolutionary Search (MOSES) engine implements program
evolution using genetic programming. It evolves program trees that can represent
boolean logic, arithmetic, and conditional expressions.

**Program Node Types**:
- Boolean: AND, OR, NOT, TRUE, FALSE
- Arithmetic: PLUS, TIMES, SIN, LOG, EXP
- Control: IF (conditional)
- Terminals: CONST (float), INPUT (variable reference)

**Genetic Operators**:
- **Tournament Selection**: Configurable tournament size (default: 3)
- **Subtree Crossover**: Random subtree exchange between parents
- **Point Mutation**: Random node replacement with type-compatible alternatives
- **Elitism**: Top individuals preserved across generations

**Configuration**:
```c
typedef struct MOSESConfig {
    uint32_t population_size;      /* Default: 100 */
    uint32_t max_generations;      /* Default: 100 */
    float mutation_rate;           /* Default: 0.05 */
    float crossover_rate;         /* Default: 0.7 */
    float elitism_rate;           /* Default: 0.1 */
    uint32_t tournament_size;     /* Default: 3 */
    uint32_t max_program_depth;   /* Default: 6 */
    float target_fitness;         /* Default: 0.99 */
    uint32_t max_evals;           /* Default: 100000 */
    float complexity_penalty;     /* Default: 0.01 */
    uint32_t num_inputs;          /* Default: 2 */
} MOSESConfig;
```

### 4. Cognitive IPC Channels

**Files**: `kernel/ipc/cognitive_channel.c`, `kernel/ipc/cognitive_channel.h`

The Cognitive IPC system provides inter-process communication channels optimized for
atom-based messaging. Processes can exchange atom IDs through typed, prioritized channels.

**Channel Types**:
- `CHANNEL_TYPE_LOCAL`: Same-machine communication
- `CHANNEL_TYPE_REMOTE`: Cross-machine communication (via distributed layer)
- `CHANNEL_TYPE_BROADCAST`: One-to-many messaging

**Message Priorities**: LOW (0), NORMAL (1), HIGH (2), URGENT (3)

**Key Features**:
- Ring buffer message queues (1024 messages per channel)
- Up to 256 atom IDs per message
- Broadcast support across all channels
- Per-channel statistics (sent, received, bytes)
- Thread-safe operations

**API**:
```c
ChannelManager* channel_manager_create(void);
int channel_create(ChannelManager *mgr, uint32_t owner_pid,
                   uint32_t peer_pid, const char *name, ChannelType type);
int channel_send(ChannelManager *mgr, uint32_t channel_id,
                 uint32_t *atom_ids, uint32_t count, MessagePriority priority);
int channel_recv(ChannelManager *mgr, uint32_t channel_id, CogMessage *msg);
int channel_broadcast(ChannelManager *mgr, uint32_t sender_pid,
                      uint32_t *atom_ids, uint32_t count);
```

### 5. Pattern Matching Engine

**Files**: `kernel/pattern/pattern_matcher.c`, `kernel/pattern/pattern_matcher.h`

The Pattern Matcher implements graph pattern matching with variable binding over the
AtomSpace. It supports variable nodes as wildcards, type constraints, and multi-result
matching.

**Key Features**:
- Variable binding with backtracking
- Type-constrained matching
- Recursive subgraph matching
- Multiple result collection
- Configurable match limits

**API**:
```c
PatternMatcher* pattern_matcher_create(AtomSpace *as);
int pattern_match(PatternMatcher *pm, uint32_t pattern_id,
                  PatternResult *results, uint32_t max_results);
int pattern_match_link(PatternMatcher *pm, uint16_t link_type,
                       uint32_t *outgoing_pattern, uint32_t arity,
                       PatternResult *results, uint32_t max_results);
```

### 6. Distributed AtomSpace

**Files**: `kernel/distributed/distributed_atomspace.c`, `kernel/distributed/distributed_atomspace.h`

The Distributed AtomSpace layer enables cognitive operations across multiple machines.
It provides node discovery, load balancing, synchronization, and remote atom operations.

**Key Features**:
- Up to 64 cluster nodes
- Round-robin and latency-based load balancing
- Lazy and eager synchronization modes
- Node health monitoring (ping/heartbeat)
- Per-node statistics (atom count, load, latency)

**API**:
```c
DistributedAtomSpace* dist_atomspace_create(AtomSpace *local);
int dist_add_node(DistributedAtomSpace *das, const char *hostname, uint16_t port);
int dist_ping_node(DistributedAtomSpace *das, uint32_t node_id);
uint32_t dist_select_node(DistributedAtomSpace *das);
int dist_sync_all(DistributedAtomSpace *das);
```

### 7. Attention Allocation (ECAN)

**Files**: `kernel/attention/attention_portable.c`, `kernel/attention/attention.h`

The Economic Attention Networks (ECAN) module manages cognitive resource allocation
using an economic metaphor. Atoms compete for attention through STI (Short-Term
Importance) values.

**Key Features**:
- Attentional Focus with configurable capacity
- Importance spreading between linked atoms
- Hebbian learning for importance links
- STI/LTI rent and wage mechanisms
- Sorted focus set for efficient access

### 8. Reasoning Engine (Legacy)

**Files**: `kernel/reasoning/reasoning_portable.c`, `kernel/reasoning/reasoning.h`

The legacy reasoning engine provides a unified interface for PLN and URE (Unified Rule
Engine) operations. It wraps the PLN engine and provides forward/backward chaining
and pattern matching through a simplified API.

### 9. Cognitive 9P Filesystem

**Files**: `kernel/cognitive9p/cognitive9p_portable.c`, `kernel/cognitive9p/cognitive9p.h`

The Cognitive 9P filesystem exposes all cognitive services through the Plan 9 filesystem
protocol. Every cognitive operation is a file read or write.

**Filesystem Tree**:
```
/cognitive/
├── atomspace/     (atoms, queries, patterns, stats, count)
├── reasoning/     (pln/rules/proofs/beliefs, ure/forward/back, moses/)
├── attention/     (focus, importance, urgency, allocation, sti_funds, lti_funds)
├── memory/        (working/capacity/contents, episodic, semantic, procedural)
├── learning/      (supervised, unsupervised, reinforcement, meta)
├── perception/    (vision, audio, text, sensors)
└── action/        (motor, speech, commands)
```

## Cognitive Filesystems (`fs/`)

Each cognitive service is also exposed as a standalone filesystem module:

| Filesystem | Mount Point | Purpose |
|------------|-------------|---------|
| atomfs | `/atoms/` | AtomSpace CRUD operations |
| reasonfs | `/reasoning/` | PLN inference and rule management |
| attnfs | `/attention/` | ECAN stimulation and focus queries |
| learnfs | `/learning/` | MOSES evolution and URE rules |
| perceptfs | `/perception/` | Sensory input channels |
| actionfs | `/action/` | Motor output commands |

## Limbo Modules (`limbo/`)

High-level Limbo language bindings for cognitive services:

- `atomspace/atomspace.m` - AtomSpace ADT and operations
- `pln/pln.m` - PLN inference rules and chaining
- `attention/attention.m` - ECAN attention bank management
- `distributed/cognet.m` - Distributed cognitive networking

## Cognitive Filesystem Protocol

All cognitive operations follow the read/write file paradigm:

### Creating an Atom
```
echo "ConceptNode Socrates 0.9 0.8" > /atoms/new
cat /atoms/new  # Returns atom ID
```

### Querying Atoms
```
echo "ConceptNode" > /atoms/query
cat /atoms/query  # Returns matching atom IDs
```

### Performing Inference
```
echo "42 43" > /reasoning/pln  # Premise atom IDs
cat /reasoning/results/latest  # Read conclusion
```

### Stimulating Attention
```
echo "42 100" > /attention/stimulate  # Atom 42, +100 STI
cat /attention/focus  # Read atoms in attentional focus
```

## Distributed Architecture

The distributed cognitive network uses 9P/Styx for inter-node communication:

```
   Node A                    Node B                    Node C
┌──────────┐            ┌──────────┐            ┌──────────┐
│ AtomSpace│◄──9P/Styx─►│ AtomSpace│◄──9P/Styx─►│ AtomSpace│
│ PLN      │            │ PLN      │            │ PLN      │
│ ECAN     │            │ ECAN     │            │ ECAN     │
│ MOSES    │            │ MOSES    │            │ MOSES    │
│ IPC      │            │ IPC      │            │ IPC      │
└──────────┘            └──────────┘            └──────────┘
     │                       │                       │
     └───────────────────────┴───────────────────────┘
                    Cognitive Namespace
                /net/cognitive/<host>/atoms/
```

Each node:
1. Runs its own cognitive kernel with all modules
2. Exports its AtomSpace via 9P
3. Can mount remote AtomSpaces
4. Synchronizes knowledge automatically via the Distributed AtomSpace layer
5. Communicates atom operations via Cognitive IPC channels

## Build System

The project uses CMake with the following build options:

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_KERNEL_MODULES` | ON | Build core kernel modules |
| `BUILD_FILESYSTEMS` | ON | Build cognitive filesystem modules |
| `BUILD_USERSPACE` | ON | Build userspace client libraries |
| `BUILD_TESTS` | ON | Build comprehensive test suite |
| `BUILD_PLN_ENGINE` | ON | Build PLN inference engine |
| `BUILD_MOSES_ENGINE` | ON | Build MOSES evolutionary learning |
| `BUILD_IPC` | ON | Build cognitive IPC channels |
| `BUILD_PATTERN_MATCHER` | ON | Build pattern matching engine |
| `ENABLE_DISTRIBUTED` | ON | Enable distributed features |

### Building on Linux/macOS

```bash
mkdir build && cd build
cmake .. -DBUILD_KERNEL_MODULES=ON -DBUILD_TESTS=ON
make -j$(nproc)
ctest --output-on-failure
```

### Building on Windows

```powershell
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
ctest -C Release --output-on-failure
```

## Test Suite

The test suite covers all kernel modules with 56 tests across 7 suites:

| Test Suite | Tests | Description |
|------------|-------|-------------|
| `test_atomspace` | 8 | AtomSpace CRUD, hash table, global instance, bulk operations |
| `test_reasoning` | 13 | PLN deduction, forward chaining, multi-rule inference |
| `test_cognitive9p` | 10 | 9P filesystem read/write/readdir, tree structure |
| `test_pln_engine` | 4 | PLN engine lifecycle, rule registration, deduction, forward chain |
| `test_moses_engine` | 7 | Program eval, boolean logic, copy, complexity, crossover, mutation, evolution |
| `test_cognitive_ipc` | 10 | Channel CRUD, send/recv, priority, broadcast, bulk, distributed nodes |
| `test_integration` | 4 | Full pipeline: Socrates syllogism, large KB, distributed network, boot sequence |

## Cross-Platform Compatibility

The portable implementation uses platform-abstracted primitives:

| Feature | Windows | Linux/macOS |
|---------|---------|-------------|
| Locking | CRITICAL_SECTION | pthread_mutex |
| Timing | GetTickCount64() | clock_gettime() |
| Networking | ws2_32 (Winsock) | POSIX sockets |
| Math | Built-in | libm |
| Threads | Windows threads | pthreads |

## Future Work

- **GPU Acceleration**: CUDA/OpenCL kernels for attention spreading and pattern matching
- **Persistent Storage**: Memory-mapped AtomSpace with WAL journaling
- **WebSocket API**: Real-time cognitive event streaming
- **MOSES-AtomSpace Integration**: Evolved programs stored as AtomSpace atoms
- **Limbo Bindings**: Full Limbo language interface for cognitive operations
- **Real Distributed Transport**: TCP/UDP transport for cross-machine atom operations
- **Cognitive Scheduling**: Priority-based kernel process scheduling for cognitive tasks
