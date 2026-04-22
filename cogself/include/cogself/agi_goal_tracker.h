/**
 * AGI Goal Tracker
 * Tracks progress toward AGI objectives and milestones
 */

#ifndef _COGSELF_AGI_GOAL_TRACKER_H
#define _COGSELF_AGI_GOAL_TRACKER_H

#include <memory>
#include <string>
#include <vector>

namespace cogself {

/**
 * AGIGoalTracker - Tracks and manages AGI goals
 */
class AGIGoalTracker {
public:
    AGIGoalTracker();
    ~AGIGoalTracker();

    // Add a new AGI goal
    void addGoal(const std::string& goalName, double targetValue);

    // Update goal progress
    void updateProgress(const std::string& goalName, double currentValue);

    // Get progress for a specific goal
    double getProgress(const std::string& goalName) const;

    // Get all goals
    std::vector<std::string> getAllGoals() const;

    // Calculate overall AGI progress
    double calculateOverallProgress() const;

    // Generate progress report
    std::string generateProgressReport() const;

    // Identify next priority goals
    std::string identifyPriorityGoals() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace cogself

#endif // _COGSELF_AGI_GOAL_TRACKER_H
