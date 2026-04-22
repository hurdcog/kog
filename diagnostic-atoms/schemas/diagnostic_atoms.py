"""
Diagnostic Atom Classes for OpenCog Autognostic System

This module provides Python class definitions for diagnostic atoms that
correspond to the Scheme type definitions in diagnostic_types.scm.

Author: OCC Evolution Team
Date: 2025-12-12
"""

from dataclasses import dataclass, field
from typing import Optional, Dict, Any, List
from datetime import datetime
from enum import Enum


class SeverityLevel(Enum):
    """Severity levels for diagnostic atoms."""
    CRITICAL = "critical"
    HIGH = "high"
    MEDIUM = "medium"
    LOW = "low"
    INFO = "info"


class ComponentType(Enum):
    """System components that can generate diagnostics."""
    ATOMSPACE = "atomspace"
    ECAN = "ecan"
    PLN = "pln"
    COGSERVER = "cogserver"
    ATTENTION = "attention"
    URE = "ure"
    MOSES = "moses"
    UNKNOWN = "unknown"


@dataclass
class DiagnosticAtom:
    """
    Base class for all diagnostic atoms.
    
    Attributes:
        atom_id: Unique identifier for this diagnostic atom
        timestamp: When this diagnostic was generated
        component: Which system component generated this diagnostic
        severity: Severity level of the diagnostic
        confidence: Confidence level (0.0 to 1.0) in this diagnostic
        metadata: Additional metadata as key-value pairs
    """
    atom_id: str
    timestamp: datetime = field(default_factory=datetime.utcnow)
    component: ComponentType = ComponentType.UNKNOWN
    severity: SeverityLevel = SeverityLevel.INFO
    confidence: float = 1.0
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert diagnostic atom to dictionary representation."""
        return {
            "atom_id": self.atom_id,
            "atom_type": self.__class__.__name__,
            "timestamp": self.timestamp.isoformat(),
            "component": self.component.value,
            "severity": self.severity.value,
            "confidence": self.confidence,
            "metadata": self.metadata
        }
    
    def to_atomese(self) -> str:
        """Convert diagnostic atom to Atomese representation."""
        raise NotImplementedError("Subclasses must implement to_atomese()")


@dataclass
class ErrorAtom(DiagnosticAtom):
    """
    Represents a system error or exception.
    
    Attributes:
        error_type: Type of error (e.g., "RuntimeError", "ValueError")
        message: Error message
        stack_trace: Stack trace if available
        context: Additional context about the error
    """
    error_type: str = ""
    message: str = ""
    stack_trace: Optional[str] = None
    context: Dict[str, Any] = field(default_factory=dict)
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        return f"""(ErrorAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "error_type")
  (List
    (ErrorAtom "{self.atom_id}")
    (Concept "{self.error_type}")))

(Evaluation
  (Predicate "message")
  (List
    (ErrorAtom "{self.atom_id}")
    (Concept "{self.message}")))

(Evaluation
  (Predicate "severity")
  (List
    (ErrorAtom "{self.atom_id}")
    (Concept "{self.severity.value}")))"""


@dataclass
class CriticalErrorAtom(ErrorAtom):
    """Represents a critical error requiring immediate attention."""
    
    def __post_init__(self):
        self.severity = SeverityLevel.CRITICAL


@dataclass
class RecoverableErrorAtom(ErrorAtom):
    """Represents an error that can potentially be recovered from."""
    recovery_suggestions: List[str] = field(default_factory=list)


@dataclass
class PredictionMismatchAtom(DiagnosticAtom):
    """
    Represents a mismatch between predicted and actual outcomes.
    
    Attributes:
        predicted_value: The value that was predicted
        actual_value: The actual observed value
        prediction_confidence: Confidence in the original prediction
        mismatch_magnitude: Magnitude of the mismatch
    """
    predicted_value: Any = None
    actual_value: Any = None
    prediction_confidence: float = 0.0
    mismatch_magnitude: float = 0.0
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        return f"""(PredictionMismatchAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "predicted_value")
  (List
    (PredictionMismatchAtom "{self.atom_id}")
    (Concept "{self.predicted_value}")))

