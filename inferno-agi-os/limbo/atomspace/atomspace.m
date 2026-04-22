AtomSpace: module
{
    PATH: con "/dis/lib/atomspace.dis";
    
    #
    # AtomSpace Module for Limbo
    #
    # This module provides high-level Limbo bindings to the AtomSpace
    # kernel module, enabling cognitive applications to be written in
    # the Limbo programming language.
    #
    
    # Atom Types
    NODE: con 1;
    CONCEPT: con 2;
    PREDICATE: con 3;
    SCHEMA: con 4;
    GROUNDED_SCHEMA: con 5;
    VARIABLE: con 6;
    NUMBER: con 7;
    
    LINK: con 100;
    ORDERED_LINK: con 101;
    UNORDERED_LINK: con 102;
    INHERITANCE: con 103;
    SIMILARITY: con 104;
    IMPLICATION: con 105;
    EQUIVALENCE: con 106;
    EVALUATION: con 107;
    EXECUTION: con 108;
    MEMBER: con 109;
    LIST: con 110;
    AND: con 111;
    OR: con 112;
    NOT: con 113;
    
    # Truth Value ADT
    TruthValue: adt {
        strength: real;      # Probability [0.0, 1.0]
        confidence: real;    # Confidence [0.0, 1.0]
        count: int;          # Evidence count
        
        # Create a new truth value
        mk: fn(strength: real, confidence: real): ref TruthValue;
        
        # Default truth value (0.0, 0.0)
        default: fn(): ref TruthValue;
        
        # Merge two truth values
        merge: fn(tv1: self ref TruthValue, tv2: ref TruthValue): ref TruthValue;
        
        # Get mean strength
        mean: fn(tv: self ref TruthValue): real;
        
        # Convert to string
        text: fn(tv: self ref TruthValue): string;
    };
    
    # Attention Value ADT
    AttentionValue: adt {
        sti: int;            # Short-term importance
        lti: int;            # Long-term importance
        vlti: int;           # Very long-term importance
        
        # Create a new attention value
        mk: fn(sti: int, lti: int, vlti: int): ref AttentionValue;
        
        # Default attention value (0, 0, 0)
        default: fn(): ref AttentionValue;
        
        # Stimulate attention
        stimulate: fn(av: self ref AttentionValue, amount: int);
        
        # Decay attention
        decay: fn(av: self ref AttentionValue, rate: real);
        
        # Convert to string
        text: fn(av: self ref AttentionValue): string;
    };
    
    # Atom ADT
    Atom: adt {
        id: int;             # Unique atom identifier
        atype: int;          # Atom type
        tv: ref TruthValue;  # Truth value
        av: ref AttentionValue; # Attention value
        
        # Get atom name (for nodes)
        name: fn(a: self ref Atom): string;
        
        # Get outgoing atoms (for links)
        outgoing: fn(a: self ref Atom): array of ref Atom;
        
        # Get incoming atoms
        incoming: fn(a: self ref Atom): array of ref Atom;
        
        # Set truth value
        settv: fn(a: self ref Atom, tv: ref TruthValue);
        
        # Get truth value
        gettv: fn(a: self ref Atom): ref TruthValue;
        
        # Set attention value
        setav: fn(a: self ref Atom, av: ref AttentionValue);
        
        # Get attention value
        getav: fn(a: self ref Atom): ref AttentionValue;
        
        # Convert to string
        text: fn(a: self ref Atom): string;
    };
    
    # AtomSpace ADT
    Space: adt {
        fd: ref Sys->FD;     # File descriptor to /atoms
        
        # Open the AtomSpace
        open: fn(): ref Space;
        
        # Close the AtomSpace
        close: fn(s: self ref Space);
        
        # Create a concept node
        concept: fn(s: self ref Space, name: string): ref Atom;
        
        # Create a predicate node
        predicate: fn(s: self ref Space, name: string): ref Atom;
        
        # Create a schema node
        schema: fn(s: self ref Space, name: string): ref Atom;
        
        # Create a variable node
        variable: fn(s: self ref Space, name: string): ref Atom;
        
        # Create a number node
        number: fn(s: self ref Space, value: real): ref Atom;
        
        # Create an inheritance link
        inheritance: fn(s: self ref Space, child: ref Atom, parent: ref Atom): ref Atom;
        
        # Create a similarity link
        similarity: fn(s: self ref Space, a: ref Atom, b: ref Atom): ref Atom;
        
        # Create an implication link
        implication: fn(s: self ref Space, antecedent: ref Atom, consequent: ref Atom): ref Atom;
        
        # Create an evaluation link
        evaluation: fn(s: self ref Space, predicate: ref Atom, args: array of ref Atom): ref Atom;
        
        # Create a list link
        list: fn(s: self ref Space, atoms: array of ref Atom): ref Atom;
        
        # Create an AND link
        andlink: fn(s: self ref Space, atoms: array of ref Atom): ref Atom;
        
        # Create an OR link
        orlink: fn(s: self ref Space, atoms: array of ref Atom): ref Atom;
        
        # Create a NOT link
        notlink: fn(s: self ref Space, atom: ref Atom): ref Atom;
        
        # Query atoms by name
        queryname: fn(s: self ref Space, name: string): array of ref Atom;
        
        # Query atoms by type
        querytype: fn(s: self ref Space, atype: int): array of ref Atom;
        
        # Get atom by ID
        get: fn(s: self ref Space, id: int): ref Atom;
        
        # Delete atom
        delete: fn(s: self ref Space, atom: ref Atom): int;
        
        # Get statistics
        stats: fn(s: self ref Space): (int, int, int);  # (total, nodes, links)
    };
    
    # Initialize the module
    init: fn();
    
    # Create a node (low-level)
    createnode: fn(atype: int, name: string): ref Atom;
    
    # Create a link (low-level)
    createlink: fn(atype: int, outgoing: array of ref Atom): ref Atom;
    
    # Query the AtomSpace (low-level)
    query: fn(pattern: string): array of ref Atom;
    
    # Convert atom type to string
    typename: fn(atype: int): string;
    
    # Convert string to atom type
    typecode: fn(name: string): int;
};
