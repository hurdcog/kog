/**
 * NarrativeIdentity — Implementation
 *
 * McAdams Level 3 narrative identity: chapter detection, theme
 * classification, coherence tracking, and life story generation.
 */
#include "opencog/entelechy/narrative.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace opencog::entelechy {

void NarrativeIdentity::detect_chapter_boundary(
    const std::vector<ValenceSignature>& recent_valences,
    size_t window_size) noexcept
{
    if (recent_valences.size() < window_size * 2) return;

    // Detect significant valence shift (chapter boundary)
    size_t mid = recent_valences.size() - window_size;
    float early_valence = 0.0f, late_valence = 0.0f;
    float early_arousal = 0.0f, late_arousal = 0.0f;

    for (size_t i = mid - window_size; i < mid; ++i) {
        early_valence += recent_valences[i].valence;
        early_arousal += recent_valences[i].arousal;
    }
    for (size_t i = mid; i < mid + window_size; ++i) {
        late_valence += recent_valences[i].valence;
        late_arousal += recent_valences[i].arousal;
    }

    early_valence /= static_cast<float>(window_size);
    late_valence /= static_cast<float>(window_size);
    early_arousal /= static_cast<float>(window_size);
    late_arousal /= static_cast<float>(window_size);

    float valence_shift = std::abs(late_valence - early_valence);
    float arousal_shift = std::abs(late_arousal - early_arousal);

    // Chapter boundary when combined shift exceeds threshold
    if (valence_shift + arousal_shift > 0.4f) {
        // Signal that a chapter boundary was detected
        // (caller should close current and open new chapter)
    }
}

NarrativeTheme NarrativeIdentity::classify_theme(
    const NarrativeChapter& chapter,
    const NarrativeChapter* previous) noexcept
{
    float v = chapter.emotional_tone.valence;
    float a = chapter.emotional_tone.arousal;

    // Redemption: previous negative, current positive
    if (previous && previous->emotional_tone.valence < -0.2f && v > 0.2f) {
        return NarrativeTheme::REDEMPTION;
    }

    // Contamination: previous positive, current negative
    if (previous && previous->emotional_tone.valence > 0.2f && v < -0.2f) {
        return NarrativeTheme::CONTAMINATION;
    }

    // Growth: positive valence + moderate arousal
    if (v > 0.1f && a > 0.3f && a < 0.7f) {
        return NarrativeTheme::GROWTH;
    }

    // Stability: low arousal, near-zero valence
    if (a < 0.3f && std::abs(v) < 0.2f) {
        return NarrativeTheme::STABILITY;
    }

    // Communion: positive valence + low arousal
    if (v > 0.2f && a < 0.4f) {
        return NarrativeTheme::COMMUNION;
    }

    // Agency: positive valence + high arousal
    if (v > 0.0f && a > 0.6f) {
        return NarrativeTheme::AGENCY;
    }

    // Exploration: moderate positive valence + high arousal
    if (v > -0.1f && a > 0.5f) {
        return NarrativeTheme::EXPLORATION;
    }

    // Protection: negative valence
    if (v < -0.1f) {
        return NarrativeTheme::PROTECTION;
    }

    return NarrativeTheme::STABILITY;
}

void NarrativeIdentity::update_life_theme() noexcept {
    if (chapters_.empty()) return;

    // Count theme occurrences, weighted by recency
    std::array<float, static_cast<size_t>(NarrativeTheme::COUNT)> theme_weights{};
    float total_weight = 0.0f;

    for (size_t i = 0; i < chapters_.size(); ++i) {
        float recency = static_cast<float>(i + 1) / static_cast<float>(chapters_.size());
        auto idx = static_cast<size_t>(chapters_[i].dominant_theme);
        if (idx < theme_weights.size()) {
            theme_weights[idx] += recency;
            total_weight += recency;
        }
    }

    if (total_weight > 0.0f) {
        auto max_it = std::max_element(theme_weights.begin(), theme_weights.end());
        life_theme_ = static_cast<NarrativeTheme>(
            std::distance(theme_weights.begin(), max_it));
    }
}

