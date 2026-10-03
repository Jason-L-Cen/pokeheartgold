	.extern spl_calc_convergence
	.extern spl_calc_gravity
	.extern spl_calc_magnet
	.extern spl_calc_random
	.extern spl_calc_scfield
	.extern spl_calc_spin
	.extern spl_push_front
	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"

.text
	arm_func_start SPL_Load
SPL_Load: ; 0x0209857C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x2c
	mov sb, r1
	ldrh r1, [sb, #8]
	mov sl, r0
	mov r8, #0x20
	strh r1, [sl, #0x30]
	ldrh r0, [sb, #0xa]
	strh r0, [sl, #0x32]
	ldrh r0, [sl, #0x30]
	ldr r1, [sl]
	mov r0, r0, lsl #5
	blx r1
	str r0, [sl, #0x28]
	ldrh r2, [sl, #0x30]
	ldr r0, [sl, #0x28]
	mov r1, #0
	mov r2, r2, lsl #5
	bl MI_CpuFill8
	ldrh r1, [sl, #0x30]
	mov r0, #0
	str r0, [sp]
	cmp r1, #0
	ble _0209881C
	mov r6, r0
	str r0, [sp, #0x10]
	str r0, [sp, #0x14]
	str r0, [sp, #0x18]
	str r0, [sp, #0x1c]
	str r0, [sp, #0x20]
	str r0, [sp, #0x24]
_020985F8:
	ldr r1, [sl, #0x28]
	add r0, sb, r8
	str r0, [r1, r6]
	ldr r0, [r1, r6]
	add r8, r8, #0x58
	ldr r0, [r0]
	add r7, r1, r6
	str r0, [sp, #0x28]
	mov r0, r0, lsl #0x17
	movs r0, r0, lsr #0x1f
	addne r0, sb, r8
	strne r0, [r7, #4]
	ldreq r0, [sp, #0x10]
	addne r8, r8, #0xc
	streq r0, [r7, #4]
	ldr r0, [sp, #0x28]
	mov r0, r0, lsl #0x16
	movs r0, r0, lsr #0x1f
	addne r0, sb, r8
	strne r0, [r7, #8]
	ldreq r0, [sp, #0x14]
	addne r8, r8, #0xc
	streq r0, [r7, #8]
	ldr r0, [sp, #0x28]
	mov r0, r0, lsl #0x15
	movs r0, r0, lsr #0x1f
	addne r0, sb, r8
	strne r0, [r7, #0xc]
	ldreq r0, [sp, #0x18]
	addne r8, r8, #8
	streq r0, [r7, #0xc]
	ldr r0, [sp, #0x28]
	mov r0, r0, lsl #0x14
	movs r0, r0, lsr #0x1f
	addne r0, sb, r8
	strne r0, [r7, #0x10]
	ldreq r0, [sp, #0x1c]
	addne r8, r8, #0xc
	streq r0, [r7, #0x10]
	ldr r0, [sp, #0x28]
	mov r0, r0, lsl #0xf
	movs r0, r0, lsr #0x1f
	addne r0, sb, r8
	strne r0, [r7, #0x14]
	ldreq r0, [sp, #0x20]
	addne r8, r8, #0x14
	streq r0, [r7, #0x14]
	ldr r0, [sp, #0x28]
	mov r1, r0, lsl #7
	mov r2, r0, lsl #6
	mov r5, r1, lsr #0x1f
	mov r4, r2, lsr #0x1f
	add r1, r5, r2, lsr #31
	mov r2, r0, lsl #5
	add r1, r1, r2, lsr #31
	mov r3, r0, lsl #4
	mov fp, r2, lsr #0x1f
	add r2, r1, r3, lsr #31
	mov r1, r3, lsr #0x1f
	mov ip, r0, lsl #3
	str r1, [sp, #4]
	mov r3, r0, lsl #2
	mov r0, ip, lsr #0x1f
	add r1, r2, ip, lsr #31
	str r0, [sp, #8]
	add r0, r1, r3, lsr #31
	strh r0, [r7, #0x1c]
	mov r0, r3, lsr #0x1f
	str r0, [sp, #0xc]
	ldrh r0, [r7, #0x1c]
	cmp r0, #0
	beq _020987F8
	mov r0, r0, lsl #3
	ldr r1, [sl]
	blx r1
	str r0, [r7, #0x18]
	cmp r5, #0
	ldr r0, [r7, #0x18]
	beq _0209874C
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988DC ; =spl_calc_gravity
	add r8, r8, #8
	str r1, [r0]
	add r0, r0, #8
_0209874C:
	cmp r4, #0
	beq _0209876C
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988E0 ; =spl_calc_random
	add r8, r8, #8
	str r1, [r0]
	add r0, r0, #8
_0209876C:
	cmp fp, #0
	beq _0209878C
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988E4 ; =spl_calc_magnet
	add r8, r8, #0x10
	str r1, [r0]
	add r0, r0, #8
_0209878C:
	ldr r1, [sp, #4]
	cmp r1, #0
	beq _020987B0
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988E8 ; =spl_calc_spin
	add r8, r8, #4
	str r1, [r0]
	add r0, r0, #8
_020987B0:
	ldr r1, [sp, #8]
	cmp r1, #0
	beq _020987D4
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988EC ; =spl_calc_scfield
	add r8, r8, #8
	str r1, [r0]
	add r0, r0, #8
_020987D4:
	ldr r1, [sp, #0xc]
	cmp r1, #0
	beq _02098800
	add r1, sb, r8
	str r1, [r0, #4]
	ldr r1, _020988F0 ; =spl_calc_convergence
	add r8, r8, #0x10
	str r1, [r0]
	b _02098800
_020987F8:
	ldr r0, [sp, #0x24]
	str r0, [r7, #0x18]
_02098800:
	ldr r0, [sp]
	ldrh r1, [sl, #0x30]
	add r0, r0, #1
	add r6, r6, #0x20
	str r0, [sp]
	cmp r0, r1
	blt _020985F8
_0209881C:
	ldrh r2, [sl, #0x32]
	mov r0, #0x14
	ldr r1, [sl]
	mul r0, r2, r0
	blx r1
	str r0, [sl, #0x2c]
	ldrh r3, [sl, #0x32]
	mov r1, #0x14
	ldr r0, [sl, #0x2c]
	mul r2, r3, r1
	mov r1, #0
	bl MI_CpuFill8
	ldrh r0, [sl, #0x32]
	mov r3, #0
	cmp r0, #0
	addle sp, sp, #0x2c
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxle lr
	mov r4, r3
	mov r0, #1
_0209886C:
	ldr r2, [sl, #0x2c]
	add r1, sb, r8
	str r1, [r2, r4]
	ldr r5, [r1, #4]
	add r2, r2, r4
	mov r5, r5, lsl #0x18
	mov r5, r5, lsr #0x1c
	add r5, r5, #3
	mov r5, r0, lsl r5
	strh r5, [r2, #0x10]
	ldr r5, [r1, #4]
	add r3, r3, #1
	mov r5, r5, lsl #0x14
	mov r5, r5, lsr #0x1c
	add r5, r5, #3
	mov r5, r0, lsl r5
	strh r5, [r2, #0x12]
	ldr r5, [r1, #4]
	add r4, r4, #0x14
	str r5, [r2, #0xc]
	ldrh r2, [sl, #0x32]
	ldr r1, [r1, #0x1c]
	cmp r3, r2
	add r8, r8, r1
	blt _0209886C
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	.align 2, 0
_020988DC: .word spl_calc_gravity
_020988E0: .word spl_calc_random
_020988E4: .word spl_calc_magnet
_020988E8: .word spl_calc_spin
_020988EC: .word spl_calc_scfield
_020988F0: .word spl_calc_convergence
	arm_func_end SPL_Load
