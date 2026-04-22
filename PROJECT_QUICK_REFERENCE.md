# AGI-OS Integration: Quick Reference Guide

**Sprint Duration**: December 9 - December 22, 2025 (10 working days)  
**Primary Deliverables**: `cognumach-cognitive-scheduler` and `hurdcog-occ-bridge` Debian packages

---

## Week 1 Summary

The first week focuses on **implementation and unit testing**. By the end of Week 1, all stub code should be replaced with functional implementations, and all new code should have passing unit tests.

| Day | Focus Area                     | Key Deliverable                          |
|:---:|--------------------------------|------------------------------------------|
| 1   | Planning & Specifications      | Spec documents completed                 |
| 2   | Scheduler C Implementation     | `cognitive_scheduler.c` functional       |
| 3   | Scheduler IPC Integration      | IPC hooks implemented                    |
| 4   | Bridge Python Implementation   | Python bridge script functional          |
| 5   | Unit Testing                   | All unit tests passing                   |

---

## Week 2 Summary

The second week focuses on **integration testing, packaging, and documentation**. By the end of Week 2, both packages should be built as `.deb` files and fully documented.

| Day | Focus Area                     | Key Deliverable                          |
|:---:|--------------------------------|------------------------------------------|
| 6   | Bridge Scheme Implementation   | Scheme bridge script functional          |
| 7   | Integration Test (Scheduler)   | End-to-end scheduler test passing        |
| 8   | Integration Test (Bridge)      | End-to-end bridge test passing           |
| 9   | Debian Packaging               | All `debian/` files finalized            |
| 10  | Build & Documentation          | Final `.deb` packages and API docs       |

---

## Key Milestones

1.  **End of Day 1**: Specifications approved and development begins.
2.  **End of Day 5**: All unit tests passing (Week 1 Milestone).
3.  **End of Day 8**: All integration tests passing.
4.  **End of Day 10**: Final `.deb` packages delivered (Project Complete).

---

## Critical Path

The following tasks are on the critical path and any delay will directly impact the project timeline:

1.  **Day 3**: Scheduler IPC Integration (complex, high risk).
2.  **Day 7**: Integration Test (Scheduler) (depends on all prior scheduler work).
3.  **Day 9**: Debian Packaging (depends on all code being complete).

---

## Risk Mitigation

*   **Mach IPC Complexity**: Allocate extra time on Day 3. Refer to existing Cognumach examples.
*   **Integration Test Failures**: Use detailed logging. Isolate issues with unit tests.
*   **Build Environment Issues**: Rebuild the development container if necessary.

---

## Team Roles

*   **Senior Systems Engineer**: C implementation and kernel integration.
*   **Software Engineer**: Python and Scheme bridge implementation.
*   **QA Engineer**: All testing (unit and integration).

---

## Success Criteria

*   ✅ All features implemented as per specifications.
*   ✅ All unit and integration tests passing.
*   ✅ Both `.deb` packages build and install successfully.
*   ✅ Complete API documentation.
*   ✅ Code passes linting and quality checks.

---

## Contact and Communication

*   **Daily Stand-ups**: 9:00 AM (15 minutes).
*   **Communication Channel**: Slack `#agi-os-integration`.
*   **Status Reports**: Weekly (Mondays).

---

## Document References

*   **Full Project Plan**: `AGI_OS_INTEGRATION_PROJECT_PLAN.md`
*   **Scheduler Spec**: `COGNITIVE_SCHEDULER_SPEC.md`
*   **Bridge Spec**: `HURDCOG_BRIDGE_SPEC.md`
*   **Kanban Board**: `PROJECT_KANBAN.md`

---

**Last Updated**: December 9, 2025
