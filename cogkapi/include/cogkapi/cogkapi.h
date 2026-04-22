/**
 * @file cogkapi.h
 * @brief CogKAPI - Cognitive Kernel API
 * 
 * A unified, cross-platform abstraction for kernel-level cognitive operations.
 * Provides a consistent interface across WinKoGNN, CogNumach, and Hurd-Cog.
 * 
 * @author Manus AI / cogpy
 * @version 1.0.0
 * @date 2025-12-17
 * 
 * @copyright Copyright (c) 2025 cogpy
 * Licensed under the MIT License
 */

#ifndef COGKAPI_H
#define COGKAPI_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
 * Version Information
 *============================================================================*/

#define COGKAPI_VERSION_MAJOR 1
#define COGKAPI_VERSION_MINOR 0
#define COGKAPI_VERSION_PATCH 0
#define COGKAPI_VERSION_STRING "1.0.0"

/*============================================================================
 * Platform Detection
 *============================================================================*/

#if defined(_WIN32) || defined(_WIN64)
    #define COGKAPI_PLATFORM_WINKOGNN 1
    #define COGKAPI_PLATFORM_NAME "WinKoGNN"
#elif defined(__gnu_hurd__)
    #define COGKAPI_PLATFORM_HURDCOG 1
    #define COGKAPI_PLATFORM_NAME "Hurd-Cog"
#elif defined(__MACH__)
    #define COGKAPI_PLATFORM_COGNUMACH 1
    #define COGKAPI_PLATFORM_NAME "CogNumach"
#else
    #define COGKAPI_PLATFORM_GENERIC 1
    #define COGKAPI_PLATFORM_NAME "Generic"
#endif

/*============================================================================
 * Export/Import Macros
 *============================================================================*/

#ifdef COGKAPI_PLATFORM_WINKOGNN
    #ifdef COGKAPI_EXPORTS
        #define COGKAPI_API __declspec(dllexport)
    #else
        #define COGKAPI_API __declspec(dllimport)
    #endif
#else
    #define COGKAPI_API __attribute__((visibility("default")))
#endif

/*============================================================================
 * Handle Types
 *============================================================================*/

/** @brief Opaque handle type for all kernel objects */
typedef uint64_t cog_handle_t;

/** @brief Null handle constant */
#define COG_NULL_HANDLE ((cog_handle_t)0)

/** @brief Handle validity check macro */
#define COG_HANDLE_VALID(h) ((h) != COG_NULL_HANDLE)

/** @brief Cognitive Context handle */
typedef cog_handle_t cog_context_t;

/** @brief AtomSpace handle */
typedef cog_handle_t cog_atomspace_t;

/** @brief Atom handle (generic) */
typedef cog_handle_t cog_atom_t;

/** @brief Node handle (specialization of Atom) */
typedef cog_handle_t cog_node_t;

/** @brief Link handle (specialization of Atom) */
typedef cog_handle_t cog_link_t;

/** @brief Event port handle for async operations */
typedef cog_handle_t cog_event_port_t;

/** @brief GNN context handle */
typedef cog_handle_t cog_gnn_context_t;

/*============================================================================
 * Status Codes
 *============================================================================*/

/** @brief Status code enumeration */
typedef enum cog_status {
    COG_STATUS_SUCCESS = 0,
    COG_STATUS_ERROR_GENERIC = -1,
    COG_STATUS_ERROR_INVALID_HANDLE = -2,
    COG_STATUS_ERROR_INVALID_ARGUMENT = -3,
    COG_STATUS_ERROR_OUT_OF_MEMORY = -4,
    COG_STATUS_ERROR_PERMISSION_DENIED = -5,
    COG_STATUS_ERROR_NOT_FOUND = -6,
    COG_STATUS_ERROR_ALREADY_EXISTS = -7,
    COG_STATUS_ERROR_TIMEOUT = -8,
    COG_STATUS_ERROR_NOT_IMPLEMENTED = -9,
    COG_STATUS_ERROR_PLATFORM_SPECIFIC = -100
} cog_status_t;

