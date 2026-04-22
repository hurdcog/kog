# AGI-OS Integration Guide: 5-Repository Architecture

## Overview

This guide provides detailed technical specifications for integrating five OpenCog repositories into a unified AGI-OS (AGI Operating System) with a complete AGI-Kern foundation. It complements the [AGI Kernel Evaluation](AGI_KERNEL_EVALUATION.md) with practical integration strategies.

---

## Repository Integration Map

### 1. opencog/atomspace - Knowledge Kernel
**Role**: Core memory management and knowledge representation

**Kernel Functions Provided**:
- ✅ Memory management (graph-based allocation)
- ✅ System calls (AtomSpace API)
- ✅ Basic I/O (storage backends)
- ✅ Synchronization (thread-safe operations)

**Integration Points**:
```cpp
// Memory allocation through AtomSpace
Handle atom = atomspace->add_node(CONCEPT_NODE, "example");

// Query "syscall"
Handle result = atomspace->execute(pattern_query);

// Persistent I/O
StorageNode storage = atomspace->get_storage_node("rocks://data/");
storage->store_atomspace();
```

**APIs to Expose**:
- Graph CRUD operations
- Pattern matching engine
- Type system
- Value streams
- Storage backends

---

### 2. opencog/cogserver - Service Framework
**Role**: Process management, I/O, and system calls

**Kernel Functions Provided**:
- ✅ Process/thread management (modules, agents)
- ✅ System call interface (Request system)
- ✅ I/O primitives (network, console)
- ✅ Boot/init (server lifecycle)

**Integration Points**:
```cpp
// Register a kernel service
class MyKernelService : public Request {
    bool execute() override {
        // Service logic
        return true;
    }
};

// Load kernel module
cogserver().loadModule("libMyModule.so");

// Network I/O
NetworkServer server(atomspace);
server.listen(17001);
```

**APIs to Expose**:
- Module loading/unloading
- Request registration and dispatch
- Network server management
- Console/shell access
- Session management

---

### 3. opencog/pln - Reasoning Subsystem
**Role**: Cognitive task scheduling and inference engine

**Kernel Functions Provided**:
- 🟢 CPU scheduling (inference task prioritization)
- 🟢 Interrupt handling (inference triggers)

**Integration Points**:
```cpp
// Schedule inference task with priority
InferenceTask task(query, context);
task.set_priority(HIGH);
pln_scheduler.submit(task);

// Interrupt-driven inference
attention_signal.connect([&](Handle atom) {
    pln_scheduler.trigger_inference(atom);
});
```

**APIs to Expose**:
- Inference task submission
- Rule selection strategies
- Backward/forward chaining
- Uncertainty tracking (TruthValues)
- Inference callbacks

**Integration with AGI-Kern**:
- Map inference tasks to scheduler priorities
- Use attention signals as cognitive "interrupts"
- Integrate with timer service for deadline inference

---

### 4. opencog/attention - Resource Manager
**Role**: Priority scheduling, interrupt handling, resource allocation

**Kernel Functions Provided**:
- 🟢 CPU scheduling (attention-based prioritization)
- 🟢 Interrupt handling (importance-triggered events)
- 🟢 Memory management (importance-based retention)

**Integration Points**:
```cpp
// Set atom importance (priority)
AttentionValue av(100, 10, 5); // STI, LTI, VLTI
atom->set_av(av);

// Importance spreading (resource propagation)
ImportanceDiffusion diffuser(atomspace);
diffuser.spread_importance();

// Attention allocation (scheduler)
AttentionBank bank(atomspace);
Handle most_important = bank.get_top_STI_atom();

// Forgetting (memory management)
Forgetting forget(atomspace);
forget.forget_atoms(threshold);
```

**APIs to Expose**:
- Importance value getters/setters
- Attention allocation algorithms
- Forgetting mechanisms
- HebbianLink learning
- Attention signals/events

