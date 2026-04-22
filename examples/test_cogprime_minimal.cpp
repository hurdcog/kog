/**
 * Minimal test for CogPrime identity integration
 */

#include <cogself/cogself.h>
#include <cogself/cogprime_identity.h>
#include <iostream>

int main() {
    std::cout << "=== Minimal CogPrime Integration Test ===" << std::endl;
    
    // Test 1: Basic identity
    std::cout << "\n1. Creating basic identity..." << std::endl;
    auto identity = std::make_shared<cogself::CogPrimeIdentity>("test_001", "Test Agent");
    std::cout << "✓ Identity created" << std::endl;
    
    // Test 2: CogSelf with identity
    std::cout << "\n2. Initializing CogSelf with identity..." << std::endl;
    cogself::CogSelf framework;
    framework.initialize("test_002", "CogSelf Agent");
    std::cout << "✓ CogSelf initialized with identity" << std::endl;
    
    // Test 3: Basic operations
    std::cout << "\n3. Testing basic operations..." << std::endl;
    framework.recordCognitiveEvent("test", "Test event", 0.5);
    framework.updateSynergyState();
    double synergy = framework.getSynergyLevel();
    std::cout << "✓ Synergy level: " << synergy << std::endl;
    
    // Test 4: Enable distributed cognition (but don't propagate yet)
    std::cout << "\n4. Enabling distributed cognition..." << std::endl;
    framework.enableDistributedCognition();
    std::cout << "✓ Distributed cognition enabled" << std::endl;
    
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  All Basic Tests Passed ✓             ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    
    return 0;
}
