/*
 * Cognitive 9P Filesystem Implementation
 * 
 * Exposes cognitive resources (AtomSpace, reasoning, memory, attention)
 * through the 9P protocol as a hierarchical filesystem. This allows
 * cognitive operations to be performed using standard file operations.
 */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../atomspace/atomspace.h"
#include "cognitive9p.h"

/* Cognitive filesystem structure */
typedef struct CogFs {
    Qid qid;
    int type;
    char *name;
    u32int mode;
    u32int atime;
    u32int mtime;
    u64int length;
    void *data;
    struct CogFs *parent;
    struct CogFs *children;
    struct CogFs *next;
} CogFs;

/* Root of cognitive filesystem */
static CogFs *cogfs_root = nil;
static Lock cogfs_lock;

/* Qid path counter */
static u64int next_qid_path = 1;

/* Forward declarations */
static CogFs* cogfs_create_dir(char *name, CogFs *parent);
static CogFs* cogfs_create_file(char *name, CogFs *parent, u32int mode);
static CogFs* cogfs_lookup(CogFs *dir, char *name);
static void cogfs_init_tree(void);

/*
 * Initialize the Cognitive 9P filesystem
 */
void
cognitive9p_init(void)
{
    print("Cognitive9P: Initializing filesystem\n");
    
    /* Create root directory */
    cogfs_root = cogfs_create_dir("cognitive", nil);
    if (cogfs_root == nil) {
        panic("Cognitive9P: Failed to create root directory");
    }
    
    /* Build cognitive filesystem tree */
    cogfs_init_tree();
    
    print("Cognitive9P: Filesystem initialized\n");
}

/*
 * Initialize the cognitive filesystem tree structure
 */
