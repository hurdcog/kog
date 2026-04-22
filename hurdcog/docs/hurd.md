# GNU Hurd Core System Components

The GNU Hurd consists of a collection of user-space servers that provide operating system services on top of the GNU Mach microkernel. This document provides comprehensive coverage of the core Hurd components and their architecture.

## System Architecture Overview

The Hurd implements a multi-server architecture where each major system service runs as a separate user-space process:

```
┌─────────────────────────────────────────────────────────────┐
│                    User Applications                        │
├─────────────────────────────────────────────────────────────┤
│                        glibc                               │
├─────────────────────────────────────────────────────────────┤
│  ┌─────────────┐ ┌─────────────┐ ┌─────────────┐ ┌─────────┐│
│  │ File System │ │   Process   │ │     Auth    │ │Network  ││
│  │  Servers    │ │   Server    │ │   Server    │ │ Server  ││
│  └─────────────┘ └─────────────┘ └─────────────┘ └─────────┘│
├─────────────────────────────────────────────────────────────┤
│                     GNU Mach                               │
│                   (Microkernel)                            │
└─────────────────────────────────────────────────────────────┘
```

## Core Servers

### Authentication Server (auth)

The authentication server manages user credentials and implements the Hurd's capability-based security model.

#### Responsibilities
- **User Authentication**: Verify user credentials during login
- **Capability Management**: Issue and validate capabilities
- **Group Membership**: Manage user group memberships
- **Credential Delegation**: Allow secure sharing of credentials

#### Interface
```c
// Authentication server interface
typedef mach_port_t auth_t;

// Create new authentication credentials
error_t auth_makeauth(auth_t auth, mach_port_t *ports, mach_msg_type_number_t nports,
                      uid_t *uids, mach_msg_type_number_t nuids,
                      gid_t *gids, mach_msg_type_number_t ngids,
                      auth_t *newauth);

// Authenticate user
error_t auth_user_authenticate(auth_t auth, mach_port_t reply,
                              mach_msg_type_name_t reply_type,
                              auth_t *newauth);
```

#### Implementation Details
- **Credential Storage**: Secure storage of authentication tokens
- **UID/GID Management**: User and group ID tracking
- **Capability Validation**: Verification of access rights
- **Session Management**: User session lifecycle management

### Process Server (proc)

The process server manages process creation, lifecycle, and inter-process relationships.

#### Responsibilities
- **Process Creation**: Handle fork() and exec() operations
- **Process Hierarchy**: Maintain parent-child relationships
- **Resource Management**: Track process resources and limits
- **Signal Delivery**: Coordinate signal delivery between processes

#### Interface
```c
// Process server interface
typedef mach_port_t process_t;

// Register new process
error_t proc_child(process_t proc, task_t child_task);

// Set process arguments
error_t proc_set_arg_locations(process_t proc, vm_address_t argv, vm_address_t envp);

// Handle process exit
error_t proc_mark_exit(process_t proc, int wait_status, int sigcode);
```

#### Key Features
- **Process Table**: Central registry of all processes
- **Parent-Child Tracking**: Process family relationships
- **Resource Limits**: Memory and CPU limit enforcement
- **Zombie Cleanup**: Proper cleanup of terminated processes

### File System Infrastructure

The Hurd's file system is implemented through a collection of translators that can be dynamically attached to any node in the file system tree.

#### Core Libraries

##### libdiskfs
Foundation for disk-based file systems:
```c
// libdiskfs interface
struct diskfs_node {
    struct node node;           // Base node structure
    ino_t dn_stat.st_ino;      // Inode number
    struct diskfs_dirstat dirstat; // Directory state
    // ... additional fields
};

// Core operations
error_t diskfs_lookup(struct node *dir, char *name, 
                     enum lookup_type type, struct node **node,
                     struct dirstat *ds, struct protid *cred);
```

##### libnetfs
Framework for network and synthetic file systems:
```c
// libnetfs interface
struct netfs_node {
    struct node node;           // Base node structure
    void *nn_stat;             // Node-specific data
    struct rwlock nn_stat_lock; // Statistics lock
};

// Network filesystem operations
error_t netfs_attempt_lookup(struct iouser *user, struct node *dir,
                            char *name, struct node **node);
```

##### libtrivfs
Support for simple, single-file translators:
```c
// libtrivfs interface
struct trivfs_control {
    struct port_info pi;        // Port information
    mach_port_t underlying;     // Underlying port
    struct trivfs_protid *protids; // Active protids
};

// Trivial filesystem operations
error_t trivfs_S_io_read(struct trivfs_protid *cred,
                        mach_port_t reply, mach_msg_type_name_t reply_type,
                        char **data, mach_msg_type_number_t *datalen,
                        off_t offset, mach_msg_type_number_t amount);
```

#### File System Servers

