# 04 Test Specification — STM32F407 Register-Level Access Control System

> **What this file is:** the list of all tests, what each one checks, and what result is expected.
> V-model: unit tests check the **design** ([design.md](03-design.md)), integration tests check the **architecture**
> ([architecture.md](02-architecture.md)), system tests check the **requirements** ([requirements.md](01-requirements.md)).

---

# 1. Overview

```text
requirements.md   ←→   System tests        (ST-xx)   whole product, board or Renode
architecture.md   ←→   Integration tests   (IT-xx)   modules together
design.md         ←→   Unit tests          (UT-xx)   one module, GoogleTest on the host
```

## Status key

| Mark | Meaning |
|---|---|
| **E** | Exists (host test). Name in the "Test name" column. |
| **P** | Proposed, new test to write. |
| **B** | Unity board test source exists, never recorded as run on the board. |
| **M** | Manual test on the board, not run yet. |
| **C** | Characterization: measures real behavior, result becomes a new requirement. No pass or fail yet. |

## Test environments

| Env | Setup |
|---|---|
| **Host** | CMake, GoogleTest v1.15.2 via FetchContent, `make tests` / `ctest`. Executables: `unit_tests` (App + KeypadAdapter), `driver_tests` (`keypad.c` with hardware fakes), `platform_tests` (EXTI/NVIC helpers). |
| **Board** | STM32F407G-DISC1, 4×4 keypad wired as in `keypad_connection.md`, ST-LINK, OpenOCD, GDB, SWO log (`make openocd`, `make swo`, `tail -f swo.log`). Stopwatch. |
| **Renode** | Renode 1.17.0 + Robot Framework, simulated key presses (currently on hold). |

## Entry and exit criteria

| Level | Start when | Done when |
|---|---|---|
| Unit | module compiles on the host | all UT cases pass, every REQ in the trace has a test |
| Integration | unit tests green | all IT host cases pass, board IT cases run once |
| System | firmware flashed on the board | all ST cases run, results noted, characterization results turned into requirements or accepted limitations |

---

# 2. Unit tests (GoogleTest, host)

## 2.1 `AccessController` (`unit_tests`, fixture `AccessControllerTest`, design §1.1, §2.1, §2.2, §4)