(Evaluation
  (Predicate "actual_value")
  (List
    (PredictionMismatchAtom "{self.atom_id}")
    (Concept "{self.actual_value}")))

(Evaluation
  (Predicate "mismatch_magnitude")
  (List
    (PredictionMismatchAtom "{self.atom_id}")
    (Number {self.mismatch_magnitude})))"""


@dataclass
class InferenceFailureAtom(DiagnosticAtom):
    """
    Represents a failure in PLN inference process.
    
    Attributes:
        rule_name: Name of the inference rule that failed
        premises: List of premise atoms
        failure_reason: Reason for the failure
        attempted_conclusion: The conclusion that was being attempted
    """
    rule_name: str = ""
    premises: List[str] = field(default_factory=list)
    failure_reason: str = ""
    attempted_conclusion: Optional[str] = None
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        premises_str = "\n    ".join([f'(Concept "{p}")' for p in self.premises])
        return f"""(InferenceFailureAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "rule_name")
  (List
    (InferenceFailureAtom "{self.atom_id}")
    (Concept "{self.rule_name}")))

(Evaluation
  (Predicate "premises")
  (List
    (InferenceFailureAtom "{self.atom_id}")
    (List
      {premises_str})))

(Evaluation
  (Predicate "failure_reason")
  (List
    (InferenceFailureAtom "{self.atom_id}")
    (Concept "{self.failure_reason}")))"""


@dataclass
class StallAtom(DiagnosticAtom):
    """
    Represents a performance stall or bottleneck.
    
    Attributes:
        duration_ms: Duration of the stall in milliseconds
        operation: Operation that stalled
        resource_type: Type of resource causing the stall
    """
    duration_ms: float = 0.0
    operation: str = ""
    resource_type: str = ""
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        return f"""(StallAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "duration_ms")
  (List
    (StallAtom "{self.atom_id}")
    (Number {self.duration_ms})))

(Evaluation
  (Predicate "operation")
  (List
    (StallAtom "{self.atom_id}")
    (Concept "{self.operation}")))

(Evaluation
  (Predicate "resource_type")
  (List
    (StallAtom "{self.atom_id}")
    (Concept "{self.resource_type}")))"""


@dataclass
class ResourceAtom(DiagnosticAtom):
    """
    Represents resource utilization metrics.
    
    Attributes:
        cpu_percent: CPU utilization percentage
        memory_mb: Memory usage in megabytes
        io_operations: Number of I/O operations
        threshold_exceeded: Whether a threshold was exceeded
    """
    cpu_percent: float = 0.0
    memory_mb: float = 0.0
    io_operations: int = 0
    threshold_exceeded: bool = False
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        return f"""(ResourceAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "cpu_percent")
  (List
    (ResourceAtom "{self.atom_id}")
    (Number {self.cpu_percent})))

(Evaluation
  (Predicate "memory_mb")
  (List
    (ResourceAtom "{self.atom_id}")
    (Number {self.memory_mb})))

(Evaluation
  (Predicate "threshold_exceeded")
  (List
    (ResourceAtom "{self.atom_id}")
    (Concept "{"true" if self.threshold_exceeded else "false"}")))"""


@dataclass
class AttentionAllocationFailureAtom(DiagnosticAtom):
    """
    Represents failures in ECAN attention allocation.
    
    Attributes:
        atom_handle: Handle of the atom that failed to get attention
        requested_sti: Requested STI value
        allocated_sti: Actually allocated STI value
        constraint_violated: Which constraint was violated
    """
    atom_handle: str = ""
    requested_sti: float = 0.0
    allocated_sti: float = 0.0
    constraint_violated: str = ""
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        return f"""(AttentionAllocationFailureAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "atom_handle")
  (List
    (AttentionAllocationFailureAtom "{self.atom_id}")
    (Concept "{self.atom_handle}")))

(Evaluation
  (Predicate "requested_sti")
  (List
    (AttentionAllocationFailureAtom "{self.atom_id}")
    (Number {self.requested_sti})))

