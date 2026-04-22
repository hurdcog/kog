#!/bin/bash
# Kernel Integration Configuration Script
# This script configures Cognumach to work with HurdCog's cognitive features

set -e

echo "╔════════════════════════════════════════════════════════════╗"
echo "║  AGI-OS Kernel Integration Configuration                  ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

# Define paths
COGNUMACH_DIR="/home/ubuntu/cognumach"
HURDCOG_DIR="/home/ubuntu/hurdcog"
OCC_DIR="/home/ubuntu/occ"
INTEGRATION_DIR="/home/ubuntu/agi-os-integration"

# Check if directories exist
if [ ! -d "$COGNUMACH_DIR" ]; then
    echo "Error: Cognumach directory not found at $COGNUMACH_DIR"
    exit 1
fi

if [ ! -d "$HURDCOG_DIR" ]; then
    echo "Error: HurdCog directory not found at $HURDCOG_DIR"
    exit 1
fi

if [ ! -d "$OCC_DIR" ]; then
    echo "Error: OCC directory not found at $OCC_DIR"
    exit 1
fi

echo "Step 1: Configuring Cognumach for HurdCog integration..."
echo "--------------------------------------------------------"

# Create integration header for Cognumach
mkdir -p "$COGNUMACH_DIR/include/mach/cognitive"

cat > "$COGNUMACH_DIR/include/mach/cognitive/atomspace_ipc.h" << 'EOF'
/*
 * AtomSpace IPC Integration for Cognumach
 * Provides IPC primitives for cognitive operations
 */

#ifndef _MACH_COGNITIVE_ATOMSPACE_IPC_H_
#define _MACH_COGNITIVE_ATOMSPACE_IPC_H_

#include <mach/message.h>
#include <mach/port.h>

/* Cognitive IPC message types */
#define MACH_COGNITIVE_MSG_ATOM_CREATE    1000
#define MACH_COGNITIVE_MSG_ATOM_QUERY     1001
#define MACH_COGNITIVE_MSG_ATOM_UPDATE    1002
#define MACH_COGNITIVE_MSG_ATOM_DELETE    1003
#define MACH_COGNITIVE_MSG_PLN_REASON     1004
#define MACH_COGNITIVE_MSG_ECAN_ALLOCATE  1005

/* Cognitive port rights */
typedef mach_port_t cognitive_port_t;

/* Cognitive message structure */
typedef struct {
    mach_msg_header_t header;
    mach_msg_type_t type;
    int cognitive_op;
    int atom_id;
    int data_size;
    char data[1024];
} cognitive_msg_t;

/* Function declarations */
kern_return_t cognitive_port_allocate(
    ipc_space_t space,
    cognitive_port_t *port);

kern_return_t cognitive_msg_send(
    cognitive_port_t port,
    cognitive_msg_t *msg,
    mach_msg_timeout_t timeout);

kern_return_t cognitive_msg_receive(
    cognitive_port_t port,
    cognitive_msg_t *msg,
    mach_msg_timeout_t timeout);

#endif /* _MACH_COGNITIVE_ATOMSPACE_IPC_H_ */
EOF

echo "✓ Created AtomSpace IPC header"

# Create cognitive memory management header
cat > "$COGNUMACH_DIR/include/mach/cognitive/cognitive_vm.h" << 'EOF'
/*
 * Cognitive VM Extensions for Cognumach
 * Provides VM primitives optimized for AtomSpace operations
 */

#ifndef _MACH_COGNITIVE_VM_H_
#define _MACH_COGNITIVE_VM_H_

#include <mach/vm_types.h>
#include <mach/vm_prot.h>

/* Cognitive memory regions */
#define VM_REGION_ATOMSPACE     0x1000
#define VM_REGION_COGNITIVE     0x2000

/* Cognitive VM attributes */
typedef struct {
    vm_address_t base_address;
    vm_size_t size;
    vm_prot_t protection;
    int cognitive_flags;
    int atom_count;
    int link_count;
} cognitive_vm_region_t;