static void
cogfs_init_tree(void)
{
    CogFs *atomspace, *reasoning, *memory, *attention, *perception, *action, *learning;
    CogFs *pln, *ure, *moses;
    CogFs *working, *episodic, *semantic, *procedural;
    
    /* Create main directories */
    atomspace = cogfs_create_dir("atomspace", cogfs_root);
    reasoning = cogfs_create_dir("reasoning", cogfs_root);
    memory = cogfs_create_dir("memory", cogfs_root);
    attention = cogfs_create_dir("attention", cogfs_root);
    perception = cogfs_create_dir("perception", cogfs_root);
    action = cogfs_create_dir("action", cogfs_root);
    learning = cogfs_create_dir("learning", cogfs_root);
    
    /* AtomSpace subdirectories */
    cogfs_create_dir("atoms", atomspace);
    cogfs_create_dir("links", atomspace);
    cogfs_create_dir("queries", atomspace);
    cogfs_create_dir("patterns", atomspace);
    cogfs_create_file("stats", atomspace, 0444);
    cogfs_create_file("count", atomspace, 0444);
    
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
    
    cogfs_create_file("capacity", working, 0644);
    cogfs_create_file("contents", working, 0444);
    
    /* Attention subdirectories */
    cogfs_create_dir("focus", attention);
    cogfs_create_dir("importance", attention);
    cogfs_create_dir("urgency", attention);
    cogfs_create_file("allocation", attention, 0444);
    cogfs_create_file("sti_funds", attention, 0644);
    cogfs_create_file("lti_funds", attention, 0644);
    
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
 * Create a directory node in the cognitive filesystem
 */
static CogFs*
cogfs_create_dir(char *name, CogFs *parent)
{
    CogFs *dir;
    
    dir = mallocz(sizeof(CogFs), 1);
    if (dir == nil) {
        return nil;
    }
    
    dir->qid.path = next_qid_path++;
    dir->qid.vers = 0;
    dir->qid.type = QTDIR;
    dir->type = QTDIR;
    dir->name = strdup(name);
    dir->mode = DMDIR | 0755;
    dir->atime = seconds();
    dir->mtime = seconds();
    dir->length = 0;
    dir->data = nil;
    dir->parent = parent;
    dir->children = nil;
    dir->next = nil;
    
    /* Add to parent's children list */
    if (parent != nil) {
        lock(&cogfs_lock);
        dir->next = parent->children;
        parent->children = dir;
        unlock(&cogfs_lock);
    }
    
    return dir;
}

/*
 * Create a file node in the cognitive filesystem
 */
static CogFs*
cogfs_create_file(char *name, CogFs *parent, u32int mode)
{
    CogFs *file;
    
    file = mallocz(sizeof(CogFs), 1);
    if (file == nil) {
        return nil;
    }
    
    file->qid.path = next_qid_path++;
    file->qid.vers = 0;
    file->qid.type = QTFILE;
    file->type = QTFILE;
    file->name = strdup(name);
    file->mode = mode;
    file->atime = seconds();
    file->mtime = seconds();
    file->length = 0;
    file->data = nil;
    file->parent = parent;
    file->children = nil;
    file->next = nil;
    
    /* Add to parent's children list */
    if (parent != nil) {
        lock(&cogfs_lock);
        file->next = parent->children;
        parent->children = file;
        unlock(&cogfs_lock);
    }
    
    return file;
}

/*
 * Lookup a file or directory by name in a directory
 */
static CogFs*
cogfs_lookup(CogFs *dir, char *name)
{
    CogFs *child;
    
    if (dir == nil || name == nil) {
        return nil;
    }
    
    lock(&cogfs_lock);
    
    child = dir->children;
    while (child != nil) {
        if (strcmp(child->name, name) == 0) {
            unlock(&cogfs_lock);
            return child;
        }
        child = child->next;
    }
    
    unlock(&cogfs_lock);
    return nil;
}

/*
 * Read from a cognitive file
 * Dynamically generates content based on file type
 */
long
cognitive9p_read(CogFs *file, void *buf, long n, vlong offset)
{
    char *content;
    long len;
    
    if (file == nil || buf == nil) {
        return -1;
    }
    
    /* Generate dynamic content based on file path */
    if (strcmp(file->name, "stats") == 0) {
        /* AtomSpace statistics */
        content = smprint("AtomSpace Statistics\n"
                         "Total Atoms: %ud\n"
                         "Nodes: %ud\n"
                         "Links: %ud\n",
                         0, 0, 0);  /* TODO: Get real stats */
    } else if (strcmp(file->name, "count") == 0) {
        content = smprint("%ud\n", 0);  /* TODO: Get real count */
    } else if (strcmp(file->name, "allocation") == 0) {
        /* Attention allocation */
        content = smprint("Attention Allocation\n"
                         "STI Funds: %d\n"
                         "LTI Funds: %d\n",
                         0, 0);  /* TODO: Get real values */
    } else {
        content = smprint("Cognitive resource: %s\n", file->name);
    }
    
    if (content == nil) {
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
    
    memmove(buf, content + offset, n);
    free(content);
    
    return n;
}

/*
 * Write to a cognitive file
 * Performs cognitive operations based on file type
 */
long
cognitive9p_write(CogFs *file, void *buf, long n, vlong offset)
{
    char *data;
    
    if (file == nil || buf == nil) {
        return -1;
    }
    
    /* Allocate buffer for data */
    data = mallocz(n + 1, 1);
    if (data == nil) {
        return -1;
    }
    
    memmove(data, buf, n);
    data[n] = '\0';
    
    /* Process write based on file type */
    if (strcmp(file->name, "capacity") == 0) {
        /* Set working memory capacity */
        /* TODO: Parse and set capacity */
        print("Cognitive9P: Setting working memory capacity: %s\n", data);
    } else if (strcmp(file->name, "sti_funds") == 0) {
        /* Set STI funds */
        /* TODO: Parse and set STI funds */
        print("Cognitive9P: Setting STI funds: %s\n", data);
    } else if (strcmp(file->name, "lti_funds") == 0) {
        /* Set LTI funds */
        /* TODO: Parse and set LTI funds */
        print("Cognitive9P: Setting LTI funds: %s\n", data);
    } else {
        print("Cognitive9P: Write to %s: %s\n", file->name, data);
    }
    
    free(data);
    
    /* Update modification time */
    file->mtime = seconds();
    
    return n;
}

/*
 * Create a new atom through the filesystem
 * Writing to /cognitive/atomspace/atoms/new creates a new atom
 */
u32int
cognitive9p_create_atom(char *type, char *name)
{
    u16int atom_type;
    u32int atom_id;
    
    /* Parse atom type */
    if (strcmp(type, "ConceptNode") == 0) {
        atom_type = ATOM_TYPE_CONCEPT;
    } else if (strcmp(type, "PredicateNode") == 0) {
        atom_type = ATOM_TYPE_PREDICATE;
    } else {
        atom_type = ATOM_TYPE_NODE;
    }
    
    /* Create atom in global atomspace */
    atom_id = atomspace_add_node(get_global_atomspace(), atom_type, name);
    
    print("Cognitive9P: Created atom %ud: %s(%s)\n", atom_id, type, name);
    
    return atom_id;
}

/*
 * Query atoms through the filesystem
 * Reading from /cognitive/atomspace/queries/<pattern> performs a query
 */
long
cognitive9p_query_atoms(char *pattern, void *buf, long n)
{
    char *result;
    long len;
    
    /* TODO: Implement pattern matching */
    result = smprint("Query results for pattern: %s\n"
                    "No atoms matched\n", pattern);
    
    if (result == nil) {
        return -1;
    }
    
    len = strlen(result);
    if (n > len) {
        n = len;
    }
    
    memmove(buf, result, n);
    free(result);
    
    return n;
}

/*
 * Get global AtomSpace instance
 */
AtomSpace*
get_global_atomspace(void)
{
    extern AtomSpace *global_atomspace;
    return global_atomspace;
}

/*
 * Print cognitive filesystem tree (for debugging)
 */
static void
print_cogfs_tree_recursive(CogFs *node, int depth)
{
    CogFs *child;
    int i;
    
    if (node == nil) {
        return;
    }
    
    /* Print indentation */
    for (i = 0; i < depth; i++) {
        print("  ");
    }
    
    /* Print node */
    if (node->type == QTDIR) {
        print("[DIR]  %s/\n", node->name);
    } else {
        print("[FILE] %s\n", node->name);
    }
    
    /* Print children */
    child = node->children;
    while (child != nil) {
        print_cogfs_tree_recursive(child, depth + 1);
        child = child->next;
    }
}

void
print_cogfs_tree(void)
{
    print("Cognitive Filesystem Tree:\n");
    print_cogfs_tree_recursive(cogfs_root, 0);
}
