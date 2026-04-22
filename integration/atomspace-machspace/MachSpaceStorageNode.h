/*
 * MachSpaceStorageNode.h
 * 
 * AtomSpace storage backend using HurdCog's MachSpace
 * Provides distributed hypergraph storage at kernel level
 *
 * Copyright (C) 2025 OpenCog Foundation
 */

#ifndef _OPENCOG_MACHSPACE_STORAGE_NODE_H
#define _OPENCOG_MACHSPACE_STORAGE_NODE_H

#include <opencog/atomspace/AtomSpace.h>
#include <opencog/persist/api/BackingStore.h>
#include <opencog/atoms/base/Node.h>

namespace opencog
{

class MachSpaceStorageNode : public Node
{
private:
    std::string _uri;
    bool _connected;
    void* _machspace_handle;  // Opaque handle to MachSpace connection
    
    // Internal methods
    void connect_to_machspace();
    void disconnect_from_machspace();
    
    // Atom serialization for IPC
    std::string serialize_atom(const Handle& h);
    Handle deserialize_atom(const std::string& data);

protected:
    // BackingStore interface implementation
    void do_open();
    void do_close();
    void do_store_atom(const Handle& h);
    Handle do_fetch_atom(const Handle& h);
    void do_fetch_incoming_set(AtomSpace*, const Handle& h);
    void do_fetch_incoming_by_type(AtomSpace*, const Handle& h, Type t);
    void do_load_atomspace(AtomSpace*);
    void do_store_atomspace(AtomSpace*);
    
public:
    MachSpaceStorageNode(const std::string& uri);
    virtual ~MachSpaceStorageNode();
    
    // Connection management
    void open();
    void close();
    bool connected() const { return _connected; }
    
    // Atom operations
    void store_atom(const Handle& h);
    Handle fetch_atom(const Handle& h);
    HandleSeq fetch_incoming_set(const Handle& h);
    
    // Bulk operations
    void barrier();  // Sync with MachSpace
    void flush();    // Force write to MachSpace
    
    // MachSpace-specific operations
    void set_attention_value(const Handle& h, const AttentionValuePtr& av);
    AttentionValuePtr get_attention_value(const Handle& h);
    
    // Query interface
    HandleSeq query_by_pattern(const Handle& pattern);
    HandleSeq query_by_attention(float min_sti, float max_sti);
    
    // Statistics
    size_t get_atom_count();
    size_t get_memory_usage();
    
    static Handle factory(const Handle&);
};

typedef std::shared_ptr<MachSpaceStorageNode> MachSpaceStorageNodePtr;

} // namespace opencog

#endif // _OPENCOG_MACHSPACE_STORAGE_NODE_H
