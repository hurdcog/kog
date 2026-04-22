/**
 * ATenSpace Advanced Features Tests
 * 
 * Tests for TimeServer, AttentionBank, Serialization, and new link types
 */

#include <ATen/atomspace/ATenSpace.h>
#include <cassert>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>

using namespace at;
using namespace at::atomspace;

void test_timeserver() {
    std::cout << "Testing TimeServer..." << std::endl;
    
    AtomSpace space;
    TimeServer timeServer;
    
    auto node1 = createConceptNode(space, "test");
    
    // Record creation
    timeServer.recordCreation(node1);
    assert(timeServer.size() == 1);
    
    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Record access
    timeServer.recordAccess(node1);
    
    // Get temporal info
    auto info = timeServer.getTemporalInfo(node1);
    assert(info.lastAccessTime > info.creationTime);
    
    // Record event
    timeServer.recordEvent(node1, "test event");
    auto events = timeServer.getEventHistory(node1);
    assert(events.size() == 1);
    assert(events[0].second == "test event");
    
    // Time range queries
    auto now = std::chrono::system_clock::now();
    auto past = now - std::chrono::hours(1);
    auto future = now + std::chrono::hours(1);
    
    auto atomsCreated = timeServer.getAtomsCreatedBetween(past, future);
    assert(atomsCreated.size() == 1);
    
    std::cout << "✓ TimeServer tests passed" << std::endl;
}

