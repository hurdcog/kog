# User-space Device Driver Framework

## Overview

The GNU Mach User-space Device Driver Framework allows device drivers to run in user-space instead of kernel-space, providing better isolation, security, and stability. This framework is designed to maintain the microkernel philosophy while enabling modern driver development practices.

## Architecture

### Components

1. **Kernel-side Driver Registry** (`device/userspace_driver.c`)
   - Manages registration and lifecycle of user-space drivers
   - Provides driver proxy objects that handle kernel-userspace communication
   - Enforces resource limits and security policies

2. **MIG Interface** (`include/device/userspace_driver.defs`)
   - Defines the IPC protocol between kernel and user-space drivers
   - Handles driver registration, device operations, and status updates
   - Provides asynchronous operation support

3. **Driver SDK** (`include/device/userspace_driver_sdk.h`)
   - Complete API for developing user-space drivers
   - Provides callback-based architecture for driver operations
   - Includes resource management and debugging utilities

4. **Testing Framework** (`tests/test-userspace-driver.c`)
   - Comprehensive validation of framework components
   - Tests driver registration, isolation, and communication protocols

## Key Features

### Security and Isolation

- **Sandboxed Execution**: Drivers run in isolated address spaces
- **Resource Limits**: Configurable limits for memory, CPU, I/O operations
- **Validation Cookies**: Corruption detection for driver data structures
- **Fault Containment**: Driver crashes don't affect kernel stability

### Resource Management

```c
// Resource types managed by the framework
#define USRD_RESOURCE_MEMORY    0   /* Memory usage limit (KB) */
#define USRD_RESOURCE_CPU       1   /* CPU usage limit (percentage) */
#define USRD_RESOURCE_IO_OPS    2   /* Max concurrent I/O operations */
#define USRD_RESOURCE_INTERRUPTS 3  /* Max interrupt rate (per second) */
#define USRD_RESOURCE_TIMEOUT   4   /* Operation timeout (milliseconds) */
#define USRD_RESOURCE_BANDWIDTH 5   /* Bandwidth limit (KB/s) */
```

### Driver Capabilities

Drivers declare their capabilities during registration:

```c
#define USRD_CAP_BLOCK_DEVICE   0x00000001  /* Block device driver */
#define USRD_CAP_CHAR_DEVICE    0x00000002  /* Character device driver */
#define USRD_CAP_NETWORK        0x00000004  /* Network device driver */
#define USRD_CAP_INTERRUPT      0x00000200  /* Interrupt handling capability */
#define USRD_CAP_DMA            0x00000400  /* DMA capability */
```

## Usage

### Driver Development

1. **Create Driver Instance**
```c
usrd_driver_t driver = usrd_driver_create("my-driver", "My Driver Description",
                                          1, 0, USRD_CAP_CHAR_DEVICE, &callbacks);
```

2. **Set Resource Limits**
```c
USRD_SET_MEMORY_LIMIT(driver, 1024);  // 1MB memory limit
USRD_SET_IO_LIMIT(driver, 10);        // Max 10 concurrent I/O ops
```

3. **Register with Kernel**
```c
if (usrd_driver_register(driver, mach_task_self()) != KERN_SUCCESS) {
    fprintf(stderr, "Failed to register driver\n");
    return 1;
}
```

4. **Run Driver**
```c
usrd_driver_run(driver);  // Starts message loop
```

### Callback Implementation

Drivers implement callbacks for device operations:

```c
struct usrd_driver_callbacks callbacks = {
    .driver_init = my_driver_init,
    .driver_shutdown = my_driver_shutdown,
    .device_open = my_device_open,
    .device_close = my_device_close,
    .device_read = my_device_read,
    .device_write = my_device_write,
};
```

### Example Driver Skeleton