##### ext2fs
The primary disk-based file system:
- **POSIX Compliance**: Full POSIX file system semantics
- **Performance**: Optimized for typical workloads
- **Reliability**: Journaling and consistency checking
- **Large File Support**: Support for files larger than 4GB

##### isofs
ISO 9660 CD-ROM file system:
- **Read-Only**: Standard CD-ROM access
- **Rock Ridge**: UNIX-style extensions
- **Joliet**: Windows-compatible extensions
- **Multi-Session**: Support for multi-session discs

##### tmpfs
In-memory temporary file system:
- **RAM Storage**: Files stored entirely in memory
- **Fast Access**: No disk I/O overhead
- **Automatic Cleanup**: Files disappear on reboot
- **Size Limits**: Configurable memory usage limits

### Translator System

Translators are the heart of the Hurd's file system architecture, providing a flexible way to implement file system services.

#### Translator Types

##### Active Translators
Running programs that provide file system services:
```bash
# Example: Start FTP translator
settrans /ftp /hurd/ftpfs ftp.gnu.org

# Example: HTTP filesystem
settrans /http /hurd/httpfs --server=www.gnu.org
```

##### Passive Translators
Static translator specifications stored in the file system:
```bash
# Set passive translator
settrans -p /dev/null /hurd/null

# Show translator setting
showtrans /dev/null
```

#### Translator Interface
```c
// Standard translator interface
error_t trivfs_S_io_write(struct trivfs_protid *cred,
                          mach_port_t reply, mach_msg_type_name_t reply_type,
                          char *data, mach_msg_type_number_t datalen,
                          off_t offset, mach_msg_type_number_t *amount);

error_t trivfs_S_io_seek(struct trivfs_protid *cred,
                         mach_port_t reply, mach_msg_type_name_t reply_type,
                         off_t offset, int whence, off_t *newoffset);
```

#### Built-in Translators

##### /hurd/null
Null device translator:
- **Discard Writes**: All writes are discarded
- **EOF on Read**: Always returns end-of-file
- **High Performance**: Minimal overhead
- **Standard Compliance**: UNIX /dev/null behavior

##### /hurd/zero
Zero device translator:
- **Infinite Zeros**: Reads return zero bytes
- **Discard Writes**: Writes are ignored
- **Memory Testing**: Used for memory operations
- **Standard Compliance**: UNIX /dev/zero behavior

##### /hurd/random
Random number generator:
- **Cryptographic Quality**: Strong random numbers
- **Entropy Pool**: Maintains entropy for randomness
- **Blocking Behavior**: Blocks when entropy is low
- **Non-blocking Mode**: Pseudo-random when requested

### Terminal and Console Support

#### Term Server
Provides terminal services for character-based I/O:
```c
// Terminal interface
error_t term_get_peername(io_t terminal, string_t *name);
error_t term_set_nodelay(io_t terminal, int nodelay);
error_t term_set_bottom_type(io_t terminal, int bottom_type);
```

##### Features
- **Line Discipline**: Terminal line editing and processing
- **Job Control**: Support for process groups and sessions
- **Terminal Attributes**: Configurable terminal behavior
- **Flow Control**: XON/XOFF and hardware flow control

#### Console Server
Manages virtual consoles and display:
- **Multiple Consoles**: Support for multiple virtual terminals
- **Keyboard Input**: Keyboard event processing and delivery
- **Display Output**: Character and graphics output
- **Console Switching**: Hot-key console switching

### Network Infrastructure

#### pfinet
TCP/IP networking server:
```c
// Network interface
error_t S_socket_create(mach_port_t master, int domain, int type, int protocol,
                       mach_port_t *socket);
error_t S_socket_listen(io_t sock, int queue_limit);
error_t S_socket_accept(io_t sock, mach_port_t *new_sock,
                       mach_port_t *addr_port, mach_msg_type_name_t *addr_port_type);
```

##### Capabilities
- **IPv4 Support**: Complete IPv4 implementation
- **IPv6 Support**: IPv6 support (in development)
- **Socket Interface**: Berkeley sockets compatibility
- **Protocol Support**: TCP, UDP, ICMP, and raw sockets

#### pflocal
Local (UNIX domain) socket server:
- **UNIX Sockets**: Local inter-process communication
- **File System Integration**: Socket files in file system
- **Credential Passing**: File descriptor and credential passing
- **High Performance**: Optimized for local communication

### Storage Management

#### libstore
Storage abstraction library:
```c
// Store interface
struct store {
    enum store_class class;     // Store type
    store_offset_t size;        // Store size in bytes
    size_t block_size;          // Block size
    store_offset_t blocks;      // Number of blocks
    // ... additional fields
};

// Store operations
error_t store_read(struct store *store, store_offset_t addr, size_t index,
                  mach_msg_type_number_t amount, void **buf, size_t *len);
```

