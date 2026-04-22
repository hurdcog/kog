/**
 * SocialSelf — Theory of Mind + Social Identity
 *
 * Implements:
 *   - Attachment style (Bowlby internal working models)
 *   - Social role repertoire
 *   - Theory of mind depth
 *   - Cultural self-construal (Markus & Kitayama)
 *
 * Part of the Reactor Core: arc-halo => cyc-phoenix
 * Copyright 2025-2026 OpenCog Community. AGPL-3.0.
 */
#ifndef OPENCOG_ENTELECHY_SOCIAL_HPP
#define OPENCOG_ENTELECHY_SOCIAL_HPP

#include "opencog/entelechy/types.hpp"
#include <algorithm>

namespace opencog::entelechy {

class SocialSelf {
public:
    SocialSelf() = default;

    /// Add a social role
    void add_role(SocialRole role) noexcept;

    /// Update role salience based on context
    void update_role_salience(const std::string& context) noexcept;

    /// Develop theory of mind from social interaction
    void develop_theory_of_mind(float interaction_quality, float dt) noexcept;

    /// Update attachment style from experience
    void update_attachment(float security_delta, float anxiety_delta) noexcept;

    /// Periodic update
    void update(float dt) noexcept;

    /// Accessors
    [[nodiscard]] AttachmentStyle attachment_style() const noexcept { return attachment_style_; }
    [[nodiscard]] float theory_of_mind_depth() const noexcept { return theory_of_mind_depth_; }
    [[nodiscard]] float social_confidence() const noexcept { return social_confidence_; }
    [[nodiscard]] const std::vector<SocialRole>& roles() const noexcept { return role_repertoire_; }
    [[nodiscard]] float independent_self() const noexcept { return independent_self_; }
    [[nodiscard]] float interdependent_self() const noexcept { return interdependent_self_; }

    /// Social integration metric [0,1]
    [[nodiscard]] float social_integration() const noexcept;

private:
    AttachmentStyle attachment_style_{AttachmentStyle::SECURE};
    float attachment_security_{0.5f};
    float attachment_anxiety_{0.3f};
    std::vector<SocialRole> role_repertoire_;
    float theory_of_mind_depth_{0.0f};
    float social_confidence_{0.5f};
    float independent_self_{0.5f};
    float interdependent_self_{0.5f};
};

} // namespace opencog::entelechy

#endif // OPENCOG_ENTELECHY_SOCIAL_HPP
