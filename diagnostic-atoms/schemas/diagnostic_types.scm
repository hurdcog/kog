;; Diagnostic Atom Type Definitions for OpenCog Autognostic System
;; 
;; This file defines specialized atom types for system diagnostics,
;; introspection, and self-repair capabilities.
;;
;; Author: OCC Evolution Team
;; Date: 2025-12-12

;; ============================================================================
;; Base Diagnostic Atom Type
;; ============================================================================

(define-type 'DiagnosticAtom 'Node
  "Base type for all diagnostic atoms. Contains metadata about system state,
   errors, performance metrics, and other introspective information.")

;; ============================================================================
;; Error Diagnostic Atoms
;; ============================================================================

(define-type 'ErrorAtom 'DiagnosticAtom
  "Represents a system error or exception. Contains error type, message,
   stack trace, and context information.")

(define-type 'CriticalErrorAtom 'ErrorAtom
  "Represents a critical error that requires immediate attention and may
   cause system instability.")

(define-type 'RecoverableErrorAtom 'ErrorAtom
  "Represents an error that can potentially be recovered from through
   repair actions.")

;; ============================================================================
;; Prediction and Reasoning Diagnostics
;; ============================================================================

(define-type 'PredictionMismatchAtom 'DiagnosticAtom
  "Represents a mismatch between predicted and actual outcomes in reasoning.
   Contains expected value, actual value, and confidence metrics.")

(define-type 'InferenceFailureAtom 'DiagnosticAtom
  "Represents a failure in PLN inference process. Contains rule information,
   premises, and failure reason.")

(define-type 'TruthValueInconsistencyAtom 'DiagnosticAtom
  "Represents inconsistencies in truth value assignments across related atoms.
   Contains conflicting atoms and their truth values.")

;; ============================================================================
;; Performance and Resource Diagnostics
;; ============================================================================

(define-type 'StallAtom 'DiagnosticAtom
  "Represents a performance stall or bottleneck. Contains duration,
   affected component, and resource utilization.")

(define-type 'ResourceAtom 'DiagnosticAtom
  "Represents resource utilization metrics. Contains CPU, memory, I/O
   statistics and thresholds.")

(define-type 'MemoryPressureAtom 'ResourceAtom
  "Represents high memory usage conditions that may require attention
   allocation adjustments or forgetting.")

(define-type 'CPUBottleneckAtom 'ResourceAtom
  "Represents CPU-bound operations that are limiting system performance.")

;; ============================================================================
;; Attention Allocation Diagnostics (ECAN)
;; ============================================================================

(define-type 'AttentionDiagnosticAtom 'DiagnosticAtom
  "Base type for ECAN attention allocation diagnostics.")

(define-type 'AttentionAllocationFailureAtom 'AttentionDiagnosticAtom
  "Represents failures in attention allocation process. Contains STI/LTI
   values and allocation constraints.")

(define-type 'ThresholdViolationAtom 'AttentionDiagnosticAtom
  "Represents violations of STI/LTI thresholds. Contains threshold values,
   actual values, and affected atoms.")

(define-type 'ForgettingEventAtom 'AttentionDiagnosticAtom
  "Represents atoms being forgotten due to low attention values. Contains
   forgotten atom information and forgetting criteria.")

;; ============================================================================
;; State Snapshot Diagnostics
;; ============================================================================

(define-type 'StateAtom 'DiagnosticAtom
  "Represents a snapshot of system state at a specific point in time.
   Contains timestamp, component states, and metrics.")

(define-type 'CheckpointAtom 'StateAtom
  "Represents a full system checkpoint that can be used for rollback.
   Contains serialized state and metadata.")

;; ============================================================================
;; Repair and Action Diagnostics
;; ============================================================================

(define-type 'RepairAtom 'DiagnosticAtom
  "Represents a repair action or suggestion. Contains action type,
   parameters, and expected outcome.")

(define-type 'RepairSuccessAtom 'DiagnosticAtom
  "Represents successful execution of a repair action. Contains before/after
   metrics and improvement measurements.")

(define-type 'RepairFailureAtom 'DiagnosticAtom
  "Represents failed repair action. Contains failure reason and rollback
   information.")

;; ============================================================================
;; Meta-Level Diagnostics
;; ============================================================================

(define-type 'MetaDiagnosticAtom 'DiagnosticAtom
  "Represents diagnostics about the diagnostic system itself. Used for
   recursive introspection.")

(define-type 'DiagnosticQualityAtom 'MetaDiagnosticAtom
  "Represents quality metrics of diagnostic generation. Contains accuracy,
   completeness, and timeliness measures.")

;; ============================================================================
;; Diagnostic Relationships
;; ============================================================================

(define-type 'CausedByLink 'Link
  "Links a diagnostic atom to its probable cause. Used for causal reasoning
   about system issues.")

(define-type 'TriggeredByLink 'Link
  "Links a diagnostic atom to the event or condition that triggered it.")

(define-type 'RepairedByLink 'Link
  "Links a diagnostic atom to the repair action that addressed it.")

(define-type 'PrecedesLink 'Link
  "Temporal relationship between diagnostic events.")

;; ============================================================================
;; Diagnostic Metadata Predicates
;; ============================================================================

(define-type 'SeverityPredicate 'Predicate
  "Predicate for diagnostic severity level: critical, high, medium, low.")

(define-type 'TimestampPredicate 'Predicate
  "Predicate for diagnostic timestamp in ISO 8601 format.")

(define-type 'ComponentPredicate 'Predicate
  "Predicate for affected system component: ECAN, PLN, Atomspace, etc.")

(define-type 'ConfidencePredicate 'Predicate
  "Predicate for diagnostic confidence level (0.0 to 1.0).")

;; ============================================================================
;; Example Usage
;; ============================================================================

;; Example: Error diagnostic with metadata
;; (Evaluation
;;   (Predicate "severity")
;;   (List
;;     (ErrorAtom "inference_timeout_001")
;;     (Concept "critical")))
;;
;; (Evaluation
;;   (Predicate "timestamp")
;;   (List
;;     (ErrorAtom "inference_timeout_001")
;;     (Concept "2025-12-12T10:30:45Z")))
;;
;; (Evaluation
;;   (Predicate "component")
;;   (List
;;     (ErrorAtom "inference_timeout_001")
;;     (Concept "PLN")))

;; Example: Causal relationship
;; (CausedByLink
;;   (InferenceFailureAtom "pln_failure_042")
;;   (MemoryPressureAtom "mem_pressure_015"))

;; Example: Repair action
;; (RepairedByLink
;;   (StallAtom "reasoning_stall_007")
;;   (RepairAtom "increase_attention_budget"))
