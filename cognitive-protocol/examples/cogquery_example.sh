#!/bin/sh
# 9P-Cog Protocol Usage Examples
# Demonstrates cognitive operations via the Styx protocol

echo "=== 9P-Cog Protocol Examples ==="

# 1. Query local AtomSpace for all ConceptNodes
echo ""
echo "1. Query local AtomSpace:"
echo "query ConceptNode" > /dev/cogserver/request 2>/dev/null && \
    cat /dev/cogserver/request 2>/dev/null || \
    echo "   (cogserver not running - simulation mode)"

# 2. Create an atom via cogserver
echo ""
echo "2. Create ConceptNode 'cat':"
echo "1 2 cat" > /dev/cogserver/request 2>/dev/null && \
    echo "   Created" || echo "   (simulation mode)"

# 3. Set truth value
echo ""
echo "3. Set truth value for atom:"
echo "3 1 0.9 0.8" > /dev/cogserver/request 2>/dev/null && \
    echo "   TV set to <0.9, 0.8>" || echo "   (simulation mode)"

# 4. Mount a remote cognitive node and query it
echo ""
echo "4. Mount remote node (requires running node):"
echo "   mount -A tcp!cognode1!564 /n/node1"
echo "   echo 'query ConceptNode' > /n/node1/cog/atomspace/nodes"

# 5. Trigger forward reasoning
echo ""
echo "5. Forward reasoning on goal atom 42:"
echo "   echo '42 5' > /dev/ure/forward"
echo "   cat /dev/ure/forward"

echo ""
echo "=== Examples complete ==="
