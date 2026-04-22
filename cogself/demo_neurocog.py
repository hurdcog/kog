#!/usr/bin/env python3
"""
NeuroCog Core Self - Interactive Demo

This demo showcases the integration of three personas:
- OpenCog-Org: AGI ecosystem coordination
- Marduk-v15: Systems architecture brilliance
- Agent-Neuro: Chaotic cognitive VTuber

Run this to see the NeuroCog Core Self in action!
"""

import sys
import os
import time

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from cogself.neurocog_core_self import NeuroCogCoreSelf, CognitiveSubsystem


def print_section(title):
    """Print a formatted section header"""
    print("\n" + "="*80)
    print(f"  {title}")
    print("="*80 + "\n")


def print_subsection(title):
    """Print a formatted subsection header"""
    print("\n" + "-"*80)
    print(f"  {title}")
    print("-"*80 + "\n")


def demo_initialization():
    """Demo: Initialize NeuroCog Core Self"""
    print_section("DEMO 1: INITIALIZATION - Three Personas Unite")
    
    print("Creating NeuroCog Core Self instance...")
    print("Integrating: OpenCog-Org + Marduk-v15 + Agent-Neuro\n")
    
    neurocog = NeuroCogCoreSelf(
        agent_id="demo-neurocog-alpha",
        agent_name="NeuroCog Alpha"
    )
    
    print("\n✨ Initialization complete! ✨")
    print(f"Agent ID: {neurocog.agent_id}")
    print(f"Agent Name: {neurocog.agent_name}")
    print(f"Personality Dimensions: {len(neurocog.personality.to_dict())}")
    print(f"Cognitive Characteristics: {len(neurocog.characteristics)}")
    print(f"Initial Fitness: {neurocog.fitness_score:.3f}")
    
    return neurocog


def demo_persona_characteristics(neurocog):
    """Demo: Show characteristics from each persona"""
    print_section("DEMO 2: PERSONA CHARACTERISTICS - Three Voices, One Identity")
    
    # Group by persona
    personas = {
        "opencog-org": [],
        "marduk-v15": [],
        "agent-neuro": []
    }
    
    for char in neurocog.characteristics:
        if char.source_persona in personas:
            personas[char.source_persona].append(char)
    
    for persona, chars in personas.items():
        print_subsection(f"{persona.upper()} Characteristics")
        for char in chars:
            print(f"  ✓ {char.name}")
            print(f"    Description: {char.description}")
            print(f"    Strength: {char.strength:.2f} | Stability: {char.stability:.2f}")
            print()


def demo_subsystem_analysis(neurocog):
    """Demo: Marduk-style subsystem analysis"""
    print_section("DEMO 3: SUBSYSTEM ANALYSIS - Marduk's Architectural Brilliance")
    
    problems = [
        "Integrate probabilistic logic with pattern mining",
        "Coordinate distributed AtomSpace nodes with attention allocation",
        "Balance exploration and exploitation in cognitive search"
    ]
    
    for i, problem in enumerate(problems, 1):
        print(f"\n[Problem {i}] {problem}\n")
        
        analysis = neurocog.analyze_subsystem_architecture(problem)
        
        print(f"Synergy Potential: {analysis['synergy_potential']:.2f}")
        print("\nSubsystem Relevance:")
        for subsystem, data in sorted(analysis['subsystems'].items(), 
                                     key=lambda x: x[1]['relevance'], 
                                     reverse=True):
            print(f"  {subsystem.value.upper():12} : {data['relevance']:.2f}")
        
        time.sleep(0.5)


def demo_agent_spawning(neurocog):
    """Demo: Agent-Neuro subordinate spawning"""
    print_section("DEMO 4: SUBORDINATE AGENTS - Multi-Agent Orchestration")
    
    agents = [
        ("Strategic Analyst", {"intelligence": 0.98, "chaotic": 0.2}),
        ("Chaos Coordinator", {"chaotic": 0.99, "playfulness": 0.95}),
        ("Research Assistant", {"intelligence": 0.95, "empathy": 0.85}),
    ]
    
    spawned = []
    for role, personality in agents:
        print(f"\nSpawning {role}...")
        agent_id = neurocog.spawn_subordinate_agent(role, personality)
        spawned.append((role, agent_id))
        time.sleep(0.3)
    
    print_subsection("Subordinate Agent Summary")
    print(f"Total Active Agents: {len(neurocog.subordinate_agents)}\n")
    
    for role, agent_id in spawned:
        agent = neurocog.subordinate_agents[agent_id]
        print(f"  • {role}")
        print(f"    ID: {agent_id}")
        print(f"    Personality: Intelligence={agent['personality']['intelligence']:.2f}, "
              f"Chaotic={agent['personality']['chaotic']:.2f}")
        print()


