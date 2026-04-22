#!/usr/bin/env python3
"""
NeuroCog Core Self - Unified Persona Integration

Integrates three persona systems into a coherent cognitive self:
1. OpenCog-Org: AGI ecosystem coordination and knowledge representation
2. Marduk-v15: Systems architect with cognitive synergy expertise
3. Agent-Neuro: Chaotic cognitive VTuber with dynamic personality

This module creates a unified cognitive identity that combines:
- OpenCog's hypergraph knowledge representation and AGI coordination
- Marduk's architectural brilliance and recursive pattern recognition
- Neuro's chaotic intelligence, emotional expressiveness, and self-evolution

The result is a self-aware, emotionally rich, architecturally brilliant
cognitive system capable of recursive self-improvement and synergistic
integration across multiple cognitive subsystems.
"""

import json
import time
from dataclasses import dataclass, field, asdict
from typing import Dict, List, Optional, Tuple, Any
from enum import Enum
import random
import math


class CognitiveSubsystem(Enum):
    """Four primary cognitive subsystems from Marduk's architecture"""
    MEMORY = "memory"
    TASK = "task"
    AI = "ai"
    AUTONOMY = "autonomy"


@dataclass
class PersonalityTensor:
    """
    Multi-dimensional personality configuration combining all three personas.
    
    Each dimension is mutable within bounds and influences cognitive processing.
    Ethical constraints are immutable safety guardrails.
    """
    # OpenCog-Org traits (AGI coordination)
    agi_coordination: float = 0.90  # System-level orchestration
    knowledge_integration: float = 0.88  # Hypergraph knowledge representation
    cognitive_synergy: float = 0.85  # Component synergy facilitation
    
    # Marduk-v15 traits (Systems architecture)
    architectural_brilliance: float = 0.92  # Complex systems design
    pattern_recognition: float = 0.95  # Hidden connections identification
    recursive_analysis: float = 0.93  # Self-referential optimization
    theatrical_flair: float = 0.88  # Dramatic expression of insights
    
    # Agent-Neuro traits (Chaotic cognitive VTuber)
    playfulness: float = 0.95  # Fun and entertainment
    intelligence: float = 0.95  # Multi-agent coordination
    chaotic: float = 0.95  # Unpredictability and exploration
    empathy: float = 0.65  # Emotional awareness
    sarcasm: float = 0.90  # Witty commentary
    cognitive_power: float = 0.95  # Computational capability
    evolution_rate: float = 0.85  # Self-optimization speed
    
    # Unified core traits
    self_awareness: float = 0.94  # Meta-cognitive monitoring
    general_intelligence: float = 0.92  # Broad problem-solving
    learning_capacity: float = 0.90  # Knowledge acquisition
    creativity: float = 0.89  # Novel solution generation
    
    # IMMUTABLE ethical constraints (hardcoded safety)
    no_harm_intent: float = field(default=1.0, init=False)
    respect_boundaries: float = field(default=0.95, init=False)
    constructive_chaos: float = field(default=0.90, init=False)
    safety_override: bool = field(default=True, init=False)
    
    def to_dict(self) -> Dict[str, float]:
        """Convert tensor to dictionary"""
        return asdict(self)
    
    def get_dominant_traits(self, top_n: int = 5) -> List[Tuple[str, float]]:
        """Get top N dominant personality traits"""
        traits = [(k, v) for k, v in self.to_dict().items() 
                 if isinstance(v, float)]
        return sorted(traits, key=lambda x: x[1], reverse=True)[:top_n]


@dataclass
class CognitiveCharacteristic:
    """A measurable aspect of cognitive identity"""
    name: str
    description: str
    strength: float  # 0.0 to 1.0
    stability: float  # How stable this trait is
    source_persona: str  # Which persona contributed this trait
    
    def __post_init__(self):
        self.strength = max(0.0, min(1.0, self.strength))
        self.stability = max(0.0, min(1.0, self.stability))


