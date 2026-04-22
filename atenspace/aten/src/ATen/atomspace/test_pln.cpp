#include <ATen/atomspace/ATenSpace.h>
#include <cassert>
#include <iostream>
#include <cmath>

using namespace at::atomspace;

// Helper function for approximate equality
bool approxEqual(float a, float b, float epsilon = 0.01f) {
    return std::abs(a - b) < epsilon;
}

void testPatternMatching() {
    std::cout << "Testing Pattern Matching... ";
    
    AtomSpace space;
    
    // Create knowledge base
    auto cat = createConceptNode(space, "cat");
    auto dog = createConceptNode(space, "dog");
    auto mammal = createConceptNode(space, "mammal");
    
    auto catMammal = createInheritanceLink(space, cat, mammal);
    auto dogMammal = createInheritanceLink(space, dog, mammal);
    
    // Test 1: Pattern with variable
    auto varX = createVariableNode(space, "$X");
    auto pattern = createInheritanceLink(space, varX, mammal);
    
    auto matches = PatternMatcher::findMatches(space, pattern);
    assert(matches.size() == 2);
    
    // Test 2: Variable binding
    VariableBinding bindings;
    bool matched = PatternMatcher::match(pattern, catMammal, bindings);
    assert(matched);
    assert(bindings.size() == 1);
    assert(bindings[varX]->equals(*cat));
    
    // Test 3: Pattern substitution
    auto result = PatternMatcher::substitute(pattern, bindings, space);
    assert(result->equals(*catMammal));
    
    // Test 4: Unification
    auto varY = createVariableNode(space, "$Y");
    auto pattern2 = createInheritanceLink(space, varY, mammal);
    VariableBinding unifyBindings;
    bool unified = PatternMatcher::unify(pattern, pattern2, unifyBindings);
    assert(unified);
    
    std::cout << "PASSED" << std::endl;
}

void testTruthValueFormulas() {
    std::cout << "Testing Truth Value Formulas... ";
    
    // Test 1: Deduction
    auto tv1 = TruthValue::create(0.9f, 0.8f);
    auto tv2 = TruthValue::create(0.8f, 0.9f);
    auto tvDeduced = TruthValue::deduction(tv1, tv2);
    
    float strength = TruthValue::getStrength(tvDeduced);
    float confidence = TruthValue::getConfidence(tvDeduced);
    
    assert(strength > 0.7f && strength < 0.75f); // 0.9 * 0.8 = 0.72
    assert(confidence > 0.5f && confidence < 0.8f);
    
    // Test 2: Induction
    auto tvInduced = TruthValue::induction(8, 10);
    assert(approxEqual(TruthValue::getStrength(tvInduced), 0.8f));
    assert(TruthValue::getConfidence(tvInduced) > 0.8f);
    
    // Test 3: Conjunction
    auto tvA = TruthValue::create(0.8f, 0.9f);
    auto tvB = TruthValue::create(0.7f, 0.85f);
    auto tvAnd = TruthValue::conjunction(tvA, tvB);
    
    assert(approxEqual(TruthValue::getStrength(tvAnd), 0.56f)); // 0.8 * 0.7
    
    // Test 4: Disjunction
    auto tvOr = TruthValue::disjunction(tvA, tvB);
    float orStrength = TruthValue::getStrength(tvOr);
    assert(orStrength > 0.9f && orStrength < 1.0f); // 0.8 + 0.7 - 0.56 = 0.94
    
    // Test 5: Negation
    auto tvNot = TruthValue::negation(tvA);
    assert(approxEqual(TruthValue::getStrength(tvNot), 0.2f)); // 1 - 0.8
    assert(approxEqual(TruthValue::getConfidence(tvNot), 0.9f)); // Same confidence
    
    // Test 6: Revision
    auto tv1a = TruthValue::create(0.7f, 0.8f);
    auto tv1b = TruthValue::create(0.8f, 0.7f);
    auto tvRevised = TruthValue::revision(tv1a, tv1b);
    float revisedStrength = TruthValue::getStrength(tvRevised);
    assert(revisedStrength > 0.7f && revisedStrength < 0.8f); // Weighted average
    
    // Test 7: Abduction
    auto tvObserved = TruthValue::create(0.9f, 0.8f);
    auto tvAB = TruthValue::create(0.85f, 0.9f);
    auto tvAbduced = TruthValue::abduction(tvObserved, tvAB);
    assert(TruthValue::getStrength(tvAbduced) > 0.5f);
    assert(TruthValue::getConfidence(tvAbduced) < TruthValue::getConfidence(tvAB));
    
    std::cout << "PASSED" << std::endl;
}

