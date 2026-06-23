.syntax unified
.cpu cortex-m4
.thumb

.global Reset_Handler
.global Default_Handler

/* Stack pointer */
.word _estack

/* Vector Table */
.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object

g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word Default_Handler /* NMI */
    .word Default_Handler /* HardFault */
    .word Default_Handler /* MemManage */
    .word Default_Handler /* BusFault */
    .word Default_Handler /* UsageFault */

/* ---- Reset Handler ---- */
.section .text.Reset_Handler
.weak Reset_Handler
.type Reset_Handler, %function

Reset_Handler:
    /* Copy .data from FLASH to RAM */
    LDR r0, =_sdata
    LDR r1, =_edata
    LDR r2, =_sidata

copy_data:
    CMP r0, r1
    ITTT LT
    LDRLT r3, [r2], #4
    STRLT r3, [r0], #4
    BLT copy_data

    /* Zero .bss */
    LDR r0, =_sbss
    LDR r1, =_ebss
    MOVS r2, #0

zero_bss:
    CMP r0, r1
    IT LT
    STRLT r2, [r0], #4
    BLT zero_bss

    /* Call main */
    BL main

loop_forever:
    B loop_forever

/* Default Handler */
.section .text.Default_Handler
.weak Default_Handler
.type Default_Handler, %function

Default_Handler:
    B Default_Handler