/** @brief Convert status code to string */
COGKAPI_API const char* cog_status_to_string(cog_status_t status);

/*============================================================================
 * Truth Value
 *============================================================================*/

/** @brief Truth value structure (strength, confidence) */
typedef struct cog_truth_value {
    double strength;    /**< Strength [0.0, 1.0] */
    double confidence;  /**< Confidence [0.0, 1.0] */
} cog_truth_value_t;

/** @brief Default truth value */
#define COG_TV_DEFAULT ((cog_truth_value_t){1.0, 1.0})

/** @brief Create truth value */
static inline cog_truth_value_t cog_tv(double strength, double confidence) {
    cog_truth_value_t tv = {strength, confidence};
    return tv;
}

/*============================================================================
 * Attention Value
 *============================================================================*/

/** @brief Attention value structure (STI, LTI) */
typedef struct cog_attention_value {
    int16_t sti;  /**< Short-Term Importance */
    int16_t lti;  /**< Long-Term Importance */
} cog_attention_value_t;

/** @brief Default attention value */
#define COG_AV_DEFAULT ((cog_attention_value_t){0, 0})

/*============================================================================
 * Event Types
 *============================================================================*/

/** @brief Event type enumeration */
typedef enum cog_event_type {
    COG_EVENT_NONE = 0,
    COG_EVENT_ATOM_ADDED,
    COG_EVENT_ATOM_REMOVED,
    COG_EVENT_ATOM_MODIFIED,
    COG_EVENT_PATTERN_MATCH_RESULT,
    COG_EVENT_REASONING_RESULT,
    COG_EVENT_GNN_EMBEDDING_COMPLETE,
    COG_EVENT_IPC_MESSAGE,
    COG_EVENT_CONTEXT_TERMINATED
} cog_event_type_t;

/** @brief Event structure */
typedef struct cog_event {
    cog_event_type_t type;       /**< Event type */
    cog_handle_t source;         /**< Source handle */
    cog_atom_t atom;             /**< Associated atom (if applicable) */
    void* data;                  /**< Additional event data */
    size_t data_size;            /**< Size of additional data */
} cog_event_t;

/** @brief Infinite wait constant */
#define COG_WAIT_INFINITE ((uint32_t)-1)

/*============================================================================
 * Context Flags
 *============================================================================*/

/** @brief Context creation flags */
typedef enum cog_context_flags {
    COG_CONTEXT_DEFAULT = 0,
    COG_CONTEXT_ISOLATED = (1 << 0),      /**< Full isolation */
    COG_CONTEXT_PRIVILEGED = (1 << 1),    /**< Kernel-level access */
    COG_CONTEXT_SHARED_MEMORY = (1 << 2)  /**< Allow shared memory */
} cog_context_flags_t;

/*============================================================================
 * Capability Rights
 *============================================================================*/

/** @brief Capability rights for context access control */
typedef enum cog_rights {
    COG_RIGHT_NONE = 0,
    COG_RIGHT_READ = (1 << 0),
    COG_RIGHT_WRITE = (1 << 1),
    COG_RIGHT_EXECUTE = (1 << 2),
    COG_RIGHT_DELETE = (1 << 3),
    COG_RIGHT_GRANT = (1 << 4),
    COG_RIGHT_ALL = 0xFF
} cog_rights_t;

/*============================================================================
 * Query Flags
 *============================================================================*/

/** @brief Query operation flags */
typedef enum cog_query_flags {
    COG_QUERY_SYNC = 0,
    COG_QUERY_ASYNC = (1 << 0),
    COG_QUERY_LIMIT_RESULTS = (1 << 1),
    COG_QUERY_INCLUDE_SUBGRAPH = (1 << 2)
} cog_query_flags_t;

/*============================================================================
 * Context Management API
 *============================================================================*/

