# Inferno Kernel Modules for OpenCog AGI

## Overview

This directory contains the Inferno OS kernel modules that implement OpenCog cognitive primitives as first-class kernel services. These modules transform Inferno into a **Cognitive Operating System** where intelligence emerges from the OS itself.

## Architecture

### Kernel-Level Cognitive Services

Traditional AGI systems run as applications on general-purpose operating systems. This architecture inverts that relationship: **the operating system IS the cognitive system**. Every system call, every resource allocation, every process interaction is mediated through cognitive primitives.

### Module Structure

```
inferno-kern/
├── atomspace/          # Hypergraph knowledge representation
│   └── atomspace.b    # AtomSpace kernel module
├── attention/          # Attention allocation mechanism
│   └── attention.b    # Attention kernel module
├── inference/          # Reasoning engines
│   ├── pln.b          # Probabilistic Logic Networks
│   ├── ure.b          # Unified Rule Engine
│   └── moses.b        # Meta-Optimizing Evolutionary Search
├── pattern/            # Pattern matching
│   └── pattern.b      # Pattern matcher kernel module
├── learning/           # Learning subsystems
│   ├── temporal.b     # Temporal learning
│   ├── spatial.b      # Spatial learning
│   └── causal.b       # Causal learning
└── tensor/             # Neural-symbolic integration
    └── tensor.b       # ATen/PyTorch integration
```

## Implemented Modules

### 1. AtomSpace (`atomspace/atomspace.b`)

**Status:** ✅ Implemented

The AtomSpace module implements the OpenCog hypergraph as a kernel-level data structure. This makes atoms, truth values, and attention values first-class kernel objects.

**Key Features:**
- Kernel-level atom storage
- Truth value propagation
- Attention value management
- Incoming/outgoing set tracking
- Hash-based indexing for fast lookup
- Type-based atom organization

**Data Structures:**
- `Atom`: Core atom representation (nodes and links)
- `TruthValue`: Probabilistic truth representation
- `AttentionValue`: Importance-based attention
- `Space`: AtomSpace database
- `HashTable`: Fast indexing structure

**Device Interface:**
```
/dev/cog/atomspace
```

**Usage Example:**
```limbo
include "atomspace.m";

atomspace: AtomSpace;
atomspace = load AtomSpace AtomSpace->PATH;
atomspace->init();

# Create a concept node
tv := atomspace->new_truth_value(0.8, 0.9);
cat := space.create_node("ConceptNode", "cat", tv);

# Create an inheritance link
mammal := space.create_node("ConceptNode", "mammal", tv);
link := space.create_link("InheritanceLink", (cat, mammal), tv);
```

### 2. Attention Allocation (`attention/attention.b`)

**Status:** ✅ Implemented

The Attention module implements attention allocation as a kernel-level process scheduler. This replaces traditional CPU scheduling with cognitive importance-based scheduling.

**Key Features:**
- Attentional focus management
- STI/LTI/VLTI importance tracking
- Attention spreading/diffusion
- Hebbian learning for attention
- Cognitive garbage collection (forgetting)
- Importance-based scheduling

**Data Structures:**
- `AttentionalFocus`: Set of atoms in focus
- `AttentionBank`: Central attention allocation
- `ImportanceUpdater`: Rent/wage mechanism
- `HebbianUpdater`: Hebbian learning

**Device Interface:**
```
/dev/cog/attention
```

**Usage Example:**
```limbo
include "attention.m";

attention: Attention;
attention = load Attention Attention->PATH;
attention->init();

# Stimulate an atom
attention->stimulate_atom(cat, 100);

# Spread attention from an atom
attention->spread_attention(cat);

# Get next atom to process (cognitive scheduling)
next := attention->schedule_next();
```

### 3. Inference Engines (`inference/`)

**Status:** 🔄 Planned

Inference engines as kernel services for logical reasoning.

**Modules:**
- `pln.b`: Probabilistic Logic Networks
- `ure.b`: Unified Rule Engine
- `moses.b`: Meta-Optimizing Evolutionary Search

**Device Interface:**
```
/dev/cog/inference
```

### 4. Pattern Matcher (`pattern/pattern.b`)

**Status:** ✅ Implemented

The Pattern Matcher module implements kernel-level pattern matching and query execution over the AtomSpace hypergraph. It provides the foundation for PLN, URE, and MOSES inference engines.

