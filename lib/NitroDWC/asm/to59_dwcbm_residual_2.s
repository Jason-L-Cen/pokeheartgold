	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"
	.public DWCi_BACKUPlWriteAll
	.extern _021D43C8
	.extern verify
	.extern writeDisable
	.extern writeNvram

.text
	arm_func_start DWCi_BACKUPlWritePage
DWCi_BACKUPlWritePage: ; 0x0209E654
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r3, _0209E6D8 ; =_021D43C8
	mov sl, #0x100
	ldr r5, [r3, #8]
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r4, #0
	mov sb, sl
_0209E678:
	ldr r0, [r7, r4, lsl #2]
	cmp r0, #0
	beq _0209E6B0
_0209E684:
	mov r0, r5
	mov r1, sl
	mov r2, r8
	bl writeNvram
	mov r0, r8
	mov r1, r5
	mov r2, sb
	mov r3, r6
	bl verify
	cmp r0, #0
	beq _0209E684
_0209E6B0:
	add r4, r4, #1
	cmp r4, #4
	add r8, r8, #0x100
	add r5, r5, #0x100
	blt _0209E678
	bl writeDisable
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_0209E6D8: .word _021D43C8
	arm_func_end DWCi_BACKUPlWritePage

	arm_func_start DWCi_BACKUPlWriteAll
DWCi_BACKUPlWriteAll: ; 0x0209E6DC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r1, _0209E76C ; =_021D43C8
	mov r8, r0
	mov sl, #0x100
	ldr r6, [r1, #8]
	mov r7, r8
	mov r5, #0
	mov sb, sl
	mov r4, #0xfe
_0209E700:
	mov r1, r7
	mov r2, r4
	add r0, r8, #0x500
	bl MATH_CalcCRC16
	add r1, r8, r5, lsl #8
	strh r0, [r1, #0xfe]
_0209E718:
	mov r0, r6
	mov r1, sl
	mov r2, r7
	bl writeNvram
	mov r0, r7
	mov r1, r6
	mov r2, sb
	add r3, r8, #0x400
	bl verify
	cmp r0, #0
	beq _0209E718
	add r5, r5, #1
	cmp r5, #4
	add r7, r7, #0x100
	add r6, r6, #0x100
	blt _0209E700
	bl writeDisable
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_0209E76C: .word _021D43C8
	arm_func_end DWCi_BACKUPlWriteAll