**Integration with AGI-Kern**:
```cpp
// AGI_Scheduler uses attention for priorities
class AGI_Scheduler {
    Task* get_next_task() {
        Handle important_atom = attention_bank.get_top_STI();
        return task_map[important_atom];
    }
};

// AGI_InterruptController uses importance thresholds
class AGI_InterruptController {
    void on_importance_change(Handle h, AttentionValue av) {
        if (av.getSTI() > INTERRUPT_THRESHOLD) {
            trigger_event(IMPORTANCE_INTERRUPT, h);
        }
    }
};

// AGI_MemoryManager uses forgetting
class AGI_MemoryManager {
    void gc() {
        forgetting.forget_atoms(importance_threshold);
    }
};
```

---

### 5. opencog/moses - Meta-Learning Subsystem
**Role**: Self-modification, adaptive optimization

**Kernel Functions Provided**:
- 🟡 Boot/init (self-optimization at startup)
- 🟡 CPU scheduling (adaptive algorithm synthesis)

**Integration Points**:
```cpp
// Evolve scheduler policy
combo_tree policy = moses_evolve(scheduler_fitness);
scheduler.update_policy(policy);

// Synthesize memory allocator
combo_tree allocator = moses_evolve(allocation_fitness);
memory_manager.update_strategy(allocator);

// Self-improvement loop
while (running) {
    metrics = collect_performance_metrics();
    improved_algorithm = moses.evolve(metrics);
    if (improved_algorithm.fitness > current.fitness) {
        replace_algorithm(improved_algorithm);
    }
}
```

**APIs to Expose**:
- Program evolution
- Fitness function integration
- Metapopulation management
- Combo tree execution
- Feature selection

**Integration with AGI-Kern**:
```cpp
// Self-optimizing kernel
class AGI_SelfOptimizer {
    void optimize_kernel() {
        // Collect kernel metrics
        KernelMetrics metrics = {
            .scheduler_latency = measure_scheduler_latency(),
            .memory_fragmentation = measure_fragmentation(),
            .io_throughput = measure_io_throughput()
        };
        
        // Evolve improvements
        auto improved_scheduler = moses.evolve(
            scheduler_fitness(metrics)
        );
        
        // Hot-swap algorithms (with safety checks)
        if (validate_safety(improved_scheduler)) {
            kernel.update_scheduler(improved_scheduler);
        }
    }
};
```

---

## AGI-Kern Implementation Details

### AGI_Boot: Unified Boot Manager

```cpp
class AGI_Boot {
public:
    void boot() {
        // Phase 1: Initialize core data structures
        atomspace_ = std::make_shared<AtomSpace>();
        
        // Phase 2: Start foundational services
        cogserver_ = &cogserver(atomspace_);
        cogserver_->loadModule("libattention.so");
        cogserver_->loadModule("libpln.so");
        
        // Phase 3: Initialize resource managers
        attention_bank_ = std::make_shared<AttentionBank>(atomspace_);
        pln_engine_ = std::make_shared<PLNEngine>(atomspace_);
        
        // Phase 4: Start cognitive subsystems
        coggml_kernel_.initialize();
        cogself_framework_.initialize();
        
        // Phase 5: Load user modules
        load_user_modules();
        
        // Phase 6: Start self-optimization
        if (config_.enable_self_optimization) {
            moses_optimizer_.start();
        }
        
        // Phase 7: Enter main loop
        running_ = true;
        cogserver_->serverLoop();
    }
    
    void shutdown() {
        running_ = false;
        moses_optimizer_.stop();
        cogself_framework_.shutdown();
        coggml_kernel_.shutdown();
        cogserver_->stop();
        atomspace_->clear();
    }
    
private:
    AtomSpacePtr atomspace_;
    CogServer* cogserver_;
    AttentionBankPtr attention_bank_;
    PLNEnginePtr pln_engine_;
    coggml::Microkernel coggml_kernel_;
    cogself::Framework cogself_framework_;
    MOSESOptimizer moses_optimizer_;
    bool running_;
};
```

---

### AGI_Scheduler: Attention-Aware Task Scheduler

