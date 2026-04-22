# User-space Driver Examples

This directory contains example implementations of user-space device drivers using the GNU Mach User-space Driver Framework.

## Examples

### null-driver.c

A simple null device driver that demonstrates the basic structure and API usage of the user-space driver framework.

**Features:**
- Character device implementation
- Resource limit configuration
- Proper device lifecycle management
- Logging and error handling
- Device statistics tracking

**Functionality:**
- **Write operations**: Discards all data (like `/dev/null`)
- **Read operations**: Always returns EOF (0 bytes)
- **Status queries**: Returns device statistics

**Usage:**
```bash
# Compile the driver
gcc -o null-driver null-driver.c -I../include -lmach -pthread

# Run the driver
./null-driver

# Test the driver (in another terminal)
echo "test data" > /dev/null  # Should discard data
cat /dev/null                 # Should return EOF
```

## Driver Development Template

The null driver serves as a template for developing other user-space drivers. Key components include:

1. **Device Context Structure**: Maintains per-device state
2. **Callback Functions**: Handle device operations (open, close, read, write, status)
3. **Driver Registration**: Register with kernel and set resource limits
4. **Message Loop**: Handle incoming requests from kernel
5. **Cleanup**: Properly unregister and cleanup resources

## Key API Functions Demonstrated

- `usrd_driver_create()` - Create driver instance
- `usrd_driver_register()` - Register with kernel
- `usrd_driver_add_device()` - Add device to driver
- `usrd_driver_run()` - Start message processing loop
- `usrd_malloc()/usrd_free()` - Resource-tracked memory allocation
- `usrd_log()` - Driver logging facility

## Resource Management

Example resource limit configuration:
```c
USRD_SET_MEMORY_LIMIT(driver, 512);   /* 512KB memory limit */
USRD_SET_IO_LIMIT(driver, 16);        /* Max 16 concurrent I/O ops */
usrd_driver_set_resource_limits(driver, USRD_RESOURCE_TIMEOUT, 10000);
```

## Callback Implementation Pattern

```c
static kern_return_t device_operation(usrd_driver_t driver, 
                                     void *device_context, ...)
{
    struct my_device_context *ctx = (struct my_device_context *)device_context;
    
    /* Validate parameters */
    if (!ctx) return KERN_INVALID_ARGUMENT;
    
    /* Log operation */
    usrd_log(driver, USRD_LOG_DEBUG, "Operation on device '%s'", 
             ctx->device_name);
    
    /* Perform device-specific operation */
    // ... implementation ...
    
    /* Update statistics */
    ctx->operation_count++;
    
    return KERN_SUCCESS;
}
```

## Error Handling

The framework provides comprehensive error handling:
- Return appropriate `kern_return_t` values
- Use `usrd_log()` for error reporting
- Clean up resources on failure
- Validate all input parameters

## Future Examples

Additional examples that could be added:
- **memory-driver.c**: RAM disk implementation
- **serial-driver.c**: Serial port driver
- **network-driver.c**: Network interface driver
- **block-driver.c**: Block device driver
- **input-driver.c**: Input device driver

Each example would demonstrate different aspects of the framework and different device types.