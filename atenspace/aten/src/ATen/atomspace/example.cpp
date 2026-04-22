/**
 * ATenSpace Example - Knowledge Graph with Tensor Embeddings
 * 
 * This example demonstrates how to use ATenSpace to build a knowledge graph
 * with tensor embeddings for semantic similarity queries.
 */

#include <ATen/atomspace/ATenSpace.h>
#include <iostream>

using namespace at;
using namespace at::atomspace;

void printAtom(const Atom::Handle& atom) {
    std::cout << atom->toString() << std::endl;
}

int main() {
    std::cout << "=== ATenSpace Example ===" << std::endl << std::endl;
    
    // Create an AtomSpace
    AtomSpace space;
    
    // Example 1: Basic nodes and links
    std::cout << "Example 1: Creating basic knowledge" << std::endl;
    
    auto cat = createConceptNode(space, "cat");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    
    // Create inheritance hierarchy: cat -> mammal -> animal
    auto inh1 = createInheritanceLink(space, cat, mammal);
    auto inh2 = createInheritanceLink(space, mammal, animal);
    
    std::cout << "Knowledge graph:" << std::endl;
    printAtom(inh1);
    printAtom(inh2);
    std::cout << std::endl;
    
    // Example 2: Evaluation links (relations)
    std::cout << "Example 2: Creating relations" << std::endl;
    
    auto hasProperty = createPredicateNode(space, "has-property");
    auto furry = createConceptNode(space, "furry");
    
    // Create: cat has-property furry
    auto eval = createEvaluationLink(space, hasProperty, {cat, furry});
    std::cout << "Relation:" << std::endl;
    printAtom(eval);
    std::cout << std::endl;
    
    // Example 3: Tensor embeddings and similarity queries
    std::cout << "Example 3: Tensor embeddings and similarity" << std::endl;
    
    // Create concept nodes with random embeddings (in practice, use learned embeddings)
    auto dog = createConceptNode(space, "dog", torch::randn({128}));
    auto catWithEmb = space.addNode(Atom::Type::CONCEPT_NODE, "cat", torch::randn({128}));
    auto fish = createConceptNode(space, "fish", torch::randn({128}));
    auto tree = createConceptNode(space, "tree", torch::randn({128}));
    
    // Query for concepts similar to a given embedding
    Tensor query = torch::randn({128});
    auto results = space.querySimilar(query, /*k=*/2, /*threshold=*/0.0);
    
    std::cout << "Top 2 similar concepts to query:" << std::endl;
    for (const auto& [atom, similarity] : results) {
        std::cout << "  " << atom->toString() 
                  << " (similarity: " << similarity << ")" << std::endl;
    }
    std::cout << std::endl;
    
    // Example 4: Truth values
    std::cout << "Example 4: Truth values" << std::endl;
    
    auto uncertain = createConceptNode(space, "uncertain-fact");
    // Set truth value: [strength=0.7, confidence=0.5]
    uncertain->setTruthValue(torch::tensor({0.7f, 0.5f}));
    
    std::cout << "Truth value of 'uncertain-fact': " 
              << uncertain->getTruthValue() << std::endl;
    std::cout << std::endl;
    
    // Example 5: Querying atoms
    std::cout << "Example 5: Querying atoms" << std::endl;
    
    std::cout << "Total atoms in space: " << space.size() << std::endl;
    
    auto conceptNodes = space.getAtomsByType(Atom::Type::CONCEPT_NODE);
    std::cout << "Concept nodes: " << conceptNodes.size() << std::endl;
    for (const auto& node : conceptNodes) {
        std::cout << "  " << node->toString() << std::endl;
    }
    std::cout << std::endl;
    
    auto inheritanceLinks = space.getAtomsByType(Atom::Type::INHERITANCE_LINK);
    std::cout << "Inheritance links: " << inheritanceLinks.size() << std::endl;
    for (const auto& link : inheritanceLinks) {
        std::cout << "  " << link->toString() << std::endl;
    }
    std::cout << std::endl;
    
    // Example 6: Incoming sets (what references this atom)
    std::cout << "Example 6: Incoming sets" << std::endl;
    
    std::cout << "Atoms referencing 'cat':" << std::endl;
    const auto& incoming = cat->getIncomingSet();
    for (const auto& weakAtom : incoming) {
        if (auto atom = weakAtom.lock()) {
            std::cout << "  " << atom->toString() << std::endl;
        }
    }
    
    std::cout << std::endl << "=== Example Complete ===" << std::endl;
    
    return 0;
}
