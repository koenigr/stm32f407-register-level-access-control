# 05 Traceability — STM32F407 Register-Level Access Control System

> **What this file is:** one table that links every requirement to the design that implements it and to the
> tests that check it. If a requirement has no test, or a test checks no requirement, it shows up here.
> Documents: `requirements.md` (REQ, CON) → `architecture.md` → `design.md` → `04-test-specification.md` (UT, IT, ST, INS).
> **Status:** v0.2. Test IDs from `04-test-specification.md`. "Design" refers to `design.md` sections.

## Status key

| Mark | Meaning |
|---|---|
| **Host** | at least one host test exists (name in `04-test-specification.md`) |
| **Host+** | needs new host tests (proposed) |
| **HW open** | no hardware evidence yet |
| **HW src** | Unity board test source exists, not run |
| **Insp** | by inspection |

"(indirect)" in the System column: the system test exercises the requirement, but it is not listed in the "Verifies" column of `04-test-specification.md`.

---

# 1. Functional requirements

| Requirement | Design | Unit | Integration | System | Status |
|---|---|---|---|---|---|
| REQ-F-001 start in Locked | §1.1, §4.1 | UT-AC-01 | n/a | ST-01 | Host+ (no explicit startup test), HW open |
| REQ-F-002 red on, green off at start | §1.1, §1.5 | UT-AC-01, UT-LA-02 | IT-03 | ST-01 | Host+, HW open |
| REQ-F-003 default PIN `1234` | §1.2 | UT-PV-01, UT-PV-05, UT-PV-06, UT-AC-02 | n/a | ST-02 | Host, HW open |
| REQ-F-004 changed PIN lost on reset | §1.2 | UT-PV-03 | n/a | ST-10 | Host, HW open |
| REQ-F-005 digits 0–9 accepted | §2.1 | UT-AC-19, UT-AC-22, UT-PV-04, UT-PV-09, UT-KD-02 | IT-02 | ST-02 (indirect) | Host, Host+, HW open |
| REQ-F-006 `#` submits | §2.1 | UT-AC-02 | IT-02 | ST-02 | Host, HW open |
| REQ-F-007 validate only with four digits | §2.1 | UT-AC-03, UT-AC-04, UT-PV-07, UT-PV-08, UT-PV-09 | n/a | ST-03, ST-04 (indirect) | Host, HW open |
| REQ-F-008 short PIN discarded, not counted | §2.1 | UT-AC-04, UT-AC-23 | n/a | ST-04 | Host, Host+, HW open |
| REQ-F-009 fifth digit clears input | §2.1 | UT-AC-05, UT-AC-18 | n/a | ST-05 | Host, HW open |
| REQ-F-010 other key clears input | §2.1 | UT-AC-06 | n/a | ST-05 | Host, HW open |
| REQ-F-011 buffer cleared after submit or invalid | §2.1 | UT-AC-04, UT-AC-07 | n/a | ST-04, ST-05 (indirect) | Host, HW open |
| REQ-F-012 correct PIN → Unlocked | §2.1, §4.2 | UT-AC-02 | IT-01, IT-02 | ST-02, ST-18 | Host, HW open |
| REQ-F-013 wrong PIN stays Locked | §2.1, §4.2 | UT-AC-03, UT-PV-02 | IT-01 | ST-03, ST-18 (indirect) | Host, HW open |
| REQ-F-014 count only complete rejected PINs | §1.3 | UT-AC-03, UT-AC-04, UT-AC-21, UT-AT-01, UT-AT-02 | IT-01 | ST-04 | Host, HW open |
| REQ-F-015 LockedOut after three failures | §1.3, §4.2 | UT-AC-08, UT-AC-21, UT-AT-02 | IT-01 | ST-06 | Host, HW open |
| REQ-F-016 keypad ignored in LockedOut | §1.1, §4.2 | UT-AC-09 | IT-01, IT-09 | ST-06, ST-19 | Host, HW open |
| REQ-F-017 red blinks, green off in LockedOut | §1.5 | UT-AC-08, UT-LA-04 | IT-01, IT-03 | ST-06, ST-16 | Host, Host+, HW open |
| REQ-F-018 lockout lasts 30 s | §2.2, §6 | UT-AC-10, UT-AC-13 | IT-01, IT-05 | ST-07 | Host, HW open |
| REQ-F-019 back to Locked after 30 s | §2.2, §4.2 | UT-AC-10, UT-AC-11 | IT-01 | ST-07, ST-08 | Host, HW open |
| REQ-F-020 expiry resets counter and buffer | §2.2 | UT-AC-11, UT-AT-03 | IT-01, IT-09 | ST-08, ST-19 | Host, HW open |
| REQ-F-021 new lockout from third failure | §2.2 | UT-AC-12 | IT-01 | ST-08 | Host, HW open |
| REQ-F-022 green on, red off when Unlocked | §1.5, §4.1 | UT-AC-02, UT-LA-03 | IT-01, IT-03 | ST-02 | Host, Host+, HW open |
| REQ-F-023 PIN entry while Unlocked | §2.1 | UT-AC-14 | IT-01 | ST-09, ST-18 | Host, HW open |
| REQ-F-024 new PIN stored, back to Locked | §1.2, §2.1 | UT-AC-14, UT-AC-15, UT-AC-22, UT-PV-03, UT-PV-10 | IT-01 | ST-09, ST-18 | Host, Host+, HW open |
| REQ-F-025 `#` with short input in Unlocked | §2.1, §4.2 | UT-AC-16, UT-AC-20 | n/a | ST-11 | Host, HW open |
| REQ-F-026 invalid input in Unlocked | §2.1, §4.2 | UT-AC-17, UT-AC-18 | n/a | ST-11 | Host, HW open |
| REQ-F-027 counter reset after successful unlock | §1.1, §1.3 | UT-AC-21 | n/a | ST-20 | Host, HW open |

