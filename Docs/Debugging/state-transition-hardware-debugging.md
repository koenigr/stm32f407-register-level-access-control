# STM32 Access Control System — Hardware Debugging Log

## 1. Problem Statement

Investigate unreliable state transitions in the STM32 access control system on real hardware.

The objective is to identify why the application does not consistently behave as expected, despite passing unit tests. PArticular attention is given to the LockedOut timeout, which should return the system to the Locked state adter 30 seconds.

## 2. Current Status

- [x] Access control state machine implemented
- [x] SysTick based millisecond clock implemented
- [x] Unit tests pass locally
- [x] GitHub Actions CI pipeline passes
- [x] Expected behavior verified on real hardware
- [x] Root cause identified
- [x] Fix implemented and verified
- [x] Regression testing completed

## 3. Test Environment

- Target: STM32F407G-DISC1
- Firmware: Commit 21842c8
- Debugger: GDB via ST-Link / OpenOCD
- Build configuration: CMake, ARM GCC, Debug build
- Test setup: STM32F407G-DISC1 board connected via USB; OpenOCD and GDB; firmware flashed to physical hardware

## 4. Problem Description

**Expected behavior:**

The AccessController's actions are based on internal states, which are visualized by LED behavior (Locked – red light, Unlocked – green light, LockedOut – blinking red). The state after reset/initialization is Locked. There is an initial PIN (1234). The PIN entry is confirmed by pressing '#'. If a valid PIN is entered, the state transitions to Unlocked, and the green LED lights up. To leave this state, the user must enter a 4-digit PIN (the old or a new one) and confirm with '#'. This transitions the system back to the Locked state. If the user enters a wrong PIN three times, the system transitions to the LockedOut state, where the red LED blinks. This state is controlled by the SysTick timer and transitions back to the Locked state after 30 seconds.

**Actual behavior:**
- Normal transitions between Locked and Unlocked worked as expected.
- Once the system entered the lockout state, the red LED continued blinking instead of returning to the expected behavior after the 30-second timeout.
- Entering a valid PIN after the timeout caused the LED to turn green briefly, but the red LED then resumed blinking.

**Reproduction steps:**

1. Build and flash the firmware to the STM32F407G-DISC1.
2. Enter an incorrect PIN three times, confirming each attempt with #.
3. Verify that the system enters LockedOut and the red LED blinks.
4. Wait at least 30 seconds.
5. Observe whether the system returns to Locked and accepts keypad input again.
6. Record the observed LED behavior and response to keypad input.

## 5. Investigation Log

### Investigation 1 — SysTick and Lockout Timeout

**Question:** Does the millisecond counter advance correctly, and does the lockout timeout expire after 30 seconds?

**Method:**
- Enabled GDB debugging and SWO tracing
- Monitored SWO output to verify that keypad presses were correctly detected in the interrupt handlers.
- Used GDB to inspect g_ms and lockoutStartsMs_ and verify the timekeeping and lockout start time.
- Set a watchpoint on state_ to observe state transitions and identify the code executed during each transition.

**Observation:**
- Keypad presses were correctly detected in the interrupt handlers, as confirmed by SWO output.
- g_ms advanced as expected, and lockoutStartsMs_ was set correctly.
- The state transitions occurred as expected.
- The corresponding LED functions (leds_.Red(), leds_.Green(), and leds_,BlinkRed()) were called at the appropriate points in the application logic.

**Preliminary Conclusion:**
The investigation found no evidence of a problem in the SysTick timekeeping, lockout timing, keypad interrupt handling, or state transitions. The investigation therefore narrowed the suspected fault to the LED control functions.

## 6. Findings
- Hardware debugging confirmed that g_ms advanced as expected and lockoutStartsMs_ was set correctly.
- The state transitions occurred as expected. The investigation therefore ruled out the suspected timekeeping and state-transition logic as the cause of the observed LED behavior.
- The fault was isolated to the LED output adapter.
- **Debugging lesson:** When setting breakpoints or evaluating expressions in GDB, the current execution context matters. The visibility and accessibility of local variables and object members depend on the current scope and stack frame.


## 7. Root Cause
![Original LED output adapter implementation](../img/blinking_bug.jpg)

The fault was caused by a persistent blinking flag in the LED output adapter.

The BlinkRed() function set blinking_ to true, but the flag was never reset when leaving the lockout state. Consequently, the blinking behavior remained active after the lockout had expired.

The Update() function continued executing the blinking logic and controlling the GPIO pins accordingly. When a valid PIN was entered after the timeout, the green LED was activated briefly, but the next blinking update restored the red LED output.

## 8. Resolution
**Fix:**
- Reset blinking_ when leaving the blinking state, preventing the blinking logic from remaining active after the lockout ends.
- **Fix commit:** bcd904d Fix Blinking Bug

![Corrected LED output adapter implementation](../img/corrected_blinking_bug.jpg)

  
**Verification:**
- Repeated the lockout test on the STM32 hardware.
- Confirmed that the red LED stopped blinking after the 30-second timeout.
- Confirmed that entering a valid PIN produced the expected green LED behavior.
- Verified that the existing unit tests passed and the GitHub Actions CI pipeline completed successfully.
