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
- Firmware: 21842c8
- Debugger: GDB via ST-Link / OpenOCD
- Build configuration: CMake, ARM GCC, Debug build
- Test setup: STM32F407G-DISC1 board connected via USB; OpenOCD and GDB; firmware flashed to physical hardware

## 4. Problem Description

**Expected behavior:**

The AccessController's actions are based on internal states, which are visualized by LED behavior (Locked - red light, Unlocked - green light, LockedOut - blinking red). The state after reset/initialization is Locked. There is an initial PIN (1234). The PIN entry is confirmed by pressing '#'. If the valid PIN is entered, the state transitions to Unlocked, and the green LED is lit. To leave this state, the user must enter a 4-digit PIN (the old or a new one) and confirm with '#'. This transitions the system back to the Locked state. If the user enters a wrong PIN three times, the system transitions to the LockedOut state, where the red LED is blinking. This state is controlled by the SysTick timer and transitions back to the Locked state after 30 seconds.

**Actual behavior:**

The transitions are not reliable when entering the PIN at a normal typing speed. Sometimes it works, sometimes it doesn't. The LockedOut state never transitions back to Locked state.

**Reproduction steps:**

1. Build the firmware. 
2. Flash the firmware to the target. 
3. (Start the debugger.) 
4. Execute the test scenario. 
5. (Record the observed behavior.) 

## 5. Test Matrix
### 5.1 Unit Tests

### 5.2 Continuous Integration

### 5.3 Hardware Tests

### 5.4 Regression Tests

## 6. Investigation Log

### Investigation 1 — 2026-10-27

**Question:** [What is being investigated?]

**Method:** [What was changed, measured, or inspected?]

**Observation:** [What was actually observed?]

**Conclusion:** [What does the evidence establish?]

**Next step:** [What should be investigated next?]

## 7. Findings

[Record confirmed findings separately from unverified hypotheses.]

## 8. Next Steps

- Characterize the hardware failure precisely.
- Inspect the relevant scheduler state and execution flow.
- Form and test hypotheses based on the observations.
- Implement a correction once the cause is established.
- Repeat the original hardware test.
- Verify that existing unit tests still pass.
- Document the final resolution and any remaining limitations.

## 9. Root Cause

## 10. Resolution
