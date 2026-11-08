# 06 Review — STM32F407 Register-Level Access Control System

> **What this file is:** the check that the documents agree with each other and with the code, plus a list
> of findings to fix. In the V-model this is the closing review: are requirements, architecture, design and
> tests complete, consistent and traceable?
> **Status:** draft v0.1. This is a document review done from the texts only. Nothing was compared with the
> source code yet. Please confirm or reject each finding. Reviewer and sign-off are left blank for you.

---

# 1. Scope

| Item | Document | Version |
|---|---|---|
| Requirements | `requirements.md` | 0.1 |
| Architecture | `architecture.md` | 0.4 |
| Detailed design | `design.md` | 0.1 |
| Test specification | `04-test-specification.md` | 0.1 |
| Traceability | `05-traceability.md` | 0.1 |
| Hardware docs | `hardware.md`, `keypad_connection.md`, `register_map.md`, `startup.md`, `linker-script.md`, `swo.md` | current |

Method: read each document against the others; check that every ID is used consistently; look for
statements that contradict each other; list what is marked "confirm" or "open".

---

# 2. Checklist

| Check | Result | Note |
|---|---|---|
| Every requirement has a design element | ✔ | `05-traceability.md` §1–3 |
| Every requirement has at least one planned test | ✔ | planned, not all written |
| Every test traces to a requirement or has a stated reason | ✔ | `05-traceability.md` §4 |
| Requirements are testable (measurable) | ✘ | blink frequency, debounce time missing (F-04) |
| Requirements are consistent with each other | ✘ | counter after unlock unspecified (F-03) |
| Documents agree with each other | ✘ | LED table, function names (F-01, F-02) |
| Open items are visible, not hidden | ✔ | `[confirm]` marks and "open" statuses |
| Hardware verification evidence exists | ✘ | none recorded (F-07) |
| Design limits are documented | ✔ | architecture "sharp edges", design §8 |

---

# 3. Findings

Severity: **High** can make the system behave wrongly, **Medium** documentation or test gap, **Low** cosmetic.

| ID | Severity | Finding | Where | Proposed action | Status |
|---|---|---|---|---|---|
| F-01 | Medium | `hardware.md` says green = "Valid PIN", red = "Invalid PIN". Requirements say red = Locked, blinking red = LockedOut. | `hardware.md` LED table | change the table to Locked / Unlocked / LockedOut | Open |
| F-02 | Medium | Requirements say `Keypad_EnableInterrupt()` configures EXTI and NVIC; `keypad_connection.md` says `Keypad_Init()` does it. | requirements §5, keypad doc | check the source, align the names | Open |
| F-03 | Medium | No requirement says what happens to the failed-attempt counter after a successful unlock. Two failures, then a success: does the next failure count as the first or the third? | requirements §3.3 | decide the rule, add a requirement, add test UT-AC-21 | Resolved |
| F-04 | Medium | Debounce time and blink frequency are not specified. | requirements §4 | measure (ST-15, ST-16), then add NF requirements | Open |
| F-05 | High | In LockedOut the keypad was not scanned, so a key pressed during the lockout stayed pending. After the lockout ended, this stale key reached the controller, so the first PIN had a fifth digit and was aborted. Confirmed on the host: `NoStaleKeyAfterLockout` failed before the fix. | design §1.1, §3.3 | Fixed: `AccessController::Update()` reads and discards the keypad in LockedOut. Test `AccessControllerTest.NoStaleKeyAfterLockout` (IT-09) passes. Board check ST-19 still open. | Resolved (host), board check open |
| F-06 | Medium | REQ-F-025 and REQ-F-026 have no confirmed explicit tests. | requirements §3.4 | check UT-AC-16, 17 against the test file | Open |
| F-07 | High | No hardware test evidence for any requirement. Unity board tests exist as source but are not recorded as run. | requirements §7 | run IT-06, IT-07, ST-01…ST-19, fill the results log | Open |
| F-08 | Medium | The keypad release loop blocks the main loop: no LED blinking and no lockout check while a key is held. | design §6, architecture sharp edges | decide: accept as a limit (write it into requirements) or redesign (non-blocking release) | Open |
| F-09 | Medium | Renode tests on hold (GPIO ODR behavior with input pins unresolved). | `docs/debugging/2026-11-01-renode-gpio-odr.md` | keep ST-01, 02, 03, 06, 09 as Robot candidates; revisit later | Open |
| F-10 | Low | `ITM->TPR` should be `1`, not `0`. Works today only because `main()` runs privileged without an RTOS. | `swo.md` | set it to `1` | Open |
| F-11 | Low | The clock value 16,000,000 exists in three places: `swoInit()`, `scripts/swo.gdb`, `kCoreClockHz` in `SysTickClock.cpp`. | `swo.md`, `register_map.md` | one shared constant, or a comment at each place | Open |
| F-12 | Low | `SysTickClock` accesses SysTick registers directly instead of through a driver. | architecture | accept or add a timer driver | Open |
| F-13 | Medium | Interface names and signatures (`IKeypad`, `ILedOutput`, `AttemptCounter`), call order in `main()`, ISR flag handling order are marked `[confirm]`. | design | check the source, remove the marks | Open |
| F-14 | Medium | Names of the existing GoogleTest cases are not in any document. | `04-test-specification.md` | fill the "Code name" column from `--gtest_list_tests` | Open |
| F-15 | Low | `hardware.md` shows LEDs and keypad pins but not SWO (PB3). | `hardware.md` | add one row | Open |
| F-16 | Low | Requirements ID scheme has gaps by design (REQ-F, REQ-NF, CON). Tests use UT, IT, ST, INS. | all | keep, mention in the README | Open |

---

# 4. What is in good shape

- Layering and dependency direction are described the same way in `architecture.md`, `design.md`,
  `register_map.md` and `startup.md`.
- Interrupt handling (vectors 23 and 40, flag only in the ISR, scan in the main loop) is consistent across
  `keypad_connection.md`, `startup.md`, `register_map.md` and the new documents.
- Lockout logic (30 s, wraparound-safe subtraction, counter reset on expiry) matches in requirements and design.
- Known limits are written down, not hidden.

---

# 5. Next steps

| Order | Step | Closes |
|---|---|---|
| 1 | Fill code names for existing tests, mark which proposed tests already exist | F-14, F-06 |
| 2 | Write the new host tests marked **P** (start with UT-AC-15, 18, 20 and IT-09) | F-05, F-03 |
| 3 | Fix the doc mismatches | F-01, F-02, F-13, F-15 |
| 4 | Run the board tests IT-06, IT-07, then ST-01…ST-11 and ST-17…ST-19 | F-07 |
| 5 | Run the characterization tests ST-12…ST-16, turn results into requirements | F-04, F-08 |
| 6 | Decide on the open design questions in `design.md` §8 | F-08, F-12 |

---

# 6. Sign-off

| Role | Name | Date | Decision |
|---|---|---|---|
| Author | | | |
| Reviewer | | | |

Exit criteria for this review: all High findings closed or accepted with a reason, all Medium findings
closed or scheduled, traceability table has no empty cell.
