# Requirements --- STM32F407 Register-Level Access Control System

> **Document status:** Initial baseline, reverse-engineered from the
> current source code and test sources.\
> **Purpose:** Describe the system's existing, externally observable
> behavior and constraints.\
> **Important:** This is a retrospective specification, not evidence
> that every requirement has been verified on physical hardware.

## 1. Scope

The system is a bare-metal access-control demonstration for the
STM32F407G-DISC1 board. A user enters a four-digit numeric PIN using a
4×4 matrix keypad. The system indicates its access state using green and
red LEDs.

This document covers: - PIN entry and validation - Unlocking and PIN
change - Failed attempts and lockout - Keypad input and LED indication -
Timing behavior relevant to lockout

Low-level implementation details are recorded as design constraints
where they help define the current system. They are not all user-facing
requirements.

## 2. Requirement status and verification terminology

Each requirement has an implementation status and a verification status.

| Term | Meaning |
|---|---|
| **Implemented** | The current source code contains behavior corresponding to the requirement. |
| **Host-tested** | Existing host-side tests exercise the behavior. |
| **Board-test source** | A Unity test exists that is intended to exercise the target hardware. It has not necessarily been run successfully on a physical board. |
| **Open** | The behavior needs additional testing, clarification or physical verification. |

## 3. Functional requirements

### 3.1 Startup and default state

| ID | Requirement | Implementation / verification |
|---|---|---|
| REQ-F-001 | After startup, the access controller shall enter the **Locked** state. | Implemented; host-tested through initial-state/default-PIN scenarios. |
| REQ-F-002 | After startup, the system shall indicate the Locked state by turning on the red LED and turning off the green LED. | Implemented in `main()` and LED adapter; controller LED commands are host-tested. Physical output open. |
| REQ-F-003 | After startup, the system shall accept the default PIN `1234`. | Implemented through the PIN hash; host-tested. |
| REQ-F-004 | A PIN changed during operation shall remain changed only until the next reset or power cycle. | Implemented: the changed PIN hash is held in RAM; reset behavior on physical hardware open. |

### 3.2 PIN entry and validation

| ID | Requirement | Implementation / verification |
|---|---|---|
| REQ-F-005 | The system shall accept numeric digits `0`–`9` as PIN-entry characters. | Implemented; host-tested. |
| REQ-F-006 | The system shall use `#` to submit a PIN. | Implemented; host-tested. |
| REQ-F-007 | The system shall validate a submitted PIN only if exactly four digits have been entered. | Implemented; host-tested for short and valid-length input. |
| REQ-F-008 | A submitted PIN containing fewer than four digits shall be discarded without incrementing the failed-attempt counter. | Implemented; host-tested. |
| REQ-F-009 | If a fifth digit is entered before submission, the current PIN input shall be cleared and discarded. | Implemented: `AppendDigit()` rejects the fifth digit and `ProcessKey()` clears the buffer; host-tested. |
| REQ-F-010 | If a key other than a numeric digit or `#` is entered during PIN entry, the current PIN input shall be cleared and discarded. | Implemented; host-tested. |
| REQ-F-011 | After PIN submission or invalid input, the input buffer shall be cleared. | Implemented; host-tested. |
| REQ-F-012 | If the submitted four-digit PIN is correct, the system shall transition from Locked to Unlocked. | Implemented; host-tested. |
| REQ-F-013 | If the submitted four-digit PIN is incorrect, the system shall remain Locked unless the failed-attempt threshold is reached. | Implemented; host-tested. |

**Clarification:** `#` is a submission key, not an invalid character. A short PIN followed by `#` is discarded; a four-digit PIN followed by `#` is validated.

### 3.3 Failed attempts and lockout