```c
static kern_return_t my_device_open(usrd_driver_t driver, const char *device_name,
                                   dev_mode_t mode, void **device_context)
{
    struct my_device_context *ctx = malloc(sizeof(struct my_device_context));
    if (!ctx) return KERN_RESOURCE_SHORTAGE;
    
    strncpy(ctx->device_name, device_name, sizeof(ctx->device_name) - 1);
    ctx->is_open = TRUE;
    
    *device_context = ctx;
    return KERN_SUCCESS;
}

static kern_return_t my_device_read(usrd_driver_t driver, void *device_context,
                                   dev_mode_t mode, recnum_t recnum,
                                   vm_size_t bytes_wanted, void **data,
                                   vm_size_t *bytes_read)
{
    struct my_device_context *ctx = (struct my_device_context *)device_context;
    
    // Implement device-specific read logic
    *data = usrd_malloc(driver, bytes_wanted);
    if (!*data) return KERN_RESOURCE_SHORTAGE;
    
    // Fill data with device output
    *bytes_read = bytes_wanted;
    return KERN_SUCCESS;
}
```

## Framework Operations

### Driver Registration Protocol

1. **Driver Registration Request**
   - Driver sends registration request with metadata
   - Kernel validates capabilities and resource limits
   - Registration token returned on success

2. **Device Proxy Creation**
   - Kernel creates proxy objects for each device
   - Proxies handle operation forwarding to user-space
   - Device ports created for user-space communication

3. **Operation Forwarding**
   - Kernel receives device operations from clients
   - Operations forwarded to appropriate user-space driver
   - Results returned through reply ports

### Resource Monitoring

The framework continuously monitors driver resource usage:

- **Memory Usage**: Tracked through SDK allocation functions
- **CPU Usage**: Monitored via task statistics
- **I/O Operations**: Counted during operation forwarding
- **Error Rates**: Tracked for health monitoring

### Health Monitoring

Drivers report status through heartbeat messages:

```c
typedef enum {
    USRD_STATUS_RUNNING     = 0x00000001,
    USRD_STATUS_IDLE        = 0x00000002,
    USRD_STATUS_BUSY        = 0x00000004,
    USRD_STATUS_ERROR       = 0x00000008,
    USRD_STATUS_SUSPENDED   = 0x00000010,
    USRD_STATUS_RECOVERING  = 0x00000020,
} driver_status_t;
```

## Testing

The framework includes comprehensive tests:

```bash
# Run user-space driver framework tests
make run-test-userspace-driver
```

Test coverage includes:
- Driver registration and unregistration
- Resource limit enforcement
- Security isolation verification  
- Communication protocol validation
- SDK functionality testing

## Integration

### Build System Integration

The framework is integrated into the GNU Mach build system:

```makefile
# Added to Makefrag.am
device/userspace_driver.c \
device/userspace_driver.h \
device/userspace_driver_sdk.c

include/device/userspace_driver.defs \
include/device/userspace_driver_types.h \
include/device/userspace_driver_sdk.h
```

### MIG Interface Generation

The framework uses MIG to generate IPC stubs:

```bash
# Generates client and server stubs
mig include/device/userspace_driver.defs
```

## Security Considerations

1. **Driver Isolation**: Each driver runs in its own address space
2. **Resource Limits**: Configurable limits prevent resource exhaustion
3. **Validation**: Data structures protected with validation cookies
4. **Fault Tolerance**: Driver crashes isolated from kernel and other drivers
5. **Capability System**: Drivers declare required capabilities upfront

## Future Enhancements

1. **Hot-pluggable Drivers**: Dynamic loading/unloading of drivers
2. **Driver Migration**: Moving drivers between protection domains
3. **Advanced Security**: Integration with capability-based security
4. **Performance Optimization**: Optimized IPC for high-throughput devices
5. **Driver Composition**: Combining multiple drivers for complex devices

## References

- [GNU Hurd User-space Device Drivers](https://www.gnu.org/software/hurd/open_issues/user-space_device_drivers.html)
- [Mach IPC Documentation](https://www.gnu.org/software/hurd/microkernel/mach/ipc.html)
- [GNU Mach Device Interface](docs/device-interface.md)