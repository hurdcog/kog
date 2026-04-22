/**
 * AtomSpace-HurdCog Integration Bridge
 * 
 * Provides bidirectional communication between OpenCog AtomSpace (OCC Layer 3)
 * and HurdCog cognitive kernel (Layer 2) for AGI-OS integration.
 * 
 * @file bridge.h
 * @author OCC Development Team
 * @date 2025-11-30
 * @version 1.0.0
 */

#ifndef ATOMSPACE_HURDCOG_BRIDGE_H
#define ATOMSPACE_HURDCOG_BRIDGE_H

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace opencog {
namespace hurdcog {

/**
 * @brief Cognitive operation types for kernel-level processing
 */
enum class CognitiveOpType {
    INFERENCE,          // PLN inference request
    ATTENTION_ALLOC,    // ECAN attention allocation
    LEARNING,           // Learning operation
    PATTERN_MATCH,      // Pattern matching query
    ATOM_STORE,         // Persistent atom storage
    ATOM_RETRIEVE,      // Atom retrieval from kernel
    SYNC_REQUEST        // Distributed synchronization
};

/**
 * @brief Status codes for bridge operations
 */
enum class BridgeStatus {
    SUCCESS,
    ERROR_CONNECTION,
    ERROR_SERIALIZATION,
    ERROR_TIMEOUT,
    ERROR_INVALID_OP,
    ERROR_KERNEL_BUSY
};

/**
 * @brief Cognitive message structure for IPC
 */
struct CognitiveMessage {
    CognitiveOpType op_type;
    std::string payload;           // Serialized AtomSpace data
    uint64_t timestamp;
    uint32_t priority;             // ECAN-based priority
    std::string source_node;       // For distributed operations
    
    CognitiveMessage() 
        : op_type(CognitiveOpType::INFERENCE), 
          timestamp(0), 
          priority(0) {}
};

/**
 * @brief Response from HurdCog kernel
 */
struct CognitiveResponse {
    BridgeStatus status;
    std::string result;            // Serialized result data
    uint64_t processing_time_us;
    std::string error_message;
    
    CognitiveResponse() 
        : status(BridgeStatus::SUCCESS), 
          processing_time_us(0) {}
};

/**
 * @brief Main bridge interface for AtomSpace-HurdCog communication
 */
class AtomSpaceHurdCogBridge {
public:
    /**
     * @brief Initialize the bridge connection to HurdCog kernel
     * @param kernel_port Mach port for HurdCog cognitive kernel
     * @return true if connection successful, false otherwise
     */
    bool initialize(const std::string& kernel_port = "/dev/hurdcog/cogkernel");
    
    /**
     * @brief Send a cognitive operation to HurdCog kernel
     * @param message Cognitive message to send
     * @param timeout_ms Timeout in milliseconds
     * @return Response from kernel
     */
    CognitiveResponse send_operation(
        const CognitiveMessage& message, 
        uint32_t timeout_ms = 5000
    );
    
    /**
     * @brief Register callback for asynchronous kernel events
     * @param op_type Operation type to listen for
     * @param callback Function to call when event occurs
     */
    void register_callback(
        CognitiveOpType op_type,
        std::function<void(const CognitiveMessage&)> callback
    );
    
    /**
     * @brief Serialize AtomSpace atoms for kernel storage
     * @param atom_data Atom data in Scheme format
     * @return Serialized binary representation
     */
    std::vector<uint8_t> serialize_atoms(const std::string& atom_data);
    
    /**
     * @brief Deserialize atoms from kernel storage
     * @param binary_data Serialized atom data
     * @return Atom data in Scheme format
     */
    std::string deserialize_atoms(const std::vector<uint8_t>& binary_data);
    
    /**
     * @brief Check if bridge is connected to HurdCog kernel
     * @return true if connected, false otherwise
     */
    bool is_connected() const;
    
    /**
     * @brief Get bridge statistics
     * @return Statistics as key-value pairs
     */
    std::map<std::string, uint64_t> get_statistics() const;
    
    /**
     * @brief Shutdown the bridge connection
     */
    void shutdown();
    
    /**
     * @brief Get singleton instance
     */
    static AtomSpaceHurdCogBridge& instance();

private:
    AtomSpaceHurdCogBridge();
    ~AtomSpaceHurdCogBridge();
    
    // Prevent copying
    AtomSpaceHurdCogBridge(const AtomSpaceHurdCogBridge&) = delete;
    AtomSpaceHurdCogBridge& operator=(const AtomSpaceHurdCogBridge&) = delete;
    
    class BridgeImpl;
    std::unique_ptr<BridgeImpl> impl_;
};

/**
 * @brief Helper function to send PLN inference request to kernel
 * @param query Inference query in Scheme
 * @param timeout_ms Timeout in milliseconds
 * @return Inference result
 */
std::string send_pln_inference(
    const std::string& query, 
    uint32_t timeout_ms = 10000
);

/**
 * @brief Helper function to request ECAN attention allocation
 * @param atom_handles List of atom handles
 * @return Updated attention values
 */
std::map<std::string, float> request_attention_allocation(
    const std::vector<std::string>& atom_handles
);

/**
 * @brief Helper function to store atoms persistently in kernel
 * @param atoms Atoms to store in Scheme format
 * @return true if successful
 */
bool store_atoms_in_kernel(const std::string& atoms);

/**
 * @brief Helper function to retrieve atoms from kernel storage
 * @param query Query pattern for retrieval
 * @return Retrieved atoms in Scheme format
 */
std::string retrieve_atoms_from_kernel(const std::string& query);

} // namespace hurdcog
} // namespace opencog

#endif // ATOMSPACE_HURDCOG_BRIDGE_H
