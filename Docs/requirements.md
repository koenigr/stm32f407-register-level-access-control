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

  -----------------------------------------------------------------------
  Term                                Meaning
  ----------------------------------- -----------------------------------
  **Implemented**                     The current source code contains
                                      behavior corresponding to the
                                      requirement.

  **Host-tested**                     Existing host-side tests exercise
                                      the behavior.

  **Board-test source**               A Unity test exists that is
                                      intended to exercise the target
                                      hardware. This does not mean it has
                                      been run successfully on a physical
                                      board.

  **Open**                            The behavior needs additional
                                      testing, clarification or physical
                                      verification.
  -----------------------------------------------------------------------

A source-code observation is not automatically proof that the behavior
is reliable under real hardware conditions.

## 3. Functional requirements

### 3.1 Startup and default state

  ---------------------------------------------------------------------------
  ID                      Requirement             Implementation /
                                                  verification
  ----------------------- ----------------------- ---------------------------
  REQ-F-001               After startup, the      Implemented; host-tested
                          access controller shall through
                          enter the **Locked**    initial-state/default-PIN
                          state.                  scenarios.

  REQ-F-002               After startup, the      Implemented in `main()` and
                          system shall indicate   LED adapter; controller LED
                          the Locked state by     commands are host-tested.
                          turning on the red LED  Physical output open.
                          and turning off the     
                          green LED.              

  REQ-F-003               After startup, the      Implemented through the
                          system shall accept the default PIN hash;
                          default PIN `1234`.     host-tested.

  REQ-F-004               A PIN changed during    Implemented: the changed
                          operation shall remain  PIN hash is held in RAM;
                          changed only until the  reset behavior on physical
                          next reset or power     hardware open.
                          cycle.                  
  ---------------------------------------------------------------------------

### 3.2 PIN entry and validation

  -----------------------------------------------------------------------
  ID                      Requirement             Implementation /
                                                  verification
  ----------------------- ----------------------- -----------------------
  REQ-F-005               The system shall accept Implemented;
                          numeric digits `0`--`9` host-tested.
                          as PIN-entry            
                          characters.             

  REQ-F-006               The system shall use    Implemented;
                          `#` to submit a PIN.    host-tested.

  REQ-F-007               The system shall        Implemented;
                          validate a submitted    host-tested for short
                          PIN only if exactly     and valid-length input.
                          four digits have been   
                          entered.                

  REQ-F-008               A submitted PIN         Implemented;
                          containing fewer than   host-tested.
                          four digits shall be    
                          discarded without       
                          incrementing the        
                          failed-attempt counter. 

  REQ-F-009               If a fifth digit is     Implemented:
                          entered before          `AppendDigit()` rejects
                          submission, the current the fifth digit and
                          PIN input shall be      `ProcessKey()` clears
                          cleared and discarded.  the buffer;
                                                  host-tested.

  REQ-F-010               If a key other than a   Implemented;
                          numeric digit or `#` is host-tested.
                          entered during PIN      
                          entry, the current PIN  
                          input shall be cleared  
                          and discarded.          

  REQ-F-011               After PIN submission or Implemented;
                          invalid input, the      host-tested.
                          input buffer shall be   
                          cleared.                

  REQ-F-012               If the submitted        Implemented;
                          four-digit PIN is       host-tested.
                          correct, the system     
                          shall transition from   
                          Locked to Unlocked.     

  REQ-F-013               If the submitted        Implemented;
                          four-digit PIN is       host-tested.
                          incorrect, the system   
                          shall remain Locked     
                          unless the              
                          failed-attempt          
                          threshold is reached.   
  -----------------------------------------------------------------------

**Clarification:** `#` is a submission key, not an invalid character. A
short PIN followed by `#` is discarded; a four-digit PIN followed by `#`
is validated.

### 3.3 Failed attempts and lockout

  -----------------------------------------------------------------------
  ID                      Requirement             Implementation /
                                                  verification
  ----------------------- ----------------------- -----------------------
  REQ-F-014               The system shall count  Implemented;
                          a failed attempt only   host-tested.
                          when a complete         
                          four-digit PIN is       
                          submitted and rejected. 

  REQ-F-015               The system shall enter  Implemented;
                          LockedOut after three   host-tested.
                          failed PIN submissions. 

  REQ-F-016               While LockedOut, the    Implemented;
                          system shall ignore     host-tested.
                          keypad input for        
                          access-control          
                          purposes.               

  REQ-F-017               While LockedOut, the    Blink request is
                          system shall indicate   implemented; host mock
                          the lockout state by    checks the request.
                          blinking the red LED    Actual blink behavior
                          and keeping the green   and timing open.
                          LED off.                

  REQ-F-018               The system shall remain Implemented using
                          LockedOut for 30        elapsed-time
                          seconds, measured by    comparison; host-tested
                          the system clock.       with a mock clock,
                                                  including wraparound
                                                  scenarios. Physical
                                                  timing open.

  REQ-F-019               When the 30-second      Implemented;
                          lockout period expires, host-tested.
                          the system shall return 
                          to Locked.              

  REQ-F-020               When the lockout period Implemented;
                          expires, the system     host-tested.
                          shall reset the         
                          failed-attempt counter  
                          and clear any buffered  
                          PIN input.              

  REQ-F-021               A new lockout period    Implemented using
                          shall start from the    `lockoutStartMs_`;
                          time the third failed   host-tested for a
                          attempt is registered.  subsequent lockout.
  -----------------------------------------------------------------------