Setup: `MockClock`, `MockLedOutput`, `MockKeypad`, real `PinValidator` and `AttemptCounter`.

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-AC-01 | | Start up | state Locked; LED mock got red on, green off | REQ-F-001, 002 | P |
| UT-AC-02 | CorrectPinGrantsAccess | Enter `1 2 3 4 #` (default PIN) | state Unlocked; green on, red off | REQ-F-003, 006, 012, 022 | E |
| UT-AC-03 | PinBufferIsClearedAfterAttempt, ThreeWrongPinsLockOut | Enter wrong four-digit PIN + `#` | stays Locked (red on, green off) | REQ-F-007, 013, 014 | E |
| UT-AC-04 | ShortPinIsNotAccepted, ShortPinDoesNotCountAsAttempt | Enter `1 2 #` | discarded, no unlock, no failure counted | REQ-F-007, 008, 011 | E |
| UT-AC-05 | LongPinIsRejected, OverflowDoesNotCountAsAttempt | Enter five digits + `#` | input cleared; no failure counted; state unchanged | REQ-F-009 | E |
| UT-AC-06 | InvalidKeyBeforePinIsHarmless, InvalidKeyInMiddleAbortsInput, InvalidKeyAtEndAbortsInput, StarAbortsInput, InvalidKeyDoesNotCountAsAttempt, ValidPinWorksAfterAbortedInput | Invalid key (`a`, `*`) at start, middle or end of the input | input aborted; no failure counted; next valid PIN works | REQ-F-010 | E |
| UT-AC-07 | PinBufferIsClearedAfterAttempt | After submit (right or wrong PIN) | buffer cleared, next PIN starts empty | REQ-F-011 | E |
| UT-AC-08 | ThreeWrongPinsLockOut, LockoutIgnoresInput | Three wrong PINs | LockedOut; red blink requested, green off | REQ-F-015, 017 | E |
| UT-AC-09 | LockoutIgnoresInput, StaysLockedOutBeforeTimeout | Keys during LockedOut, including the correct PIN | all ignored; state stays LockedOut | REQ-F-016 | E |
| UT-AC-10 | StaysLockedOutBeforeTimeout, LockoutEndsAfterTimeout | Lockout boundary with `MockClock` | at 29,999 ms still LockedOut; at 30,000 ms Locked | REQ-F-018, 019 | E |
| UT-AC-11 | AttemptsResetAfterTimeout, KeysDuringLockoutAreNotBuffered, CorrectPinWorksAfterTimeout | Lockout expires | attempt counter 0, buffer empty, correct PIN unlocks | REQ-F-019, 020 | E |
| UT-AC-12 | SecondLockoutRestartsTimer | Second lockout after the first | new period starts at the third failure of the second round | REQ-F-021 | E |
| UT-AC-13 | LockoutTimeoutSurvivesCounterWraparound | Lockout started near `0xFFFFFFFF`, clock wraps | still LockedOut after 512 ms; ends after 30 s | REQ-F-018 | E |
| UT-AC-14 | SetNewPinAfterUnlocked | Unlocked: enter new PIN `5 6 7 8 #` | PIN stored; state Locked; red on | REQ-F-023, 024 | E |
| UT-AC-15 | UseNewPinAfterUnlocked, OldPinRejectedAfterChange | After PIN change | new PIN unlocks; old PIN is rejected | REQ-F-024 | E |
| UT-AC-16 | ChangePinRejectsShortPin | Unlocked: `5 6 #` | input cleared; state stays Unlocked; green stays on | REQ-F-025 | E |
| UT-AC-17 | ChangePinRejectsInvalidKeyInMiddle, ChangePinWorksAfterAbortedInput | Unlocked: invalid key mid-input | input cleared; state stays Unlocked; next valid PIN change works | REQ-F-026 | E |
| UT-AC-18 | ChangePinRejectsOverflow | Unlocked: fifth digit | input cleared; state stays Unlocked | REQ-F-009, 026 | E |
| UT-AC-19 | | Each digit `0`–`9` | accepted as PIN character (loop over all ten) | REQ-F-005 | P |
| UT-AC-20 | ChangePinWithHashOnlyKeepsState | Unlocked: `#` with empty buffer | discarded; state stays Unlocked | REQ-F-025 | E |
| UT-AC-21 | UnlockResetsAttemptsCounter | Two failures, correct PIN, PIN change, two failures, third failure | no lockout after the two failures; lockout at the third failure | REQ-F-027, 014, 015 | E |
| UT-AC-22 | | PIN with leading zero `0 1 2 3 #` set via PIN change | accepted and validated correctly | REQ-F-005, 024 | P |
| UT-AC-23 | | Locked: `#` with empty buffer | discarded; no failure counted | REQ-F-008 | P |