# 2. Constraints

| Constraint | Design | Unit | Integration | System / Inspection | Status |
|---|---|---|---|---|---|
| CON-001 STM32F407VG on DISC1 | architecture | n/a | n/a | all ST on the board | HW open |
| CON-002 no HAL, register level | §1.10 | n/a | n/a | INS-01 | Insp (P) |
| CON-003 4×4 matrix keypad | §1.6 | UT-KD-02 | n/a | ST-17 | Host, HW open |
| CON-004 rows PD0–PD3 outputs | §1.6 | n/a | IT-07 | ST-17 | HW src |
| CON-005 columns PD8–PD11 inputs, pull-up | §1.6 | n/a | IT-07 | ST-17 | HW src |
| CON-006 EXTI8–11, port D, falling edge | §3.2 | UT-PL-01 | IT-06, IT-07 | n/a | Host, HW src |
| CON-007 IRQ 23 and 40 | §3.1, §3.2 | UT-PL-02 | IT-06, IT-07 | n/a | Host, HW src |
| CON-008 green PD12, red PD14 | §1.7 | UT-LA-01 | n/a | ST-17 | Host+, HW open |
| CON-009 16 MHz HSI, no PLL | §6 | UT-SC-01 | n/a | INS-02, ST-07 (indirect) | Host+, Insp |
| CON-010 SysTick as ms time base | §1.5 | UT-SC-01, UT-SC-02, UT-SC-03 | IT-05 | ST-07, INS-02 | Host+, HW open |
| CON-011 key event only after release | §1.6, §2.3 | UT-KD-03 | IT-04 | ST-12 | Host+, HW open |
| CON-012 EXTI masked during scan | §1.6, §3.3 | UT-KD-04 | IT-04 | ST-12 (indirect) | Host, HW open |

---

# 3. Reverse trace: tests without a direct requirement

These tests check design or quality, not a numbered requirement. That is fine, but each should have a reason.

| Test | Reason |
|---|---|
| UT-PV-05, UT-PV-06 | SHA-256 correctness behind REQ-F-003, 004 |
| UT-LS-01 | enum definition |
| UT-KA-01, 02, 03 | adapter forwards keys, init order |
| UT-KD-01, 05, 06, 07, 08, 10, 12 | keypad driver internals (design §3) |
| UT-KD-09, 11 | documents current limits (requirements §5) |
| UT-LA-05 | LED adapter side effects |
| UT-PL-03, 04 | init order, GPIO helper |
| IT-08 | fault handler and SWO |
| ST-13, 14, 15 | characterization, may become new requirements |
| INS-03 | code quality |

# 4. Coverage summary

| Item | Count |
|---|---|
| Functional requirements | 27, each has at least one unit and one system test planned |
| Constraints | 12, each has at least one test or inspection planned (CON-001 only implicitly by all board tests) |
| Requirements with no hardware evidence | all 39 |
| Requirements needing new host tests | REQ-F-001, 002, 005, 008, 017, 022, 024, CON-008, 009, 010, 011 |
| Requirements without an integration test | REQ-F-001, 003, 004, 007–011, 025, 026, 027, CON-001, 003, 008, 009 (covered by unit and system tests) |

# 5. Gaps found while tracing

| Gap | Where | Action |
|---|---|---|
| Blink frequency, debounce time not specified | requirements §5 | make ST-15, ST-16 results into new requirements |
| Key held during LockedOut blocks the main loop | requirements §5 (CON-011), design §1.1 | check whether blinking stops, in ST-16 |
| No explicit startup test | UT-AC-01 | find out whether `main()` or the controller sets the red LED, then add the test |
| Keypad map: 6 of 16 positions tested | UT-KD-02 | add the other ten cases |
| REQ-F-004 depends on a reset, no host test can show it | UT-PV-03 shows only RAM storage | rely on ST-10 |

## Resolved

| Gap | Resolution |
|---|---|
| Counter behavior after a successful unlock | REQ-F-027, UT-AC-21 |
| Stale key after lockout | `LockedOut` discards keys, IT-09, review F-05 (board check ST-19 open) |
| REQ-F-025, 026 test coverage | UT-AC-16, UT-AC-17 |
| Existing test names unknown | filled in `04-test-specification.md` |