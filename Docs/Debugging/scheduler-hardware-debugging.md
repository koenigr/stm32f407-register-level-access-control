# STM32 Scheduler — Hardware Debugging Log

## 1. Objective

Investigate unexpected behavior in the scheduler implementation on real STM32 hardware.

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

[Describe the expected scheduler behavior.]

**Actual behavior:**

[Describe the behavior observed on the target hardware.]

**Reproduction steps:**

1. Build the firmware.
2. Flash the firmware to the target.
3. Start the debugger.
4. Execute the test scenario.
5. Record the observed behavior.

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