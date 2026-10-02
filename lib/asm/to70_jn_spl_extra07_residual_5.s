	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public drawXYPlane
	.public drawXZPlane
	.public rotTypeXYZ
	.public spl_gen_child
	.public spl_gen_ptcl
	.extern spl_pop_front
	.extern spl_push_front
	.extern spl_rndm_get_arb_vec_xy
	.extern spl_rndm_get_arb_vec_xyz
	.extern spl_rndm_seed
	.extern spl_set_circle_axis
	.extern spl_set_cross_to_axis

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

	arm_func_start spl_gen_child
spl_gen_child: ; 0x0209C1B0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	ldr r3, [r1, #0x20]
	str r1, [sp]
	ldr r6, [r3, #0x14]
	mov r8, r0
	ldrb r3, [r6, #8]
	ldrb r0, [r6, #0xc]
	str r2, [sp, #4]
	mov r3, r3, lsl #0xc
	mov r2, r3, asr #0x1f
	mov r2, r2, lsl #4
	mov r1, #0x800
	mov r4, r3, lsl #4
	adds r4, r4, r1
	orr r2, r2, r3, lsr #28
	adc r1, r2, #0
	mov r5, r4, lsr #0xc
	cmp r0, #0
	mov r0, #0
	str r0, [sp, #8]
	orr r5, r5, r1, lsl #20
	addle sp, sp, #0x24
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxle lr
	ldr sb, _0209C4C0 ; =spl_rndm_seed
	str r0, [sp, #0x18]
	str r0, [sp, #0x14]
	str r0, [sp, #0x1c]
	str r0, [sp, #0x20]
	mov r0, #0x1000
	mov r4, r5, asr #0x1f
	str r0, [sp, #0x10]
_0209C234:
	ldr r0, [sp, #4]
	bl spl_pop_front
	movs r7, r0
	addeq sp, sp, #0x24
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	ldr r0, [sp]
	mov r1, r7
	add r0, r0, #0x14
	bl spl_push_front
	add r0, r7, #0x38
	add r1, r8, #8
	str r0, [sp, #0xc]
	add sl, r7, #8
	ldmia r1, {r0, r1, r2}
	stmia sl, {r0, r1, r2}
	ldr sl, [r8, #0x14]
	ldr ip, [sb]
	ldr r1, _0209C4C4 ; =0x5EEDF715
	ldr r0, _0209C4C8 ; =0x1B0CB173
	mov r2, sl, asr #0x1f
	mla r0, ip, r1, r0
	str r0, [sb]
	umull lr, ip, sl, r5
	mla ip, sl, r4, ip
	ldrsh r1, [r6, #2]
	mov r0, r0, lsr #0x17
	mla ip, r2, r5, ip
	mul r0, r1, r0
	mov r3, #0x800
	adds r2, lr, r3
	sub r0, r0, r1, lsl #8
	adc sl, ip, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, sl, lsl #20
	add r0, r2, r0, asr #8
	str r0, [r7, #0x14]
	ldr sl, [r8, #0x18]
	ldr ip, [sb]
	ldr r1, _0209C4C4 ; =0x5EEDF715
	ldr r0, _0209C4C8 ; =0x1B0CB173
	mov r2, sl, asr #0x1f
	mla r0, ip, r1, r0
	str r0, [sb]
	umull lr, ip, sl, r5
	mla ip, sl, r4, ip
	mla ip, r2, r5, ip
	adds r2, lr, r3
	ldrsh r1, [r6, #2]
	mov r0, r0, lsr #0x17
	adc sl, ip, #0
	mul r0, r1, r0
	mov r2, r2, lsr #0xc
	sub r0, r0, r1, lsl #8
	orr r2, r2, sl, lsl #20
	add r0, r2, r0, asr #8
	str r0, [r7, #0x18]
	ldr ip, [r8, #0x1c]
	ldr sl, [sb]
	ldr r1, _0209C4C4 ; =0x5EEDF715
	ldr r0, _0209C4C8 ; =0x1B0CB173
	mov r2, ip, asr #0x1f
	mla r0, sl, r1, r0
	umull sl, lr, ip, r5
	str r0, [sb]
	mla lr, ip, r4, lr
	adds r3, sl, r3
	mla lr, r2, r5, lr
	ldrsh r1, [r6, #2]
	mov r0, r0, lsr #0x17
	adc r2, lr, #0
	mul r0, r1, r0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	sub r0, r0, r1, lsl #8
	add r0, r3, r0, asr #8
	ldr r3, [sp, #0xc]
	add fp, r8, #0x38
	str r0, [r7, #0x1c]
	ldmia fp, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldrb r0, [r6, #9]
	ldrsh r1, [r8, #0x34]
	ldr r2, [r8, #0x30]
	add r0, r0, #1
	mul r1, r2, r1
	mov r1, r1, asr #0xc
	mul r0, r1, r0
	mov r0, r0, asr #6
	str r0, [r7, #0x30]
	ldr r0, [sp, #0x10]
	strh r0, [r7, #0x34]
	ldrh r0, [r6]
	mov r0, r0, lsl #0x19
	movs r0, r0, lsr #0x1f
	ldrneh r0, [r6, #0xa]
	strneh r0, [r7, #0x36]
	ldreqh r0, [r8, #0x36]
	streqh r0, [r7, #0x36]
	ldrh r1, [r8, #0x2e]
	ldrh r0, [r7, #0x2e]
	mov r2, r1, lsl #0x1b
	mov r1, r1, lsl #0x16
	mov r1, r1, lsr #0x1b
	mov r2, r2, lsr #0x1b
	add r1, r1, #1
	mul r1, r2, r1
	mov r1, r1, lsl #0xb
	mov r1, r1, lsr #0x10
	bic r0, r0, #0x1f
	and r1, r1, #0x1f
	orr r0, r0, r1
	strh r0, [r7, #0x2e]
	ldrh r0, [r7, #0x2e]
	bic r0, r0, #0x3e0
	orr r0, r0, #0x3e0
	strh r0, [r7, #0x2e]
	ldrh r0, [r6]
	mov r0, r0, lsl #0x1b
	movs r0, r0, lsr #0x1e
	beq _0209C438
	cmp r0, #1
	beq _0209C448
	cmp r0, #2
	ldreqh r0, [r8, #0x20]
	streqh r0, [r7, #0x20]
	ldreqsh r0, [r8, #0x22]
	streqh r0, [r7, #0x22]
	b _0209C458
_0209C438:
	ldr r0, [sp, #0x14]
	strh r0, [r7, #0x20]
	strh r0, [r7, #0x22]
	b _0209C458
_0209C448:
	ldrh r0, [r8, #0x20]
	strh r0, [r7, #0x20]
	ldr r0, [sp, #0x18]
	strh r0, [r7, #0x22]
_0209C458:
	ldrh r1, [r6, #6]
	ldr r0, _0209C4CC ; =0x0000FFFF
	strh r1, [r7, #0x24]
	ldr r1, [sp, #0x1c]
	strh r1, [r7, #0x26]
	ldrb r1, [r6, #0xf]
	strb r1, [r7, #0x2c]
	ldrh r1, [r8, #0x24]
	mov r1, r1, lsr #1
	bl _s32_div_f
	strh r0, [r7, #0x28]
	ldrh r1, [r8, #0x24]
	ldr r0, _0209C4CC ; =0x0000FFFF
	bl _s32_div_f
	strh r0, [r7, #0x2a]
	ldr r0, [sp, #0x20]
	strb r0, [r7, #0x2d]
	ldr r0, [sp, #8]
	ldrb r1, [r6, #0xc]
	add r0, r0, #1
	str r0, [sp, #8]
	cmp r0, r1
	blt _0209C234
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	.align 2, 0
_0209C4C0: .word spl_rndm_seed
_0209C4C4: .word 0x5EEDF715
_0209C4C8: .word 0x1B0CB173
_0209C4CC: .word 0x0000FFFF
	arm_func_end spl_gen_child

	arm_func_start spl_gen_ptcl
spl_gen_ptcl: ; 0x0209C4D0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc4
	mov sb, r0
	ldr r0, [sb, #0x20]
	ldrsh r3, [sb, #0x4e]
	str r0, [sp, #4]
	ldr r0, [sb, #0x58]
	ldr r2, _0209D1DC ; =0x00000FFF
	add r3, r0, r3
	ldr r0, [sp, #4]
	str r1, [sp]
	ldr r8, [r0]
	and r0, r3, r2
	strh r0, [sb, #0x4e]
	ldr r0, [r8]
	mov r0, r0, lsl #0x1c
	mov r1, r0, lsr #0x1c
	mov r0, r3, asr #0xc
	cmp r1, #2
	str r0, [sp, #0xc]
	beq _0209C538
	cmp r1, #3
	beq _0209C538
	sub r0, r1, #5
	cmp r0, #4
	bhi _0209C540
_0209C538:
	mov r0, sb
	bl spl_set_cross_to_axis
_0209C540:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	mov r0, #0
	str r0, [sp, #8]
	addle sp, sp, #0xc4
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxle lr
	str r0, [sp, #0x18]
	str r0, [sp, #0x24]
	str r0, [sp, #0x20]
	str r0, [sp, #0x1c]
	mov r0, #0x1000
	str r0, [sp, #0x34]
	ldr r0, [sp, #8]
	ldr r6, _0209D1E0 ; =spl_rndm_seed
	ldr r4, _0209D1E4 ; =0x5EEDF715
	ldr r5, _0209D1E8 ; =0x1B0CB173
	str r0, [sp, #0x38]
	str r0, [sp, #0x3c]
	str r0, [sp, #0x40]
_0209C590:
	ldr r0, [sp]
	bl spl_pop_front
	movs r7, r0
	addeq sp, sp, #0xc4
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	add r0, sb, #8
	mov r1, r7
	bl spl_push_front
	ldr r0, [r8]
	mov r0, r0, lsl #0x1c
	mov r0, r0, lsr #0x1c
	cmp r0, #9
	addls pc, pc, r0, lsl #2
	b _0209CCD0
_0209C5CC: ; jump table
	b _0209C5F4 ; case 0
	b _0209C610 ; case 1
	b _0209C680 ; case 2
	b _0209C6E8 ; case 3
	b _0209C77C ; case 4
	b _0209C87C ; case 5
	b _0209CB74 ; case 6
	b _0209CBF8 ; case 7
	b _0209C938 ; case 8
	b _0209CA08 ; case 9
_0209C5F4:
	ldr r0, [sp, #0x1c]
	str r0, [r7, #0x10]
	ldr r0, [r7, #0x10]
	str r0, [r7, #0xc]
	ldr r0, [r7, #0xc]
	str r0, [r7, #8]
	b _0209CCD0
_0209C610:
	add r0, r7, #8
	bl spl_rndm_get_arb_vec_xyz
	ldr r2, [r7, #8]
	ldr r1, [sb, #0x5c]
	mov r0, #0x800
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #8]
	ldr r2, [r7, #0xc]
	ldr r1, [sb, #0x5c]
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #0xc]
	ldr r2, [r7, #0x10]
	ldr r1, [sb, #0x5c]
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r0, r1, #0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [r7, #0x10]
	b _0209CCD0
_0209C680:
	add r0, sp, #0x58
	bl spl_rndm_get_arb_vec_xy
	ldr r2, [sp, #0x58]
	ldr r0, [sb, #0x5c]
	mov r1, #0x800
	smull r3, r0, r2, r0
	adds r2, r3, r1
	adc r0, r0, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #0x58]
	ldr r3, [sp, #0x5c]
	ldr r2, [sb, #0x5c]
	add r0, r7, #8
	smull sl, r2, r3, r2
	adds r3, sl, r1
	adc r1, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	ldr r1, [sp, #0x20]
	str r2, [sp, #0x5c]
	str r1, [sp, #0x60]
	add r1, sp, #0x58
	mov r2, sb
	bl spl_set_circle_axis
	b _0209CCD0
_0209C6E8:
	ldr r0, [sp, #0x18]
	ldr r1, [sp, #0xc]
	bl _s32_div_f
	ldr r1, [sp, #0x18]
	mov r0, r0, asr #4
	add r1, r1, #0x10000
	mov r2, r0, lsl #1
	str r1, [sp, #0x18]
	ldr r0, _0209D1EC ; =FX_SinCosTable_
	mov r1, r2, lsl #1
	ldrsh r0, [r0, r1]
	add r1, r2, #1
	mov r2, r1, lsl #1
	ldr r1, _0209D1EC ; =FX_SinCosTable_
	ldrsh r3, [r1, r2]
	ldr r2, [sb, #0x5c]
	mov r1, #0x800
	smull sl, r2, r0, r2
	adds sl, sl, r1
	adc r0, r2, #0
	mov r2, sl, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #0x64]
	ldr r2, [sb, #0x5c]
	add r0, r7, #8
	smull sl, r2, r3, r2
	adds r3, sl, r1
	adc r1, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	ldr r1, [sp, #0x24]
	str r2, [sp, #0x68]
	str r1, [sp, #0x6c]
	add r1, sp, #0x64
	mov r2, sb
	bl spl_set_circle_axis
	b _0209CCD0
_0209C77C:
	add r0, r7, #8
	bl spl_rndm_get_arb_vec_xyz
	ldr r1, [r6]
	mov r0, #0x800
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #8
	ldr r3, [r7, #8]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #8]
	ldr r1, [r6]
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #8
	ldr r3, [r7, #0xc]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #0xc]
	ldr r1, [r6]
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #8
	ldr r3, [r7, #0x10]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r0, r1, #0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [r7, #0x10]
	b _0209CCD0
_0209C87C:
	add r0, sp, #0x70
	bl spl_rndm_get_arb_vec_xy
	ldr r0, [r6]
	ldr r3, [sp, #0x70]
	mla r1, r0, r4, r5
	str r1, [r6]
	ldr r0, [sb, #0x5c]
	mov r2, #0x800
	smull sl, r0, r3, r0
	adds sl, sl, r2
	adc r3, r0, #0
	mov r0, sl, lsr #0xc
	orr r0, r0, r3, lsl #20
	mov r3, r1, lsr #0x17
	mov r3, r3, lsl #0xc
	sub r3, r3, #0x100000
	mov r3, r3, asr #8
	smull sl, r3, r0, r3
	adds sl, sl, r2
	adc r0, r3, #0
	mov r3, sl, lsr #0xc
	orr r3, r3, r0, lsl #20
	mla r0, r1, r4, r5
	str r0, [r6]
	str r3, [sp, #0x70]
	mov r0, r0, lsr #0x17
	mov r0, r0, lsl #0xc
	sub r0, r0, #0x100000
	mov r1, r0, asr #8
	ldr sl, [sp, #0x74]
	ldr r3, [sb, #0x5c]
	add r0, r7, #8
	smull fp, r3, sl, r3
	adds sl, fp, r2
	adc r3, r3, #0
	mov sl, sl, lsr #0xc
	orr sl, sl, r3, lsl #20
	smull r3, r1, sl, r1
	adds r2, r3, r2
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x74]
	add r1, sp, #0x70
	mov r2, sb
	bl spl_set_circle_axis
	b _0209CCD0
_0209C938:
	add r0, r7, #8
	bl spl_rndm_get_arb_vec_xyz
	add r0, sb, #0x84
	add r1, sb, #0x8a
	add r2, sp, #0x44
	bl VEC_Fx16CrossProduct
	ldrsh r1, [sp, #0x46]
	ldrsh r0, [sp, #0x48]
	ldrsh r2, [sp, #0x44]
	str r1, [sp, #0x80]
	str r0, [sp, #0x84]
	add r0, sp, #0x7c
	add r1, r7, #8
	str r2, [sp, #0x7c]
	bl VEC_DotProduct
	cmp r0, #0
	bgt _0209C9A0
	ldr r0, [r7, #8]
	rsb r0, r0, #0
	str r0, [r7, #8]
	ldr r0, [r7, #0xc]
	rsb r0, r0, #0
	str r0, [r7, #0xc]
	ldr r0, [r7, #0x10]
	rsb r0, r0, #0
	str r0, [r7, #0x10]
_0209C9A0:
	ldr r2, [r7, #8]
	ldr r1, [sb, #0x5c]
	mov r0, #0x800
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #8]
	ldr r2, [r7, #0xc]
	ldr r1, [sb, #0x5c]
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #0xc]
	ldr r2, [r7, #0x10]
	ldr r1, [sb, #0x5c]
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r0, r1, #0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [r7, #0x10]
	b _0209CCD0
_0209CA08:
	add r0, r7, #8
	bl spl_rndm_get_arb_vec_xyz
	add r0, sb, #0x84
	add r1, sb, #0x8a
	add r2, sp, #0x4a
	bl VEC_Fx16CrossProduct
	ldrsh r1, [sp, #0x4c]
	ldrsh r0, [sp, #0x4e]
	ldrsh r2, [sp, #0x4a]
	str r1, [sp, #0x8c]
	str r0, [sp, #0x90]
	add r0, sp, #0x88
	add r1, r7, #8
	str r2, [sp, #0x88]
	bl VEC_DotProduct
	cmp r0, #0
	bge _0209CA70
	ldr r0, [r7, #8]
	rsb r0, r0, #0
	str r0, [r7, #8]
	ldr r0, [r7, #0xc]
	rsb r0, r0, #0
	str r0, [r7, #0xc]
	ldr r0, [r7, #0x10]
	rsb r0, r0, #0
	str r0, [r7, #0x10]
_0209CA70:
	ldr r1, [r6]
	mov r0, #0x800
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #9
	add r1, r1, #0x800
	ldr r3, [r7, #8]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #8]
	ldr r1, [r6]
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #9
	add r1, r1, #0x800
	ldr r3, [r7, #0xc]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [r7, #0xc]
	ldr r1, [r6]
	mla r2, r1, r4, r5
	str r2, [r6]
	mov r1, r2, lsr #0x17
	mov r1, r1, lsl #0xc
	sub r1, r1, #0x100000
	mov r1, r1, asr #9
	add r1, r1, #0x800
	ldr r3, [r7, #0x10]
	ldr r2, [sb, #0x5c]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	smull r2, r1, r3, r1
	adds r2, r2, r0
	adc r0, r1, #0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [r7, #0x10]
	b _0209CCD0
_0209CB74:
	add r0, r7, #0x14
	bl spl_rndm_get_arb_vec_xy
	ldr r2, [r7, #0x14]
	ldr r1, [sb, #0x5c]
	mov r0, #0x800
	smull r3, r1, r2, r1
	adds r2, r3, r0
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x94]
	ldr r3, [r7, #0x18]
	ldr r2, [sb, #0x5c]
	ldr r1, [r6]
	smull sl, r2, r3, r2
	adds r3, sl, r0
	adc r0, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	mla r0, r1, r4, r5
	str r0, [r6]
	str r2, [sp, #0x98]
	mov r1, r0, lsr #0x17
	ldr r2, [sb, #0x60]
	add r0, r7, #8
	mul r1, r2, r1
	sub r1, r1, r2, lsl #8
	mov r1, r1, asr #8
	str r1, [sp, #0x9c]
	add r1, sp, #0x94
	mov r2, sb
	bl spl_set_circle_axis
	b _0209CCD0
_0209CBF8:
	add r0, r7, #0x14
	bl spl_rndm_get_arb_vec_xy
	ldr r0, [r6]
	mov r3, #0x800
	mla r2, r0, r4, r5
	str r2, [r6]
	ldr sl, [r7, #0x14]
	ldr r0, [sb, #0x5c]
	mov r1, r2, lsr #0x17
	smull fp, r0, sl, r0
	adds fp, fp, r3
	mov r1, r1, lsl #0xc
	adc sl, r0, #0
	mov r0, fp, lsr #0xc
	sub r1, r1, #0x100000
	orr r0, r0, sl, lsl #20
	mov r1, r1, asr #8
	smull sl, r1, r0, r1
	adds sl, sl, r3
	adc r0, r1, #0
	mov r1, sl, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0xa0]
	mla r1, r2, r4, r5
	str r1, [r6]
	mov r2, r1, lsr #0x17
	mov r2, r2, lsl #0xc
	sub r2, r2, #0x100000
	ldr sl, [r7, #0x18]
	ldr r0, [sb, #0x5c]
	mov r2, r2, asr #8
	smull fp, r0, sl, r0
	adds fp, fp, r3
	adc sl, r0, #0
	mov r0, fp, lsr #0xc
	orr r0, r0, sl, lsl #20
	smull sl, r2, r0, r2
	adds r3, sl, r3
	adc r0, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	mla r0, r1, r4, r5
	str r0, [r6]
	str r2, [sp, #0xa4]
	mov r1, r0, lsr #0x17
	ldr r2, [sb, #0x60]
	add r0, r7, #8
	mul r1, r2, r1
	sub r1, r1, r2, lsl #8
	mov r1, r1, asr #8
	str r1, [sp, #0xa8]
	add r1, sp, #0xa0
	mov r2, sb
	bl spl_set_circle_axis
_0209CCD0:
	ldr r1, [r6]
	mla r0, r1, r4, r5
	str r0, [r6]
	ldrb r2, [r8, #0x42]
	mov r1, r0, lsr #0x18
	ldr r3, [sb, #0x64]
	mul r1, r2, r1
	add r2, r2, #0xff
	sub r1, r2, r1, asr #7
	mul r1, r3, r1
	mov r1, r1, asr #8
	str r1, [sp, #0x10]
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x18
	ldrb r2, [r8, #0x42]
	ldr r3, [sb, #0x68]
	ldr r1, [r8]
	mul r0, r2, r0
	add r2, r2, #0xff
	sub r0, r2, r0, asr #7
	mul r0, r3, r0
	mov r0, r0, asr #8
	str r0, [sp, #0x14]
	mov r0, r1, lsl #0x1c
	mov r0, r0, lsr #0x1c
	cmp r0, #6
	bne _0209CE1C
	ldrsh r0, [sb, #0x84]
	ldr r1, [r7, #0x14]
	mov r2, #0x800
	smull r3, r0, r1, r0
	adds r1, r3, r2
	ldrsh r3, [sb, #0x8a]
	ldr lr, [r7, #0x18]
	mov ip, r1, lsr #0xc
	smull sl, r3, lr, r3
	adc r0, r0, #0
	adds sl, sl, r2
	orr ip, ip, r0, lsl #20
	adc r3, r3, #0
	mov sl, sl, lsr #0xc
	orr sl, sl, r3, lsl #20
	add r3, ip, sl
	str r3, [sp, #0xb8]
	ldrsh sl, [sb, #0x86]
	ldr r3, [r7, #0x14]
	ldrsh lr, [sb, #0x8c]
	smull ip, sl, r3, sl
	adds ip, ip, r2
	ldr fp, [r7, #0x18]
	adc sl, sl, #0
	smull r3, lr, fp, lr
	mov ip, ip, lsr #0xc
	adds r3, r3, r2
	orr ip, ip, sl, lsl #20
	adc sl, lr, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, sl, lsl #20
	add r3, ip, r3
	str r3, [sp, #0xbc]
	ldr sl, [r7, #0x18]
	ldrsh fp, [sb, #0x88]
	ldr r3, [r7, #0x14]
	str sl, [sp, #0x28]
	smull ip, fp, r3, fp
	ldrsh sl, [sb, #0x8e]
	ldr r3, [sp, #0x28]
	adds ip, ip, r2
	smull sl, lr, r3, sl
	adc r3, fp, #0
	adds r2, sl, r2
	mov sl, ip, lsr #0xc
	orr sl, sl, r3, lsl #20
	adc r3, lr, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r3, lsl #20
	add r2, sl, r2
	add r0, sp, #0xb8
	add r1, sp, #0xac
	str r2, [sp, #0xc0]
	bl VEC_Normalize
	b _0209CE58
_0209CE1C:
	ldr r0, [r7, #8]
	cmp r0, #0
	bne _0209CE4C
	ldr r0, [r7, #0xc]
	cmp r0, #0
	bne _0209CE4C
	ldr r0, [r7, #0x10]
	cmp r0, #0
	bne _0209CE4C
	add r0, sp, #0xac
	bl spl_rndm_get_arb_vec_xyz
	b _0209CE58
_0209CE4C:
	add r0, r7, #8
	add r1, sp, #0xac
	bl VEC_Normalize
_0209CE58:
	ldr r2, [sp, #0xac]
	ldr r1, [sp, #0x10]
	mov r0, #0x800
	smull sl, r1, r2, r1
	adds r2, sl, r0
	add sl, sb, #0x28
	str sl, [sp, #0x2c]
	add sl, r7, #0x38
	str sl, [sp, #0x30]
	adc r1, r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	ldrsh r1, [sb, #0x50]
	ldr sl, [sp, #0x14]
	ldr r3, [sb, #0x40]
	smull fp, sl, r1, sl
	adds fp, fp, r0
	adc r1, sl, #0
	mov sl, fp, lsr #0xc
	orr sl, sl, r1, lsl #20
	add r1, r2, sl
	add r1, r3, r1
	str r1, [r7, #0x14]
	ldr r3, [sp, #0xb0]
	ldr r1, [sp, #0x10]
	ldrsh sl, [sb, #0x52]
	smull fp, r1, r3, r1
	adds fp, fp, r0
	adc r3, r1, #0
	mov r1, fp, lsr #0xc
	orr r1, r1, r3, lsl #20
	ldr r3, [sp, #0x14]
	ldr r2, [sb, #0x44]
	smull fp, r3, sl, r3
	adds sl, fp, r0
	adc r3, r3, #0
	mov sl, sl, lsr #0xc
	orr sl, sl, r3, lsl #20
	add r1, r1, sl
	add r1, r2, r1
	str r1, [r7, #0x18]
	ldr r1, [sp, #0xb4]
	ldr r2, [sp, #0x10]
	ldrsh lr, [sb, #0x54]
	smull sl, r3, r1, r2
	ldr r1, [sp, #0x14]
	adds sl, sl, r0
	smull r2, r1, lr, r1
	adc r3, r3, #0
	adds r0, r2, r0
	mov r2, sl, lsr #0xc
	orr r2, r2, r3, lsl #20
	adc r1, r1, #0
	mov r0, r0, lsr #0xc
	orr r0, r0, r1, lsl #20
	ldr ip, [sb, #0x48]
	add r0, r2, r0
	add r0, ip, r0
	str r0, [r7, #0x1c]
	ldr r0, [sp, #0x2c]
	ldr r3, [sp, #0x30]
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, [r6]
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x18
	ldrb r1, [r8, #0x40]
	ldr r2, [sb, #0x6c]
	mul r0, r1, r0
	add r1, r1, #0xff
	sub r0, r1, r0, asr #7
	mul r0, r2, r0
	mov r0, r0, asr #8
	str r0, [r7, #0x30]
	ldr r0, [sp, #0x34]
	strh r0, [r7, #0x34]
	ldr r0, [r8]
	mov r0, r0, lsl #0x16
	movs r0, r0, lsr #0x1f
	beq _0209D010
	ldr r0, [sp, #4]
	ldr r0, [r0, #8]
	ldrh r0, [r0, #8]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	beq _0209D010
	ldr r0, [r6]
	ldr r3, _0209D1F0 ; =0x00000003
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x14
	ldr r1, [sp, #4]
	ldr r2, [r1, #8]
	ldr r1, _0209D1F4 ; =0xAAAAAAAB
	umull r1, sl, r0, r1
	mov sl, sl, lsr #1
	umull sl, fp, r3, sl
	ldrh r1, [r2]
	sub sl, r0, sl
	strh r1, [sp, #0x50]
	ldrh r0, [r8, #0x22]
	mov r1, sl, lsl #1
	strh r0, [sp, #0x52]
	ldrh r0, [r2, #2]
	strh r0, [sp, #0x54]
	add r0, sp, #0x50
	ldrh r0, [r0, r1]
	strh r0, [r7, #0x36]
	b _0209D018
_0209D010:
	ldrh r0, [r8, #0x22]
	strh r0, [r7, #0x36]
_0209D018:
	ldrh r1, [r7, #0x2e]
	ldrb r0, [sb, #0x81]
	bic r1, r1, #0x1f
	and r0, r0, #0x1f
	orr r0, r1, r0
	strh r0, [r7, #0x2e]
	ldrh r0, [r7, #0x2e]
	bic r0, r0, #0x3e0
	orr r0, r0, #0x3e0
	strh r0, [r7, #0x2e]
	ldr r0, [r8]
	mov r0, r0, lsl #0x12
	movs r0, r0, lsr #0x1f
	ldrne r0, [r6]
	mlane r1, r0, r4, r5
	strne r1, [r6]
	strneh r1, [r7, #0x20]
	ldreqh r0, [sb, #0x56]
	streqh r0, [r7, #0x20]
	ldr r0, [r8]
	mov r0, r0, lsl #0x13
	movs r0, r0, lsr #0x1f
	ldreq r0, [sp, #0x38]
	streqh r0, [r7, #0x22]
	beq _0209D0A8
	ldr r0, [r6]
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x14
	ldrsh r2, [r8, #0x34]
	ldrsh r1, [r8, #0x36]
	sub r1, r1, r2
	mul r0, r1, r0
	add r0, r0, r2, lsl #12
	mov r0, r0, lsr #0xc
	strh r0, [r7, #0x22]
_0209D0A8:
	ldr r0, [r6]
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x18
	ldrb r1, [r8, #0x41]
	ldrh r2, [sb, #0x70]
	mul r0, r1, r0
	mov r0, r0, asr #8
	rsb r0, r0, #0xff
	mul r0, r2, r0
	mov r0, r0, asr #8
	add r0, r0, #1
	strh r0, [r7, #0x24]
	ldr r0, [sp, #0x3c]
	strh r0, [r7, #0x26]
	ldr r0, [r8]
	mov r0, r0, lsl #0x14
	movs r1, r0, lsr #0x1f
	beq _0209D138
	ldr r0, [sp, #4]
	ldr r0, [r0, #0x10]
	ldr r0, [r0, #8]
	mov r0, r0, lsl #0xf
	movs r0, r0, lsr #0x1f
	beq _0209D138
	ldr r0, [r6]
	mla r1, r0, r4, r5
	str r1, [r6]
	mov r0, r1, lsr #0x14
	ldr r1, [sp, #4]
	ldr sl, [r1, #0x10]
	ldrb r1, [sl, #8]
	bl _u32_div_f
	ldrb r0, [sl, r1]
	strb r0, [r7, #0x2c]
	b _0209D168
_0209D138:
	cmp r1, #0
	beq _0209D160
	ldr r0, [sp, #4]
	ldr r1, [r0, #0x10]
	ldr r0, [r1, #8]
	mov r0, r0, lsl #0xf
	movs r0, r0, lsr #0x1f
	ldreqb r0, [r1]
	streqb r0, [r7, #0x2c]
	beq _0209D168
_0209D160:
	ldrb r0, [r8, #0x47]
	strb r0, [r7, #0x2c]
_0209D168:
	ldr r1, [sp, #4]
	ldr r0, _0209D1F8 ; =0x0000FFFF
	ldr r1, [r1]
	ldrb r1, [r1, #0x48]
	bl _s32_div_f
	strh r0, [r7, #0x28]
	ldrh r1, [r7, #0x24]
	ldr r0, _0209D1F8 ; =0x0000FFFF
	bl _s32_div_f
	strh r0, [r7, #0x2a]
	ldr r0, [sp, #0x40]
	strb r0, [r7, #0x2d]
	ldr r0, [r8]
	mov r0, r0, lsl #0xb
	movs r0, r0, lsr #0x1f
	ldrne r0, [r6]
	mlane r1, r0, r4, r5
	strne r1, [r6]
	movne r0, r1, lsr #0x18
	strneb r0, [r7, #0x2d]
	ldr r0, [sp, #8]
	add r1, r0, #1
	ldr r0, [sp, #0xc]
	str r1, [sp, #8]
	cmp r1, r0
	blt _0209C590
	add sp, sp, #0xc4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	.align 2, 0
_0209D1DC: .word 0x00000FFF
_0209D1E0: .word spl_rndm_seed
_0209D1E4: .word 0x5EEDF715
_0209D1E8: .word 0x1B0CB173
_0209D1EC: .word FX_SinCosTable_
_0209D1F0: .word 0x00000003
_0209D1F4: .word 0xAAAAAAAB
_0209D1F8: .word 0x0000FFFF
	arm_func_end spl_gen_ptcl