```cpp
class AGI_Scheduler {
public:
    AGI_Scheduler(AtomSpacePtr as, AttentionBankPtr bank)
        : atomspace_(as), attention_bank_(bank) {}
    
    void submit_task(CognitiveTask task) {
        // Get importance-based priority
        Handle task_handle = task.get_handle();
        AttentionValue av = task_handle->getAV();
        Priority priority = sti_to_priority(av.getSTI());
        
        // Add to priority queue
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.emplace(priority, task);
        cv_.notify_one();
    }
    
    void scheduler_loop() {
        while (running_) {
            CognitiveTask task = get_next_task();
            
            // Check if PLN inference task
            if (task.type() == TaskType::INFERENCE) {
                pln_engine_->execute(task);
            }
            // Check if learning task
            else if (task.type() == TaskType::LEARNING) {
                moses_engine_->execute(task);
            }
            // Generic cognitive task
            else {
                execute_task(task);
            }
            
            // Update attention based on results
            update_attention(task);
        }
    }
    
private:
    CognitiveTask get_next_task() {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        cv_.wait(lock, [this] { return !task_queue_.empty(); });
        
        auto task = task_queue_.top();
        task_queue_.pop();
        return task.second;
    }
    
    Priority sti_to_priority(AttentionValue::sti_t sti) {
        if (sti > 100) return Priority::CRITICAL;
        if (sti > 50) return Priority::HIGH;
        if (sti > 10) return Priority::NORMAL;
        return Priority::LOW;
    }
    
    void update_attention(const CognitiveTask& task) {
        // Hebbian learning: strengthen important atoms
        Handle result = task.get_result();
        if (result && task.was_successful()) {
            AttentionValue av = result->getAV();
            av.setSTI(av.getSTI() + 10);
            result->setAV(av);
            
            // Spread importance
            importance_diffusion_.spread(result);
        }
    }
    
    AtomSpacePtr atomspace_;
    AttentionBankPtr attention_bank_;
    std::priority_queue<std::pair<Priority, CognitiveTask>> task_queue_;
    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool running_ = true;
    PLNEnginePtr pln_engine_;
    MOSESEnginePtr moses_engine_;
    ImportanceDiffusion importance_diffusion_;
};
```

---

### AGI_InterruptController: Event-Driven Cognitive System

```cpp
class AGI_InterruptController {
public:
    enum InterruptType {
        ATTENTION_SPIKE,      // Importance threshold crossed
        INFERENCE_COMPLETE,   // PLN finished reasoning
        LEARNING_CONVERGED,   // MOSES found solution
        SENSOR_EVENT,         // External stimulus
        TIMER_EXPIRED,        // Scheduled event
        MEMORY_PRESSURE,      // Low memory warning
        COGNITIVE_ANOMALY     // Unexpected pattern
    };
    
    void register_handler(InterruptType type, InterruptHandler handler) {
        handlers_[type] = handler;
    }
    
    void trigger_interrupt(InterruptType type, void* data) {
        // Get priority based on type
        Priority priority = get_interrupt_priority(type);
        
        // Create interrupt task
        Interrupt interrupt{type, priority, data, std::chrono::steady_clock::now()};
        
        // Queue interrupt
        {
            std::lock_guard<std::mutex> lock(interrupt_mutex_);
            interrupt_queue_.emplace(priority, interrupt);
        }
        
        // Wake interrupt handler thread
        interrupt_cv_.notify_one();
    }
    
    void interrupt_loop() {
        while (running_) {
            Interrupt interrupt = get_next_interrupt();
            
            // Find and execute handler
            auto it = handlers_.find(interrupt.type);
            if (it != handlers_.end()) {
                it->second(interrupt.data);
            }
            
            // Log interrupt for debugging
            log_interrupt(interrupt);
        }
    }
    
    // Integration with Attention
    void setup_attention_interrupts(AttentionBankPtr bank) {
        // Trigger interrupt when importance crosses threshold
        bank->on_importance_change([this](Handle h, AttentionValue av) {
            if (av.getSTI() > ATTENTION_INTERRUPT_THRESHOLD) {
                trigger_interrupt(ATTENTION_SPIKE, &h);
            }
        });
    }
    
    // Integration with PLN
    void setup_pln_interrupts(PLNEnginePtr pln) {
        pln->on_inference_complete([this](InferenceResult result) {
            trigger_interrupt(INFERENCE_COMPLETE, &result);
        });
    }
    
private:
    std::map<InterruptType, InterruptHandler> handlers_;
    std::priority_queue<std::pair<Priority, Interrupt>> interrupt_queue_;
    std::mutex interrupt_mutex_;
    std::condition_variable interrupt_cv_;
    bool running_ = true;
};
```

