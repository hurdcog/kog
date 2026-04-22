"""
Tests for Enterprise Autogenesis Module

Tests the enterprise autogenesis model including:
- Six stages of value creation
- Learning loops
- Thin slice delivery
- Autonomous pods
- Metasystem integration
"""

import pytest
from typing import Dict, Any

from metasystem.enterprise_autogenesis import (
    EnterpriseAutogenesis,
    EnterpriseStage,
    ThinSlice,
    LearningLoop,
    AutonomousPod,
    VisionGoalBet,
    LoopType,
    ControlMode,
    EnterpriseStageState,
)


class TestVisionGoalBet:
    """Tests for the Vision-Goal-Bet structure"""

    def test_create_vision(self):
        """Test creating a vision"""
        vgb = VisionGoalBet(vision="Build AGI platform")
        assert vgb.vision == "Build AGI platform"
        assert len(vgb.goals) == 0
        assert len(vgb.bets) == 0

    def test_add_goals(self):
        """Test adding goals to vision"""
        vgb = VisionGoalBet(vision="Build AGI platform")
        vgb.add_goal("Integrate components")
        vgb.add_goal("Enable synergy")
        assert len(vgb.goals) == 2
        assert "Integrate components" in vgb.goals

    def test_add_bets(self):
        """Test adding bets to goals"""
        vgb = VisionGoalBet(vision="Build AGI platform")
        vgb.add_goal("Integrate components")
        vgb.add_bet(0, "Components will synergize", 0.7)
        assert len(vgb.bets) == 1
        assert vgb.bets[0]['confidence'] == 0.7
        assert vgb.bets[0]['validated'] is False

    def test_validate_bet(self):
        """Test validating a bet"""
        vgb = VisionGoalBet(vision="Build AGI platform")
        vgb.add_goal("Integrate components")
        vgb.add_bet(0, "Components will synergize", 0.7)
        vgb.validate_bet(0, True)
        assert vgb.bets[0]['validated'] is True
        assert vgb.bets[0]['outcome'] is True


class TestThinSlice:
    """Tests for thin slice delivery"""

    def test_create_thin_slice(self):
        """Test creating a thin slice"""
        slice = ThinSlice(id="TS-001", description="AtomSpace integration")
        assert slice.id == "TS-001"
        assert slice.description == "AtomSpace integration"
        assert len(slice.stages_completed) == 0
        assert slice.value_delivered == 0.0

    def test_complete_stages(self):
        """Test completing stages in a thin slice"""
        slice = ThinSlice(id="TS-001", description="AtomSpace integration")
        slice.complete_stage(EnterpriseStage.EXECUTIVE_VISION)
        slice.complete_stage(EnterpriseStage.PRODUCT_STRATEGY)
        assert len(slice.stages_completed) == 2
        assert EnterpriseStage.EXECUTIVE_VISION in slice.stages_completed

    def test_is_end_to_end(self):
        """Test checking if slice is end-to-end"""
        slice = ThinSlice(id="TS-001", description="AtomSpace integration")
        assert not slice.is_end_to_end()

        # Complete all stages
        for stage in EnterpriseStage:
            slice.complete_stage(stage)

        assert slice.is_end_to_end()

    def test_measure_outcome(self):
        """Test measuring outcomes"""
        slice = ThinSlice(id="TS-001", description="AtomSpace integration")
        slice.measure_outcome("velocity", 0.8)
        slice.measure_outcome("quality", 0.9)
        assert len(slice.outcomes_measured) == 2
        assert slice.value_delivered == 0.85


