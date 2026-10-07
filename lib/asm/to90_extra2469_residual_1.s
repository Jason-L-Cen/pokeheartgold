	.extern spl_pop_front
	.extern spl_push_front
	.extern spl_rndm_get_arb_vec_xy
	.extern spl_rndm_get_arb_vec_xyz
	.extern spl_rndm_seed
	.extern spl_set_circle_axis
	.extern spl_set_cross_to_axis
	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"

.text
	arm_func_start rotTypeXYZ
rotTypeXYZ: ; 0x0209BF8C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #4
	ldr r3, _0209C030 ; =0x00000555
	rsb r7, r1, #0x1000
	ldr ip, _0209C034 ; =0x0000093D
	mov r4, #0
	umull sb, r8, r7, r3
	umull r6, r5, r0, ip
	mov lr, #0x800
	adds sb, sb, lr
	mla r8, r7, r4, r8
	mov r7, r7, asr #0x1f
	mla r8, r7, r3, r8
	adc r7, r8, #0
	mov r3, sb, lsr #0xc
	adds r6, r6, lr
	orr r3, r3, r7, lsl #20
	add r1, r3, r1
	mla r5, r0, r4, r5
	mov r0, r0, asr #0x1f
	mla r5, r0, ip, r5
	adc r0, r5, #0
	mov r5, r6, lsr #0xc
	orr r5, r5, r0, lsl #20
	str r1, [r2]
	add ip, r3, r5
	str ip, [r2, #0xc]
	sub r0, r3, r5
	str r0, [r2, #0x18]
	str r4, [r2, #0x24]
	str r0, [r2, #4]
	str r1, [r2, #0x10]
	str ip, [r2, #0x1c]
	str r4, [r2, #0x28]
	str ip, [r2, #8]
	str r0, [r2, #0x14]
	str r1, [r2, #0x20]
	str r4, [r2, #0x2c]
	add sp, sp, #4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, lr}
	bx lr
	.align 2, 0
_0209C030: .word 0x00000555
_0209C034: .word 0x0000093D
	arm_func_end rotTypeXYZ

	arm_func_start drawXZPlane
drawXZPlane: ; 0x0209C038
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub r7, r2, #0x1000
	add r5, r3, #0x1000
	add r4, r2, #0x1000
	mov r2, r0, lsl #8
	mov r0, r1, lsl #8
	sub r3, r3, #0x1000
	mov r0, r0, lsr #0x10
	mov r1, r2, lsr #0x10
	ldr ip, _0209C0E0 ; =0x04000500
	mov r8, #1
	ldr r6, _0209C0E4 ; =0x000003FF
	mov r2, r7, lsl #0x10
	mov lr, r5, lsl #0x10
	mov r4, r4, lsl #0x10
	mov r5, r3, lsl #0x10
	ldr r3, _0209C0E8 ; =0x04000488
	str r8, [ip]
	mov ip, #0
	and r2, r6, r2, asr #22
	and r7, r6, lr, asr #22
	and lr, r6, r4, asr #22
	and r4, r6, r5, asr #22
	ldr r6, _0209C0EC ; =0x04000490
	str ip, [r3]
	orr r5, r2, r7, lsl #20
	str r5, [r6]
	str r1, [r3]
	orr r5, lr, r7, lsl #20
	str r5, [r6]
	orr r1, r1, r0, lsl #16
	str r1, [r3]
	orr r1, lr, r4, lsl #20
	str r1, [r6]
	mov r0, r0, lsl #0x10
	str r0, [r3]
	orr r1, r2, r4, lsl #20
	ldr r0, _0209C0F0 ; =0x04000504
	str r1, [r6]
	str ip, [r0]
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_0209C0E0: .word 0x04000500
_0209C0E4: .word 0x000003FF
_0209C0E8: .word 0x04000488
_0209C0EC: .word 0x04000490
_0209C0F0: .word 0x04000504
	arm_func_end drawXZPlane

	arm_func_start drawXYPlane
drawXYPlane: ; 0x0209C0F4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	add ip, r3, #0x1000
	sub r5, r2, #0x1000
	add r4, r2, #0x1000
	mov r2, r0, lsl #8
	mov r0, r1, lsl #8
	sub r3, r3, #0x1000
	mov r0, r0, lsr #0x10
	mov r1, r2, lsr #0x10
	ldr r7, _0209C19C ; =0x04000500
	mov r8, #1
	ldr r6, _0209C1A0 ; =0x000003FF
	mov r2, ip, lsl #0x10
	mov lr, r5, lsl #0x10
	mov r4, r4, lsl #0x10
	mov r5, r3, lsl #0x10
	ldr r3, _0209C1A4 ; =0x04000488
	str r8, [r7]
	mov ip, #0
	and r7, r6, r2, asr #22
	and r2, r6, lr, asr #22
	and lr, r6, r4, asr #22
	and r4, r6, r5, asr #22
	ldr r6, _0209C1A8 ; =0x04000490
	str ip, [r3]
	orr r5, r2, r7, lsl #10
	str r5, [r6]
	str r1, [r3]
	orr r5, lr, r7, lsl #10
	str r5, [r6]
	orr r1, r1, r0, lsl #16
	str r1, [r3]
	orr r1, lr, r4, lsl #10
	str r1, [r6]
	mov r0, r0, lsl #0x10
	str r0, [r3]
	orr r1, r2, r4, lsl #10
	ldr r0, _0209C1AC ; =0x04000504
	str r1, [r6]
	str ip, [r0]
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_0209C19C: .word 0x04000500
_0209C1A0: .word 0x000003FF
_0209C1A4: .word 0x04000488
_0209C1A8: .word 0x04000490
_0209C1AC: .word 0x04000504
	arm_func_end drawXYPlane