---

### AGI_MemoryManager: Attention-Based Memory

```cpp
class AGI_MemoryManager {
public:
    AGI_MemoryManager(AtomSpacePtr as, AttentionBankPtr bank)
        : atomspace_(as), attention_bank_(bank) {}
    
    // Hierarchical memory: L1=RAM, L2=RocksDB, L3=PostgreSQL
    void setup_storage_hierarchy() {
        // L1: AtomSpace in RAM
        ram_storage_ = atomspace_;
        
        // L2: RocksDB for warm storage
        rocks_storage_ = StorageNode::factory(
            "rocks:///var/lib/agi-os/atomspace/"
        );
        rocks_storage_->open();
        
        // L3: PostgreSQL for cold storage
        pg_storage_ = StorageNode::factory(
            "postgres://username@localhost/atomspace"
        );
        pg_storage_->open();
    }
    
    // Importance-based swapping
    void manage_memory() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::seconds(60));
            
            // Check memory pressure
            if (get_memory_usage() > HIGH_WATER_MARK) {
                // Move low-importance atoms to L2
                HandleSeq low_importance = attention_bank_->get_bottom_STI(1000);
                for (Handle h : low_importance) {
                    rocks_storage_->store_atom(h);
                    atomspace_->extract_atom(h);
                }
            }
            
            if (get_storage_usage() > L2_HIGH_WATER_MARK) {
                // Move very low importance to L3
                HandleSeq very_low = get_low_importance_from_rocks(1000);
                for (Handle h : very_low) {
                    pg_storage_->store_atom(h);
                    rocks_storage_->remove_atom(h);
                }
            }
        }
    }
    
    // Fault handler: retrieve from lower levels
    Handle fetch_atom(const std::string& name, Type type) {
        // Try L1 (RAM)
        Handle h = atomspace_->get_node(type, name);
        if (h) return h;
        
        // Try L2 (RocksDB)
        h = rocks_storage_->fetch_atom(type, name);
        if (h) {
            atomspace_->add_atom(h);
            return h;
        }
        
        // Try L3 (PostgreSQL)
        h = pg_storage_->fetch_atom(type, name);
        if (h) {
            atomspace_->add_atom(h);
            // Also cache in L2
            rocks_storage_->store_atom(h);
            return h;
        }
        
        return Handle::UNDEFINED;
    }
    
    // Forgetting (garbage collection)
    void forget_unused() {
        Forgetting forgetter(atomspace_);
        
        // Forget atoms with LTI < threshold
        forgetter.set_LTI_threshold(LTI_FORGET_THRESHOLD);
        HandleSeq forgotten = forgetter.forget();
        
        logger_.info("Forgot {} atoms", forgotten.size());
    }
    
private:
    AtomSpacePtr atomspace_;
    AttentionBankPtr attention_bank_;
    StorageNodePtr rocks_storage_;
    StorageNodePtr pg_storage_;
    bool running_ = true;
};
```

---

### AGI_SecurityManager: Capability-Based Access Control