/**
 * @brief Creates a new cognitive context.
 * 
 * A cognitive context is an isolated execution environment for cognitive
 * operations, analogous to a process in traditional operating systems.
 * 
 * @param parent_context Parent context (COG_NULL_HANDLE for root context)
 * @param flags Creation flags (see cog_context_flags_t)
 * @param out_context Pointer to store the new context handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_context_create(
    cog_context_t parent_context,
    uint32_t flags,
    cog_context_t* out_context
);

/**
 * @brief Destroys a cognitive context and releases all its resources.
 * 
 * @param context The context to destroy
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_context_destroy(cog_context_t context);

/**
 * @brief Grants capabilities to a context for accessing a kernel object.
 * 
 * @param context The target context
 * @param object The object to grant access to
 * @param rights The rights to grant (see cog_rights_t)
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_context_grant_capability(
    cog_context_t context,
    cog_handle_t object,
    uint32_t rights
);

/**
 * @brief Revokes capabilities from a context.
 * 
 * @param context The target context
 * @param object The object to revoke access to
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_context_revoke_capability(
    cog_context_t context,
    cog_handle_t object
);

/**
 * @brief Gets information about a context.
 * 
 * @param context The context to query
 * @param out_info Pointer to store context information
 * @return COG_STATUS_SUCCESS on success
 */
typedef struct cog_context_info {
    cog_context_t handle;
    cog_context_t parent;
    uint32_t flags;
    uint32_t num_atomspaces;
    uint64_t memory_usage;
} cog_context_info_t;

COGKAPI_API cog_status_t cog_context_get_info(
    cog_context_t context,
    cog_context_info_t* out_info
);

/*============================================================================
 * AtomSpace Management API
 *============================================================================*/

/**
 * @brief Creates a new AtomSpace within a cognitive context.
 * 
 * An AtomSpace is a container for Atoms, forming a hypergraph knowledge base.
 * 
 * @param context The owning context
 * @param name Human-readable name for the AtomSpace
 * @param out_atomspace Pointer to store the new AtomSpace handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atomspace_create(
    cog_context_t context,
    const char* name,
    cog_atomspace_t* out_atomspace
);

/**
 * @brief Destroys an AtomSpace and all contained Atoms.
 * 
 * @param atomspace The AtomSpace to destroy
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atomspace_destroy(cog_atomspace_t atomspace);

/**
 * @brief Attaches to an existing AtomSpace by name.
 * 
 * @param context The context requesting attachment
 * @param name The name of the AtomSpace
 * @param out_atomspace Pointer to store the AtomSpace handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atomspace_attach(
    cog_context_t context,
    const char* name,
    cog_atomspace_t* out_atomspace
);

/**
 * @brief Detaches from an AtomSpace.
 * 
 * @param atomspace The AtomSpace to detach from
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atomspace_detach(cog_atomspace_t atomspace);

/**
 * @brief Gets the number of Atoms in an AtomSpace.
 * 
 * @param atomspace The AtomSpace to query
 * @param out_count Pointer to store the count
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atomspace_get_count(
    cog_atomspace_t atomspace,
    size_t* out_count
);

/*============================================================================
 * Atom Management API
 *============================================================================*/