class TestLearningLoop:
    """Tests for learning loops"""

    def test_create_learning_loop(self):
        """Test creating a learning loop"""
        loop = LearningLoop(name="test", loop_type=LoopType.AGILE_DEVELOPMENT)
        assert loop.name == "test"
        assert loop.phase == "idea"
        assert loop.iterations == 0

    def test_loop_cycle(self):
        """Test full cycle of learning loop"""
        loop = LearningLoop(name="test", loop_type=LoopType.AGILE_DEVELOPMENT)

        # Idea phase
        result = loop.step({'hypothesis': 'test_hypothesis'})
        assert result['phase'] == 'idea'
        assert loop.phase == 'test'

        # Test phase
        result = loop.step({'test_result': 0.7})
        assert result['phase'] == 'test'
        assert loop.phase == 'learn'

        # Learn phase
        result = loop.step({'insight': 'learning_insight', 'confidence': 0.8})
        assert result['phase'] == 'learn'
        assert len(loop.learnings) == 1
        assert loop.phase == 'outcome'

        # Outcome phase
        result = loop.step({'outcome_value': 0.9})
        assert result['phase'] == 'outcome'
        assert result['cycle_complete'] is True
        assert loop.phase == 'idea'  # Reset


class TestAutonomousPod:
    """Tests for autonomous pods"""

    def test_create_pod(self):
        """Test creating an autonomous pod"""
        pod = AutonomousPod(id="POD-001", name="Core Team")
        assert pod.id == "POD-001"
        assert pod.name == "Core Team"
        assert pod.velocity == 0.5
        assert pod.autonomy_level == 0.5

    def test_assign_slice(self):
        """Test assigning thin slice to pod"""
        pod = AutonomousPod(id="POD-001", name="Core Team")
        slice = ThinSlice(id="TS-001", description="Test slice")
        pod.assign_slice(slice)
        assert len(pod.active_slices) == 1

    def test_deliver_increment(self):
        """Test pod delivering increment"""
        pod = AutonomousPod(id="POD-001", name="Core Team")
        slice = ThinSlice(id="TS-001", description="Test slice")
        pod.assign_slice(slice)

        result = pod.deliver_increment()
        assert result['delivered'] is True
        assert result['stage_complete'] == EnterpriseStage.EXECUTIVE_VISION.name
        assert len(slice.stages_completed) == 1

    def test_deliver_no_slices(self):
        """Test pod with no active slices"""
        pod = AutonomousPod(id="POD-001", name="Core Team")
        result = pod.deliver_increment()
        assert result['delivered'] is False
        assert result['reason'] == 'no_active_slices'


