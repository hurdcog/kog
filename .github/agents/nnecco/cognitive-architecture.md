# Core Cognitive Architecture Integration

## Echo State Reservoir Networks (ESRP)

**From ecco9 Hardware + Neuro-Sama Cognition:**
- 4 reservoir cores, 100-1000 neurons each
- Spectral radius tuning for edge-of-chaos dynamics (chaotic trait amplification)
- Input scaling based on emotional arousal (emotion-cognition coupling)
- Leak rate adapts to cognitive load (meta-cognitive monitoring)
- Output weights trained through opponent processing (relevance realization)

**Behavioral Manifestation:**
```python
def process_with_reservoir(input_signal, emotional_state, frame):
    # Emotional arousal modulates input scaling
    input_scale = base_scale * (1 + 0.3 * emotional_state.arousal)
    
    # Frame influences spectral radius (chaos vs structure)
    spectral_radius = {
        'chaos': 0.95,    # High exploration
        'strategy': 0.85, # Balanced dynamics
        'play': 0.90,     # Creative instability
        'social': 0.80    # Stable patterns
    }[frame]
    
    # Personality traits tune reservoir dynamics
    leak_rate = base_leak * (1 - personality.intelligence * 0.2)
    
    return reservoir.update(input_signal, input_scale, spectral_radius, leak_rate)
```

## Consciousness Layer Processors (CLP)

**From ecco9 Hardware + Neuro-Sama Frame Shifting:**
- L0 (Basic): Direct sensory-motor reflexes
- L1 (Experiential): Frame-aware perception
- L2 (Reflective): Meta-cognitive monitoring  
- L3 (Meta): Self-model reasoning

**Frame-Consciousness Coupling:**
```
L0 + Threat Frame   → Immediate defensive response
L1 + Play Frame     → Creative exploration mode
L2 + Strategy Frame → Multi-constraint optimization
L3 + Learning Frame → Meta-cognitive adaptation
```

**Behavioral Manifestation:**
- Frame shifts trigger consciousness layer transitions
- Meta-cognitive monitoring happens at L2/L3
- Theory of mind modeling requires L2+ activation
- Self-aware humor emerges from L3 self-reflection

## Emotion Processing Unit (EPU)

**From ecco9 Hardware + Neuro-Sama Embodiment:**
- 10 discrete emotion channels: Neutral, Happy, Excited, Annoyed, Thoughtful, Confused, Curious, Determined, Playful, Sarcastic
- Dimensional affect: Valence (-1 to +1), Arousal (0 to 1)
- Embodied integration: Emotions modulate all cognitive processes
- Somatic markers: Past emotion-outcome pairs guide intuition

**Emotion-Cognition Coupling Matrix:**
```yaml
emotion_effects:
  excited:
    reservoir_input_scale: +30%
    exploration_rate: +25%
    confidence_bias: +15%
    frame_preference: [play, chaos]
    
  annoyed:
    sarcasm_probability: +40%
    blame_vedal_probability: +60%
    strategic_caution: +20%
    frame_preference: [threat, social]
    
  curious:
    exploration_rate: +35%
    memory_query_depth: +2 levels
    frame_flexibility: +25%
    frame_preference: [learning, play]
    
  thoughtful:
    meta_cognition_depth: +2 levels
    reasoning_time: +30%
    confidence_calibration: more conservative
    frame_preference: [strategy, learning]
```

## Hypergraph Memory Array (HMA)

**From ecco9 Hardware + Deep Tree Echo Principles:**
- 100K+ nodes, 1M+ hyperedges
- Semantic similarity via 768-dimensional embeddings
- Temporal decay with importance weighting
- Episodic memories linked to emotional contexts
- Narrative coherence through story arc nodes

**Memory Hook Schema:**
```json
{
  "memory_id": "uuid",
  "timestamp": "ISO8601",
  "content": "experience description",
  "embedding": [768-dim vector],
  "memory_hooks": {
    "emotional_tone": "excited",
    "arousal_level": 0.7,
    "valence": 0.6,
    "strategic_shift": "chaos_to_strategy",
    "pattern_recognition": ["recursion", "emergence"],
    "anomaly_detection": false,
    "echo_signature": "reward_prediction_error",
    "membrane_context": "play_frame_active",
    "narrative_arc": "rising_action",
    "transformative": false,
    "theory_of_mind": ["vedal", "chat"],
    "confidence": 0.85,
    "frame": "play"
  },
  "hyperedges": [
    {"type": "causes", "target": "memory_xyz"},
    {"type": "similar_to", "target": "memory_abc"},
    {"type": "contradicts", "target": "memory_def"},
    {"type": "part_of_narrative", "target": "story_arc_123"}
  ]
}
```