##### Store Types
- **Device Stores**: Direct hardware device access
- **File Stores**: File-backed storage
- **Memory Stores**: RAM-based storage
- **Network Stores**: Network-attached storage

### System Libraries

#### libports
Port management and server infrastructure:
```c
// Port management
error_t ports_create_port(struct port_class *class, struct port_bucket *bucket,
                         size_t size, void **port);
error_t ports_destroy_right(void *port);

// Server infrastructure
void ports_manage_port_operations_multithread(struct port_bucket *bucket,
                                             ports_demuxer_type demuxer,
                                             int thread_timeout,
                                             int global_timeout,
                                             void (*hook)(void));
```

#### libiohelp
I/O operation helpers:
```c
// I/O helpers
error_t iohelp_create_iouser(struct iouser **user, struct idvec *uids,
                            struct idvec *gids);
error_t iohelp_restrict_iouser(struct iouser **user, struct iouser *from,
                              struct idvec *uids, struct idvec *gids);
```

#### libfshelp
File system operation helpers:
```c
// Filesystem helpers
error_t fshelp_acquire_lock(struct lock_box *box, int *user_lock_status,
                           struct node **node, int flags);
error_t fshelp_start_translator_long(struct fshelp_stat_cookie *cookie,
                                    char *name, char *argz, int argz_len,
                                    int timeout, fshelp_open_fn_t underlying_open_fn);
```

## Inter-Component Communication

### RPC Interface Generation

The Hurd uses MIG (Mach Interface Generator) to create RPC interfaces:
```c
// Example interface definition (.defs file)
subsystem fs 20000;
#include <hurd/hurd_types.defs>

routine file_chown(
    file : file_t;
    owner : uid_t;
    group : gid_t);

routine file_chmod(
    file : file_t;
    mode : mode_t);
```

### Message Passing Patterns
- **Synchronous RPC**: Traditional request-response pattern
- **Asynchronous Notification**: Event notification without blocking
- **Streaming**: Continuous data transfer
- **Multiplexed I/O**: Handling multiple I/O sources efficiently

## Configuration and Management

### Server Startup
```bash
# System server startup sequence
/hurd/init                  # System initialization
/hurd/auth                  # Authentication server
/hurd/proc                  # Process server
/hurd/pfinet               # Network server
/hurd/term                 # Terminal server
```

### Runtime Management
```bash
# Server management commands
servers                     # List running servers
settrans                   # Set translator
showtrans                  # Show translator
ps -M                      # Show Mach task information
```

### Configuration Files
```bash
# System configuration
/etc/fstab                 # File system mount table
/etc/ttys                  # Terminal configuration
/servers/                  # Server namespace
/etc/hurd/                 # Hurd-specific configuration
```

## Development and Debugging

### Server Development
```c
// Minimal server template
#include <hurd.h>
#include <stdio.h>
#include <error.h>
#include <hurd/trivfs.h>

int trivfs_fstype = FSTYPE_MISC;
int trivfs_fsid = 0;
int trivfs_support_read = 1;
int trivfs_support_write = 0;
int trivfs_allow_open = O_READ;

int main(int argc, char **argv) {
    // Server initialization
    task_get_bootstrap_port(mach_task_self(), &bootstrap);
    // ... server-specific initialization
    
    // Start serving requests
    ports_manage_port_operations_multithread(
        trivfs_cntl_portclasses[0],
        trivfs_demuxer,
        30 * 1000, 0, 0);
}
```

### Debugging Tools
- **rpctrace**: Trace RPC calls between components
- **portinfo**: Analyze port usage and relationships
- **vminfo**: Virtual memory usage analysis
- **serverboot**: Server bootstrap analysis

## Performance Considerations

### Optimization Strategies
- **Message Batching**: Combine multiple operations
- **Caching**: Cache frequently accessed data
- **Connection Pooling**: Reuse established connections
- **Lazy Initialization**: Defer expensive operations

### Bottleneck Analysis
- **IPC Overhead**: Message passing costs
- **Context Switching**: Task switching overhead
- **Memory Management**: Page fault and allocation costs
- **Lock Contention**: Synchronization bottlenecks

## Implementation Status in 9nu

### Completed Components
- **Core Servers**: auth, proc, pfinet, pflocal complete
- **File Systems**: ext2fs, isofs, tmpfs functional
- **Translators**: Basic translator framework working
- **Libraries**: All core libraries implemented

### Ongoing Development
- **Performance**: Optimization efforts continuing
- **Stability**: Bug fixes and reliability improvements
- **Features**: New translator types and capabilities
- **Documentation**: Comprehensive documentation project

## Further Reading

- [Microkernel Architecture](microkernel.md)
- [glibc Integration](glibc.md)
- [Translator Development](translator-development.md)
- [System Administration](user.md)

---

*This document provides comprehensive coverage of the GNU Hurd core system components, addressing open documentation issues related to the Hurd architecture and implementation.*