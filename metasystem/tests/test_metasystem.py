"""
Tests for the Metasystem module

Comprehensive tests for:
- Three planes of existence
- Six cycles
- Spiral and Triad (7 steps)
- Evolutionary Helix (11 stages)
- Autopoiesis
- Integration
"""

import pytest
import sys
from pathlib import Path

# Add parent directory to path
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from metasystem.planes import (
    PlaneOfEnergy, PlaneOfInformation, PlaneOfTotality, ThreePlanes
)
from metasystem.cycles import (
    VortexCycle, RetroactionCycle, SelfReferentialCycle,
    MorphogenesisCycle, AutopoiesisCycle, AutogenesisCycle,
    SixCycles, CycleState
)
from metasystem.spiral_triad import SpiralTriad, SpiralStep
from metasystem.evolutionary_helix import EvolutionaryHelix, HelixStage, HelixPlane
from metasystem.autopoiesis import (
    AutopoieticSystem, AutopoieticComponent, AutopoieticBoundary,
    ComponentType, AutopoieticState
)
from metasystem.metasystem_core import Metasystem, MetasystemConfig, MetasystemMode


class TestPlanes:
    """Tests for the three planes of existence"""

    def test_plane_of_energy_creation(self):
        """Test creating a plane of energy"""
        plane = PlaneOfEnergy()
        assert plane.flux == 0.5
        assert plane.stability == 0.5
        assert len(plane.objects) == 0
        assert len(plane.structures) == 0

    def test_plane_of_energy_vortex(self):
        """Test vortex cycle in energy plane"""
        plane = PlaneOfEnergy()
        result = plane.vortex_cycle()
        assert 'cycling' in result
        assert result['cycling'] is True

    def test_plane_of_energy_morphogenesis(self):
        """Test morphogenesis in energy plane"""
        plane = PlaneOfEnergy()
        result = plane.morphogenesis(0.8)  # High energy should create structure
        assert result['created'] is True
        assert len(plane.structures) == 1

    def test_plane_of_information_creation(self):
        """Test creating a plane of information"""
        plane = PlaneOfInformation()
        assert plane.coherence == 0.5
        assert len(plane.relations) == 0

    def test_plane_of_information_retroaction(self):
        """Test retroaction cycle"""
        plane = PlaneOfInformation()
        result = plane.retroaction_cycle(0.5)  # At setpoint
        assert result['state'] == 'regulating'
        assert abs(result['error']) < 0.1

    def test_plane_of_totality_creation(self):
        """Test creating a plane of totality"""
        plane = PlaneOfTotality()
        assert plane.wholeness == 0.0
        assert plane.autonomy == 0.0

    def test_plane_of_totality_self_reference(self):
        """Test self-referential loop"""
        plane = PlaneOfTotality()
        plane.update_being({'key1': 'value1'})
        plane.update_image({'key1': 'value1'})
        result = plane.self_referential_loop()
        assert result['self_awareness'] > 0

    def test_three_planes_step(self):
        """Test stepping all three planes"""
        planes = ThreePlanes()
        result = planes.step(0.5)
        assert 'energy' in result
        assert 'information' in result
        assert 'totality' in result


class TestCycles:
    """Tests for the six cycles"""

    def test_vortex_cycle(self):
        """Test vortex cycle"""
        cycle = VortexCycle()
        result = cycle.step(0.5)
        assert 'output' in result
        assert 'state' in result
        assert result['cycling'] is True

    def test_retroaction_cycle(self):
        """Test retroaction cycle"""
        cycle = RetroactionCycle()
        result = cycle.step(0.5)
        assert 'error' in result
        assert 'correction' in result

    def test_self_referential_cycle(self):
        """Test self-referential cycle"""
        cycle = SelfReferentialCycle()
        result = cycle.step(0.5)
        assert 'self_awareness' in result

    def test_morphogenesis_cycle(self):
        """Test morphogenesis cycle"""
        cycle = MorphogenesisCycle()
        result = cycle.step(0.8)
        assert 'new_structure' in result

    def test_autopoiesis_cycle(self):
        """Test autopoiesis cycle"""
        cycle = AutopoiesisCycle()
        result = cycle.step(0.5)
        assert 'production' in result
        assert 'production_capacity' in result

    def test_autogenesis_cycle(self):
        """Test autogenesis cycle"""
        cycle = AutogenesisCycle()
        result = cycle.step(0.5)
        assert 'autonomy_level' in result

    def test_six_cycles_step(self):
        """Test stepping all six cycles"""
        cycles = SixCycles()
        result = cycles.step(0.5)
        assert 'horizontal' in result
        assert 'vertical' in result
        assert 'stability_index' in result
        assert 'change_index' in result


