# STM32 Scheduler — Hardware Debugging Log

## 1. Problem Statement

Investigate unexpected behavior in the scheduler implementation (issue #24) on real STM32 hardware.

The objective is to identify the discrepancy between the behavior verified by unit tests and the behavior observed during hardware testing.

## 2. Current Status

- [x] Scheduler implementation completed
- [x] Unit tests pass
- [ ] Expected behavior verified on real hardware
- [ ] Root cause identified
- [ ] Fix implemented and verified
- [ ] Regression testing completed

## 3. Test Environment

- Target: STM32F407G-DISC1
- Firmware: [commit hash / build identifier]
- Debugger: GDB via [debug interface]
- Build configuration: [configuration]
- Test setup: [relevant hardware and software configuration]

## 4. Problem Description

**Expected behavior:**

The AccessController's actions are based on internal states, which are visualized by LED behavior (Locked - red light, Unlocked - green light, LockedOut - blinking red). The state after reset/initialization is Locked. There is an initial PIN (1234). The PIN entry is confirmed by pressing '#'. If the valid PIN is entered, the state transitions to Unlocked, and the green LED is lit. To leave this state, the user must enter a 4-digit PIN (the old or a new one) and confirm with '#'. This transitions the system back to the Locked state. If the user enters a wrong PIN three times, the system transitions to the LockedOut state, where the red LED is blinking. This state is controlled by the SysTick timer and transitions back to the Locked state after 30 seconds.

**Actual behavior:**

The transitions are not reliable when entering the PIN at a normal typing speed. Sometimes it works, sometimes it doesn't. The LockedOut state never transitions back to Locked state.

**Reproduction steps:**

1. Build the firmware.
2. Flash the firmware to the target.
(3. Start the debugger.)
4. Execute the test scenario.
(5. Record the observed behavior.)

## 5. Investigation Log

### Investigation 1 — [Date]

**Question:** [What is being investigated?]

**Method:** [What was changed, measured, or inspected?]

**Observation:** [What was actually observed?]

**Conclusion:** [What does the evidence establish?]

**Next step:** [What should be investigated next?]

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
