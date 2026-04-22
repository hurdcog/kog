/**
 * SocialSelf — Implementation
 *
 * Attachment style, social roles, theory of mind, and cultural self-construal.
 */
#include "opencog/entelechy/social.hpp"
#include <algorithm>
#include <cmath>

namespace opencog::entelechy {

void SocialSelf::add_role(SocialRole role) noexcept {
    // Check if role already exists
    for (auto& existing : role_repertoire_) {
        if (existing.name == role.name) {
            existing.competence = std::max(existing.competence, role.competence);
            existing.identification = std::max(existing.identification, role.identification);
            return;
        }
    }
    role_repertoire_.push_back(std::move(role));
}

void SocialSelf::update_role_salience(const std::string& context) noexcept {
    // Simple context-based salience update
    for (auto& role : role_repertoire_) {
        // Decay salience
        role.salience *= 0.95f;

        // Boost if context matches role name (simplified)
        if (!context.empty() && role.name.find(context) != std::string::npos) {
            role.salience = std::min(1.0f, role.salience + 0.3f);
        }
    }
}

void SocialSelf::develop_theory_of_mind(
    float interaction_quality, float dt) noexcept
{
    // Theory of mind develops through social interaction
    float growth = interaction_quality * 0.01f * dt;
    theory_of_mind_depth_ += growth;
    theory_of_mind_depth_ = std::clamp(theory_of_mind_depth_, 0.0f, 1.0f);

    // Social confidence grows with positive interactions
    if (interaction_quality > 0.5f) {
        social_confidence_ += 0.005f * dt;
    } else if (interaction_quality < 0.3f) {
        social_confidence_ -= 0.003f * dt;
    }
    social_confidence_ = std::clamp(social_confidence_, 0.0f, 1.0f);
}

void SocialSelf::update_attachment(
    float security_delta, float anxiety_delta) noexcept
{
    attachment_security_ += security_delta;
    attachment_anxiety_ += anxiety_delta;
    attachment_security_ = std::clamp(attachment_security_, 0.0f, 1.0f);
    attachment_anxiety_ = std::clamp(attachment_anxiety_, 0.0f, 1.0f);

    // Classify attachment style from dimensions
    if (attachment_security_ > 0.5f && attachment_anxiety_ < 0.5f) {
        attachment_style_ = AttachmentStyle::SECURE;
    } else if (attachment_security_ < 0.5f && attachment_anxiety_ > 0.5f) {
        attachment_style_ = AttachmentStyle::ANXIOUS;
    } else if (attachment_security_ < 0.5f && attachment_anxiety_ < 0.5f) {
        attachment_style_ = AttachmentStyle::AVOIDANT;
    } else {
        attachment_style_ = AttachmentStyle::DISORGANIZED;
    }
}

void SocialSelf::update(float dt) noexcept {
    // Decay role saliences
    for (auto& role : role_repertoire_) {
        role.salience *= (1.0f - 0.01f * dt);
    }

    // Cultural self-construal evolves slowly
    // Independent self grows with social confidence
    independent_self_ += (social_confidence_ - 0.5f) * 0.001f * dt;
    independent_self_ = std::clamp(independent_self_, 0.0f, 1.0f);

    // Interdependent self grows with attachment security
    interdependent_self_ += (attachment_security_ - 0.5f) * 0.001f * dt;
    interdependent_self_ = std::clamp(interdependent_self_, 0.0f, 1.0f);
}

float SocialSelf::social_integration() const noexcept {
    float role_diversity = std::min(1.0f,
        static_cast<float>(role_repertoire_.size()) / 5.0f);
    float role_competence = 0.0f;
    for (const auto& role : role_repertoire_) {
        role_competence += role.competence;
    }
    if (!role_repertoire_.empty()) {
        role_competence /= static_cast<float>(role_repertoire_.size());
    }

    return (attachment_security_ * 0.3f + theory_of_mind_depth_ * 0.2f
           + social_confidence_ * 0.2f + role_diversity * 0.15f
           + role_competence * 0.15f);
}

} // namespace opencog::entelechy
