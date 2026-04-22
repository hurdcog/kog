#include <ATen/atomspace/ATenSpace.h>
#include <iostream>
#include <iomanip>

using namespace at::atomspace;

void printTV(const std::string& label, const Tensor& tv) {
    std::cout << label << ": [strength=" << std::fixed << std::setprecision(3)
              << TruthValue::getStrength(tv) << ", confidence=" 
              << TruthValue::getConfidence(tv) << "]" << std::endl;
}

void example1_PatternMatching() {
    std::cout << "\n=== Example 1: Pattern Matching ===" << std::endl;
    
    AtomSpace space;
    
    // Create a knowledge base
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    
    createInheritanceLink(space, cat, mammal);
    createInheritanceLink(space, dog, mammal);
    
    // Create a pattern with a variable
    auto varX = createVariableNode(space, "$X");
    auto pattern = createInheritanceLink(space, varX, mammal);
    
    std::cout << "Pattern: " << pattern->toString() << std::endl;
    std::cout << "\nMatching results:" << std::endl;
    
    // Find all matches
    PatternMatcher::query(space, pattern, 
        [](const Atom::Handle& match, const VariableBinding& bindings) {
            std::cout << "  Match: " << match->toString() << std::endl;
            for (const auto& [var, value] : bindings) {
                std::cout << "    " << var->toString() << " -> " << value->toString() << std::endl;
            }
        });
}

void example2_TruthValueFormulas() {
    std::cout << "\n=== Example 2: Truth Value Formulas ===" << std::endl;
    
    // Deduction: If cat→mammal [0.9, 0.8] and mammal→animal [0.95, 0.9]
    // Then cat→animal with computed truth value
    auto tvCatMammal = TruthValue::create(0.9f, 0.8f);
    auto tvMammalAnimal = TruthValue::create(0.95f, 0.9f);
    
    auto tvCatAnimal = TruthValue::deduction(tvCatMammal, tvMammalAnimal);
    
    printTV("cat→mammal", tvCatMammal);
    printTV("mammal→animal", tvMammalAnimal);
    printTV("cat→animal (deduced)", tvCatAnimal);
    
    // Induction: From 8 positive instances out of 10 observations
    std::cout << "\nInduction from observations:" << std::endl;
    auto tvInduced = TruthValue::induction(8, 10);
    printTV("Induced rule", tvInduced);
    
    // Conjunction: A AND B
    std::cout << "\nLogical operations:" << std::endl;
    auto tvA = TruthValue::create(0.8f, 0.9f);
    auto tvB = TruthValue::create(0.7f, 0.85f);
    auto tvAnd = TruthValue::conjunction(tvA, tvB);
    auto tvOr = TruthValue::disjunction(tvA, tvB);
    auto tvNotA = TruthValue::negation(tvA);
    
    printTV("A", tvA);
    printTV("B", tvB);
    printTV("A ∧ B", tvAnd);
    printTV("A ∨ B", tvOr);
    printTV("¬A", tvNotA);
}

void example3_ForwardChaining() {
    std::cout << "\n=== Example 3: Forward Chaining Inference ===" << std::endl;
    
    AtomSpace space;
    
    // Build a knowledge base with inheritance hierarchy
    auto cat = createConceptNode(space, "cat");
    auto mammal = createConceptNode(space, "mammal");
    auto animal = createConceptNode(space, "animal");
    auto livingThing = createConceptNode(space, "living-thing");
    
    // Create inheritance links with truth values
    auto catMammal = createInheritanceLink(space, cat, mammal);
    catMammal->setTruthValue(TruthValue::create(0.95f, 0.9f));
    
    auto mammalAnimal = createInheritanceLink(space, mammal, animal);
    mammalAnimal->setTruthValue(TruthValue::create(0.98f, 0.95f));
    
    auto animalLiving = createInheritanceLink(space, animal, livingThing);
    animalLiving->setTruthValue(TruthValue::create(0.99f, 0.98f));
    
    std::cout << "Initial knowledge base:" << std::endl;
    std::cout << "  " << catMammal->toString() << std::endl;
    printTV("  ", catMammal->getTruthValue());
    std::cout << "  " << mammalAnimal->toString() << std::endl;
    printTV("  ", mammalAnimal->getTruthValue());
    std::cout << "  " << animalLiving->toString() << std::endl;
    printTV("  ", animalLiving->getTruthValue());
    
    // Create forward chainer
    ForwardChainer chainer(space);
    chainer.setMaxIterations(10);
    chainer.setConfidenceThreshold(0.1f);
    
    std::cout << "\nRunning forward chaining..." << std::endl;
    size_t initialSize = space.getSize();
    int newAtoms = chainer.run();
    
    std::cout << "Created " << newAtoms << " new inferences" << std::endl;
    std::cout << "AtomSpace size: " << initialSize << " → " << space.getSize() << std::endl;
    
    // Check for inferred links
    std::cout << "\nInferred knowledge:" << std::endl;
    auto catAnimal = space.getLink(Atom::Type::INHERITANCE_LINK, {cat, animal});
    if (catAnimal) {
        std::cout << "  " << catAnimal->toString() << std::endl;
        printTV("  ", catAnimal->getTruthValue());
    }
    
    auto catLiving = space.getLink(Atom::Type::INHERITANCE_LINK, {cat, livingThing});
    if (catLiving) {
        std::cout << "  " << catLiving->toString() << std::endl;
        printTV("  ", catLiving->getTruthValue());
    }
}

