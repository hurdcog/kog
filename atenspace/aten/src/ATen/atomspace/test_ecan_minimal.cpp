#include "Atom.h"
#include "AtomSpace.h"
#include "AttentionBank.h"
#include "ECAN.h"
#include <iostream>

using namespace at::atomspace;

int main() {
    std::cout << "Testing ECAN compilation..." << std::endl;
    
    // Create atomspace and attention bank
    AtomSpace space;
    AttentionBank attentionBank;
    ECANManager ecan(space, attentionBank);
    
    // Create test atoms
    auto cat = space.addNode(Atom::Type::CONCEPT_NODE, "cat");
    auto dog = space.addNode(Atom::Type::CONCEPT_NODE, "dog");
    
    // Set attention
    attentionBank.setAttentionValue(cat, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(dog, AttentionBank::AttentionValue(80.0f, 50.0f, 10.0f));
    
    // Run ECAN cycle
    ecan.runCycle();
    
    std::cout << "ECAN cycle completed successfully!" << std::endl;
    std::cout << "Hebbian links: " << ecan.getHebbianLinkCount() << std::endl;
    std::cout << "Cycles run: " << ecan.getCycleCount() << std::endl;
    
    return 0;
}