## 2.2 `PinValidator` and SHA-256 (`unit_tests`, fixture `PinValidatorTest`, design §1.2)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-PV-01 | AcceptsCorrectPin | Default PIN `1234` | accepted | REQ-F-003 | E |
| UT-PV-02 | RejectsWrongPin | Wrong four-digit PIN (`0000`) | rejected | REQ-F-013 | E |
| UT-PV-03 | SetsNewPin | `SetPin("5678")` | `5678` accepted, `1234` rejected | REQ-F-004, 024 | E |
| UT-PV-04 | | `0000` and `0123` set as PIN | work like any other PIN | REQ-F-005 | P |
| UT-PV-05 | | SHA-256 known-answer: `"abc"` | `ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad` | design (hash correctness) | P |
| UT-PV-06 | | SHA-256 known-answer: empty input | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` | design (hash correctness) | P |
| UT-PV-07 | RejectsTooLongPin | `Validate("12345")` | rejected | REQ-F-007 | E |
| UT-PV-08 | RejectsTooShortPin | `Validate("123")` | rejected | REQ-F-007 | E |
| UT-PV-09 | RejectsNonDigitPin | `Validate("12a4")` | rejected | REQ-F-005, 007 | E |
| UT-PV-10 | RejectsInvalidNewPin | `SetPin("12a4")` | ignored; `1234` still valid | REQ-F-024 | E |

## 2.3 `AttemptCounter` and `LockState` (`unit_tests`, fixture `AttemptCounterTest`, design §1.3)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-AT-01 | InitiallyNotLockedOut | New counter | limit not reached | REQ-F-014 | E |
| UT-AT-02 | NotLockedOutAfterTwoFailures, LockedOutAfterThreeFailures | Register failures one by one | limit reached exactly at the third | REQ-F-014, 015 | E |
| UT-AT-03 | ResetClearsLockout | `Reset()` after three failures | limit no longer reached | REQ-F-020 | E |
| UT-LS-01 | | `LockState` values | three distinct states, `uint8_t` base | design | P |

## 2.4 `KeypadAdapter` (`unit_tests` with `keypad_stub.c`, fixture `KeypadAdapterTest`, design §1.5)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-KA-01 | ScanForwardsReadEvent | Stub returns a key event (`5`) | `Scan` returns the same character | design | E |
| UT-KA-02 | ScanForwardsReadEvent | Stub reports no event | `Scan` returns `'\0'` | design | E |
| UT-KA-03 | ConstructorInitializesBeforeEnablingInterrupt | Create the adapter | `Keypad_Init` runs before `Keypad_EnableInterrupt` | design | E |

## 2.5 `LedOutputAdapter` (needs a GPIO fake, design §1.5)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-LA-01 | | Init | PD12 and PD14 configured as outputs | CON-008 | P |
| UT-LA-02 | | Show Locked | red on, green off | REQ-F-002 | P |
| UT-LA-03 | | Show Unlocked | green on, red off | REQ-F-022 | P |
| UT-LA-04 | | Lockout blink, call `Update()` many times | red toggles, green stays off | REQ-F-017 | P |
| UT-LA-05 | | Setting one LED | does not change the other pin or other port D pins | design | P |

## 2.6 `SysTickClock` (needs a register stub, design §1.5)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-SC-01 | | `Init()` writes the SysTick registers | `SYST_RVR = 15999`, `SYST_CVR = 0`, `SYST_CSR = 0x7` | CON-009, 010 | P |
| UT-SC-02 | | Call `SysTick_Handler` N times | `NowMs()` increases by N | CON-010 | P |
| UT-SC-03 | | Counter at `0xFFFFFFFF`, one more tick | wraps to 0 without error | CON-010 | P |

## 2.7 Keypad driver (`driver_tests`, fixture `KeypadDriverTest`, design §1.6, §2.3, §3)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-KD-01 | ReadEventWithoutIrqReturnsNothing | `Keypad_ReadEvent()` without pending flag | returns "none", no scan | design | E |
| UT-KD-02 | ReadEventReturnsPressedKey | Fake press at six sample positions (`1`, `A`, `6`, `8`, `*`, `D`) | correct key per the key map | CON-003, REQ-F-005 | E |
| UT-KD-03 | | Fake key held | no event until the fake reports release | CON-011 | P |
| UT-KD-04 | ReadEventRestoresStateAfterScan | Watch EXTI mask during a scan | lines 8–11 masked during GPIO reads, unmasked afterwards | CON-012 | E |
| UT-KD-05 | ReadEventRestoresStateAfterScan | After a scan | rows back LOW (idle), `EXTI->PR` cleared, event consumed | design §3.3 | E |
| UT-KD-06 | Exti9_5HandlerSetsFlagAndClearsLines | Call `EXTI9_5_IRQHandler` | `PR` bits cleared, flag set | design §3.1 | E |
| UT-KD-07 | Exti15_10HandlerSetsFlagAndClearsLines | Call `EXTI15_10_IRQHandler` | `PR` bits cleared, flag set | design §3.1 | E |
| UT-KD-08 | Exti9_5HandlerSetsFlagAndClearsLines | `Keypad_IrqPending()` after an ISR | returns 1 once, then 0 (the call consumes the flag) | design §3.1 | E |
| UT-KD-09 | | Two columns LOW at once | first LOW column wins (documents current behavior) | requirements §5 | P |
| UT-KD-10 | ReadEventWithoutKeyStillRestoresState | Pending flag set but no column LOW (bounce) | returns "none", idle restored, EXTI unmasked | design §3.3 | E |
| UT-KD-11 | | Two ISRs before one `ReadEvent()` | one event returned (documents coalescing) | requirements §5 | P |
| UT-KD-12 | NoPendingBitDoesNotSetFlag | Both handlers called with no `PR` bit set | flag stays clear | design §3.1 | E |

## 2.8 Platform helpers (`platform_tests`, fixture `ExtiTest`, design §1.10, §3.2)

| ID | Test name | Test case | Expected | Verifies | Status |
|---|---|---|---|---|---|
| UT-PL-01 | MapPortUsesCorrectFieldForLines8To11, MapPortSetsFieldInExticr3, SetFallingEdgeSetsFtsrAndClearsRtsr, EnableUnmasksLine | EXTI setup for lines 8–11 | `EXTICR3` selects port D (other fields untouched), `FTSR` set, `RTSR` cleared, `IMR` set | CON-006 | E |
| UT-PL-02 | NvicEnableIrq23SetsIser0Bit23, NvicEnableIrq40SetsIser1Bit8 | NVIC enable for IRQ 23 and 40 | `ISER0` bit 23 and `ISER1` bit 8 set, nothing else | CON-007 | E |
| UT-PL-03 | MapPortEnablesSyscfgClock | `EXTI_MapPort` | `SYSCFGEN` set in `RCC->APB2ENR` | design §3.2 | E |
| UT-PL-04 | | `GPIO_WritePin`, `GPIO_TogglePin` | change only the requested pin | design §1.7 | P |

---

# 3. Integration tests

| ID | Test name | Modules | Test case | Expected | Env | Verifies | Status |
|---|---|---|---|---|---|---|---|
| IT-01 | | `AccessController` + `PinValidator` + `AttemptCounter` + mocks | Scenario: unlock → change PIN → lock → three failures → wait 30 s → new PIN unlocks | full sequence passes, LED requests in the right order | Host | REQ-F-012–024 | P |
| IT-02 | | `KeypadAdapter` + keypad driver (fakes) + controller | Fake presses `1 2 3 4 #` through the whole chain | controller reaches Unlocked | Host | REQ-F-005, 006, 012 | P |
| IT-03 | | `LedOutputAdapter` + GPIO driver | State changes with a register fake | `ODR` bits PD12 / PD14 as expected for Locked, Unlocked, blink | Host | REQ-F-002, 017, 022 | P |
| IT-04 | `KeypadDriverTest.ReadEventRestoresStateAfterScan` | ISR + flag + `Keypad_ReadEvent()` | Trigger the ISR, then read the event | key returned, `PR` cleared, flag consumed | Host | CON-011, 012 | E |
| IT-05 | | `SysTickClock` + controller | Call `SysTick_Handler` 30,000 times | lockout ends at the 30,000th tick | Host | REQ-F-018, CON-010 | P |
| IT-06 | `TestExtiInterrupts` | EXTI + NVIC + keypad ISR on the chip | Unity: set `EXTI->SWIER` for lines 8 and 10 | handler runs, flag set, `PR` cleared | Board | CON-006, 007 | B |
| IT-07 | `TestKeypadConfiguration` | GPIO + SYSCFG + EXTI + NVIC | Unity: read back registers after `Keypad_Init()` and `Keypad_EnableInterrupt()` | GPIOD clock on, rows output, pull-ups on PD8–11, SYSCFG clock on, `EXTICR3` port D, `FTSR`, `IMR`, `ISER0` bit 23, `ISER1` bit 8 | Board | CON-004–007 | B |
| IT-08 | | Fault handler + SWO | Temporary write to `0xFFFFFFF0` | crash dump in `swo.log`, `PC` at the store, `CFSR` shows precise bus error | Board | design §1.9 | M |
| IT-09 | `AccessControllerTest.NoStaleKeyAfterLockout` | Keypad + `AccessController` during LockedOut | Press keys while LockedOut, wait for expiry, then enter a PIN | first PIN after expiry works, no stale key from the lockout | Host | REQ-F-016, 020 | E |