class TestSpiralTriad:
    """Tests for the Spiral and Triad (7 steps)"""

    def test_spiral_creation(self):
        """Test creating a spiral"""
        spiral = SpiralTriad()
        assert spiral.current_step == SpiralStep.UNITY
        assert spiral.energy == 1.0
        assert spiral.integration == 0.0

    def test_spiral_step_execution(self):
        """Test executing a spiral step"""
        spiral = SpiralTriad(energy=0.8)
        result = spiral.execute_current_step(0.5)
        assert 'step' in result
        assert result['step'] == SpiralStep.UNITY

    def test_spiral_advancement(self):
        """Test spiral advancement"""
        spiral = SpiralTriad(energy=0.8)
        result = spiral.advance(0.7)
        # Should advance from UNITY with enough energy
        assert 'advanced' in result

    def test_spiral_run(self):
        """Test running spiral for multiple steps"""
        spiral = SpiralTriad(energy=0.8)
        results = spiral.run_spiral(10)
        assert len(results) == 10

    def test_spiral_state(self):
        """Test getting spiral state"""
        spiral = SpiralTriad()
        state = spiral.state()
        assert 'current_step' in state
        assert 'energy' in state
        assert 'integration' in state

    def test_all_spiral_steps(self):
        """Test that all spiral steps are defined"""
        assert len(SpiralStep) == 7
        assert SpiralStep.UNITY.value == 0
        assert SpiralStep.AUTOGENESIS.value == 6


class TestEvolutionaryHelix:
    """Tests for the Evolutionary Helix (11 stages)"""

    def test_helix_creation(self):
        """Test creating a helix"""
        helix = EvolutionaryHelix()
        assert helix.current_stage == HelixStage.ENTROPIC_DRIFT
        assert helix.spiral_level == 0

    def test_helix_advancement(self):
        """Test helix advancement"""
        helix = EvolutionaryHelix(momentum=0.6)
        result = helix.advance(0.7)
        assert 'advanced' in result

    def test_helix_stage_info(self):
        """Test getting stage info"""
        helix = EvolutionaryHelix()
        info = helix.stage_info()
        assert 'name' in info
        assert 'description' in info

    def test_helix_plane_detection(self):
        """Test plane detection"""
        helix = EvolutionaryHelix()
        assert helix.current_plane() == HelixPlane.OBJECTAL

        # Move to relational plane
        helix.current_stage = HelixStage.SELF_REGULATION
        assert helix.current_plane() == HelixPlane.RELATIONAL

        # Move to existential plane
        helix.current_stage = HelixStage.SELF_PRODUCTION
        assert helix.current_plane() == HelixPlane.EXISTENTIAL

    def test_helix_run(self):
        """Test running helix for multiple steps"""
        helix = EvolutionaryHelix(momentum=0.6)
        results = helix.run_helix(20)
        assert len(results) == 20

    def test_all_helix_stages(self):
        """Test that all helix stages are defined"""
        assert len(HelixStage) == 11
        assert HelixStage.ENTROPIC_DRIFT.value == 0
        assert HelixStage.TOWARD_AUTONOMY.value == 10


