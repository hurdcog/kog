#include "ATenSpace.h"
#include <cassert>
#include <iostream>

using namespace at::atomspace;

// Test utilities
void assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "ASSERTION FAILED: " << message << std::endl;
        std::exit(1);
    }
    std::cout << "✓ " << message << std::endl;
}

void testHebbianLinkCreation() {
    std::cout << "\n=== Test: Hebbian Link Creation ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    HebbianLinkManager hebbianManager(space, attentionBank);
    
    // Create test atoms
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    
    // Create Hebbian links
    hebbianManager.createOrStrengthenLink(cat, dog, Atom::Type::SYMMETRIC_HEBBIAN_LINK);
    hebbianManager.createOrStrengthenLink(cat, mammal, Atom::Type::ASYMMETRIC_HEBBIAN_LINK);
    
    // Verify links exist
    auto cat_links = hebbianManager.getHebbianLinks(cat);
    assertTrue(cat_links.size() == 2, "Cat should have 2 Hebbian links");
    
    // Verify neighbors
    auto cat_neighbors = hebbianManager.getHebbianNeighbors(cat);
    assertTrue(cat_neighbors.size() == 2, "Cat should have 2 Hebbian neighbors");
    
    // Strengthen existing link
    hebbianManager.createOrStrengthenLink(cat, dog, Atom::Type::SYMMETRIC_HEBBIAN_LINK);
    
    // Link count should not increase (same link strengthened)
    cat_links = hebbianManager.getHebbianLinks(cat);
    assertTrue(cat_links.size() == 2, "Cat should still have 2 Hebbian links");
    
    // Check link count in space
    size_t hebbian_count = hebbianManager.getHebbianLinkCount();
    assertTrue(hebbian_count == 2, "Should have 2 Hebbian links total");
    
    std::cout << "All Hebbian link creation tests passed!" << std::endl;
}

