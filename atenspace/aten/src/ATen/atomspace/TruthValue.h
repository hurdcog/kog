#pragma once

#include <ATen/ATen.h>
#include <cmath>
#include <algorithm>

namespace at {
namespace atomspace {

/**
 * TruthValue - PLN (Probabilistic Logic Networks) truth value formulas
 * 
 * Implements truth value calculations from OpenCog's PLN system using tensors.
 * Truth values are represented as [strength, confidence] pairs where:
 * - Strength: probability estimate (0.0 to 1.0)
 * - Confidence: certainty in the estimate (0.0 to 1.0)
 * 
 * Higher confidence means more evidence supporting the strength value.
 */
class TruthValue {
public:
    // Constants for numerical stability and default parameters
    static constexpr float EPSILON = 0.0001f;
    static constexpr float INDEFINITE_K = 10.0f;  // Controls confidence growth rate
    
    /**
     * Create a truth value tensor [strength, confidence]
     */
    static Tensor create(float strength, float confidence) {
        return torch::tensor({
            std::clamp(strength, 0.0f, 1.0f),
            std::clamp(confidence, 0.0f, 1.0f)
        });
    }
    
    /**
     * Get strength component
     */
    static float getStrength(const Tensor& tv) {
        return tv[0].item<float>();
    }
    
    /**
     * Get confidence component
     */
    static float getConfidence(const Tensor& tv) {
        return tv[1].item<float>();
    }
    
    /**
     * Deduction formula: (A→B, B→C) ⊢ A→C
     * 
     * If A implies B with truth value tv1, and B implies C with truth value tv2,
     * then A implies C with the deduced truth value.
     * 
     * @param tv1 Truth value of A→B
     * @param tv2 Truth value of B→C
     * @return Truth value of A→C
     */
    static Tensor deduction(const Tensor& tv1, const Tensor& tv2) {
        float s1 = getStrength(tv1);
        float c1 = getConfidence(tv1);
        float s2 = getStrength(tv2);
        float c2 = getConfidence(tv2);
        
        // Deduction strength: product of strengths
        float strength = s1 * s2;
        
        // Confidence: geometric mean of confidences, weighted by strengths
        float confidence = (c1 * c2 * (s1 + s2)) / (1.0f + s1 * s2);
        
        return create(strength, confidence);
    }
    
    /**
     * Induction formula: (A, A→B observed multiple times) ⊢ A→B
     * 
     * Generalize from observed instances to a general rule.
     * 
     * @param positiveCount Number of positive instances (A and B both true)
     * @param totalCount Total number of instances of A
     * @return Truth value of the induced A→B
     */
    static Tensor induction(int positiveCount, int totalCount) {
        if (totalCount <= 0) {
            return create(0.5f, 0.0f); // No evidence
        }
        
        // Strength: frequency of positive instances
        float strength = static_cast<float>(positiveCount) / totalCount;
        
        // Confidence: increases with more evidence, asymptotes to 1.0
        // Using Laplace smoothing: (n + 1) / (n + 2)
        float confidence = static_cast<float>(totalCount) / (totalCount + 1.0f);
        confidence = std::min(confidence, 0.99f); // Cap at 0.99
        
        return create(strength, confidence);
    }
    
    /**
     * Abduction formula: (B, A→B) ⊢ A
     * 
     * Given an observation B and a rule A→B, abduce that A might be true.
     * This is "reasoning to best explanation" - less certain than deduction.
     * 
     * @param tvB Truth value of observation B
     * @param tvAB Truth value of rule A→B
     * @return Truth value of abduced A
     */
    static Tensor abduction(const Tensor& tvB, const Tensor& tvAB) {
        float sB = getStrength(tvB);
        float cB = getConfidence(tvB);
        float sAB = getStrength(tvAB);
        float cAB = getConfidence(tvAB);
        
        // Abduction strength: 
        // If B is true and A→B is strong, then A is likely true
        // But weaker than deduction because B could have other causes
        float strength = sB * sAB;
        
        // Confidence is reduced because abduction is less certain
        float confidence = (cB * cAB * sAB) / (1.0f + sB);
        
        return create(strength, confidence * 0.8f); // Discount for uncertainty
    }
    
    /**
     * Revision formula: combine two independent estimates of the same fact
     * 
     * When we have multiple pieces of evidence for the same proposition,
     * combine them to get a more accurate estimate.
     * 
     * @param tv1 First truth value estimate
     * @param tv2 Second truth value estimate
     * @return Revised truth value
     */
    static Tensor revision(const Tensor& tv1, const Tensor& tv2) {
        float s1 = getStrength(tv1);
        float c1 = getConfidence(tv1);
        float s2 = getStrength(tv2);
        float c2 = getConfidence(tv2);
        
        // Weighted average of strengths, weighted by confidence
        float totalConfidence = c1 + c2;
        if (totalConfidence < EPSILON) {
            return create(0.5f, 0.0f);
        }
        
        float strength = (s1 * c1 + s2 * c2) / totalConfidence;
        
        // Combined confidence: higher than either alone, but not simply additive
        float confidence = (c1 * c2) / (c1 + c2 - c1 * c2 + EPSILON);
        confidence = std::min(confidence, 0.99f);
        
        return create(strength, confidence);
    }
    
