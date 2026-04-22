# Cognitive Filesystem Reference

## Overview

The Cognitive Filesystem exposes all cognitive services as files following the Inferno/Plan 9 "everything is a file" philosophy. Each cognitive subsystem is mounted as a directory in the cognitive namespace, and all operations are performed through standard file read/write operations.

## Namespace Layout

```
/cognitive/
├── atoms/                  # AtomSpace hypergraph database
│   ├── new                 # Create atoms (write)
│   ├── delete              # Delete atoms (write)
│   ├── stats               # Statistics (read)
│   ├── query               # Type-based queries (write/read)
│   ├── concepts/           # ConceptNode directory
│   ├── predicates/         # PredicateNode directory
│   ├── schemas/            # SchemaNode directory
│   ├── links/              # Link directory
│   ├── truth/              # Truth value access
│   │   └── <id>            # Per-atom truth value
│   └── attention/          # Attention value access
│       └── <id>            # Per-atom attention value
├── reasoning/              # PLN inference engine
│   ├── pln                 # PLN inference (write premises, read conclusions)
│   ├── forward             # Forward chaining (write/read)
│   ├── backward            # Backward chaining (write/read)
│   ├── rules/              # Available inference rules
│   │   ├── deduction       # Rule description (read)
│   │   ├── induction       # Rule description (read)
│   │   ├── abduction       # Rule description (read)
│   │   ├── modus_ponens    # Rule description (read)
│   │   └── modus_tollens   # Rule description (read)
│   ├── results/            # Recent inference results
│   │   └── latest          # Most recent result (read)
│   └── stats               # Reasoning statistics (read)
├── attention/              # ECAN attention allocation
│   ├── stimulate           # Stimulate atom (write)
│   ├── focus               # Attentional focus atoms (read)
│   ├── threshold           # AF threshold (read/write)
│   ├── spread              # Spread importance (write)
│   ├── rent                # Trigger rent collection (write)
│   ├── stats               # Attention statistics (read)
│   └── bank/               # Attention bank state
│       ├── sti_funds       # Available STI funds (read)
│       ├── lti_funds       # Available LTI funds (read)
│       └── total_sti       # Total STI in system (read)
├── learning/               # MOSES/URE learning
│   ├── moses/              # MOSES evolutionary learning
│   │   ├── evolve          # Start evolution (write/read)
│   │   ├── population      # Population stats (read)
│   │   └── params          # MOSES parameters (read/write)
│   ├── ure/                # Unified Rule Engine
│   │   ├── rules           # URE rules (read)
│   │   ├── engine          # URE query (write/read)
│   │   └── config          # URE configuration (read/write)
│   └── stats               # Learning statistics (read)
├── perception/             # Sensory input channels
│   ├── text                # Text input (write)
│   ├── numeric             # Numeric data (write)
│   ├── spatial             # Spatial data (write)
│   ├── temporal            # Timestamped events (write)
│   ├── raw                 # Raw byte streams (write)
│   └── stats               # Perception statistics (read)
├── action/                 # Motor output channels
│   ├── execute             # Execute action (write)
│   ├── queue               # Pending actions (read)
│   ├── history             # Action history (read)
│   └── stats               # Action statistics (read)
└── net/                    # Distributed cognitive network
    └── cognitive/
        ├── connect         # Connect to remote node (write)
        ├── replicate       # Replicate atom to remote (write)
        ├── status          # Cluster status (read)
        └── <host>/         # Remote node namespace
            └── atoms/      # Remote AtomSpace (mounted)
```

## File Operations

### `/atoms/new` - Create Atom

Write format: `<type> <name> <strength> <confidence>`

```bash
echo "ConceptNode Socrates 0.95 0.85" > /cognitive/atoms/new
# Returns: atom ID (e.g., "42")
```

### `/atoms/<id>` - Read Atom

Read format: `<id> <type> <name> <strength> <confidence> <sti> <lti>`

```bash
cat /cognitive/atoms/42
# Returns: "42 ConceptNode Socrates 0.9500 0.8500 100 0"
```

### `/atoms/truth/<id>` - Set Truth Value

Write format: `<strength> <confidence>`

```bash
echo "0.99 0.95" > /cognitive/atoms/truth/42
```

### `/reasoning/pln` - PLN Inference

Write format: space-separated premise atom IDs
Read format: `<conclusion_id> <strength> <confidence> <rule> <steps>`

```bash
echo "42 43" > /cognitive/reasoning/pln
cat /cognitive/reasoning/results/latest
# Returns: "1042 0.9405 0.8075 deduction 1"
```

### `/attention/stimulate` - Stimulate Atom

Write format: `<atom_id> <amount>`

```bash
echo "42 100" > /cognitive/attention/stimulate
```

### `/attention/focus` - Get Attentional Focus

Read format: space-separated atom IDs with STI above threshold

```bash
cat /cognitive/attention/focus
# Returns: "42 43 45"
```