| ID | Requirement | Implementation / verification |
|---|---|---|
| REQ-F-014 | The system shall count a failed attempt only when a complete four-digit PIN is submitted and rejected. | Implemented; host-tested. |
| REQ-F-015 | The system shall enter LockedOut after three failed PIN submissions. | Implemented; host-tested. |
| REQ-F-016 | While LockedOut, the system shall ignore keypad input for access-control purposes. | Implemented; host-tested. |
| REQ-F-017 | While LockedOut, the system shall indicate the lockout state by blinking the red LED and keeping the green LED off. | Blink request is implemented; host mock checks the request. Actual blink behavior and timing open. |
| REQ-F-018 | The system shall remain LockedOut for 30 seconds, measured by the system clock. | Implemented using elapsed-time comparison; host-tested with a mock clock, including wraparound scenarios. Physical timing open. |
| REQ-F-019 | When the 30-second lockout period expires, the system shall return to Locked. | Implemented; host-tested. |
| REQ-F-020 | When the lockout period expires, the system shall reset the failed-attempt counter and clear any buffered PIN input. | Implemented; host-tested. |
| REQ-F-021 | A new lockout period shall start from the time the third failed attempt is registered. | Implemented using `lockoutStartMs_`; host-tested for a subsequent lockout. |
| REQ-F-027 | After a successful unlock, the system shall reset the failed-attempt counter to 0. | Implemented in `ValidatePin()` (`attempts_.Reset()`); host-tested (`UnlockResetsAttemptsCounter`). |

### 3.4 Unlocked state and PIN change

| ID | Requirement | Implementation / verification |
|---|---|---|
| REQ-F-022 | When the system is Unlocked, it shall indicate this state by turning on the green LED and turning off the red LED. | Implemented; host-tested through the LED mock. Physical output open. |
| REQ-F-023 | While Unlocked, the user shall be able to enter a new four-digit numeric PIN and submit it using `#`. | Implemented; host-tested. |
| REQ-F-024 | After a valid four-digit PIN has been submitted in the Unlocked state, the system shall store the new PIN and return to Locked. | Implemented; host-tested. The new hash is stored in RAM. |
| REQ-F-025 | If `#` is pressed in the Unlocked state before four digits have been entered, the input shall be cleared and the system shall remain Unlocked. | Implemented; verify explicit test coverage. |
| REQ-F-026 | Invalid input while Unlocked shall clear the current PIN input without changing the access state. | Implemented in `ProcessKey()` and `Update()`; verify explicit test coverage. |



## 4. Hardware and implementation constraints

These describe the current design and target platform. They are not necessarily product-level functional requirements.

| ID | Constraint | Current implementation |
|---|---|---|
| CON-001 | The target shall be the STM32F407VG microcontroller on the STM32F407G-DISC1 board. | Current firmware target. |
| CON-002 | The application shall use bare-metal, register-level peripheral access without STM32 HAL or CubeMX-generated application code. | Current project architecture. |
| CON-003 | The keypad shall be a 4×4 matrix keypad. | Current keypad driver. |
| CON-004 | Keypad rows shall use GPIOD pins PD0–PD3. | Configured as outputs. Board-test source exists. |
| CON-005 | Keypad columns shall use GPIOD pins PD8–PD11. | Configured as inputs with internal pull-ups. Board-test source exists. |
| CON-006 | Keypad column interrupts shall use EXTI lines 8–11, mapped to port D and configured for falling edges. | Configured by `Keypad_EnableInterrupt()`. Board-test source exists. |
| CON-007 | The keypad EXTI interrupt groups shall use NVIC IRQ23 and IRQ40. | Enabled by `Keypad_EnableInterrupt()`. Board-test source exists. |
| CON-008 | The green LED shall use GPIOD pin PD12 and the red LED shall use GPIOD pin PD14. | Configured by `LedOutputAdapter`. |
| CON-009 | The system shall use the 16 MHz HSI clock configuration assumed by the current SysTick setup. | `SysTickClock` calculates a 1 kHz tick from 16 MHz. No PLL setup is used in the described configuration. |
| CON-010 | The system shall use SysTick as the millisecond time base. | SysTick reload value is 15,999; interrupt increments a 32-bit millisecond counter. Provided to the controller by `SysTickClock`, which tests replace with a mock. Actual SysTick accuracy open. |
| CON-011 | The keypad driver shall not return a detected key event until the key has been released. | Implemented by a blocking release loop in `Keypad_Scan()`; host fake simulates release. Physical behavior open. |
| CON-012 | The keypad driver shall mask keypad EXTI interrupts while it scans and processes a key event. | Implemented; host-side fake checks masking during GPIO reads. Board-level behavior open. |

