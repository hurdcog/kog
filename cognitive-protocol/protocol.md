# Cognitive 9P Protocol Specification (9P-Cog)

## Overview

The **9P-Cog** protocol extends the standard 9P (Styx) protocol with cognitive
operations for distributed AtomSpace access, reasoning, and learning.

## Design Principles

1. **Everything is a file**: Atoms, truth values, and inference results are
   exposed as files in a cognitive namespace.
2. **9P native**: All extensions stay within the 9P framing and tag model.
3. **Backward compatible**: Standard 9P clients can access the namespace;
   cognitive clients get the extended operations.

## Extended Message Types

| Type       | Number | Description                          |
|------------|--------|--------------------------------------|
| Tcogquery  | 200    | Pattern query against AtomSpace      |
| Rcogquery  | 201    | Query result (list of atoms)         |
| Tcogatom   | 202    | Retrieve a single atom by ID         |
| Rcogatom   | 203    | Single atom data                     |
| Tcogreason | 204    | Invoke forward/backward chaining     |
| Rcogreason | 205    | Inference results                    |
| Tcoglearn  | 206    | Submit training examples             |
| Rcoglearn  | 207    | Learned model / updated truth values |

## Message Formats

### Tcogquery / Rcogquery

```
Tcogquery  tag[2] fid[4] pattern[s]
Rcogquery  tag[2] count[2] atom[count * atom_record]

atom_record:
  id[4] type[2] namelen[2] name[namelen]
  tv_strength[4] tv_confidence[4]
  av_sti[2] av_lti[2]
```

### Tcogatom / Rcogatom

```
Tcogatom   tag[2] fid[4] atomid[4]
Rcogatom   tag[2] atom_record
```

### Tcogreason / Rcogreason

```
Tcogreason tag[2] fid[4] query[s] direction[1] depth[4]
  direction: 0 = forward, 1 = backward

Rcogreason tag[2] count[2] atom[count * atom_record]
```

### Tcoglearn / Rcoglearn

```
Tcoglearn  tag[2] fid[4] nexamples[2] example[nexamples * atom_record]
Rcoglearn  tag[2] nupdated[2] atom[nupdated * atom_record]
```

## Cognitive File Namespace

Each cognitive node exports the following namespace via 9P:

```
/cog/
├── atomspace/
│   ├── nodes/         # ConceptNodes, PredicateNodes, ...
│   ├── links/         # InheritanceLinks, EvaluationLinks, ...
│   └── stats          # Atom count, link count, etc.
├── attention/
│   ├── focus          # Current attentional focus (high-STI atoms)
│   ├── bank           # STI/LTI totals
│   └── ctl            # Control: cycle, stimulate, decay
├── reasoning/
│   ├── ure/
│   │   ├── forward    # Write target atom_id to trigger forward chain
│   │   ├── backward   # Write goal atom_id to trigger backward chain
│   │   ├── rules      # Active rule list
│   │   └── ctl        # maxdepth, maxresults
│   └── pln/
│       └── ctl        # PLN-specific parameters
└── learning/
    └── ctl            # Trigger learning cycle
```

## Example: Distributed Query

```sh
# Mount remote cognitive node
mount -A tcp!cognode1!564 /n/node1

# Query for concept atoms matching 'animal'
echo 'query ConceptNode animal' > /n/node1/cog/reasoning/ure/forward
cat /n/node1/cog/atomspace/nodes/

# Stimulate an atom on the remote node
echo 'atom:42 150' > /n/node1/cog/attention/ctl

# Run a forward reasoning step on the remote node
echo '42 5' > /n/node1/cog/reasoning/ure/forward
cat /n/node1/cog/reasoning/ure/forward
```

## Security

- Cognitive namespaces respect standard 9P permission bits.
- Write access to `/cog/atomspace` requires appropriate credentials.
- Read-only mirrors can be exported for monitoring.

## Error Codes

| Code | Meaning                               |
|------|---------------------------------------|
| ENOATOM | Atom ID not found                  |
| EBADRULE| Rule ID not in rule base           |
| EDEPTH  | Max inference depth exceeded       |
| EPARSE  | Pattern parse error                |