### 3.4 Unlocked state and PIN change

  -----------------------------------------------------------------------
  ID                      Requirement             Implementation /
                                                  verification
  ----------------------- ----------------------- -----------------------
  REQ-F-022               When the system is      Implemented;
                          Unlocked, it shall      host-tested through the
                          indicate this state by  LED mock. Physical
                          turning on the green    output open.
                          LED and turning off the 
                          red LED.                

  REQ-F-023               While Unlocked, the     Implemented;
                          user shall be able to   host-tested.
                          enter a new four-digit  
                          numeric PIN and submit  
                          it using `#`.           

  REQ-F-024               After a valid           Implemented;
                          four-digit PIN has been host-tested. The new
                          submitted in the        hash is stored in RAM.
                          Unlocked state, the     
                          system shall store the  
                          new PIN and return to   
                          Locked.                 

  REQ-F-025               If `#` is pressed in    Implemented; verify
                          the Unlocked state      explicit test coverage.
                          before four digits have 
                          been entered, the input 
                          shall be cleared and    
                          the system shall remain 
                          Unlocked.               

  REQ-F-026               Invalid input while     Implemented in
                          Unlocked shall clear    `ProcessKey()` and
                          the current PIN input   `Update()`; verify
                          without changing the    explicit test coverage.
                          access state.           
  -----------------------------------------------------------------------

## 4. Non-functional requirements and timing characteristics

The following requirements describe timing or operational
characteristics visible in the current implementation. Values that have
not been measured on hardware are explicitly marked as unverified.

  -----------------------------------------------------------------------
  ID                      Requirement /           Implementation /
                          characteristic          verification
  ----------------------- ----------------------- -----------------------
  REQ-NF-001              The lockout duration    Implemented as 30,000
                          shall be 30 seconds.    ms; host-tested with
                                                  mock time. Physical
                                                  timing accuracy open.

  REQ-NF-002              The system clock        Implemented by
                          interface used for      `SysTickClock`;
                          lockout measurement     controller tests use a
                          shall provide           mock. Actual SysTick
                          millisecond units.      accuracy open.

  REQ-NF-003              The keypad driver shall Implemented by a
                          not return a detected   blocking release loop
                          key event until the key in `Keypad_Scan()`;
                          has been released.      host fake simulates
                                                  release. Physical
                                                  behavior open.

  REQ-NF-004              The keypad driver shall Implemented; host-side
                          mask keypad EXTI        fake checks masking
                          interrupts while it     during GPIO reads.
                          scans and processes a   Board-level behavior
                          key event.              open.
  -----------------------------------------------------------------------

**Not specified as a requirement yet:** keypad debounce time and red LED
blink frequency. The current driver uses an empty busy-loop delay, and
the LED blink interval depends on the number of calls to
`LedOutputAdapter::Update()`. Neither currently establishes a reliable
time-based specification.

## 5. Hardware and implementation constraints

These describe the current design and target platform. They are not
necessarily product-level functional requirements.

  -----------------------------------------------------------------------------
  ID                      Constraint              Current implementation
  ----------------------- ----------------------- -----------------------------
  CON-001                 The target shall be the Current firmware target.
                          STM32F407VG             
                          microcontroller on the  
                          STM32F407G-DISC1 board. 

  CON-002                 The application shall   Current project architecture.
                          use bare-metal,         
                          register-level          
                          peripheral access       
                          without STM32 HAL or    
                          CubeMX-generated        
                          application code.       

  CON-003                 The keypad shall be a   Current keypad driver.
                          4×4 matrix keypad.      

  CON-004                 Keypad rows shall use   Configured as outputs.
                          GPIOD pins PD0--PD3.    

  CON-005                 Keypad columns shall    Configured as inputs with
                          use GPIOD pins          internal pull-ups.
                          PD8--PD11.              

  CON-006                 Keypad column           Configured by
                          interrupts shall use    `Keypad_EnableInterrupt()`.
                          EXTI lines 8--11,       
                          mapped to port D and    
                          configured for falling  
                          edges.                  

  CON-007                 The keypad EXTI         Enabled by
                          interrupt groups shall  `Keypad_EnableInterrupt()`.
                          use NVIC IRQ23 and      
                          IRQ40.                  

  CON-008                 The green LED shall use Configured by
                          GPIOD pin PD12 and the  `LedOutputAdapter`.
                          red LED shall use GPIOD 
                          pin PD14.               

  CON-009                 The system shall use    `SysTickClock` calculates a 1
                          the 16 MHz HSI clock    kHz tick from 16 MHz. No PLL
                          configuration assumed   setup is used in the
                          by the current SysTick  described configuration.
                          setup.                  

  CON-010                 The system shall use    SysTick reload value is
                          SysTick as the          15,999; interrupt increments
                          millisecond time base.  a 32-bit millisecond counter.
  -----------------------------------------------------------------------------

