	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public spl_alp_in_out
	.public spl_clr_in_out
	.public spl_tex_ptn_anm
	.extern spl_rndm_seed

.text
	arm_func_start spl_tex_ptn_anm
spl_tex_ptn_anm: ; 0x0209D688
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #4
	ldr r5, [r1, #0x10]
	mov r4, #0
	ldrb r3, [r5, #8]
	cmp r3, #0
	addle sp, sp, #4
	ldmleia sp!, {r4, r5, lr}
	bxle lr
	ldrb ip, [r5, #9]
	mov lr, r4
_0209D6B4:
	add r1, lr, ip
	cmp r2, r1
	ldrltb r1, [r5, r4]
	addlt sp, sp, #4
	strltb r1, [r0, #0x2c]
	ldmltia sp!, {r4, r5, lr}
	bxlt lr
	add r4, r4, #1
	cmp r4, r3
	add lr, lr, ip
	blt _0209D6B4
	add sp, sp, #4
	ldmia sp!, {r4, r5, lr}
	bx lr
	arm_func_end spl_tex_ptn_anm

	arm_func_start spl_alp_in_out
spl_alp_in_out: ; 0x0209D6EC
	stmdb sp!, {r4, r5, r6, lr}
	ldr r4, [r1, #0xc]
	mov r5, r0
	ldrb r1, [r4, #4]
	ldrb r3, [r4, #5]
	cmp r2, r1
	bge _0209D72C
	ldrh r3, [r4]
	mov r0, r3, lsl #0x16
	mov r6, r3, lsl #0x1b
	mov r0, r0, lsr #0x1b
	sub r0, r0, r6, lsr #27
	mul r0, r2, r0
	bl _s32_div_f
	add r0, r0, r6, lsr #27
	b _0209D768
_0209D72C:
	cmp r2, r3
	ldrlth r0, [r4]
	movlt r0, r0, lsl #0x16
	movlt r0, r0, lsr #0x1b
	blt _0209D768
	ldrh r0, [r4]
	sub r2, r2, #0xff
	rsb r1, r3, #0xff
	mov r6, r0, lsl #0x11
	mov r3, r6, lsr #0x1b
	mov r0, r0, lsl #0x16
	sub r0, r3, r0, lsr #27
	mul r0, r2, r0
	bl _s32_div_f
	add r0, r0, r6, lsr #27
_0209D768:
	ldr r3, _0209D7BC ; =spl_rndm_seed
	ldr r1, _0209D7C0 ; =0x5EEDF715
	ldr ip, [r3]
	ldr r2, _0209D7C4 ; =0x1B0CB173
	mla r1, ip, r1, r2
	str r1, [r3]
	ldrb r2, [r4, #2]
	mov r1, r1, lsr #0x18
	ldrh r3, [r5, #0x2e]
	mul r1, r2, r1
	mov r1, r1, asr #8
	rsb r1, r1, #0xff
	mul r1, r0, r1
	mov r0, r1, lsl #8
	mov r0, r0, lsr #0x10
	bic r1, r3, #0x3e0
	and r0, r0, #0x1f
	orr r0, r1, r0, lsl #5
	strh r0, [r5, #0x2e]
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
	.align 2, 0
_0209D7BC: .word spl_rndm_seed
_0209D7C0: .word 0x5EEDF715
_0209D7C4: .word 0x1B0CB173
	arm_func_end spl_alp_in_out

	arm_func_start spl_clr_in_out
spl_clr_in_out: ; 0x0209D7C8
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #4
	ldr r4, [r1, #8]
	mov sl, r0
	ldrb r3, [r4, #4]
	ldr r0, [r1]
	ldrb r1, [r4, #5]
	cmp r2, r3
	ldrb fp, [r4, #6]
	ldrlth r0, [r4]
	addlt sp, sp, #4
	strlth r0, [sl, #0x36]
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxlt lr
	cmp r2, r1
	bge _0209D8C0
	ldrh sb, [r0, #0x22]
	ldrh r0, [r4, #8]
	ldrh r8, [r4]
	mov r7, sb, asr #5
	mov fp, sb, asr #0xa
	mov r4, r0, lsl #0x1d
	mov r6, r8, asr #5
	mov r5, r8, asr #0xa
	movs r4, r4, lsr #0x1f
	and sb, sb, #0x1f
	and r7, r7, #0x1f
	and r0, fp, #0x1f
	orreq r1, sb, r7, lsl #5
	orreq r0, r1, r0, lsl #10
	and r8, r8, #0x1f
	and r6, r6, #0x1f
	and r5, r5, #0x1f
	addeq sp, sp, #4
	streqh r0, [sl, #0x36]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	sub r4, r2, r3
	sub r0, r0, r5
	mul r0, r4, r0
	sub fp, r1, r3
	mov r1, fp
	bl _s32_div_f
	sub r1, sb, r8
	mov sb, r0
	mul r0, r4, r1
	mov r1, fp
	bl _s32_div_f
	sub r1, r7, r6
	mov r7, r0
	mul r0, r4, r1
	mov r1, fp
	bl _s32_div_f
	add r1, r8, r7
	add r0, r6, r0
	add r2, r5, sb
	orr r0, r1, r0, lsl #5
	orr r0, r0, r2, lsl #10
	add sp, sp, #4
	strh r0, [sl, #0x36]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
_0209D8C0:
	cmp r2, fp
	bge _0209D980
	ldrh r7, [r4, #2]
	ldrh r8, [r0, #0x22]
	ldrh r0, [r4, #8]
	mov r5, r7, asr #5
	mov r4, r7, asr #0xa
	mov r3, r0, lsl #0x1d
	mov r6, r8, asr #5
	mov sb, r8, asr #0xa
	movs r3, r3, lsr #0x1f
	and r7, r7, #0x1f
	and r5, r5, #0x1f
	and r0, r4, #0x1f
	orreq r1, r7, r5, lsl #5
	orreq r0, r1, r0, lsl #10
	and r8, r8, #0x1f
	and r6, r6, #0x1f
	and sb, sb, #0x1f
	addeq sp, sp, #4
	streqh r0, [sl, #0x36]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	sub r4, r2, r1
	sub r0, r0, sb
	mul r0, r4, r0
	sub fp, fp, r1
	mov r1, fp
	bl _s32_div_f
	sub r1, r7, r8
	mov r7, r0
	mul r0, r4, r1
	mov r1, fp
	bl _s32_div_f
	sub r1, r5, r6
	mov r5, r0
	mul r0, r4, r1
	mov r1, fp
	bl _s32_div_f
	add r1, r8, r5
	add r0, r6, r0
	add r2, sb, r7
	orr r0, r1, r0, lsl #5
	orr r0, r0, r2, lsl #10
	add sp, sp, #4
	strh r0, [sl, #0x36]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
_0209D980:
	ldrh r0, [r4, #2]
	strh r0, [sl, #0x36]
	add sp, sp, #4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	arm_func_end spl_clr_in_out
