# MISRA C:2012 Deviations

This document records intentional deviations accepted for the STM32F407 register-level access-control project. Suppressions are not a substitute for reviewing the underlying code: each exception must remain limited to the stated rule and scope.

## Rule 11.4 — Conversion between object pointers and integer types

**Scope:** `Drivers/keypad.c`, `Platform/stm32f407xx_gpio.c`, `Platform/stm32f407xx_exti.c`  
**Suppression:** `misra_suppressions.txt`

**Rationale:** The firmware accesses memory-mapped peripheral registers. Register access requires converting fixed hardware addresses to pointers to the corresponding register structures.

**Review/mitigation:** Keep address definitions and register access confined to the platform/driver layer. Verify addresses against the STM32F407 reference manual and device header; do not use these conversions for arbitrary application data.

## Rule 8.7 — Objects/functions should not be defined with external linkage if used in only one translation unit

**Scope:** `Platform/stm32f407xx_gpio.h`, `Platform/stm32f407xx_exti.h`, `Drivers/keypad.h`, `Drivers/swo.h`  
**Suppression:** `misra_suppressions.txt`

**Rationale:** These headers intentionally expose module interfaces. Some functions may currently have only one caller, but retaining the public interface is deliberate for reuse and separation between modules (including the SWO debugging utility).

**Review/mitigation:** Keep declarations in the owning module's header and definitions in the corresponding source file. Remove an interface only after confirming it is no longer part of the intended module API.

## Rule 2.5 — Unused macro

**Scope:** `Drivers/keypad.h`, `Platform/stm32f407xx_exti.h`, `Platform/stm32f407xx_gpio.h`, `Platform/cortex_m4.h`  
**Suppression:** `misra_suppressions.txt`

**Rationale:** The reported macros include header guards and intentionally available platform definitions, including GPIO definitions retained for the hardware abstraction/platform interface. These are not accidental application-level leftovers.

**Review/mitigation:** Keep the exception limited to these headers. Reassess individual definitions if the platform interface changes; do not extend the suppression to unrelated files.

## Rule 15.5 — A function should have a single point of exit

**Scope:** `Drivers/keypad.c`  
**Suppression:** `misra_suppressions.txt`

**Rationale:** The early returns in the keypad scanning/control flow are intentional. They make the successful key-detection path and function outcomes easier to follow; changing them solely to satisfy the rule would add control-flow complexity without a clear safety benefit.

**Review/mitigation:** Keep the function's exit behavior straightforward and ensure each return path has the intended result.

## Rule 10.4 — Operands should have the same essential type category

**Scope:** Two comparisons in `Drivers/keypad.c` that compare `GPIO_ReadPin(...)` with `KEYPAD_LOW`  
**Suppression:** Inline `cppcheck-suppress` comments immediately before the affected `if` and `while` statements.

**Rationale:** `GPIO_ReadPin` returns `uint8_t`, and `KEYPAD_LOW` is explicitly defined as `((uint8_t)0U)`. Cppcheck 2.7 still reports Rule 10.4 for these comparisons. The comparisons test the digital input's low state and are intentional.

**Review/mitigation:** Keep the return type and low-state constant explicit. Revisit the inline suppressions if the checker version or expression changes; do not suppress Rule 10.4 for the entire file.

## Rule 12.2 — Shift and bitwise expression

**Status:** No active suppression.

The previous finding at the `RCC_APB2ENR_SYSCFGEN` use was resolved by expressing the mask as `(UINT32_C(1) << 14U)`. Keep the shift count within the width of the left operand and rerun the MISRA check after changes to register masks.

## Validation

Run the project's MISRA target and inspect the resulting findings:

```sh
make misra
grep -oE 'misra-c[0-9]+-[0-9]+\\.[0-9]+' misra.log | sort | uniq -c | sort -rn
```

Any new finding must be reviewed and preferably fixed. Add a deviation only when the behavior is intentional, the rationale is documented, and the suppression is as narrow as practical.