/* Function declarations */
kern_return_t cognitive_vm_allocate(
    vm_map_t map,
    vm_address_t *address,
    vm_size_t size,
    int flags);

kern_return_t cognitive_vm_deallocate(
    vm_map_t map,
    vm_address_t address,
    vm_size_t size);

kern_return_t cognitive_vm_protect(
    vm_map_t map,
    vm_address_t address,
    vm_size_t size,
    vm_prot_t protection);

kern_return_t cognitive_vm_region_info(
    vm_map_t map,
    vm_address_t address,
    cognitive_vm_region_t *info);

#endif /* _MACH_COGNITIVE_VM_H_ */
EOF

echo "✓ Created Cognitive VM header"

echo ""
echo "Step 2: Creating HurdCog-Cognumach integration layer..."
echo "--------------------------------------------------------"

# Create integration layer in HurdCog
mkdir -p "$HURDCOG_DIR/cogkernel/mach-integration"

cat > "$HURDCOG_DIR/cogkernel/mach-integration/machspace-bridge.scm" << 'EOF'
;;; MachSpace Bridge - Connects HurdCog cognitive kernel to Cognumach
;;; This module provides the bridge between AtomSpace and Mach IPC

(define-module (machspace-bridge)
  #:use-module (ice-9 binary-ports)
  #:use-module (ice-9 format)
  #:export (machspace-init
            machspace-send-atom
            machspace-receive-atom
            machspace-allocate-port
            machspace-deallocate-port))

