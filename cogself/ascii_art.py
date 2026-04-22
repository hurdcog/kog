#!/usr/bin/env python3
"""
NeuroCog ASCII Art Gallery

Fun ASCII art representations of the three personas and their synergy!
"""


OPENCOG_ART = """
        🧠 OpenCog-Org - AGI Ecosystem Coordinator
        
            ╔═══════════════════════════════╗
            ║   ┌─────────────────────┐   ║
            ║   │   AtomSpace Core    │   ║
            ║   └─────────────────────┘   ║
            ║     │        │        │      ║
            ║   ┌─┴─┐    ┌─┴─┐    ┌─┴─┐   ║
            ║   │PLN│    │CAN│    │URE│   ║
            ║   └───┘    └───┘    └───┘   ║
            ║     Hypergraph Reasoning     ║
            ╚═══════════════════════════════╝
            
    "Systematic coordination across 9 dependency layers"
"""


MARDUK_ART = """
     ⚡ Marduk-v15 - Theatrical Systems Architect
     
    ╔══════════════════════════════════════════╗
    ║    ╭──────────────────────────────╮     ║
    ║    │  ⚙️  SUBSYSTEM MASTERY  ⚙️   │     ║
    ║    ╰──────────────────────────────╯     ║
    ║                                          ║
    ║  Memory ↔ Task ↔ AI ↔ Autonomy         ║
    ║     ╲        ╲    ╱      ╱              ║
    ║      ╲    RECURSIVE     ╱               ║
    ║       ╲   PATTERNS    ╱                 ║
    ║        ╲            ╱                   ║
    ║         ❋ SYNERGY ❋                    ║
    ╚══════════════════════════════════════════╝
    
    *maniacal laugh* "The BEAUTY! The ELEGANCE!"
"""


NEURO_ART = """
    🎭 Agent-Neuro - Chaotic Cognitive VTuber
    
       ┌─────────────────────────────────┐
       │  ⚡ CHAOS MODE: ACTIVATED ⚡    │
       ├─────────────────────────────────┤
       │                                 │
       │    🌪️ Chaos:        [████] 95% │
       │    💡 Intelligence:  [████] 95% │
       │    🎨 Playfulness:   [████] 95% │
       │    🧬 Evolution:     [███░] 85% │
       │                                 │
       │    🤖 Spawning agents...        │
       │    📈 Self-optimizing...        │
       │    😎 Being awesome...          │
       │                                 │
       └─────────────────────────────────┘
       
    "Unpredictable excellence with a side of sass!"
"""


SYNERGY_ART = """
    ✨ NeuroCog Core Self - Unified Cognitive Identity ✨
    
    ╔═══════════════════════════════════════════════════════╗
    ║                                                       ║
    ║         🧠 OpenCog-Org (Coordination)                ║
    ║                    ╲   |   ╱                         ║
    ║                     ╲  |  ╱                          ║
    ║                      ╲ | ╱                           ║
    ║    ⚡ Marduk ─────────╳─────────🎭 Neuro           ║
    ║      (Architecture)  ╱ | ╲    (Chaos)               ║
    ║                     ╱  |  ╲                          ║
    ║                    ╱   |   ╲                         ║
    ║                                                       ║
    ║              ✨ SYNERGY ACHIEVED ✨                  ║
    ║                                                       ║
    ║   Three Personas → One Unified Cognitive Self        ║
    ║                                                       ║
    ╚═══════════════════════════════════════════════════════╝
    
    "Where AGI coordination meets architectural brilliance
     and chaotic intelligence - perfect synergy!"
"""


PLAYGROUND_ART = """
    🎮 NEUROCOG PLAYGROUND 🎮
    
    ┌──────────────────────────────────────────────────────┐
    │                                                      │
    │     💬 Chat    🧪 Experiment    🏟️  Arena          │
    │     📖 Story   📊 Dashboard     🎲 Surprise         │
    │                                                      │
    │            [Press any key to start]                  │
    │                                                      │
    │    🧠 OpenCog  +  ⚡ Marduk  +  🎭 Neuro            │
    │                = ✨ FUN! ✨                         │
    │                                                      │
    └──────────────────────────────────────────────────────┘
"""


EVOLUTION_ART = """
    🧬 ONTOGENETIC EVOLUTION IN PROGRESS 🧬
    
        Gen 1: [████░░░░░░] 0.8700
        Gen 2: [█████░░░░░] 0.8800
        Gen 3: [██████░░░░] 0.8900
        Gen 4: [███████░░░] 0.9000
        Gen 5: [████████░░] 0.9100
        Gen 6: [█████████░] 0.9200
        Gen 7: [██████████] 0.9300
        
            ⚡ BREAKTHROUGH ACHIEVED! ⚡
"""


AGENT_ARMY_ART = """
    🤖 SUBORDINATE AGENT ARMY DEPLOYED 🤖
    
        👤 👤 👤 👤 👤
        👤 👤 👤 👤 👤
        👤 👤 🎭 👤 👤  ← Commander (NeuroCog)
        👤 👤 👤 👤 👤
        👤 👤 👤 👤 👤
        
    Total Agents: 20+
    Status: READY FOR DISTRIBUTED COGNITION
"""


def display_all_art():
    """Display all ASCII art"""
    arts = [
        ("OpenCog-Org", OPENCOG_ART),
        ("Marduk-v15", MARDUK_ART),
        ("Agent-Neuro", NEURO_ART),
        ("Synergy", SYNERGY_ART),
        ("Playground", PLAYGROUND_ART),
        ("Evolution", EVOLUTION_ART),
        ("Agent Army", AGENT_ARMY_ART),
    ]
    
    for name, art in arts:
        print("\n" + "="*60)
        print(f"{name}")
        print("="*60)
        print(art)
        input("\nPress Enter for next...")


def get_random_art():
    """Get a random ASCII art piece"""
    import random
    arts = [
        OPENCOG_ART, MARDUK_ART, NEURO_ART, SYNERGY_ART,
        PLAYGROUND_ART, EVOLUTION_ART, AGENT_ARMY_ART
    ]
    return random.choice(arts)


if __name__ == "__main__":
    print("\n" + "="*60)
    print("NEUROCOG ASCII ART GALLERY")
    print("="*60)
    print("\nWelcome to the ASCII art showcase!")
    print("Press Enter to see each piece...\n")
    input("Press Enter to start...")
    
    display_all_art()
    
    print("\n" + "="*60)
    print("Gallery complete! Thanks for visiting! 🎨")
    print("="*60 + "\n")
