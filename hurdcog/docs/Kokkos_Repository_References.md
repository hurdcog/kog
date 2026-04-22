# Kokkos Ecosystem Repository References

**Generated:** August 3, 2025  
**Context:** Supporting documentation for Kokkos Integration Analysis

## Core Kokkos Repositories

### Primary Components
1. **kokkos/kokkos** (2,270 stars) - Core programming model
   - URL: https://github.com/kokkos/kokkos
   - Description: C++ Performance Portability Programming Ecosystem: The Programming Model - Parallel Execution and Memory Abstraction
   - Status: Active, mature
   - Key Features: CUDA, HIP, SYCL, HPX, OpenMP, C++ threads backends

2. **kokkos/kokkos-kernels** (348 stars) - Math kernels
   - URL: https://github.com/kokkos/kokkos-kernels  
   - Description: BLAS, Sparse BLAS and Graph Kernels
   - Status: Active, mature
   - Key Features: Linear algebra, sparse matrix operations, graph algorithms

3. **kokkos/kokkos-remote-spaces** (47 stars) - Distributed computing
   - URL: https://github.com/kokkos/kokkos-remote-spaces
   - Description: Distributed View Extension for Kokkos
   - Status: Active, experimental
   - Key Features: SHMEM, NVSHMEM, ROCSHMEM, MPI backends

### Development and Integration Tools
4. **kokkos/kokkos-tools** (134 stars) - Profiling and debugging
   - URL: https://github.com/kokkos/kokkos-tools
   - Description: Profiling and Debugging Tools
   - Status: Active, mature
   - Key Features: Memory analysis, performance monitoring, debugging

5. **kokkos/kokkos-tutorials** (337 stars) - Learning resources
   - URL: https://github.com/kokkos/kokkos-tutorials
   - Description: Tutorials for the Kokkos C++ Performance Portability Programming Ecosystem
   - Status: Active, educational
   - Key Features: Hands-on examples, learning materials

### Language Bindings and Extensions
6. **kokkos/pykokkos** (113 stars) - Python integration
   - URL: https://github.com/kokkos/pykokkos
   - Description: Performance portable parallel programming in Python
   - Status: Active, experimental
   - Key Features: Python bindings for Kokkos

7. **kokkos/pykokkos-base** (28 stars) - Python data interoperability
   - URL: https://github.com/kokkos/pykokkos-base
   - Description: Python bindings for data interoperability with Kokkos (View, DynRankView)
   - Status: Active, stable
   - Key Features: Data exchange between Python and Kokkos

8. **kokkos/kokkos-fortran-interop** (37 stars) - Fortran integration
   - URL: https://github.com/kokkos/kokkos-fortran-interop
   - Description: Tools and interfaces for Fortran-C++ interactions using Kokkos
   - Status: Active, stable
   - Key Features: Fortran interoperability

### Specialized Mathematics and Algorithms
9. **kokkos/kokkos-fft** (41 stars) - Fast Fourier Transform
   - URL: https://github.com/kokkos/kokkos-fft
   - Description: A shared-memory FFT for the Kokkos ecosystem
   - Status: Active, experimental
   - Key Features: Performance portable FFT implementations

10. **kokkos/kokkos-comm** (21 stars) - MPI wrapper
    - URL: https://github.com/kokkos/kokkos-comm
    - Description: Experimental MPI Wrapper for Kokkos
    - Status: Active, experimental
    - Key Features: MPI integration with Kokkos

11. **kokkos/stdBLAS** (144 stars) - Standard BLAS implementation
    - URL: https://github.com/kokkos/stdBLAS
    - Description: Reference Implementation for stdBLAS
    - Status: Active, standards-track
    - Key Features: C++ standard library BLAS operations

### Standards and Memory Management
12. **kokkos/mdspan** (471 stars) - Multi-dimensional arrays
    - URL: https://github.com/kokkos/mdspan
    - Description: Reference implementation of mdspan targeting C++23
    - Status: Active, standards-track
    - Key Features: C++23 mdspan standard implementation

### Infrastructure and Resilience
13. **kokkos/kokkos-resilience** (6 stars) - Fault tolerance
    - URL: https://github.com/kokkos/kokkos-resilience
    - Description: Resilience Extensions for Kokkos
    - Status: Active, experimental
    - Key Features: Fault tolerance, checkpointing, recovery

14. **kokkos/ci-containers** (1 star) - Continuous integration
    - URL: https://github.com/kokkos/ci-containers
    - Description: Container images for CI/CD
    - Status: Active, infrastructure
    - Key Features: Docker containers for testing

### Documentation and Examples
15. **kokkos/kokkos-core-wiki** (5 stars) - Documentation
    - URL: https://github.com/kokkos/kokkos-core-wiki
    - Description: Wiki documentation for Kokkos Core
    - Status: Active, documentation
    - Key Features: Comprehensive documentation, API reference

16. **kokkos/kokkos.github.io** (3 stars) - Official website
    - URL: https://github.com/kokkos/kokkos.github.io
    - Homepage: https://kokkos.org
    - Description: Source code for kokkos.org pages
    - Status: Active, documentation
    - Key Features: Official website content

