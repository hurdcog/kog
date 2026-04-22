#include "ATenSpace.h"
#include <iostream>
#include <iomanip>

using namespace at::atomspace;

void printAttentionValues(const std::string& label, AttentionBank& bank, const std::vector<Atom::Handle>& atoms) {
    std::cout << "\n" << label << ":" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    std::cout << std::left << std::setw(20) << "Atom" 
              << std::setw(12) << "STI" 
              << std::setw(12) << "LTI" 
              << std::setw(12) << "VLTI" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    for (const auto& atom : atoms) {
        auto av = bank.getAttentionValue(atom);
        auto node = std::dynamic_pointer_cast<Node>(atom);
        if (node) {
            std::cout << std::left << std::setw(20) << node->getName()
                      << std::setw(12) << std::fixed << std::setprecision(2) << av.sti
                      << std::setw(12) << av.lti
                      << std::setw(12) << av.vlti << std::endl;
        }
    }
    std::cout << std::string(60, '-') << std::endl;
}

void example1_BasicECAN() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 1: Basic ECAN Operations" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Create atomspace and attention bank
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create a simple knowledge graph
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    
    // Set initial attention values
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(80.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(20.0f, 50.0f, 10.0f));
    
    std::vector<Atom::Handle> atoms = {cat, dog, mammal};
    printAttentionValues("Initial Attention Values", attentionBank, atoms);
    
    // Run ECAN cycle
    std::cout << "\nRunning ECAN cycle..." << std::endl;
    ecan.runCycle();
    
    printAttentionValues("After ECAN Cycle", attentionBank, atoms);
    
    std::cout << "\nStatistics:" << std::endl;
    std::cout << "  Hebbian links formed: " << ecan.getHebbianLinkCount() << std::endl;
    std::cout << "  ECAN cycles run: " << ecan.getCycleCount() << std::endl;
}

void example2_AttentionSpreading() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 2: Attention Spreading via Hebbian Links" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create animal taxonomy
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto fish = createConceptNode(space, "fish");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    // Create knowledge structure
    createInheritanceLink(space, cat, mammal);
    createInheritanceLink(space, dog, mammal);
    createInheritanceLink(space, mammal, animal);
    
    // Initial focus on specific animals
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 60.0f, 15.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(100.0f, 60.0f, 15.0f));
    attentionBank.setAttentionValue(fish, AttentionBank::AttentionValue(50.0f, 40.0f, 10.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(20.0f, 70.0f, 20.0f));
    attentionBank.setAttentionValue(animal, AttentionBank::AttentionValue(10.0f, 80.0f, 25.0f));
    
    std::vector<Atom::Handle> atoms = {cat, dog, fish, mammal, animal};
    printAttentionValues("Initial State", attentionBank, atoms);
    
    // Configure spreading
    ecan.getSpreadingAgent().setSpreadPercentage(0.15f);  // Spread 15%
    ecan.getRentAgent().setRentRate(0.5f);  // Low rent
    
    // Simulate multiple cognitive cycles
    std::cout << "\nSimulating 5 cognitive cycles with attention spreading..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        ecan.runCycle();
        
        // Show attentional focus
        auto focus = attentionBank.getAttentionalFocus();
        std::cout << "  Cycle " << (i + 1) << " - Focus: ";
        for (const auto& atom : focus) {
            auto node = std::dynamic_pointer_cast<Node>(atom);
            if (node) std::cout << node->getName() << " ";
        }
        std::cout << std::endl;
    }
    
    printAttentionValues("After 5 Cycles", attentionBank, atoms);
    
    std::cout << "\nNote: Attention spreads from focused atoms (cat, dog) to connected atoms" << std::endl;
    std::cout << "via Hebbian links formed through co-occurrence in attentional focus." << std::endl;
    std::cout << "\nHebbian links: " << ecan.getHebbianLinkCount() << std::endl;
}

