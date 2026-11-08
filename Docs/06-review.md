# 06 Review — STM32F407 Register-Level Access Control System

> **What this file is:** the check that the documents agree with each other and with the code, plus a list
> of findings to fix. In the V-model this is the closing review: are requirements, architecture, design and
> tests complete, consistent and traceable?
> **Status:** v0.2. Document review plus a comparison of the host test sources with `04-test-specification.md`.
> The firmware source was checked only where findings F-03, F-05 and F-06 say so.

---

# 1. Scope

| Item | Document | Version |
|---|---|---|
| Requirements | `requirements.md` | 0.2 |
| Architecture | `architecture.md` | 0.4 |
| Detailed design | `design.md` | 0.1 |
| Test specification | `04-test-specification.md` | 0.2 |
| Traceability | `05-traceability.md` | 0.2 |
| Hardware docs | `hardware.md`, `keypad_connection.md`, `register_map.md`, `startup.md`, `linker-script.md`, `swo.md` | current |

Method: read each document against the others; check that every ID is used consistently; look for
statements that contradict each other; list what is marked "confirm" or "open".

---

# 2. Checklist

| Check | Result | Note |
|---|---|---|
| Every requirement has a design element | ✔ | `05-traceability.md` §1–2 |
| Every requirement has at least one planned test | ✔ | planned, not all written |
| Every test traces to a requirement or has a stated reason | ✔ | `05-traceability.md` §3 |
| Requirements are testable (measurable) | ✘ | blink frequency, debounce time missing (F-04) |
| Requirements are consistent with each other | ✔ | counter after unlock decided (F-03) |
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
| F-02 | Medium | Requirements say `Keypad_EnableInterrupt()` configures EXTI and NVIC; `keypad_connection.md` says `Keypad_Init()` does it. | requirements §4, keypad doc | check the source, align the names | Open |
| F-03 | Medium | No requirement said what happens to the failed-attempt counter after a successful unlock. Two failures, then a success: does the next failure count as the first or the third? | requirements §3.3 | Decided: reset to 0. REQ-F-027 added. `ValidatePin()` already called `attempts_.Reset()`. Test `AccessControllerTest.UnlockResetsAttemptsCounter` (UT-AC-21) passes. | Resolved |
| F-04 | Medium | Debounce time and blink frequency are not specified. | requirements §5 | measure (ST-15, ST-16), then add requirements | Open |
| F-05 | High | In LockedOut the keypad was not scanned, so a key pressed during the lockout stayed pending. After the lockout ended, this stale key reached the controller, so the first PIN had a fifth digit and was aborted. Confirmed on the host: `NoStaleKeyAfterLockout` failed before the fix. | design §1.1, §3.3 | Fixed: `AccessController::Update()` reads and discards the keypad in LockedOut. Test `AccessControllerTest.NoStaleKeyAfterLockout` (IT-09) passes. Board check ST-19 still open. | Resolved (host), board check open |
| F-06 | Medium | REQ-F-025 and REQ-F-026 had no confirmed explicit tests. | requirements §3.4 | Checked against the test file: `ChangePinRejectsShortPin` and `ChangePinWithHashOnlyKeepsState` (REQ-F-025), `ChangePinRejectsInvalidKeyInMiddle` (REQ-F-026). | Resolved |
| F-07 | High | No hardware test evidence for any requirement. Unity board tests exist as source but are not recorded as run. | requirements §6 | run IT-06, IT-07, ST-01…ST-11 and ST-17…ST-20, note the results | Open |
| F-08 | Medium | The keypad release loop blocks the main loop: no LED blinking and no lockout check while a key is held. Since the fix for F-05, `LockedOut` also calls `Keypad_Scan()`, so this now applies during lockout, too. | design §6, architecture sharp edges | decide: accept as a limit (write it into requirements) or redesign (non-blocking release); check in ST-16 | Open |
| F-09 | Medium | Renode tests on hold (GPIO ODR behavior with input pins unresolved). | `docs/debugging/2026-11-01-renode-gpio-odr.md` | keep ST-01, 02, 03, 06, 09 as Robot candidates; revisit later | Open |
| F-10 | Low | `ITM->TPR` should be `1`, not `0`. Works today only because `main()` runs privileged without an RTOS. | `swo.md` | set it to `1` | Open |
| F-11 | Low | The clock value 16,000,000 exists in three places: `swoInit()`, `scripts/swo.gdb`, `kCoreClockHz` in `SysTickClock.cpp`. | `swo.md`, `register_map.md` | one shared constant, or a comment at each place | Open |
| F-12 | Low | `SysTickClock` accesses SysTick registers directly instead of through a driver. | architecture | accept or add a timer driver | Open |
| F-13 | Medium | Interface names and signatures (`IKeypad`, `ILedOutput`, `AttemptCounter`), call order in `main()`, ISR flag handling order are marked `[confirm]`. | design | check the source, remove the marks | Open |
| F-14 | Medium | Names of the existing GoogleTest cases were not in any document. | `04-test-specification.md` | filled in the "Test name" column from the test files | Resolved |
| F-15 | Low | `hardware.md` shows LEDs and keypad pins but not SWO (PB3). | `hardware.md` | add one row | Open |
| F-16 | Low | Requirements ID scheme has gaps by design (REQ-F, CON). Tests use UT, IT, ST, INS. | all | keep, mention in the README | Open |
| F-17 | Medium | No test checks the state and LEDs right after startup (UT-AC-01). It is unclear whether `main()` or the controller sets the red LED. | `04-test-specification.md` §2.1, REQ-F-001, 002 | check the source, add the test | Open |
| F-18 | Low | `ReadEventReturnsPressedKey` covers 6 of 16 key positions. | `04-test-specification.md` §2.7 | add the other ten cases to the array | Open |

---

# 4. What is in good shape

- Layering and dependency direction are described the same way in `architecture.md`, `design.md`,
  `register_map.md` and `startup.md`.
- Interrupt handling (vectors 23 and 40, flag only in the ISR, scan in the main loop) is consistent across
  `keypad_connection.md`, `startup.md`, `register_map.md` and the new documents.
- Lockout logic (30 s, wraparound-safe subtraction, counter reset on expiry) matches in requirements, design and tests.
- Known limits are written down, not hidden.
- Controller tests cover all functional requirements except the startup state (F-17).

---

# 5. Next steps

| Order | Step | Closes |
|---|---|---|
| 1 | Add the startup test and the missing keypad map cases | F-17, F-18 |
| 2 | Write the new host tests marked **P** (start with UT-AC-19, 22, 23, UT-PV-04 to 06, IT-01, IT-02) | |
| 3 | Fix the doc mismatches | F-01, F-02, F-13, F-15 |
| 4 | Run the board tests IT-06, IT-07, then ST-01…ST-11 and ST-17…ST-20 | F-07, F-05 (board check) |
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