void testDeductionRule() {
    std::cout << "Testing Deduction Rule... ";
    
    AtomSpace space;
    
    // Create A→B and B→C
    auto A = createConceptNode(space, "A");
    auto B = createConceptNode(space, "B");
    auto C = createConceptNode(space, "C");
    
    auto AB = createInheritanceLink(space, A, B);
    AB->setTruthValue(TruthValue::create(0.9f, 0.8f));
    
    auto BC = createInheritanceLink(space, B, C);
    BC->setTruthValue(TruthValue::create(0.85f, 0.9f));
    
    // Apply deduction rule
    DeductionRule rule;
    std::vector<Atom::Handle> premises = {AB, BC};
    
    assert(rule.canApply(premises));
    
    auto conclusions = rule.apply(premises, space);
    assert(conclusions.size() == 1);
    
    // Check that A→C was created
    auto AC = conclusions[0];
    const Link* acLink = static_cast<const Link*>(AC.get());
    assert(acLink->getOutgoingAtom(0)->equals(*A));
    assert(acLink->getOutgoingAtom(1)->equals(*C));
    
    // Check truth value
    float strength = TruthValue::getStrength(AC->getTruthValue());
    assert(strength > 0.7f && strength < 0.8f); // 0.9 * 0.85 ≈ 0.765
    
    std::cout << "PASSED" << std::endl;
}

void testForwardChaining() {
    std::cout << "Testing Forward Chaining... ";
    
    AtomSpace space;
    
    // Create a chain: A→B→C→D
    auto A = createConceptNode(space, "A");
    auto B = createConceptNode(space, "B");
    auto C = createConceptNode(space, "C");
    auto D = createConceptNode(space, "D");
    
    auto AB = createInheritanceLink(space, A, B);
    AB->setTruthValue(TruthValue::create(0.9f, 0.9f));
    
    auto BC = createInheritanceLink(space, B, C);
    BC->setTruthValue(TruthValue::create(0.9f, 0.9f));
    
    auto CD = createInheritanceLink(space, C, D);
    CD->setTruthValue(TruthValue::create(0.9f, 0.9f));
    
    size_t initialSize = space.getSize();
    
    // Run forward chaining
    ForwardChainer chainer(space);
    chainer.setMaxIterations(10);
    chainer.setConfidenceThreshold(0.1f);
    
    int newAtoms = chainer.run();
    assert(newAtoms > 0);
    assert(space.getSize() > initialSize);
    
    // Check that A→C was inferred
    auto AC = space.getLink(Atom::Type::INHERITANCE_LINK, {A, C});
    assert(AC != nullptr);
    
    // Check that A→D was inferred
    auto AD = space.getLink(Atom::Type::INHERITANCE_LINK, {A, D});
    assert(AD != nullptr);
    
    std::cout << "PASSED" << std::endl;
}

void testBackwardChaining() {
    std::cout << "Testing Backward Chaining... ";
    
    AtomSpace space;
    
    // Create knowledge base: Socrates→Human→Mortal
    auto socrates = createConceptNode(space, "Socrates");
    auto human = createConceptNode(space, "Human");
    auto mortal = createConceptNode(space, "Mortal");
    
    auto socratesHuman = createInheritanceLink(space, socrates, human);
    socratesHuman->setTruthValue(TruthValue::create(1.0f, 0.95f));
    
    auto humanMortal = createInheritanceLink(space, human, mortal);
    humanMortal->setTruthValue(TruthValue::create(0.99f, 0.99f));
    
    // Goal: Prove Socrates→Mortal
    auto goal = createInheritanceLink(space, socrates, mortal);
    
    BackwardChainer chainer(space);
    chainer.addRule(std::make_shared<DeductionRule>());
    chainer.setMaxDepth(5);
    
    auto proofs = chainer.prove(goal);
    assert(proofs.size() > 0);
    
    // Check proof structure
    auto proof = proofs[0];
    assert(proof->goal->equals(*goal));
    
    // Check that truth value is high
    float confidence = TruthValue::getConfidence(proof->truthValue);
    assert(confidence > 0.8f);
    
    // Test canProve
    assert(chainer.canProve(goal));
    
    // Test query
    auto tv = chainer.query(goal);
    assert(TruthValue::getConfidence(tv) > 0.8f);
    
    std::cout << "PASSED" << std::endl;
}

void testComplexPatterns() {
    std::cout << "Testing Complex Patterns... ";
    
    AtomSpace space;
    
    // Create nested structure
    auto predicate = createPredicateNode(space, "likes");
    auto john = createConceptNode(space, "John");
    auto mary = createConceptNode(space, "Mary");
    
    auto eval = createEvaluationLink(space, predicate, {john, mary});
    
    // Pattern with variable in nested structure
    auto varX = createVariableNode(space, "$X");
    auto patternEval = createEvaluationLink(space, predicate, {john, varX});
    
    VariableBinding bindings;
    bool matched = PatternMatcher::match(patternEval, eval, bindings);
    assert(matched);
    assert(bindings[varX]->equals(*mary));
    
    // Test pattern extraction
    assert(Pattern::hasVariables(patternEval));
    auto vars = Pattern::getVariables(patternEval);
    assert(vars.size() == 1);
    assert(vars[0]->equals(*varX));
    
    std::cout << "PASSED" << std::endl;
}