void test_attention_bank() {
    std::cout << "Testing AttentionBank..." << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    
    auto node1 = createConceptNode(space, "important");
    auto node2 = createConceptNode(space, "less_important");
    auto node3 = createConceptNode(space, "not_important");
    
    // Set attention values
    attentionBank.setAttentionValue(node1, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    attentionBank.setAttentionValue(node2, AttentionBank::AttentionValue(50.0f, 25.0f, 5.0f));
    attentionBank.setAttentionValue(node3, AttentionBank::AttentionValue(10.0f, 5.0f, 1.0f));
    
    assert(attentionBank.size() == 3);
    
    // Get attention value
    auto av1 = attentionBank.getAttentionValue(node1);
    assert(av1.sti == 100.0f);
    assert(av1.lti == 50.0f);
    assert(av1.vlti == 10.0f);
    
    // Test stimulation
    attentionBank.stimulate(node2, 60.0f);
    auto av2 = attentionBank.getAttentionValue(node2);
    assert(av2.sti == 110.0f);
    
    // Test attentional focus
    attentionBank.setMaxAFSize(2);
    auto focus = attentionBank.getAttentionalFocus();
    assert(focus.size() <= 2);
    
    // Test top STI
    auto topSTI = attentionBank.getTopSTI(2);
    assert(topSTI.size() == 2);
    assert(topSTI[0].second >= topSTI[1].second);
    
    // Test threshold queries
    auto aboveThreshold = attentionBank.getAtomsAboveSTI(50.0f);
    assert(aboveThreshold.size() >= 2);
    
    // Test decay
    attentionBank.decaySTI(0.5f);
    auto av1_after = attentionBank.getAttentionValue(node1);
    assert(av1_after.sti == 50.0f);  // 100 * 0.5
    
    // Test STI transfer
    attentionBank.transferSTI(node1, node3, 20.0f);
    auto av1_final = attentionBank.getAttentionValue(node1);
    auto av3_final = attentionBank.getAttentionValue(node3);
    assert(av1_final.sti == 30.0f);  // 50 - 20
    assert(av3_final.sti == 25.0f);  // 5 (after decay) + 20
    
    std::cout << "✓ AttentionBank tests passed" << std::endl;
}

void test_new_link_types() {
    std::cout << "Testing new link types..." << std::endl;
    
    AtomSpace space;
    
    auto a = createConceptNode(space, "A");
    auto b = createConceptNode(space, "B");
    auto c = createConceptNode(space, "C");
    
    // Test VariableNode
    auto varX = createVariableNode(space, "$X");
    assert(varX != nullptr);
    assert(varX->getType() == Atom::Type::VARIABLE_NODE);
    
    // Test logical links
    auto andLink = createAndLink(space, {a, b, c});
    assert(andLink->getType() == Atom::Type::AND_LINK);
    
    auto orLink = createOrLink(space, {a, b});
    assert(orLink->getType() == Atom::Type::OR_LINK);
    
    auto notLink = createNotLink(space, a);
    assert(notLink->getType() == Atom::Type::NOT_LINK);
    
    // Test set links
    auto memberLink = createMemberLink(space, a, b);
    assert(memberLink->getType() == Atom::Type::MEMBER_LINK);
    
    auto subsetLink = createSubsetLink(space, a, b);
    assert(subsetLink->getType() == Atom::Type::SUBSET_LINK);
    
    // Test contextual links
    auto context = createConceptNode(space, "context1");
    auto contextLink = createContextLink(space, context, a);
    assert(contextLink->getType() == Atom::Type::CONTEXT_LINK);
    
    // Test temporal links
    auto seqLink = createSequentialLink(space, {a, b, c});
    assert(seqLink->getType() == Atom::Type::SEQUENTIAL_LINK);
    
    auto simLink = createSimultaneousLink(space, {a, b});
    assert(simLink->getType() == Atom::Type::SIMULTANEOUS_LINK);
    
    // Test similarity link
    auto simLink2 = createSimilarityLink(space, a, b);
    assert(simLink2->getType() == Atom::Type::SIMILARITY_LINK);
    
    // Test execution link
    auto proc = createConceptNode(space, "procedure");
    auto execLink = createExecutionLink(space, proc, {a, b});
    assert(execLink->getType() == Atom::Type::EXECUTION_LINK);
    
    std::cout << "✓ New link types tests passed" << std::endl;
}

void test_serialization() {
    std::cout << "Testing serialization..." << std::endl;
    
    // Create a knowledge graph
    AtomSpace space1;
    
    auto cat = createConceptNode(space1, "cat");
    auto dog = createConceptNode(space1, "dog");
    auto mammal = createConceptNode(space1, "mammal");
    
    cat->setTruthValue(torch::tensor({0.95f, 0.9f}));
    cat->setAttention(0.8f);
    
    auto inh1 = createInheritanceLink(space1, cat, mammal);
    auto inh2 = createInheritanceLink(space1, dog, mammal);
    
    // Save to file
    std::string filename = "/tmp/test_atomspace.txt";
    bool saved = Serializer::save(space1, filename);
    assert(saved);
    
    // Check file exists
    std::ifstream file(filename);
    assert(file.good());
    file.close();
    
    // Load into new atomspace
    AtomSpace space2;
    bool loaded = Serializer::load(space2, filename);
    assert(loaded);
    
    // Verify loaded atoms
    auto cat2 = space2.getNode(Atom::Type::CONCEPT_NODE, "cat");
    assert(cat2 != nullptr);
    
    auto tv = cat2->getTruthValue();
    assert(tv.defined());
    assert(std::abs(tv[0].item<float>() - 0.95f) < 0.001f);
    assert(std::abs(tv[1].item<float>() - 0.9f) < 0.001f);
    assert(std::abs(cat2->getAttention() - 0.8f) < 0.001f);
    
    // Test toString
    std::string str = Serializer::toString(space1);
    assert(!str.empty());
    assert(str.find("cat") != std::string::npos);
    
    // Cleanup
    std::remove(filename.c_str());
    
    std::cout << "✓ Serialization tests passed" << std::endl;
}

void test_integration() {
    std::cout << "Testing integration of all features..." << std::endl;
    
    AtomSpace space;
    TimeServer timeServer;
    AttentionBank attentionBank;
    
    // Create a knowledge graph with temporal and attention tracking
    auto concept1 = createConceptNode(space, "concept1");
    timeServer.recordCreation(concept1);
    attentionBank.setAttentionValue(concept1, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    
    auto concept2 = createConceptNode(space, "concept2");
    timeServer.recordCreation(concept2);
    attentionBank.setAttentionValue(concept2, AttentionBank::AttentionValue(80.0f, 40.0f, 8.0f));
    
    // Create relationships
    auto simLink = createSimilarityLink(space, concept1, concept2);
    timeServer.recordCreation(simLink);
    
    // Access tracking
    timeServer.recordAccess(concept1);
    attentionBank.stimulate(concept1, 20.0f);
    
    // Verify everything works together
    auto av = attentionBank.getAttentionValue(concept1);
    assert(av.sti == 120.0f);
    
    auto info = timeServer.getTemporalInfo(concept1);
    assert(info.lastAccessTime >= info.creationTime);
    
    // Serialize the integrated knowledge
    std::string filename = "/tmp/test_integrated.txt";
    Serializer::save(space, filename);
    
    // Load and verify
    AtomSpace space2;
    Serializer::load(space2, filename);
    assert(space2.size() >= 3);
    
    std::remove(filename.c_str());
    
    std::cout << "✓ Integration tests passed" << std::endl;
}

int main() {
    std::cout << "=== ATenSpace Advanced Features Tests ===" << std::endl << std::endl;
    
    try {
        test_timeserver();
        test_attention_bank();
        test_new_link_types();
        test_serialization();
        test_integration();
        
        std::cout << "\n=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
