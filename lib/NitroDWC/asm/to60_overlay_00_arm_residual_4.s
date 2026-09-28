	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_021FD568
	.extern ov00_021FD4F8

.text
	arm_func_start ov00_021FD568
ov00_021FD568: ; 0x021FD568
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	add r4, r2, r2, lsl #1
	str r0, [sp]
	mov r0, r4, lsr #2
	str r1, [sp, #4]
	cmp r3, r4, lsr #2
	str r0, [sp, #8]
	blo _021FD59C
	and fp, r2, #3
	sub r0, r2, fp
	str r0, [sp, #0xc]
	b _021FD5A8
_021FD59C:
	add sp, sp, #0x18
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_021FD5A8:
	cmp r0, #0
	mov r7, #0
	ble _021FD630
	mov sb, r7
	add r5, sp, #0x14
	mov r4, #6
_021FD5C0:
	ldr r0, [sp]
	mov r6, #0
	mov r8, r6
	add sl, r0, r7
_021FD5D0:
	ldrb r0, [sl], #1
	bl ov00_021FD4F8
	rsb r1, r8, #3
	mul r2, r1, r4
	orr r6, r6, r0, lsl r2
	add r8, r8, #1
	cmp r8, #4
	blt _021FD5D0
	ldr r0, [sp, #4]
	add r1, sb, sb, lsl #1
	mov r2, #0
	str r6, [sp, #0x14]
	add r1, r0, r1
_021FD604:
	rsb r0, r2, #2
	ldrb r0, [r5, r0]
	add r2, r2, #1
	cmp r2, #3
	strb r0, [r1], #1
	blt _021FD604
	ldr r0, [sp, #0xc]
	add r7, r7, #4
	cmp r7, r0
	add sb, sb, #1
	blt _021FD5C0
_021FD630:
	cmp fp, #0
	beq _021FD6C4
	mov r5, #0
	mov r6, r5
	str r5, [sp, #0x10]
	ble _021FD684
	ldr r1, [sp]
	ldr r0, [sp, #0xc]
	mov r8, r5
	add r4, r1, r0
	mov r7, #6
_021FD65C:
	ldrb r0, [r4], #1
	bl ov00_021FD4F8
	rsb r1, r6, #3
	mul r2, r1, r7
	orr r5, r5, r0, lsl r2
	add r6, r6, #1
	cmp r6, fp
	orr r8, r8, r5
	blt _021FD65C
	str r8, [sp, #0x10]
_021FD684:
	cmp fp, #0
	mov r3, #0
	ble _021FD6C4
	ldr r0, [sp, #0xc]
	add r1, r0, r0, lsl #1
	mov r0, r1, asr #1
	add r1, r1, r0, lsr #30
	ldr r0, [sp, #4]
	add r2, r0, r1, asr #2
	add r1, sp, #0x10
_021FD6AC:
	rsb r0, r3, #2
	ldrb r0, [r1, r0]
	add r3, r3, #1
	cmp r3, fp
	strb r0, [r2], #1
	blt _021FD6AC
_021FD6C4:
	ldr r0, [sp, #8]
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov00_021FD568