class TestAutopoiesis:
    """Tests for the autopoietic system"""

    def test_component_creation(self):
        """Test creating a component"""
        comp = AutopoieticComponent(
            id="test_comp",
            component_type=ComponentType.STRUCTURAL
        )
        assert comp.integrity == 1.0
        assert comp.is_functional()

    def test_component_decay(self):
        """Test component decay"""
        comp = AutopoieticComponent(
            id="test_comp",
            component_type=ComponentType.STRUCTURAL
        )
        comp.decay(0.5)
        assert comp.integrity < 1.0

    def test_boundary_creation(self):
        """Test creating a boundary"""
        boundary = AutopoieticBoundary()
        assert boundary.permeability == 0.5
        assert boundary.integrity == 1.0

    def test_boundary_filtering(self):
        """Test boundary filtering"""
        boundary = AutopoieticBoundary()
        filtered = boundary.filter_input(1.0)
        assert filtered == 0.5  # permeability * integrity

    def test_autopoietic_system_init(self):
        """Test initializing autopoietic system"""
        system = AutopoieticSystem()
        system.initialize()
        assert len(system.components) > 0
        assert system.state == AutopoieticState.PRODUCING

    def test_autopoietic_cycle(self):
        """Test autopoietic cycle"""
        system = AutopoieticSystem()
        system.initialize()
        result = system.autopoietic_cycle(0.7)
        assert 'production' in result
        assert 'boundary' in result
        assert 'regeneration' in result

    def test_autopoietic_check(self):
        """Test autopoietic system check"""
        system = AutopoieticSystem()
        system.initialize()
        # Run several cycles to build up metrics
        for _ in range(10):
            system.autopoietic_cycle(0.8)
        # Check if it has some closure
        assert system.operational_closure >= 0


class TestMetasystem:
    """Tests for the complete metasystem"""

    def test_metasystem_creation(self):
        """Test creating a metasystem"""
        ms = Metasystem()
        assert ms.state.mode == MetasystemMode.DORMANT

    def test_metasystem_initialization(self):
        """Test initializing metasystem"""
        ms = Metasystem()
        result = ms.initialize()
        assert result['initialized'] is True
        assert ms.state.mode == MetasystemMode.OPERATING

    def test_metasystem_step(self):
        """Test metasystem step"""
        ms = Metasystem()
        ms.initialize()
        result = ms.step(0.5)
        assert 'planes' in result
        assert 'cycles' in result
        assert 'spiral' in result

    def test_metasystem_evolution(self):
        """Test metasystem evolution"""
        ms = Metasystem()
        ms.initialize()
        results = ms.evolve(10)
        assert len(results) == 10

    def test_metasystem_reflection(self):
        """Test metasystem reflection"""
        ms = Metasystem()
        ms.initialize()
        ms.evolve(5)
        reflection = ms.reflect()
        assert 'diagnosis' in reflection
        assert 'recommendations' in reflection

    def test_metasystem_viability(self):
        """Test metasystem viability check"""
        ms = Metasystem()
        ms.initialize()
        ms.evolve(20)
        # After evolution, should have some viability
        is_viable = ms.is_viable()
        assert isinstance(is_viable, bool)

    def test_metasystem_progress(self):
        """Test progress summary"""
        ms = Metasystem()
        ms.initialize()
        ms.evolve(10)
        progress = ms.progress_summary()
        assert 'coherence' in progress
        assert 'emergence' in progress
        assert 'autonomy' in progress
        assert 'spiral' in progress
        assert 'helix' in progress


class TestConfig:
    """Tests for metasystem configuration"""

    def test_default_config(self):
        """Test default configuration"""
        config = MetasystemConfig()
        assert config.initial_energy == 0.5
        assert config.enable_autopoiesis is True
        assert config.enable_evolution is True

    def test_custom_config(self):
        """Test custom configuration"""
        config = MetasystemConfig(
            initial_energy=0.8,
            enable_autopoiesis=False
        )
        ms = Metasystem(config=config)
        ms.initialize()
        state = ms.get_state()
        assert state['autopoiesis'] is None


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
