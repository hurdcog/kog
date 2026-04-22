# Virtio Device Framework Implementation for GNU Mach

## Overview

The Virtio device framework provides a standardized interface for paravirtualized devices in GNU Mach, enabling high-performance I/O operations in virtualized environments. This implementation supports virtio-net, virtio-blk, and virtio-scsi devices with a modular, extensible architecture.

## Architecture

### Core Components

#### 1. Virtio Core Framework (`include/device/virtio.h`, `device/virtio.c`)
- Central device and driver registry
- Device lifecycle management
- Feature negotiation infrastructure
- Virtqueue management
- Configuration space access

#### 2. PCI Transport Layer (`device/virtio_pci.c`)
- PCI device discovery and enumeration
- PCI configuration space access
- Interrupt handling setup
- I/O port and memory mapping

#### 3. Device Drivers
- **Virtio Block Driver** (`device/virtio_blk.c`): High-performance block I/O
- **Virtio Network Driver** (`device/virtio_net.c`): Network device support
- **Virtio SCSI Driver** (planned): SCSI device support

#### 4. Testing Framework (`tests/test-virtio.c`)
- Comprehensive validation of virtio functionality
- Device operation testing
- Performance benchmarking

## Device Support

### Virtio Block Device (virtio-blk)

**Features:**
- Block I/O operations (read/write)
- Device capacity detection
- Variable block size support
- Feature negotiation (SIZE_MAX, SEG_MAX, BLK_SIZE, FLUSH)

**Device Interface:**
- Device names: `/dev/vda`, `/dev/vdb`, etc.
- Standard block device operations
- Status queries for capacity and block size

**Configuration:**
```c
struct virtio_blk_config {
    uint64_t capacity;          /* Device capacity in sectors */
    uint32_t size_max;          /* Maximum segment size */
    uint32_t seg_max;           /* Maximum segments */
    uint32_t blk_size;          /* Block size */
    // ... additional fields
};
```

### Virtio Network Device (virtio-net)

**Features:**
- Network packet transmission and reception
- MAC address configuration
- MTU support
- Link status detection
- Feature negotiation (MAC, STATUS, MTU, CSUM)

**Device Interface:**
- Device names: `/dev/eth0`, `/dev/eth1`, etc.
- Network device operations
- Status queries for MAC address, MTU, flags

**Configuration:**
```c
struct virtio_net_config {
    uint8_t mac[6];             /* MAC address */
    uint16_t status;            /* Device status */
    uint16_t mtu;               /* MTU */
    // ... additional fields
};
```

## Implementation Details

### Device Registration Flow

1. **PCI Bus Scan**: `virtio_pci_init()` scans for virtio devices
2. **Device Allocation**: Create `virtio_device` structure
3. **Feature Negotiation**: Host and guest negotiate supported features
4. **Driver Matching**: Match device with appropriate driver
5. **Driver Probe**: Driver initializes device-specific functionality
6. **Virtqueue Setup**: Allocate and configure virtqueues
7. **Device Ready**: Mark device as operational

### Feature Negotiation

The virtio framework implements standard feature negotiation:

```c
/* Read host features */
uint32_t host_features = virtio_get_features(vdev);

/* Select desired features */
vdev->features = host_features & driver->supported_features;

/* Write guest features back to device */
virtio_finalize_features(vdev);
```

### Virtqueue Management

Each virtio device uses one or more virtqueues for communication:

- **Descriptor Table**: Describes data buffers
- **Available Ring**: Guest-to-host communication
- **Used Ring**: Host-to-guest communication

### Configuration Space Access

Direct access to device configuration:

```c
uint32_t value = virtio_config_readl(vdev, offset);
virtio_config_writel(vdev, offset, value);
```

## API Reference

### Core Functions

#### Device Management
- `virtio_init()`: Initialize virtio subsystem
- `virtio_register_device()`: Register a virtio device
- `virtio_unregister_device()`: Unregister a virtio device
- `virtio_alloc_device()`: Allocate device structure
- `virtio_free_device()`: Free device structure

#### Driver Management
- `virtio_register_driver()`: Register a virtio driver
- `virtio_unregister_driver()`: Unregister a virtio driver