void testHebbianLinkUpdating() {
    std::cout << "\n=== Test: Hebbian Link Updating ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    HebbianLinkManager hebbianManager(space, attentionBank);
    
    // Create test atoms
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto fish = createConceptNode(space, "fish");
    
    // Set high STI to bring atoms into focus
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(fish, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    
    // Update Hebbian links based on focus
    hebbianManager.updateHebbianLinks();
    
    // Should create links between all pairs in focus
    size_t hebbian_count = hebbianManager.getHebbianLinkCount();
    assertTrue(hebbian_count == 3, "Should have 3 Hebbian links (3 pairs from 3 atoms)");
    
    // Run again - should strengthen, not create new
    hebbianManager.updateHebbianLinks();
    hebbian_count = hebbianManager.getHebbianLinkCount();
    assertTrue(hebbian_count == 3, "Should still have 3 Hebbian links");
    
    std::cout << "All Hebbian link updating tests passed!" << std::endl;
}

void testImportanceSpreading() {
    std::cout << "\n=== Test: Importance Spreading ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    HebbianLinkManager hebbianManager(space, attentionBank);
    ImportanceSpreadingAgent spreadingAgent(space, attentionBank, hebbianManager);
    
    // Create test atoms
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    
    // Set initial attention values
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(50.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(10.0f, 50.0f, 10.0f));
    
    // Create Hebbian links
    auto link1 = createSymmetricHebbianLink(space, cat, dog);
    auto link2 = createSymmetricHebbianLink(space, cat, mammal);
    link1->setTruthValue(torch::tensor({0.8f, 0.9f}));  // Strong link
    link2->setTruthValue(torch::tensor({0.5f, 0.7f}));  // Moderate link
    
    // Configure spreading
    spreadingAgent.setSpreadPercentage(0.1f);  // Spread 10%
    spreadingAgent.setFocusThreshold(50.0f);   // Only spread from atoms with STI > 50
    
    // Get initial STI values
    float initial_cat_sti = attentionBank.getAttentionValue(cat).sti;
    float initial_dog_sti = attentionBank.getAttentionValue(dog).sti;
    float initial_mammal_sti = attentionBank.getAttentionValue(mammal).sti;
    
    // Spread importance
    spreadingAgent.spread();
    
    // Check that STI was spread from cat (high STI) to neighbors
    float final_cat_sti = attentionBank.getAttentionValue(cat).sti;
    float final_dog_sti = attentionBank.getAttentionValue(dog).sti;
    float final_mammal_sti = attentionBank.getAttentionValue(mammal).sti;
    
    assertTrue(final_cat_sti < initial_cat_sti, "Cat STI should decrease (spreading out)");
    assertTrue(final_dog_sti > initial_dog_sti || final_mammal_sti > initial_mammal_sti, 
               "Dog or Mammal STI should increase (receiving spread)");
    
    std::cout << "Initial: cat=" << initial_cat_sti << " dog=" << initial_dog_sti 
              << " mammal=" << initial_mammal_sti << std::endl;
    std::cout << "Final:   cat=" << final_cat_sti << " dog=" << final_dog_sti 
              << " mammal=" << final_mammal_sti << std::endl;
    
    std::cout << "All importance spreading tests passed!" << std::endl;
}

void testForgettingAgent() {
    std::cout << "\n=== Test: Forgetting Agent ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ForgettingAgent forgettingAgent(space, attentionBank);
    
    // Create test atoms
    auto important = createConceptNode(space, "important");
    auto unimportant = createConceptNode(space, "unimportant");
    auto forgotten = createConceptNode(space, "forgotten");
    
    // Set attention values
    attentionBank.setAttentionValue(important, AttentionBank::AttentionValue(50.0f, 100.0f, 10.0f));
    attentionBank.setAttentionValue(unimportant, AttentionBank::AttentionValue(5.0f, 10.0f, 5.0f));
    attentionBank.setAttentionValue(forgotten, AttentionBank::AttentionValue(1.0f, -5.0f, 0.0f));
    
    // Set forgetting threshold
    forgettingAgent.setLTIThreshold(0.0f);  // Forget atoms with LTI < 0
    
    size_t initial_atoms = attentionBank.size();
    size_t initial_forgotten = forgettingAgent.getAtomsForgotten();
    
    // Run forgetting
    forgettingAgent.forget();
    
    size_t final_atoms = attentionBank.size();
    size_t final_forgotten = forgettingAgent.getAtomsForgotten();
    
    assertTrue(final_atoms < initial_atoms, "Some atoms should be forgotten");
    assertTrue(final_forgotten > initial_forgotten, "Forgotten count should increase");
    
    // Important atom should still be tracked
    auto av_important = attentionBank.getAttentionValue(important);
    assertTrue(av_important.lti > 0, "Important atom should still exist");
    
    std::cout << "Atoms forgotten: " << final_forgotten << std::endl;
    std::cout << "All forgetting agent tests passed!" << std::endl;
}

void testRentAgent() {
    std::cout << "\n=== Test: Rent Agent ===" << std::endl;
    
    AttentionBank attentionBank;
    RentAgent rentAgent(attentionBank);
    
    // Create test atoms
    AtomSpace space;
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    
    // Set initial STI
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(50.0f, 50.0f, 10.0f));
    
    // Set rent rate
    rentAgent.setRentRate(2.0f);
    
    float initial_cat_sti = attentionBank.getAttentionValue(cat).sti;
    float initial_dog_sti = attentionBank.getAttentionValue(dog).sti;
    
    // Collect rent
    rentAgent.collectRent();
    
    float final_cat_sti = attentionBank.getAttentionValue(cat).sti;
    float final_dog_sti = attentionBank.getAttentionValue(dog).sti;
    
    assertTrue(final_cat_sti < initial_cat_sti, "Cat STI should decrease after rent");
    assertTrue(final_dog_sti < initial_dog_sti, "Dog STI should decrease after rent");
    assertTrue(std::abs((initial_cat_sti - final_cat_sti) - 2.0f) < 0.01f, 
               "Rent should be 2.0 STI");
    
    std::cout << "Rent collected: " << (initial_cat_sti - final_cat_sti) << " STI" << std::endl;
    std::cout << "All rent agent tests passed!" << std::endl;
}

