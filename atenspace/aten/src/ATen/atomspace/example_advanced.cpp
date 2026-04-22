/**
 * ATenSpace Advanced Features Example
 * 
 * Demonstrates TimeServer, AttentionBank, Serialization, and new link types
 */

#include <ATen/atomspace/ATenSpace.h>
#include <iostream>
#include <chrono>
#include <thread>

using namespace at;
using namespace at::atomspace;

void example_timeserver() {
    std::cout << "\n=== TimeServer Example ===" << std::endl;
    
    AtomSpace space;
    TimeServer timeServer;
    
    // Create knowledge with temporal tracking
    auto event1 = createConceptNode(space, "morning_meeting");
    timeServer.recordCreation(event1);
    timeServer.recordEvent(event1, "scheduled");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto event2 = createConceptNode(space, "lunch");
    timeServer.recordCreation(event2);
    timeServer.recordEvent(event2, "started");
    
    // Create temporal sequence
    auto sequence = createSequentialLink(space, {event1, event2});
    timeServer.recordCreation(sequence);
    
    // Query temporal information
    auto info1 = timeServer.getTemporalInfo(event1);
    auto events = timeServer.getEventHistory(event1);
    
    std::cout << "Event 1 has " << events.size() << " recorded events" << std::endl;
    for (const auto& [time, desc] : events) {
        std::cout << "  - " << desc << std::endl;
    }
    
    // Time range queries
    auto now = std::chrono::system_clock::now();
    auto past = now - std::chrono::hours(1);
    auto recentAtoms = timeServer.getAtomsCreatedBetween(past, now);
    std::cout << "Created " << recentAtoms.size() << " atoms in the last hour" << std::endl;
}

void example_attention_bank() {
    std::cout << "\n=== AttentionBank Example ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    
    // Create concepts with different importance levels
    auto urgent = createConceptNode(space, "urgent_task");
    auto important = createConceptNode(space, "important_project");
    auto routine = createConceptNode(space, "routine_work");
    auto background = createConceptNode(space, "background_task");
    
    // Set initial attention values
    attentionBank.setAttentionValue(urgent, AttentionBank::AttentionValue(150.0f, 80.0f, 20.0f));
    attentionBank.setAttentionValue(important, AttentionBank::AttentionValue(120.0f, 100.0f, 50.0f));
    attentionBank.setAttentionValue(routine, AttentionBank::AttentionValue(50.0f, 60.0f, 30.0f));
    attentionBank.setAttentionValue(background, AttentionBank::AttentionValue(20.0f, 40.0f, 20.0f));
    
    // Get attentional focus (most important atoms)
    attentionBank.setMaxAFSize(3);
    auto focus = attentionBank.getAttentionalFocus();
    
    std::cout << "Attentional Focus (top " << focus.size() << " atoms):" << std::endl;
    for (const auto& atom : focus) {
        auto nodePtr = std::dynamic_pointer_cast<Node>(atom);
        if (nodePtr) {
            auto av = attentionBank.getAttentionValue(atom);
            std::cout << "  - " << nodePtr->getName() 
                      << " (STI: " << av.sti << ", LTI: " << av.lti << ")" << std::endl;
        }
    }
    
    // Simulate attention dynamics
    std::cout << "\nSimulating attention dynamics..." << std::endl;
    attentionBank.stimulate(routine, 100.0f);  // Routine work becomes urgent
    attentionBank.decaySTI(0.8f);  // All STI values decay
    
    auto topSTI = attentionBank.getTopSTI(3);
    std::cout << "After stimulation and decay, top 3 by STI:" << std::endl;
    for (const auto& [atom, sti] : topSTI) {
        auto nodePtr = std::dynamic_pointer_cast<Node>(atom);
        if (nodePtr) {
            std::cout << "  - " << nodePtr->getName() << " (STI: " << sti << ")" << std::endl;
        }
    }
}

void example_logical_links() {
    std::cout << "\n=== Logical Links Example ===" << std::endl;
    
    AtomSpace space;
    
    // Create logical propositions
    auto isAnimal = createConceptNode(space, "is_animal");
    auto hasFur = createConceptNode(space, "has_fur");
    auto isMammal = createConceptNode(space, "is_mammal");
    
    // Build logical expressions
    // (is_animal AND has_fur) -> is_mammal
    auto conjunction = createAndLink(space, {isAnimal, hasFur});
    std::cout << "Created AND link: " << conjunction->toString() << std::endl;
    
    auto implication = createInheritanceLink(space, conjunction, isMammal);
    
    // Negation
    auto notAnimal = createNotLink(space, isAnimal);
    std::cout << "Created NOT link: " << notAnimal->toString() << std::endl;
    
    // Disjunction
    auto disjunction = createOrLink(space, {isAnimal, isMammal});
    std::cout << "Created OR link: " << disjunction->toString() << std::endl;
}

void example_set_operations() {
    std::cout << "\n=== Set Operations Example ===" << std::endl;
    
    AtomSpace space;
    
    // Create sets
    auto mammals = createConceptNode(space, "mammals");
    auto animals = createConceptNode(space, "animals");
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    
    // Member relationships
    auto memberLink1 = createMemberLink(space, cat, mammals);
    auto memberLink2 = createMemberLink(space, dog, mammals);
    std::cout << "Created member links: cat ∈ mammals, dog ∈ mammals" << std::endl;
    
    // Subset relationship
    auto subsetLink = createSubsetLink(space, mammals, animals);
    std::cout << "Created subset link: mammals ⊆ animals" << std::endl;
    
    // Query incoming sets to find all members
    auto incoming = mammals->getIncomingSet();
    std::cout << "Members of 'mammals': " << incoming.size() << " links reference it" << std::endl;
}