void example3_WagesAndRent() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 3: Economic Dynamics - Wages and Rent" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create concepts
    auto useful = createConceptNode(space, "useful_concept");
    auto neutral = createConceptNode(space, "neutral_concept");
    auto unused = createConceptNode(space, "unused_concept");
    
    // Set initial attention
    attentionBank.setAttentionValue(useful, AttentionBank::AttentionValue(80.0f, 70.0f, 15.0f));
    attentionBank.setAttentionValue(neutral, AttentionBank::AttentionValue(80.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(unused, AttentionBank::AttentionValue(80.0f, 30.0f, 5.0f));
    
    // Configure economic parameters
    ecan.getRentAgent().setRentRate(2.0f);     // Rent costs 2 STI per cycle
    ecan.getWageAgent().setWageAmount(10.0f);  // Wages pay 10 STI
    
    std::vector<Atom::Handle> atoms = {useful, neutral, unused};
    printAttentionValues("Initial State", attentionBank, atoms);
    
    std::cout << "\nSimulating 10 cycles with economic dynamics..." << std::endl;
    std::cout << "  - 'useful_concept' receives wages every 2 cycles (used in reasoning)" << std::endl;
    std::cout << "  - 'neutral_concept' occasionally receives wages" << std::endl;
    std::cout << "  - 'unused_concept' never receives wages" << std::endl;
    std::cout << "  - All atoms pay rent each cycle\n" << std::endl;
    
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Run ECAN (includes rent collection)
        ecan.runCycle();
        
        // Pay wages to atoms "used" in cognitive processes
        if (cycle % 2 == 0) {
            ecan.payWage(useful);
            std::cout << "  Cycle " << (cycle + 1) << ": Paid wage to useful_concept" << std::endl;
        }
        if (cycle % 5 == 0) {
            ecan.payWage(neutral);
            std::cout << "  Cycle " << (cycle + 1) << ": Paid wage to neutral_concept" << std::endl;
        }
    }
    
    printAttentionValues("\nFinal State (After 10 Cycles)", attentionBank, atoms);
    
    std::cout << "\nObservation: Useful atoms gain STI (wages > rent), while unused atoms" << std::endl;
    std::cout << "lose STI (only paying rent) and may eventually leave attentional focus." << std::endl;
}

void example4_ForgettingMechanism() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 4: Forgetting Low-Importance Atoms" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create atoms with varying long-term importance
    std::vector<std::pair<std::string, float>> atom_data = {
        {"critical_knowledge", 100.0f},
        {"important_fact", 75.0f},
        {"useful_info", 50.0f},
        {"minor_detail", 25.0f},
        {"trivial_fact", 5.0f},
        {"outdated_info", -10.0f}
    };
    
    std::vector<Atom::Handle> atoms;
    for (const auto& [name, lti] : atom_data) {
        auto atom = createConceptNode(space, name);
        attentionBank.setAttentionValue(atom, AttentionBank::AttentionValue(50.0f, lti, 10.0f));
        atoms.push_back(atom);
    }
    
    printAttentionValues("Initial State", attentionBank, atoms);
    
    // Configure forgetting
    ecan.getForgettingAgent().setLTIThreshold(10.0f);  // Forget atoms with LTI < 10
    
    size_t initial_count = attentionBank.size();
    std::cout << "\nInitial atoms tracked: " << initial_count << std::endl;
    
    // Run multiple cycles (forgetting runs every 10 cycles)
    std::cout << "Running 20 ECAN cycles (forgetting runs every 10 cycles)..." << std::endl;
    for (int i = 0; i < 20; ++i) {
        ecan.runCycle();
    }
    
    size_t final_count = attentionBank.size();
    size_t forgotten = ecan.getAtomsForgotten();
    
    std::cout << "Final atoms tracked: " << final_count << std::endl;
    std::cout << "Atoms forgotten: " << forgotten << std::endl;
    
    printAttentionValues("\nRemaining Atoms", attentionBank, atoms);
    
    std::cout << "\nNote: Atoms with LTI < 10 were removed from attention tracking," << std::endl;
    std::cout << "simulating memory forgetting of unimportant knowledge." << std::endl;
}

