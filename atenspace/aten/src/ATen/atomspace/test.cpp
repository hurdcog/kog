/**
 * ATenSpace Tests
 * 
 * Basic tests for ATenSpace functionality
 */

#include <ATen/atomspace/ATenSpace.h>
#include <cassert>
#include <iostream>

using namespace at;
using namespace at::atomspace;

void test_basic_nodes() {
    std::cout << "Testing basic node creation..." << std::endl;
    
    AtomSpace space;
    
    auto node1 = createConceptNode(space, "test");
    auto node2 = createConceptNode(space, "test");
    
    // Should return same node (uniqueness)
    assert(node1 == node2);
    assert(space.size() == 1);
    
    auto node3 = createConceptNode(space, "different");
    assert(node1 != node3);
    assert(space.size() == 2);
    
    std::cout << "✓ Basic node tests passed" << std::endl;
}

void test_links() {
    std::cout << "Testing link creation..." << std::endl;
    
    AtomSpace space;
    
    auto cat = createConceptNode(space, "cat");
    auto mammal = createConceptNode(space, "mammal");
    
    auto link1 = createInheritanceLink(space, cat, mammal);
    auto link2 = createInheritanceLink(space, cat, mammal);
    
    // Should return same link (uniqueness)
    assert(link1 == link2);
    
    // Link should have 2 outgoing atoms
    auto linkPtr = std::dynamic_pointer_cast<Link>(link1);
    assert(linkPtr != nullptr);
    assert(linkPtr->getArity() == 2);
    assert(linkPtr->getOutgoingAtom(0) == cat);
    assert(linkPtr->getOutgoingAtom(1) == mammal);
    
    std::cout << "✓ Link tests passed" << std::endl;
}

void test_incoming_sets() {
    std::cout << "Testing incoming sets..." << std::endl;
    
    AtomSpace space;
    
    auto cat = createConceptNode(space, "cat");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    auto link1 = createInheritanceLink(space, cat, mammal);
    auto link2 = createInheritanceLink(space, cat, animal);
    
    // Cat should have 2 incoming links
    assert(cat->getIncomingSet().size() == 2);
    
    // Mammal should have 1 incoming link
    assert(mammal->getIncomingSet().size() == 1);
    
    std::cout << "✓ Incoming set tests passed" << std::endl;
}

void test_truth_values() {
    std::cout << "Testing truth values..." << std::endl;
    
    AtomSpace space;
    
    auto node = createConceptNode(space, "test");
    
    // Default truth value should be [1.0, 1.0]
    auto tv = node->getTruthValue();
    assert(tv[0].item<float>() == 1.0f);
    assert(tv[1].item<float>() == 1.0f);
    
    // Set custom truth value
    node->setTruthValue(torch::tensor({0.7f, 0.5f}));
    tv = node->getTruthValue();
    assert(tv[0].item<float>() == 0.7f);
    assert(tv[1].item<float>() == 0.5f);
    
    std::cout << "✓ Truth value tests passed" << std::endl;
}

void test_embeddings() {
    std::cout << "Testing embeddings..." << std::endl;
    
    AtomSpace space;
    
    Tensor emb1 = torch::randn({128});
    auto node1 = createConceptNode(space, "test", emb1);
    
    auto nodePtr = std::dynamic_pointer_cast<Node>(node1);
    assert(nodePtr != nullptr);
    assert(nodePtr->hasEmbedding());
    
    auto retrieved = nodePtr->getEmbedding();
    assert(torch::allclose(retrieved, emb1));
    
    std::cout << "✓ Embedding tests passed" << std::endl;
}

void test_query_by_type() {
    std::cout << "Testing query by type..." << std::endl;
    
    AtomSpace space;
    
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto hasProperty = createPredicateNode(space, "has-property");
    
    auto link = createInheritanceLink(space, cat, dog);
    
    auto concepts = space.getAtomsByType(Atom::Type::CONCEPT_NODE);
    assert(concepts.size() == 2);
    
    auto predicates = space.getAtomsByType(Atom::Type::PREDICATE_NODE);
    assert(predicates.size() == 1);
    
    auto inheritanceLinks = space.getAtomsByType(Atom::Type::INHERITANCE_LINK);
    assert(inheritanceLinks.size() == 1);
    
    std::cout << "✓ Query by type tests passed" << std::endl;
}

void test_similarity_query() {
    std::cout << "Testing similarity query..." << std::endl;
    
    AtomSpace space;
    
    // Create nodes with embeddings
    Tensor emb1 = torch::ones({128});
    Tensor emb2 = torch::ones({128}) * 0.9f;  // Similar to emb1
    Tensor emb3 = torch::ones({128}) * -1.0f; // Opposite to emb1
    
    auto node1 = createConceptNode(space, "similar1", emb1);
    auto node2 = createConceptNode(space, "similar2", emb2);
    auto node3 = createConceptNode(space, "different", emb3);
    
    // Query with embedding similar to emb1
    auto results = space.querySimilar(emb1, /*k=*/2);
    
    assert(results.size() == 2);
    // First result should be most similar
    assert(results[0].second > results[1].second);
    
    std::cout << "✓ Similarity query tests passed" << std::endl;
}

void test_evaluation_links() {
    std::cout << "Testing evaluation links..." << std::endl;
    
    AtomSpace space;
    
    auto cat = createConceptNode(space, "cat");
    auto furry = createConceptNode(space, "furry");
    auto hasProperty = createPredicateNode(space, "has-property");
    
    auto eval = createEvaluationLink(space, hasProperty, {cat, furry});
    
    auto evalPtr = std::dynamic_pointer_cast<Link>(eval);
    assert(evalPtr != nullptr);
    assert(evalPtr->getArity() == 2);
    
    // First atom should be the predicate
    assert(evalPtr->getOutgoingAtom(0) == hasProperty);
    
    // Second atom should be a list link
    auto listLink = evalPtr->getOutgoingAtom(1);
    auto listPtr = std::dynamic_pointer_cast<Link>(listLink);
    assert(listPtr != nullptr);
    assert(listPtr->getType() == Atom::Type::LIST_LINK);
    
    std::cout << "✓ Evaluation link tests passed" << std::endl;
}

int main() {
    std::cout << "=== ATenSpace Tests ===" << std::endl << std::endl;
    
    try {
        test_basic_nodes();
        test_links();
        test_incoming_sets();
        test_truth_values();
        test_embeddings();
        test_query_by_type();
        test_similarity_query();
        test_evaluation_links();
        
        std::cout << std::endl << "=== All Tests Passed ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
