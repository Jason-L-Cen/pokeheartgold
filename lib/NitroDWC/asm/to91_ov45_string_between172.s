	.extern printf
	.extern abort
	.extern ov45_02238104
	.extern ov45_02255144
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_02237E04
.text
	arm_func_start ov45_02237E04
ov45_02237E04: ; 0x02237E04
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r4, [r3]
	mov r8, r0
	mov r0, r4, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r7, r1
	mov r6, r2
	ldrne r4, [r3, #8]
	ldrne r5, [r3, #4]
	bne _02237E40
	ldrb r0, [r3]
	add r4, r3, #1
	mov r0, r0, lsl #0x18
	mov r5, r0, lsr #0x19
_02237E40:
	ldr r0, [sp, #0x28]
	cmp r0, r5
	bls _02237E58
	ldr r0, _02237EAC ; =ov45_02255144
	bl printf
	bl abort
_02237E58:
	ldr r2, [sp, #0x28]
	ldr r0, [sp, #0x2c]
	sub r1, r5, r2
	cmp r1, r0
	addlo r0, sp, #0xc
	str r1, [sp, #0xc]
	add r4, r4, r2
	addhs r0, sp, #0x2c
	ldr r0, [r0]
	ldrb r5, [sp, #8]
	str r0, [sp, #0x2c]
	add r0, r4, r0
	str r0, [sp]
	mov r0, r8
	mov r1, r7
	mov r2, r6
	mov r3, r4
	strb r5, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02237EAC: .word ov45_02255144
	arm_func_end ov45_02237E04
