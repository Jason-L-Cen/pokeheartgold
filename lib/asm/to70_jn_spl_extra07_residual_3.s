	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public sDrawParent
	.public spl_calc
	.public spl_init
	.extern spl_alp_in_out
	.extern spl_chld_alp_out
	.extern spl_chld_scl_out
	.extern spl_clr_in_out
	.extern spl_del
	.extern spl_draw_bb
	.extern spl_draw_dbb
	.extern spl_draw_dpl
	.extern spl_draw_pol
	.extern spl_gen_child
	.extern spl_gen_ptcl
	.extern spl_push_front
	.extern spl_scl_in_out
	.extern spl_set_tex
	.extern spl_set_tex_dummy
	.extern spl_tex_ptn_anm

.text
	arm_func_start sDrawParent
sDrawParent: ; 0x02098C58
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	ldr r6, [r7, #0x40]
	ldr r2, [r7, #0x2c]
	ldr r1, [r6, #0x20]
	mov r0, #0x14
	ldr r5, [r1]
	mov r4, #0
	ldrb r1, [r5, #0x47]
	mla r0, r1, r0, r2
	bl spl_set_tex
	ldr r0, [r5]
	mov r0, r0, lsl #0x1a
	mov r0, r0, lsr #0x1e
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	b _02098CD4
_02098C9C: ; jump table
	b _02098CB0 ; case 0
	b _02098CB8 ; case 1
	b _02098CC0 ; case 2
	b _02098CC8 ; case 3
	b _02098CD0 ; case 4
_02098CB0:
	ldr r4, _02098D2C ; =spl_draw_bb
	b _02098CD4
_02098CB8:
	ldr r4, _02098D30 ; =spl_draw_dbb
	b _02098CD4
_02098CC0:
	ldr r4, _02098D34 ; =spl_draw_pol
	b _02098CD4
_02098CC8:
	ldr r4, _02098D38 ; =spl_draw_dpl
	b _02098CD4
_02098CD0:
	ldr r4, _02098D38 ; =spl_draw_dpl
_02098CD4:
	ldr r0, [r5]
	ldr r8, [r6, #8]
	mov r0, r0, lsl #0x14
	movs r0, r0, lsr #0x1f
	ldrne r5, _02098D3C ; =spl_set_tex
	ldreq r5, _02098D40 ; =spl_set_tex_dummy
	cmp r8, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	bxeq lr
	mov r6, #0x14
_02098CFC:
	ldrb r1, [r8, #0x2c]
	ldr r0, [r7, #0x2c]
	mla r0, r1, r6, r0
	blx r5
	mov r0, r7
	mov r1, r8
	blx r4
	ldr r8, [r8]
	cmp r8, #0
	bne _02098CFC
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_02098D2C: .word spl_draw_bb
_02098D30: .word spl_draw_dbb
_02098D34: .word spl_draw_pol
_02098D38: .word spl_draw_dpl
_02098D3C: .word spl_set_tex
_02098D40: .word spl_set_tex_dummy
	arm_func_end sDrawParent

	arm_func_start spl_calc
spl_calc: ; 0x02098D44
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x74
	mov sb, r1
	ldr r8, [sb, #0x20]
	mov sl, r0
	ldr r0, [r8, #0x14]
	ldr r4, [r8]
	str r0, [sp, #4]
	ldr r0, [r4]
	mov r6, #0
	str r0, [sp, #0x24]
	ldrb r0, [r4, #0x46]
	ldr r2, [sb, #0x90]
	ldrh r7, [r8, #0x1c]
	add r0, r0, #0x180
	cmp r2, #0
	str r0, [sp, #8]
	beq _02098D98
	mov r0, sb
	mov r1, r6
	blx r2
_02098D98:
	ldrh r1, [r4, #0x3c]
	cmp r1, #0
	beq _02098DB0
	ldrh r0, [sb, #0x4c]
	cmp r0, r1
	bhs _02098DF8
_02098DB0:
	ldrh r0, [sb, #0x4c]
	ldrb r1, [sb, #0x80]
	bl _s32_div_f
	cmp r1, #0
	bne _02098DF8
	ldr r1, [sb, #0x24]
	mov r0, r1, lsl #0x1f
	movs r0, r0, lsr #0x1f
	bne _02098DF8
	mov r0, r1, lsl #0x1e
	movs r0, r0, lsr #0x1f
	bne _02098DF8
	mov r0, r1, lsl #0x1b
	movs r0, r0, lsr #0x1f
	beq _02098DF8
	mov r0, sb
	add r1, sl, #0x1c
	bl spl_gen_ptcl
_02098DF8:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x17
	movs r0, r0, lsr #0x1f
	beq _02098E28
	ldr r0, _02099574 ; =spl_scl_in_out
	add r6, r6, #1
	str r0, [sp, #0x28]
	ldr r0, [r8, #4]
	ldrh r0, [r0, #8]
	mov r0, r0, lsl #0x1f
	mov r0, r0, lsr #0x1f
	str r0, [sp, #0x2c]
_02098E28:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x16
	movs r0, r0, lsr #0x1f
	beq _02098E74
	ldr r3, [r8, #8]
	ldrh r0, [r3, #8]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	bne _02098E74
	ldr r2, _02099578 ; =spl_clr_in_out
	add r1, sp, #0x28
	str r2, [r1, r6, lsl #3]
	ldrh r1, [r3, #8]
	mov r2, r6, lsl #3
	add r0, sp, #0x2c
	mov r1, r1, lsl #0x1e
	mov r1, r1, lsr #0x1f
	add r6, r6, #1
	str r1, [r0, r2]
_02098E74:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x15
	movs r0, r0, lsr #0x1f
	beq _02098EB0
	ldr r2, _0209957C ; =spl_alp_in_out
	add r1, sp, #0x28
	str r2, [r1, r6, lsl #3]
	ldr r1, [r8, #0xc]
	mov r2, r6, lsl #3
	ldrh r1, [r1, #2]
	add r0, sp, #0x2c
	add r6, r6, #1
	mov r1, r1, lsl #0x17
	mov r1, r1, lsr #0x1f
	str r1, [r0, r2]
_02098EB0:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x14
	movs r0, r0, lsr #0x1f
	beq _02098EF8
	ldr r3, [r8, #0x10]
	ldr r0, [r3, #8]
	mov r0, r0, lsl #0xf
	movs r0, r0, lsr #0x1f
	bne _02098EF8
	ldr r2, _02099580 ; =spl_tex_ptn_anm
	add r1, sp, #0x28
	str r2, [r1, r6, lsl #3]
	ldr r1, [r3, #8]
	add r0, sp, #0x2c
	mov r1, r1, lsl #0xe
	mov r1, r1, lsr #0x1f
	str r1, [r0, r6, lsl #3]
	add r6, r6, #1
_02098EF8:
	ldr r4, [sb, #8]
	cmp r4, #0
	beq _02099234
	ldr r0, [sp, #0x24]
	add fp, sp, #0x28
	mov r2, r0, lsl #0x10
	mov r1, r0, lsl #0xf
	mov r0, r2, lsr #0x1f
	str r0, [sp, #0xc]
	mov r0, r1, lsr #0x1f
	str r0, [sp, #0x10]
	mov r0, #0
	str r0, [sp, #0x18]
_02098F2C:
	ldr r0, [r4]
	ldrh r1, [r4, #0x2a]
	str r0, [sp]
	ldrh r0, [r4, #0x26]
	cmp r6, #0
	ldr r5, [sp, #0x18]
	mul r2, r1, r0
	mov r1, r2, asr #8
	strb r1, [sp, #0x20]
	ldrh r1, [r4, #0x28]
	ldrb r2, [r4, #0x2d]
	mul r0, r1, r0
	add r0, r2, r0, asr #8
	strb r0, [sp, #0x21]
	ble _02098F94
_02098F68:
	add r2, fp, r5, lsl #3
	ldr ip, [r2, #4]
	add r2, sp, #0x20
	ldrb r2, [r2, ip]
	ldr r3, [fp, r5, lsl #3]
	mov r0, r4
	mov r1, r8
	blx r3
	add r5, r5, #1
	cmp r5, r6
	blt _02098F68
_02098F94:
	ldr r0, [sp, #0x18]
	ldr r5, [sp, #0x18]
	str r0, [sp, #0x70]
	str r0, [sp, #0x6c]
	str r0, [sp, #0x68]
	ldr r0, [sp, #0xc]
	cmp r0, #0
	addne r0, sb, #0x28
	addne r3, r4, #0x38
	ldmneia r0, {r0, r1, r2}
	stmneia r3, {r0, r1, r2}
	cmp r7, #0
	ble _02098FF4
_02098FC8:
	ldr r2, [r8, #0x18]
	mov r1, r4
	add r0, r2, r5, lsl #3
	ldr ip, [r2, r5, lsl #3]
	ldr r0, [r0, #4]
	add r2, sp, #0x68
	mov r3, sb
	blx ip
	add r5, r5, #1
	cmp r5, r7
	blt _02098FC8
_02098FF4:
	ldr r0, [sp, #0x10]
	ldrh r1, [r4, #0x20]
	cmp r0, #0
	ldrsh r0, [r4, #0x22]
	add r0, r1, r0
	strh r0, [r4, #0x20]
	ldr r1, [r4, #0x14]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r4, #0x14]
	ldr r1, [r4, #0x18]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r4, #0x18]
	ldr r1, [r4, #0x1c]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r4, #0x1c]
	ldr r1, [r4, #0x14]
	ldr r0, [sp, #0x68]
	add r0, r1, r0
	str r0, [r4, #0x14]
	ldr r1, [r4, #0x18]
	ldr r0, [sp, #0x6c]
	add r0, r1, r0
	str r0, [r4, #0x18]
	ldr r1, [r4, #0x1c]
	ldr r0, [sp, #0x70]
	add r0, r1, r0
	str r0, [r4, #0x1c]
	ldr r1, [r4, #0x14]
	ldr r0, [sb, #0x34]
	ldr r2, [r4, #8]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r4, #8]
	ldr r1, [r4, #0x18]
	ldr r0, [sb, #0x38]
	ldr r2, [r4, #0xc]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r4, #0xc]
	ldr r1, [r4, #0x1c]
	ldr r0, [sb, #0x3c]
	ldr r2, [r4, #0x10]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r4, #0x10]
	beq _0209912C
	ldr r0, [sp, #4]
	ldrh r2, [r4, #0x24]
	ldrb r1, [r0, #0xd]
	ldrh r3, [r4, #0x26]
	mov r2, r2, lsl #0xc
	mov r1, r1, lsl #0xc
	mov r0, r3, lsl #0xc
	smull r5, r3, r2, r1
	mov r1, #0x800
	adds r2, r5, r1
	adc r1, r3, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	mov r1, r2, asr #8
	subs r0, r0, r1
	bmi _0209912C
	ldr r1, [sp, #4]
	mov r0, r0, asr #0xc
	ldrb r1, [r1, #0xe]
	bl _s32_div_f
	cmp r1, #0
	bne _0209912C
	mov r0, r4
	mov r1, sb
	add r2, sl, #0x1c
	bl spl_gen_child
_0209912C:
	ldr r0, [sb, #0x20]
	ldr r0, [r0]
	ldr r0, [r0]
	mov r0, r0, lsl #1
	movs r0, r0, lsr #0x1f
	beq _02099170
	ldrh r0, [r4, #0x2e]
	ldr r1, [sl, #0x38]
	bic r0, r0, #0xfc00
	mov r1, r1, lsl #8
	mov r1, r1, lsr #0x1a
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	and r1, r1, #0x3f
	orr r0, r0, r1, lsl #10
	strh r0, [r4, #0x2e]
	b _020991F0
_02099170:
	ldrh r0, [r4, #0x2e]
	ldr r2, [sl, #0x38]
	add r1, sl, #0x38
	bic r0, r0, #0xfc00
	mov r2, r2, lsl #0xe
	mov r2, r2, lsr #0x1a
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	and r2, r2, #0x3f
	orr r0, r0, r2, lsl #10
	strh r0, [r4, #0x2e]
	ldr r2, [sl, #0x38]
	bic r0, r2, #0x3f000
	mov r2, r2, lsl #0xe
	mov r2, r2, lsr #0x1a
	add r2, r2, #1
	and r2, r2, #0x3f
	orr r0, r0, r2, lsl #12
	str r0, [sl, #0x38]
	ldr r2, [sl, #0x38]
	mov r0, r2, lsl #0xe
	mov r3, r0, lsr #0x1a
	mov r0, r2, lsl #0x14
	cmp r3, r0, lsr #26
	bls _020991F0
	mov r0, r2, lsl #0x1a
	mov r0, r0, lsr #0x1a
	and r0, r0, #0x3f
	ldr r2, [r1]
	bic r2, r2, #0x3f000
	orr r0, r2, r0, lsl #12
	str r0, [r1]
_020991F0:
	ldrh r0, [r4, #0x26]
	add r0, r0, #1
	strh r0, [r4, #0x26]
	ldrh r1, [r4, #0x26]
	ldrh r0, [r4, #0x24]
	cmp r1, r0
	bls _02099224
	mov r1, r4
	add r0, sb, #8
	bl spl_del
	mov r1, r0
	add r0, sl, #0x1c
	bl spl_push_front
_02099224:
	ldr r0, [sp]
	cmp r0, #0
	mov r4, r0
	bne _02098F2C
_02099234:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0xf
	movs r0, r0, lsr #0x1f
	beq _0209953C
	ldr r0, [sp, #4]
	mov r6, #0
	ldrh r0, [r0]
	mov r0, r0, lsl #0x1e
	movs r0, r0, lsr #0x1f
	ldrne r0, _02099584 ; =spl_chld_scl_out
	strne r6, [sp, #0x4c]
	strne r0, [sp, #0x48]
	ldr r0, [sp, #4]
	addne r6, r6, #1
	ldrh r0, [r0]
	mov r0, r0, lsl #0x1d
	movs r0, r0, lsr #0x1f
	beq _02099298
	ldr r2, _02099588 ; =spl_chld_alp_out
	add r1, sp, #0x48
	str r2, [r1, r6, lsl #3]
	add r0, sp, #0x4c
	mov r1, #0
	str r1, [r0, r6, lsl #3]
	add r6, r6, #1
_02099298:
	ldr r0, [sp, #4]
	ldr r5, [sb, #0x14]
	ldrh r0, [r0]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	moveq r7, #0
	cmp r5, #0
	beq _0209953C
	mov r0, #0
	str r0, [sp, #0x1c]
_020992C0:
	ldrh r2, [r5, #0x26]
	ldr r0, [r5]
	ldrh r1, [r5, #0x24]
	str r0, [sp, #0x14]
	mov r0, r2, lsl #8
	bl _s32_div_f
	ldr r4, [sp, #0x1c]
	strb r0, [sp, #0x20]
	cmp r6, #0
	ble _02099310
	ldrb fp, [sp, #0x20]
_020992EC:
	add r3, sp, #0x48
	ldr r3, [r3, r4, lsl #3]
	mov r0, r5
	mov r1, r8
	mov r2, fp
	blx r3
	add r4, r4, #1
	cmp r4, r6
	blt _020992EC
_02099310:
	ldr r0, [sp, #0x1c]
	ldr r4, [sp, #0x1c]
	str r0, [sp, #0x70]
	str r0, [sp, #0x6c]
	str r0, [sp, #0x68]
	ldr r0, [sp, #4]
	ldrh r0, [r0]
	mov r0, r0, lsl #0x1a
	movs r0, r0, lsr #0x1f
	addne r0, sb, #0x28
	addne r3, r5, #0x38
	ldmneia r0, {r0, r1, r2}
	stmneia r3, {r0, r1, r2}
	cmp r7, #0
	ble _02099378
_0209934C:
	ldr r2, [r8, #0x18]
	mov r1, r5
	add r0, r2, r4, lsl #3
	ldr fp, [r2, r4, lsl #3]
	ldr r0, [r0, #4]
	add r2, sp, #0x68
	mov r3, sb
	blx fp
	add r4, r4, #1
	cmp r4, r7
	blt _0209934C
_02099378:
	ldrh r1, [r5, #0x20]
	ldrsh r0, [r5, #0x22]
	add r0, r1, r0
	strh r0, [r5, #0x20]
	ldr r1, [r5, #0x14]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r5, #0x14]
	ldr r1, [r5, #0x18]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r5, #0x18]
	ldr r1, [r5, #0x1c]
	ldr r0, [sp, #8]
	mul r0, r1, r0
	mov r0, r0, asr #9
	str r0, [r5, #0x1c]
	ldr r1, [r5, #0x14]
	ldr r0, [sp, #0x68]
	add r0, r1, r0
	str r0, [r5, #0x14]
	ldr r1, [r5, #0x18]
	ldr r0, [sp, #0x6c]
	add r0, r1, r0
	str r0, [r5, #0x18]
	ldr r1, [r5, #0x1c]
	ldr r0, [sp, #0x70]
	add r0, r1, r0
	str r0, [r5, #0x1c]
	ldr r1, [r5, #0x14]
	ldr r0, [sb, #0x34]
	ldr r2, [r5, #8]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r5, #8]
	ldr r1, [r5, #0x18]
	ldr r0, [sb, #0x38]
	ldr r2, [r5, #0xc]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r5, #0xc]
	ldr r1, [r5, #0x1c]
	ldr r0, [sb, #0x3c]
	ldr r2, [r5, #0x10]
	add r0, r1, r0
	add r0, r2, r0
	str r0, [r5, #0x10]
	ldr r0, [sb, #0x20]
	ldr r0, [r0]
	ldr r0, [r0]
	movs r0, r0, lsr #0x1f
	beq _0209947C
	ldr r0, [sl, #0x38]
	ldrh r1, [r5, #0x2e]
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x1a
	bic r1, r1, #0xfc00
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	and r0, r0, #0x3f
	orr r0, r1, r0, lsl #10
	strh r0, [r5, #0x2e]
	b _020994FC
_0209947C:
	ldr r0, [sl, #0x38]
	ldrh r1, [r5, #0x2e]
	mov r0, r0, lsl #0xe
	mov r0, r0, lsr #0x1a
	bic r1, r1, #0xfc00
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	and r0, r0, #0x3f
	orr r0, r1, r0, lsl #10
	strh r0, [r5, #0x2e]
	ldr r1, [sl, #0x38]
	add r0, sl, #0x38
	bic r2, r1, #0x3f000
	mov r1, r1, lsl #0xe
	mov r1, r1, lsr #0x1a
	add r1, r1, #1
	and r1, r1, #0x3f
	orr r1, r2, r1, lsl #12
	str r1, [sl, #0x38]
	ldr r2, [sl, #0x38]
	mov r1, r2, lsl #0xe
	mov r3, r1, lsr #0x1a
	mov r1, r2, lsl #0x14
	cmp r3, r1, lsr #26
	bls _020994FC
	mov r1, r2, lsl #0x1a
	mov r1, r1, lsr #0x1a
	and r1, r1, #0x3f
	ldr r2, [r0]
	bic r2, r2, #0x3f000
	orr r1, r2, r1, lsl #12
	str r1, [r0]
_020994FC:
	ldrh r0, [r5, #0x26]
	add r0, r0, #1
	strh r0, [r5, #0x26]
	ldrh r1, [r5, #0x26]
	ldrh r0, [r5, #0x24]
	cmp r1, r0
	bls _02099530
	mov r1, r5
	add r0, sb, #0x14
	bl spl_del
	mov r1, r0
	add r0, sl, #0x1c
	bl spl_push_front
_02099530:
	ldr r5, [sp, #0x14]
	movs r0, r5
	bne _020992C0
_0209953C:
	ldrh r0, [sb, #0x4c]
	add r0, r0, #1
	strh r0, [sb, #0x4c]
	ldr r2, [sb, #0x90]
	cmp r2, #0
	addeq sp, sp, #0x74
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	mov r0, sb
	mov r1, #1
	blx r2
	add sp, sp, #0x74
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	.align 2, 0
_02099574: .word spl_scl_in_out
_02099578: .word spl_clr_in_out
_0209957C: .word spl_alp_in_out
_02099580: .word spl_tex_ptn_anm
_02099584: .word spl_chld_scl_out
_02099588: .word spl_chld_alp_out
	arm_func_end spl_calc

	arm_func_start spl_init
spl_init: ; 0x0209958C
	stmdb sp!, {r4, r5, r6, lr}
	str r1, [r0, #0x20]
	mov r3, #0
	str r3, [r0, #0x24]
	ldr r1, [r0, #0x20]
	ldr r5, [r2]
	ldr r1, [r1]
	ldr r4, _02099850 ; =0x00007FFF
	ldr r1, [r1, #4]
	ldr ip, _02099854 ; =0x0007FFFF
	add r1, r5, r1
	str r1, [r0, #0x28]
	ldr r1, [r0, #0x20]
	ldr r6, [r2, #4]
	ldr r1, [r1]
	mov lr, #0x80000000
	ldr r5, [r1, #8]
	mov r1, #0x1000
	add r5, r6, r5
	str r5, [r0, #0x2c]
	ldr r5, [r0, #0x20]
	ldr r6, [r2, #8]
	ldr r2, [r5]
	ldr r2, [r2, #0xc]
	add r2, r6, r2
	str r2, [r0, #0x30]
	str r3, [r0, #0x40]
	str r3, [r0, #0x44]
	str r3, [r0, #0x48]
	str r3, [r0, #0x3c]
	ldr r2, [r0, #0x3c]
	str r2, [r0, #0x38]
	ldr r2, [r0, #0x38]
	str r2, [r0, #0x34]
	strh r3, [r0, #0x4c]
	strh r3, [r0, #0x4e]
	ldr r2, [r0, #0x20]
	ldr r5, [r2]
	ldrh r3, [r5, #0x1c]
	ldrh r2, [r5, #0x1e]
	strh r3, [r0, #0x50]
	strh r2, [r0, #0x52]
	ldrh r2, [r5, #0x20]
	strh r2, [r0, #0x54]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldrh r2, [r2, #0x38]
	strh r2, [r0, #0x56]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x10]
	str r2, [r0, #0x58]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x14]
	str r2, [r0, #0x5c]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x18]
	str r2, [r0, #0x60]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x24]
	str r2, [r0, #0x64]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x28]
	str r2, [r0, #0x68]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x2c]
	str r2, [r0, #0x6c]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldrh r2, [r2, #0x3e]
	strh r2, [r0, #0x70]
	strh r4, [r0, #0x72]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldrb r2, [r2, #0x44]
	strb r2, [r0, #0x80]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldrb r2, [r2, #0x45]
	strb r2, [r0, #0x81]
	ldr r2, [r0, #0x80]
	bic r2, r2, #0x70000
	str r2, [r0, #0x80]
	ldr r2, [r0, #0x80]
	and r2, r2, ip
	str r2, [r0, #0x80]
	str lr, [r0, #0x74]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x48]
	mov r2, r2, lsl #6
	mov r2, r2, lsr #0x1e
	mov r2, r1, lsl r2
	strh r2, [r0, #0x78]
	ldr r2, [r0, #0x20]
	ldr r2, [r2]
	ldr r2, [r2, #0x48]
	mov r2, r2, lsl #4
	mov r2, r2, lsr #0x1e
	mov r1, r1, lsl r2
	strh r1, [r0, #0x7a]
	ldr r1, [r0, #0x20]
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrnesh r2, [r0, #0x78]
	mvnne r1, #0
	smulbbne r1, r2, r1
	strneh r1, [r0, #0x78]
	ldr r1, [r0, #0x20]
	ldr r1, [r1]
	ldr r1, [r1, #0x4c]
	mov r1, r1, lsl #0x1e
	movs r1, r1, lsr #0x1f
	ldrnesh r2, [r0, #0x7a]
	mvnne r1, #0
	smulbbne r1, r2, r1
	strneh r1, [r0, #0x7a]
	ldr r2, [r0, #0x20]
	ldr r1, [r2]
	ldr r1, [r1]
	mov r1, r1, lsl #0xf
	movs r1, r1, lsr #0x1f
	beq _02099814
	ldr r1, [r2, #0x14]
	mov r2, #0x1000
	ldr r1, [r1, #0x10]
	mov r1, r1, lsl #0x1e
	mov r1, r1, lsr #0x1e
	mov r1, r2, lsl r1
	strh r1, [r0, #0x7c]
	ldr r1, [r0, #0x20]
	ldr r1, [r1, #0x14]
	ldr r1, [r1, #0x10]
	mov r1, r1, lsl #0x1c
	mov r1, r1, lsr #0x1e
	mov r1, r2, lsl r1
	strh r1, [r0, #0x7e]
	ldr r1, [r0, #0x20]
	ldr r1, [r1, #0x14]
	ldr r1, [r1, #0x10]
	mov r1, r1, lsl #0x1b
	movs r1, r1, lsr #0x1f
	ldrnesh r2, [r0, #0x7c]
	mvnne r1, #0
	smulbbne r1, r2, r1
	strneh r1, [r0, #0x7c]
	ldr r1, [r0, #0x20]
	ldr r1, [r1, #0x14]
	ldr r1, [r1, #0x10]
	mov r1, r1, lsl #0x1a
	movs r1, r1, lsr #0x1f
	ldrnesh r2, [r0, #0x7e]
	mvnne r1, #0
	smulbbne r1, r2, r1
	strneh r1, [r0, #0x7e]
_02099814:
	mov r2, #0
	str r2, [r0, #4]
	ldr r1, [r0, #4]
	str r1, [r0]
	str r2, [r0, #0x14]
	ldr r1, [r0, #0x14]
	str r1, [r0, #8]
	str r2, [r0, #0x18]
	ldr r1, [r0, #0x18]
	str r1, [r0, #0xc]
	str r2, [r0, #0x90]
	str r2, [r0, #0x94]
	str r2, [r0, #0x98]
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
	.align 2, 0
_02099850: .word 0x00007FFF
_02099854: .word 0x0007FFFF
	arm_func_end spl_init
