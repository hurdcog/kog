/*
 * Cognitive 9P Filesystem Portable Implementation
 * 
 * Cross-platform implementation of the cognitive filesystem that exposes
 * cognitive resources through a 9P-like interface. This allows cognitive
 * operations to be performed using standard file operations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define LOCK_TYPE CRITICAL_SECTION
#define LOCK_INIT(l) InitializeCriticalSection(&(l))
#define LOCK_ACQUIRE(l) EnterCriticalSection(&(l))
#define LOCK_RELEASE(l) LeaveCriticalSection(&(l))
#define LOCK_DESTROY(l) DeleteCriticalSection(&(l))
#else
#include <pthread.h>
#define LOCK_TYPE pthread_mutex_t
#define LOCK_INIT(l) pthread_mutex_init(&(l), NULL)
#define LOCK_ACQUIRE(l) pthread_mutex_lock(&(l))
#define LOCK_RELEASE(l) pthread_mutex_unlock(&(l))
#define LOCK_DESTROY(l) pthread_mutex_destroy(&(l))
#endif

/* File types */
#define COGFS_DIR  1
#define COGFS_FILE 2

/* File modes */
#define COGFS_READ  0x01
#define COGFS_WRITE 0x02
#define COGFS_EXEC  0x04

/* Cognitive filesystem node */
typedef struct CogFsNode {
    uint64_t id;
    int type;
    char *name;
    uint32_t mode;
    uint32_t atime;
    uint32_t mtime;
    uint64_t length;
    void *data;
    struct CogFsNode *parent;
    struct CogFsNode *children;
    struct CogFsNode *next;
} CogFsNode;

/* Cognitive filesystem */
typedef struct CogFs {
    CogFsNode *root;
    uint64_t next_id;
    LOCK_TYPE lock;
} CogFs;

/* Global cognitive filesystem */
static CogFs *global_cogfs = NULL;

/* Forward declarations */
static CogFsNode* cogfs_create_dir(const char *name, CogFsNode *parent);
static CogFsNode* cogfs_create_file(const char *name, CogFsNode *parent, uint32_t mode);
static void cogfs_init_tree(void);

/*
 * Get current time in seconds
 */
static uint32_t get_time(void)
{
    return (uint32_t)time(NULL);
}

/*
 * Create a new cognitive filesystem
 */
CogFs* cogfs_create(void)
{
    CogFs *fs;
    
    fs = (CogFs*)calloc(1, sizeof(CogFs));
    if (fs == NULL) {
        return NULL;
    }
    
    fs->root = NULL;
    fs->next_id = 1;
    LOCK_INIT(fs->lock);
    
    return fs;
}

/*
 * Destroy a cognitive filesystem
 */
static void cogfs_destroy_node(CogFsNode *node)
{
    CogFsNode *child, *next;
    
    if (node == NULL) {
        return;
    }
    
    /* Recursively destroy children */
    child = node->children;
    while (child != NULL) {
        next = child->next;
        cogfs_destroy_node(child);
        child = next;
    }
    
    free(node->name);
    free(node->data);
    free(node);
}

void cogfs_destroy(CogFs *fs)
{
    if (fs == NULL) {
        return;
    }
    
    cogfs_destroy_node(fs->root);
    LOCK_DESTROY(fs->lock);
    free(fs);
}

/*
 * Create a directory node
 */
static CogFsNode* cogfs_create_dir(const char *name, CogFsNode *parent)
{
    CogFsNode *dir;
    
    dir = (CogFsNode*)calloc(1, sizeof(CogFsNode));
    if (dir == NULL) {
        return NULL;
    }
    
    LOCK_ACQUIRE(global_cogfs->lock);
    
    dir->id = global_cogfs->next_id++;
    dir->type = COGFS_DIR;
    dir->name = strdup(name);
    dir->mode = COGFS_READ | COGFS_EXEC;
    dir->atime = get_time();
    dir->mtime = get_time();
    dir->length = 0;
    dir->data = NULL;
    dir->parent = parent;
    dir->children = NULL;
    dir->next = NULL;
    
    /* Add to parent's children list */
    if (parent != NULL) {
        dir->next = parent->children;
        parent->children = dir;
    }
    
    LOCK_RELEASE(global_cogfs->lock);
    
    return dir;
}

/*
 * Create a file node
 */
static CogFsNode* cogfs_create_file(const char *name, CogFsNode *parent, uint32_t mode)
{
    CogFsNode *file;
    
    file = (CogFsNode*)calloc(1, sizeof(CogFsNode));
    if (file == NULL) {
        return NULL;
    }
    
    LOCK_ACQUIRE(global_cogfs->lock);
    
    file->id = global_cogfs->next_id++;
    file->type = COGFS_FILE;
    file->name = strdup(name);
    file->mode = mode;
    file->atime = get_time();
    file->mtime = get_time();
    file->length = 0;
    file->data = NULL;
    file->parent = parent;
    file->children = NULL;
    file->next = NULL;
    
    /* Add to parent's children list */
    if (parent != NULL) {
        file->next = parent->children;
        parent->children = file;
    }
    
    LOCK_RELEASE(global_cogfs->lock);
    
    return file;
}