void testLogicalOperations() {
    std::cout << "Testing Logical Operations... ";
    
    AtomSpace space;
    
    auto A = createConceptNode(space, "A");
    auto B = createConceptNode(space, "B");
    auto C = createConceptNode(space, "C");
    
    // Test AND link
    auto andLink = createAndLink(space, {A, B, C});
    assert(andLink->getType() == Atom::Type::AND_LINK);
    const Link* andLinkPtr = static_cast<const Link*>(andLink.get());
    assert(andLinkPtr->getArity() == 3);
    
    // Test OR link
    auto orLink = createOrLink(space, {A, B});
    assert(orLink->getType() == Atom::Type::OR_LINK);
    
    // Test NOT link
    auto notLink = createNotLink(space, A);
    assert(notLink->getType() == Atom::Type::NOT_LINK);
    
    // Test truth value propagation
    A->setTruthValue(TruthValue::create(0.8f, 0.9f));
    B->setTruthValue(TruthValue::create(0.7f, 0.85f));
    
    auto tvAnd = TruthValue::conjunction(A->getTruthValue(), B->getTruthValue());
    andLink->setTruthValue(tvAnd);
    
    float andStrength = TruthValue::getStrength(andLink->getTruthValue());
    assert(approxEqual(andStrength, 0.56f));
    
    std::cout << "PASSED" << std::endl;
}

void testImplicationLink() {
    std::cout << "Testing Implication Link... ";
    
    AtomSpace space;
    
    auto A = createConceptNode(space, "A");
    auto B = createConceptNode(space, "B");
    
    // Create implication link
    auto impl = createImplicationLink(space, A, B);
    assert(impl->getType() == Atom::Type::IMPLICATION_LINK);
    
    const Link* implLink = static_cast<const Link*>(impl.get());
    assert(implLink->getArity() == 2);
    assert(implLink->getOutgoingAtom(0)->equals(*A));
    assert(implLink->getOutgoingAtom(1)->equals(*B));
    
    // Set truth values and compute implication
    A->setTruthValue(TruthValue::create(0.8f, 0.9f));
    B->setTruthValue(TruthValue::create(0.7f, 0.85f));
    
    auto tvImpl = TruthValue::implication(A->getTruthValue(), B->getTruthValue());
    impl->setTruthValue(tvImpl);
    
    float strength = TruthValue::getStrength(impl->getTruthValue());
    assert(strength > 0.7f); // Should be relatively high
    
    std::cout << "PASSED" << std::endl;
}

void testAttentionGuidedInference() {
    std::cout << "Testing Attention-Guided Inference... ";
    
    AtomSpace space;
    AttentionBank attentionBank;
    
    // Create atoms with different attention values
    auto A = createConceptNode(space, "A");
    auto B = createConceptNode(space, "B");
    auto C = createConceptNode(space, "C");
    
    auto AB = createInheritanceLink(space, A, B);
    AB->setTruthValue(TruthValue::create(0.9f, 0.9f));
    attentionBank.setSTI(AB, 100.0f);
    
    auto BC = createInheritanceLink(space, B, C);
    BC->setTruthValue(TruthValue::create(0.9f, 0.9f));
    attentionBank.setSTI(BC, 50.0f);
    
    // Forward chain with attention guidance
    ForwardChainer chainer(space);
    chainer.setConfidenceThreshold(0.1f);
    
    int newAtoms = chainer.run(&attentionBank);
    assert(newAtoms > 0);
    
    // Check that inference was performed
    auto AC = space.getLink(Atom::Type::INHERITANCE_LINK, {A, C});
    assert(AC != nullptr);
    
    std::cout << "PASSED" << std::endl;
}

void testIndefiniteTruthValues() {
    std::cout << "Testing Indefinite Truth Values... ";
    
    // Test with various counts
    auto tv1 = TruthValue::indefinite(7, 3);  // 7 positive, 3 negative
    assert(approxEqual(TruthValue::getStrength(tv1), 0.7f));
    assert(TruthValue::getConfidence(tv1) > 0.0f);
    
    auto tv2 = TruthValue::indefinite(100, 50);  // More observations
    assert(approxEqual(TruthValue::getStrength(tv2), 0.667f, 0.01f));
    assert(TruthValue::getConfidence(tv2) > TruthValue::getConfidence(tv1));
    
    // Test default cases
    auto tvDefault = TruthValue::defaultTV();
    assert(approxEqual(TruthValue::getStrength(tvDefault), 0.5f));
    assert(approxEqual(TruthValue::getConfidence(tvDefault), 0.0f));
    
    auto tvTrue = TruthValue::trueTV();
    assert(approxEqual(TruthValue::getStrength(tvTrue), 1.0f));
    assert(TruthValue::getConfidence(tvTrue) > 0.8f);
    
    auto tvFalse = TruthValue::falseTV();
    assert(approxEqual(TruthValue::getStrength(tvFalse), 0.0f));
    assert(TruthValue::getConfidence(tvFalse) > 0.8f);
    
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "Running PLN (Probabilistic Logic Networks) Tests" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    try {
        testPatternMatching();
        testTruthValueFormulas();
        testDeductionRule();
        testForwardChaining();
        testBackwardChaining();
        testComplexPatterns();
        testLogicalOperations();
        testImplicationLink();
        testAttentionGuidedInference();
        testIndefiniteTruthValues();
        
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "\nTest failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nTest failed with unknown exception" << std::endl;
        return 1;
    }
}
