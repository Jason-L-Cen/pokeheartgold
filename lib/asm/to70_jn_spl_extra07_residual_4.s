	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public spl_draw_bb
	.public spl_draw_chld_bb
	.public spl_draw_chld_dbb
	.public spl_draw_chld_dpl
	.public spl_draw_chld_pol
	.public spl_draw_dbb
	.public spl_draw_dpl
	.public spl_draw_pol
	.public spl_set_tex
	.extern _021105DC
	.extern _021105E4
	.extern drawXYPlane

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

	arm_func_start spl_draw_chld_dpl
spl_draw_chld_dpl: ; 0x02099948
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xf0
	mov r4, r1
	ldrh r2, [r4, #0x2e]
	mov r5, r0
	ldr r0, [r5, #0x3c]
	mov r1, r2, lsl #0x16
	mov r3, r2, lsl #0x1b
	mov r1, r1, lsr #0x1b
	mov r3, r3, lsr #0x1b
	add r1, r1, #1
	mul r6, r3, r1
	mov r1, r2, lsl #0x10
	movs r2, r6, asr #5
	mov r1, r1, lsr #0x1a
	orr r0, r0, #0xc0
	orr r1, r0, r1, lsl #24
	ldr r0, _02099DA4 ; =0x040004A4
	orr r1, r1, r2, lsl #16
	str r1, [r0]
	addeq sp, sp, #0xf0
	ldr r0, [r0]
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	bxeq lr
	ldr r0, [r5, #0x40]
	ldrh r1, [r4, #0x20]
	ldr r0, [r0, #0x20]
	ldr r6, _02099DA8 ; =FX_SinCosTable_
	ldr r0, [r0, #0x14]
	mov r1, r1, asr #4
	ldrh r0, [r0]
	mov r2, r1, lsl #1
	add r1, r2, #1
	mov r7, r2, lsl #1
	mov r1, r1, lsl #1
	mov r0, r0, lsl #0x15
	mov r3, r0, lsr #0x1e
	ldr r2, _02099DAC ; =_021105E4
	ldrsh r0, [r6, r7]
	ldr r3, [r2, r3, lsl #2]
	ldrsh r1, [r6, r1]
	add r2, sp, #0x30
	blx r3
	add r0, sp, #0xc0
	bl MTX_Identity43_
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r0, [r0, #0x14]
	ldr r0, [r0, #0x10]
	mov r0, r0, lsl #0x19
	movs r0, r0, lsr #0x1f
	bne _02099A28
	add r1, sp, #0x90
	add r0, r4, #0x14
	bl VEC_Normalize
	b _02099A58
_02099A28:
	add r1, sp, #0x90
	add r0, r4, #8
	bl VEC_Normalize
	ldr r2, [sp, #0x90]
	ldr r1, [sp, #0x94]
	ldr r0, [sp, #0x98]
	rsb r2, r2, #0
	rsb r1, r1, #0
	rsb r0, r0, #0
	str r2, [sp, #0x90]
	str r1, [sp, #0x94]
	str r0, [sp, #0x98]
_02099A58:
	mov r3, #0
	mov r2, #0x1000
	add r0, sp, #0x90
	add r1, sp, #0xb4
	str r3, [sp, #0xb4]
	str r2, [sp, #0xb8]
	str r3, [sp, #0xbc]
	bl VEC_DotProduct
	ldr r1, _02099DB0 ; =0x00000CCD
	cmp r0, r1
	bgt _02099A90
	ldr r1, _02099DB4 ; =0xFFFFF333
	cmp r0, r1
	bge _02099AA4
_02099A90:
	mov r0, #0
	mov r1, #0x1000
	str r1, [sp, #0xb4]
	str r0, [sp, #0xb8]
	str r0, [sp, #0xbc]
_02099AA4:
	add r0, sp, #0x90
	add r1, sp, #0xb4
	add r2, sp, #0x9c
	bl VEC_CrossProduct
	add r0, sp, #0x90
	add r1, sp, #0x9c
	add r2, sp, #0xa8
	bl VEC_CrossProduct
	ldr r7, [sp, #0x9c]
	ldr r6, [sp, #0xa0]
	ldr r3, [sp, #0xa4]
	ldr r0, [sp, #0x90]
	ldr r1, [sp, #0x94]
	ldr r2, [sp, #0x98]
	str r7, [sp, #0xc0]
	ldr r7, [sp, #0xa8]
	str r6, [sp, #0xc4]
	ldr r6, [sp, #0xac]
	str r3, [sp, #0xc8]
	ldr r3, [sp, #0xb0]
	str r0, [sp, #0xcc]
	str r1, [sp, #0xd0]
	add r0, sp, #0x30
	str r2, [sp, #0xd4]
	add r1, sp, #0xc0
	mov r2, r0
	str r7, [sp, #0xd8]
	str r6, [sp, #0xdc]
	str r3, [sp, #0xe0]
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0x30]
	ldr r0, [r0, #0x20]
	mov ip, #0x800
	ldr r0, [r0]
	mov r3, r2, asr #0x1f
	ldrsh r1, [r0, #0x30]
	ldr r0, [r0, #0x48]
	mov lr, #0
	smull r6, r1, r2, r1
	adds r7, r6, ip
	adc r6, r1, #0
	mov r0, r0, lsl #1
	mov r1, r7, lsr #0xc
	orr r1, r1, r6, lsl #20
	movs r0, r0, lsr #0x1d
	beq _02099B74
	cmp r0, #1
	beq _02099BB0
	cmp r0, #2
	beq _02099BCC
	b _02099BF0
_02099B74:
	ldrsh r0, [r4, #0x34]
	mov r8, r0, asr #0x1f
	umull r7, r6, r2, r0
	mla r6, r2, r8, r6
	smull r8, r2, r1, r0
	adds r1, r8, ip
	adc r8, r2, lr
	adds r2, r7, ip
	mla r6, r3, r0, r6
	mov r1, r1, lsr #0xc
	adc r0, r6, lr
	mov r2, r2, lsr #0xc
	orr r1, r1, r8, lsl #20
	orr r2, r2, r0, lsl #20
	b _02099BF0
_02099BB0:
	ldrsh r0, [r4, #0x34]
	smull r3, r0, r1, r0
	adds r1, r3, ip
	adc r0, r0, lr
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	b _02099BF0
_02099BCC:
	ldrsh r6, [r4, #0x34]
	mov r0, r6, asr #0x1f
	umull r8, r7, r2, r6
	mla r7, r2, r0, r7
	adds r2, r8, ip
	mla r7, r3, r6, r7
	adc r0, r7, lr
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
_02099BF0:
	add r0, sp, #0x60
	mov r3, r2
	bl MTX_Scale43_
	add r0, sp, #0x30
	add r1, sp, #0x60
	add r2, sp, #0
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r3, [r0]
	ldr r0, [r3]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _02099C6C
	ldr r1, [r4, #8]
	ldr r0, [r4, #0x38]
	add r0, r1, r0
	str r0, [sp, #0x24]
	ldr r1, [r4, #0xc]
	ldr r0, [r4, #0x3c]
	add r0, r1, r0
	str r0, [sp, #0x28]
	ldr r1, [r4, #0x10]
	ldr r0, [r4, #0x40]
	add r0, r1, r0
	str r0, [sp, #0x2c]
	ldr r0, [r5, #0x44]
	bl G3_LoadMtx43
	add r0, sp, #0
	bl G3_MultMtx43
	b _02099D10
_02099C6C:
	ldr r2, [r4, #8]
	ldr r1, [r4, #0x38]
	ldr r0, [r3, #4]
	add r1, r2, r1
	sub r0, r1, r0
	str r0, [sp, #0x24]
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r4, #0x3c]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #8]
	ldr r0, _02099DB8 ; =0x04000454
	sub r1, r2, r1
	str r1, [sp, #0x28]
	ldr r1, [r5, #0x40]
	ldr r3, [r4, #0x10]
	ldr r1, [r1, #0x20]
	ldr r2, [r4, #0x40]
	ldr r1, [r1]
	add r3, r3, r2
	ldr r2, [r1, #0xc]
	mov r1, #0
	sub r2, r3, r2
	str r2, [sp, #0x2c]
	str r1, [r0]
	ldr r1, [r5, #0x40]
	ldr r0, _02099DBC ; =0x04000470
	ldr r1, [r1, #0x20]
	ldr r1, [r1]
	ldr r3, [r1, #0xc]
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r1, [r0]
	str r2, [r0]
	str r3, [r0]
	ldr r0, [r5, #0x44]
	bl G3_MultMtx43
	add r0, sp, #0
	bl G3_MultMtx43
_02099D10:
	ldr r0, [r5, #0x40]
	ldrh r4, [r4, #0x36]
	ldrh ip, [r0, #0x72]
	mov r2, #0
	and r1, r4, #0x1f
	and r0, ip, #0x1f
	mul r3, r1, r0
	and r1, r4, #0x3e0
	and r0, ip, #0x3e0
	mul r0, r1, r0
	and r4, r4, #0x7c00
	and r1, ip, #0x7c00
	mul ip, r4, r1
	mov r1, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, ip, asr #0x19
	orr r0, r1, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	ldr r1, _02099DC0 ; =0x04000480
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r5, [r5, #0x40]
	ldr r4, _02099DC4 ; =_021105DC
	ldr r1, [r5, #0x20]
	ldrsh r0, [r5, #0x7c]
	ldr r3, [r1, #0x14]
	ldrsh r1, [r5, #0x7e]
	ldrh r5, [r3]
	mov r3, r2
	mov r5, r5, lsl #0x14
	mov r5, r5, lsr #0x1f
	ldr r4, [r4, r5, lsl #2]
	blx r4
	add sp, sp, #0xf0
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_02099DA4: .word 0x040004A4
_02099DA8: .word FX_SinCosTable_
_02099DAC: .word _021105E4
_02099DB0: .word 0x00000CCD
_02099DB4: .word 0xFFFFF333
_02099DB8: .word 0x04000454
_02099DBC: .word 0x04000470
_02099DC0: .word 0x04000480
_02099DC4: .word _021105DC
	arm_func_end spl_draw_chld_dpl

	arm_func_start spl_draw_dpl
spl_draw_dpl: ; 0x02099DC8
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xf0
	mov r4, r1
	ldrh r2, [r4, #0x2e]
	mov r5, r0
	ldr r0, [r5, #0x3c]
	mov r1, r2, lsl #0x16
	mov r3, r2, lsl #0x1b
	mov r1, r1, lsr #0x1b
	mov r3, r3, lsr #0x1b
	add r1, r1, #1
	mul r6, r3, r1
	mov r1, r2, lsl #0x10
	movs r2, r6, asr #5
	mov r1, r1, lsr #0x1a
	orr r0, r0, #0xc0
	orr r1, r0, r1, lsl #24
	ldr r0, _0209A220 ; =0x040004A4
	orr r1, r1, r2, lsl #16
	str r1, [r0]
	addeq sp, sp, #0xf0
	ldr r0, [r0]
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	bxeq lr
	ldr r0, [r5, #0x40]
	ldrh r1, [r4, #0x20]
	ldr r0, [r0, #0x20]
	ldr r6, _0209A224 ; =FX_SinCosTable_
	ldr r0, [r0]
	mov r1, r1, asr #4
	mov r2, r1, lsl #1
	ldr r0, [r0]
	add r1, r2, #1
	mov r7, r2, lsl #1
	mov r1, r1, lsl #1
	mov r0, r0, lsl #0xd
	mov r3, r0, lsr #0x1e
	ldr r2, _0209A228 ; =_021105E4
	ldrsh r0, [r6, r7]
	ldr r3, [r2, r3, lsl #2]
	ldrsh r1, [r6, r1]
	add r2, sp, #0x30
	blx r3
	add r0, sp, #0xc0
	bl MTX_Identity43_
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r0, [r0]
	ldr r0, [r0, #0x48]
	movs r0, r0, lsr #0x1f
	bne _02099EA4
	add r1, sp, #0x90
	add r0, r4, #0x14
	bl VEC_Normalize
	b _02099ED4
_02099EA4:
	add r1, sp, #0x90
	add r0, r4, #8
	bl VEC_Normalize
	ldr r2, [sp, #0x90]
	ldr r1, [sp, #0x94]
	ldr r0, [sp, #0x98]
	rsb r2, r2, #0
	rsb r1, r1, #0
	rsb r0, r0, #0
	str r2, [sp, #0x90]
	str r1, [sp, #0x94]
	str r0, [sp, #0x98]
_02099ED4:
	mov r3, #0
	mov r2, #0x1000
	add r0, sp, #0x90
	add r1, sp, #0xb4
	str r3, [sp, #0xb4]
	str r2, [sp, #0xb8]
	str r3, [sp, #0xbc]
	bl VEC_DotProduct
	ldr r1, _0209A22C ; =0x00000CCD
	cmp r0, r1
	bgt _02099F0C
	ldr r1, _0209A230 ; =0xFFFFF333
	cmp r0, r1
	bge _02099F20
_02099F0C:
	mov r0, #0
	mov r1, #0x1000
	str r1, [sp, #0xb4]
	str r0, [sp, #0xb8]
	str r0, [sp, #0xbc]
_02099F20:
	add r0, sp, #0x90
	add r1, sp, #0xb4
	add r2, sp, #0x9c
	bl VEC_CrossProduct
	add r0, sp, #0x90
	add r1, sp, #0x9c
	add r2, sp, #0xa8
	bl VEC_CrossProduct
	ldr r7, [sp, #0x9c]
	ldr r6, [sp, #0xa0]
	ldr r3, [sp, #0xa4]
	ldr r0, [sp, #0x90]
	ldr r1, [sp, #0x94]
	ldr r2, [sp, #0x98]
	str r7, [sp, #0xc0]
	ldr r7, [sp, #0xa8]
	str r6, [sp, #0xc4]
	ldr r6, [sp, #0xac]
	str r3, [sp, #0xc8]
	ldr r3, [sp, #0xb0]
	str r0, [sp, #0xcc]
	str r1, [sp, #0xd0]
	add r0, sp, #0x30
	str r2, [sp, #0xd4]
	add r1, sp, #0xc0
	mov r2, r0
	str r7, [sp, #0xd8]
	str r6, [sp, #0xdc]
	str r3, [sp, #0xe0]
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0x30]
	ldr r0, [r0, #0x20]
	mov ip, #0x800
	ldr r0, [r0]
	mov r3, r2, asr #0x1f
	ldrsh r1, [r0, #0x30]
	ldr r0, [r0, #0x48]
	mov lr, #0
	smull r6, r1, r2, r1
	adds r7, r6, ip
	adc r6, r1, #0
	mov r0, r0, lsl #1
	mov r1, r7, lsr #0xc
	orr r1, r1, r6, lsl #20
	movs r0, r0, lsr #0x1d
	beq _02099FF0
	cmp r0, #1
	beq _0209A02C
	cmp r0, #2
	beq _0209A048
	b _0209A06C
_02099FF0:
	ldrsh r0, [r4, #0x34]
	mov r8, r0, asr #0x1f
	umull r7, r6, r2, r0
	mla r6, r2, r8, r6
	smull r8, r2, r1, r0
	adds r1, r8, ip
	adc r8, r2, lr
	adds r2, r7, ip
	mla r6, r3, r0, r6
	mov r1, r1, lsr #0xc
	adc r0, r6, lr
	mov r2, r2, lsr #0xc
	orr r1, r1, r8, lsl #20
	orr r2, r2, r0, lsl #20
	b _0209A06C
_0209A02C:
	ldrsh r0, [r4, #0x34]
	smull r3, r0, r1, r0
	adds r1, r3, ip
	adc r0, r0, lr
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	b _0209A06C
_0209A048:
	ldrsh r6, [r4, #0x34]
	mov r0, r6, asr #0x1f
	umull r8, r7, r2, r6
	mla r7, r2, r0, r7
	adds r2, r8, ip
	mla r7, r3, r6, r7
	adc r0, r7, lr
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
_0209A06C:
	add r0, sp, #0x60
	mov r3, r2
	bl MTX_Scale43_
	add r0, sp, #0x60
	add r1, sp, #0x30
	add r2, sp, #0
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r3, [r0]
	ldr r0, [r3]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209A0E8
	ldr r1, [r4, #8]
	ldr r0, [r4, #0x38]
	add r0, r1, r0
	str r0, [sp, #0x24]
	ldr r1, [r4, #0xc]
	ldr r0, [r4, #0x3c]
	add r0, r1, r0
	str r0, [sp, #0x28]
	ldr r1, [r4, #0x10]
	ldr r0, [r4, #0x40]
	add r0, r1, r0
	str r0, [sp, #0x2c]
	ldr r0, [r5, #0x44]
	bl G3_LoadMtx43
	add r0, sp, #0
	bl G3_MultMtx43
	b _0209A18C
_0209A0E8:
	ldr r2, [r4, #8]
	ldr r1, [r4, #0x38]
	ldr r0, [r3, #4]
	add r1, r2, r1
	sub r0, r1, r0
	str r0, [sp, #0x24]
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r4, #0x3c]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #8]
	ldr r0, _0209A234 ; =0x04000454
	sub r1, r2, r1
	str r1, [sp, #0x28]
	ldr r1, [r5, #0x40]
	ldr r3, [r4, #0x10]
	ldr r1, [r1, #0x20]
	ldr r2, [r4, #0x40]
	ldr r1, [r1]
	add r3, r3, r2
	ldr r2, [r1, #0xc]
	mov r1, #0
	sub r2, r3, r2
	str r2, [sp, #0x2c]
	str r1, [r0]
	ldr r1, [r5, #0x40]
	ldr r0, _0209A238 ; =0x04000470
	ldr r1, [r1, #0x20]
	ldr r1, [r1]
	ldr r3, [r1, #0xc]
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r1, [r0]
	str r2, [r0]
	str r3, [r0]
	ldr r0, [r5, #0x44]
	bl G3_MultMtx43
	add r0, sp, #0
	bl G3_MultMtx43
_0209A18C:
	ldr r0, [r5, #0x40]
	ldrh r4, [r4, #0x36]
	ldrh ip, [r0, #0x72]
	ldr r1, _0209A23C ; =0x04000480
	and r2, r4, #0x1f
	and r0, ip, #0x1f
	mul r3, r2, r0
	and r2, r4, #0x3e0
	and r0, ip, #0x3e0
	mul r0, r2, r0
	and r4, r4, #0x7c00
	and r2, ip, #0x7c00
	mul ip, r4, r2
	mov r2, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, ip, asr #0x19
	orr r0, r2, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r2, [r5, #0x40]
	ldr r4, _0209A240 ; =_021105DC
	ldr r1, [r2, #0x20]
	ldrsh r0, [r2, #0x78]
	ldr ip, [r1]
	ldrsh r1, [r2, #0x7a]
	ldr r3, [ip]
	ldrsh r2, [ip, #0x50]
	mov r3, r3, lsl #0xc
	mov r5, r3, lsr #0x1f
	ldrsh r3, [ip, #0x52]
	ldr r4, [r4, r5, lsl #2]
	blx r4
	add sp, sp, #0xf0
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_0209A220: .word 0x040004A4
_0209A224: .word FX_SinCosTable_
_0209A228: .word _021105E4
_0209A22C: .word 0x00000CCD
_0209A230: .word 0xFFFFF333
_0209A234: .word 0x04000454
_0209A238: .word 0x04000470
_0209A23C: .word 0x04000480
_0209A240: .word _021105DC
	arm_func_end spl_draw_dpl

	arm_func_start spl_draw_chld_pol
spl_draw_chld_pol: ; 0x0209A244
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x90
	mov r4, r1
	ldrh r2, [r4, #0x2e]
	mov r5, r0
	ldr r0, [r5, #0x3c]
	mov r1, r2, lsl #0x16
	mov r3, r2, lsl #0x1b
	mov r1, r1, lsr #0x1b
	mov r3, r3, lsr #0x1b
	add r1, r1, #1
	mul r6, r3, r1
	mov r1, r2, lsl #0x10
	movs r2, r6, asr #5
	mov r1, r1, lsr #0x1a
	orr r0, r0, #0xc0
	orr r1, r0, r1, lsl #24
	ldr r0, _0209A578 ; =0x040004A4
	orr r1, r1, r2, lsl #16
	str r1, [r0]
	addeq sp, sp, #0x90
	ldr r0, [r0]
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	bxeq lr
	ldr r0, [r5, #0x40]
	ldrh r1, [r4, #0x20]
	ldr r0, [r0, #0x20]
	ldr r6, _0209A57C ; =FX_SinCosTable_
	ldr r0, [r0, #0x14]
	mov r1, r1, asr #4
	ldrh r0, [r0]
	mov r2, r1, lsl #1
	add r1, r2, #1
	mov r7, r2, lsl #1
	mov r1, r1, lsl #1
	mov r0, r0, lsl #0x15
	mov r3, r0, lsr #0x1e
	ldr r2, _0209A580 ; =_021105E4
	ldrsh r0, [r6, r7]
	ldr r3, [r2, r3, lsl #2]
	ldrsh r1, [r6, r1]
	add r2, sp, #0x30
	blx r3
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0x30]
	ldr r0, [r0, #0x20]
	mov ip, #0x800
	ldr r0, [r0]
	mov r3, r2, asr #0x1f
	ldrsh r1, [r0, #0x30]
	ldr r0, [r0, #0x48]
	mov lr, #0
	smull r6, r1, r2, r1
	adds r7, r6, ip
	adc r6, r1, #0
	mov r1, r7, lsr #0xc
	mov r0, r0, lsl #1
	orr r1, r1, r6, lsl #20
	movs r0, r0, lsr #0x1d
	beq _0209A348
	cmp r0, #1
	beq _0209A384
	cmp r0, #2
	beq _0209A3A0
	b _0209A3C4
_0209A348:
	ldrsh r0, [r4, #0x34]
	mov r8, r0, asr #0x1f
	umull r7, r6, r2, r0
	mla r6, r2, r8, r6
	smull r8, r2, r1, r0
	adds r1, r8, ip
	adc r8, r2, lr
	adds r2, r7, ip
	mla r6, r3, r0, r6
	mov r1, r1, lsr #0xc
	adc r0, r6, lr
	mov r2, r2, lsr #0xc
	orr r1, r1, r8, lsl #20
	orr r2, r2, r0, lsl #20
	b _0209A3C4
_0209A384:
	ldrsh r0, [r4, #0x34]
	smull r3, r0, r1, r0
	adds r1, r3, ip
	adc r0, r0, lr
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	b _0209A3C4
_0209A3A0:
	ldrsh r6, [r4, #0x34]
	mov r0, r6, asr #0x1f
	umull r8, r7, r2, r6
	mla r7, r2, r0, r7
	adds r2, r8, ip
	mla r7, r3, r6, r7
	adc r0, r7, lr
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
_0209A3C4:
	add r0, sp, #0x60
	mov r3, r2
	bl MTX_Scale43_
	add r0, sp, #0x30
	add r1, sp, #0x60
	add r2, sp, #0
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r3, [r0]
	ldr r0, [r3]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209A440
	ldr r1, [r4, #8]
	ldr r0, [r4, #0x38]
	add r0, r1, r0
	str r0, [sp, #0x24]
	ldr r1, [r4, #0xc]
	ldr r0, [r4, #0x3c]
	add r0, r1, r0
	str r0, [sp, #0x28]
	ldr r1, [r4, #0x10]
	ldr r0, [r4, #0x40]
	add r0, r1, r0
	str r0, [sp, #0x2c]
	ldr r0, [r5, #0x44]
	bl G3_LoadMtx43
	add r0, sp, #0
	bl G3_MultMtx43
	b _0209A4E4
_0209A440:
	ldr r2, [r4, #8]
	ldr r1, [r4, #0x38]
	ldr r0, [r3, #4]
	add r1, r2, r1
	sub r0, r1, r0
	str r0, [sp, #0x24]
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r4, #0x3c]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #8]
	ldr r0, _0209A584 ; =0x04000454
	sub r1, r2, r1
	str r1, [sp, #0x28]
	ldr r1, [r5, #0x40]
	ldr r3, [r4, #0x10]
	ldr r1, [r1, #0x20]
	ldr r2, [r4, #0x40]
	ldr r1, [r1]
	add r3, r3, r2
	ldr r2, [r1, #0xc]
	mov r1, #0
	sub r2, r3, r2
	str r2, [sp, #0x2c]
	str r1, [r0]
	ldr r1, [r5, #0x40]
	ldr r0, _0209A588 ; =0x04000470
	ldr r1, [r1, #0x20]
	ldr r1, [r1]
	ldr r3, [r1, #0xc]
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r1, [r0]
	str r2, [r0]
	str r3, [r0]
	ldr r0, [r5, #0x44]
	bl G3_MultMtx43
	add r0, sp, #0
	bl G3_MultMtx43
_0209A4E4:
	ldr r0, [r5, #0x40]
	ldrh r4, [r4, #0x36]
	ldrh ip, [r0, #0x72]
	mov r2, #0
	and r1, r4, #0x1f
	and r0, ip, #0x1f
	mul r3, r1, r0
	and r1, r4, #0x3e0
	and r0, ip, #0x3e0
	mul r0, r1, r0
	and r4, r4, #0x7c00
	and r1, ip, #0x7c00
	mul ip, r4, r1
	mov r1, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, ip, asr #0x19
	orr r0, r1, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	ldr r1, _0209A58C ; =0x04000480
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r5, [r5, #0x40]
	ldr r4, _0209A590 ; =_021105DC
	ldr r1, [r5, #0x20]
	ldrsh r0, [r5, #0x7c]
	ldr r3, [r1, #0x14]
	ldrsh r1, [r5, #0x7e]
	ldrh r5, [r3]
	mov r3, r2
	mov r5, r5, lsl #0x14
	mov r5, r5, lsr #0x1f
	ldr r4, [r4, r5, lsl #2]
	blx r4
	add sp, sp, #0x90
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_0209A578: .word 0x040004A4
_0209A57C: .word FX_SinCosTable_
_0209A580: .word _021105E4
_0209A584: .word 0x04000454
_0209A588: .word 0x04000470
_0209A58C: .word 0x04000480
_0209A590: .word _021105DC
	arm_func_end spl_draw_chld_pol

	arm_func_start spl_draw_pol
spl_draw_pol: ; 0x0209A594
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x90
	mov r4, r1
	ldrh r2, [r4, #0x2e]
	mov r5, r0
	ldr r0, [r5, #0x3c]
	mov r1, r2, lsl #0x16
	mov r3, r2, lsl #0x1b
	mov r1, r1, lsr #0x1b
	mov r3, r3, lsr #0x1b
	add r1, r1, #1
	mul r6, r3, r1
	mov r1, r2, lsl #0x10
	movs r2, r6, asr #5
	mov r1, r1, lsr #0x1a
	orr r0, r0, #0xc0
	orr r1, r0, r1, lsl #24
	ldr r0, _0209A8C8 ; =0x040004A4
	orr r1, r1, r2, lsl #16
	str r1, [r0]
	addeq sp, sp, #0x90
	ldr r0, [r0]
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	bxeq lr
	ldr r0, [r5, #0x40]
	ldrh r1, [r4, #0x20]
	ldr r0, [r0, #0x20]
	ldr r6, _0209A8CC ; =FX_SinCosTable_
	ldr r0, [r0]
	mov r1, r1, asr #4
	mov r2, r1, lsl #1
	ldr r0, [r0]
	add r1, r2, #1
	mov r7, r2, lsl #1
	mov r1, r1, lsl #1
	mov r0, r0, lsl #0xd
	mov r3, r0, lsr #0x1e
	ldr r2, _0209A8D0 ; =_021105E4
	ldrsh r0, [r6, r7]
	ldr r3, [r2, r3, lsl #2]
	ldrsh r1, [r6, r1]
	add r2, sp, #0x30
	blx r3
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0x30]
	ldr r0, [r0, #0x20]
	mov ip, #0x800
	ldr r0, [r0]
	mov r3, r2, asr #0x1f
	ldrsh r1, [r0, #0x30]
	ldr r0, [r0, #0x48]
	mov lr, #0
	smull r6, r1, r2, r1
	adds r7, r6, ip
	adc r6, r1, #0
	mov r1, r7, lsr #0xc
	mov r0, r0, lsl #1
	orr r1, r1, r6, lsl #20
	movs r0, r0, lsr #0x1d
	beq _0209A698
	cmp r0, #1
	beq _0209A6D4
	cmp r0, #2
	beq _0209A6F0
	b _0209A714
_0209A698:
	ldrsh r0, [r4, #0x34]
	mov r8, r0, asr #0x1f
	umull r7, r6, r2, r0
	mla r6, r2, r8, r6
	smull r8, r2, r1, r0
	adds r1, r8, ip
	adc r8, r2, lr
	adds r2, r7, ip
	mla r6, r3, r0, r6
	mov r1, r1, lsr #0xc
	adc r0, r6, lr
	mov r2, r2, lsr #0xc
	orr r1, r1, r8, lsl #20
	orr r2, r2, r0, lsl #20
	b _0209A714
_0209A6D4:
	ldrsh r0, [r4, #0x34]
	smull r3, r0, r1, r0
	adds r1, r3, ip
	adc r0, r0, lr
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	b _0209A714
_0209A6F0:
	ldrsh r6, [r4, #0x34]
	mov r0, r6, asr #0x1f
	umull r8, r7, r2, r6
	mla r7, r2, r0, r7
	adds r2, r8, ip
	mla r7, r3, r6, r7
	adc r0, r7, lr
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
_0209A714:
	add r0, sp, #0x60
	mov r3, r2
	bl MTX_Scale43_
	add r0, sp, #0x60
	add r1, sp, #0x30
	add r2, sp, #0
	bl MTX_Concat43
	ldr r0, [r5, #0x40]
	ldr r0, [r0, #0x20]
	ldr r3, [r0]
	ldr r0, [r3]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209A790
	ldr r1, [r4, #8]
	ldr r0, [r4, #0x38]
	add r0, r1, r0
	str r0, [sp, #0x24]
	ldr r1, [r4, #0xc]
	ldr r0, [r4, #0x3c]
	add r0, r1, r0
	str r0, [sp, #0x28]
	ldr r1, [r4, #0x10]
	ldr r0, [r4, #0x40]
	add r0, r1, r0
	str r0, [sp, #0x2c]
	ldr r0, [r5, #0x44]
	bl G3_LoadMtx43
	add r0, sp, #0
	bl G3_MultMtx43
	b _0209A834
_0209A790:
	ldr r2, [r4, #8]
	ldr r1, [r4, #0x38]
	ldr r0, [r3, #4]
	add r1, r2, r1
	sub r0, r1, r0
	str r0, [sp, #0x24]
	ldr r0, [r5, #0x40]
	ldr r2, [r4, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r4, #0x3c]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #8]
	ldr r0, _0209A8D4 ; =0x04000454
	sub r1, r2, r1
	str r1, [sp, #0x28]
	ldr r1, [r5, #0x40]
	ldr r3, [r4, #0x10]
	ldr r1, [r1, #0x20]
	ldr r2, [r4, #0x40]
	ldr r1, [r1]
	add r3, r3, r2
	ldr r2, [r1, #0xc]
	mov r1, #0
	sub r2, r3, r2
	str r2, [sp, #0x2c]
	str r1, [r0]
	ldr r1, [r5, #0x40]
	ldr r0, _0209A8D8 ; =0x04000470
	ldr r1, [r1, #0x20]
	ldr r1, [r1]
	ldr r3, [r1, #0xc]
	ldr r2, [r1, #8]
	ldr r1, [r1, #4]
	str r1, [r0]
	str r2, [r0]
	str r3, [r0]
	ldr r0, [r5, #0x44]
	bl G3_MultMtx43
	add r0, sp, #0
	bl G3_MultMtx43
_0209A834:
	ldr r0, [r5, #0x40]
	ldrh r4, [r4, #0x36]
	ldrh ip, [r0, #0x72]
	ldr r1, _0209A8DC ; =0x04000480
	and r2, r4, #0x1f
	and r0, ip, #0x1f
	mul r3, r2, r0
	and r2, r4, #0x3e0
	and r0, ip, #0x3e0
	mul r0, r2, r0
	and r4, r4, #0x7c00
	and r2, ip, #0x7c00
	mul ip, r4, r2
	mov r2, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, ip, asr #0x19
	orr r0, r2, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r2, [r5, #0x40]
	ldr r4, _0209A8E0 ; =_021105DC
	ldr r1, [r2, #0x20]
	ldrsh r0, [r2, #0x78]
	ldr ip, [r1]
	ldrsh r1, [r2, #0x7a]
	ldr r3, [ip]
	ldrsh r2, [ip, #0x50]
	mov r3, r3, lsl #0xc
	mov r5, r3, lsr #0x1f
	ldrsh r3, [ip, #0x52]
	ldr r4, [r4, r5, lsl #2]
	blx r4
	add sp, sp, #0x90
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	bx lr
	.align 2, 0
_0209A8C8: .word 0x040004A4
_0209A8CC: .word FX_SinCosTable_
_0209A8D0: .word _021105E4
_0209A8D4: .word 0x04000454
_0209A8D8: .word 0x04000470
_0209A8DC: .word 0x04000480
_0209A8E0: .word _021105DC
	arm_func_end spl_draw_pol

	arm_func_start spl_draw_chld_dbb
spl_draw_chld_dbb: ; 0x0209A8E4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x88
	mov r6, r1
	ldrh r1, [r6, #0x2e]
	mov r7, r0
	ldr r4, [r7, #0x40]
	mov r0, r1, lsl #0x16
	mov r2, r1, lsl #0x1b
	mov r0, r0, lsr #0x1b
	mov r2, r2, lsr #0x1b
	add r0, r0, #1
	mul r3, r2, r0
	ldr r2, [r4, #0x20]
	movs r3, r3, asr #5
	ldr r0, [r7, #0x3c]
	ldr r4, [r2]
	mov r1, r1, lsl #0x10
	mov r2, r1, lsr #0x1a
	orr r1, r0, #0xc0
	orr r2, r1, r2, lsl #24
	ldrsh r0, [r4, #0x30]
	ldr r8, [r7, #0x44]
	ldr r1, _0209AFB4 ; =0x040004A4
	orr r2, r2, r3, lsl #16
	str r2, [r1]
	addeq sp, sp, #0x88
	ldr r1, [r1]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
	ldr r4, [r6, #0x30]
	ldr r1, [r7, #0x40]
	smull r2, r5, r4, r0
	ldr r0, [r1, #0x20]
	mov r3, #0x800
	adds r1, r2, r3
	ldr r2, [r0]
	adc sb, r5, #0
	ldr r0, [r2, #0x48]
	mov r5, r1, lsr #0xc
	mov r0, r0, lsl #1
	mov r1, r4, asr #0x1f
	orr r5, r5, sb, lsl #20
	movs r0, r0, lsr #0x1d
	mov sb, #0
	beq _0209A9AC
	cmp r0, #1
	beq _0209A9E8
	cmp r0, #2
	beq _0209AA04
	b _0209AA28
_0209A9AC:
	ldrsh r0, [r6, #0x34]
	mov lr, r0, asr #0x1f
	umull ip, sl, r4, r0
	mla sl, r4, lr, sl
	smull lr, r4, r5, r0
	mla sl, r1, r0, sl
	adds r0, lr, r3
	adc lr, r4, sb
	adds r1, ip, r3
	mov r5, r0, lsr #0xc
	adc r0, sl, sb
	mov r4, r1, lsr #0xc
	orr r5, r5, lr, lsl #20
	orr r4, r4, r0, lsl #20
	b _0209AA28
_0209A9E8:
	ldrsh r0, [r6, #0x34]
	smull r1, r0, r5, r0
	adds r1, r1, r3
	adc r0, r0, sb
	mov r5, r1, lsr #0xc
	orr r5, r5, r0, lsl #20
	b _0209AA28
_0209AA04:
	ldrsh sl, [r6, #0x34]
	mov r0, sl, asr #0x1f
	umull lr, ip, r4, sl
	mla ip, r4, r0, ip
	adds r3, lr, r3
	mla ip, r1, sl, ip
	adc r0, ip, sb
	mov r4, r3, lsr #0xc
	orr r4, r4, r0, lsl #20
_0209AA28:
	ldr r0, [r2]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209AC90
	ldr r1, [r6, #8]
	ldr r0, [r6, #0x38]
	add sb, sp, #0x18
	add r0, r1, r0
	str r0, [sp, #0xc]
	ldr r2, [r6, #0xc]
	ldr r1, [r6, #0x3c]
	add r0, r6, #0x14
	add r1, r2, r1
	str r1, [sp, #0x10]
	ldr r2, [r6, #0x10]
	ldr r1, [r6, #0x40]
	add r1, r2, r1
	str r1, [sp, #0x14]
	ldmia r0, {r0, r1, r2}
	stmia sb, {r0, r1, r2}
	ldr r0, [r8, #8]
	add r1, sp, #0x24
	str r0, [sp, #0x24]
	ldr r2, [r8, #0x14]
	mov r0, sb
	str r2, [sp, #0x28]
	ldr r3, [r8, #0x20]
	mov r2, sb
	str r3, [sp, #0x2c]
	bl VEC_CrossProduct
	ldr r0, [sp, #0x18]
	cmp r0, #0
	bne _0209AACC
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bne _0209AACC
	ldr r0, [sp, #0x20]
	cmp r0, #0
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
_0209AACC:
	add r0, sp, #0x18
	mov r1, r0
	bl VEC_Normalize
	add r1, sp, #0x30
	mov r0, r8
	bl MI_Copy36B
	add r0, sp, #0x18
	add r1, sp, #0x30
	mov r2, r0
	bl MTX_MultVec33
	add r0, sp, #0xc
	mov r1, r8
	mov r2, r0
	bl MTX_MultVec43
	add r0, r6, #0x14
	add r3, sp, #0
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	mov r0, r3
	mov r1, r3
	bl VEC_Normalize
	ldr r0, [r8, #0x20]
	ldr r1, [sp, #8]
	rsb r0, r0, #0
	smull r3, r0, r1, r0
	mov r2, #0x800
	adds r1, r3, r2
	ldr r3, [r8, #0x14]
	adc r0, r0, #0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	ldr r0, [r8, #8]
	ldr sb, [sp]
	rsb r0, r0, #0
	smull r8, r0, sb, r0
	adds sb, r8, r2
	adc r8, r0, #0
	mov r0, sb, lsr #0xc
	orr r0, r0, r8, lsl #20
	ldr r8, [sp, #4]
	rsb r3, r3, #0
	smull sb, r3, r8, r3
	adds r8, sb, r2
	adc r2, r3, #0
	mov r3, r8, lsr #0xc
	orr r3, r3, r2, lsl #20
	add r0, r0, r3
	adds r1, r1, r0
	ldr r0, [r7, #0x40]
	rsbmi r1, r1, #0
	ldr r0, [r0, #0x20]
	rsb r2, r1, #0x1000
	ldr r0, [r0]
	mov sb, #0x800
	ldr r0, [r0, #0x48]
	ldr ip, [sp, #0x18]
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x10
	smull r8, r0, r2, r0
	adds r2, r8, sb
	adc r0, r0, #0
	mov r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	add r0, r2, #0x1000
	smull r2, r0, r4, r0
	adds r2, r2, sb
	adc r0, r0, #0
	mov lr, r2, lsr #0xc
	orr lr, lr, r0, lsl #20
	smull r0, r4, ip, r5
	adds r8, r0, sb
	ldr r3, [sp, #0x1c]
	ldr r0, [sp, #0xc]
	rsb r2, r3, #0
	str r0, [sp, #0x78]
	ldr r0, [sp, #0x10]
	smull sl, r5, r3, r5
	str r0, [sp, #0x7c]
	smull r3, r0, r2, lr
	adc r4, r4, #0
	mov r8, r8, lsr #0xc
	orr r8, r8, r4, lsl #20
	adds r3, r3, sb
	smull lr, r2, ip, lr
	str r8, [sp, #0x54]
	adc r0, r0, #0
	adds r8, sl, sb
	adc r4, r5, #0
	adds r5, lr, sb
	mov sb, r3, lsr #0xc
	orr sb, sb, r0, lsl #20
	mov r8, r8, lsr #0xc
	orr r8, r8, r4, lsl #20
	ldr r3, [sp, #0x14]
	adc r0, r2, #0
	mov r5, r5, lsr #0xc
	orr r5, r5, r0, lsl #20
	mov r4, #0x1000
	ldr r2, _0209AFB8 ; =0x04000454
	str sb, [sp, #0x60]
	str r8, [sp, #0x58]
	add r0, sp, #0x54
	str r5, [sp, #0x64]
	str r1, [sp, #0x6c]
	str r1, [sp, #0x70]
	str r1, [sp, #0x5c]
	str r1, [sp, #0x68]
	str r4, [sp, #0x74]
	str r3, [sp, #0x80]
	str r1, [r2]
	bl G3_MultMtx43
	b _0209AF3C
_0209AC90:
	ldr r3, [r6, #8]
	ldr r1, [r6, #0x38]
	ldr r0, [r2, #4]
	add r1, r3, r1
	sub r0, r1, r0
	str r0, [sp, #0xc]
	ldr r0, [r7, #0x40]
	ldr r2, [r6, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r6, #0x3c]
	ldr r0, [r0]
	add r1, r2, r1
	ldr r0, [r0, #8]
	add sb, sp, #0x18
	sub r0, r1, r0
	str r0, [sp, #0x10]
	ldr r0, [r7, #0x40]
	ldr r2, [r6, #0x10]
	ldr r0, [r0, #0x20]
	ldr r1, [r6, #0x40]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #0xc]
	add r0, r6, #0x14
	sub r1, r2, r1
	str r1, [sp, #0x14]
	ldmia r0, {r0, r1, r2}
	stmia sb, {r0, r1, r2}
	ldr r0, [r8, #8]
	add r1, sp, #0x24
	str r0, [sp, #0x24]
	ldr r2, [r8, #0x14]
	mov r0, sb
	str r2, [sp, #0x28]
	ldr r3, [r8, #0x20]
	mov r2, sb
	str r3, [sp, #0x2c]
	bl VEC_CrossProduct
	ldr r0, [sp, #0x18]
	cmp r0, #0
	bne _0209AD54
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bne _0209AD54
	ldr r0, [sp, #0x20]
	cmp r0, #0
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
_0209AD54:
	add r0, sp, #0x18
	mov r1, r0
	bl VEC_Normalize
	add r1, sp, #0x30
	mov r0, r8
	bl MI_Copy36B
	add r0, sp, #0x18
	add r1, sp, #0x30
	mov r2, r0
	bl MTX_MultVec33
	add r0, sp, #0xc
	mov r1, r8
	mov r2, r0
	bl MTX_MultVec43
	add r0, r6, #0x14
	add r3, sp, #0
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	mov r0, r3
	mov r1, r3
	bl VEC_Normalize
	ldr r0, [r8, #0x20]
	ldr r1, [sp, #8]
	rsb r0, r0, #0
	smull r3, r0, r1, r0
	mov r2, #0x800
	adds r1, r3, r2
	ldr r3, [r8, #0x14]
	adc r0, r0, #0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	ldr r0, [r8, #8]
	ldr sb, [sp]
	rsb r0, r0, #0
	smull r8, r0, sb, r0
	adds sb, r8, r2
	adc r8, r0, #0
	mov r0, sb, lsr #0xc
	orr r0, r0, r8, lsl #20
	ldr r8, [sp, #4]
	rsb r3, r3, #0
	smull sb, r3, r8, r3
	adds r8, sb, r2
	adc r2, r3, #0
	mov r3, r8, lsr #0xc
	orr r3, r3, r2, lsl #20
	add r0, r0, r3
	adds r1, r1, r0
	ldr r0, [r7, #0x40]
	rsbmi r1, r1, #0
	ldr r0, [r0, #0x20]
	rsb r2, r1, #0x1000
	ldr r0, [r0]
	mov sb, #0x800
	ldr r0, [r0, #0x48]
	ldr lr, [sp, #0x18]
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x10
	smull r8, r0, r2, r0
	adds r2, r8, sb
	adc r0, r0, #0
	mov r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	add r0, r2, #0x1000
	smull r2, r0, r4, r0
	adds r2, r2, sb
	adc r0, r0, #0
	mov r8, r2, lsr #0xc
	orr r8, r8, r0, lsl #20
	smull r0, r4, lr, r5
	adds ip, r0, sb
	ldr r3, [sp, #0x1c]
	ldr r0, [sp, #0xc]
	rsb r2, r3, #0
	str r0, [sp, #0x78]
	ldr r0, [sp, #0x10]
	smull sl, r5, r3, r5
	str r0, [sp, #0x7c]
	smull r3, r0, r2, r8
	adc r4, r4, #0
	mov ip, ip, lsr #0xc
	adds r3, r3, sb
	orr ip, ip, r4, lsl #20
	adc r0, r0, #0
	adds sl, sl, sb
	smull r8, r2, lr, r8
	adc r4, r5, #0
	adds r5, r8, sb
	mov sb, r3, lsr #0xc
	orr sb, sb, r0, lsl #20
	mov r8, sl, lsr #0xc
	orr r8, r8, r4, lsl #20
	ldr r3, [sp, #0x14]
	adc r0, r2, #0
	mov r2, r5, lsr #0xc
	orr r2, r2, r0, lsl #20
	mov r4, #0x1000
	ldr r0, _0209AFB8 ; =0x04000454
	str ip, [sp, #0x54]
	str sb, [sp, #0x60]
	str r8, [sp, #0x58]
	str r1, [sp, #0x6c]
	str r2, [sp, #0x64]
	str r1, [sp, #0x70]
	str r1, [sp, #0x5c]
	str r1, [sp, #0x68]
	str r4, [sp, #0x74]
	str r3, [sp, #0x80]
	str r1, [r0]
	ldr r0, [r7, #0x40]
	ldr r1, _0209AFBC ; =0x04000470
	ldr r2, [r0, #0x20]
	add r0, sp, #0x54
	ldr r2, [r2]
	ldr r4, [r2, #0xc]
	ldr r3, [r2, #8]
	ldr r2, [r2, #4]
	str r2, [r1]
	str r3, [r1]
	str r4, [r1]
	bl G3_MultMtx43
_0209AF3C:
	ldr r0, [r7, #0x40]
	ldrh r4, [r6, #0x36]
	ldrh r5, [r0, #0x72]
	mov r2, #0
	and r1, r4, #0x1f
	and r0, r5, #0x1f
	mul r3, r1, r0
	and r1, r4, #0x3e0
	and r0, r5, #0x3e0
	mul r0, r1, r0
	and r4, r4, #0x7c00
	and r1, r5, #0x7c00
	mul r5, r4, r1
	mov r1, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, r5, asr #0x19
	orr r0, r1, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	ldr r1, _0209AFC0 ; =0x04000480
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r1, [r7, #0x40]
	mov r3, r2
	ldrsh r0, [r1, #0x7c]
	ldrsh r1, [r1, #0x7e]
	bl drawXYPlane
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bx lr
	.align 2, 0
_0209AFB4: .word 0x040004A4
_0209AFB8: .word 0x04000454
_0209AFBC: .word 0x04000470
_0209AFC0: .word 0x04000480
	arm_func_end spl_draw_chld_dbb

	arm_func_start spl_draw_dbb
spl_draw_dbb: ; 0x0209AFC4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x88
	mov r6, r1
	ldrh r1, [r6, #0x2e]
	mov r7, r0
	ldr r4, [r7, #0x40]
	mov r0, r1, lsl #0x16
	mov r2, r1, lsl #0x1b
	mov r0, r0, lsr #0x1b
	mov r2, r2, lsr #0x1b
	add r0, r0, #1
	mul r3, r2, r0
	ldr r2, [r4, #0x20]
	movs r3, r3, asr #5
	ldr r0, [r7, #0x3c]
	ldr r4, [r2]
	mov r1, r1, lsl #0x10
	mov r2, r1, lsr #0x1a
	orr r1, r0, #0xc0
	orr r2, r1, r2, lsl #24
	ldrsh r0, [r4, #0x30]
	ldr r8, [r7, #0x44]
	ldr r1, _0209B69C ; =0x040004A4
	orr r2, r2, r3, lsl #16
	str r2, [r1]
	addeq sp, sp, #0x88
	ldr r1, [r1]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
	ldr r4, [r6, #0x30]
	ldr r1, [r7, #0x40]
	smull r2, r5, r4, r0
	ldr r0, [r1, #0x20]
	mov r3, #0x800
	adds r1, r2, r3
	ldr r2, [r0]
	adc sb, r5, #0
	ldr r0, [r2, #0x48]
	mov r5, r1, lsr #0xc
	mov r0, r0, lsl #1
	mov r1, r4, asr #0x1f
	orr r5, r5, sb, lsl #20
	movs r0, r0, lsr #0x1d
	mov sb, #0
	beq _0209B08C
	cmp r0, #1
	beq _0209B0C8
	cmp r0, #2
	beq _0209B0E4
	b _0209B108
_0209B08C:
	ldrsh r0, [r6, #0x34]
	mov lr, r0, asr #0x1f
	umull ip, sl, r4, r0
	mla sl, r4, lr, sl
	smull lr, r4, r5, r0
	mla sl, r1, r0, sl
	adds r0, lr, r3
	adc lr, r4, sb
	adds r1, ip, r3
	mov r5, r0, lsr #0xc
	adc r0, sl, sb
	mov r4, r1, lsr #0xc
	orr r5, r5, lr, lsl #20
	orr r4, r4, r0, lsl #20
	b _0209B108
_0209B0C8:
	ldrsh r0, [r6, #0x34]
	smull r1, r0, r5, r0
	adds r1, r1, r3
	adc r0, r0, sb
	mov r5, r1, lsr #0xc
	orr r5, r5, r0, lsl #20
	b _0209B108
_0209B0E4:
	ldrsh sl, [r6, #0x34]
	mov r0, sl, asr #0x1f
	umull lr, ip, r4, sl
	mla ip, r4, r0, ip
	adds r3, lr, r3
	mla ip, r1, sl, ip
	adc r0, ip, sb
	mov r4, r3, lsr #0xc
	orr r4, r4, r0, lsl #20
_0209B108:
	ldr r0, [r2]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209B370
	ldr r1, [r6, #8]
	ldr r0, [r6, #0x38]
	add sb, sp, #0x18
	add r0, r1, r0
	str r0, [sp, #0xc]
	ldr r2, [r6, #0xc]
	ldr r1, [r6, #0x3c]
	add r0, r6, #0x14
	add r1, r2, r1
	str r1, [sp, #0x10]
	ldr r2, [r6, #0x10]
	ldr r1, [r6, #0x40]
	add r1, r2, r1
	str r1, [sp, #0x14]
	ldmia r0, {r0, r1, r2}
	stmia sb, {r0, r1, r2}
	ldr r0, [r8, #8]
	add r1, sp, #0x24
	str r0, [sp, #0x24]
	ldr r2, [r8, #0x14]
	mov r0, sb
	str r2, [sp, #0x28]
	ldr r3, [r8, #0x20]
	mov r2, sb
	str r3, [sp, #0x2c]
	bl VEC_CrossProduct
	ldr r0, [sp, #0x18]
	cmp r0, #0
	bne _0209B1AC
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bne _0209B1AC
	ldr r0, [sp, #0x20]
	cmp r0, #0
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
_0209B1AC:
	add r0, sp, #0x18
	mov r1, r0
	bl VEC_Normalize
	add r1, sp, #0x30
	mov r0, r8
	bl MI_Copy36B
	add r0, sp, #0x18
	add r1, sp, #0x30
	mov r2, r0
	bl MTX_MultVec33
	add r0, sp, #0xc
	mov r1, r8
	mov r2, r0
	bl MTX_MultVec43
	add r0, r6, #0x14
	add r3, sp, #0
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	mov r0, r3
	mov r1, r3
	bl VEC_Normalize
	ldr r0, [r8, #0x20]
	ldr r1, [sp, #8]
	rsb r0, r0, #0
	smull r3, r0, r1, r0
	mov r2, #0x800
	adds r1, r3, r2
	ldr r3, [r8, #0x14]
	adc r0, r0, #0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	ldr r0, [r8, #8]
	ldr sb, [sp]
	rsb r0, r0, #0
	smull r8, r0, sb, r0
	adds sb, r8, r2
	adc r8, r0, #0
	mov r0, sb, lsr #0xc
	orr r0, r0, r8, lsl #20
	ldr r8, [sp, #4]
	rsb r3, r3, #0
	smull sb, r3, r8, r3
	adds r8, sb, r2
	adc r2, r3, #0
	mov r3, r8, lsr #0xc
	orr r3, r3, r2, lsl #20
	add r0, r0, r3
	adds r1, r1, r0
	ldr r0, [r7, #0x40]
	rsbmi r1, r1, #0
	ldr r0, [r0, #0x20]
	rsb r2, r1, #0x1000
	ldr r0, [r0]
	mov sb, #0x800
	ldr r0, [r0, #0x48]
	ldr ip, [sp, #0x18]
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x10
	smull r8, r0, r2, r0
	adds r2, r8, sb
	adc r0, r0, #0
	mov r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	add r0, r2, #0x1000
	smull r2, r0, r4, r0
	adds r2, r2, sb
	adc r0, r0, #0
	mov lr, r2, lsr #0xc
	orr lr, lr, r0, lsl #20
	smull r0, r4, ip, r5
	adds r8, r0, sb
	ldr r3, [sp, #0x1c]
	ldr r0, [sp, #0xc]
	rsb r2, r3, #0
	str r0, [sp, #0x78]
	ldr r0, [sp, #0x10]
	smull sl, r5, r3, r5
	str r0, [sp, #0x7c]
	smull r3, r0, r2, lr
	adc r4, r4, #0
	mov r8, r8, lsr #0xc
	orr r8, r8, r4, lsl #20
	adds r3, r3, sb
	smull lr, r2, ip, lr
	str r8, [sp, #0x54]
	adc r0, r0, #0
	adds r8, sl, sb
	adc r4, r5, #0
	adds r5, lr, sb
	mov sb, r3, lsr #0xc
	orr sb, sb, r0, lsl #20
	mov r8, r8, lsr #0xc
	orr r8, r8, r4, lsl #20
	ldr r3, [sp, #0x14]
	adc r0, r2, #0
	mov r5, r5, lsr #0xc
	orr r5, r5, r0, lsl #20
	mov r4, #0x1000
	ldr r2, _0209B6A0 ; =0x04000454
	str sb, [sp, #0x60]
	str r8, [sp, #0x58]
	add r0, sp, #0x54
	str r5, [sp, #0x64]
	str r1, [sp, #0x6c]
	str r1, [sp, #0x70]
	str r1, [sp, #0x5c]
	str r1, [sp, #0x68]
	str r4, [sp, #0x74]
	str r3, [sp, #0x80]
	str r1, [r2]
	bl G3_MultMtx43
	b _0209B61C
_0209B370:
	ldr r3, [r6, #8]
	ldr r1, [r6, #0x38]
	ldr r0, [r2, #4]
	add r1, r3, r1
	sub r0, r1, r0
	str r0, [sp, #0xc]
	ldr r0, [r7, #0x40]
	ldr r2, [r6, #0xc]
	ldr r0, [r0, #0x20]
	ldr r1, [r6, #0x3c]
	ldr r0, [r0]
	add r1, r2, r1
	ldr r0, [r0, #8]
	add sb, sp, #0x18
	sub r0, r1, r0
	str r0, [sp, #0x10]
	ldr r0, [r7, #0x40]
	ldr r2, [r6, #0x10]
	ldr r0, [r0, #0x20]
	ldr r1, [r6, #0x40]
	ldr r0, [r0]
	add r2, r2, r1
	ldr r1, [r0, #0xc]
	add r0, r6, #0x14
	sub r1, r2, r1
	str r1, [sp, #0x14]
	ldmia r0, {r0, r1, r2}
	stmia sb, {r0, r1, r2}
	ldr r0, [r8, #8]
	add r1, sp, #0x24
	str r0, [sp, #0x24]
	ldr r2, [r8, #0x14]
	mov r0, sb
	str r2, [sp, #0x28]
	ldr r3, [r8, #0x20]
	mov r2, sb
	str r3, [sp, #0x2c]
	bl VEC_CrossProduct
	ldr r0, [sp, #0x18]
	cmp r0, #0
	bne _0209B434
	ldr r0, [sp, #0x1c]
	cmp r0, #0
	bne _0209B434
	ldr r0, [sp, #0x20]
	cmp r0, #0
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
_0209B434:
	add r0, sp, #0x18
	mov r1, r0
	bl VEC_Normalize
	add r1, sp, #0x30
	mov r0, r8
	bl MI_Copy36B
	add r0, sp, #0x18
	add r1, sp, #0x30
	mov r2, r0
	bl MTX_MultVec33
	add r0, sp, #0xc
	mov r1, r8
	mov r2, r0
	bl MTX_MultVec43
	add r0, r6, #0x14
	add r3, sp, #0
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	mov r0, r3
	mov r1, r3
	bl VEC_Normalize
	ldr r0, [r8, #0x20]
	ldr r1, [sp, #8]
	rsb r0, r0, #0
	smull r3, r0, r1, r0
	mov r2, #0x800
	adds r1, r3, r2
	ldr r3, [r8, #0x14]
	adc r0, r0, #0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	ldr r0, [r8, #8]
	ldr sb, [sp]
	rsb r0, r0, #0
	smull r8, r0, sb, r0
	adds sb, r8, r2
	adc r8, r0, #0
	mov r0, sb, lsr #0xc
	orr r0, r0, r8, lsl #20
	ldr r8, [sp, #4]
	rsb r3, r3, #0
	smull sb, r3, r8, r3
	adds r8, sb, r2
	adc r2, r3, #0
	mov r3, r8, lsr #0xc
	orr r3, r3, r2, lsl #20
	add r0, r0, r3
	adds r1, r1, r0
	ldr r0, [r7, #0x40]
	rsbmi r1, r1, #0
	ldr r0, [r0, #0x20]
	rsb r2, r1, #0x1000
	ldr r0, [r0]
	mov sb, #0x800
	ldr r0, [r0, #0x48]
	ldr lr, [sp, #0x18]
	mov r0, r0, lsl #8
	mov r0, r0, lsr #0x10
	smull r8, r0, r2, r0
	adds r2, r8, sb
	adc r0, r0, #0
	mov r1, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	add r0, r2, #0x1000
	smull r2, r0, r4, r0
	adds r2, r2, sb
	adc r0, r0, #0
	mov r8, r2, lsr #0xc
	orr r8, r8, r0, lsl #20
	smull r0, r4, lr, r5
	adds ip, r0, sb
	ldr r3, [sp, #0x1c]
	ldr r0, [sp, #0xc]
	rsb r2, r3, #0
	str r0, [sp, #0x78]
	ldr r0, [sp, #0x10]
	smull sl, r5, r3, r5
	str r0, [sp, #0x7c]
	smull r3, r0, r2, r8
	adc r4, r4, #0
	mov ip, ip, lsr #0xc
	adds r3, r3, sb
	orr ip, ip, r4, lsl #20
	adc r0, r0, #0
	adds sl, sl, sb
	smull r8, r2, lr, r8
	adc r4, r5, #0
	adds r5, r8, sb
	mov sb, r3, lsr #0xc
	orr sb, sb, r0, lsl #20
	mov r8, sl, lsr #0xc
	orr r8, r8, r4, lsl #20
	ldr r3, [sp, #0x14]
	adc r0, r2, #0
	mov r2, r5, lsr #0xc
	orr r2, r2, r0, lsl #20
	mov r4, #0x1000
	ldr r0, _0209B6A0 ; =0x04000454
	str ip, [sp, #0x54]
	str sb, [sp, #0x60]
	str r8, [sp, #0x58]
	str r1, [sp, #0x6c]
	str r2, [sp, #0x64]
	str r1, [sp, #0x70]
	str r1, [sp, #0x5c]
	str r1, [sp, #0x68]
	str r4, [sp, #0x74]
	str r3, [sp, #0x80]
	str r1, [r0]
	ldr r0, [r7, #0x40]
	ldr r1, _0209B6A4 ; =0x04000470
	ldr r2, [r0, #0x20]
	add r0, sp, #0x54
	ldr r2, [r2]
	ldr r4, [r2, #0xc]
	ldr r3, [r2, #8]
	ldr r2, [r2, #4]
	str r2, [r1]
	str r3, [r1]
	str r4, [r1]
	bl G3_MultMtx43
_0209B61C:
	ldr r0, [r7, #0x40]
	ldrh r4, [r6, #0x36]
	ldrh r5, [r0, #0x72]
	ldr r1, _0209B6A8 ; =0x04000480
	and r2, r4, #0x1f
	and r0, r5, #0x1f
	mul r3, r2, r0
	and r2, r4, #0x3e0
	and r0, r5, #0x3e0
	mul r0, r2, r0
	and r4, r4, #0x7c00
	and r2, r5, #0x7c00
	mul r5, r4, r2
	mov r2, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, r5, asr #0x19
	orr r0, r2, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r2, [r7, #0x40]
	ldr r1, [r2, #0x20]
	ldrsh r0, [r2, #0x78]
	ldr r3, [r1]
	ldrsh r1, [r2, #0x7a]
	ldrsh r2, [r3, #0x50]
	ldrsh r3, [r3, #0x52]
	bl drawXYPlane
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bx lr
	.align 2, 0
_0209B69C: .word 0x040004A4
_0209B6A0: .word 0x04000454
_0209B6A4: .word 0x04000470
_0209B6A8: .word 0x04000480
	arm_func_end spl_draw_dbb

	arm_func_start spl_draw_chld_bb
spl_draw_chld_bb: ; 0x0209B6AC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x40
	mov r6, r1
	ldrh r1, [r6, #0x2e]
	mov r7, r0
	ldr r4, [r7, #0x40]
	mov r0, r1, lsl #0x16
	mov r2, r1, lsl #0x1b
	mov r0, r0, lsr #0x1b
	mov r2, r2, lsr #0x1b
	add r0, r0, #1
	mul r3, r2, r0
	ldr r2, [r4, #0x20]
	ldr r0, [r7, #0x3c]
	ldr r4, [r2]
	mov r1, r1, lsl #0x10
	mov r2, r1, lsr #0x1a
	orr r1, r0, #0xc0
	ldrsh r0, [r4, #0x30]
	movs r4, r3, asr #5
	orr r3, r1, r2, lsl #24
	ldr r1, [r7, #0x44]
	ldr r2, _0209BAEC ; =0x040004A4
	orr r3, r3, r4, lsl #16
	str r3, [r2]
	addeq sp, sp, #0x40
	ldr r2, [r2]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bxeq lr
	ldr r4, [r6, #0x30]
	ldr r2, [r7, #0x40]
	smull r3, r5, r4, r0
	ldr r0, [r2, #0x20]
	mov r8, #0x800
	adds r2, r3, r8
	ldr r3, [r0]
	adc sb, r5, #0
	ldr r0, [r3, #0x48]
	mov r5, r2, lsr #0xc
	mov r0, r0, lsl #1
	mov r2, r4, asr #0x1f
	orr r5, r5, sb, lsl #20
	movs r0, r0, lsr #0x1d
	mov sb, #0
	beq _0209B774
	cmp r0, #1
	beq _0209B7B0
	cmp r0, #2
	beq _0209B7CC
	b _0209B7F0
_0209B774:
	ldrsh r0, [r6, #0x34]
	mov lr, r0, asr #0x1f
	umull ip, sl, r4, r0
	mla sl, r4, lr, sl
	smull lr, r4, r5, r0
	mla sl, r2, r0, sl
	adds r0, lr, r8
	adc lr, r4, sb
	adds r2, ip, r8
	mov r5, r0, lsr #0xc
	adc r0, sl, sb
	mov r4, r2, lsr #0xc
	orr r5, r5, lr, lsl #20
	orr r4, r4, r0, lsl #20
	b _0209B7F0
_0209B7B0:
	ldrsh r0, [r6, #0x34]
	smull r2, r0, r5, r0
	adds r2, r2, r8
	adc r0, r0, sb
	mov r5, r2, lsr #0xc
	orr r5, r5, r0, lsl #20
	b _0209B7F0
_0209B7CC:
	ldrsh sl, [r6, #0x34]
	mov r0, sl, asr #0x1f
	umull lr, ip, r4, sl
	mla ip, r4, r0, ip
	adds r4, lr, r8
	mla ip, r2, sl, ip
	adc r0, ip, sb
	mov r4, r4, lsr #0xc
	orr r4, r4, r0, lsl #20
_0209B7F0:
	ldr r0, [r3]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209B910
	ldr r3, [r6, #8]
	ldr r2, [r6, #0x38]
	add r0, sp, #0
	add r2, r3, r2
	str r2, [sp]
	ldr r8, [r6, #0xc]
	ldr r3, [r6, #0x3c]
	mov r2, r0
	add r3, r8, r3
	str r3, [sp, #4]
	ldr r8, [r6, #0x10]
	ldr r3, [r6, #0x40]
	add r3, r8, r3
	str r3, [sp, #8]
	bl MTX_MultVec43
	ldrh r0, [r6, #0x20]
	ldr r2, _0209BAF0 ; =FX_SinCosTable_
	mov sb, #0
	mov r0, r0, asr #4
	mov r3, r0, lsl #1
	mov r0, r3, lsl #1
	ldrsh r1, [r2, r0]
	add r0, r3, #1
	mov r0, r0, lsl #1
	ldrsh r0, [r2, r0]
	mov lr, #0x1000
	ldr r3, [sp]
	ldr r2, [sp, #4]
	str r3, [sp, #0x30]
	smull ip, sl, r1, r5
	str lr, [sp, #0x2c]
	rsb r8, r1, #0
	ldr r1, [sp, #8]
	smull lr, r5, r0, r5
	mov r3, #0x800
	str r2, [sp, #0x34]
	adds r2, lr, r3
	adc r5, r5, #0
	adds ip, ip, r3
	mov lr, r2, lsr #0xc
	orr lr, lr, r5, lsl #20
	smull r5, r2, r8, r4
	adc sl, sl, #0
	mov r8, ip, lsr #0xc
	orr r8, r8, sl, lsl #20
	adds r5, r5, r3
	smull sl, r4, r0, r4
	adc r0, r2, #0
	adds r2, sl, r3
	mov r5, r5, lsr #0xc
	orr r5, r5, r0, lsl #20
	mov r3, r2, lsr #0xc
	adc r0, r4, #0
	orr r3, r3, r0, lsl #20
	ldr r2, _0209BAF4 ; =0x04000454
	str lr, [sp, #0xc]
	add r0, sp, #0xc
	str r8, [sp, #0x10]
	str sb, [sp, #0x14]
	str r5, [sp, #0x18]
	str r3, [sp, #0x1c]
	str sb, [sp, #0x20]
	str sb, [sp, #0x24]
	str sb, [sp, #0x28]
	str r1, [sp, #0x38]
	str sb, [r2]
	bl G3_MultMtx43
	b _0209BA74
_0209B910:
	ldr r8, [r6, #8]
	ldr r2, [r6, #0x38]
	ldr r0, [r3, #4]
	add r2, r8, r2
	sub r0, r2, r0
	str r0, [sp]
	ldr r2, [r7, #0x40]
	ldr r8, [r6, #0xc]
	ldr r2, [r2, #0x20]
	ldr r3, [r6, #0x3c]
	ldr r2, [r2]
	add r3, r8, r3
	ldr r2, [r2, #8]
	add r0, sp, #0
	sub r2, r3, r2
	str r2, [sp, #4]
	ldr r2, [r7, #0x40]
	ldr r8, [r6, #0x10]
	ldr r2, [r2, #0x20]
	ldr r3, [r6, #0x40]
	ldr r2, [r2]
	add r8, r8, r3
	ldr r3, [r2, #0xc]
	mov r2, r0
	sub r3, r8, r3
	str r3, [sp, #8]
	bl MTX_MultVec43
	ldrh r0, [r6, #0x20]
	ldr r2, _0209BAF0 ; =FX_SinCosTable_
	mov lr, #0
	mov r0, r0, asr #4
	mov r3, r0, lsl #1
	mov r0, r3, lsl #1
	ldrsh r1, [r2, r0]
	add r0, r3, #1
	mov r0, r0, lsl #1
	ldrsh r0, [r2, r0]
	mov sl, #0x1000
	ldr r3, [sp]
	ldr r2, [sp, #4]
	str r3, [sp, #0x30]
	smull sb, r8, r1, r5
	str sl, [sp, #0x2c]
	rsb ip, r1, #0
	smull sl, r5, r0, r5
	ldr r1, [sp, #8]
	mov r3, #0x800
	str r2, [sp, #0x34]
	adds r2, sl, r3
	adc r5, r5, #0
	mov sl, r2, lsr #0xc
	orr sl, sl, r5, lsl #20
	str r1, [sp, #0x38]
	adds sb, sb, r3
	str sl, [sp, #0xc]
	adc sl, r8, #0
	smull r5, r2, ip, r4
	mov r8, sb, lsr #0xc
	orr r8, r8, sl, lsl #20
	smull sb, r4, r0, r4
	adds r5, r5, r3
	adc r0, r2, #0
	adds r2, sb, r3
	mov r3, r5, lsr #0xc
	orr r3, r3, r0, lsl #20
	adc r0, r4, #0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	ldr r0, _0209BAF4 ; =0x04000454
	str r8, [sp, #0x10]
	str lr, [sp, #0x14]
	str r3, [sp, #0x18]
	str r2, [sp, #0x1c]
	str lr, [sp, #0x20]
	str lr, [sp, #0x24]
	str lr, [sp, #0x28]
	str lr, [r0]
	ldr r0, [r7, #0x40]
	ldr r1, _0209BAF8 ; =0x04000470
	ldr r2, [r0, #0x20]
	add r0, sp, #0xc
	ldr r2, [r2]
	ldr r4, [r2, #0xc]
	ldr r3, [r2, #8]
	ldr r2, [r2, #4]
	str r2, [r1]
	str r3, [r1]
	str r4, [r1]
	bl G3_MultMtx43
_0209BA74:
	ldr r0, [r7, #0x40]
	ldrh r4, [r6, #0x36]
	ldrh r5, [r0, #0x72]
	mov r2, #0
	and r1, r4, #0x1f
	and r0, r5, #0x1f
	mul r3, r1, r0
	and r1, r4, #0x3e0
	and r0, r5, #0x3e0
	mul r0, r1, r0
	and r4, r4, #0x7c00
	and r1, r5, #0x7c00
	mul r5, r4, r1
	mov r1, r3, asr #5
	mov r0, r0, asr #0xf
	mov r3, r5, asr #0x19
	orr r0, r1, r0, lsl #5
	orr r0, r0, r3, lsl #10
	mov r0, r0, lsl #0x10
	ldr r1, _0209BAFC ; =0x04000480
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r1, [r7, #0x40]
	mov r3, r2
	ldrsh r0, [r1, #0x7c]
	ldrsh r1, [r1, #0x7e]
	bl drawXYPlane
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	bx lr
	.align 2, 0
_0209BAEC: .word 0x040004A4
_0209BAF0: .word FX_SinCosTable_
_0209BAF4: .word 0x04000454
_0209BAF8: .word 0x04000470
_0209BAFC: .word 0x04000480
	arm_func_end spl_draw_chld_bb

	arm_func_start spl_draw_bb
spl_draw_bb: ; 0x0209BB00
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	mov r8, r1
	ldrh r3, [r8, #0x2e]
	mov sb, r0
	ldr r1, [sb, #0x40]
	mov r0, r3, lsl #0x16
	mov r2, r3, lsl #0x1b
	mov r0, r0, lsr #0x1b
	ldr r4, [r1, #0x20]
	mov r2, r2, lsr #0x1b
	ldr r5, [r4]
	add r0, r0, #1
	mul r0, r2, r0
	movs fp, r0, asr #5
	mov r3, r3, lsl #0x10
	ldr r2, [sb, #0x3c]
	mov r6, r3, lsr #0x1a
	orr r3, r2, #0xc0
	ldr r4, [r5, #0x48]
	ldrsh r7, [r5, #0x30]
	mov r2, r4, lsl #1
	ldrh r4, [r1, #0x72]
	orr r3, r3, r6, lsl #24
	ldrsh sl, [r8, #0x34]
	ldrh r5, [r8, #0x36]
	ldr r1, [sb, #0x44]
	ldr r0, _0209BF38 ; =0x040004A4
	orr r3, r3, fp, lsl #16
	str r3, [r0]
	mov r2, r2, lsr #0x1d
	addeq sp, sp, #0x3c
	and r3, r2, #0xff
	ldr r0, [r0]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bxeq lr
	ldr r6, [r8, #0x30]
	mov r0, #0x800
	smull r7, r2, r6, r7
	adds r7, r7, r0
	adc r2, r2, #0
	mov r7, r7, lsr #0xc
	cmp r3, #0
	mov lr, r6, asr #0x1f
	orr r7, r7, r2, lsl #20
	mov r2, #0
	bne _0209BBF4
	mov ip, sl, asr #0x1f
	umull fp, r3, r6, sl
	mla r3, r6, ip, r3
	mla r3, lr, sl, r3
	smull r6, sl, r7, sl
	adds r6, r6, r0
	adc sl, sl, r2
	adds fp, fp, r0
	adc r0, r3, r2
	mov r7, r6, lsr #0xc
	mov r6, fp, lsr #0xc
	orr r7, r7, sl, lsl #20
	orr r6, r6, r0, lsl #20
	b _0209BC34
_0209BBF4:
	cmp r3, #1
	bne _0209BC14
	smull sl, r3, r7, sl
	adds r7, sl, r0
	adc r0, r3, r2
	mov r7, r7, lsr #0xc
	orr r7, r7, r0, lsl #20
	b _0209BC34
_0209BC14:
	mov ip, sl, asr #0x1f
	umull fp, r3, r6, sl
	adds r0, fp, r0
	mla r3, r6, ip, r3
	mla r3, lr, sl, r3
	adc r2, r3, r2
	mov r6, r0, lsr #0xc
	orr r6, r6, r2, lsl #20
_0209BC34:
	ldr r0, [sb, #0x40]
	ldr r0, [r0, #0x20]
	ldr sl, [r0]
	ldr r0, [sl]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	bne _0209BD60
	ldr r3, [r8, #8]
	ldr r2, [r8, #0x38]
	add r0, sp, #0
	add r2, r3, r2
	str r2, [sp]
	ldr sl, [r8, #0xc]
	ldr r3, [r8, #0x3c]
	mov r2, r0
	add r3, sl, r3
	str r3, [sp, #4]
	ldr sl, [r8, #0x10]
	ldr r3, [r8, #0x40]
	add r3, sl, r3
	str r3, [sp, #8]
	bl MTX_MultVec43
	ldrh r0, [r8, #0x20]
	ldr r2, _0209BF3C ; =FX_SinCosTable_
	ldr ip, [sp]
	mov r0, r0, asr #4
	mov r3, r0, lsl #1
	mov r0, r3, lsl #1
	ldrsh r1, [r2, r0]
	add r0, r3, #1
	mov r0, r0, lsl #1
	ldrsh r0, [r2, r0]
	str ip, [sp, #0x30]
	mov ip, #0x1000
	ldr r2, [sp, #4]
	smull fp, sl, r1, r7
	str ip, [sp, #0x2c]
	rsb r3, r1, #0
	ldr r1, [sp, #8]
	str r2, [sp, #0x34]
	mov r8, #0
	smull ip, r7, r0, r7
	mov r2, #0x800
	str r1, [sp, #0x38]
	adds r1, ip, r2
	adc ip, r7, #0
	adds r7, fp, r2
	mov fp, r1, lsr #0xc
	orr fp, fp, ip, lsl #20
	smull ip, r1, r3, r6
	adc sl, sl, #0
	mov r7, r7, lsr #0xc
	orr r7, r7, sl, lsl #20
	adds r3, ip, r2
	smull sl, r6, r0, r6
	adc r0, r1, #0
	adds r1, sl, r2
	mov r3, r3, lsr #0xc
	orr r3, r3, r0, lsl #20
	mov r2, r1, lsr #0xc
	adc r0, r6, #0
	orr r2, r2, r0, lsl #20
	ldr r1, _0209BF40 ; =0x04000454
	str fp, [sp, #0xc]
	add r0, sp, #0xc
	str r7, [sp, #0x10]
	str r8, [sp, #0x14]
	str r3, [sp, #0x18]
	str r2, [sp, #0x1c]
	str r8, [sp, #0x20]
	str r8, [sp, #0x24]
	str r8, [sp, #0x28]
	str r8, [r1]
	bl G3_MultMtx43
	b _0209BEC4
_0209BD60:
	ldr r3, [r8, #8]
	ldr r2, [r8, #0x38]
	ldr r0, [sl, #4]
	add r2, r3, r2
	sub r0, r2, r0
	str r0, [sp]
	ldr r2, [sb, #0x40]
	ldr sl, [r8, #0xc]
	ldr r2, [r2, #0x20]
	ldr r3, [r8, #0x3c]
	ldr r2, [r2]
	add r3, sl, r3
	ldr r2, [r2, #8]
	add r0, sp, #0
	sub r2, r3, r2
	str r2, [sp, #4]
	ldr r2, [sb, #0x40]
	ldr sl, [r8, #0x10]
	ldr r2, [r2, #0x20]
	ldr r3, [r8, #0x40]
	ldr r2, [r2]
	add sl, sl, r3
	ldr r3, [r2, #0xc]
	mov r2, r0
	sub r3, sl, r3
	str r3, [sp, #8]
	bl MTX_MultVec43
	ldrh r0, [r8, #0x20]
	ldr r2, _0209BF3C ; =FX_SinCosTable_
	mov r8, #0
	mov r0, r0, asr #4
	mov r3, r0, lsl #1
	mov r0, r3, lsl #1
	ldrsh r1, [r2, r0]
	add r0, r3, #1
	mov r0, r0, lsl #1
	ldrsh r0, [r2, r0]
	ldr ip, [sp]
	ldr r2, [sp, #4]
	str ip, [sp, #0x30]
	mov ip, #0x1000
	str r2, [sp, #0x34]
	smull fp, sl, r1, r7
	str ip, [sp, #0x2c]
	rsb r3, r1, #0
	ldr r1, [sp, #8]
	smull ip, r7, r0, r7
	mov r2, #0x800
	str r1, [sp, #0x38]
	adds r1, ip, r2
	adc ip, r7, #0
	adds r7, fp, r2
	mov fp, r1, lsr #0xc
	orr fp, fp, ip, lsl #20
	smull ip, r1, r3, r6
	adc sl, sl, #0
	mov r7, r7, lsr #0xc
	orr r7, r7, sl, lsl #20
	smull sl, r6, r0, r6
	adds r3, ip, r2
	adc r0, r1, #0
	adds r1, sl, r2
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	adc r0, r6, #0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x1c]
	ldr r0, _0209BF40 ; =0x04000454
	str fp, [sp, #0xc]
	str r7, [sp, #0x10]
	str r8, [sp, #0x14]
	str r2, [sp, #0x18]
	str r8, [sp, #0x20]
	str r8, [sp, #0x24]
	str r8, [sp, #0x28]
	str r8, [r0]
	ldr r0, [sb, #0x40]
	ldr r1, _0209BF44 ; =0x04000470
	ldr r2, [r0, #0x20]
	add r0, sp, #0xc
	ldr r2, [r2]
	ldr r6, [r2, #0xc]
	ldr r3, [r2, #8]
	ldr r2, [r2, #4]
	str r2, [r1]
	str r3, [r1]
	str r6, [r1]
	bl G3_MultMtx43
_0209BEC4:
	and r1, r5, #0x1f
	and r0, r4, #0x1f
	mul r2, r1, r0
	and r1, r5, #0x3e0
	and r0, r4, #0x3e0
	mul r0, r1, r0
	and r3, r5, #0x7c00
	and r1, r4, #0x7c00
	mul r4, r3, r1
	mov r1, r2, asr #5
	mov r0, r0, asr #0xf
	mov r2, r4, asr #0x19
	orr r0, r1, r0, lsl #5
	orr r0, r0, r2, lsl #10
	mov r0, r0, lsl #0x10
	ldr r1, _0209BF48 ; =0x04000480
	mov r0, r0, lsr #0x10
	str r0, [r1]
	ldr r2, [sb, #0x40]
	ldr r1, [r2, #0x20]
	ldrsh r0, [r2, #0x78]
	ldr r3, [r1]
	ldrsh r1, [r2, #0x7a]
	ldrsh r2, [r3, #0x50]
	ldrsh r3, [r3, #0x52]
	bl drawXYPlane
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bx lr
	.align 2, 0
_0209BF38: .word 0x040004A4
_0209BF3C: .word FX_SinCosTable_
_0209BF40: .word 0x04000454
_0209BF44: .word 0x04000470
_0209BF48: .word 0x04000480
	arm_func_end spl_draw_bb