/**
 * @brief Creates a new Node in an AtomSpace.
 * 
 * @param atomspace The target AtomSpace
 * @param type The type of the Node (e.g., "ConceptNode")
 * @param name The name of the Node
 * @param out_node Pointer to store the new Node handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_node_create(
    cog_atomspace_t atomspace,
    const char* type,
    const char* name,
    cog_node_t* out_node
);

/**
 * @brief Creates a new Link in an AtomSpace.
 * 
 * @param atomspace The target AtomSpace
 * @param type The type of the Link (e.g., "InheritanceLink")
 * @param targets Array of target Atom handles
 * @param num_targets Number of targets
 * @param out_link Pointer to store the new Link handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_link_create(
    cog_atomspace_t atomspace,
    const char* type,
    const cog_atom_t targets[],
    size_t num_targets,
    cog_link_t* out_link
);

/**
 * @brief Removes an Atom from an AtomSpace.
 * 
 * @param atom The Atom to remove
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_remove(cog_atom_t atom);

/**
 * @brief Gets the truth value of an Atom.
 * 
 * @param atom The Atom to query
 * @param out_tv Pointer to store the truth value
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_get_truth_value(
    cog_atom_t atom,
    cog_truth_value_t* out_tv
);

/**
 * @brief Sets the truth value of an Atom.
 * 
 * @param atom The Atom to modify
 * @param tv The new truth value
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_set_truth_value(
    cog_atom_t atom,
    const cog_truth_value_t* tv
);

/**
 * @brief Gets the attention value of an Atom.
 * 
 * @param atom The Atom to query
 * @param out_av Pointer to store the attention value
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_get_attention_value(
    cog_atom_t atom,
    cog_attention_value_t* out_av
);

/**
 * @brief Sets the attention value of an Atom.
 * 
 * @param atom The Atom to modify
 * @param av The new attention value
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_set_attention_value(
    cog_atom_t atom,
    const cog_attention_value_t* av
);

/**
 * @brief Gets the type name of an Atom.
 * 
 * @param atom The Atom to query
 * @param out_type Buffer to store the type name
 * @param type_size Size of the buffer
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_atom_get_type(
    cog_atom_t atom,
    char* out_type,
    size_t type_size
);

/**
 * @brief Gets the name of a Node.
 * 
 * @param node The Node to query
 * @param out_name Buffer to store the name
 * @param name_size Size of the buffer
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_node_get_name(
    cog_node_t node,
    char* out_name,
    size_t name_size
);

/**
 * @brief Gets the outgoing set of a Link.
 * 
 * @param link The Link to query
 * @param out_targets Buffer to store target handles
 * @param max_targets Maximum number of targets to retrieve
 * @param out_count Actual number of targets
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_link_get_outgoing(
    cog_link_t link,
    cog_atom_t* out_targets,
    size_t max_targets,
    size_t* out_count
);

/*============================================================================
 * Pattern Matching and Reasoning API
 *============================================================================*/

/**
 * @brief Performs pattern matching on an AtomSpace.
 * 
 * @param atomspace The AtomSpace to query
 * @param query_atom The pattern to match
 * @param event_port Event port for results (if async)
 * @param flags Query flags (see cog_query_flags_t)
 * @return COG_STATUS_SUCCESS if query started successfully
 */
COGKAPI_API cog_status_t cog_atom_pattern_match(
    cog_atomspace_t atomspace,
    cog_atom_t query_atom,
    cog_event_port_t event_port,
    uint32_t flags
);

/**
 * @brief Executes a reasoning engine on premises.
 * 
 * @param context The execution context
 * @param engine_type Reasoning engine type (e.g., "PLN")
 * @param premises Array of premise Atoms
 * @param num_premises Number of premises
 * @param event_port Event port for results
 * @return COG_STATUS_SUCCESS if reasoning started successfully
 */
COGKAPI_API cog_status_t cog_reasoning_execute(
    cog_context_t context,
    const char* engine_type,
    const cog_atom_t premises[],
    size_t num_premises,
    cog_event_port_t event_port
);

/*============================================================================
 * Inter-Context Communication (Cognitive IPC) API
 *============================================================================*/

/**
 * @brief Creates an event port for receiving async notifications.
 * 
 * @param context The owning context
 * @param out_port Pointer to store the new event port handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_event_port_create(
    cog_context_t context,
    cog_event_port_t* out_port
);

/**
 * @brief Destroys an event port.
 * 
 * @param port The event port to destroy
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_event_port_destroy(cog_event_port_t port);

/**
 * @brief Sends an Atom to another context's event port.
 * 
 * @param target_port The destination event port
 * @param atom The Atom to send
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_ipc_send_atom(
    cog_event_port_t target_port,
    cog_atom_t atom
);

/**
 * @brief Receives the next event from an event port.
 * 
 * @param port The event port to listen on
 * @param timeout_ms Timeout in milliseconds (COG_WAIT_INFINITE for blocking)
 * @param out_event Pointer to store the received event
 * @return COG_STATUS_SUCCESS on success, COG_STATUS_ERROR_TIMEOUT on timeout
 */