---

# 4. System tests

Run on the board unless noted. "Pass" is the expected column. Rows marked **C** have no pass or fail until
a requirement exists.

| ID | Test case (steps) | Expected | Verifies | Env | Status |
|---|---|---|---|---|---|
| ST-01 | Power on or reset | red LED on, green off | REQ-F-001, 002 | Board, Renode | M |
| ST-02 | Enter `1 2 3 4 #` | green on, red off | REQ-F-003, 012, 022 | Board, Renode | M |
| ST-03 | Enter `1 1 1 1 #` | stays locked, red on | REQ-F-013 | Board, Renode | M |
| ST-04 | Enter `1 2 #` three times, then `1 2 3 4 #` | stays locked after each short PIN; then unlocks (short PINs are not counted as failures) | REQ-F-008, 014 | Board | M |
| ST-05 | Enter `1 2 A`, then `1 2 3 4 5` | each input discarded, no lockout after repeating this five times | REQ-F-009, 010 | Board | M |
| ST-06 | Three wrong PINs, then press keys | red blinks, green off, keys ignored (try the correct PIN) | REQ-F-015, 016, 017 | Board, Renode | M |
| ST-07 | Third wrong PIN, start stopwatch, wait for blinking to stop | lockout ends at 30 s (tolerance ±1 s) | REQ-F-018, 019, CON-010 | Board | M |
| ST-08 | After lockout, enter the correct PIN; then repeat three failures again | unlocks; second lockout again needs three failures | REQ-F-019, 020, 021 | Board | M |
| ST-09 | Unlock, enter `5 6 7 8 #`, then use the new PIN | locks after the change; `5678` unlocks; `1234` fails | REQ-F-023, 024 | Board, Renode | M |
| ST-10 | Change PIN, press reset, enter `1234` | `1234` works again, `5678` does not | REQ-F-004 | Board | M |
| ST-11 | Unlocked: enter `1 2 #`, and an invalid key mid-input | input cleared, green stays on | REQ-F-025, 026 | Board | M |
| ST-12 | Hold a key for 10 s | exactly one key event after release; note LED behavior meanwhile | CON-011 | Board | C |
| ST-13 | Press keys as fast as possible, 20 digits | count lost or merged events | requirements §5 | Board | C |
| ST-14 | Press two or three keys at once | record which key wins, check for ghost keys | requirements §5 | Board | C |
| ST-15 | Logic analyzer on a column during a press | measure bounce duration; basis for a debounce requirement | requirements §5 (delay not specified) | Board | C |
| ST-16 | In lockout, measure the blink period; repeat while holding a key | record period; does blinking stop while a key is held? | requirements §5 (blocking loop), REQ-F-017 | Board | C |
| ST-17 | Probe or toggle each signal | rows PD0–PD3, columns PD8–PD11, green PD12, red PD14 as specified | CON-003, 004, 005, 008 | Board | M |
| ST-18 | End-to-end: unlock → change PIN → lock → three failures → wait → new PIN | whole workflow passes | REQ-F-012–024 | Board | M |
| ST-19 | Press keys during lockout, wait for expiry, then enter a PIN | PIN works on the first try (see IT-09) | REQ-F-016, 020 | Board | M |
| ST-20 | Enter `9 9 9 9 #` twice, `1 2 3 4 #`, then `5 6 7 8 #` (PIN change), then `1 1 1 1 #` twice | no lockout after the two failures (counter was reset by the unlock); third failure starts the lockout | REQ-F-027 | Board | M |