/*
 * Initialize the cognitive filesystem tree
 */
static void cogfs_init_tree(void)
{
    CogFsNode *atomspace, *reasoning, *memory, *attention, *perception, *action, *learning;
    CogFsNode *pln, *ure, *moses;
    CogFsNode *working, *episodic, *semantic, *procedural;
    
    /* Create root */
    global_cogfs->root = cogfs_create_dir("cognitive", NULL);
    
    /* Create main directories */
    atomspace = cogfs_create_dir("atomspace", global_cogfs->root);
    reasoning = cogfs_create_dir("reasoning", global_cogfs->root);
    memory = cogfs_create_dir("memory", global_cogfs->root);
    attention = cogfs_create_dir("attention", global_cogfs->root);
    perception = cogfs_create_dir("perception", global_cogfs->root);
    action = cogfs_create_dir("action", global_cogfs->root);
    learning = cogfs_create_dir("learning", global_cogfs->root);
    
    /* AtomSpace subdirectories */
    cogfs_create_dir("atoms", atomspace);
    cogfs_create_dir("links", atomspace);
    cogfs_create_dir("queries", atomspace);
    cogfs_create_dir("patterns", atomspace);
    cogfs_create_file("stats", atomspace, COGFS_READ);
    cogfs_create_file("count", atomspace, COGFS_READ);
    
    /* Reasoning subdirectories */
    pln = cogfs_create_dir("pln", reasoning);
    ure = cogfs_create_dir("ure", reasoning);
    moses = cogfs_create_dir("moses", reasoning);
    
    cogfs_create_dir("rules", pln);
    cogfs_create_dir("proofs", pln);
    cogfs_create_dir("beliefs", pln);
    
    cogfs_create_dir("forward", ure);
    cogfs_create_dir("backward", ure);
    
    cogfs_create_dir("populations", moses);
    cogfs_create_dir("fitness", moses);
    cogfs_create_dir("best", moses);
    
    /* Memory subdirectories */
    working = cogfs_create_dir("working", memory);
    episodic = cogfs_create_dir("episodic", memory);
    semantic = cogfs_create_dir("semantic", memory);
    procedural = cogfs_create_dir("procedural", memory);
    
    cogfs_create_file("capacity", working, COGFS_READ | COGFS_WRITE);
    cogfs_create_file("contents", working, COGFS_READ);
    
    /* Attention subdirectories */
    cogfs_create_dir("focus", attention);
    cogfs_create_dir("importance", attention);
    cogfs_create_dir("urgency", attention);
    cogfs_create_file("allocation", attention, COGFS_READ);
    cogfs_create_file("sti_funds", attention, COGFS_READ | COGFS_WRITE);
    cogfs_create_file("lti_funds", attention, COGFS_READ | COGFS_WRITE);
    
    /* Perception subdirectories */
    cogfs_create_dir("vision", perception);
    cogfs_create_dir("audio", perception);
    cogfs_create_dir("text", perception);
    cogfs_create_dir("sensors", perception);
    
    /* Action subdirectories */
    cogfs_create_dir("motor", action);
    cogfs_create_dir("speech", action);
    cogfs_create_dir("commands", action);
    
    /* Learning subdirectories */
    cogfs_create_dir("supervised", learning);
    cogfs_create_dir("unsupervised", learning);
    cogfs_create_dir("reinforcement", learning);
    cogfs_create_dir("meta", learning);
}

/*
 * Initialize the Cognitive 9P filesystem
 */
void cognitive9p_init(void)
{
    printf("Cognitive9P: Initializing portable filesystem\n");
    
    global_cogfs = cogfs_create();
    if (global_cogfs == NULL) {
        fprintf(stderr, "Cognitive9P: Failed to create filesystem\n");
        return;
    }
    
    /* Build cognitive filesystem tree */
    cogfs_init_tree();
    
    printf("Cognitive9P: Portable filesystem initialized\n");
}

/*
 * Shutdown the Cognitive 9P filesystem
 */
void cognitive9p_shutdown(void)
{
    printf("Cognitive9P: Shutting down portable filesystem\n");
    
    if (global_cogfs != NULL) {
        cogfs_destroy(global_cogfs);
        global_cogfs = NULL;
    }
    
    printf("Cognitive9P: Portable filesystem shutdown complete\n");
}

/*
 * Lookup a node by path
 */