void example4_BackwardChaining() {
    std::cout << "\n=== Example 4: Backward Chaining (Goal-Directed) ===" << std::endl;
    
    AtomSpace space;
    
    // Build knowledge base
    auto socrates = createConceptNode(space, "Socrates");
    auto human = createConceptNode(space, "human");
    auto mortal = createConceptNode(space, "mortal");
    
    // Facts
    auto socratesHuman = createInheritanceLink(space, socrates, human);
    socratesHuman->setTruthValue(TruthValue::create(1.0f, 0.95f));
    
    auto humanMortal = createInheritanceLink(space, human, mortal);
    humanMortal->setTruthValue(TruthValue::create(0.99f, 0.99f));
    
    std::cout << "Knowledge base:" << std::endl;
    std::cout << "  " << socratesHuman->toString() << std::endl;
    std::cout << "  " << humanMortal->toString() << std::endl;
    
    // Goal: Prove that Socrates is mortal
    auto goal = createInheritanceLink(space, socrates, mortal);
    
    std::cout << "\nGoal: " << goal->toString() << std::endl;
    
    // Create backward chainer
    BackwardChainer chainer(space);
    chainer.addRule(std::make_shared<DeductionRule>());
    chainer.setMaxDepth(5);
    
    // Try to prove the goal
    auto proofs = chainer.prove(goal);
    
    std::cout << "\nFound " << proofs.size() << " proof(s)" << std::endl;
    
    for (size_t i = 0; i < proofs.size(); ++i) {
        std::cout << "\nProof " << (i + 1) << ":" << std::endl;
        std::cout << proofs[i]->toString();
    }
    
    // Query the goal's truth value
    auto tv = chainer.query(goal);
    printTV("Proven truth value", tv);
}

void example5_ComplexReasoning() {
    std::cout << "\n=== Example 5: Complex Reasoning with Attention ===" << std::endl;
    
    AtomSpace space;
    AttentionBank attentionBank;
    
    // Build a richer knowledge base
    auto bird = createConceptNode(space, "bird");
    auto canFly = createConceptNode(space, "can-fly");
    auto penguin = createConceptNode(space, "penguin");
    auto animal = createConceptNode(space, "animal");
    
    // General rule: birds can fly (with exceptions)
    auto birdFly = createInheritanceLink(space, bird, canFly);
    birdFly->setTruthValue(TruthValue::create(0.9f, 0.8f));
    attentionBank.setSTI(birdFly, 100.0f);
    
    // Penguin is a bird
    auto penguinBird = createInheritanceLink(space, penguin, bird);
    penguinBird->setTruthValue(TruthValue::create(0.99f, 0.95f));
    attentionBank.setSTI(penguinBird, 80.0f);
    
    // Bird is an animal
    auto birdAnimal = createInheritanceLink(space, bird, animal);
    birdAnimal->setTruthValue(TruthValue::create(0.98f, 0.95f));
    attentionBank.setSTI(birdAnimal, 60.0f);
    
    std::cout << "Knowledge base with attention values:" << std::endl;
    auto focus = attentionBank.getAttentionalFocus();
    for (const auto& atom : focus) {
        std::cout << "  " << atom->toString();
        auto av = attentionBank.getAttentionValue(atom);
        std::cout << " [STI=" << av.sti << "]" << std::endl;
    }
    
    // Forward chain with attention guidance
    ForwardChainer chainer(space);
    chainer.setConfidenceThreshold(0.1f);
    
    std::cout << "\nRunning attention-guided inference..." << std::endl;
    int newAtoms = chainer.run(&attentionBank);
    std::cout << "Created " << newAtoms << " new inferences" << std::endl;
    
    // Check what we can infer about penguin
    auto penguinFly = space.getLink(Atom::Type::INHERITANCE_LINK, {penguin, canFly});
    if (penguinFly) {
        std::cout << "\nInferred: " << penguinFly->toString() << std::endl;
        printTV("Truth value", penguinFly->getTruthValue());
        std::cout << "(Note: High strength but this contradicts reality - "
                  << "demonstrating need for non-monotonic reasoning)" << std::endl;
    }
}

void example6_PatternSubstitution() {
    std::cout << "\n=== Example 6: Pattern Substitution ===" << std::endl;
    
    AtomSpace space;
    
    // Create a pattern: $X → animal
    auto varX = createVariableNode(space, "$X");
    auto animal = createConceptNode(space, "animal");
    auto pattern = createInheritanceLink(space, varX, animal);
    
    // Create bindings
    VariableBinding bindings;
    auto dog = createConceptNode(space, "dog");
    bindings[varX] = dog;
    
    std::cout << "Pattern: " << pattern->toString() << std::endl;
    std::cout << "Bindings: $X -> " << dog->toString() << std::endl;
    
    // Substitute
    auto result = PatternMatcher::substitute(pattern, bindings, space);
    std::cout << "Result: " << result->toString() << std::endl;
}

int main() {
    std::cout << "ATenSpace PLN (Probabilistic Logic Networks) Examples" << std::endl;
    std::cout << "======================================================" << std::endl;
    
    try {
        example1_PatternMatching();
        example2_TruthValueFormulas();
        example3_ForwardChaining();
        example4_BackwardChaining();
        example5_ComplexReasoning();
        example6_PatternSubstitution();
        
        std::cout << "\n=== All examples completed successfully ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