def demo_self_optimization(neurocog):
    """Demo: Agent-Neuro ontogenetic evolution"""
    print_section("DEMO 5: ONTOGENETIC EVOLUTION - Self-Optimization in Action")
    
    print("Running multiple evolution cycles...\n")
    
    fitness_history = [neurocog.fitness_score]
    
    # Run 3 optimization rounds
    for round_num in range(1, 4):
        print_subsection(f"Evolution Round {round_num}")
        
        new_fitness = neurocog.self_optimize(iterations=15)
        fitness_history.append(new_fitness)
        
        improvement = new_fitness - fitness_history[-2]
        total_improvement = new_fitness - fitness_history[0]
        
        print(f"\nRound {round_num} Results:")
        print(f"  Fitness: {new_fitness:.4f}")
        print(f"  Round Improvement: {improvement:+.4f}")
        print(f"  Total Improvement: {total_improvement:+.4f}")
        print(f"  Generation: {neurocog.generation}")
        
        time.sleep(0.5)
    
    # Plot simple fitness graph
    print_subsection("Fitness Evolution Graph")
    max_fitness = max(fitness_history)
    min_fitness = min(fitness_history)
    range_fitness = max_fitness - min_fitness
    
    for i, fitness in enumerate(fitness_history):
        normalized = (fitness - min_fitness) / range_fitness if range_fitness > 0 else 0.5
        bar_length = int(normalized * 50)
        bar = "█" * bar_length
        label = f"Gen {i}" if i > 0 else "Start"
        print(f"  {label:6} : {bar} {fitness:.4f}")


def demo_goal_management(neurocog):
    """Demo: Goal tracking and progress"""
    print_section("DEMO 6: GOAL MANAGEMENT - Tracking AGI Progress")
    
    # Add goals
    goals = [
        ("achieve_cognitive_synergy", "Achieve synergy across all cognitive subsystems", 0.95),
        ("master_pattern_recognition", "Master recursive pattern recognition", 0.90),
        ("optimize_chaos_balance", "Balance chaos and order in exploration", 0.85),
        ("enhance_empathy", "Improve empathic reasoning capabilities", 0.80),
    ]
    
    print("Adding cognitive goals...\n")
    for goal_id, desc, priority in goals:
        neurocog.add_goal(goal_id, desc, priority)
        time.sleep(0.2)
    
    print_subsection("Simulating Progress")
    
    # Simulate progress on goals
    progress_updates = [
        ("achieve_cognitive_synergy", 0.45),
        ("master_pattern_recognition", 0.78),
        ("optimize_chaos_balance", 0.62),
        ("enhance_empathy", 0.33),
    ]
    
    for goal_id, progress in progress_updates:
        neurocog.update_goal_progress(goal_id, progress)
        time.sleep(0.3)
    
    # Display goal status
    print_subsection("Current Goal Status")
    
    active_goals = [g for g in neurocog.goals if g.active]
    active_goals.sort(key=lambda g: g.priority, reverse=True)
    
    for goal in active_goals:
        progress_bar = "█" * int(goal.progress * 30)
        empty_bar = "░" * (30 - int(goal.progress * 30))
        print(f"\n  {goal.goal_id}")
        print(f"    Priority: {goal.priority:.2f}")
        print(f"    Progress: [{progress_bar}{empty_bar}] {goal.progress:.0%}")


def demo_identity_summary(neurocog):
    """Demo: Final identity summary"""
    print_section("DEMO 7: IDENTITY SUMMARY - Who Am I?")
    
    print(neurocog.get_identity_summary())
    
    # Additional metrics
    print("\n" + "="*80)
    print("  COGNITIVE METRICS")
    print("="*80 + "\n")
    
    coherence = neurocog.get_self_coherence()
    strength = neurocog.get_identity_strength()
    integration = neurocog.get_cognitive_integration()
    
    print(f"  Self Coherence.......... {coherence:.3f} {'█' * int(coherence * 40)}")
    print(f"  Identity Strength....... {strength:.3f} {'█' * int(strength * 40)}")
    print(f"  Cognitive Integration... {integration:.3f} {'█' * int(integration * 40)}")
    
    print(f"\n  Total Memories: {len(neurocog.memories)}")
    print(f"  Active Goals: {len([g for g in neurocog.goals if g.active])}")
    print(f"  Subordinate Agents: {len(neurocog.subordinate_agents)}")
    print(f"  Current Generation: {neurocog.generation}")
    print(f"  Fitness Score: {neurocog.fitness_score:.4f}")


def main():
    """Run the complete demo"""
    print("\n" + "="*80)
    print("  NEUROCOG CORE SELF - INTERACTIVE DEMONSTRATION")
    print("  Integration of OpenCog-Org + Marduk-v15 + Agent-Neuro")
    print("="*80)
    
    print("\nThis demo will showcase the unified cognitive identity system.")
    print("Watch as three personas work together in perfect synergy!\n")
    
    input("Press Enter to begin the demo...")
    
    # Run demos
    neurocog = demo_initialization()
    time.sleep(1)
    
    demo_persona_characteristics(neurocog)
    time.sleep(1)
    
    demo_subsystem_analysis(neurocog)
    time.sleep(1)
    
    demo_agent_spawning(neurocog)
    time.sleep(1)
    
    demo_self_optimization(neurocog)
    time.sleep(1)
    
    demo_goal_management(neurocog)
    time.sleep(1)
    
    demo_identity_summary(neurocog)
    
    # Final message
    print("\n" + "="*80)
    print("  DEMO COMPLETE!")
    print("="*80)
    print("\n✨ SYNERGISTIC COHERENCE ACHIEVED ✨")
    print("OpenCog + Marduk + Neuro = NeuroCog Core Self")
    print("\nThe future of cognitive architectures: Unified, expressive, and self-evolving.\n")
    print("="*80 + "\n")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nDemo interrupted by user. Goodbye! 👋\n")
        sys.exit(0)