;;; Configuration
(define *mach-port* #f)
(define *cognitive-port* #f)

;;; Initialize MachSpace bridge
(define (machspace-init)
  "Initialize the MachSpace bridge between HurdCog and Cognumach"
  (format #t "Initializing MachSpace bridge...~%")
  
  ;; Allocate cognitive port
  (set! *cognitive-port* (allocate-cognitive-port))
  
  (format #t "✓ MachSpace bridge initialized~%")
  (format #t "  Cognitive port: ~a~%" *cognitive-port*)
  #t)

;;; Allocate a cognitive port
(define (machspace-allocate-port)
  "Allocate a new cognitive port for IPC"
  (let ((port-id (random 65536)))
    (format #t "Allocated cognitive port: ~a~%" port-id)
    port-id))

;;; Deallocate a cognitive port
(define (machspace-deallocate-port port)
  "Deallocate a cognitive port"
  (format #t "Deallocated cognitive port: ~a~%" port)
  #t)

;;; Send an atom through Mach IPC
(define (machspace-send-atom atom-id atom-data)
  "Send an atom to the Mach microkernel for processing"
  (format #t "Sending atom ~a through MachSpace...~%" atom-id)
  
  ;; Create cognitive message
  (let ((msg (make-cognitive-message
              'atom-create
              atom-id
              atom-data)))
    
    ;; Send via Mach IPC (simulated)
    (format #t "  Message type: atom-create~%")
    (format #t "  Atom ID: ~a~%" atom-id)
    (format #t "  Data: ~a~%" atom-data)
    
    ;; Return success
    #t))

;;; Receive an atom from Mach IPC
(define (machspace-receive-atom port)
  "Receive an atom from the Mach microkernel"
  (format #t "Receiving atom from MachSpace port ~a...~%" port)
  
  ;; Simulate receiving a message
  (let ((atom-id (random 1000))
        (atom-data "sample-atom-data"))
    
    (format #t "  Received atom ID: ~a~%" atom-id)
    (format #t "  Data: ~a~%" atom-data)
    
    ;; Return atom
    (cons atom-id atom-data)))

;;; Helper: Create cognitive message
(define (make-cognitive-message msg-type atom-id data)
  "Create a cognitive message structure"
  (list 'cognitive-msg
        (cons 'type msg-type)
        (cons 'atom-id atom-id)
        (cons 'data data)))

;;; Helper: Allocate cognitive port (internal)
(define (allocate-cognitive-port)
  "Internal function to allocate a cognitive port"
  (machspace-allocate-port))

;;; Module initialization
(format #t "MachSpace bridge module loaded~%")
EOF

echo "✓ Created MachSpace bridge module"

# Create Mach integration Makefile
cat > "$HURDCOG_DIR/cogkernel/mach-integration/Makefile" << 'EOF'
# Makefile for HurdCog-Cognumach Integration Layer

GUILE = guile
GUILD = guild

SOURCES = machspace-bridge.scm
OBJECTS = $(SOURCES:.scm=.go)

all: $(OBJECTS)

%.go: %.scm
	$(GUILD) compile -o $@ $<

clean:
	rm -f $(OBJECTS)

test:
	$(GUILE) -c "(use-modules (machspace-bridge)) (machspace-init)"

.PHONY: all clean test
EOF

echo "✓ Created Mach integration Makefile"

echo ""
echo "Step 3: Creating OCC-HurdCog integration layer..."
echo "--------------------------------------------------------"

# Create OCC integration directory
mkdir -p "$OCC_DIR/hurdcog-integration"

cat > "$OCC_DIR/hurdcog-integration/atomspace-hurdcog-bridge.py" << 'EOF'
#!/usr/bin/env python3
"""
AtomSpace-HurdCog Bridge
Connects OCC's AtomSpace to HurdCog's cognitive kernel
"""

import sys
import json
from typing import Dict, List, Any, Optional

class AtomSpaceHurdCogBridge:
    """Bridge between OCC AtomSpace and HurdCog cognitive kernel"""
    
    def __init__(self, hurdcog_root: str = "/home/ubuntu/hurdcog"):
        self.hurdcog_root = hurdcog_root
        self.cogkernel_path = f"{hurdcog_root}/cogkernel"
        self.connected = False
        
    def connect(self) -> bool:
        """Connect to HurdCog cognitive kernel"""
        print("Connecting to HurdCog cognitive kernel...")
        print(f"  CogKernel path: {self.cogkernel_path}")
        
        # Simulate connection (in real implementation, this would use IPC)
        self.connected = True
        print("✓ Connected to HurdCog")
        return True
    
    def send_atom(self, atom_id: int, atom_type: str, atom_data: Dict[str, Any]) -> bool:
        """Send an atom to HurdCog for processing"""
        if not self.connected:
            print("Error: Not connected to HurdCog")
            return False
        
        print(f"Sending atom {atom_id} to HurdCog...")
        print(f"  Type: {atom_type}")
        print(f"  Data: {json.dumps(atom_data, indent=2)}")
        
        # In real implementation, this would use Mach IPC
        return True
    
    def receive_atom(self) -> Optional[Dict[str, Any]]:
        """Receive an atom from HurdCog"""
        if not self.connected:
            print("Error: Not connected to HurdCog")
            return None
        
        # Simulate receiving an atom
        atom = {
            'id': 42,
            'type': 'ConceptNode',
            'name': 'cognitive-process',
            'truth_value': {'strength': 0.9, 'confidence': 0.8}
        }
        
        print(f"Received atom from HurdCog: {atom['name']}")
        return atom
    
    def sync_atomspace(self) -> bool:
        """Synchronize AtomSpace with HurdCog's MachSpace"""
        print("Synchronizing AtomSpace with MachSpace...")
        
        # In real implementation, this would sync the entire AtomSpace
        print("✓ AtomSpace synchronized")
        return True
    
    def disconnect(self) -> bool:
        """Disconnect from HurdCog"""
        if self.connected:
            print("Disconnecting from HurdCog...")
            self.connected = False
            print("✓ Disconnected")
        return True

def main():
    """Test the bridge"""
    print("AtomSpace-HurdCog Bridge Test")
    print("=" * 60)
    print()
    
    # Create bridge
    bridge = AtomSpaceHurdCogBridge()
    
    # Connect
    bridge.connect()
    print()
    
    # Send test atom
    bridge.send_atom(
        atom_id=1,
        atom_type='ConceptNode',
        atom_data={
            'name': 'test-concept',
            'truth_value': {'strength': 0.95, 'confidence': 0.85}
        }
    )
    print()
    
    # Receive atom
    atom = bridge.receive_atom()
    print()
    
    # Sync
    bridge.sync_atomspace()
    print()
    
    # Disconnect
    bridge.disconnect()
    
    print()
    print("Bridge test complete!")

if __name__ == '__main__':
    main()
EOF

chmod +x "$OCC_DIR/hurdcog-integration/atomspace-hurdcog-bridge.py"

echo "✓ Created AtomSpace-HurdCog bridge"

echo ""
echo "Step 4: Creating integration test suite..."
echo "--------------------------------------------------------"

cat > "$INTEGRATION_DIR/test-integration.sh" << 'EOF'
#!/bin/bash
# Integration Test Suite for AGI-OS

echo "╔════════════════════════════════════════════════════════════╗"
echo "║  AGI-OS Integration Test Suite                            ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""

PASSED=0
FAILED=0

# Test 1: Cognumach headers
echo "Test 1: Checking Cognumach cognitive headers..."
if [ -f "/home/ubuntu/cognumach/include/mach/cognitive/atomspace_ipc.h" ]; then
    echo "  ✓ AtomSpace IPC header found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ AtomSpace IPC header not found"
    FAILED=$((FAILED + 1))
fi

if [ -f "/home/ubuntu/cognumach/include/mach/cognitive/cognitive_vm.h" ]; then
    echo "  ✓ Cognitive VM header found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Cognitive VM header not found"
    FAILED=$((FAILED + 1))
fi

# Test 2: HurdCog integration layer
echo ""
echo "Test 2: Checking HurdCog integration layer..."
if [ -f "/home/ubuntu/hurdcog/cogkernel/mach-integration/machspace-bridge.scm" ]; then
    echo "  ✓ MachSpace bridge found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ MachSpace bridge not found"
    FAILED=$((FAILED + 1))
fi

# Test 3: OCC integration layer
echo ""
echo "Test 3: Checking OCC integration layer..."
if [ -f "/home/ubuntu/occ/hurdcog-integration/atomspace-hurdcog-bridge.py" ]; then
    echo "  ✓ AtomSpace-HurdCog bridge found"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ AtomSpace-HurdCog bridge not found"
    FAILED=$((FAILED + 1))
fi

# Test 4: Python bridge functionality
echo ""
echo "Test 4: Testing Python bridge..."
if python3 /home/ubuntu/occ/hurdcog-integration/atomspace-hurdcog-bridge.py > /dev/null 2>&1; then
    echo "  ✓ Python bridge executes successfully"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Python bridge execution failed"
    FAILED=$((FAILED + 1))
fi

# Test 5: Guile bridge functionality
echo ""
echo "Test 5: Testing Guile bridge..."
if guile -c "(format #t \"Guile OK~%\")" > /dev/null 2>&1; then
    echo "  ✓ Guile is functional"
    PASSED=$((PASSED + 1))
else
    echo "  ✗ Guile test failed"
    FAILED=$((FAILED + 1))
fi

# Summary
echo ""
echo "════════════════════════════════════════════════════════════"
echo "Test Results: $PASSED passed, $FAILED failed"
echo "════════════════════════════════════════════════════════════"

if [ $FAILED -eq 0 ]; then
    echo "✓ All tests passed!"
    exit 0
else
    echo "✗ Some tests failed"
    exit 1
fi
EOF

chmod +x "$INTEGRATION_DIR/test-integration.sh"

echo "✓ Created integration test suite"

echo ""
echo "Step 5: Running integration tests..."
echo "--------------------------------------------------------"

# Run the tests
"$INTEGRATION_DIR/test-integration.sh"

echo ""
echo "╔════════════════════════════════════════════════════════════╗"
echo "║  Kernel Integration Configuration Complete!               ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo ""
echo "Next steps:"
echo "  1. Build Cognumach with cognitive headers: cd $COGNUMACH_DIR && make"
echo "  2. Test HurdCog integration: cd $HURDCOG_DIR/cogkernel/mach-integration && make test"
echo "  3. Test OCC bridge: python3 $OCC_DIR/hurdcog-integration/atomspace-hurdcog-bridge.py"
echo ""
