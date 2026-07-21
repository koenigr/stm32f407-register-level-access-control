# Keypad Connection

## Overview

The system uses a 4x4 matrix keypad as user input device.

The keypad is connected directly to GPIO pins of the STM32F407VG.

The keypad driver scans the matrix by activating rows and reading column states.

## Wiring

![Keypad Connection](img/KeypadConnection.jpg)

## Pin Assignment

| Signal | MCU Pin |
|---|---|
| Row 1 | PD0 |
| Row 2 | PD1 |
| Row 3 | PD2 |
| Row 4 | PD3 |
| Column 1 | PD8 |
| Column 2 | PD9 |
| Column 3 | PD10 |
| Column 4 | PD11 |

## Key Layout


```text
    1 2 3 A
    4 5 6 B
    7 8 9 C
    * 0 # D
```

## Implementation

The keypad hardware is accessed through the keypad driver.

The driver handles:

- row activation
- column reading
- matrix scanning
- key detection