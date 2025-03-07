/**
  ******************************************************************************
  * @file      bootloader.s
  * @author    saurab from UESTC
  * @brief     STM32H743xx Devices vector table for GCC based toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == BootLoader,
  *                - Change SCB->VTOR to new RAM value
  *                - Branches to Reset_Handler
  *            After the BootLoader .isr_vector \ .text \ .rodata goes into RAM
  *            and PC indicates Reset_Handler and SP indicates _estack.
*/

  .syntax unified
  .cpu cortex-m7
  .fpu softvfp
  .thumb

.global BootLoader
.global load

/* SCB->VTOR address */
.equ SCB_VTOR, 0xE000ED08

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/* start address for the .isr_vector section. defined in linker script */
.word  _svector_text
/* end address for the .isr_vector section. defined in linker script */
.word  _evector_text
/* start address for the initialization values of the .isr_vector section.
defined in linker script */
.word _sivector_text

  .section .bootloader,"ax",%progbits
  .type BootLoader, %function

BootLoader:
  ldr sp, =_estack  /* Set stack pointer */

/* confirm if the code has been to SRAM */
  ldr r0, =_svector_text
  ldr r1, =_sivector_text
  ldr r0, [r0]
  ldr r1, [r1]
  cmp r0, r1
  beq NoCopy

/* Copy the isr_vector and text section initializers from flash to SRAM*/
  ldr r0, =_svector_text
  ldr r1, =_evector_text
  ldr r2, =_sivector_text
  movs r3, #0
  b LoopCopyCodeInit

CopyCodeInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3 ,r3 ,#4

LoopCopyCodeInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc CopyCodeInit

NoCopy:
/* change SCB->VTOR to new RAM value */
  ldr r1, =SCB_VTOR
  ldr r2, =#0x00000000 /* .ld链接脚本中的DTCMRAM地址 */
  str r2, [r1]

  ldr r0, =Reset_Handler
  blx r0

  ldr r0, =Default_Handler
  blx r0

  bx lr


.size  BootLoader, .-BootLoader

  .section  .load,"a",%progbits
  .type  load, %object
load:
  .word _estack
  .word BootLoader
.size  load, .-load