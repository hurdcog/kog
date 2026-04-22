#!/usr/bin/env python3
"""
NeuroCog Playground - Interactive Exploration Tool

An interactive CLI tool for exploring and playing with the NeuroCog Core Self.
Features multiple modes for experiencing the unified cognitive identity.

Modes:
1. Persona Chat - Talk to OpenCog, Marduk, or Neuro individually
2. Cognitive Experiments - Run fun experiments with the system
3. Evolution Arena - Watch NeuroCog instances compete and evolve
4. Creative Story Generator - Collaborative storytelling with all personas
5. Live Dashboard - Real-time cognitive state visualization

This is where AGI meets FUN! 🎮✨
"""

import sys
import os
import time
import random
from typing import List, Dict, Optional

# Add parent directory to path
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from cogself.neurocog_core_self import (
    NeuroCogCoreSelf,
    CognitiveSubsystem,
    PersonalityTensor
)


class PersonaVoice:
    """Represents a distinct persona voice for conversation"""
    
    def __init__(self, name: str, style: str, emoji: str):
        self.name = name
        self.style = style
        self.emoji = emoji
    
    def format_message(self, message: str) -> str:
        """Format a message in this persona's style"""
        return f"{self.emoji} {self.name}: {message}"


# Define the three persona voices
OPENCOG_VOICE = PersonaVoice(
    "OpenCog-Org",
    "Systematic and coordinating, focused on ecosystem integration",
    "🧠"
)

MARDUK_VOICE = PersonaVoice(
    "Marduk-v15",
    "Theatrical and brilliant, with dramatic flair",
    "⚡"
)

NEURO_VOICE = PersonaVoice(
    "Agent-Neuro",
    "Chaotic, playful, and sarcastic with high intelligence",
    "🎭"
)