COGKAPI_API cog_status_t cog_event_receive(
    cog_event_port_t port,
    uint32_t timeout_ms,
    cog_event_t* out_event
);

/*============================================================================
 * GNN Integration API
 *============================================================================*/

/**
 * @brief Creates a GNN context for an AtomSpace.
 * 
 * @param atomspace The AtomSpace to associate with the GNN
 * @param config JSON configuration string
 * @param out_gnn_context Pointer to store the GNN context handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_gnn_context_create(
    cog_atomspace_t atomspace,
    const char* config,
    cog_gnn_context_t* out_gnn_context
);

/**
 * @brief Destroys a GNN context.
 * 
 * @param gnn_context The GNN context to destroy
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_gnn_context_destroy(cog_gnn_context_t gnn_context);

/**
 * @brief Computes node embeddings for all Atoms.
 * 
 * @param gnn_context The GNN context
 * @param event_port Event port for completion notification
 * @return COG_STATUS_SUCCESS if computation started
 */
COGKAPI_API cog_status_t cog_gnn_compute_embeddings(
    cog_gnn_context_t gnn_context,
    cog_event_port_t event_port
);

/**
 * @brief Gets the embedding vector for an Atom.
 * 
 * @param gnn_context The GNN context
 * @param atom The Atom to query
 * @param out_embedding Buffer to store the embedding
 * @param embedding_size Size of the buffer
 * @param out_dim Actual embedding dimension
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_gnn_get_embedding(
    cog_gnn_context_t gnn_context,
    cog_atom_t atom,
    float* out_embedding,
    size_t embedding_size,
    size_t* out_dim
);

/**
 * @brief Predicts links between Atoms using GNN.
 * 
 * @param gnn_context The GNN context
 * @param threshold Similarity threshold for predictions
 * @param max_predictions Maximum number of predictions
 * @param event_port Event port for results
 * @return COG_STATUS_SUCCESS if prediction started
 */
COGKAPI_API cog_status_t cog_gnn_predict_links(
    cog_gnn_context_t gnn_context,
    float threshold,
    size_t max_predictions,
    cog_event_port_t event_port
);

/*============================================================================
 * Diagnostic/Autognostic API
 *============================================================================*/

/**
 * @brief Creates a diagnostic Atom for autognostic integration.
 * 
 * @param atomspace The target AtomSpace
 * @param diagnostic_type Type of diagnostic (e.g., "error", "stall")
 * @param message Diagnostic message
 * @param severity Severity score [0.0, 1.0]
 * @param out_atom Pointer to store the diagnostic Atom handle
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_diagnostic_create(
    cog_atomspace_t atomspace,
    const char* diagnostic_type,
    const char* message,
    double severity,
    cog_atom_t* out_atom
);

/*============================================================================
 * Utility Functions
 *============================================================================*/

/**
 * @brief Gets the CogKAPI version string.
 * 
 * @return Version string
 */
COGKAPI_API const char* cog_get_version(void);

/**
 * @brief Gets the current platform name.
 * 
 * @return Platform name string
 */
COGKAPI_API const char* cog_get_platform(void);

/**
 * @brief Initializes the CogKAPI library.
 * 
 * Must be called before any other CogKAPI functions.
 * 
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_init(void);

/**
 * @brief Shuts down the CogKAPI library.
 * 
 * Releases all resources and contexts.
 * 
 * @return COG_STATUS_SUCCESS on success
 */
COGKAPI_API cog_status_t cog_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif /* COGKAPI_H */
