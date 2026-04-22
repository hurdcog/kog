# Contributing

This project is organized into four phases, with execution posture and review depth changing by phase. See `.cursorrules` for the full posture.

## New to GNU Mach Development?

👋 **Welcome!** If you're new to kernel development or this project:
- Read the [New Developer Guide](docs/new-developer-guide.md) for step-by-step onboarding
- Consider joining our [Mentorship Program](docs/mentorship-program.md) for guided support
- Start with issues labeled `good first issue` and `phase:1`

## Quick start
- Build (WSL/Linux recommended):
  - `./scripts/dev-setup-wsl.sh`
  - `autoreconf --install && ./configure --host=i686-gnu && make -j$(nproc)`
- Run tests:
  - `make check`
  - Single test: `make run-hello`
  - Debug: `make debug-hello` then attach `gdb gnumach -ex 'target remote :1234'`
- Static analysis:
  - `./scripts/run-static-analysis.sh`

## Phases and expectations
- Phase 1 (Foundation & Quick Wins)
  - Scope: housekeeping, well-known fixes; no significant feature changes
  - Edits: minimal, targeted, reversible; keep PRs small
  - Definition of done: zero new warnings, tests pass, CI green
- Phase 2 (Improvements to existing subsystems)
  - Scope: targeted perf/robustness improvements using known methods
  - Provide brief rationale; verify with tests/benchmarks
- Phase 3 (New features & architecture changes)
  - Scope: new subsystem features and supportive structures
  - Provide short design options and chosen approach; cohesive edits
- Phase 4 (Experimental R&D)
  - Scope: prototypes, research, iteration; capture hypotheses and outcomes

## Issue templates
Use the appropriate template under `.github/ISSUE_TEMPLATE` (phase 1–4). Each template asks for:
- Context/goal
- Phase-appropriate approach
- Files/functions to touch
- Verify steps and DoD

## Coding standards
- Match existing formatting and style
- Descriptive names; guard clauses; handle errors early; avoid deep nesting
- Prefer explicit types in public APIs; avoid unsafe casts; respect strict aliasing (Linux glue may opt out)

## Build, test, CI
- Primary build: autotools+make; QEMU-based tests via `make check`
- CI builds and tests on pushes/PRs; a `-Werror` trial job exists and may be tightened

Thank you for contributing!

