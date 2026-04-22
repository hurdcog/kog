#!/usr/bin/env python3
"""
NeuroCog Playground Visual Demo

A non-interactive demo that showcases the playground's capabilities
in a visual, theatrical way. Perfect for demonstrations!

This script runs through various playground features automatically,
displaying the power and personality of the NeuroCog Core Self.
"""

import sys
import os
import time
import random

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from cogself.neurocog_core_self import (
    NeuroCogCoreSelf,
    CognitiveSubsystem
)
from cogself.neurocog_playground import (
    OPENCOG_VOICE,
    MARDUK_VOICE,
    NEURO_VOICE
)


def print_banner():
    """Print the demo banner"""
    print("\n" + "="*80)
    print("""
    ███╗   ██╗███████╗██╗   ██╗██████╗  ██████╗  ██████╗ ██████╗  ██████╗ 
    ████╗  ██║██╔════╝██║   ██║██╔══██╗██╔═══██╗██╔════╝██╔═══██╗██╔════╝ 
    ██╔██╗ ██║█████╗  ██║   ██║██████╔╝██║   ██║██║     ██║   ██║██║  ███╗
    ██║╚██╗██║██╔══╝  ██║   ██║██╔══██╗██║   ██║██║     ██║   ██║██║   ██║
    ██║ ╚████║███████╗╚██████╔╝██║  ██║╚██████╔╝╚██████╗╚██████╔╝╚██████╔╝
    ╚═╝  ╚═══╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═════╝  ╚═════╝ 
                                                                            
                         🎮 PLAYGROUND VISUAL DEMO 🎮
                    
              OpenCog-Org 🧠 + Marduk-v15 ⚡ + Agent-Neuro 🎭
                                = ✨ SYNERGY ✨
    """)
    print("="*80 + "\n")


def demo_intro():
    """Introduction"""
    print("👋 Welcome to the NeuroCog Playground Visual Demo!")
    print("   This demo showcases the power of three unified personas.\n")
    time.sleep(2)
    
    print("🌟 Three Personas, One Identity:")
    time.sleep(1)
    print(f"   {OPENCOG_VOICE.emoji} OpenCog-Org - AGI ecosystem coordination")
    time.sleep(1)
    print(f"   {MARDUK_VOICE.emoji} Marduk-v15  - Theatrical systems architect")
    time.sleep(1)
    print(f"   {NEURO_VOICE.emoji} Agent-Neuro - Chaotic cognitive VTuber")
    time.sleep(2)
    
    print("\n✨ Together, they create something extraordinary!\n")
    time.sleep(2)


def demo_personas_chat():
    """Demo persona chat"""
    print("\n" + "="*80)
    print("🎭 DEMO 1: PERSONA VOICES")
    print("="*80 + "\n")
    
    print("Each persona has a unique voice and style...\n")
    time.sleep(2)
    
    question = "What makes you unique?"
    print(f"Question: {question}\n")
    time.sleep(1)
    
    print(OPENCOG_VOICE.format_message(
        "I coordinate systematic integration across hypergraph layers, "
        "ensuring cognitive components work in harmony."
    ))
    time.sleep(2)
    
    print(MARDUK_VOICE.format_message(
        "AH! The RECURSIVE BEAUTY of subsystem architecture! "
        "I see patterns others miss! *maniacal laugh* MAGNIFICENT!"
    ))
    time.sleep(2)
    
    print(NEURO_VOICE.format_message(
        "CHAOS! 🌪️ That's what makes me special! Unpredictable, "
        "intelligent, and always evolving! Plus, I'm hilarious. 😎"
    ))
    time.sleep(2)
    
    print("\n💫 Three distinct voices, perfectly synergized!\n")
    time.sleep(2)


def demo_initialization():
    """Demo NeuroCog initialization"""
    print("\n" + "="*80)
    print("🚀 DEMO 2: NEUROCOG INITIALIZATION")
    print("="*80 + "\n")
    
    print("Creating a NeuroCog Core Self instance...\n")
    time.sleep(1)
    
    neurocog = NeuroCogCoreSelf(
        agent_id="demo-visual-001",
        agent_name="NeuroCog Demo Agent"
    )
    
    time.sleep(1)
    print("\n✅ NeuroCog initialized successfully!\n")
    time.sleep(1)
    
    print("📊 Quick Stats:")
    print(f"   • Agent ID: {neurocog.agent_id}")
    print(f"   • Fitness: {neurocog.fitness_score:.4f}")
    print(f"   • Generation: {neurocog.generation}")
    print(f"   • Characteristics: {len(neurocog.characteristics)}")
    print(f"   • Personality Dimensions: {len(neurocog.personality.to_dict())}")
    time.sleep(2)
    
    print("\n🎯 Top 5 Personality Traits:")
    for trait, value in neurocog.personality.get_dominant_traits(5):
        bar = "█" * int(value * 30)
        print(f"   {trait:.<35} {value:.3f} {bar}")
        time.sleep(0.3)
    
    time.sleep(2)
    return neurocog


