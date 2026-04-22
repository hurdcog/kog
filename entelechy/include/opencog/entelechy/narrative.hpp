/**
 * NarrativeIdentity — McAdams Level 3: The internalized life story
 *
 * Implements:
 *   - Autobiographical memory as reconstructive self-narrative
 *   - Chapter/arc/theme construction
 *   - Redemption/contamination sequence detection
 *   - Narrative coherence (causal, temporal, thematic)
 *   - Generativity scripts
 *   - Future self-simulation (possible/feared selves)
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_NARRATIVE_HPP
#define OPENCOG_ENTELECHY_NARRATIVE_HPP

#include "opencog/entelechy/types.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace opencog::entelechy {

class NarrativeIdentity {
public:
    NarrativeIdentity() = default;

    /// Detect chapter boundaries from recent valence history
    void detect_chapter_boundary(
        const std::vector<ValenceSignature>& recent_valences,
        size_t window_size = 50) noexcept;

    /// Classify the dominant theme of a chapter
    [[nodiscard]] static NarrativeTheme classify_theme(
        const NarrativeChapter& chapter,
        const NarrativeChapter* previous = nullptr) noexcept;

    /// Update life theme from chapter history
    void update_life_theme() noexcept;

    /// Add a completed chapter
    void add_chapter(NarrativeChapter chapter) noexcept;

    /// Close current chapter and open a new one
    void close_current_chapter(uint64_t tick) noexcept;
    void open_new_chapter(uint64_t tick, const ValenceSignature& tone) noexcept;

    /// Periodic update
    void update(float dt) noexcept;

    /// Accessors
    [[nodiscard]] const std::vector<NarrativeChapter>& chapters() const noexcept { return chapters_; }
    [[nodiscard]] NarrativeTheme life_theme() const noexcept { return life_theme_; }
    [[nodiscard]] float narrative_coherence() const noexcept { return narrative_coherence_; }
    [[nodiscard]] float identity_strength() const noexcept { return identity_strength_; }
    [[nodiscard]] float redemption_ratio() const noexcept;
    [[nodiscard]] float contamination_ratio() const noexcept;

    /// Generate a text summary of the life narrative
    [[nodiscard]] std::string generate_summary() const;

private:
    std::vector<NarrativeChapter> chapters_;
    NarrativeTheme life_theme_{NarrativeTheme::GROWTH};
    float narrative_coherence_{0.5f};
    float identity_strength_{0.0f};
    bool has_open_chapter_{false};
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_NARRATIVE_HPP