```cpp
class AGI_SecurityManager {
public:
    enum PrivilegeLevel {
        KERNEL,   // Core kernel code
        SYSTEM,   // Trusted modules
        USER      // Untrusted user code
    };
    
    struct Capability {
        std::string operation;  // e.g., "atomspace.read", "pln.execute"
        HandleSet scope;        // Atoms this applies to
        time_t expiry;         // Capability expiration
    };
    
    // Grant capability token
    CapabilityToken grant(PrivilegeLevel level, 
                          const std::string& operation,
                          HandleSet scope = HandleSet()) {
        Capability cap{operation, scope, time(nullptr) + 3600};
        CapabilityToken token = generate_token();
        
        std::lock_guard<std::mutex> lock(cap_mutex_);
        capabilities_[token] = cap;
        
        return token;
    }
    
    // Check access
    bool check(CapabilityToken token, 
               const std::string& operation,
               Handle atom = Handle::UNDEFINED) {
        std::lock_guard<std::mutex> lock(cap_mutex_);
        
        auto it = capabilities_.find(token);
        if (it == capabilities_.end()) {
            return false;  // Invalid token
        }
        
        Capability& cap = it->second;
        
        // Check expiry
        if (time(nullptr) > cap.expiry) {
            capabilities_.erase(it);
            return false;
        }
        
        // Check operation
        if (cap.operation != operation && cap.operation != "*") {
            return false;
        }
        
        // Check scope
        if (atom != Handle::UNDEFINED && 
            !cap.scope.empty() && 
            cap.scope.find(atom) == cap.scope.end()) {
            return false;
        }
        
        return true;
    }
    
    // Sandbox execution
    template<typename Func>
    auto sandbox_execute(PrivilegeLevel level, Func&& func) {
        // Set thread-local privilege level
        current_privilege_ = level;
        
        try {
            auto result = func();
            current_privilege_ = KERNEL;
            return result;
        } catch (...) {
            current_privilege_ = KERNEL;
            throw;
        }
    }
    
private:
    std::map<CapabilityToken, Capability> capabilities_;
    std::mutex cap_mutex_;
    thread_local static PrivilegeLevel current_privilege_;
    
    CapabilityToken generate_token() {
        // Cryptographically secure token generation
        return /* implementation */;
    }
};
```

---

## Integration Testing Strategy

### Test 1: Boot Sequence
```cpp
TEST(AGI_OS_Integration, BootSequence) {
    AGI_Boot boot;
    ASSERT_NO_THROW(boot.boot());
    
    // Verify all components initialized
    EXPECT_TRUE(boot.atomspace() != nullptr);
    EXPECT_TRUE(boot.cogserver() != nullptr);
    EXPECT_TRUE(boot.attention_bank() != nullptr);
    EXPECT_TRUE(boot.pln_engine() != nullptr);
    
    ASSERT_NO_THROW(boot.shutdown());
}
```

### Test 2: Attention-Based Scheduling
```cpp
TEST(AGI_OS_Integration, AttentionScheduling) {
    AtomSpacePtr as = std::make_shared<AtomSpace>();
    AttentionBankPtr bank = std::make_shared<AttentionBank>(as);
    AGI_Scheduler scheduler(as, bank);
    
    // Create tasks with different importance
    Handle h1 = as->add_node(CONCEPT_NODE, "urgent");
    h1->setSTI(200);  // High importance
    
    Handle h2 = as->add_node(CONCEPT_NODE, "normal");
    h2->setSTI(50);   // Medium importance
    
    CognitiveTask task1(h1, []() { /* urgent work */ });
    CognitiveTask task2(h2, []() { /* normal work */ });
    
    scheduler.submit_task(task2);
    scheduler.submit_task(task1);
    
    // Verify high-importance task executes first
    CognitiveTask next = scheduler.get_next_task();
    EXPECT_EQ(next.get_handle(), h1);
}
```

### Test 3: Hierarchical Memory
```cpp
TEST(AGI_OS_Integration, HierarchicalMemory) {
    AtomSpacePtr as = std::make_shared<AtomSpace>();
    AttentionBankPtr bank = std::make_shared<AttentionBank>(as);
    AGI_MemoryManager mem_mgr(as, bank);
    
    mem_mgr.setup_storage_hierarchy();
    
    // Add atoms with varying importance
    Handle h_important = as->add_node(CONCEPT_NODE, "critical");
    h_important->setSTI(200);
    
    Handle h_unimportant = as->add_node(CONCEPT_NODE, "trivial");
    h_unimportant->setSTI(1);
    
    // Trigger memory pressure
    mem_mgr.manage_memory();
    
    // Verify important atom stays in RAM
    EXPECT_TRUE(as->get_node(CONCEPT_NODE, "critical"));
    
    // Verify unimportant atom moved to storage
    // (may not be in AtomSpace but should be fetchable)
    Handle h_fetch = mem_mgr.fetch_atom("trivial", CONCEPT_NODE);
    EXPECT_TRUE(h_fetch);
}
```

