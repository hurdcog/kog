# GNU Mach ASLR Implementation

## Overview

This implementation adds Address Space Layout Randomization (ASLR) and performance-oriented memory region placement to GNU Mach microkernel's virtual memory system.

## Features Implemented

### 1. Address Space Layout Randomization (ASLR)
- **Configurable entropy**: 4-16 bits of randomization (16-65536 possible positions)
- **Adaptive randomization**: Reduces entropy under memory pressure to improve allocation success
- **Multiple placement points**: Randomization applied at empty map allocation, gap-based allocation, and first-free hint
- **Safety bounds**: Entropy limited to 25% of address space to prevent fragmentation

### 2. Performance Optimizations
- **Large page alignment**: Automatically aligns allocations ≥2MB to 2MB boundaries
- **High address preference**: Configurable option to place large allocations in upper address space
- **Memory pressure detection**: Adapts placement strategy based on address space utilization

### 3. Configuration API
- `vm_map_set_aslr(map, enabled, entropy_bits)` - Configure ASLR per map
- `vm_map_get_aslr_entropy(map, size)` - Generate entropy for placement
- `vm_map_optimize_placement(map, size, addr)` - Apply performance optimizations
- `vm_map_memory_pressure(map)` - Detect memory pressure

## Implementation Details

### VM Map Structure Extensions
```c
struct vm_map {
    // ... existing fields ...
    unsigned int aslr_enabled:1;        /* ASLR enabled flag */
    unsigned int prefer_high_addr:1;    /* High address preference */
    unsigned int aslr_entropy_bits;     /* Entropy configuration */
    // ... rest of structure ...
};
```

### Constants
- `VM_MAP_ASLR_DEFAULT_ENTROPY_BITS`: 8 bits (default)
- `VM_MAP_ASLR_MAX_ENTROPY_BITS`: 16 bits (maximum)
- `VM_MAP_ASLR_MIN_ENTROPY_BITS`: 4 bits (minimum)
- `VM_MAP_LARGE_PAGE_SIZE`: 2MB large page size
- `VM_MAP_PREFER_HIGH_THRESHOLD`: 128MB threshold for high address preference

### Placement Strategy
1. **Empty Maps**: Apply ASLR randomization to base address, then optimize for performance
2. **Gap Allocation**: Randomize within available gaps while respecting alignment
3. **First-Free Hint**: Limited randomization in gaps to maintain performance

### Memory Pressure Adaptation
- Monitors address space utilization (>75% = high pressure)
- Reduces ASLR entropy by half under pressure
- Maintains minimum security while improving allocation success

## Security Benefits
- Makes memory layout unpredictable for attackers
- Complicates buffer overflow and ROP attacks
- Provides configurable security/performance tradeoff

## Performance Benefits
- Large page alignment reduces TLB pressure
- High address preference optimizes cache behavior for large allocations
- Adaptive placement maintains performance under memory pressure
- Minimal overhead on existing allocation algorithms

## Testing
- `test-vm-aslr.c` provides comprehensive testing:
  - Basic ASLR functionality verification
  - Entropy bounds validation
  - Large page optimization testing
  - High address preference validation

## Usage Example
```c
vm_map_t map = vm_map_create(pmap, min_addr, max_addr);

/* Enable ASLR with 12 bits of entropy */
vm_map_set_aslr(map, TRUE, 12);

/* Enable high address preference for performance */
map->prefer_high_addr = TRUE;

/* Normal allocation - will use ASLR and optimizations */
vm_map_enter(map, &addr, size, 0, TRUE, object, 0, FALSE, 
             VM_PROT_READ|VM_PROT_WRITE, VM_PROT_ALL, VM_INHERIT_DEFAULT);
```

## Integration Notes
- ASLR is enabled by default for all maps
- Kernel maps can disable ASLR by calling `vm_map_set_aslr(map, FALSE, 0)`
- Existing code unchanged - ASLR is transparent to callers
- Red-black tree structure and gap management preserved
- Backward compatible with existing memory management features

## Future Enhancements
- Hardware random number generator integration
- Per-process ASLR configuration
- Stack/heap specific placement policies
- NUMA-aware placement for multi-processor systems