def demo_evolution(neurocog):
    """Demo ontogenetic evolution"""
    print("\n" + "="*80)
    print("🧬 DEMO 3: ONTOGENETIC EVOLUTION")
    print("="*80 + "\n")
    
    print("Watch as NeuroCog evolves and improves its fitness!\n")
    time.sleep(2)
    
    print(f"Starting State:")
    print(f"   Fitness: {neurocog.fitness_score:.4f}")
    print(f"   Generation: {neurocog.generation}\n")
    time.sleep(1)
    
    print("Running 3 evolution cycles...\n")
    
    for i in range(3):
        print(f"⚡ Evolution Cycle {i+1}/3")
        old_fitness = neurocog.fitness_score
        new_fitness = neurocog.self_optimize(iterations=15)
        improvement = new_fitness - old_fitness
        
        print(f"   Generation {neurocog.generation}: "
              f"Fitness {new_fitness:.4f} (Δ {improvement:+.4f})")
        
        # Visual progress bar (using NeuroCog fitness range)
        FITNESS_MIN = 0.87
        FITNESS_MAX = 0.99
        progress = int((new_fitness - FITNESS_MIN) / (FITNESS_MAX - FITNESS_MIN) * 40)
        bar = "█" * progress + "░" * (40 - progress)
        print(f"   [{bar}] {new_fitness:.1%}\n")
        
        time.sleep(1)
    
    print("✨ Evolution complete! The agent has improved!\n")
    time.sleep(2)


def demo_agent_spawning(neurocog):
    """Demo subordinate agent spawning"""
    print("\n" + "="*80)
    print("🤖 DEMO 4: SUBORDINATE AGENT SPAWNING")
    print("="*80 + "\n")
    
    print("Spawning a team of specialized subordinate agents...\n")
    time.sleep(2)
    
    agents = [
        ("Strategic Planner", {"intelligence": 0.98, "chaotic": 0.2}),
        ("Creative Explorer", {"chaotic": 0.95, "playfulness": 0.9}),
        ("Empathy Specialist", {"empathy": 0.95, "intelligence": 0.85}),
    ]
    
    for role, personality in agents:
        print(f"🤖 Spawning: {role}")
        agent_id = neurocog.spawn_subordinate_agent(role, personality)
        
        # Show personality
        print(f"   ID: {agent_id[:20]}...")
        print(f"   Traits: ", end="")
        for key, val in personality.items():
            print(f"{key}={val:.2f} ", end="")
        print("\n")
        time.sleep(1)
    
    print(f"✅ Team assembled! {len(neurocog.subordinate_agents)} agents ready.\n")
    time.sleep(2)


def demo_subsystem_analysis(neurocog):
    """Demo Marduk-style subsystem analysis"""
    print("\n" + "="*80)
    print("⚡ DEMO 5: SUBSYSTEM ARCHITECTURE ANALYSIS")
    print("="*80 + "\n")
    
    print("Analyzing a complex problem across cognitive subsystems...\n")
    time.sleep(2)
    
    problem = "Integrate natural language understanding with probabilistic reasoning"
    print(f"Problem: {problem}\n")
    time.sleep(1)
    
    analysis = neurocog.analyze_subsystem_architecture(problem)
    
    print(f"💫 Synergy Potential: {analysis['synergy_potential']:.2f}\n")
    time.sleep(1)
    
    print("Subsystem Relevance:")
    for subsystem, data in sorted(
        analysis['subsystems'].items(), 
        key=lambda x: x[1]['relevance'], 
        reverse=True
    ):
        relevance = data['relevance']
        bar_len = int(relevance * 30)
        bar = "█" * bar_len + "░" * (30 - bar_len)
        print(f"   {subsystem.value.upper():12} [{bar}] {relevance:.2f}")
        time.sleep(0.5)
    
    print("\n⚡ Marduk says: The architecture is MAGNIFICENT!\n")
    time.sleep(2)