void example5_IntegratedCognitiveSystem() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 5: Integrated Cognitive System with ECAN + PLN" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create knowledge base
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    auto furry = createConceptNode(space, "furry");
    
    // Create knowledge structure with truth values
    auto inh1 = createInheritanceLink(space, cat, mammal);
    auto inh2 = createInheritanceLink(space, dog, mammal);
    auto inh3 = createInheritanceLink(space, mammal, animal);
    
    inh1->setTruthValue(torch::tensor({0.95f, 0.9f}));
    inh2->setTruthValue(torch::tensor({0.95f, 0.9f}));
    inh3->setTruthValue(torch::tensor({0.98f, 0.95f}));
    
    // Set initial attention - focus on cat
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 60.0f, 15.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(30.0f, 60.0f, 15.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(20.0f, 70.0f, 20.0f));
    attentionBank.setAttentionValue(animal, AttentionBank::AttentionValue(10.0f, 80.0f, 25.0f));
    attentionBank.setAttentionValue(furry, AttentionBank::AttentionValue(15.0f, 40.0f, 10.0f));
    
    // Configure ECAN
    ecan.getSpreadingAgent().setSpreadPercentage(0.12f);
    ecan.getRentAgent().setRentRate(1.5f);
    ecan.getWageAgent().setWageAmount(8.0f);
    attentionBank.setMaxAFSize(4);  // Limited attentional focus
    
    std::vector<Atom::Handle> atoms = {cat, dog, mammal, animal, furry};
    printAttentionValues("Initial State", attentionBank, atoms);
    
    // Simulate cognitive processing
    std::cout << "\nSimulating cognitive processing with attention-guided reasoning..." << std::endl;
    
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Run ECAN cycle
        ecan.runCycle();
        
        // Show current focus
        auto focus = attentionBank.getAttentionalFocus();
        std::cout << "\nCycle " << (cycle + 1) << " - Attentional Focus: ";
        for (const auto& atom : focus) {
            auto node = std::dynamic_pointer_cast<Node>(atom);
            if (node) std::cout << node->getName() << " ";
        }
        std::cout << std::endl;
        
        // Simulate reasoning using focused atoms
        // In a real system, this would be ForwardChainer or BackwardChainer
        if (cycle == 2) {
            std::cout << "  → Reasoning about 'cat' (using in inference)" << std::endl;
            ecan.payWage(cat);
            ecan.payWage(mammal);  // mammal also used
        }
        
        if (cycle == 5) {
            std::cout << "  → Reasoning about 'dog' (using in inference)" << std::endl;
            ecan.payWage(dog);
            ecan.payWage(mammal);  // mammal also used
        }
        
        if (cycle == 8) {
            std::cout << "  → Reasoning about taxonomy (using animal)" << std::endl;
            ecan.payWage(animal);
        }
    }
    
    printAttentionValues("\nFinal State", attentionBank, atoms);
    
    std::cout << "\nECAN Statistics:" << std::endl;
    std::cout << "  Total cycles: " << ecan.getCycleCount() << std::endl;
    std::cout << "  Hebbian links: " << ecan.getHebbianLinkCount() << std::endl;
    std::cout << "  Atoms forgotten: " << ecan.getAtomsForgotten() << std::endl;
    
    // Show Hebbian network
    std::cout << "\nHebbian Link Network:" << std::endl;
    for (const auto& atom : atoms) {
        auto neighbors = ecan.getHebbianManager().getHebbianNeighbors(atom);
        if (!neighbors.empty()) {
            auto node = std::dynamic_pointer_cast<Node>(atom);
            std::cout << "  " << node->getName() << " → ";
            for (const auto& [neighbor, strength] : neighbors) {
                auto neighbor_node = std::dynamic_pointer_cast<Node>(neighbor);
                std::cout << neighbor_node->getName() << "(s=" << std::fixed 
                         << std::setprecision(2) << strength << ") ";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "\nObservation: ECAN creates attention dynamics that guide reasoning." << std::endl;
    std::cout << "Atoms used in inference receive wages and stay in focus, while" << std::endl;
    std::cout << "unused atoms fade from attention, creating efficient resource allocation." << std::endl;
}

void example6_HebbianLearning() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "Example 6: Hebbian Learning and Association" << std::endl;
    std::cout << "========================================" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    HebbianLinkManager hebbianManager(space, attentionBank);
    
    // Create concepts
    auto coffee = createConceptNode(space, "coffee");
    auto morning = createConceptNode(space, "morning");
    auto tired = createConceptNode(space, "tired");
    auto alert = createConceptNode(space, "alert");
    
    std::cout << "Concepts created: coffee, morning, tired, alert" << std::endl;
    std::cout << "\nSimulating co-occurrence patterns..." << std::endl;
    
    // Simulate different co-occurrence patterns
    struct Pattern {
        std::vector<Atom::Handle> atoms;
        std::string description;
    };
    
    std::vector<Pattern> patterns = {
        {{coffee, morning}, "coffee with morning"},
        {{coffee, morning}, "coffee with morning (repeated)"},
        {{coffee, alert}, "coffee with alert"},
        {{tired, morning}, "tired in morning"},
        {{coffee, morning, alert}, "coffee, morning, alert together"}
    };
    
    for (size_t i = 0; i < patterns.size(); ++i) {
        const auto& pattern = patterns[i];
        
        // Bring atoms into focus
        for (const auto& atom : pattern.atoms) {
            attentionBank.setAttentionValue(atom, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
        }
        
        // Update Hebbian links based on co-occurrence
        hebbianManager.updateHebbianLinks();
        
        std::cout << "  Pattern " << (i + 1) << ": " << pattern.description << std::endl;
        
        // Reset attention
        for (const auto& atom : pattern.atoms) {
            attentionBank.setAttentionValue(atom, AttentionBank::AttentionValue(10.0f, 50.0f, 10.0f));
        }
    }
    
    std::cout << "\nLearned Associations (Hebbian Links):" << std::endl;
    std::cout << std::string(60, '-') << std::endl;
    
    std::vector<Atom::Handle> all_atoms = {coffee, morning, tired, alert};
    for (const auto& atom : all_atoms) {
        auto neighbors = hebbianManager.getHebbianNeighbors(atom);
        if (!neighbors.empty()) {
            auto node = std::dynamic_pointer_cast<Node>(atom);
            std::cout << node->getName() << " is associated with:" << std::endl;
            for (const auto& [neighbor, strength] : neighbors) {
                auto neighbor_node = std::dynamic_pointer_cast<Node>(neighbor);
                std::cout << "    → " << std::left << std::setw(15) << neighbor_node->getName()
                         << " (strength: " << std::fixed << std::setprecision(3) 
                         << strength << ")" << std::endl;
            }
        }
    }
    
    std::cout << "\nTotal Hebbian links: " << hebbianManager.getHebbianLinkCount() << std::endl;
    std::cout << "\nNote: Link strength increases with repeated co-occurrence," << std::endl;
    std::cout << "implementing Hebbian learning: 'neurons that fire together, wire together.'" << std::endl;
}

int main() {
    std::cout << "==========================================" << std::endl;
    std::cout << "  ATenSpace ECAN Examples" << std::endl;
    std::cout << "  Economic Attention Networks" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        example1_BasicECAN();
        example2_AttentionSpreading();
        example3_WagesAndRent();
        example4_ForgettingMechanism();
        example5_IntegratedCognitiveSystem();
        example6_HebbianLearning();
        
        std::cout << "\n==========================================" << std::endl;
        std::cout << "  All examples completed successfully!" << std::endl;
        std::cout << "==========================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nExample failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