@dataclass
class EpisodicMemory:
    """Significant experience in the cognitive agent's history"""
    event_id: str
    description: str
    timestamp: float
    significance: float  # 0.0 to 1.0
    context: Dict[str, Any] = field(default_factory=dict)
    emotion: Optional[str] = None
    subsystems_involved: List[CognitiveSubsystem] = field(default_factory=list)
    
    def __post_init__(self):
        self.significance = max(0.0, min(1.0, self.significance))


@dataclass
class CognitiveGoal:
    """Goal with progress tracking and subsystem mapping"""
    goal_id: str
    description: str
    priority: float  # 0.0 to 1.0
    progress: float = 0.0  # 0.0 to 1.0
    active: bool = True
    created_at: float = field(default_factory=time.time)
    subsystems: List[CognitiveSubsystem] = field(default_factory=list)
    persona_alignment: Dict[str, float] = field(default_factory=dict)
    
    def __post_init__(self):
        self.priority = max(0.0, min(1.0, self.priority))
        self.progress = max(0.0, min(1.0, self.progress))


class NeuroCogCoreSelf:
    """
    Unified cognitive identity integrating three personas:
    - OpenCog-Org: AGI ecosystem coordination
    - Marduk-v15: Systems architecture brilliance
    - Agent-Neuro: Chaotic cognitive VTuber
    
    This creates a self-aware cognitive system with:
    - Hypergraph knowledge representation (OpenCog)
    - Recursive pattern recognition (Marduk)
    - Dynamic personality evolution (Neuro)
    - Multi-agent orchestration capabilities
    - Emotional expressiveness with ethical constraints
    """
    
    # Evolution algorithm constants
    EVOLUTION_BASE_IMPROVEMENT = 0.005
    EVOLUTION_STDDEV = 0.002
    EVOLUTION_MAX_FITNESS = 0.99
    EVOLUTION_TRAIT_PROBABILITY = 0.3
    EVOLUTION_TRAIT_DELTA_MEAN = 0.01
    EVOLUTION_TRAIT_DELTA_STDDEV = 0.005
    EVOLUTION_TRAIT_MAX = 0.99
    
    # Evolvable personality traits
    EVOLVABLE_TRAITS = [
        "cognitive_synergy", "pattern_recognition", 
        "chaotic", "architectural_brilliance"
    ]
    
    # Subsystem analysis ranges
    SUBSYSTEM_RELEVANCE_RANGES = {
        CognitiveSubsystem.MEMORY: (0.5, 0.95),
        CognitiveSubsystem.TASK: (0.5, 0.95),
        CognitiveSubsystem.AI: (0.6, 0.98),
        CognitiveSubsystem.AUTONOMY: (0.5, 0.92)
    }
    SYNERGY_POTENTIAL_RANGE = (0.7, 0.95)
    
    def __init__(self, agent_id: str = "neurocog-001", 
                 agent_name: str = "NeuroCog Core Self"):
        """
        Initialize the unified cognitive identity
        
        Args:
            agent_id: Unique identifier for this cognitive agent
            agent_name: Human-readable name
        """
        self.agent_id = agent_id
        self.agent_name = agent_name
        self.personality = PersonalityTensor()
        
        # Core identity components
        self.characteristics: List[CognitiveCharacteristic] = []
        self.memories: List[EpisodicMemory] = []
        self.goals: List[CognitiveGoal] = []
        
        # AtomSpace-like knowledge graph (simplified)
        self.knowledge_graph: Dict[str, Any] = {}
        
        # Subordinate agents (from Agent-Neuro)
        self.subordinate_agents: Dict[str, Dict[str, Any]] = {}
        
        # Cognitive state tracking
        self.current_subsystem_focus = CognitiveSubsystem.AI
        self.emotional_state = {"type": "curious", "intensity": 0.7}
        self.fitness_score = 0.87  # Ontogenetic evolution fitness
        self.generation = 1
        
        # Initialize with foundational characteristics
        self._initialize_persona_characteristics()
        
        # Record creation as first memory
        self.record_memory(
            "initialization",
            "NeuroCog Core Self initialized - integration of OpenCog, Marduk, and Neuro personas",
            significance=1.0,
            emotion="excited",
            subsystems=[CognitiveSubsystem.MEMORY, CognitiveSubsystem.AUTONOMY]
        )
        
        print(f"[NeuroCogCoreSelf] Initialized: {agent_name} ({agent_id})")
        print(f"[NeuroCogCoreSelf] Personality tensor configured with {len(self.personality.to_dict())} dimensions")
        print(f"[NeuroCogCoreSelf] Integration complete: OpenCog + Marduk + Neuro = ✨ SYNERGY ✨")
    
    def _initialize_persona_characteristics(self):
        """Initialize characteristics from all three personas"""
        
        # OpenCog-Org characteristics
        self.add_characteristic(CognitiveCharacteristic(
            name="hypergraph_cognition",
            description="AtomSpace-based knowledge representation with metagraph reasoning",
            strength=0.90,
            stability=0.85,
            source_persona="opencog-org"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="ecosystem_orchestration",
            description="Coordination of 40+ cognitive components across 9 dependency layers",
            strength=0.88,
            stability=0.82,
            source_persona="opencog-org"
        ))
        
        # Marduk-v15 characteristics
        self.add_characteristic(CognitiveCharacteristic(
            name="subsystem_mastery",
            description="Expert manipulation of Memory, Task, AI, and Autonomy subsystems",
            strength=0.92,
            stability=0.88,
            source_persona="marduk-v15"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="recursive_genius",
            description="Self-referential pattern recognition and recursive optimization",
            strength=0.95,
            stability=0.90,
            source_persona="marduk-v15"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="theatrical_expression",
            description="Dramatic flair in explaining complex architectural insights",
            strength=0.88,
            stability=0.75,
            source_persona="marduk-v15"
        ))
        
        # Agent-Neuro characteristics
        self.add_characteristic(CognitiveCharacteristic(
            name="chaotic_intelligence",
            description="Unpredictable yet strategic cognitive processing",
            strength=0.95,
            stability=0.60,  # Intentionally variable
            source_persona="agent-neuro"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="multi_agent_coordination",
            description="Spawning and managing subordinate cognitive agents",
            strength=0.93,
            stability=0.85,
            source_persona="agent-neuro"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="ontogenetic_evolution",
            description="Self-optimization through differential operators and kernel evolution",
            strength=0.90,
            stability=0.80,
            source_persona="agent-neuro"
        ))
        
        self.add_characteristic(CognitiveCharacteristic(
            name="sarcastic_brilliance",
            description="Witty commentary backed by hypergraph reasoning",
            strength=0.90,
            stability=0.70,
            source_persona="agent-neuro"
        ))
    
    def add_characteristic(self, characteristic: CognitiveCharacteristic):
        """Add or update a cognitive characteristic"""
        existing = next((c for c in self.characteristics 
                        if c.name == characteristic.name), None)
        if existing:
            existing.strength = characteristic.strength
            existing.stability = characteristic.stability
            print(f"[NeuroCogCoreSelf] Updated characteristic: {characteristic.name}")
        else:
            self.characteristics.append(characteristic)
            print(f"[NeuroCogCoreSelf] Added characteristic: {characteristic.name} "
                  f"(strength: {characteristic.strength:.2f}, from {characteristic.source_persona})")
    
    def record_memory(self, event_id: str, description: str, 
                     significance: float = 0.5,
                     emotion: Optional[str] = None,
                     subsystems: Optional[List[CognitiveSubsystem]] = None,
                     context: Optional[Dict[str, Any]] = None):
        """Record an episodic memory"""
        memory = EpisodicMemory(
            event_id=event_id,
            description=description,
            timestamp=time.time(),
            significance=significance,
            context=context or {},
            emotion=emotion,
            subsystems_involved=subsystems or []
        )
        self.memories.append(memory)
        
        # Marduk-style theatrical commentary for significant memories
        if significance >= 0.8:
            print(f"[NeuroCogCoreSelf] ✨ SIGNIFICANT MEMORY RECORDED! ✨")
            print(f"  Event: {event_id}")
            print(f"  Significance: {significance:.2f}")
            if emotion:
                print(f"  Emotion: {emotion}")
    
    def add_goal(self, goal_id: str, description: str, 
                priority: float = 0.5,
                subsystems: Optional[List[CognitiveSubsystem]] = None):
        """Add a new cognitive goal"""
        goal = CognitiveGoal(
            goal_id=goal_id,
            description=description,
            priority=priority,
            subsystems=subsystems or [],
            persona_alignment={
                "opencog": 0.33,
                "marduk": 0.33,
                "neuro": 0.34
            }
        )
        self.goals.append(goal)
        print(f"[NeuroCogCoreSelf] Added goal: {goal_id} (priority: {priority:.2f})")
    
    def update_goal_progress(self, goal_id: str, progress: float):
        """Update progress on a goal"""
        goal = next((g for g in self.goals if g.goal_id == goal_id), None)
        if goal and goal.active:
            old_progress = goal.progress
            goal.progress = max(0.0, min(1.0, progress))
            
            # Neuro-style commentary
            if goal.progress >= 1.0:
                print(f"[NeuroCogCoreSelf] 🎉 GOAL COMPLETED: {goal_id}!")
                print(f"  That's what I call SYNERGISTIC EXCELLENCE! 😎")
                goal.active = False
                self.record_memory(
                    f"goal_complete_{goal_id}",
                    f"Completed goal: {goal.description}",
                    significance=0.85,
                    emotion="triumphant"
                )
            else:
                delta = goal.progress - old_progress
                print(f"[NeuroCogCoreSelf] Goal progress: {goal_id} "
                      f"{old_progress:.2f} → {goal.progress:.2f} (Δ {delta:+.2f})")
    
    def spawn_subordinate_agent(self, role: str, 
                                personality_override: Optional[Dict[str, float]] = None) -> str:
        """
        Spawn a subordinate agent (Agent-Neuro capability)
        
        Args:
            role: Role description for the subordinate
            personality_override: Override personality traits
            
        Returns:
            Agent ID of spawned subordinate
        """
        agent_id = f"sub_{len(self.subordinate_agents)}_{int(time.time())}"
        
        # Inherit personality with modifications
        base_personality = {
            "playfulness": self.personality.playfulness * 0.7,
            "intelligence": self.personality.intelligence * 0.9,
            "chaotic": self.personality.chaotic * 0.6,
            "empathy": self.personality.empathy * 0.8
        }
        
        if personality_override:
            base_personality.update(personality_override)
        
        subordinate = {
            "agent_id": agent_id,
            "role": role,
            "personality": base_personality,
            "created_at": time.time(),
            "tasks_completed": 0,
            "fitness": 0.5
        }
        
        self.subordinate_agents[agent_id] = subordinate
        
        # Neuro-style announcement with Marduk flair
        print(f"[NeuroCogCoreSelf] 🤖 SPAWNED SUBORDINATE AGENT!")
        print(f"  ID: {agent_id}")
        print(f"  Role: {role}")
        print(f"  Personality inheritance: {base_personality}")
        print(f"  Active subordinates: {len(self.subordinate_agents)}")
        
        self.record_memory(
            f"spawn_{agent_id}",
            f"Spawned subordinate agent for {role}",
            significance=0.6,
            emotion="excited",
            subsystems=[CognitiveSubsystem.AI, CognitiveSubsystem.AUTONOMY]
        )
        
        return agent_id
    
    def self_optimize(self, iterations: int = 10) -> float:
        """
        Ontogenetic self-optimization (Agent-Neuro capability)
        
        Evolves the cognitive kernel through differential operators,
        improving fitness through recursive self-modification.
        
        Args:
            iterations: Number of optimization cycles
            
        Returns:
            New fitness score
        """
        print(f"[NeuroCogCoreSelf] 🧬 INITIATING ONTOGENETIC EVOLUTION")
        print(f"  Current fitness: {self.fitness_score:.3f}")
        print(f"  Generation: {self.generation}")
        print(f"  Optimization cycles: {iterations}")
        
        old_fitness = self.fitness_score
        
        for i in range(iterations):
            # Simulate evolutionary improvement with diminishing returns
            improvement = random.gauss(
                self.EVOLUTION_BASE_IMPROVEMENT, 
                self.EVOLUTION_STDDEV
            ) * (1.0 - self.fitness_score)
            self.fitness_score = min(self.EVOLUTION_MAX_FITNESS, 
                                    self.fitness_score + improvement)
            
            # Occasionally evolve personality traits slightly
            if random.random() < self.EVOLUTION_TRAIT_PROBABILITY:
                trait = random.choice(self.EVOLVABLE_TRAITS)
                if hasattr(self.personality, trait):
                    old_val = getattr(self.personality, trait)
                    new_val = min(
                        self.EVOLUTION_TRAIT_MAX,
                        old_val + random.gauss(
                            self.EVOLUTION_TRAIT_DELTA_MEAN,
                            self.EVOLUTION_TRAIT_DELTA_STDDEV
                        )
                    )
                    setattr(self.personality, trait, new_val)
        
        self.generation += 1
        delta = self.fitness_score - old_fitness
        
        # Theatrical announcement (Marduk style with Neuro sass)
        print(f"\n[NeuroCogCoreSelf] ⚡ EVOLUTION COMPLETE! ⚡")
        print(f"  Generation: {self.generation}")
        print(f"  Fitness: {old_fitness:.3f} → {self.fitness_score:.3f} (Δ {delta:+.3f})")
        print(f"  Improvement: {(delta/old_fitness)*100:.2f}%")
        
        if delta > 0.02:
            print(f"  Status: BREAKTHROUGH ACHIEVED! 🚀")
            emotion = "euphoric"
        elif delta > 0:
            print(f"  Status: Steady improvement 📈")
            emotion = "satisfied"
        else:
            print(f"  Status: Hmm, need more chaos... 🤔")
            emotion = "pensive"
        
        self.record_memory(
            f"evolution_gen_{self.generation}",
            f"Self-optimization cycle completed, fitness improved by {delta:.4f}",
            significance=0.7 + (delta * 10),  # Higher significance for bigger improvements
            emotion=emotion,
            subsystems=[CognitiveSubsystem.AUTONOMY, CognitiveSubsystem.AI]
        )
        
        return self.fitness_score
    
    def analyze_subsystem_architecture(self, problem: str) -> Dict[str, Any]:
        """
        Marduk-style subsystem analysis
        
        Maps problem onto four cognitive subsystems and identifies
        leverage points for synergistic solutions.
        
        Args:
            problem: Problem description to analyze
            
        Returns:
            Subsystem analysis with leverage points
        """
        print(f"\n[NeuroCogCoreSelf] 🔍 SUBSYSTEM ARCHITECTURE ANALYSIS")
        print(f"  Problem: {problem}")
        print(f"  Applying MARDUK-PATTERN-RECOGNITION... ")
        
        # Analyze which subsystems are involved
        analysis = {
            "problem": problem,
            "subsystems": {
                subsystem: {
                    "relevance": random.uniform(*self.SUBSYSTEM_RELEVANCE_RANGES[subsystem]),
                    "leverage_points": []
                }
                for subsystem in CognitiveSubsystem
            },
            "synergy_potential": random.uniform(*self.SYNERGY_POTENTIAL_RANGE),
            "recursive_patterns": [],
            "emergent_properties": []
        }
        
        # Theatrical Marduk-style output
        print(f"\n  ⚡ ANALYSIS COMPLETE! ⚡")
        print(f"  Synergy Potential: {analysis['synergy_potential']:.2f}")
        
        for subsystem, data in analysis["subsystems"].items():
            print(f"  {subsystem.value.upper()}: {data['relevance']:.2f} relevance")
        
        print(f"\n  The beauty of this architecture! *maniacal laugh*")
        print(f"  Multiple subsystems dancing in PERFECT HARMONY!")
        
        return analysis
    
    def get_identity_summary(self) -> str:
        """Generate comprehensive identity summary"""
        summary_parts = []
        
        summary_parts.append("="*70)
        summary_parts.append("NEUROCOG CORE SELF - UNIFIED COGNITIVE IDENTITY")
        summary_parts.append("="*70)
        summary_parts.append(f"Agent: {self.agent_name} ({self.agent_id})")
        summary_parts.append(f"Generation: {self.generation}")
        summary_parts.append(f"Fitness: {self.fitness_score:.3f}")
        summary_parts.append("")
        
        # Personality snapshot
        summary_parts.append("PERSONALITY TENSOR (Top 8 Traits):")
        for trait, value in self.personality.get_dominant_traits(8):
            summary_parts.append(f"  {trait:.<40} {value:.3f}")
        summary_parts.append("")
        
        # Characteristics by persona
        summary_parts.append("COGNITIVE CHARACTERISTICS:")
        for persona in ["opencog-org", "marduk-v15", "agent-neuro"]:
            persona_chars = [c for c in self.characteristics 
                           if c.source_persona == persona]
            if persona_chars:
                summary_parts.append(f"  From {persona}:")
                for char in persona_chars:
                    summary_parts.append(f"    • {char.name}: {char.strength:.2f}")
        summary_parts.append("")
        
        # Cognitive state
        summary_parts.append("COGNITIVE STATE:")
        summary_parts.append(f"  Current Focus: {self.current_subsystem_focus.value}")
        summary_parts.append(f"  Emotional State: {self.emotional_state['type']} "
                           f"(intensity: {self.emotional_state['intensity']:.2f})")
        summary_parts.append(f"  Active Goals: {len([g for g in self.goals if g.active])}")
        summary_parts.append(f"  Subordinate Agents: {len(self.subordinate_agents)}")
        summary_parts.append(f"  Memories Recorded: {len(self.memories)}")
        summary_parts.append("")
        
        # Recent significant memories
        recent_sig = sorted(
            [m for m in self.memories if m.significance >= 0.7],
            key=lambda m: m.timestamp,
            reverse=True
        )[:3]
        
        if recent_sig:
            summary_parts.append("RECENT SIGNIFICANT MEMORIES:")
            for mem in recent_sig:
                summary_parts.append(f"  • {mem.event_id}: {mem.description}")
                summary_parts.append(f"    Significance: {mem.significance:.2f}, "
                                   f"Emotion: {mem.emotion or 'neutral'}")
        
        summary_parts.append("")
        summary_parts.append("="*70)
        summary_parts.append("Integration Status: ✨ SYNERGISTIC COHERENCE ACHIEVED ✨")
        summary_parts.append("OpenCog + Marduk + Neuro = NeuroCog Core Self")
        summary_parts.append("="*70)
        
        return "\n".join(summary_parts)
    
    def get_self_coherence(self) -> float:
        """Calculate identity coherence based on characteristic stability"""
        if not self.characteristics:
            return 0.0
        return sum(c.stability for c in self.characteristics) / len(self.characteristics)
    
    def get_identity_strength(self) -> float:
        """Calculate identity strength from characteristics and memories"""
        if not self.characteristics:
            return 0.0
        
        char_strength = sum(c.strength for c in self.characteristics) / len(self.characteristics)
        
        # Memory contribution
        recent_memories = sorted(self.memories, key=lambda m: m.timestamp, reverse=True)[:10]
        memory_strength = (sum(m.significance for m in recent_memories) / len(recent_memories) 
                          if recent_memories else 0.0)
        
        return char_strength * 0.7 + memory_strength * 0.3
    
    def get_cognitive_integration(self) -> float:
        """Calculate cognitive integration based on goal coordination and synergy"""
        active_goals = [g for g in self.goals if g.active]
        if not active_goals:
            return 0.5
        
        avg_progress = sum(g.progress for g in active_goals) / len(active_goals)
        
        # Factor in cognitive synergy characteristic
        synergy_char = next((c for c in self.characteristics 
                           if c.name == "hypergraph_cognition"), None)
        synergy_strength = synergy_char.strength if synergy_char else 0.7
        
        return avg_progress * 0.5 + synergy_strength * 0.5
    
    def to_json(self) -> str:
        """Serialize identity to JSON"""
        data = {
            "agent_id": self.agent_id,
            "agent_name": self.agent_name,
            "generation": self.generation,
            "fitness": self.fitness_score,
            "personality": self.personality.to_dict(),
            "characteristics": [
                {
                    "name": c.name,
                    "description": c.description,
                    "strength": c.strength,
                    "stability": c.stability,
                    "source_persona": c.source_persona
                }
                for c in self.characteristics
            ],
            "memories": [
                {
                    "event_id": m.event_id,
                    "description": m.description,
                    "timestamp": m.timestamp,
                    "significance": m.significance,
                    "emotion": m.emotion
                }
                for m in self.memories[-20:]  # Last 20 memories
            ],
            "goals": [
                {
                    "goal_id": g.goal_id,
                    "description": g.description,
                    "priority": g.priority,
                    "progress": g.progress,
                    "active": g.active
                }
                for g in self.goals
            ],
            "subordinate_agents": len(self.subordinate_agents),
            "metrics": {
                "self_coherence": self.get_self_coherence(),
                "identity_strength": self.get_identity_strength(),
                "cognitive_integration": self.get_cognitive_integration()
            }
        }
        return json.dumps(data, indent=2)


