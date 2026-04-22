"""
Unit tests for diagnostic atoms module.

Author: OCC Evolution Team
Date: 2025-12-12
"""

import pytest
from datetime import datetime
from diagnostic_atoms.schemas.diagnostic_atoms import (
    DiagnosticAtom,
    ErrorAtom,
    CriticalErrorAtom,
    PredictionMismatchAtom,
    InferenceFailureAtom,
    StallAtom,
    ResourceAtom,
    AttentionAllocationFailureAtom,
    StateAtom,
    RepairAtom,
    SeverityLevel,
    ComponentType,
    create_error_diagnostic,
    create_performance_diagnostic,
    create_attention_diagnostic
)


class TestDiagnosticAtom:
    """Test base DiagnosticAtom class."""
    
    def test_creation(self):
        """Test basic diagnostic atom creation."""
        atom = DiagnosticAtom(
            atom_id="test_001",
            component=ComponentType.ATOMSPACE,
            severity=SeverityLevel.INFO
        )
        assert atom.atom_id == "test_001"
        assert atom.component == ComponentType.ATOMSPACE
        assert atom.severity == SeverityLevel.INFO
        assert atom.confidence == 1.0
        
    def test_to_dict(self):
        """Test dictionary conversion."""
        atom = DiagnosticAtom(
            atom_id="test_002",
            component=ComponentType.PLN,
            severity=SeverityLevel.HIGH
        )
        d = atom.to_dict()
        assert d["atom_id"] == "test_002"
        assert d["component"] == "pln"
        assert d["severity"] == "high"
        assert "timestamp" in d


class TestErrorAtom:
    """Test ErrorAtom class."""
    
    def test_creation(self):
        """Test error atom creation."""
        error = ErrorAtom(
            atom_id="error_001",
            error_type="RuntimeError",
            message="Test error message",
            component=ComponentType.PLN,
            severity=SeverityLevel.HIGH
        )
        assert error.error_type == "RuntimeError"
        assert error.message == "Test error message"
        
    def test_to_atomese(self):
        """Test Atomese conversion."""
        error = ErrorAtom(
            atom_id="error_002",
            error_type="ValueError",
            message="Invalid value"
        )
        atomese = error.to_atomese()
        assert "ErrorAtom" in atomese
        assert "error_002" in atomese
        assert "ValueError" in atomese
        
    def test_factory_function(self):
        """Test error creation factory function."""
        error = create_error_diagnostic(
            error_type="TypeError",
            message="Type mismatch",
            component=ComponentType.ECAN,
            severity=SeverityLevel.CRITICAL
        )
        assert error.error_type == "TypeError"
        assert error.component == ComponentType.ECAN
        assert error.severity == SeverityLevel.CRITICAL


class TestCriticalErrorAtom:
    """Test CriticalErrorAtom class."""
    
    def test_severity_auto_set(self):
        """Test that severity is automatically set to CRITICAL."""
        error = CriticalErrorAtom(
            atom_id="critical_001",
            error_type="SystemError",
            message="Critical failure"
        )
        assert error.severity == SeverityLevel.CRITICAL


class TestPredictionMismatchAtom:
    """Test PredictionMismatchAtom class."""
    
    def test_creation(self):
        """Test prediction mismatch atom creation."""
        mismatch = PredictionMismatchAtom(
            atom_id="mismatch_001",
            predicted_value=0.8,
            actual_value=0.3,
            prediction_confidence=0.9,
            mismatch_magnitude=0.5
        )
        assert mismatch.predicted_value == 0.8
        assert mismatch.actual_value == 0.3
        assert mismatch.mismatch_magnitude == 0.5
        
    def test_to_atomese(self):
        """Test Atomese conversion."""
        mismatch = PredictionMismatchAtom(
            atom_id="mismatch_002",
            predicted_value="success",
            actual_value="failure"
        )
        atomese = mismatch.to_atomese()
        assert "PredictionMismatchAtom" in atomese
        assert "predicted_value" in atomese
        assert "actual_value" in atomese


class TestStallAtom:
    """Test StallAtom class."""
    
    def test_creation(self):
        """Test stall atom creation."""
        stall = StallAtom(
            atom_id="stall_001",
            duration_ms=1500.0,
            operation="backward_chaining",
            resource_type="cpu"
        )
        assert stall.duration_ms == 1500.0
        assert stall.operation == "backward_chaining"
        
    def test_factory_function(self):
        """Test performance diagnostic factory function."""
        stall = create_performance_diagnostic(
            operation="inference",
            duration_ms=2000.0,
            component=ComponentType.PLN
        )
        assert stall.operation == "inference"
        assert stall.duration_ms == 2000.0
        assert stall.severity == SeverityLevel.HIGH  # > 1000ms


class TestAttentionAllocationFailureAtom:
    """Test AttentionAllocationFailureAtom class."""
    
    def test_creation(self):
        """Test attention failure atom creation."""
        failure = AttentionAllocationFailureAtom(
            atom_id="attention_001",
            atom_handle="atom_12345",
            requested_sti=100.0,
            allocated_sti=50.0,
            constraint_violated="budget_exceeded"
        )
        assert failure.atom_handle == "atom_12345"
        assert failure.requested_sti == 100.0
        assert failure.allocated_sti == 50.0
        
    def test_factory_function(self):
        """Test attention diagnostic factory function."""
        failure = create_attention_diagnostic(
            atom_handle="atom_67890",
            requested_sti=200.0,
            allocated_sti=100.0,
            constraint="threshold_violation"
        )
        assert failure.component == ComponentType.ECAN
        assert failure.constraint_violated == "threshold_violation"


class TestRepairAtom:
    """Test RepairAtom class."""
    
    def test_creation(self):
        """Test repair atom creation."""
        repair = RepairAtom(
            atom_id="repair_001",
            action_type="increase_attention_budget",
            parameters={"amount": 100, "target": "inference"},
            expected_outcome="reduced_stalls",
            target_diagnostic="stall_007"
        )
        assert repair.action_type == "increase_attention_budget"
        assert repair.parameters["amount"] == 100
        assert repair.target_diagnostic == "stall_007"


class TestSerialization:
    """Test serialization capabilities."""
    
    def test_dict_serialization(self):
        """Test dictionary serialization."""
        error = ErrorAtom(
            atom_id="error_test",
            error_type="TestError",
            message="Test message"
        )
        d = error.to_dict()
        assert isinstance(d, dict)
        assert "atom_id" in d
        assert "atom_type" in d
        assert d["atom_type"] == "ErrorAtom"
        
    def test_atomese_serialization(self):
        """Test Atomese serialization."""
        stall = StallAtom(
            atom_id="stall_test",
            duration_ms=500.0,
            operation="test_op"
        )
        atomese = stall.to_atomese()
        assert isinstance(atomese, str)
        assert "StallAtom" in atomese
        assert "stall_test" in atomese


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
