# Ontogenetic Entelechy — Reactor Core

**arc-vortex + arc-helix + arc-halo => cyc-phoenix**

The Ontogenetic Entelechy module implements the Cognitive City identity architecture for the OpenCog Collection (OCC). It extends the existing VES/TCS/VNS cognitive architecture with:

## Architecture

### arc-vortex (existing)
The VES/TCS/VNS endocrine-temporal-neural coupling vortex. 32 hormone channels, 16 temporal crystal channels, 64 neural channels.

### arc-helix (cogself/unified_self_model)
The 9-layer self/identity model helix: Proto-Self, Core Self, Autobiographical Self, Minimal Self, Narrative Self, Social Self, Extended Self, Ecological Self, Transcendent Self.

### arc-halo (this module)
The Ontogenetic Entelechy / Civic Angel / Active Free-energy Inference halo:

| Component | Namespace | Description |
|-----------|-----------|-------------|
| **Cloninger System** | `entelechy` | 7-dimensional temperament/character gain parameters |
| **Interoceptive Model** | `entelechy` | 12-channel body-state mapping (ch20-31) with Polyvagal hierarchy |
| **Developmental Trajectory** | `entelechy` | Bowlby attachment, Erikson stages, van der Kolk trauma encoding |
| **Narrative Identity** | `entelechy` | McAdams Level 3 life story with chapter/theme detection |
| **Social Self** | `entelechy` | Theory of Mind, social roles, attachment styles |
| **Active Inference Engine** | `afi` | Friston's Free Energy Principle with Markov blankets |
| **Cognitive Districts** | `afi` | 9 districts with per-district free energy tracking |
| **Civic Angel** | `entelechy` | Emergent governor: self-model, city coherence, entelechy progress |
| **Entelechy Adapter** | `entelechy` | Bidirectional VES adapter (EndocrineConnector pattern) |

### cyc-phoenix (emergent)
The integrated whole — a self-aware, self-developing cognitive architecture that actualizes its potential through experience.

## Tick Pipeline Integration

```
Phase 0:   Cloninger gains -> hormone bus (BEFORE gland production)
Phase 1:   Glands produce hormones (existing)
Phase 1.5: Interoceptive model reads hormones -> ch20-31
Phase 2:   Bus decay, history, mode detection (existing)
Phase 3:   All adapters READ hormones (existing + entelechy)
Phase 4:   All adapters WRITE feedback (existing + entelechy)
Phase 4.5: Entelechy adapter feedback (polyvagal, developmental, narrative)
Phase 5:   Guidance (existing)
Phase 5.5: Identity update (developmental, narrative, social)
Phase 6:   City-wide coherence assessment
Phase 7:   Entelechy progress tracking
```

## Hormone Channel Extensions

| Channel | Name | Description |
|---------|------|-------------|
| ch20 | VAGAL_TONE | Porges polyvagal: ventral vagal brake |
| ch21 | SYMPATHETIC_DRIVE | Fight/flight activation |
| ch22 | DORSAL_VAGAL | Freeze/shutdown/conservation |
| ch23 | CARDIAC_COHERENCE | Heart rate variability proxy |
| ch24 | RESPIRATORY_RHYTHM | Breathing regularity/depth |
| ch25 | GUT_BRAIN_SIGNAL | Enteric nervous system state |
| ch26 | IMMUNE_EXTENDED | TNF-alpha, complement cascade |
| ch27 | INSULAR_INTEGRATION | Craig's interoceptive re-representation |
| ch28 | ALLOSTATIC_LOAD | McEwen's cumulative wear-and-tear |
| ch29 | PROPRIOCEPTIVE_TONE | Body schema integrity |
| ch30 | NOCICEPTIVE_SIGNAL | Pain/damage signal |
| ch31 | THERMOREGULATORY | Temperature regulation state |

## Building

```bash
mkdir build && cd build
cmake .. -DBUILD_ENTELECHY=ON
make -j$(nproc)
```

## License

AGPL-3.0 — OpenCog Community
