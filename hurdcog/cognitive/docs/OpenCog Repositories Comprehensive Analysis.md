# OpenCog Repositories Comprehensive Analysis

## Overview
Based on research of the OpenCog GitHub organization (https://github.com/opencog), there are **87 repositories** organized into several categories:

## Repository Categories

### 1. OpenCog AtomSpace (Core System)
**Active, stable and supported as of 2025**

1. **atomspace** - The OpenCog (hyper-)graph database and graph rewriting system
   - Language: C++
   - Stars: 902, Forks: 245
   - Core hypergraph database and query engine

2. **atomspace-storage** - AtomSpace StorageNode base system
   - Language: C++
   - Base class for saving, loading, sending and receiving Atoms

3. **cogserver** - Distributed AtomSpace Network Server
   - Language: C++
   - Stars: 22, Forks: 25
   - Networking, json, websockets

4. **atomspace-cog** - Distributed AtomSpace Network client
   - Language: C++
   - Stars: 12, Forks: 9
   - Network client for distributed AtomSpace

5. **atomspace-rocks** - AtomSpace Graph Database RocksDB backend
   - Language: C++
   - Stars: 17, Forks: 10
   - Disk I/O storage based on RocksDB

6. **atomspace-pgres** - Postgres StorageNode for the AtomSpace
   - Language: C++
   - Postgres StorageNode (works but old, deprecated)

7. **matrix** - AtomSpace Graph Sparse Vector Library
   - Language: Scheme
   - Working with graphs as embeddings in sparse vectors

8. **link-grammar** - The CMU Link Grammar natural language parser
   - Language: C
   - Stars: 398, Forks: 118
   - Maximal Planar Graph (MPG) parsing, natural language parsing

9. **lg-atomese** - Atomese API for Link Grammar
   - Language: C++
   - Stars: 7, Forks: 5

10. **docker** - Docker containers for OpenCog - Robot Operating System (ROS)
    - Language: Dockerfile
    - Stars: 77, Forks: 76
    - System integration and demos

### 2. OpenCog Research (Active Research)

11. **learn** - Neuro-symbolic interpretation learning
    - Language: Scheme
    - Stars: 165, Forks: 37
    - Symbolic learning (mature, batch-based processing)

12. **agents** - Refactoring learning for an interactive environment
    - Language: C++
    - Interactive learning agents

13. **sensory** - Low-level sensory I/O Atoms
    - Language: C++
    - Stars: 9, Forks: 1
    - Dataflow of graphlets to/from external world

14. **motor** - Sensorimotor research
    - Language: CMake
    - Stars: 1, Forks: 0
    - Controlling focus of sensory attention, perception-action

15. **generate** - Generate networks from syntax
    - Language: C++
    - Stars: 31, Forks: 12
    - Natural language, math proofs, action plans, biome/reactome nets

### 3. OpenCog Utilities and Tools

16. **cogutil** - Very low-level C++ programming utilities
    - Language: C++
    - Stars: 40, Forks: 84
    - Used by several components

17. **ocpkg** - Installing, packaging, deploying and managing OpenCog
    - Language: Shell
    - Stars: 21, Forks: 50

18. **benchmark** - Benchmarking the AtomSpace and pattern matcher
    - Language: Scheme
    - Stars: 6, Forks: 9

### 4. OpenCog Machine Learning

19. **asmoses** - MOSES Machine Learning
    - Language: C++
    - Stars: 43, Forks: 32
    - Meta-Optimizing Semantic Evolutionary Search for AtomSpace

20. **miner** - Frequent and surprising subhypergraph pattern miner
    - Language: Scheme
    - Stars: 9, Forks: 16

### 5. OpenCog Fossils (Deprecated/Obsolete)

21. **opencog** - A framework for integrated Artificial Intelligence & AGI
    - Language: Scheme
    - Stars: 2.4k, Forks: 729
    - **Note: Main legacy framework, no longer actively maintained**

22. **attention** - OpenCog Attention Allocation Subsystem
    - Language: C++
    - Stars: 14, Forks: 24
    - **Status: Fossil - no longer maintained**

23. **ure** - Unified Rule Engine
    - Language: C++
    - Stars: 55, Forks: 30
    - **Status: NO LONGER MAINTAINED, superseded by https://github.com/trueagi-io/chaining**

24. **pln** - Probabilistic Logic Network
    - Language: Scheme
    - Stars: 16, Forks: 17
    - **Status: NO LONGER MAINTAINED, superseded by https://github.com/trueagi-io/pln-experimental**

### 6. OpenCog Specialized Applications

25. **agi-bio** - Genomic and Proteomic data exploration
    - Language: Scheme
    - Stars: 25, Forks: 32
    - Used by MOZI and rejuve.bio

26. **vision** - Atomese wrappers around OpenCV subset
    - Language: C++
    - Stars: 3, Forks: 3
    - Extracting structure from images, video

27. **cheminformatics** - Molecular Chemistry
    - Language: CMake
    - Stars: 3, Forks: 1

28. **spacetime** - Save, track and query 3D+time locations
    - Language: C++
    - Stars: 13, Forks: 14
    - Octree spatial bounding boxes and time intervals

### 7. OpenCog Web and Interfaces

29. **link-grammar-website** - Copy of last working version of link-grammar website
    - Language: HTML
    - Stars: 1, Forks: 0

30. **.github** - Public profile README
    - Stars: 0, Forks: 14

### 8. Additional Repositories (Partial List)

31. **evidence** - Hierarchical similarity and evidence-gathering
32. Various other specialized repositories for specific research areas

## Key Architecture Components

### Core Technologies
- **AtomSpace**: Hypergraph database and knowledge representation
- **CogServer**: Network server for distributed processing
- **Link Grammar**: Natural language parsing
- **MOSES**: Evolutionary machine learning
- **PLN**: Probabilistic logic networks (deprecated)
- **URE**: Unified rule engine (deprecated)

### Programming Languages Used
- **C++**: Core system components
- **Scheme**: Logic programming and AI algorithms
- **Python**: Some utilities and bindings
- **JavaScript**: Web interfaces
- **CMake**: Build system

### Storage Backends
- **RocksDB**: Primary disk storage (atomspace-rocks)
- **PostgreSQL**: Database storage (atomspace-pgres, deprecated)
- **Network**: Distributed storage (atomspace-cog)

## Current Status (2025)

### Active Projects
- AtomSpace core system and related storage/networking
- Learning and research components
- Link Grammar parser
- Docker containers and deployment tools

### Deprecated Projects
- Original OpenCog framework
- PLN (Probabilistic Logic Networks)
- URE (Unified Rule Engine)
- Attention allocation system

### Migration Path
Many components have been superseded by projects under:
- https://github.com/trueagi-io/ (TrueAGI organization)
- OpenCog Hyperon (developed by Singularity.net)

## Repository Statistics Summary
- **Total Repositories**: 87
- **Active Core Repositories**: ~15-20
- **Deprecated/Fossil Repositories**: ~25-30
- **Specialized/Research Repositories**: ~30-40
- **Most Starred**: opencog/opencog (2.4k stars)
- **Most Active**: atomspace, learn, cogserver

This analysis provides the foundation for comparing OpenCog subsystems with GNU packages and evaluating their potential as solutions for GNU Hurd issues.

