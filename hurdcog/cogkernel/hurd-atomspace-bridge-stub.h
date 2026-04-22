/* HurdCog AtomSpace-Microkernel Bridge Header - Stub Version
 * For testing on non-Hurd systems
 */

#ifndef _HURD_ATOMSPACE_BRIDGE_STUB_H_
#define _HURD_ATOMSPACE_BRIDGE_STUB_H_

#include <sys/time.h>
#include <time.h>

/* Maximum limits for tracked objects */
#define MAX_PORTS 256
#define MAX_SERVERS 64
#define MAX_NAME_LENGTH 256
#define MAX_PATH_LENGTH 1024

/* AtomSpace port representation */
typedef struct {
    char name[MAX_NAME_LENGTH];
    int port;
    int type;
    time_t registered_time;
} atomspace_port_t;

/* AtomSpace server representation */
typedef struct {
    char name[MAX_NAME_LENGTH];
    char path[MAX_PATH_LENGTH];
    int port;
    time_t registered_time;
} atomspace_server_t;

/* Bridge context structure */
typedef struct {
    /* Simulated Mach/Hurd interface */
    int task_port;
    int host_priv_port;
    
    /* AtomSpace objects */
    atomspace_port_t ports[MAX_PORTS];
    atomspace_server_t servers[MAX_SERVERS];
    
    /* Statistics */
    int atom_count;
    int port_count;
    int server_count;
    unsigned long ipc_calls;
    unsigned long errors;
    struct timeval init_time;
} atomspace_bridge_t;

/* Statistics structure */
typedef struct {
    int atom_count;
    int port_count;
    int server_count;
    unsigned long ipc_calls;
    unsigned long errors;
    long uptime_seconds;
} atomspace_stats_t;

/* Bridge lifecycle functions */
int hurd_atomspace_bridge_init(void);
void hurd_atomspace_bridge_shutdown(void);

/* Registration functions */
int hurd_atomspace_register_port(const char *port_name, int port, int port_type);
int hurd_atomspace_register_server(const char *server_name, const char *server_path, int server_port);

/* IPC functions with cognitive routing */
int hurd_atomspace_ipc_send(const char *destination, const void *data, size_t size);

/* Query functions */
int hurd_atomspace_query_objects(const char *object_type, char **results, int max_results);

/* Performance monitoring */
void hurd_atomspace_get_stats(atomspace_stats_t *stats);
void hurd_atomspace_monitor_performance(void);

/* Bootstrap functions */
int hurd_atomspace_bootstrap_core(void);

#endif /* _HURD_ATOMSPACE_BRIDGE_STUB_H_ */