    /**
     * Conjunction (AND): (A, B) ⊢ A∧B
     * 
     * Truth value of "A AND B" given truth values of A and B.
     * 
     * @param tvA Truth value of A
     * @param tvB Truth value of B
     * @return Truth value of A∧B
     */
    static Tensor conjunction(const Tensor& tvA, const Tensor& tvB) {
        float sA = getStrength(tvA);
        float cA = getConfidence(tvA);
        float sB = getStrength(tvB);
        float cB = getConfidence(tvB);
        
        // Strength: product (both must be true)
        float strength = sA * sB;
        
        // Confidence: product (uncertainty compounds)
        float confidence = cA * cB;
        
        return create(strength, confidence);
    }
    
    /**
     * Disjunction (OR): (A, B) ⊢ A∨B
     * 
     * Truth value of "A OR B" given truth values of A and B.
     * 
     * @param tvA Truth value of A
     * @param tvB Truth value of B
     * @return Truth value of A∨B
     */
    static Tensor disjunction(const Tensor& tvA, const Tensor& tvB) {
        float sA = getStrength(tvA);
        float cA = getConfidence(tvA);
        float sB = getStrength(tvB);
        float cB = getConfidence(tvB);
        
        // Strength: probability that at least one is true
        // P(A∨B) = P(A) + P(B) - P(A∧B)
        float strength = sA + sB - sA * sB;
        
        // Confidence: average confidence
        float confidence = (cA + cB) / 2.0f;
        
        return create(strength, confidence);
    }
    
    /**
     * Negation (NOT): A ⊢ ¬A
     * 
     * Truth value of "NOT A" given truth value of A.
     * 
     * @param tvA Truth value of A
     * @return Truth value of ¬A
     */
    static Tensor negation(const Tensor& tvA) {
        float sA = getStrength(tvA);
        float cA = getConfidence(tvA);
        
        // Strength: complement probability
        float strength = 1.0f - sA;
        
        // Confidence: same as original (we're equally certain)
        float confidence = cA;
        
        return create(strength, confidence);
    }
    
    /**
     * Similarity: measure similarity between two truth values
     * 
     * @param tv1 First truth value
     * @param tv2 Second truth value
     * @return Similarity score (0.0 to 1.0)
     */
    static float similarity(const Tensor& tv1, const Tensor& tv2) {
        float s1 = getStrength(tv1);
        float c1 = getConfidence(tv1);
        float s2 = getStrength(tv2);
        float c2 = getConfidence(tv2);
        
        // Euclidean distance in [strength, confidence] space
        float strengthDiff = s1 - s2;
        float confidenceDiff = c1 - c2;
        float distance = std::sqrt(strengthDiff * strengthDiff + 
                                  confidenceDiff * confidenceDiff);
        
        // Convert to similarity (0 distance = 1 similarity, max distance √2 = 0 similarity)
        return 1.0f - (distance / std::sqrt(2.0f));
    }
    
    /**
     * Implication strength: measure how much A→B is supported
     * 
     * @param tvA Truth value of A
     * @param tvB Truth value of B
     * @return Truth value of A→B
     */
    static Tensor implication(const Tensor& tvA, const Tensor& tvB) {
        float sA = getStrength(tvA);
        float cA = getConfidence(tvA);
        float sB = getStrength(tvB);
        float cB = getConfidence(tvB);
        
        // Material implication: P(A→B) = 1 - P(A) + P(A∧B)
        float strength = 1.0f - sA + sA * sB;
        
        // Confidence: minimum of the two confidences
        float confidence = std::min(cA, cB);
        
        return create(strength, confidence);
    }
    
    /**
     * Higher order: indefinite truth value with count
     * 
     * Represents truth values from a set of observations.
     * 
     * @param positiveCount Number of positive observations
     * @param negativeCount Number of negative observations
     * @return Truth value representing the observations
     */
    static Tensor indefinite(int positiveCount, int negativeCount) {
        int totalCount = positiveCount + negativeCount;
        if (totalCount <= 0) {
            return create(0.5f, 0.0f);
        }
        
        // Strength: ratio of positive to total
        float strength = static_cast<float>(positiveCount) / totalCount;
        
        // Confidence: based on sample size with diminishing returns
        // Using formula: n / (n + k) where k controls confidence growth rate
        float confidence = static_cast<float>(totalCount) / (totalCount + INDEFINITE_K);
        
        return create(strength, confidence);
    }
    
    /**
     * Default truth value (maximum uncertainty)
     */
    static Tensor defaultTV() {
        return create(0.5f, 0.0f);
    }
    
    /**
     * True with high confidence
     */
    static Tensor trueTV() {
        return create(1.0f, 0.9f);
    }
    
    /**
     * False with high confidence
     */
    static Tensor falseTV() {
        return create(0.0f, 0.9f);
    }
};

} // namespace atomspace
} // namespace at