17. **kokkos/code-examples** (1 star) - Code snippets
    - URL: https://github.com/kokkos/code-examples
    - Description: Code snippets from academic papers
    - Status: Active, examples
    - Key Features: Research code examples

18. **kokkos/kokkos-miniapps** (12 stars) - Mini applications
    - URL: https://github.com/kokkos/kokkos-miniapps
    - Description: Mini-applications that exclusively use the Kokkos programming model
    - Status: Active, examples
    - Key Features: Representative applications

### Performance and Benchmarking
19. **kokkos/kokkos-benchmark-results** (4 stars) - Performance data
    - URL: https://github.com/kokkos/kokkos-benchmark-results
    - Description: Benchmark results and performance data
    - Status: Active, data
    - Key Features: Performance tracking, benchmarks

### Archived/Legacy Components
20. **kokkos/array_ref** (36 stars) - ARCHIVED
    - URL: https://github.com/kokkos/array_ref
    - Description: Polymorphic multidimensional array view
    - Status: Archived (superseded by mdspan)

21. **kokkos/mdarray** (9 stars) - ARCHIVED
    - URL: https://github.com/kokkos/mdarray
    - Description: Multi-dimensional array implementation
    - Status: Archived (superseded by mdspan)

22. **kokkos/simd-math** (21 stars) - ARCHIVED
    - URL: https://github.com/kokkos/simd-math
    - Description: Length agnostic SIMD intrinsic support
    - Status: Archived

23. **kokkos/nvcc_wrapper** (8 stars) - ARCHIVED
    - URL: https://github.com/kokkos/nvcc_wrapper
    - Description: Wrapper shell script for NVIDIA nvcc
    - Status: Archived (integrated into Kokkos Core)

24. **kokkos/hpcbind** (7 stars) - ARCHIVED
    - URL: https://github.com/kokkos/hpcbind
    - Description: Binding utilities for MPI, OpenMP and GPUs
    - Status: Archived

25. **kokkos/ProgrammingGuide** (3 stars) - ARCHIVED
    - URL: https://github.com/kokkos/ProgrammingGuide
    - Description: Programming guide (LaTeX source)
    - Status: Archived

### Infrastructure and Governance
26. **kokkos/governance** (2 stars) - Project governance
    - URL: https://github.com/kokkos/governance
    - Description: Documents describing the governance of the Kokkos Ecosystem
    - Status: Active, governance
    - Key Features: Project governance documents

27. **kokkos/.github** (0 stars) - GitHub configuration
    - URL: https://github.com/kokkos/.github
    - Description: GitHub organization configuration
    - Status: Active, infrastructure
    - Key Features: Issue templates, organization profile

28. **kokkos/kokkos-docs** (0 stars) - Legacy documentation
    - URL: https://github.com/kokkos/kokkos-docs
    - Description: Documentation and GitHub pages
    - Status: Legacy
    - Key Features: Historical documentation

29. **kokkos/kokkos-openmptarget-examples** (1 star) - OpenMP examples
    - URL: https://github.com/kokkos/kokkos-openmptarget-examples
    - Description: OpenMP 5 target usage examples and reproducers
    - Status: Active, examples
    - Key Features: OpenMP target offload examples

## Key Integration Targets for HurdCog

### Tier 1 (Immediate Integration)
- **kokkos/kokkos** - Core parallel programming model
- **kokkos/kokkos-kernels** - Essential mathematical operations
- **kokkos/mdspan** - Multi-dimensional data structures

### Tier 2 (Phase 2 Integration)  
- **kokkos/kokkos-remote-spaces** - Distributed memory support
- **kokkos/kokkos-tools** - Performance monitoring and debugging
- **kokkos/kokkos-comm** - MPI integration

### Tier 3 (Advanced Features)
- **kokkos/kokkos-fft** - Signal processing for cognitive operations
- **kokkos/kokkos-resilience** - Fault tolerance
- **kokkos/pykokkos** - Python ecosystem integration

## Additional Resources

### Official Documentation
- **Main Website**: https://kokkos.org
- **Wiki Documentation**: https://kokkos.org/kokkos-core-wiki/
- **Getting Started**: https://kokkos.org/kokkos-core-wiki/get-started.html
- **Programming Guide**: https://kokkos.org/kokkos-core-wiki/programmingguide.html

### Community and Support
- **Slack Channel**: https://kokkosteam.slack.com
- **GitHub Issues**: Individual repositories have issue tracking
- **Mailing Lists**: Community support through GitHub discussions

### Performance Data
- **Benchmark Results**: https://github.com/kokkos/kokkos-benchmark-results
- **Performance Studies**: Available in academic papers and documentation

## Repository Statistics Summary
- **Total Repositories**: 29
- **Active Repositories**: 24
- **Archived Repositories**: 5
- **Total Stars**: ~5,500+ across ecosystem
- **Primary Language**: C++ (with Python, Fortran bindings)
- **License**: Apache 2.0 with LLVM Exception (most components)

This ecosystem represents one of the most comprehensive performance portability frameworks available for HPC and parallel computing applications.