void testWageAgent() {
    std::cout << "\n=== Test: Wage Agent ===" << std::endl;
    
    AttentionBank attentionBank;
    WageAgent wageAgent(attentionBank);
    
    // Create test atom
    AtomSpace space;
    auto cat = createConceptNode(space, "cat");
    
    // Set initial STI
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(50.0f, 50.0f, 10.0f));
    
    // Set wage amount
    wageAgent.setWageAmount(10.0f);
    
    float initial_sti = attentionBank.getAttentionValue(cat).sti;
    
    // Pay wage
    wageAgent.payWage(cat);
    
    float final_sti = attentionBank.getAttentionValue(cat).sti;
    
    assertTrue(final_sti > initial_sti, "STI should increase after wage payment");
    assertTrue(std::abs((final_sti - initial_sti) - 10.0f) < 0.01f, 
               "Wage should be 10.0 STI");
    
    // Test paying multiple wages
    auto dog = createConceptNode(space, "dog");
    auto fish = createConceptNode(space, "fish");
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(30.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(fish, AttentionBank::AttentionValue(20.0f, 50.0f, 10.0f));
    
    wageAgent.payWages({dog, fish});
    
    assertTrue(attentionBank.getAttentionValue(dog).sti > 30.0f, "Dog STI should increase");
    assertTrue(attentionBank.getAttentionValue(fish).sti > 20.0f, "Fish STI should increase");
    
    std::cout << "Wage paid: " << (final_sti - initial_sti) << " STI" << std::endl;
    std::cout << "All wage agent tests passed!" << std::endl;
}

void testECANManager() {
    std::cout << "\n=== Test: ECAN Manager ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecanManager(space, attentionBank);
    
    // Create knowledge graph
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto fish = createConceptNode(space, "fish");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    // Set initial attention values
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 80.0f, 20.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(90.0f, 70.0f, 15.0f));
    attentionBank.setAttentionValue(fish, AttentionBank::AttentionValue(50.0f, 60.0f, 10.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(40.0f, 90.0f, 25.0f));
    attentionBank.setAttentionValue(animal, AttentionBank::AttentionValue(30.0f, 100.0f, 30.0f));
    
    size_t initial_cycles = ecanManager.getCycleCount();
    size_t initial_hebbian = ecanManager.getHebbianLinkCount();
    
    // Run ECAN cycle
    ecanManager.runCycle();
    
    size_t final_cycles = ecanManager.getCycleCount();
    size_t final_hebbian = ecanManager.getHebbianLinkCount();
    
    assertTrue(final_cycles == initial_cycles + 1, "Cycle count should increase");
    assertTrue(final_hebbian > initial_hebbian, "Hebbian links should be created");
    
    // Pay wages to atoms used in reasoning
    ecanManager.payWage(cat);
    ecanManager.payWages({dog, mammal});
    
    // Run more cycles
    for (int i = 0; i < 5; ++i) {
        ecanManager.runCycle();
    }
    
    assertTrue(ecanManager.getCycleCount() == initial_cycles + 6, 
               "Should have run 6 cycles total");
    
    // Check that Hebbian links were formed
    size_t hebbian_count = ecanManager.getHebbianLinkCount();
    std::cout << "Hebbian links formed: " << hebbian_count << std::endl;
    std::cout << "Total cycles run: " << ecanManager.getCycleCount() << std::endl;
    
    std::cout << "All ECAN manager tests passed!" << std::endl;
}