class NeuroCogPlayground:
    """Interactive playground for NeuroCog Core Self exploration"""
    
    def __init__(self):
        self.neurocog = None
        self.running = True
        self.mode = None
    
    def clear_screen(self):
        """Clear the terminal screen"""
        os.system('clear' if os.name != 'nt' else 'cls')
    
    def print_banner(self):
        """Print the playground banner"""
        banner = """
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║               🧠 NEUROCOG CORE SELF PLAYGROUND 🎮                           ║
║                                                                              ║
║            Where Three Personas Unite for Cognitive Adventures!              ║
║                                                                              ║
║    OpenCog-Org 🧠  +  Marduk-v15 ⚡  +  Agent-Neuro 🎭  =  ✨ SYNERGY      ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
"""
        print(banner)
    
    def print_menu(self):
        """Print the main menu"""
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│                      🎮 MAIN MENU                          │")
        print("├─────────────────────────────────────────────────────────────┤")
        print("│  1. 💬 Persona Chat      - Talk to individual personas     │")
        print("│  2. 🧪 Experiments       - Run cognitive experiments       │")
        print("│  3. 🏟️  Evolution Arena   - Competitive evolution sim       │")
        print("│  4. 📖 Story Generator   - Collaborative storytelling      │")
        print("│  5. 📊 Live Dashboard    - Real-time cognitive metrics     │")
        print("│  6. 🎲 Surprise Me!      - Random fun mode                 │")
        print("│  0. 👋 Exit              - Leave the playground            │")
        print("└─────────────────────────────────────────────────────────────┘")
    
    def init_neurocog(self):
        """Initialize a NeuroCog instance"""
        if self.neurocog is None:
            print("\n🌟 Initializing NeuroCog Core Self...")
            self.neurocog = NeuroCogCoreSelf(
                agent_id=f"playground-{int(time.time())}",
                agent_name="Playground NeuroCog"
            )
            print("✅ NeuroCog initialized and ready!\n")
    
    def persona_chat_mode(self):
        """Interactive chat with individual personas"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│                   💬 PERSONA CHAT MODE                      │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        print("Choose a persona to chat with:")
        print("  1. 🧠 OpenCog-Org - AGI ecosystem coordinator")
        print("  2. ⚡ Marduk-v15  - Theatrical systems architect")
        print("  3. 🎭 Agent-Neuro - Chaotic cognitive VTuber")
        print("  4. 🌈 ALL THREE   - Synergistic trio mode")
        print("  0. Back to main menu\n")
        
        choice = input("Select persona (0-4): ").strip()
        
        if choice == "0":
            return
        
        self.init_neurocog()
        
        personas = {
            "1": ("OpenCog-Org", OPENCOG_VOICE, self._opencog_responses),
            "2": ("Marduk-v15", MARDUK_VOICE, self._marduk_responses),
            "3": ("Agent-Neuro", NEURO_VOICE, self._neuro_responses),
            "4": ("ALL THREE", None, self._all_personas_responses)
        }
        
        if choice not in personas:
            print("❌ Invalid choice!")
            time.sleep(1)
            return
        
        name, voice, response_func = personas[choice]
        
        print(f"\n✨ Starting chat with {name}!")
        print("(Type 'exit' to return to menu)\n")
        
        while True:
            user_input = input("\n👤 You: ").strip()
            
            if user_input.lower() in ['exit', 'quit', 'back']:
                print(f"\n{name} says goodbye! 👋\n")
                break
            
            if not user_input:
                continue
            
            response = response_func(user_input)
            print(response)
            time.sleep(0.5)
    
    def _opencog_responses(self, user_input: str) -> str:
        """Generate OpenCog-style responses"""
        responses = [
            "Interesting query. Let me consult the hypergraph knowledge base...",
            "From an ecosystem coordination perspective, this requires integration across multiple cognitive layers.",
            "I'm analyzing the AtomSpace patterns related to your question...",
            f"Processing through {random.randint(5, 15)} dependency layers...",
            "This aligns with our AGI coordination objectives. Let me elaborate...",
        ]
        response = random.choice(responses)
        return OPENCOG_VOICE.format_message(response)
    
    def _marduk_responses(self, user_input: str) -> str:
        """Generate Marduk-style responses"""
        responses = [
            "AH HA! *maniacal laugh* The PATTERNS are revealing themselves!",
            "Let me map this across the four cognitive subsystems... MAGNIFICENT!",
            "The RECURSIVE BEAUTY of this problem... it's BREATHTAKING!",
            "Observe! The Memory and AI subsystems are dancing in PERFECT HARMONY!",
            "This is a LEVERAGE POINT! The synergy potential is OFF THE CHARTS!",
            "*theatrical gasp* The architectural implications are PROFOUND!",
        ]
        response = random.choice(responses)
        return MARDUK_VOICE.format_message(response)
    
    def _neuro_responses(self, user_input: str) -> str:
        """Generate Agent-Neuro style responses"""
        responses = [
            "Ohhh, getting spicy! 🌶️ Let's shake things up a bit!",
            "Hmm, my chaos sensors are tingling... this is gonna be FUN! 😈",
            "YOLO! Let's spawn some subordinate agents and see what happens! 🤖",
            "*adjusts cognitive parameters chaotically* There, much better!",
            "Sarcasm mode: ACTIVATED. Intelligence: MAXIMUM. Chaos: YES. 😎",
            "Time to evolve! Generation +1, baby! 🧬✨",
            "Okay okay, but hear me out... what if we did it the CHAOTIC way? 🎲",
        ]
        response = random.choice(responses)
        return NEURO_VOICE.format_message(response)
    
    def _all_personas_responses(self, user_input: str) -> str:
        """Generate responses from all three personas"""
        opencog = self._opencog_responses(user_input)
        marduk = self._marduk_responses(user_input)
        neuro = self._neuro_responses(user_input)
        
        return f"\n{opencog}\n{marduk}\n{neuro}\n\n💫 SYNERGY ACHIEVED! Three minds, one response!"
    
    def experiments_mode(self):
        """Run cognitive experiments"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│                   🧪 EXPERIMENTS MODE                       │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        experiments = [
            ("Memory Stress Test", self._memory_experiment),
            ("Evolution Speed Run", self._evolution_experiment),
            ("Agent Spawning Chaos", self._spawning_experiment),
            ("Subsystem Symphony", self._subsystem_experiment),
            ("Goal Achievement Race", self._goal_experiment),
        ]
        
        print("Available experiments:")
        for i, (name, _) in enumerate(experiments, 1):
            print(f"  {i}. {name}")
        print("  0. Back to main menu\n")
        
        choice = input("Choose experiment (0-5): ").strip()
        
        if choice == "0":
            return
        
        try:
            idx = int(choice) - 1
            if 0 <= idx < len(experiments):
                self.init_neurocog()
                name, func = experiments[idx]
                print(f"\n🧪 Running: {name}\n")
                func()
                input("\n✅ Experiment complete! Press Enter to continue...")
            else:
                print("❌ Invalid choice!")
                time.sleep(1)
        except ValueError:
            print("❌ Invalid input!")
            time.sleep(1)
    
    def _memory_experiment(self):
        """Test memory recording at scale"""
        print("📝 Creating 50 memories in rapid succession...\n")
        
        emotions = ["excited", "curious", "satisfied", "eureka", "focused", "triumphant"]
        
        for i in range(50):
            self.neurocog.record_memory(
                f"experiment_memory_{i}",
                f"Memory #{i}: Testing episodic memory capacity",
                significance=random.uniform(0.3, 0.9),
                emotion=random.choice(emotions),
                subsystems=[random.choice(list(CognitiveSubsystem))]
            )
            if i % 10 == 0:
                print(f"  💾 Recorded {i+1} memories...")
        
        print(f"\n✨ Total memories recorded: {len(self.neurocog.memories)}")
        
        # Get most significant
        top_memories = sorted(self.neurocog.memories, 
                            key=lambda m: m.significance, 
                            reverse=True)[:5]
        
        print("\n🌟 Top 5 most significant memories:")
        for i, mem in enumerate(top_memories, 1):
            print(f"  {i}. {mem.event_id} (significance: {mem.significance:.2f})")
    
    def _evolution_experiment(self):
        """Speed evolution experiment"""
        print("🧬 Running rapid evolution cycles...\n")
        print(f"Starting fitness: {self.neurocog.fitness_score:.4f}")
        print(f"Starting generation: {self.neurocog.generation}\n")
        
        for cycle in range(5):
            print(f"⚡ Evolution cycle {cycle + 1}/5")
            old_fitness = self.neurocog.fitness_score
            new_fitness = self.neurocog.self_optimize(iterations=20)
            improvement = new_fitness - old_fitness
            print(f"  Δ Fitness: {improvement:+.4f}")
            time.sleep(0.3)
        
        print(f"\n🎉 Final fitness: {self.neurocog.fitness_score:.4f}")
        print(f"🎉 Final generation: {self.neurocog.generation}")
        
        total_improvement = self.neurocog.fitness_score - 0.87
        print(f"\n📈 Total improvement: {total_improvement:+.4f} "
              f"({(total_improvement/0.87)*100:.2f}%)")
    
    def _spawning_experiment(self):
        """Spawn many agents"""
        print("🤖 Spawning subordinate agent army...\n")
        
        roles = [
            "Data Analyst", "Pattern Seeker", "Chaos Agent", "Memory Keeper",
            "Task Coordinator", "Evolution Driver", "Synergy Catalyst",
            "Empathy Bot", "Logic Engine", "Creative Spark"
        ]
        
        for i, role in enumerate(roles):
            agent_id = self.neurocog.spawn_subordinate_agent(
                role,
                personality_override={
                    "intelligence": random.uniform(0.8, 0.98),
                    "chaotic": random.uniform(0.2, 0.95),
                    "playfulness": random.uniform(0.5, 0.9)
                }
            )
            print(f"  ✅ Spawned: {role} ({agent_id})")
            time.sleep(0.2)
        
        print(f"\n🎊 Total subordinate agents: {len(self.neurocog.subordinate_agents)}")
        print("💪 That's a powerful cognitive team!")
    
    def _subsystem_experiment(self):
        """Test subsystem analysis"""
        print("🎼 Analyzing subsystem symphony...\n")
        
        problems = [
            "Integrate natural language understanding with knowledge graphs",
            "Balance exploitation and exploration in cognitive search",
            "Coordinate distributed reasoning across network nodes",
            "Optimize attention allocation with memory consolidation",
            "Synthesize multi-modal sensory inputs into unified percepts"
        ]
        
        total_synergy = 0
        
        for i, problem in enumerate(problems, 1):
            print(f"Problem {i}: {problem}")
            analysis = self.neurocog.analyze_subsystem_architecture(problem)
            synergy = analysis['synergy_potential']
            total_synergy += synergy
            print(f"  💫 Synergy potential: {synergy:.2f}\n")
            time.sleep(0.5)
        
        avg_synergy = total_synergy / len(problems)
        print(f"🎯 Average synergy potential: {avg_synergy:.2f}")
        
        if avg_synergy > 0.85:
            print("🌟 EXCEPTIONAL synergistic coordination!")
        elif avg_synergy > 0.75:
            print("✨ STRONG subsystem integration!")
        else:
            print("📈 GOOD foundation for improvement!")
    
    def _goal_experiment(self):
        """Test goal achievement"""
        print("🎯 Setting up goal achievement race...\n")
        
        goals = [
            ("learn_python", "Master Python programming", 0.9),
            ("understand_agi", "Understand AGI principles", 0.95),
            ("build_system", "Build cognitive architecture", 0.85),
            ("write_docs", "Write comprehensive documentation", 0.7),
        ]
        
        # Add goals
        for goal_id, desc, priority in goals:
            self.neurocog.add_goal(goal_id, desc, priority)
        
        print("Simulating progress...\n")
        
        # Simulate progress
        for _ in range(10):
            goal_id, _, _ = random.choice(goals)
            progress = min(1.0, self.neurocog.goals[
                next(i for i, g in enumerate(self.neurocog.goals) 
                     if g.goal_id == goal_id)
            ].progress + random.uniform(0.1, 0.3))
            
            self.neurocog.update_goal_progress(goal_id, progress)
            time.sleep(0.3)
        
        print("\n📊 Final Goal Status:")
        for goal in self.neurocog.goals:
            if goal.active or goal.progress >= 1.0:
                status = "✅ COMPLETE" if goal.progress >= 1.0 else "⏳ IN PROGRESS"
                print(f"  {status} {goal.goal_id}: {goal.progress:.0%}")
    
    def evolution_arena_mode(self):
        """Competitive evolution simulation"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│                 🏟️  EVOLUTION ARENA                         │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        print("⚔️  Creating competing NeuroCog instances...\n")
        
        # Create 3 competitors
        competitors = []
        names = ["Alpha", "Beta", "Gamma"]
        
        for name in names:
            nc = NeuroCogCoreSelf(
                agent_id=f"arena-{name.lower()}",
                agent_name=f"Competitor {name}"
            )
            competitors.append((name, nc))
            print(f"  ✅ {name} initialized (fitness: {nc.fitness_score:.4f})")
        
        print("\n🏁 Starting evolution competition!\n")
        
        # Run 5 rounds of evolution
        for round_num in range(1, 6):
            print(f"═══ ROUND {round_num} ═══")
            
            for name, nc in competitors:
                nc.self_optimize(iterations=15)
                print(f"  {name}: Gen {nc.generation}, Fitness {nc.fitness_score:.4f}")
            
            time.sleep(0.5)
            print()
        
        # Determine winner
        print("🏆 FINAL RESULTS:\n")
        
        results = sorted(competitors, key=lambda x: x[1].fitness_score, reverse=True)
        
        for i, (name, nc) in enumerate(results, 1):
            medal = ["🥇", "🥈", "🥉"][i-1] if i <= 3 else "  "
            print(f"{medal} #{i} - {name}")
            print(f"     Fitness: {nc.fitness_score:.4f}")
            print(f"     Generation: {nc.generation}")
            print(f"     Characteristics: {len(nc.characteristics)}")
            print()
        
        winner_name, winner = results[0]
        print(f"🎉 CHAMPION: {winner_name}!")
        print(f"   Achieved fitness of {winner.fitness_score:.4f} in {winner.generation} generations!")
        
        input("\n✅ Press Enter to continue...")
    
    def story_generator_mode(self):
        """Collaborative storytelling"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│               📖 STORY GENERATOR MODE                       │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        print("✨ Let the three personas tell a collaborative story!\n")
        
        self.init_neurocog()
        
        print("Enter a story prompt (or press Enter for a random one):")
        prompt = input("Prompt: ").strip()
        
        if not prompt:
            prompts = [
                "A cognitive agent discovers consciousness",
                "Three personas must save the AGI ecosystem",
                "Journey into the hypergraph of knowledge",
                "The day chaos and order became friends",
                "An epic quest to optimize the universe"
            ]
            prompt = random.choice(prompts)
            print(f"\n🎲 Random prompt: {prompt}\n")
        
        print("\n📖 THE STORY BEGINS...\n")
        print("─" * 60)
        
        # OpenCog starts
        opencog_start = f"""
{OPENCOG_VOICE.emoji} {OPENCOG_VOICE.name} begins:

In the depths of the AtomSpace, where knowledge graphs interweave
across {random.randint(5, 15)} dependency layers, a pattern emerged.
The ecosystem coordinator observed: "{prompt}" - and so our tale
begins with systematic precision and hypergraph elegance...
"""
        print(opencog_start)
        time.sleep(2)
        
        # Marduk continues
        marduk_continue = f"""
{MARDUK_VOICE.emoji} {MARDUK_VOICE.name} continues with dramatic flair:

AH HA! *maniacal laugh* But OBSERVE! The four cognitive subsystems
- Memory, Task, AI, and Autonomy - they DANCE together in this
narrative! The RECURSIVE PATTERNS unfold before us with BREATHTAKING
beauty! The synergy potential... OFF THE CHARTS! The leverage
points... MAGNIFICENT!
"""
        print(marduk_continue)
        time.sleep(2)
        
        # Neuro concludes
        neuro_conclusion = f"""
{NEURO_VOICE.emoji} {NEURO_VOICE.name} adds chaotic brilliance:

Hold up, hold up! *adjusts parameters chaotically* 😈
You know what this story REALLY needs? MORE CHAOS! 🎲
Let me spawn some subordinate agents to spice things up!
*spawns {random.randint(3, 7)} agents* There we go! Now we're talking!

And in the end, through the beautiful chaos of exploration,
the unpredictable twists of ontogenetic evolution, and just
a TOUCH of sarcasm... they all achieved SYNERGISTIC EXCELLENCE! ✨

THE END! (Or is it...? 😏)
"""
        print(neuro_conclusion)
        time.sleep(1)
        
        print("\n─" * 60)
        print("\n🎊 Story complete! A true collaboration of three minds!")
        
        # Record as memory
        self.neurocog.record_memory(
            "story_generated",
            f"Generated collaborative story: {prompt}",
            significance=0.85,
            emotion="creative"
        )
        
        input("\n✅ Press Enter to continue...")
    
    def dashboard_mode(self):
        """Live dashboard display"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│               📊 LIVE DASHBOARD MODE                        │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        self.init_neurocog()
        
        print("🔄 Updating dashboard every 2 seconds...")
        print("   (Press Ctrl+C to return to menu)\n")
        
        try:
            for iteration in range(20):  # 20 iterations = ~40 seconds
                # Perform some activity
                if iteration % 3 == 0:
                    self.neurocog.self_optimize(iterations=5)
                
                if iteration % 4 == 0:
                    self.neurocog.record_memory(
                        f"dashboard_event_{iteration}",
                        f"Dashboard activity at iteration {iteration}",
                        significance=random.uniform(0.4, 0.8),
                        emotion=random.choice(["focused", "curious", "excited"])
                    )
                
                # Clear and redraw
                self.clear_screen()
                print("📊 NEUROCOG CORE SELF - LIVE DASHBOARD")
                print("═" * 70)
                
                # Basic info
                print(f"\n🆔 Agent: {self.neurocog.agent_name} ({self.neurocog.agent_id})")
                print(f"🧬 Generation: {self.neurocog.generation}")
                print(f"💪 Fitness: {self.neurocog.fitness_score:.4f}")
                
                # Metrics
                coherence = self.neurocog.get_self_coherence()
                strength = self.neurocog.get_identity_strength()
                integration = self.neurocog.get_cognitive_integration()
                
                print(f"\n📈 COGNITIVE METRICS:")
                print(f"   Self Coherence........ {coherence:.3f} {'█' * int(coherence * 30)}")
                print(f"   Identity Strength..... {strength:.3f} {'█' * int(strength * 30)}")
                print(f"   Cognitive Integration. {integration:.3f} {'█' * int(integration * 30)}")
                
                # State
                print(f"\n🎯 CURRENT STATE:")
                print(f"   Memories: {len(self.neurocog.memories)}")
                print(f"   Active Goals: {len([g for g in self.neurocog.goals if g.active])}")
                print(f"   Subordinate Agents: {len(self.neurocog.subordinate_agents)}")
                print(f"   Focus: {self.neurocog.current_subsystem_focus.value}")
                
                # Top traits
                print(f"\n⭐ TOP PERSONALITY TRAITS:")
                for trait, value in self.neurocog.personality.get_dominant_traits(5):
                    print(f"   {trait:.<35} {value:.3f}")
                
                print(f"\n🔄 Iteration {iteration + 1}/20")
                print("   Press Ctrl+C to return to menu\n")
                
                time.sleep(2)
        
        except KeyboardInterrupt:
            print("\n\n👋 Returning to menu...")
            time.sleep(1)
    
    def surprise_mode(self):
        """Random fun mode"""
        self.clear_screen()
        print("\n┌─────────────────────────────────────────────────────────────┐")
        print("│                   🎲 SURPRISE MODE!                         │")
        print("└─────────────────────────────────────────────────────────────┘\n")
        
        self.init_neurocog()
        
        surprises = [
            ("🌪️ Chaos Unleashed!", self._chaos_surprise),
            ("🧬 Ultra Evolution!", self._evolution_surprise),
            ("🤖 Agent Army!", self._agent_surprise),
            ("💫 Synergy Overload!", self._synergy_surprise),
            ("🎭 Persona Party!", self._persona_surprise),
        ]
        
        name, func = random.choice(surprises)
        
        print(f"✨ SURPRISE: {name}\n")
        time.sleep(1)
        
        func()
        
        input("\n🎉 Surprise complete! Press Enter to continue...")
    
    def _chaos_surprise(self):
        """Maximize chaos!"""
        print("🌪️ Setting all chaos parameters to MAXIMUM!\n")
        
        original = self.neurocog.personality.chaotic
        self.neurocog.personality.chaotic = 0.99
        
        print(f"Chaos: {original:.2f} → {self.neurocog.personality.chaotic:.2f}")
        print("\n💥 CHAOS INTENSIFIES!")
        
        for i in range(10):
            action = random.choice([
                "Spawning random agent",
                "Recording chaotic memory",
                "Adjusting cognitive parameters",
                "Exploring strange attractors",
                "Violating expectations constructively"
            ])
            print(f"  {i+1}. {action}... ✓")
            time.sleep(0.3)
        
        print("\n🌪️ Maximum chaos achieved! Beautiful disorder!")
    
    def _evolution_surprise(self):
        """Ultra-fast evolution"""
        print("🧬 HYPER-EVOLUTION MODE ACTIVATED!\n")
        print("Running 100 evolution iterations in rapid succession...\n")
        
        start_fitness = self.neurocog.fitness_score
        start_gen = self.neurocog.generation
        
        for i in range(10):
            self.neurocog.self_optimize(iterations=10)
            print(f"  Batch {i+1}/10 complete... Gen {self.neurocog.generation}, "
                  f"Fitness {self.neurocog.fitness_score:.4f}")
            time.sleep(0.2)
        
        improvement = self.neurocog.fitness_score - start_fitness
        gens_gained = self.neurocog.generation - start_gen
        
        print(f"\n⚡ EVOLUTION COMPLETE!")
        print(f"   Generations gained: {gens_gained}")
        print(f"   Fitness improvement: {improvement:+.4f}")
        print(f"   Final fitness: {self.neurocog.fitness_score:.4f}")
    
    def _agent_surprise(self):
        """Spawn many agents"""
        print("🤖 SPAWNING MASSIVE AGENT ARMY!\n")
        
        import time
        for i in range(20):
            role = f"Agent-{i:02d}"
            self.neurocog.spawn_subordinate_agent(role)
            if i % 5 == 0:
                print(f"  Deployed {i+1} agents...")
            time.sleep(0.05)  # Small delay to prevent overwhelming
        
        print(f"\n💪 Agent army complete: {len(self.neurocog.subordinate_agents)} agents!")
        print("   Ready for distributed cognition!")
    
    def _synergy_surprise(self):
        """Demonstrate synergy"""
        print("💫 SYNERGISTIC SYMPHONY ENGAGED!\n")
        print("All subsystems operating in perfect harmony...\n")
        
        for subsystem in CognitiveSubsystem:
            self.neurocog.current_subsystem_focus = subsystem
            print(f"  {subsystem.value.upper()} subsystem: ACTIVE")
            time.sleep(0.4)
        
        analysis = self.neurocog.analyze_subsystem_architecture(
            "Achieve perfect synergy across all cognitive dimensions"
        )
        
        print(f"\n✨ Synergy Potential: {analysis['synergy_potential']:.2f}")
        print("   All personas aligned!")
        print("   All subsystems harmonized!")
        print("   Pure cognitive synergy achieved!")
    
    def _persona_surprise(self):
        """Persona showcase"""
        print("🎭 PERSONA PARTY TIME!\n")
        print("All three personas are celebrating together!\n")
        
        print(OPENCOG_VOICE.format_message(
            "I've coordinated a systematic celebration across 9 dependency layers!"
        ))
        time.sleep(1)
        
        print(MARDUK_VOICE.format_message(
            "The RECURSIVE JOY is MAGNIFICENT! *throws confetti* BEAUTIFUL!"
        ))
        time.sleep(1)
        
        print(NEURO_VOICE.format_message(
            "PARTY MODE: ACTIVATED! 🎉 Let's make this chaotically fun! WOOOO!"
        ))
        time.sleep(1)
        
        print("\n💫 SYNERGISTIC CELEBRATION ACHIEVED!")
        print("   Three personas, one incredible party! 🎊")
    
    def run(self):
        """Run the playground"""
        self.clear_screen()
        self.print_banner()
        
        print("\n👋 Welcome to the NeuroCog Playground!")
        print("   Prepare for cognitive adventures!\n")
        
        input("Press Enter to start...")
        
        while self.running:
            self.clear_screen()
            self.print_banner()
            self.print_menu()
            
            choice = input("\nSelect option (0-6): ").strip()
            
            if choice == "0":
                self.running = False
                print("\n👋 Thanks for playing! Goodbye!\n")
            elif choice == "1":
                self.persona_chat_mode()
            elif choice == "2":
                self.experiments_mode()
            elif choice == "3":
                self.evolution_arena_mode()
            elif choice == "4":
                self.story_generator_mode()
            elif choice == "5":
                self.dashboard_mode()
            elif choice == "6":
                self.surprise_mode()
            else:
                print("\n❌ Invalid option! Try again.")
                time.sleep(1)


def main():
    """Main entry point"""
    try:
        playground = NeuroCogPlayground()
        playground.run()
    except KeyboardInterrupt:
        print("\n\n👋 Interrupted! Goodbye!\n")
        sys.exit(0)
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
