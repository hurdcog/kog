/*
 * MultiScaleAnalysis.h - Multi-Scale Temporal Analysis
 *
 * Provides time-series tensor analysis at different granularities
 * for pattern detection and temporal reasoning.
 */

#ifndef _OPENCOG_MULTI_SCALE_ANALYSIS_H
#define _OPENCOG_MULTI_SCALE_ANALYSIS_H

#include <vector>
#include <cstddef>

namespace opencog {
namespace tensor_logic {

class MultiScaleAnalysis {
public:
    MultiScaleAnalysis(size_t num_scales = 5, double scale_factor = 2.0);
    ~MultiScaleAnalysis() = default;

    std::vector<float> analyzeAtScale(const std::vector<float>& input, size_t scale_index);
    std::vector<std::vector<float>> decompose(const std::vector<float>& input);
    std::vector<float> reconstruct(const std::vector<std::vector<float>>& scales);

    std::vector<float> detectPeriodicity(const std::vector<float>& input);
    std::vector<float> extractTrend(const std::vector<float>& input);
    std::vector<float> extractNoise(const std::vector<float>& input);

    size_t getNumScales() const { return num_scales_; }
    double getScaleFactor() const { return scale_factor_; }

private:
    size_t num_scales_;
    double scale_factor_;

    std::vector<float> downsample(const std::vector<float>& input, size_t factor);
    std::vector<float> upsample(const std::vector<float>& input, size_t factor);
    std::vector<float> lowPassFilter(const std::vector<float>& input, size_t window);
};

} // namespace tensor_logic
} // namespace opencog

#endif // _OPENCOG_MULTI_SCALE_ANALYSIS_H