(Evaluation
  (Predicate "allocated_sti")
  (List
    (AttentionAllocationFailureAtom "{self.atom_id}")
    (Number {self.allocated_sti})))

(Evaluation
  (Predicate "constraint_violated")
  (List
    (AttentionAllocationFailureAtom "{self.atom_id}")
    (Concept "{self.constraint_violated}")))"""


@dataclass
class StateAtom(DiagnosticAtom):
    """
    Represents a snapshot of system state.
    
    Attributes:
        state_data: Dictionary containing state information
        checkpoint_id: ID of associated checkpoint if any
    """
    state_data: Dict[str, Any] = field(default_factory=dict)
    checkpoint_id: Optional[str] = None
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        state_items = "\n    ".join([
            f'(Evaluation (Predicate "{k}") (List (StateAtom "{self.atom_id}") (Concept "{v}")))'
            for k, v in self.state_data.items()
        ])
        return f"""(StateAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

{state_items}"""


@dataclass
class RepairAtom(DiagnosticAtom):
    """
    Represents a repair action or suggestion.
    
    Attributes:
        action_type: Type of repair action
        parameters: Parameters for the repair action
        expected_outcome: Expected outcome of the repair
        target_diagnostic: ID of the diagnostic this repair addresses
    """
    action_type: str = ""
    parameters: Dict[str, Any] = field(default_factory=dict)
    expected_outcome: str = ""
    target_diagnostic: Optional[str] = None
    
    def to_atomese(self) -> str:
        """Convert to Atomese representation."""
        params_str = "\n    ".join([
            f'(Evaluation (Predicate "{k}") (List (RepairAtom "{self.atom_id}") (Concept "{v}")))'
            for k, v in self.parameters.items()
        ])
        return f"""(RepairAtom "{self.atom_id}"
  (stv {self.confidence} 1.0))

(Evaluation
  (Predicate "action_type")
  (List
    (RepairAtom "{self.atom_id}")
    (Concept "{self.action_type}")))

(Evaluation
  (Predicate "expected_outcome")
  (List
    (RepairAtom "{self.atom_id}")
    (Concept "{self.expected_outcome}")))

{params_str}"""


# Utility functions for creating diagnostic atoms

def create_error_diagnostic(
    error_type: str,
    message: str,
    component: ComponentType = ComponentType.UNKNOWN,
    severity: SeverityLevel = SeverityLevel.HIGH,
    stack_trace: Optional[str] = None
) -> ErrorAtom:
    """Create an error diagnostic atom."""
    atom_id = f"error_{component.value}_{datetime.utcnow().timestamp()}"
    return ErrorAtom(
        atom_id=atom_id,
        error_type=error_type,
        message=message,
        component=component,
        severity=severity,
        stack_trace=stack_trace
    )


def create_performance_diagnostic(
    operation: str,
    duration_ms: float,
    component: ComponentType = ComponentType.UNKNOWN
) -> StallAtom:
    """Create a performance stall diagnostic atom."""
    atom_id = f"stall_{component.value}_{datetime.utcnow().timestamp()}"
    severity = SeverityLevel.HIGH if duration_ms > 1000 else SeverityLevel.MEDIUM
    return StallAtom(
        atom_id=atom_id,
        operation=operation,
        duration_ms=duration_ms,
        component=component,
        severity=severity
    )


def create_attention_diagnostic(
    atom_handle: str,
    requested_sti: float,
    allocated_sti: float,
    constraint: str
) -> AttentionAllocationFailureAtom:
    """Create an attention allocation failure diagnostic atom."""
    atom_id = f"attention_failure_{datetime.utcnow().timestamp()}"
    return AttentionAllocationFailureAtom(
        atom_id=atom_id,
        atom_handle=atom_handle,
        requested_sti=requested_sti,
        allocated_sti=allocated_sti,
        constraint_violated=constraint,
        component=ComponentType.ECAN,
        severity=SeverityLevel.MEDIUM
    )