**Key Features:**
- Scheme-like pattern syntax (OpenCog Atomese compatible)
- Variable binding and unification
- Constraint satisfaction (type, value, TV, AV)
- Graph traversal matching
- Query optimization and caching
- Attention-based candidate prioritization

**Data Structures:**
- `Variable`: Pattern variables ($X, $Y)
- `Term`: Variables or constant atoms
- `Clause`: Pattern clauses (link type + arguments)
- `Constraint`: Type, value, TV, AV constraints
- `Pattern`: Complete pattern with variables, clauses, constraints
- `BindingSet`: Variable-to-atom bindings
- `MatchResult`: Query results with binding sets

**Device Interface:**
```
/dev/cog/pattern
```

**Usage Example:**
```limbo
include "pattern.m";

pattern: PatternMatcher;
pattern = load PatternMatcher PatternMatcher->PATH;

# Compile and execute a query
query := "(BindLink (VariableList $X) (InheritanceLink $X (ConceptNode \"mammal\")))";
result := pattern.query(query, atomspace);

# Process results
for (bs := result.binding_sets; bs != nil; bs = tl bs) {
    bindings := hd bs;
    x_value := bindings.get("$X");
    sys->print("Found: %s\n", x_value.name);
}
```

### 5. Learning Subsystems (`learning/`)

**Status:** 🔄 Planned

Learning mechanisms as kernel services.

**Modules:**
- `temporal.b`: Temporal sequence learning
- `spatial.b`: Spatial relationship learning
- `causal.b`: Causal relationship learning

**Device Interface:**
```
/dev/cog/learning
```

### 6. Tensor Integration (`tensor/`)

**Status:** 🔄 Planned

ATen/PyTorch integration for neural-symbolic computing.

**Device Interface:**
```
/dev/cog/tensor
```

## Building and Installation

### Prerequisites

1. **Inferno OS**: Install Inferno OS from http://www.vitanuova.com/inferno/
2. **Limbo Compiler**: Included with Inferno
3. **Dis Virtual Machine**: Included with Inferno

### Build Instructions

```bash
# Navigate to inferno-kern directory
cd inferno-kern

# Compile AtomSpace module
limbo -o atomspace/atomspace.dis atomspace/atomspace.b

# Compile Attention module
limbo -o attention/attention.dis attention/attention.b

# Install to Inferno module directory
cp atomspace/atomspace.dis /usr/inferno/dis/
cp attention/attention.dis /usr/inferno/dis/
```

### Loading Modules

```limbo
# In your Limbo program
include "atomspace.m";
include "attention.m";

atomspace: AtomSpace;
attention: Attention;

init()
{
    atomspace = load AtomSpace AtomSpace->PATH;
    atomspace->init();
    
    attention = load Attention Attention->PATH;
    attention->init();
}
```

## Device Files

Cognitive operations are exposed as device files in `/dev/cog/`:

```
/dev/cog/
├── atomspace          # AtomSpace hypergraph database
├── attention          # Attention allocation mechanism
├── inference          # Inference engine
├── pattern            # Pattern matcher
├── learning           # Learning subsystem
├── perception         # Perceptual processing
├── action             # Action generation
└── metacog            # Meta-cognitive monitoring
```

### Reading/Writing Device Files

```bash
# Create a concept (write to atomspace)
echo "ConceptNode cat" > /dev/cog/atomspace

# Query for patterns (read from pattern matcher)
cat /dev/cog/pattern < query.scm

# Monitor attention dynamics (tail attention device)
tail -f /dev/cog/attention

# Trigger inference (write to inference engine)
echo "InheritanceLink cat mammal" > /dev/cog/inference
```

## Cognitive System Calls

New system calls for cognitive operations (to be implemented in C kernel):

```c
// AtomSpace operations
int atom_create(char *type, char *name, TruthValue tv);
int atom_link(int *atoms, int count, char *link_type);
Atom* atom_query(Pattern *pattern);
int atom_update_tv(int atom_id, TruthValue tv);
int atom_update_av(int atom_id, AttentionValue av);

// Inference operations
int infer_pln(Atom *premises, Atom *conclusion);
int infer_ure(RuleSet *rules, Atom *target);
int infer_moses(Problem *problem, Solution *solution);

// Attention allocation
int attention_focus(Atom *atom, int sti_delta);
int attention_spread(Atom *source, float diffusion_rate);
AttentionValue attention_get(int atom_id);

// Pattern matching
Pattern* pattern_compile(char *pattern_expr);
BindLink* pattern_match(Pattern *pattern, AtomSpace *space);
int pattern_execute(BindLink *binding);
```