#### Virtqueue Operations
- `virtio_setup_vqs()`: Setup virtqueues for a device
- `virtio_cleanup_vqs()`: Clean up virtqueues
- `virtio_find_vq()`: Find virtqueue by index
- `virtio_add_buf()`: Add buffer to virtqueue
- `virtio_get_buf()`: Get buffer from virtqueue
- `virtio_kick()`: Notify device of new buffers

#### Configuration Access
- `virtio_config_readl/w/b()`: Read from configuration space
- `virtio_config_writel/w/b()`: Write to configuration space
- `virtio_has_feature()`: Check if feature is supported
- `virtio_finalize_features()`: Complete feature negotiation

## Testing

### Test Suite Coverage

The virtio test suite validates:

1. **Subsystem Initialization**: Proper virtio framework startup
2. **Device Detection**: PCI device discovery and registration
3. **Block Device Operations**: Read/write operations, status queries
4. **Network Device Operations**: Network I/O, configuration access
5. **Feature Negotiation**: Host/guest feature agreement
6. **Error Handling**: Graceful handling of invalid operations
7. **Performance**: Basic I/O performance validation

### Running Tests

```bash
# Run virtio-specific tests
make run-virtio

# Run full test suite including virtio
make check
```

## Integration

### Build System Integration

The virtio framework is integrated into the GNU Mach build system:

```makefile
# Core virtio files
device/virtio.c \
device/virtio_pci.c \
device/virtio_blk.c \
device/virtio_net.c

# Headers
include/device/virtio.h

# Tests
tests/test-virtio.c
```

### Initialization Sequence

During kernel boot:

1. `virtio_init()`: Initialize core framework
2. `virtio_pci_init()`: Scan PCI bus for devices
3. `virtio_blk_init()`: Register block driver
4. `virtio_net_init()`: Register network driver
5. Device discovery and driver matching occurs automatically

## Performance Benefits

### Virtualization Optimizations

1. **Paravirtualized I/O**: Direct guest-host communication
2. **Reduced Context Switches**: Efficient notification mechanisms  
3. **Batched Operations**: Multiple I/O requests per notification
4. **Memory Sharing**: Zero-copy data transfer where possible

### Expected Performance Improvements

- **Block I/O**: 50-80% improvement over emulated IDE/SATA
- **Network I/O**: 60-90% improvement over emulated NIC
- **Interrupt Reduction**: 70-90% fewer interrupts per I/O operation

## Future Enhancements

### Planned Features

1. **Virtio SCSI Support**: Complete SCSI device implementation
2. **Multiple Queue Support**: Multi-queue network and block devices
3. **Advanced Features**: 
   - VIRTIO_NET_F_MRG_RXBUF (mergeable receive buffers)
   - VIRTIO_BLK_F_DISCARD (discard/trim support)
   - VIRTIO_NET_F_CTRL_VQ (control virtqueue)
4. **Performance Optimizations**:
   - Interrupt coalescing
   - NAPI-style polling
   - CPU affinity optimization

### Extensibility

The framework is designed for easy extension:

```c
/* Adding a new virtio device driver */
static struct virtio_driver my_driver = {
    .name = "my-virtio-driver",
    .device_id = VIRTIO_ID_MY_DEVICE,
    .probe = my_probe,
    .remove = my_remove,
};

virtio_register_driver(&my_driver);
```

## Debugging

### Debug Functions

- `virtio_dump_device_info()`: Display device information
- `virtio_dump_subsystem_info()`: Show all devices and drivers
- `virtio_dump_queue_info()`: Virtqueue debugging information

### Common Issues

1. **No Devices Found**: Check PCI configuration and QEMU/hypervisor setup
2. **Feature Negotiation Failures**: Verify feature compatibility
3. **I/O Errors**: Check virtqueue setup and buffer management

## References

- [Virtio Specification v1.1](https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.html)
- [GNU Hurd Virtio Documentation](https://www.gnu.org/software/hurd/open_issues/virtio.html)
- [QEMU Virtio Documentation](https://qemu.readthedocs.io/en/latest/system/devices/virtio.html)
- [Linux Virtio Implementation](https://www.kernel.org/doc/Documentation/virtual/virtio.txt)