## 6. Keypad behavior and known limitations

The following observations are important for architecture, design and
test documentation. They should not all be interpreted as desired
requirements.

-   The keypad map is:

      Row     Column 0   Column 1   Column 2   Column 3
      ----- ---------- ---------- ---------- ----------
      0            `1`        `2`        `3`        `A`
      1            `4`        `5`        `6`        `B`
      2            `7`        `8`        `9`        `C`
      3            `*`        `0`        `#`        `D`

-   The EXTI handlers set a single pending flag. There is no keypad
    event queue.

-   The scanner checks rows from 0 to 3 and columns from 0 to 3; the
    first detected low column is returned.

-   The scanner waits in a blocking loop until the detected key is
    released. This blocks the main loop while the key is held.

-   The driver uses a busy-loop delay before waiting for release and
    another delay after scanning. Its duration is not specified or
    calibrated.

-   Multiple simultaneous key behavior, including possible matrix
    ghosting, has not been established on physical hardware.

-   Because the event mechanism is a single flag rather than a queue,
    multiple events may be coalesced or lost while processing is in
    progress.

-   The application accepts only numeric digits and `#` for PIN
    operations. `A`--`D` and `*` are treated as invalid input by the
    controller.

These points should be revisited if the keypad driver is redesigned.

## 7. Verification summary

  -----------------------------------------------------------------------
  Verification level      Existing evidence       Limitations / next
                                                  action
  ----------------------- ----------------------- -----------------------
  Host unit tests         Tests cover controller  Review individual
                          state transitions, PIN  assertions against each
                          validation, attempt     requirement; add tests
                          counting, keypad driver for any uncovered
                          logic with fakes and    cases.
                          EXTI helper behavior.   

  Unity board-test source Tests check GPIO        Source exists, but
                          configuration,          successful execution on
                          pull-ups, SYSCFG/EXTI   physical hardware must
                          configuration, NVIC     be recorded separately.
                          enables and             
                          software-triggered EXTI 
                          handling.               

  Manual hardware tests   No physical execution   Verify real keypad
                          evidence recorded in    operation, LED outputs,
                          this baseline.          lockout timing, reset
                                                  behavior and complete
                                                  user interaction when
                                                  the board is available.
  -----------------------------------------------------------------------

### Known verification gaps

1.  Measure and specify keypad debounce behavior on real hardware.
2.  Test simultaneous key presses and determine the actual
    priority/ghosting behavior.
3.  Verify behavior for long-held keys and rapid successive key presses.
4.  Verify LED blink cadence and behavior when the main loop is blocked
    by keypad scanning.
5.  Verify SysTick timing and the 30-second lockout duration on the
    target.
6.  Verify that a changed PIN is lost after reset and the default PIN is
    restored.
7.  Run the Unity board tests on the physical STM32F407 board and record
    the result.
8.  Perform an end-to-end test of the access-control workflow using the
    physical keypad and LEDs.

## 8. Traceability

The following is an initial mapping. Test names are intentionally
described by test area rather than guessed file paths; exact test
identifiers can be added when the test files are integrated into the
repository documentation.

  -----------------------------------------------------------------------
  Requirement group                   Existing test evidence
  ----------------------------------- -----------------------------------
  REQ-F-001--REQ-F-004                Access-controller and PIN-validator
                                      host tests; startup LED
                                      initialization visible in source.

  REQ-F-005--REQ-F-013                Access-controller host tests for
                                      PIN entry, invalid input, buffer
                                      clearing and validation.

  REQ-F-014--REQ-F-021                Access-controller and
                                      attempt-counter host tests using
                                      `MockClock`.

  REQ-F-022--REQ-F-026                Access-controller host tests using
                                      `MockLedOutput`; confirm individual
                                      edge-case coverage against test
                                      assertions.

  REQ-NF-001--REQ-NF-002              Host tests with mock time; SysTick
                                      hardware configuration has no
                                      confirmed physical timing result.

  REQ-NF-003--REQ-NF-004              Keypad host tests with hardware
                                      fakes.

  CON-004--CON-007                    Unity board-test source checks
                                      GPIO, EXTI and NVIC configuration;
                                      physical run status open.

  CON-008--CON-010                    Source-code inspection; additional
                                      target-level verification remains
                                      open.
  -----------------------------------------------------------------------

## 9. Change history

  -----------------------------------------------------------------------
  Version                             Description
  ----------------------------------- -----------------------------------
  0.1                                 Initial retrospective requirements
                                      baseline derived from the current
                                      source code and available
                                      host/Unity test sources. Open
                                      verification items explicitly
                                      retained.

  -----------------------------------------------------------------------
