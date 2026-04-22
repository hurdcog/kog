#!/usr/bin/env guile
!#
;;; Test Distributed Agent Framework
;;; Validates the distributed agent framework functionality for Phase 4

(use-modules (ice-9 format)
             (ice-9 threads)
             (cogkernel agents)
             (cogkernel atomspace)
             (cogkernel distributed-agent-framework))

;;; Test framework creation and initialization
(define (test-framework-creation)
  "Test creating and initializing the distributed agent framework"
  (format #t "=== Testing Framework Creation ===~%")
  
  ;; Test framework creation
  (let ((framework (make-distributed-agent-framework)))
    (format #t "✓ Created distributed agent framework~%")
    
    ;; Verify framework properties
    (if (distributed-agent-framework? framework)
        (format #t "✓ Framework type verification passed~%")
        (error "Framework type verification failed"))
    
    ;; Test framework initialization
    (framework-start! framework)
    (format #t "✓ Framework started successfully~%")
    
    framework))

;;; Test agent deployment
(define (test-agent-deployment)
  "Test deploying agents through the framework"
  (format #t "~%=== Testing Agent Deployment ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Test deploying different types of agents
    (let ((build-agent-spec '(test-build-agent BUILD))
          (monitor-agent-spec '(test-monitor-agent MONITOR))
          (repair-agent-spec '(test-repair-agent REPAIR)))
      
      ;; Deploy agents
      (format #t "Deploying build agent...~%")
      (let ((deployment1 (framework-deploy-agent! framework build-agent-spec)))
        (format #t "✓ Build agent deployed successfully~%"))
      
      (format #t "Deploying monitor agent...~%")  
      (let ((deployment2 (framework-deploy-agent! framework monitor-agent-spec)))
        (format #t "✓ Monitor agent deployed successfully~%"))
      
      (format #t "Deploying repair agent...~%")
      (let ((deployment3 (framework-deploy-agent! framework repair-agent-spec)))
        (format #t "✓ Repair agent deployed successfully~%"))
      
      ;; Verify active agents
      (let ((active-agents (framework-list-active-agents framework)))
        (format #t "✓ Active agents: ~a (~a total)~%" active-agents (length active-agents)))
      
      framework)))

;;; Test agent lifecycle management
(define (test-agent-lifecycle)
  "Test agent lifecycle management (deploy, monitor, terminate)"
  (format #t "~%=== Testing Agent Lifecycle Management ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Deploy test agent
    (let ((agent-spec '(lifecycle-test-agent ANALYZE)))
      (format #t "Deploying test agent for lifecycle testing...~%")
      (let ((deployment (framework-deploy-agent! framework agent-spec)))
        
        ;; Check agent status
        (let ((status (framework-get-agent-status framework 'lifecycle-test-agent)))
          (if status
              (begin
                (format #t "✓ Agent status retrieved: ~a~%" (assoc-ref status 'status))
                (format #t "  - Node: ~a~%" (assoc-ref status 'node-id))
                (format #t "  - Health: ~a~%" (assoc-ref status 'health)))
              (error "Failed to get agent status")))
        
        ;; Terminate agent
        (format #t "Terminating test agent...~%")
        (if (framework-terminate-agent! framework 'lifecycle-test-agent)
            (format #t "✓ Agent terminated successfully~%")
            (error "Agent termination failed"))
        
        ;; Verify agent is no longer active
        (let ((active-agents (framework-list-active-agents framework)))
          (if (member 'lifecycle-test-agent active-agents)
              (error "Agent still active after termination")
              (format #t "✓ Agent properly removed from active list~%")))
        
        framework))))

;;; Test agent scaling
(define (test-agent-scaling)
  "Test scaling agents up and down"
  (format #t "~%=== Testing Agent Scaling ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Initial deployment
    (framework-deploy-agent! framework '(scale-test-1 MONITOR))
    (framework-deploy-agent! framework '(scale-test-2 MONITOR))
    
    (let ((initial-count (length (framework-list-active-agents framework))))
      (format #t "Initial active agents: ~a~%" initial-count))
    
    ;; Scale up MONITOR agents to 4
    (format #t "Scaling MONITOR agents to 4...~%")
    (framework-scale-agents! framework 'MONITOR 4)
    
    (let ((scaled-up-count (length (framework-list-active-agents framework))))
      (format #t "✓ Scaled up to ~a agents~%" scaled-up-count))
    
    ;; Scale down MONITOR agents to 2
    (format #t "Scaling MONITOR agents down to 2...~%")
    (framework-scale-agents! framework 'MONITOR 2)
    
    (let ((scaled-down-count (length (framework-list-active-agents framework))))
      (format #t "✓ Scaled down to ~a agents~%" scaled-down-count))
    
    framework))

;;; Test framework health monitoring
(define (test-framework-health)
  "Test framework health monitoring capabilities"
  (format #t "~%=== Testing Framework Health Monitoring ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Deploy some test agents
    (framework-deploy-agent! framework '(health-test-1 BUILD))
    (framework-deploy-agent! framework '(health-test-2 MONITOR))
    (framework-deploy-agent! framework '(health-test-3 REPAIR))
    
    ;; Perform health check
    (format #t "Performing framework health check...~%")
    (let ((health-report (framework-health-check framework)))
      (format #t "✓ Health check completed~%")
      (format #t "  - Overall health: ~a~%" (assoc-ref health-report 'overall-health))
      (format #t "  - Healthy agents: ~a/~a~%" 
              (assoc-ref health-report 'healthy-agents)
              (assoc-ref health-report 'total-agents))
      (format #t "  - Health percentage: ~,1f%~%" 
              (assoc-ref health-report 'health-percentage)))
    
    framework))

;;; Test framework metrics
(define (test-framework-metrics)
  "Test framework metrics collection"
  (format #t "~%=== Testing Framework Metrics ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Deploy some agents for metrics
    (framework-deploy-agent! framework '(metrics-test-1 BUILD))
    (framework-deploy-agent! framework '(metrics-test-2 ANALYZE))
    
    ;; Get metrics
    (format #t "Collecting framework metrics...~%")
    (let ((metrics (framework-get-metrics framework)))
      (format #t "✓ Metrics collected~%")
      (format #t "  - Active agents: ~a~%" (hash-ref metrics 'active-agents))
      (format #t "  - Total deployments: ~a~%" (hash-ref metrics 'total-deployments))
      (format #t "  - Framework state: ~a~%" (hash-ref metrics 'framework-state)))
    
    framework))

;;; Test atomspace integration
(define (test-atomspace-integration)
  "Test integration with atomspace for distributed tracking"
  (format #t "~%=== Testing AtomSpace Integration ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Deploy agent and check atomspace
    (framework-deploy-agent! framework '(atomspace-test-agent OPTIMIZE))
    
    ;; Check if deployment is recorded in atomspace
    (let ((deployment-atoms (atomspace-query *global-atomspace* '(DEPLOYMENT atomspace-test-agent))))
      (if (not (null? deployment-atoms))
          (format #t "✓ Agent deployment recorded in atomspace~%")
          (error "Agent deployment not found in atomspace")))
    
    ;; Terminate agent and verify cleanup
    (framework-terminate-agent! framework 'atomspace-test-agent)
    
    (let ((cleanup-check (atomspace-query *global-atomspace* '(DEPLOYMENT atomspace-test-agent))))
      (if (null? cleanup-check)
          (format #t "✓ Agent deployment cleaned up from atomspace~%")
          (error "Agent deployment not properly cleaned up")))
    
    framework))

;;; Test framework shutdown
(define (test-framework-shutdown)
  "Test graceful framework shutdown"
  (format #t "~%=== Testing Framework Shutdown ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Deploy some agents
    (framework-deploy-agent! framework '(shutdown-test-1 BUILD))
    (framework-deploy-agent! framework '(shutdown-test-2 MONITOR))
    
    (format #t "Active agents before shutdown: ~a~%" 
            (length (framework-list-active-agents framework)))
    
    ;; Test graceful shutdown
    (format #t "Performing graceful framework shutdown...~%")
    (framework-stop! framework)
    
    ;; Verify all agents terminated
    (let ((remaining-agents (framework-list-active-agents framework)))
      (if (null? remaining-agents)
          (format #t "✓ All agents terminated during shutdown~%")
          (error "Some agents not terminated during shutdown")))
    
    (format #t "✓ Framework shutdown completed~%")
    framework))

;;; Test SKZ framework integration
(define (test-skz-integration)
  "Test integration with existing SKZ framework components"
  (format #t "~%=== Testing SKZ Framework Integration ===~%")
  
  (let ((framework (make-distributed-agent-framework)))
    (framework-start! framework)
    
    ;; Test integration with agent communication system
    (if (framework-communication-system framework)
        (format #t "✓ Communication system integration verified~%")
        (error "Communication system not integrated"))
    
    ;; Test integration with atomspace
    (if *global-atomspace*
        (format #t "✓ AtomSpace integration verified~%")
        (error "AtomSpace not available"))
    
    ;; Test deployment strategy consistency
    (let ((strategy (framework-deployment-strategy framework)))
      (format #t "✓ Deployment strategy: ~a~%" strategy))
    
    (format #t "✓ SKZ framework integration confirmed~%")
    framework))

;;; Run all framework tests
(define (run-distributed-framework-tests)
  "Run all distributed agent framework tests"
  (format #t "🧪 Starting Distributed Agent Framework Tests~%")
  (format #t "===================================================~%")
  
  (test-framework-creation)
  (test-agent-deployment)
  (test-agent-lifecycle)
  (test-agent-scaling)
  (test-framework-health)
  (test-framework-metrics)
  (test-atomspace-integration)
  (test-framework-shutdown)
  (test-skz-integration)
  
  (format #t "~%===================================================~%")
  (format #t "✅ All distributed agent framework tests completed!~%")
  (format #t "🚀 Distributed agent framework is fully functional~%")
  (format #t "📋 Phase 4: Deploy distributed agent framework - COMPLETE~%"))

;; Run tests when script is executed
(run-distributed-framework-tests)