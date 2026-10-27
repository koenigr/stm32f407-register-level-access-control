# STM32 Access Control System — Hardware Debugging Log

## 1. Problem Statement

Investigate unreliable state transitions in the STM32 access control system on real hardware.

The objective is to identify why the application does not consistently behave as expected, despite passing unit tests. PArticular attention is given to the LockedOut timeout, which should return the system to the Locked state adter 30 seconds.

## 2. Current Status

- [x] Access control state machine implemented
- [x] Unit tests pass
- [x] SysTick based millisecond clock implemented
- [x] Unit tests pass locally
- [x] GitHub Actions CI pipeline passes
- [ ] Expected behavior verified on real hardware
- [ ] Root cause identified
- [ ] Fix implemented and verified
- [ ] Regression testing completed

## 3. Test Environment

- Target: STM32F407G-DISC1
- Firmware: commit-hash: 21842c8
- Debugger: GDB via GDB via ST-Link / OpenOCD
- Build configuration: CMake, ARM GCC, Debug build
- Test setup: STM32F407G-DISC1 board connected via USB; OpenOCD and GDB; firmware flashed to physical hardware

## 4. Problem Description

**Expected behavior:**

The AccessController's actions are based on internal states, which are visualized by LED behavior (Locked – red light, Unlocked – green light, LockedOut – blinking red). The state after reset/initialization is Locked. There is an initial PIN (1234). The PIN entry is confirmed by pressing '#'. If a valid PIN is entered, the state transitions to Unlocked, and the green LED lights up. To leave this state, the user must enter a 4-digit PIN (the old or a new one) and confirm with '#'. This transitions the system back to the Locked state. If the user enters a wrong PIN three times, the system transitions to the LockedOut state, where the red LED blinks. This state is controlled by the SysTick timer and transitions back to the Locked state after 30 seconds.

**Actual behavior:**

The transitions are not reliable when entering the PIN at a normal typing speed. Sometimes it works, sometimes it doesn't. The LockedOut state never transitions back to the Locked state.

**Reproduction steps:**

1. Build and flash the firmware to the STM32F407G-DISC1.
2 Enter an incorrect PIN three times, confirming each attempt with #.
3. Verify that the system enters LockedOut and the red LED blinks.
4. Wait at least 30 seconds.
5. Observe whether the system returns to Locked and accepts keypad input again.
6. Record the observed LED behavior and response to keypad input.

## 5. Investigation Log

### Investigation 1 — SysTick and Lockout Timeout

**Question:** Does the millisecond counter advance correctly, and does the lockout timeout expire after 30 seconds?

**Method:** Inspect g_ms, SysTick_Handler(), lockoutStartsMs_, and the condition in AccessController::Update().Verify the behavior on hardware using GDB.

**Observation:** Pending hardware investigation.

**Conclusion:** No root cause established yet.

**Next step:** Verify that SysTick_Handler() is called and that clock_.NowMs() advances while the application is running.

## 6. Findings

[Record confirmed findings separately from unverified hypotheses.]

## 7. Next Steps

- Characterize the hardware failure precisely.
- Inspect the relevant scheduler state and execution flow.
- Form and test hypotheses based on the observations.
- Implement a correction once the cause is established.
- Repeat the original hardware test.
- Verify that existing unit tests still pass.
- Document the final resolution and any remaining limitations.

## 8. Root Cause

## 9. Resolutio