void example_contextual_reasoning() {
    std::cout << "\n=== Contextual Reasoning Example ===" << std::endl;
    
    AtomSpace space;
    
    // Create contexts
    auto winterContext = createConceptNode(space, "winter_season");
    auto summerContext = createConceptNode(space, "summer_season");
    
    // Create temperature facts
    auto coldTemp = createConceptNode(space, "temperature_cold");
    auto warmTemp = createConceptNode(space, "temperature_warm");
    
    // Context-dependent facts
    auto winterFact = createContextLink(space, winterContext, coldTemp);
    auto summerFact = createContextLink(space, summerContext, warmTemp);
    
    std::cout << "Created contextual facts:" << std::endl;
    std::cout << "  - In winter: " << winterFact->toString() << std::endl;
    std::cout << "  - In summer: " << summerFact->toString() << std::endl;
}

void example_serialization() {
    std::cout << "\n=== Serialization Example ===" << std::endl;
    
    // Build a knowledge graph
    AtomSpace space;
    
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    // Set truth values
    cat->setTruthValue(torch::tensor({0.95f, 0.9f}));
    dog->setTruthValue(torch::tensor({0.98f, 0.95f}));
    
    // Create relationships
    createInheritanceLink(space, cat, mammal);
    createInheritanceLink(space, dog, mammal);
    createInheritanceLink(space, mammal, animal);
    
    auto simLink = createSimilarityLink(space, cat, dog);
    simLink->setTruthValue(torch::tensor({0.75f, 0.8f}));
    
    std::cout << "Created knowledge graph with " << space.size() << " atoms" << std::endl;
    
    // Save to file
    std::string filename = "/tmp/animal_knowledge.txt";
    bool saved = Serializer::save(space, filename);
    if (saved) {
        std::cout << "Saved to: " << filename << std::endl;
    }
    
    // Load into new space
    AtomSpace space2;
    bool loaded = Serializer::load(space2, filename);
    if (loaded) {
        std::cout << "Loaded " << space2.size() << " atoms from file" << std::endl;
        
        // Verify loaded data
        auto catLoaded = space2.getNode(Atom::Type::CONCEPT_NODE, "cat");
        if (catLoaded) {
            auto tv = catLoaded->getTruthValue();
            std::cout << "Loaded 'cat' with truth value: [" 
                      << tv[0].item<float>() << ", " << tv[1].item<float>() << "]" << std::endl;
        }
    }
}

void example_integrated_system() {
    std::cout << "\n=== Integrated System Example ===" << std::endl;
    std::cout << "Combining TimeServer, AttentionBank, and Knowledge Graph" << std::endl;
    
    AtomSpace space;
    TimeServer timeServer;
    AttentionBank attentionBank;
    
    // Create a dynamic knowledge base with temporal and attention tracking
    auto task1 = createConceptNode(space, "analyze_data");
    timeServer.recordCreation(task1);
    attentionBank.setAttentionValue(task1, AttentionBank::AttentionValue(100.0f, 50.0f, 10.0f));
    
    auto task2 = createConceptNode(space, "write_report");
    timeServer.recordCreation(task2);
    attentionBank.setAttentionValue(task2, AttentionBank::AttentionValue(80.0f, 60.0f, 20.0f));
    
    auto task3 = createConceptNode(space, "review_code");
    timeServer.recordCreation(task3);
    attentionBank.setAttentionValue(task3, AttentionBank::AttentionValue(60.0f, 40.0f, 15.0f));
    
    // Create temporal sequence
    auto workflow = createSequentialLink(space, {task1, task2});
    timeServer.recordCreation(workflow);
    
    // Simulate task completion
    timeServer.recordEvent(task1, "started");
    attentionBank.stimulate(task1, 50.0f);  // Focus on current task
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    timeServer.recordEvent(task1, "completed");
    attentionBank.transferSTI(task1, task2, 70.0f);  // Shift attention to next task
    
    // Get current focus
    attentionBank.setMaxAFSize(2);
    auto focus = attentionBank.getAttentionalFocus();
    
    std::cout << "Current attentional focus:" << std::endl;
    for (const auto& atom : focus) {
        auto nodePtr = std::dynamic_pointer_cast<Node>(atom);
        if (nodePtr) {
            auto av = attentionBank.getAttentionValue(atom);
            auto info = timeServer.getTemporalInfo(atom);
            auto events = timeServer.getEventHistory(atom);
            
            std::cout << "  - " << nodePtr->getName() << std::endl;
            std::cout << "    STI: " << av.sti << ", Events: " << events.size() << std::endl;
        }
    }
    
    // Save the complete state
    std::string filename = "/tmp/integrated_system.txt";
    Serializer::save(space, filename);
    std::cout << "\nSaved complete system state to: " << filename << std::endl;
}

int main() {
    std::cout << "=== ATenSpace Advanced Features Examples ===" << std::endl;
    
    try {
        example_timeserver();
        example_attention_bank();
        example_logical_links();
        example_set_operations();
        example_contextual_reasoning();
        example_serialization();
        example_integrated_system();
        
        std::cout << "\n=== All examples completed successfully! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
