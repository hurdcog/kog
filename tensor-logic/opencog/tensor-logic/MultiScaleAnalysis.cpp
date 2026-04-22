/*
 * MultiScaleAnalysis.cpp - Implementation
 */

#include "MultiScaleAnalysis.h"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace opencog {
namespace tensor_logic {

MultiScaleAnalysis::MultiScaleAnalysis(size_t num_scales, double scale_factor)
    : num_scales_(num_scales), scale_factor_(scale_factor) {}

std::vector<float> MultiScaleAnalysis::analyzeAtScale(const std::vector<float>& input, size_t scale_index) {
    if (scale_index >= num_scales_ || input.empty()) {
        return input;
    }

    size_t factor = static_cast<size_t>(std::pow(scale_factor_, scale_index));
    return downsample(input, factor);
}

std::vector<std::vector<float>> MultiScaleAnalysis::decompose(const std::vector<float>& input) {
    std::vector<std::vector<float>> scales;
    scales.reserve(num_scales_);

    std::vector<float> current = input;
    for (size_t s = 0; s < num_scales_; ++s) {
        size_t window = static_cast<size_t>(std::pow(scale_factor_, s));
        auto smoothed = lowPassFilter(current, window);
        scales.push_back(smoothed);

        // Compute residual for next scale
        if (s < num_scales_ - 1) {
            std::vector<float> residual(current.size());
            for (size_t i = 0; i < current.size(); ++i) {
                residual[i] = current[i] - smoothed[i];
            }
            current = residual;
        }
    }

    return scales;
}

std::vector<float> MultiScaleAnalysis::reconstruct(const std::vector<std::vector<float>>& scales) {
    if (scales.empty()) return {};

    std::vector<float> result = scales[0];
    for (size_t s = 1; s < scales.size(); ++s) {
        auto upsampled = upsample(scales[s], static_cast<size_t>(std::pow(scale_factor_, s)));
        for (size_t i = 0; i < result.size() && i < upsampled.size(); ++i) {
            result[i] += upsampled[i];
        }
    }

    return result;
}

std::vector<float> MultiScaleAnalysis::detectPeriodicity(const std::vector<float>& input) {
    // Autocorrelation for periodicity detection
    std::vector<float> autocorr(input.size() / 2);

    float mean = std::accumulate(input.begin(), input.end(), 0.0f) / input.size();

    for (size_t lag = 0; lag < autocorr.size(); ++lag) {
        float sum = 0.0f;
        for (size_t i = 0; i + lag < input.size(); ++i) {
            sum += (input[i] - mean) * (input[i + lag] - mean);
        }
        autocorr[lag] = sum / (input.size() - lag);
    }

    // Normalize
    if (!autocorr.empty() && autocorr[0] > 1e-8f) {
        for (auto& v : autocorr) {
            v /= autocorr[0];
        }
    }

    return autocorr;
}

std::vector<float> MultiScaleAnalysis::extractTrend(const std::vector<float>& input) {
    // Extract low-frequency trend using largest scale
    return lowPassFilter(input, static_cast<size_t>(std::pow(scale_factor_, num_scales_ - 1)));
}

std::vector<float> MultiScaleAnalysis::extractNoise(const std::vector<float>& input) {
    auto trend = extractTrend(input);
    std::vector<float> noise(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        noise[i] = input[i] - trend[i];
    }
    return noise;
}

std::vector<float> MultiScaleAnalysis::downsample(const std::vector<float>& input, size_t factor) {
    if (factor <= 1 || input.empty()) return input;

    std::vector<float> result;
    result.reserve(input.size() / factor + 1);

    for (size_t i = 0; i < input.size(); i += factor) {
        float sum = 0.0f;
        size_t count = 0;
        for (size_t j = i; j < i + factor && j < input.size(); ++j) {
            sum += input[j];
            ++count;
        }
        result.push_back(sum / count);
    }

    return result;
}

std::vector<float> MultiScaleAnalysis::upsample(const std::vector<float>& input, size_t factor) {
    if (factor <= 1 || input.empty()) return input;

    std::vector<float> result;
    result.reserve(input.size() * factor);

    for (size_t i = 0; i < input.size(); ++i) {
        for (size_t j = 0; j < factor; ++j) {
            // Linear interpolation
            if (i + 1 < input.size()) {
                float t = static_cast<float>(j) / factor;
                result.push_back(input[i] * (1.0f - t) + input[i + 1] * t);
            } else {
                result.push_back(input[i]);
            }
        }
    }

    return result;
}

std::vector<float> MultiScaleAnalysis::lowPassFilter(const std::vector<float>& input, size_t window) {
    if (window <= 1 || input.empty()) return input;

    std::vector<float> result(input.size());
    size_t half_window = window / 2;

    for (size_t i = 0; i < input.size(); ++i) {
        float sum = 0.0f;
        size_t count = 0;
        size_t start = (i > half_window) ? i - half_window : 0;
        size_t end = std::min(i + half_window + 1, input.size());

        for (size_t j = start; j < end; ++j) {
            sum += input[j];
            ++count;
        }
        result[i] = sum / count;
    }

    return result;
}

} // namespace tensor_logic
} // namespace opencog
