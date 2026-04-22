/* 9pfs.c - Main 9P filesystem translator
 *
 * Copyright (C) 2024 Free Software Foundation, Inc.
 *
 * This file implements the main entry point and initialization
 * for the 9P filesystem translator for GNU Hurd.
 */

#include "9pfs.h"
#include <argp.h>
#include <error.h>
#include <stdlib.h>
#include <string.h>

/* Global variables */
struct p9_connection *p9_conn = NULL;
struct p9_namespace *p9_ns = NULL;
char *p9_server_addr = NULL;
int p9_server_port = 564;  /* Default 9P port */
char *p9_username = "root";
char *p9_attach_name = "";

/* Command line options */
static const struct argp_option options[] =
{
    {"server", 's', "ADDRESS", 0, "9P server address", 0},
    {"port", 'p', "PORT", 0, "9P server port (default: 564)", 0},
    {"user", 'u', "USER", 0, "Username for 9P connection", 0},
    {"attach", 'a', "NAME", 0, "Attachment name", 0},
    {0}
};

static const char doc[] = "9P filesystem translator for GNU Hurd";
static const char args_doc[] = "UNDERLYING";

/* Parse command line arguments */
static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
    case 's':
        p9_server_addr = strdup(arg);
        break;
    case 'p':
        p9_server_port = atoi(arg);
        break;
    case 'u':
        p9_username = strdup(arg);
        break;
    case 'a':
        p9_attach_name = strdup(arg);
        break;
    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
            argp_usage(state);
        /* Store underlying node - not used in this implementation */
        break;
    case ARGP_KEY_NO_ARGS:
        if (!p9_server_addr)
            argp_error(state, "server address is required");
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static const struct argp argp = { options, parse_opt, args_doc, doc };

/* Initialize the 9P filesystem */
error_t
p9fs_init(void)
{
    error_t err;
    struct p9_fid *root_fid;
    
    /* Create namespace */
    p9_ns = p9_namespace_create();
    if (!p9_ns)
        return p9_to_hurd_error(p9_errno);
    
    /* Connect to 9P server */
    p9_conn = p9_connect(p9_server_addr, p9_server_port);
    if (!p9_conn) {
        p9_namespace_destroy(p9_ns);
        return p9_to_hurd_error(p9_errno);
    }
    
    /* Negotiate protocol version */
    if (p9_version(p9_conn, P9_VERSION, 8192) < 0) {
        err = p9_to_hurd_error(p9_errno);
        goto cleanup;
    }
    
    /* Attach to filesystem root */
    root_fid = p9_attach(p9_conn, p9_username, p9_attach_name);
    if (!root_fid) {
        err = p9_to_hurd_error(p9_errno);
        goto cleanup;
    }
    
    /* Create root netfs node */
    netfs_root_node = p9fs_make_node(root_fid);
    if (!netfs_root_node) {
        p9_clunk(root_fid);
        err = ENOMEM;
        goto cleanup;
    }
    
    return 0;
    
cleanup:
    p9_disconnect(p9_conn);
    p9_namespace_destroy(p9_ns);
    return err;
}

/* Shutdown the 9P filesystem */
void
p9fs_shutdown(void)
{
    if (p9_conn) {
        p9_disconnect(p9_conn);
        p9_conn = NULL;
    }
    
    if (p9_ns) {
        p9_namespace_destroy(p9_ns);
        p9_ns = NULL;
    }
}

/* Convert 9P errors to Hurd errors */
error_t
p9_to_hurd_error(int p9_error)
{
    switch (p9_error) {
    case 0:
        return 0;
    case P9_EIO:
        return EIO;
    case P9_EPROTO:
        return EPROTO;
    case P9_ENOMEM:
        return ENOMEM;
    case P9_EINVAL:
        return EINVAL;
    case P9_ENOENT:
        return ENOENT;
    case P9_EACCES:
        return EACCES;
    case P9_EEXIST:
        return EEXIST;
    case P9_EISDIR:
        return EISDIR;
    case P9_ENOTDIR:
        return ENOTDIR;
    case P9_EMFILE:
        return EMFILE;
    default:
        return EIO;
    }
}

/* Convert 9P mode to Hurd mode */
mode_t
p9_to_hurd_mode(uint32_t p9_mode)
{
    mode_t mode = 0;
    
    /* File type */
    if (p9_mode & P9_DMDIR)
        mode |= S_IFDIR;
    else
        mode |= S_IFREG;
    
    /* Permissions */
    mode |= (p9_mode & 0777);
    
    return mode;
}

/* Convert Hurd mode to 9P mode */
uint32_t
hurd_to_p9_mode(mode_t mode)
{
    uint32_t p9_mode = 0;
    
    /* File type */
    if (S_ISDIR(mode))
        p9_mode |= P9_DMDIR;
    
    /* Permissions */
    p9_mode |= (mode & 0777);
    
    return p9_mode;
}

/* Main function */
int
main(int argc, char **argv)
{
    error_t err;
    
    /* Parse arguments */
    argp_parse(&argp, argc, argv, 0, 0, 0);
    
    /* Set default server if not specified */
    if (!p9_server_addr)
        p9_server_addr = "localhost";
    
    /* Initialize netfs */
    netfs_init();
    
    /* Initialize 9P filesystem */
    err = p9fs_init();
    if (err)
        error(1, err, "cannot initialize 9P filesystem");
    
    /* Set up signal handlers for clean shutdown */
    signal(SIGINT, (void (*)(int))p9fs_shutdown);
    signal(SIGTERM, (void (*)(int))p9fs_shutdown);
    
    /* Start netfs server */
    netfs_server_loop();
    
    /* Cleanup */
    p9fs_shutdown();
    
    return 0;
}