Procedure details for ST-07 and ST-12–16: note date, firmware version (git commit) and measured values.

---

# 5. Static and inspection checks

| ID | Check | Expected | Verifies | Status |
|---|---|---|---|---|
| INS-01 | Search the build for HAL or CubeMX files and includes (`grep -r stm32f4xx_hal`) | none | CON-002 | P |
| INS-02 | Review `SysTickClock` constants against 16 MHz | reload 15,999; `kCoreClockHz` equals 16,000,000 | CON-009, 010 | P |
| INS-03 | `make tidy`, GCC warnings (`-Wall -Wextra -Wconversion -Wshadow`) | no findings | quality | E |

---

# 6. Missing tests and decisions

| Item | Why | Proposed action |
|---|---|---|
| Startup state (UT-AC-01) | no test checks the LEDs or state right after construction | find out whether `main()` or the controller sets the red LED (REQ-F-002), then add the test |
| Keypad map: 6 of 16 positions tested | `ReadEventReturnsPressedKey` has six cases | add the other ten cases to the array |
| Key held (UT-KD-03) | release behavior (CON-011) not tested on the host | add a test with a fake that reports release late |
| LED adapter and SysTick clock have no host tests | hardware access without fakes | add GPIO and SysTick register fakes (UT-LA, UT-SC) |
| Keypad blocks in LockedOut | `Keypad_Scan()` waits for key release (CON-011), so holding a key during lockout may stop the blinking | check on the board in ST-16 |
| Board tests never run | Unity sources exist | run IT-06, IT-07 once, note the result |
| Renode tests | on hold | revive ST-01, 02, 03, 06, 09 as Robot tests after the GPIO issue is solved |
| Characterization (ST-12–16) | requirements §5 | turn the results into new requirements |

## Decided

| Item | Decision |
|---|---|
| Attempt counter after a successful unlock | reset to 0 (REQ-F-027, UT-AC-21) |
| Stale key after lockout | `LockedOut` reads and discards keys (IT-09, review F-05); board check ST-19 open |
| ST-07 tolerance | ±1 s |
| REQ-F-025, 026 coverage | covered by `ChangePinRejectsShortPin` (UT-AC-16) and `ChangePinRejectsInvalidKeyInMiddle` (UT-AC-17) |