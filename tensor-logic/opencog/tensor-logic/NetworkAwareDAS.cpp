/*
 * NetworkAwareDAS.cpp - Implementation
 */

#include "NetworkAwareDAS.h"
#include <algorithm>
#include <cmath>
#include <random>

namespace opencog {
namespace tensor_logic {

NetworkAwareDAS::NetworkAwareDAS() {}

void NetworkAwareDAS::recordFlow(const FlowRecord& flow) {
    std::lock_guard<std::mutex> lock(mutex_);
    flow_history_.push_back(flow);
    updateNodeStats(flow);

    // Limit history size
    const size_t max_history = 50000;
    if (flow_history_.size() > max_history) {
        flow_history_.erase(flow_history_.begin(),
                           flow_history_.begin() + flow_history_.size() - max_history);
    }
}

std::vector<NetworkAwareDAS::FlowRecord> NetworkAwareDAS::getFlowHistory(
    const std::string& node_id, size_t limit) {

    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<FlowRecord> result;

    for (auto it = flow_history_.rbegin();
         it != flow_history_.rend() && result.size() < limit; ++it) {
        if (it->source == node_id || it->target == node_id) {
            result.push_back(*it);
        }
    }

    return result;
}

std::vector<float> NetworkAwareDAS::predictOptimalRoute(
    const std::string& source, const std::string& target) {

    std::lock_guard<std::mutex> lock(mutex_);

    // Compute route embedding based on historical flows
    std::vector<float> route_tensor(64, 0.0f);

    auto src_it = node_stats_.find(source);
    auto tgt_it = node_stats_.find(target);

    if (src_it != node_stats_.end() && !src_it->second.load_tensor.empty()) {
        for (size_t i = 0; i < route_tensor.size() && i < src_it->second.load_tensor.size(); ++i) {
            route_tensor[i] += src_it->second.load_tensor[i];
        }
    }

    if (tgt_it != node_stats_.end() && !tgt_it->second.load_tensor.empty()) {
        for (size_t i = 0; i < route_tensor.size() && i < tgt_it->second.load_tensor.size(); ++i) {
            route_tensor[i] += tgt_it->second.load_tensor[i];
        }
    }

    // Normalize
    float norm = 0.0f;
    for (float v : route_tensor) norm += v * v;
    norm = std::sqrt(norm);
    if (norm > 1e-8f) {
        for (auto& v : route_tensor) v /= norm;
    }

    return route_tensor;
}

double NetworkAwareDAS::estimateCost(const std::string& source, const std::string& target) {
    std::lock_guard<std::mutex> lock(mutex_);

    double total_latency = 0.0;
    double total_bandwidth = 0.0;
    size_t count = 0;

    for (const auto& flow : flow_history_) {
        if (flow.source == source && flow.target == target) {
            total_latency += flow.latency;
            total_bandwidth += flow.bandwidth;
            ++count;
        }
    }

    if (count == 0) {
        return 1000.0; // Default high cost for unknown routes
    }

    double avg_latency = total_latency / count;
    double avg_bandwidth = total_bandwidth / count;

    return avg_bandwidth > 0 ? avg_latency / avg_bandwidth : avg_latency;
}

std::vector<std::string> NetworkAwareDAS::getOptimalNodePlacement(
    const std::vector<float>& atom_tensor) {

    std::lock_guard<std::mutex> lock(mutex_);

    // Score nodes based on tensor similarity and load
    std::vector<std::pair<std::string, double>> scores;

    for (const auto& pair : node_stats_) {
        double score = 0.0;

        // Similarity to atom tensor
        if (!pair.second.load_tensor.empty()) {
            double dot = 0.0;
            for (size_t i = 0; i < atom_tensor.size() && i < pair.second.load_tensor.size(); ++i) {
                dot += atom_tensor[i] * pair.second.load_tensor[i];
            }
            score += dot;
        }

        // Penalize high-load nodes
        double load = pair.second.total_inflow + pair.second.total_outflow;
        score -= load * 0.001;

        scores.emplace_back(pair.first, score);
    }

    // Sort by score descending
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (const auto& p : scores) {
        result.push_back(p.first);
    }

    return result;
}

NetworkAwareDAS::NodeStats NetworkAwareDAS::getNodeStats(const std::string& node_id) const {
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = node_stats_.find(node_id);
    if (it != node_stats_.end()) {
        return it->second;
    }
    return NodeStats{node_id, 0.0, 0.0, 0.0, {}};
}

std::vector<std::string> NetworkAwareDAS::getTopNodes(size_t k) const {
    std::lock_guard<std::mutex> lock(mutex_);

    std::vector<std::pair<std::string, double>> nodes;
    for (const auto& pair : node_stats_) {
        double activity = pair.second.total_inflow + pair.second.total_outflow;
        nodes.emplace_back(pair.first, activity);
    }

    std::sort(nodes.begin(), nodes.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    std::vector<std::string> result;
    for (size_t i = 0; i < k && i < nodes.size(); ++i) {
        result.push_back(nodes[i].first);
    }

    return result;
}

void NetworkAwareDAS::updateTopology(const std::string& node_id,
                                      const std::vector<std::string>& neighbors) {
    std::lock_guard<std::mutex> lock(mutex_);
    topology_[node_id] = neighbors;
}

std::vector<float> NetworkAwareDAS::computeNetworkEmbedding(const std::string& node_id) {
    return computeFlowTensor(node_id);
}

void NetworkAwareDAS::updateNodeStats(const FlowRecord& flow) {
    // Update source stats
    auto& src_stats = node_stats_[flow.source];
    src_stats.node_id = flow.source;
    src_stats.total_outflow += flow.bandwidth;

    // Update target stats
    auto& tgt_stats = node_stats_[flow.target];
    tgt_stats.node_id = flow.target;
    tgt_stats.total_inflow += flow.bandwidth;
    tgt_stats.avg_latency = (tgt_stats.avg_latency + flow.latency) / 2.0;

    // Update load tensors
    if (src_stats.load_tensor.empty()) {
        src_stats.load_tensor.resize(64, 0.0f);
    }
    if (tgt_stats.load_tensor.empty()) {
        tgt_stats.load_tensor.resize(64, 0.0f);
    }

    // Incorporate flow vector into load tensor
    for (size_t i = 0; i < flow.flow_vector.size() && i < 64; ++i) {
        src_stats.load_tensor[i] = src_stats.load_tensor[i] * 0.9f + flow.flow_vector[i] * 0.1f;
        tgt_stats.load_tensor[i] = tgt_stats.load_tensor[i] * 0.9f + flow.flow_vector[i] * 0.1f;
    }
}

std::vector<float> NetworkAwareDAS::computeFlowTensor(const std::string& node_id) {
    auto it = node_stats_.find(node_id);
    if (it != node_stats_.end()) {
        return it->second.load_tensor;
    }

    // Default embedding
    std::vector<float> embedding(64);
    std::hash<std::string> hasher;
    std::mt19937 gen(hasher(node_id));
    std::normal_distribution<float> dist(0.0f, 0.1f);
    for (auto& v : embedding) {
        v = dist(gen);
    }
    return embedding;
}

} // namespace tensor_logic
} // namespace opencog
