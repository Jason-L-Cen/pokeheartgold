	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern _021092E0
	.extern _021092EC

.text
	arm_func_start NNS_G2dSetupSoftwareSpriteCamera
NNS_G2dSetupSoftwareSpriteCamera: ; 0x020B7350
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	ldr r2, _020B7400 ; =0xBFFF0000
	ldr r1, _020B7404 ; =0x04000580
	mov r0, r2, lsl #6
	str r2, [r1]
	str r0, [sp]
	mov r1, #0x400000
	str r1, [sp, #4]
	mov r0, #0
	str r1, [sp, #8]
	mov r1, #1
	str r1, [sp, #0xc]
	mov r2, r0
	mov r1, #0xc0000
	mov r3, #0x100000
	str r0, [sp, #0x10]
	bl G3i_OrthoW_
	ldr r1, _020B7408 ; =0x0400044C
	mov r5, #0
	ldr r0, _020B740C ; =_021092E0
	str r5, [r1]
	add r4, sp, #0x2c
	ldr ip, _020B7410 ; =_021092EC
	add lr, sp, #0x20
	ldmia r0, {r0, r1, r2}
	stmia lr, {r0, r1, r2}
	add r3, sp, #0x14
	ldmia ip, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	mov r2, r3
	str r5, [r4]
	str r5, [r4, #4]
	str r5, [r4, #8]
	mov r0, r4
	mov r1, lr
	mov r3, #1
	str r5, [sp]
	bl G3i_LookAt_
	ldr r0, _020B7414 ; =0x04000440
	mov r1, #1
	str r1, [r0]
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020B7400: .word 0xBFFF0000
_020B7404: .word 0x04000580
_020B7408: .word 0x0400044C
_020B740C: .word _021092E0
_020B7410: .word _021092EC
_020B7414: .word 0x04000440
	arm_func_end NNS_G2dSetupSoftwareSpriteCamera