### Test 4: Security Sandbox
```cpp
TEST(AGI_OS_Integration, SecuritySandbox) {
    AGI_SecurityManager security;
    AtomSpacePtr as = std::make_shared<AtomSpace>();
    
    // Grant limited capability
    CapabilityToken token = security.grant(
        AGI_SecurityManager::USER,
        "atomspace.read"
    );
    
    // Verify allowed operation
    EXPECT_TRUE(security.check(token, "atomspace.read"));
    
    // Verify denied operation
    EXPECT_FALSE(security.check(token, "atomspace.write"));
    
    // Test sandboxed execution
    security.sandbox_execute(AGI_SecurityManager::USER, [&]() {
        // This should fail: user trying to do kernel operation
        EXPECT_THROW(
            perform_kernel_operation(),
            SecurityException
        );
    });
}
```

---

## Performance Optimization

### NUMA-Aware AtomSpace
```cpp
class NUMA_AtomSpace : public AtomSpace {
public:
    NUMA_AtomSpace(int numa_nodes) {
        // Create per-NUMA-node AtomSpace instances
        for (int i = 0; i < numa_nodes; i++) {
            numa_spaces_.push_back(
                std::make_shared<AtomSpace>()
            );
        }
    }
    
    Handle add_node(Type type, const std::string& name) override {
        // Hash name to determine NUMA node
        int node = hash(name) % numa_spaces_.size();
        
        // Pin thread to NUMA node
        numa_run_on_node(node);
        
        return numa_spaces_[node]->add_node(type, name);
    }
    
private:
    std::vector<AtomSpacePtr> numa_spaces_;
};
```

---

## Distributed AGI-OS

### Cross-Node Communication
```cpp
class Distributed_AGI_OS {
public:
    Distributed_AGI_OS(const std::vector<std::string>& node_addresses) {
        // Connect to peer nodes
        for (const auto& addr : node_addresses) {
            peers_.push_back(CogStorageNode::factory(addr));
        }
    }
    
    // Distributed query
    HandleSeq distributed_query(const Handle& pattern) {
        std::vector<std::future<HandleSeq>> futures;
        
        // Submit to all nodes
        for (auto& peer : peers_) {
            futures.push_back(std::async([&]() {
                return peer->fetch_query(pattern);
            }));
        }
        
        // Merge results
        HandleSeq results;
        for (auto& future : futures) {
            HandleSeq node_results = future.get();
            results.insert(results.end(), 
                          node_results.begin(), 
                          node_results.end());
        }
        
        return results;
    }
    
    // Distributed attention
    void synchronize_attention() {
        // Broadcast local attention values
        for (auto& peer : peers_) {
            peer->store_atomspace();  // Includes attention values
        }
        
        // Fetch remote attention values
        for (auto& peer : peers_) {
            peer->fetch_atomspace();
        }
        
        // Merge using average or max
        merge_attention_values();
    }
    
private:
    std::vector<StorageNodePtr> peers_;
};
```

---

## Conclusion

This integration guide provides:
1. **Concrete implementations** of AGI-Kern components
2. **Integration patterns** for 5 OpenCog repositories
3. **Testing strategies** for validation
4. **Performance optimizations** for production use
5. **Distribution strategies** for multi-node AGI-OS

With these specifications, the OCC can evolve into a complete AGI Operating System, bridging cognitive AI and systems programming.

**Next Steps**:
1. Implement AGI_Boot with dependency management
2. Integrate PLN and Attention with AGI_Scheduler
3. Build AGI_SecurityManager with capabilities
4. Develop comprehensive test suite
5. Deploy distributed AGI-OS across cluster

---

**Document Version**: 1.0  
**Companion to**: AGI_KERNEL_EVALUATION.md  
**Date**: October 25, 2025