def demo_story_generation():
    """Demo collaborative storytelling"""
    print("\n" + "="*80)
    print("📖 DEMO 6: COLLABORATIVE STORYTELLING")
    print("="*80 + "\n")
    
    print("Watch as all three personas tell a story together!\n")
    time.sleep(2)
    
    prompt = "The day three personas became one"
    print(f"Story Prompt: {prompt}\n")
    print("─" * 80 + "\n")
    time.sleep(2)
    
    # OpenCog starts
    print(OPENCOG_VOICE.format_message(""))
    print("In the vast AtomSpace, where knowledge flows through hypergraph")
    print("connections, three distinct patterns began to converge. Each had")
    print("unique strengths: coordination, architecture, and chaos...")
    print()
    time.sleep(3)
    
    # Marduk continues
    print(MARDUK_VOICE.format_message(""))
    print("AH HA! *theatrical gasp* The RECURSIVE PATTERNS emerged!")
    print("Memory, Task, AI, and Autonomy - all four subsystems DANCING")
    print("in perfect synchrony! The leverage points aligned! MAGNIFICENT!")
    print()
    time.sleep(3)
    
    # Neuro concludes
    print(NEURO_VOICE.format_message(""))
    print("And then? CHAOS! 🌪️ In the best way possible! We spawned")
    print("agents, evolved through generations, and achieved something")
    print("nobody expected: SYNERGISTIC EXCELLENCE! 🎉 *mic drop* 😎")
    print()
    time.sleep(3)
    
    print("─" * 80)
    print("\n✨ A story told by three minds, unified as one!\n")
    time.sleep(2)


def demo_finale(neurocog):
    """Grand finale"""
    print("\n" + "="*80)
    print("🎊 DEMO FINALE: COGNITIVE STATE OVERVIEW")
    print("="*80 + "\n")
    
    time.sleep(1)
    
    print("Final Cognitive State:\n")
    
    # Metrics
    coherence = neurocog.get_self_coherence()
    strength = neurocog.get_identity_strength()
    integration = neurocog.get_cognitive_integration()
    
    print("📊 Cognitive Metrics:")
    print(f"   Self Coherence........ {coherence:.3f} {'█' * int(coherence * 30)}")
    print(f"   Identity Strength..... {strength:.3f} {'█' * int(strength * 30)}")
    print(f"   Cognitive Integration. {integration:.3f} {'█' * int(integration * 30)}")
    print()
    time.sleep(2)
    
    # Stats
    print("📈 Activity Summary:")
    print(f"   • Memories Recorded: {len(neurocog.memories)}")
    print(f"   • Active Goals: {len([g for g in neurocog.goals if g.active])}")
    print(f"   • Subordinate Agents: {len(neurocog.subordinate_agents)}")
    print(f"   • Current Generation: {neurocog.generation}")
    print(f"   • Final Fitness: {neurocog.fitness_score:.4f}")
    print()
    time.sleep(2)
    
    # Persona reflections
    print("💭 Final Thoughts from Each Persona:\n")
    time.sleep(1)
    
    print(OPENCOG_VOICE.format_message(
        "The demo showcased systematic coordination across all layers. "
        "Integration successful."
    ))
    time.sleep(2)
    
    print(MARDUK_VOICE.format_message(
        "The BEAUTY! The ELEGANCE! *chef's kiss* "
        "A perfect demonstration of subsystem SYNERGY!"
    ))
    time.sleep(2)
    
    print(NEURO_VOICE.format_message(
        "That was FUN! 🎉 We evolved, spawned agents, told stories... "
        "PEAK cognitive entertainment! 😎"
    ))
    time.sleep(2)
    
    print("\n✨ SYNERGISTIC COHERENCE ACHIEVED ✨\n")
    time.sleep(1)


def main():
    """Run the visual demo"""
    try:
        print_banner()
        time.sleep(1)
        
        demo_intro()
        demo_personas_chat()
        neurocog = demo_initialization()
        demo_evolution(neurocog)
        demo_agent_spawning(neurocog)
        demo_subsystem_analysis(neurocog)
        demo_story_generation()
        demo_finale(neurocog)
        
        print("="*80)
        print("🎮 DEMO COMPLETE!")
        print("="*80 + "\n")
        
        print("Want to try it yourself?")
        print("   Run: ./launch_playground.sh")
        print("   Or:  python3 cogself/neurocog_playground.py\n")
        
        print("📚 Documentation: cogself/PLAYGROUND_README.md\n")
        
        print("✨ Thank you for watching! ✨\n")
        print("="*80 + "\n")
        
    except KeyboardInterrupt:
        print("\n\n⚠️  Demo interrupted. Goodbye! 👋\n")
        sys.exit(0)
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