## Distributed Cognitive Computing

### 9P Protocol Extension

Cognitive operations are network-transparent through the 9P protocol:

```bash
# Mount remote AtomSpace
mount -A tcp!cognode1!9999 /n/remote/atomspace

# Query distributed hypergraph
cat /n/remote/atomspace/query < distributed_pattern.scm

# Synchronize attention values
cp /cog/attention/* /n/remote/atomspace/attention/

# Distributed inference
echo "InferenceTask task1" > /n/remote/inference
```

### Cognitive Cluster

Multiple Inferno nodes form a cognitive cluster:

```
CogNode1 (AtomSpace Primary)
    ↕ 9P Network
CogNode2 (Inference Engine)
    ↕ 9P Network
CogNode3 (Learning Subsystem)
```

## Performance Considerations

### Memory Management

- **Attention-based GC**: Atoms below forgetting threshold are garbage collected
- **Memory-mapped files**: Large hypergraphs use memory-mapped storage
- **Lazy loading**: Atoms loaded on-demand from disk

### Concurrency

- **CSP-style**: Limbo uses Communicating Sequential Processes
- **Channel-based**: Inter-module communication via channels
- **Lock-free**: Attention allocation uses lock-free algorithms

### Optimization

- **Hash indexing**: O(1) atom lookup by name/type
- **Attention caching**: Frequently accessed atoms cached in AF
- **Distributed load**: Cognitive workload distributed across cluster

## Testing

### Unit Tests

```bash
# Run AtomSpace tests
limbo atomspace/test_atomspace.b

# Run Attention tests
limbo attention/test_attention.b
```

### Integration Tests

```bash
# Run full cognitive stack test
limbo test_cognitive_stack.b
```

### Benchmarks

```bash
# Benchmark AtomSpace operations
limbo bench_atomspace.b

# Benchmark attention allocation
limbo bench_attention.b
```

## Development

### Adding New Modules

1. Create module directory: `mkdir inferno-kern/mymodule`
2. Create module file: `touch inferno-kern/mymodule/mymodule.b`
3. Implement module interface
4. Add device file handler
5. Update this README

### Module Interface Template

```limbo
implement MyModule;

include "sys.m";
    sys: Sys;

MyModule: module {
    PATH: con "/dev/cog/mymodule";
    
    init: fn();
    # Add your functions here
};

init()
{
    sys = load Sys Sys->PATH;
    # Initialize module
}
```

## Documentation

- **Inferno Manual**: http://www.vitanuova.com/inferno/papers/
- **Limbo Language**: http://www.vitanuova.com/inferno/papers/limbo.html
- **9P Protocol**: https://9p.io/sys/man/5/INDEX.html
- **OpenCog Wiki**: https://wiki.opencog.org/

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Implement your module
4. Add tests
5. Submit a pull request

## License

Copyright (C) 2025 OpenCog Foundation

Licensed under the GNU Affero General Public License v3.0 or later (AGPL-3.0-or-later).

See LICENSE file for details.

## References

- Plan 9 from Bell Labs: https://9p.io/plan9/
- Inferno Operating System: http://www.vitanuova.com/inferno/
- OpenCog Framework: https://opencog.org/
- ATen/PyTorch: https://pytorch.org/

## Status Summary

| Module | Status | Device | Description |
|--------|--------|--------|-------------|
| AtomSpace | ✅ Implemented | /dev/cog/atomspace | Hypergraph knowledge base |
| Attention | ✅ Implemented | /dev/cog/attention | Attention allocation |
| PLN | 🔄 Planned | /dev/cog/inference/pln | Probabilistic logic |
| URE | 🔄 Planned | /dev/cog/inference/ure | Rule engine |
| MOSES | 🔄 Planned | /dev/cog/inference/moses | Evolutionary search |
| Pattern | ✅ Implemented | /dev/cog/pattern | Pattern matching |
| Learning | 🔄 Planned | /dev/cog/learning | Learning subsystems |
| Tensor | 🔄 Planned | /dev/cog/tensor | Neural integration |

## Next Steps

1. ✅ Implement AtomSpace kernel module
2. ✅ Implement Attention kernel module
3. ✅ Implement Pattern matcher (1,400+ lines)
4. 🔄 Implement PLN inference
5. 🔄 Implement URE inference
6. 🔄 Implement MOSES learning
7. 🔄 Implement Tensor integration
8. 🔄 Create device file handlers
9. 🔄 Add system call interface
10. 🔄 Test on Inferno OS