CogFsNode* cogfs_lookup(const char *path)
{
    CogFsNode *node, *child;
    char *path_copy, *token, *saveptr;
    
    if (global_cogfs == NULL || path == NULL) {
        return NULL;
    }
    
    /* Handle root path */
    if (strcmp(path, "/") == 0 || strcmp(path, "/cognitive") == 0) {
        return global_cogfs->root;
    }
    
    /* Skip leading slash and "cognitive/" */
    path_copy = strdup(path);
    if (path_copy[0] == '/') {
        token = strtok_r(path_copy + 1, "/", &saveptr);
    } else {
        token = strtok_r(path_copy, "/", &saveptr);
    }
    
    /* Skip "cognitive" if present */
    if (token != NULL && strcmp(token, "cognitive") == 0) {
        token = strtok_r(NULL, "/", &saveptr);
    }
    
    node = global_cogfs->root;
    
    while (token != NULL && node != NULL) {
        /* Find child with matching name */
        child = node->children;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                node = child;
                break;
            }
            child = child->next;
        }
        
        if (child == NULL) {
            free(path_copy);
            return NULL;
        }
        
        token = strtok_r(NULL, "/", &saveptr);
    }
    
    free(path_copy);
    return node;
}

/*
 * Read from a cognitive file
 */
long cogfs_read(const char *path, void *buf, long n, long offset)
{
    CogFsNode *node;
    char *content = NULL;
    long len;
    
    node = cogfs_lookup(path);
    if (node == NULL || node->type != COGFS_FILE) {
        return -1;
    }
    
    /* Generate dynamic content based on file path */
    if (strcmp(node->name, "stats") == 0) {
        content = (char*)malloc(256);
        snprintf(content, 256, 
                 "AtomSpace Statistics\n"
                 "Total Atoms: 0\n"
                 "Nodes: 0\n"
                 "Links: 0\n");
    } else if (strcmp(node->name, "count") == 0) {
        content = (char*)malloc(32);
        snprintf(content, 32, "0\n");
    } else if (strcmp(node->name, "allocation") == 0) {
        content = (char*)malloc(256);
        snprintf(content, 256,
                 "Attention Allocation\n"
                 "STI Funds: 10000\n"
                 "LTI Funds: 10000\n");
    } else if (strcmp(node->name, "sti_funds") == 0) {
        content = (char*)malloc(32);
        snprintf(content, 32, "10000\n");
    } else if (strcmp(node->name, "lti_funds") == 0) {
        content = (char*)malloc(32);
        snprintf(content, 32, "10000\n");
    } else {
        content = (char*)malloc(128);
        snprintf(content, 128, "Cognitive resource: %s\n", node->name);
    }
    
    if (content == NULL) {
        return -1;
    }
    
    len = strlen(content);
    
    /* Handle offset */
    if (offset >= len) {
        free(content);
        return 0;
    }
    
    /* Copy content to buffer */
    if (offset + n > len) {
        n = len - offset;
    }
    
    memcpy(buf, content + offset, n);
    free(content);
    
    /* Update access time */
    node->atime = get_time();
    
    return n;
}

/*
 * Write to a cognitive file
 */
long cogfs_write(const char *path, const void *buf, long n, long offset)
{
    CogFsNode *node;
    char *data;
    
    node = cogfs_lookup(path);
    if (node == NULL || node->type != COGFS_FILE) {
        return -1;
    }
    
    if (!(node->mode & COGFS_WRITE)) {
        return -1;  /* Read-only file */
    }
    
    /* Allocate buffer for data */
    data = (char*)malloc(n + 1);
    if (data == NULL) {
        return -1;
    }
    
    memcpy(data, buf, n);
    data[n] = '\0';
    
    /* Process write based on file type */
    if (strcmp(node->name, "capacity") == 0) {
        printf("Cognitive9P: Setting working memory capacity: %s\n", data);
    } else if (strcmp(node->name, "sti_funds") == 0) {
        printf("Cognitive9P: Setting STI funds: %s\n", data);
    } else if (strcmp(node->name, "lti_funds") == 0) {
        printf("Cognitive9P: Setting LTI funds: %s\n", data);
    } else {
        printf("Cognitive9P: Write to %s: %s\n", node->name, data);
    }
    
    free(data);
    
    /* Update modification time */
    node->mtime = get_time();
    
    return n;
}

/*
 * List directory contents
 */
int cogfs_readdir(const char *path, char **names, int max_names)
{
    CogFsNode *node, *child;
    int count = 0;
    
    node = cogfs_lookup(path);
    if (node == NULL || node->type != COGFS_DIR) {
        return -1;
    }
    
    child = node->children;
    while (child != NULL && count < max_names) {
        names[count] = strdup(child->name);
        count++;
        child = child->next;
    }
    
    return count;
}

/*
 * Print cognitive filesystem tree (for debugging)
 */
static void print_tree_recursive(CogFsNode *node, int depth)
{
    CogFsNode *child;
    int i;
    
    if (node == NULL) {
        return;
    }
    
    /* Print indentation */
    for (i = 0; i < depth; i++) {
        printf("  ");
    }
    
    /* Print node */
    if (node->type == COGFS_DIR) {
        printf("[DIR]  %s/\n", node->name);
    } else {
        printf("[FILE] %s\n", node->name);
    }
    
    /* Print children */
    child = node->children;
    while (child != NULL) {
        print_tree_recursive(child, depth + 1);
        child = child->next;
    }
}

void cogfs_print_tree(void)
{
    if (global_cogfs == NULL) {
        return;
    }
    
    printf("Cognitive Filesystem Tree:\n");
    print_tree_recursive(global_cogfs->root, 0);
}
