	.extern _021105DC
	.extern _021105E4
	.extern drawXYPlane
	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"

.text
	arm_func_start spl_set_tex
spl_set_tex: ; 0x0209985C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #4
	ldr r7, [r0, #0xc]
	ldr r3, _02099934 ; =0x040004A8
	str r7, [sp]
	ldr r1, [r0, #4]
	mov r2, r7, lsl #0x1c
	mov lr, r2, lsr #0x1c
	mov r1, r1, lsr #3
	orr ip, r1, lr, lsl #26
	mov r4, r7, lsl #0x18
	mov r5, r7, lsl #0x14
	mov r2, r7, lsl #0xf
	mov r4, r4, lsr #0x1c
	orr ip, ip, #0x40000000
	mov r6, r7, lsl #0x12
	orr ip, ip, r4, lsl #20
	mov r5, r5, lsr #0x1c
	mov r1, r7, lsl #0x10
	orr ip, ip, r5, lsl #23
	mov r4, r6, lsr #0x1e
	mov r5, r1, lsr #0x1e
	orr r1, ip, r4, lsl #16
	mov r2, r2, lsr #0x1f
	orr r1, r1, r5, lsl #18
	orr r1, r1, r2, lsl #29
	str r1, [r3]
	cmp lr, #2
	moveq r1, #1
	movne r1, #0
	ldr r2, [r0, #8]
	rsb r1, r1, #4
	mov r2, r2, lsr r1
	ldr r1, _02099938 ; =0x040004AC
	ldr ip, _0209993C ; =0x04000440
	str r2, [r1]
	mov r2, #3
	ldr r1, _02099940 ; =0x04000454
	str r2, [ip]
	mov r3, #0
	str r3, [r1]
	ldrh r1, [r0, #0x10]
	ldrh r2, [r0, #0x12]
	ldr r0, _02099944 ; =0x0400046C
	mov r1, r1, lsl #0xc
	str r1, [r0]
	mov r1, r2, lsl #0xc
	str r1, [r0]
	str r3, [r0]
	mov r0, #1
	str r0, [ip]
	add sp, sp, #4
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
	.align 2, 0
_02099934: .word 0x040004A8
_02099938: .word 0x040004AC
_0209993C: .word 0x04000440
_02099940: .word 0x04000454
_02099944: .word 0x0400046C
	arm_func_end spl_set_tex