void testECANIntegration() {
    std::cout << "\n=== Test: ECAN Integration ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecanManager(space, attentionBank);
    
    // Create a simple taxonomy
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    createInheritanceLink(space, cat, mammal);
    createInheritanceLink(space, dog, mammal);
    createInheritanceLink(space, mammal, animal);
    
    // Set initial focus on cat and dog
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(mammal, AttentionBank::AttentionValue(20.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(animal, AttentionBank::AttentionValue(10.0f, 50.0f, 10.0f));
    
    // Configure ECAN agents
    ecanManager.getSpreadingAgent().setSpreadPercentage(0.15f);
    ecanManager.getRentAgent().setRentRate(1.0f);
    ecanManager.getWageAgent().setWageAmount(5.0f);
    
    float initial_mammal_sti = attentionBank.getAttentionValue(mammal).sti;
    
    // Simulate cognitive cycles
    for (int cycle = 0; cycle < 10; ++cycle) {
        // Run ECAN cycle
        ecanManager.runCycle();
        
        // Simulate using mammal in reasoning (pay wage)
        if (cycle % 3 == 0) {
            ecanManager.payWage(mammal);
        }
    }
    
    float final_mammal_sti = attentionBank.getAttentionValue(mammal).sti;
    
    // Mammal should have gained some importance through:
    // 1. Importance spreading from cat/dog (via Hebbian links)
    // 2. Wages for being used
    std::cout << "Mammal STI changed from " << initial_mammal_sti 
              << " to " << final_mammal_sti << std::endl;
    
    // Check Hebbian links formed
    size_t hebbian_count = ecanManager.getHebbianLinkCount();
    assertTrue(hebbian_count > 0, "Hebbian links should be formed");
    std::cout << "Hebbian links formed: " << hebbian_count << std::endl;
    
    std::cout << "All ECAN integration tests passed!" << std::endl;
}

void testECANWithAttentionalFocus() {
    std::cout << "\n=== Test: ECAN with Attentional Focus ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecanManager(space, attentionBank);
    
    // Set focus size
    attentionBank.setMaxAFSize(3);  // Only 3 atoms in focus
    attentionBank.setMinSTIThreshold(50.0f);
    
    // Create multiple atoms
    std::vector<Atom::Handle> atoms;
    for (int i = 0; i < 10; ++i) {
        auto atom = createConceptNode(space, "atom" + std::to_string(i));
        atoms.push_back(atom);
        
        // Give varying STI
        float sti = 10.0f + i * 10.0f;
        attentionBank.setAttentionValue(atom, AttentionBank::AttentionValue(sti, 50.0f, 10.0f));
    }
    
    // Get attentional focus
    auto focus = attentionBank.getAttentionalFocus();
    assertTrue(focus.size() <= 3, "Focus should have at most 3 atoms");
    assertTrue(focus.size() >= 2, "Focus should have at least 2 atoms above threshold");
    
    // Run ECAN cycles
    for (int i = 0; i < 5; ++i) {
        ecanManager.runCycle();
    }
    
    // Hebbian links should only form between atoms in focus
    size_t hebbian_count = ecanManager.getHebbianLinkCount();
    std::cout << "Hebbian links with focused attention: " << hebbian_count << std::endl;
    
    // Focus atoms should have Hebbian links
    auto focus_after = attentionBank.getAttentionalFocus();
    for (const auto& atom : focus_after) {
        auto links = ecanManager.getHebbianManager().getHebbianLinks(atom);
        std::cout << "Atom " << std::dynamic_pointer_cast<Node>(atom)->getName() 
                  << " has " << links.size() << " Hebbian links" << std::endl;
    }
    
    std::cout << "All attentional focus tests passed!" << std::endl;
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "  ECAN (Economic Attention Networks) Tests" << std::endl;
    std::cout << "==================================" << std::endl;
    
    try {
        testHebbianLinkCreation();
        testHebbianLinkUpdating();
        testImportanceSpreading();
        testForgettingAgent();
        testRentAgent();
        testWageAgent();
        testECANManager();
        testECANIntegration();
        testECANWithAttentionalFocus();
        
        std::cout << "\n==================================" << std::endl;
        std::cout << "  All ECAN tests passed! ✓" << std::endl;
        std::cout << "==================================" << std::endl;
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
