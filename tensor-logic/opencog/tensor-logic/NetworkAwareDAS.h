/*
 * NetworkAwareDAS.h - Network-Aware Distributed AtomSpace
 *
 * Captures flow tensors for DAS movement patterns and provides
 * optimal routing and node placement decisions.
 */

#ifndef _OPENCOG_NETWORK_AWARE_DAS_H
#define _OPENCOG_NETWORK_AWARE_DAS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace opencog {
namespace tensor_logic {

class NetworkAwareDAS {
public:
    struct FlowRecord {
        std::string source;
        std::string target;
        std::vector<float> flow_vector;
        double bandwidth;
        double latency;
        uint64_t timestamp;
    };

    struct NodeStats {
        std::string node_id;
        double total_inflow;
        double total_outflow;
        double avg_latency;
        std::vector<float> load_tensor;
    };

    NetworkAwareDAS();
    ~NetworkAwareDAS() = default;

    void recordFlow(const FlowRecord& flow);
    std::vector<FlowRecord> getFlowHistory(const std::string& node_id, size_t limit = 100);

    std::vector<float> predictOptimalRoute(const std::string& source, const std::string& target);
    double estimateCost(const std::string& source, const std::string& target);
    std::vector<std::string> getOptimalNodePlacement(const std::vector<float>& atom_tensor);

    NodeStats getNodeStats(const std::string& node_id) const;
    std::vector<std::string> getTopNodes(size_t k) const;

    void updateTopology(const std::string& node_id, const std::vector<std::string>& neighbors);
    std::vector<float> computeNetworkEmbedding(const std::string& node_id);

private:
    mutable std::mutex mutex_;
    std::vector<FlowRecord> flow_history_;
    std::unordered_map<std::string, NodeStats> node_stats_;
    std::unordered_map<std::string, std::vector<std::string>> topology_;

    void updateNodeStats(const FlowRecord& flow);
    std::vector<float> computeFlowTensor(const std::string& node_id);
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_NETWORK_AWARE_DAS_H