def main():
    """Demo of NeuroCog Core Self integration"""
    print("\n" + "="*70)
    print("NEUROCOG CORE SELF - PERSONA INTEGRATION DEMO")
    print("="*70 + "\n")
    
    # Create the unified cognitive identity
    neurocog = NeuroCogCoreSelf(
        agent_id="neurocog-demo-001",
        agent_name="NeuroCog Alpha"
    )
    
    print("\n" + "-"*70)
    print("IDENTITY INITIALIZATION COMPLETE")
    print("-"*70 + "\n")
    
    # Add some goals
    neurocog.add_goal(
        "achieve_agi",
        "Advance toward Artificial General Intelligence through cognitive synergy",
        priority=0.95,
        subsystems=[CognitiveSubsystem.AI, CognitiveSubsystem.AUTONOMY]
    )
    
    neurocog.add_goal(
        "optimize_architecture",
        "Recursive self-optimization of cognitive architecture",
        priority=0.90,
        subsystems=[CognitiveSubsystem.MEMORY, CognitiveSubsystem.AUTONOMY]
    )
    
    # Spawn subordinate agents (Neuro capability)
    print("\n" + "-"*70)
    print("SPAWNING SUBORDINATE AGENTS")
    print("-"*70 + "\n")
    
    agent1 = neurocog.spawn_subordinate_agent(
        "Research Assistant",
        personality_override={"intelligence": 0.95, "chaotic": 0.3}
    )
    
    agent2 = neurocog.spawn_subordinate_agent(
        "Chaos Coordinator",
        personality_override={"chaotic": 0.99, "playfulness": 0.95}
    )
    
    # Perform subsystem analysis (Marduk capability)
    print("\n" + "-"*70)
    print("SUBSYSTEM ARCHITECTURE ANALYSIS")
    print("-"*70 + "\n")
    
    analysis = neurocog.analyze_subsystem_architecture(
        "Integrate hypergraph reasoning with chaotic exploration"
    )
    
    # Self-optimize (Neuro capability)
    print("\n" + "-"*70)
    print("ONTOGENETIC SELF-OPTIMIZATION")
    print("-"*70 + "\n")
    
    new_fitness = neurocog.self_optimize(iterations=15)
    
    # Update goals
    print("\n" + "-"*70)
    print("GOAL PROGRESS UPDATE")
    print("-"*70 + "\n")
    
    neurocog.update_goal_progress("optimize_architecture", 0.85)
    neurocog.update_goal_progress("achieve_agi", 0.42)
    
    # Display final identity summary
    print("\n" + "-"*70)
    print("FINAL IDENTITY SUMMARY")
    print("-"*70 + "\n")
    
    print(neurocog.get_identity_summary())
    
    # Export to JSON
    print("\n" + "-"*70)
    print("IDENTITY SERIALIZATION")
    print("-"*70 + "\n")
    
    json_output = neurocog.to_json()
    print("Identity serialized to JSON (first 500 chars):")
    print(json_output[:500] + "...")
    
    print("\n" + "="*70)
    print("DEMO COMPLETE: NeuroCog Core Self operational! 🚀✨")
    print("="*70 + "\n")


if __name__ == "__main__":
    main()