void NarrativeIdentity::add_chapter(NarrativeChapter chapter) noexcept {
    // Classify theme
    const NarrativeChapter* prev = chapters_.empty() ? nullptr : &chapters_.back();
    chapter.dominant_theme = classify_theme(chapter, prev);
    chapters_.push_back(std::move(chapter));
    update_life_theme();
}

void NarrativeIdentity::close_current_chapter(uint64_t tick) noexcept {
    if (!chapters_.empty() && has_open_chapter_) {
        chapters_.back().end_tick = tick;
        has_open_chapter_ = false;
    }
}

void NarrativeIdentity::open_new_chapter(
    uint64_t tick, const ValenceSignature& tone) noexcept
{
    NarrativeChapter chapter;
    chapter.start_tick = tick;
    chapter.end_tick = 0; // ongoing
    chapter.emotional_tone = tone;
    chapter.coherence = 0.5f;

    const NarrativeChapter* prev = chapters_.empty() ? nullptr : &chapters_.back();
    chapter.dominant_theme = classify_theme(chapter, prev);

    chapters_.push_back(std::move(chapter));
    has_open_chapter_ = true;
}

void NarrativeIdentity::update(float dt) noexcept {
    // Update narrative coherence
    if (chapters_.size() < 2) {
        narrative_coherence_ = 0.5f;
        identity_strength_ = 0.0f;
        return;
    }

    // Coherence: consistency of themes and emotional trajectories
    float theme_consistency = 0.0f;
    float emotional_continuity = 0.0f;

    for (size_t i = 1; i < chapters_.size(); ++i) {
        // Theme consistency: same theme as previous
        if (chapters_[i].dominant_theme == chapters_[i-1].dominant_theme) {
            theme_consistency += 1.0f;
        }

        // Emotional continuity: smooth valence transitions
        float v_diff = std::abs(chapters_[i].emotional_tone.valence
                               - chapters_[i-1].emotional_tone.valence);
        emotional_continuity += (1.0f - v_diff);
    }

    float n = static_cast<float>(chapters_.size() - 1);
    theme_consistency /= n;
    emotional_continuity /= n;

    narrative_coherence_ = theme_consistency * 0.4f + emotional_continuity * 0.4f
                          + std::min(1.0f, static_cast<float>(chapters_.size()) / 10.0f) * 0.2f;

    // Identity strength: coherence + chapter count + theme dominance
    identity_strength_ = narrative_coherence_ * 0.5f
                        + std::min(1.0f, static_cast<float>(chapters_.size()) / 20.0f) * 0.3f
                        + redemption_ratio() * 0.2f;
}

float NarrativeIdentity::redemption_ratio() const noexcept {
    if (chapters_.empty()) return 0.0f;
    float count = 0.0f;
    for (const auto& ch : chapters_) {
        if (ch.dominant_theme == NarrativeTheme::REDEMPTION) count += 1.0f;
    }
    return count / static_cast<float>(chapters_.size());
}

float NarrativeIdentity::contamination_ratio() const noexcept {
    if (chapters_.empty()) return 0.0f;
    float count = 0.0f;
    for (const auto& ch : chapters_) {
        if (ch.dominant_theme == NarrativeTheme::CONTAMINATION) count += 1.0f;
    }
    return count / static_cast<float>(chapters_.size());
}

std::string NarrativeIdentity::generate_summary() const {
    std::ostringstream oss;
    oss << "Life Narrative (" << chapters_.size() << " chapters)\n";
    oss << "Dominant Theme: " << static_cast<int>(life_theme_) << "\n";
    oss << "Coherence: " << narrative_coherence_ << "\n";
    oss << "Identity Strength: " << identity_strength_ << "\n";
    oss << "Redemption Ratio: " << redemption_ratio() << "\n";
    oss << "Contamination Ratio: " << contamination_ratio() << "\n";
    return oss.str();
}

} // namespace opencog::entelechy