## 5. Keypad behavior and known limitations

The following observations are important for architecture, design and test documentation. They should not all be interpreted as desired requirements.

- The keypad map is:

  | Row | Column 0 | Column 1 | Column 2 | Column 3 |
  |---|---|---|---|---|
  | 0 | `1` | `2` | `3` | `A` |
  | 1 | `4` | `5` | `6` | `B` |
  | 2 | `7` | `8` | `9` | `C` |
  | 3 | `*` | `0` | `#` | `D` |

- The EXTI handlers set a single pending flag. There is no keypad event queue.
- The scanner checks rows from 0 to 3 and columns from 0 to 3; the first detected low column is returned.
- The scanner waits in a blocking loop until the detected key is released. This blocks the main loop while the key is held.
- The driver uses a busy-loop delay before waiting for release and another delay after scanning. Its duration is not specified or calibrated.
- Multiple simultaneous key behavior, including possible matrix ghosting, has not been established on physical hardware.
- Because the event mechanism is a single flag rather than a queue, multiple events may be coalesced or lost while processing is in progress.
- The application accepts only numeric digits and `#` for PIN operations. `A`–`D` and `*` are treated as invalid input by the controller.

These points should be revisited if the keypad driver is redesigned.

## 6. Verification summary

| Verification level | Existing evidence | Limitations / next action |
|---|---|---|
| Host unit tests | Tests cover controller state transitions, PIN validation, attempt counting, keypad driver logic with fakes and EXTI helper behavior. | Review individual assertions against each requirement; add tests for any uncovered cases. |
| Unity board-test source | Tests check GPIO configuration, pull-ups, SYSCFG/EXTI configuration, NVIC enables and software-triggered EXTI handling. | Source exists, but successful execution on physical hardware must be recorded separately. |
| Manual hardware tests | No physical execution evidence recorded in this baseline. | Verify real keypad operation, LED outputs, lockout timing, reset behavior and complete user interaction when the board is available. |

## 7. Traceability

Mapping of requirements and constraints to their tests. Test IDs refer to
[04-test-specification.md](04-test-specification.md).

| Requirement group | Test IDs |
|---|---|
| REQ-F-001–REQ-F-004 | UT-AC-01, UT-AC-02, UT-PV-01, UT-PV-03, ST-01, ST-10 |
| REQ-F-005–REQ-F-013 | UT-AC-02 to UT-AC-07, UT-AC-19, UT-PV-02, UT-PV-04 |
| REQ-F-014–REQ-F-021, REQ-F-027 | UT-AC-03, UT-AC-08 to UT-AC-13, UT-AC-21, UT-AT-01 to UT-AT-03, IT-05, IT-09, ST-20 |
| REQ-F-022–REQ-F-026 | UT-AC-02, UT-AC-14 to UT-AC-18, UT-AC-20, UT-LA-03 |
| CON-004–CON-007 | IT-06, IT-07, ST-17 (board test source exists; physical run status open) |
| CON-008–CON-010 | UT-LA-01, UT-SC-01, INS-02 (no confirmed physical timing result) |
| CON-011–CON-012 | UT-KD-03, UT-KD-04 |
| CON-002 | INS-01 |
| CON-003 | UT-KA-01, UT-KD-02 |