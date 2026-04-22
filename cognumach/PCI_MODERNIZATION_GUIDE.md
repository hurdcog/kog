# GNU Mach PCI Modernization Guide

This document describes the PCI/PCIe modernization features implemented in GNU Mach.

## Overview

The GNU Mach PCI subsystem has been enhanced to support modern PCIe devices while maintaining full backward compatibility with legacy PCI devices. The implementation includes capability detection, resource management, and foundations for PCIe advanced features.

## New Features

### 1. PCIe Capability Detection

The system now automatically detects PCIe capabilities during device enumeration:

```c
/* Check if device is PCIe */
if (dev->is_pcie) {
    printf("PCIe device found, type: %d\n", dev->pcie_type);
    printf("PCIe capability at offset: 0x%02x\n", dev->pcie_cap);
}

/* Find specific capabilities */
int msi_cap = pci_find_capability(dev, PCI_CAP_ID_MSI);
int pcie_cap = pci_find_capability(dev, PCI_CAP_ID_EXP);
```

### 2. Enhanced Resource Management

Modern resource allocation and management with conflict detection:

```c
/* Request device regions */
if (pci_request_regions(pdev, "my-driver") == 0) {
    /* Use device resources */
    
    /* Release when done */
    pci_release_regions(pdev);
}

/* Manual resource assignment */
pci_assign_resource(dev, 0);  /* Assign BAR 0 */
```

### 3. Device Resource Setup

Resources are automatically enumerated during device detection:

```c
/* Resources are automatically set up in scan_bus() */
for (int i = 0; i < PCI_NUM_RESOURCES; i++) {
    if (dev->resource[i].flags & PCI_IORESOURCE_MEM) {
        printf("Memory BAR %d: 0x%08lx-0x%08lx\n", 
               i, dev->resource[i].start, dev->resource[i].end);
    } else if (dev->resource[i].flags & PCI_IORESOURCE_IO) {
        printf("I/O BAR %d: 0x%08lx-0x%08lx\n", 
               i, dev->resource[i].start, dev->resource[i].end);
    }
}
```

### 4. PCIe Configuration Space Access

Foundation for extended configuration space access:

```c
/* Standard config space (0-255) */
u16 value;
pcie_capability_read_word(dev, PCI_EXP_DEVCTL, &value);

/* Extended config space (256-4095) - Future enhancement */
u32 ext_value;
pci_read_config_dword_ext(dev, 0x100, &ext_value);
```

## Device Types Supported

The implementation recognizes the following PCIe device types:

- `PCI_EXP_TYPE_ENDPOINT` (0x0) - PCIe Endpoint
- `PCI_EXP_TYPE_LEG_END` (0x1) - Legacy Endpoint  
- `PCI_EXP_TYPE_ROOT_PORT` (0x4) - Root Port
- `PCI_EXP_TYPE_UPSTREAM` (0x5) - Upstream Port
- `PCI_EXP_TYPE_DOWNSTREAM` (0x6) - Downstream Port
- `PCI_EXP_TYPE_PCI_BRIDGE` (0x7) - PCI/PCI-X Bridge
- `PCI_EXP_TYPE_PCIE_BRIDGE` (0x8) - PCI/PCI-X to PCIe Bridge

## Capabilities Supported

The system can detect and handle these PCI capabilities:

- Power Management (`PCI_CAP_ID_PM`)
- Message Signaled Interrupts (`PCI_CAP_ID_MSI`, `PCI_CAP_ID_MSIX`)
- PCI Express (`PCI_CAP_ID_EXP`)
- PCI-X (`PCI_CAP_ID_PCIX`)
- Vendor Specific (`PCI_CAP_ID_VNDR`)
- Advanced Features (`PCI_CAP_ID_AF`)

## Resource Flags

Resource management uses these flags:

- `PCI_IORESOURCE_IO` - I/O space resource
- `PCI_IORESOURCE_MEM` - Memory space resource  
- `PCI_IORESOURCE_PREFETCH` - Prefetchable memory
- `PCI_IORESOURCE_BUSY` - Resource in use
- `PCI_IORESOURCE_UNSET` - Resource not assigned

## Backward Compatibility

All existing PCI device drivers continue to work without modification. The new features are additive and optional.

Legacy PCI devices:
- Are detected normally
- Have `is_pcie` flag set to 0
- Use standard configuration space only
- Resource management works identically

## Implementation Status

### Completed ✅
- PCIe capability detection
- Resource management infrastructure  
- Enhanced device scanning
- Multiple capability enumeration
- Device type identification
- Resource allocation/deallocation

### Future Enhancements 🔄
- ECAM (Enhanced Configuration Access Mechanism)
- PCIe error handling
- Advanced power management
- Slot management for hot-plug
- Performance monitoring

## Testing

Comprehensive test suite validates all functionality:

```bash
cd /tmp
./test_pci_comprehensive
```

The test covers:
- Multiple capability detection
- Resource setup and enumeration  
- Resource allocation and conflict detection
- Resource release and cleanup
- Device type identification

## Integration

The modernization is integrated into the existing PCI subsystem:

1. **Header File**: `linux/src/include/linux/pci.h`
   - New constants and structures
   - Function prototypes

2. **Implementation**: `linux/src/drivers/pci/pci.c` 
   - Capability detection functions
   - Resource management functions
   - Enhanced device scanning

3. **Device Structures**: Enhanced `pci_dev` structure
   - PCIe identification flags
   - Capability offset storage
   - Resource array for BAR management

The implementation maintains the existing GNU Mach architecture and coding style while adding modern PCIe support.