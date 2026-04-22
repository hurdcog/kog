#pragma once

#include "Atom.h"
#include <chrono>
#include <map>
#include <mutex>
#include <vector>

namespace at {
namespace atomspace {

/**
 * TimeServer - Manages temporal information for atoms
 * 
 * The TimeServer tracks time-related annotations and events within AtomSpace.
 * It supports temporal reasoning, time-stamping atoms, and handling time-based
 * queries, which is crucial for episodic memory and processes needing
 * chronological context.
 */
class TimeServer {
public:
    using Handle = Atom::Handle;
    using TimePoint = std::chrono::system_clock::time_point;
    using Duration = std::chrono::system_clock::duration;
    
    /**
     * Temporal information for an atom
     */
    struct TemporalInfo {
        TimePoint creationTime;
        TimePoint lastAccessTime;
        TimePoint lastModifiedTime;
        std::vector<std::pair<TimePoint, std::string>> eventHistory;
        
        TemporalInfo() 
            : creationTime(std::chrono::system_clock::now()),
              lastAccessTime(std::chrono::system_clock::now()),
              lastModifiedTime(std::chrono::system_clock::now()) {}
    };
    
    TimeServer() = default;
    ~TimeServer() = default;
    
    // Prevent copying
    TimeServer(const TimeServer&) = delete;
    TimeServer& operator=(const TimeServer&) = delete;
    
    /**
     * Record atom creation time
     */
    void recordCreation(Handle atom) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        temporal_info_[atom] = TemporalInfo();
    }
    
    /**
     * Update last access time for an atom
     */
    void recordAccess(Handle atom) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            it->second.lastAccessTime = std::chrono::system_clock::now();
        } else {
            temporal_info_[atom] = TemporalInfo();
        }
    }
    
    /**
     * Update last modified time for an atom
     */
    void recordModification(Handle atom) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            it->second.lastModifiedTime = std::chrono::system_clock::now();
        } else {
            temporal_info_[atom] = TemporalInfo();
        }
    }
    
    /**
     * Record a custom event for an atom
     */
    void recordEvent(Handle atom, const std::string& eventDescription) {
        if (!atom) return;
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            it->second.eventHistory.push_back({
                std::chrono::system_clock::now(), 
                eventDescription
            });
        } else {
            temporal_info_[atom] = TemporalInfo();
            temporal_info_[atom].eventHistory.push_back({
                std::chrono::system_clock::now(), 
                eventDescription
            });
        }
    }
    
    /**
     * Get temporal information for an atom
     */
    TemporalInfo getTemporalInfo(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            return it->second;
        }
        return TemporalInfo();
    }
    
    /**
     * Get creation time for an atom
     */
    TimePoint getCreationTime(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            return it->second.creationTime;
        }
        return std::chrono::system_clock::now();
    }
    
    /**
     * Get last access time for an atom
     */
    TimePoint getLastAccessTime(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            return it->second.lastAccessTime;
        }
        return std::chrono::system_clock::now();
    }
    
    /**
     * Get last modified time for an atom
     */
    TimePoint getLastModifiedTime(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            return it->second.lastModifiedTime;
        }
        return std::chrono::system_clock::now();
    }
    
    /**
     * Get event history for an atom
     */
    std::vector<std::pair<TimePoint, std::string>> getEventHistory(Handle atom) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = temporal_info_.find(atom);
        if (it != temporal_info_.end()) {
            return it->second.eventHistory;
        }
        return {};
    }
    
    /**
     * Get atoms created within a time range
     */
    std::vector<Handle> getAtomsCreatedBetween(TimePoint start, TimePoint end) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& [atom, info] : temporal_info_) {
            if (info.creationTime >= start && info.creationTime <= end) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Get atoms accessed within a time range
     */
    std::vector<Handle> getAtomsAccessedBetween(TimePoint start, TimePoint end) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& [atom, info] : temporal_info_) {
            if (info.lastAccessTime >= start && info.lastAccessTime <= end) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Get atoms modified within a time range
     */
    std::vector<Handle> getAtomsModifiedBetween(TimePoint start, TimePoint end) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<Handle> result;
        for (const auto& [atom, info] : temporal_info_) {
            if (info.lastModifiedTime >= start && info.lastModifiedTime <= end) {
                result.push_back(atom);
            }
        }
        return result;
    }
    
    /**
     * Remove temporal information for an atom
     */
    void remove(Handle atom) {
        std::lock_guard<std::mutex> lock(mutex_);
        temporal_info_.erase(atom);
    }
    
    /**
     * Clear all temporal information
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        temporal_info_.clear();
    }
    
    /**
     * Get number of atoms being tracked
     */
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return temporal_info_.size();
    }
    
private:
    mutable std::mutex mutex_;
    std::map<Handle, TemporalInfo> temporal_info_;
};

} // namespace atomspace
} // namespace at