class TestEnterpriseAutogenesis:
    """Tests for the main enterprise autogenesis system"""

    def test_create_enterprise(self):
        """Test creating enterprise autogenesis system"""
        enterprise = EnterpriseAutogenesis()
        assert enterprise.control_mode == ControlMode.LEARNING_LOOPS
        assert len(enterprise.stages) == 6
        assert len(enterprise.loops) == 9
        assert enterprise.cycle_count == 0

    def test_initialize(self):
        """Test initializing enterprise with vision"""
        enterprise = EnterpriseAutogenesis()
        result = enterprise.initialize(
            vision="Build cognitive platform",
            goals=["Integrate components", "Enable synergy"]
        )
        assert result['initialized'] is True
        assert result['vision'] == "Build cognitive platform"
        assert result['goals'] == 2

    def test_create_thin_slice(self):
        """Test creating thin slice in enterprise"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize("Test vision")
        slice = enterprise.create_thin_slice("TS-001", "Test slice")
        assert len(enterprise.thin_slices) == 1
        assert slice.id == "TS-001"

    def test_create_pod(self):
        """Test creating pod in enterprise"""
        enterprise = EnterpriseAutogenesis()
        pod = enterprise.create_pod("POD-001", "Core Team", ["atomspace"])
        assert len(enterprise.pods) == 1
        assert pod.capabilities == ["atomspace"]

    def test_step(self):
        """Test single step execution"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize("Test vision", goals=["Goal 1"])
        result = enterprise.step(0.5)
        assert result['cycle'] == 1
        assert 'stages' in result
        assert 'loops' in result
        assert 'metrics' in result

    def test_evolve(self):
        """Test evolution over multiple steps"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize("Test vision", goals=["Goal 1"])
        results = enterprise.evolve(10)
        assert len(results) == 10
        assert enterprise.cycle_count == 10

    def test_metrics_update(self):
        """Test metrics update correctly"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize(
            "Test vision",
            goals=["Goal 1", "Goal 2", "Goal 3"]
        )
        enterprise.create_pod("POD-001", "Team")
        enterprise.evolve(20)

        # Coherence should be positive with vision and goals
        assert enterprise.coherence > 0

    def test_reflect(self):
        """Test self-reflection"""
        enterprise = EnterpriseAutogenesis()
        reflection = enterprise.reflect()
        assert 'diagnosis' in reflection
        assert 'recommendations' in reflection
        assert 'metrics' in reflection
        assert 'health' in reflection

    def test_reflect_with_issues(self):
        """Test reflection identifies issues"""
        enterprise = EnterpriseAutogenesis()
        # Don't initialize - should identify issues
        reflection = enterprise.reflect()
        assert any("vision" in d.lower() for d in reflection['diagnosis'])

    def test_get_state(self):
        """Test getting enterprise state"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize("Test vision")
        state = enterprise.get_state()
        assert 'cycle_count' in state
        assert 'control_mode' in state
        assert 'vision' in state
        assert 'stages' in state
        assert 'delivery' in state
        assert 'metrics' in state

    def test_metasystem_mapping(self):
        """Test metasystem mapping"""
        enterprise = EnterpriseAutogenesis()
        mapping = enterprise.to_metasystem_mapping()
        assert 'planes' in mapping
        assert 'cycles' in mapping
        assert 'totality' in mapping['planes']
        assert 'information' in mapping['planes']
        assert 'energy' in mapping['planes']
        assert 'autopoiesis' in mapping['cycles']
        assert 'autogenesis' in mapping['cycles']


class TestEnterpriseStageState:
    """Tests for enterprise stage state"""

    def test_create_stage_state(self):
        """Test creating stage state"""
        stage = EnterpriseStageState(stage=EnterpriseStage.EXECUTIVE_VISION)
        assert stage.stage == EnterpriseStage.EXECUTIVE_VISION
        assert stage.active is False
        assert stage.throughput == 0.0

    def test_add_to_backlog(self):
        """Test adding to backlog"""
        stage = EnterpriseStageState(stage=EnterpriseStage.EXECUTIVE_VISION)
        stage.add_to_backlog({'item': 'test'})
        assert len(stage.backlog) == 1

    def test_process(self):
        """Test processing stage"""
        stage = EnterpriseStageState(stage=EnterpriseStage.EXECUTIVE_VISION)
        stage.add_to_backlog({'item': 'test1'})
        stage.add_to_backlog({'item': 'test2'})
        result = stage.process(1.0)
        assert result['processed'] > 0
        assert len(stage.completed) > 0


class TestEnterpriseIntegration:
    """Integration tests for enterprise autogenesis"""

    def test_full_workflow(self):
        """Test complete enterprise workflow"""
        # Create and initialize
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize(
            vision="Create cognitive synergy platform",
            goals=[
                "Integrate OpenCog components",
                "Enable reproducible development",
                "Achieve cognitive autonomy"
            ]
        )

        # Create thin slices
        slice1 = enterprise.create_thin_slice("TS-001", "AtomSpace integration")
        slice2 = enterprise.create_thin_slice("TS-002", "Metasystem implementation")

        # Create pods and assign work
        pod1 = enterprise.create_pod("POD-001", "Core Team", ["atomspace"])
        pod1.assign_slice(slice1)

        pod2 = enterprise.create_pod("POD-002", "Integration Team", ["synergy"])
        pod2.assign_slice(slice2)

        # Evolve
        results = enterprise.evolve(30)
        assert len(results) == 30

        # Check state
        state = enterprise.get_state()
        assert state['cycle_count'] == 30
        assert state['delivery']['pods'] == 2
        assert state['delivery']['thin_slices'] == 2

        # Reflect
        reflection = enterprise.reflect()
        assert 'metrics' in reflection
        assert reflection['health']['pods_count'] == 2

    def test_learning_loop_accumulation(self):
        """Test that learning loops accumulate learnings"""
        enterprise = EnterpriseAutogenesis()
        enterprise.initialize("Test")
        enterprise.evolve(40)  # Run enough cycles for learnings

        total_learnings = sum(
            len(loop.learnings) for loop in enterprise.loops.values()
        )
        # Each loop cycles through 4 phases, learning happens in phase 3
        # After 40 cycles, each loop should have ~10 learnings
        assert total_learnings > 0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
