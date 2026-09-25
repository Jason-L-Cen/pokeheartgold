	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dApplyOamManagerToHW
	.public NNS_G2dResetOamManagerBuffer
	.extern _021109C8
	.extern _021D8514
	.extern _021D8518

.text
	arm_func_start NNS_G2dGetNewOamManagerInstance
NNS_G2dGetNewOamManagerInstance: ; 0x020B7994
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r5, [sp, #0x1c]
	ldr r6, _020B7B04 ; =_021D8518
	mov r4, #0x540
	mla r4, r5, r4, r6
	mov ip, r1
	sub r1, r2, #1
	add r1, ip, r1
	mov r1, r1, lsl #0x10
	mov r2, r1, lsr #0x10
	add lr, r4, ip, lsl #1
	add r7, r4, r2, lsl #1
	mov r6, r0
	mov r5, r3
	cmp lr, r7
	bhi _020B79F4
	ldr r0, _020B7B08 ; =0x0000FFFF
_020B79D8:
	ldrh r1, [lr]
	cmp r1, r0
	movne r0, #0
	bne _020B79F8
	add lr, lr, #2
	cmp lr, r7
	bls _020B79D8
_020B79F4:
	mov r0, #1
_020B79F8:
	cmp r0, #0
	beq _020B7A3C
	ldr r0, _020B7B0C ; =_021D8514
	sub r1, r2, ip
	ldrh r7, [r0]
	add r3, r1, #1
	add r1, r4, ip, lsl #1
	add lr, r7, #1
	strh lr, [r0]
	strh r7, [r6, #0x10]
	strh ip, [r6, #4]
	strh r2, [r6, #6]
	strh ip, [r6, #8]
	ldrh r0, [r6, #0x10]
	mov r2, r3, lsl #1
	bl MIi_CpuClear16
	b _020B7A44
_020B7A3C:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B7A44:
	ldrh r0, [sp, #0x18]
	add r3, r4, #0x500
	cmp r0, #0
	bne _020B7A70
	mov r0, #0x20
	strh r0, [r6, #0xa]
	mov r0, #0
	strh r0, [r6, #0xc]
	ldrh r0, [r6, #0xa]
	strh r0, [r6, #0xe]
	b _020B7AEC
_020B7A70:
	sub r0, r0, #1
	add r0, r5, r0
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	add r7, r3, r5, lsl #1
	add r4, r3, r2, lsl #1
	cmp r7, r4
	bhi _020B7AB0
	ldr r0, _020B7B08 ; =0x0000FFFF
_020B7A94:
	ldrh r1, [r7]
	cmp r1, r0
	movne r0, #0
	bne _020B7AB4
	add r7, r7, #2
	cmp r7, r4
	bls _020B7A94
_020B7AB0:
	mov r0, #1
_020B7AB4:
	cmp r0, #0
	beq _020B7AE4
	strh r5, [r6, #0xa]
	strh r2, [r6, #0xc]
	strh r5, [r6, #0xe]
	sub r0, r2, r5
	add r2, r0, #1
	ldrh r0, [r6, #0x10]
	add r1, r3, r5, lsl #1
	mov r2, r2, lsl #1
	bl MIi_CpuClear16
	b _020B7AEC
_020B7AE4:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B7AEC:
	mov r1, #0
	ldr r0, [sp, #0x1c]
	str r1, [r6, #0x14]
	str r0, [r6]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B7B04: .word _021D8518
_020B7B08: .word 0x0000FFFF
_020B7B0C: .word _021D8514
	arm_func_end NNS_G2dGetNewOamManagerInstance

	arm_func_start NNS_G2dEntryOamManagerOamWithAffineIdx
NNS_G2dEntryOamManagerOamWithAffineIdx: ; 0x020B7B10
	stmdb sp!, {r4, lr}
	ldrh lr, [r0, #6]
	ldrh ip, [r0, #8]
	mov r4, #0
	add r3, lr, #1
	cmp ip, r3
	bgt _020B7B38
	ldrh r3, [r0, #4]
	cmp r3, lr
	movls r4, #1
_020B7B38:
	cmp r4, #0
	moveq r3, #0
	beq _020B7B54
	sub r3, lr, ip
	add r3, r3, #1
	mov r3, r3, lsl #0x10
	mov r3, r3, lsr #0x10
_020B7B54:
	cmp r3, #1
	movhs r3, #1
	movlo r3, #0
	cmp r3, #0
	beq _020B7BE4
	ldr r4, [r0]
	ldr ip, _020B7BEC ; =_021D8518
	mov r3, #0x540
	mla ip, r4, r3, ip
	ldrh r3, [r0, #8]
	add r4, ip, #0x100
	ldrh ip, [r1]
	mov lr, r3, lsl #3
	add r3, r4, r3, lsl #3
	strh ip, [r4, lr]
	ldrh lr, [r1, #2]
	ldr ip, _020B7BF0 ; =0x0000FFFE
	strh lr, [r3, #2]
	ldrh r1, [r1, #4]
	cmp r2, ip
	strh r1, [r3, #4]
	beq _020B7BD0
	ldr r1, [r3]
	mov ip, r1, lsl #0x16
	mov ip, ip, lsr #0x1e
	tst ip, #1
	beq _020B7BD0
	bic ip, r1, #0x3e000000
	mov r1, r2, lsl #0x1b
	orr r1, ip, r1, lsr #2
	str r1, [r3]
_020B7BD0:
	ldrh r1, [r0, #8]
	add r1, r1, #1
	strh r1, [r0, #8]
	mov r0, #1
	ldmia sp!, {r4, pc}
_020B7BE4:
	mov r0, #0
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B7BEC: .word _021D8518
_020B7BF0: .word 0x0000FFFE
	arm_func_end NNS_G2dEntryOamManagerOamWithAffineIdx

	arm_func_start NNS_G2dEntryOamManagerAffine
NNS_G2dEntryOamManagerAffine: ; 0x020B7BF4
	stmdb sp!, {r3, lr}
	ldrh lr, [r0, #0xc]
	ldrh ip, [r0, #0xe]
	mov r3, #0
	add r2, lr, #1
	cmp ip, r2
	bgt _020B7C1C
	ldrh r2, [r0, #0xa]
	cmp r2, lr
	movls r3, #1
_020B7C1C:
	cmp r3, #0
	moveq r2, #0
	beq _020B7C38
	sub r2, lr, ip
	add r2, r2, #1
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
_020B7C38:
	cmp r2, #1
	movhs r2, #1
	movlo r2, #0
	cmp r2, #0
	ldreq r0, _020B7CB0 ; =0x0000FFFE
	ldmeqia sp!, {r3, pc}
	ldr lr, [r0]
	ldr r3, _020B7CB4 ; =_021D8518
	mov r2, #0x540
	mla ip, lr, r2, r3
	ldr r3, [r1]
	ldrh r2, [r0, #0xe]
	add ip, ip, #0x100
	mov r3, r3, asr #4
	add ip, ip, r2, lsl #5
	strh r3, [ip, #6]
	ldr r3, [r1, #4]
	mov r3, r3, asr #4
	strh r3, [ip, #0xe]
	ldr r3, [r1, #8]
	mov r3, r3, asr #4
	strh r3, [ip, #0x16]
	ldr r1, [r1, #0xc]
	mov r1, r1, asr #4
	strh r1, [ip, #0x1e]
	ldrh r1, [r0, #0xe]
	add r1, r1, #1
	strh r1, [r0, #0xe]
	mov r0, r2
	ldmia sp!, {r3, pc}
	.align 2, 0
_020B7CB0: .word 0x0000FFFE
_020B7CB4: .word _021D8518
	arm_func_end NNS_G2dEntryOamManagerAffine

	arm_func_start NNS_G2dApplyOamManagerToHW
NNS_G2dApplyOamManagerToHW: ; 0x020B7CB8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r7, r0
	ldr r0, [r7, #0x14]
	cmp r0, #0
	beq _020B7D4C
	ldr r6, [r7]
	ldr r1, _020B7EB4 ; =_021D8518
	mov r0, #0x540
	mla r1, r6, r0, r1
	ldrh r3, [r7, #4]
	ldrh r0, [r7, #6]
	add r5, r1, #0x100
	mov r4, r3, lsl #3
	sub r0, r0, r3
	add r0, r0, #1
	mov r0, r0, lsl #0x13
	mov r8, r0, lsr #0x10
	mov r2, r4, lsl #0x10
	mov r1, r8
	add r0, r5, r3, lsl #3
	mov r7, r2, lsr #0x10
	bl DC_FlushRange
	cmp r6, #0
	beq _020B7D24
	cmp r6, #1
	beq _020B7D38
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B7D24:
	mov r1, r7
	mov r2, r8
	add r0, r5, r4
	bl GX_LoadOAM
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B7D38:
	mov r1, r7
	mov r2, r8
	add r0, r5, r4
	bl GXS_LoadOAM
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B7D4C:
	ldr r0, [r7]
	ldr r2, _020B7EB4 ; =_021D8518
	mov r1, #0x540
	mla r2, r0, r1, r2
	ldrh r5, [r7, #4]
	ldrh r1, [r7, #6]
	ldr r3, _020B7EB8 ; =_021109C8
	add r4, r2, #0x100
	sub r1, r1, r5
	add r1, r1, #1
	mov r1, r1, lsl #0x10
	mov r2, r5, lsl #0x13
	add r6, r4, r5, lsl #3
	movs r5, r1, lsr #0x10
	mov r4, r2, lsr #0x10
	ldr sb, [r3, r0, lsl #2]
	mov sl, #0
	beq _020B7DCC
	mov r8, #6
_020B7D98:
	mov r0, r6
	mov r1, r4
	mov r2, r8
	blx sb
	add r2, r4, #8
	add r0, sl, #1
	mov r1, r0, lsl #0x10
	mov r0, r2, lsl #0x10
	cmp r5, r1, lsr #16
	mov r4, r0, lsr #0x10
	add r6, r6, #8
	mov sl, r1, lsr #0x10
	bhi _020B7D98
_020B7DCC:
	ldrh r2, [r7, #0xc]
	ldrh r1, [r7, #0xe]
	mov r3, #0
	add r0, r2, #1
	cmp r1, r0
	bgt _020B7DF0
	ldrh r0, [r7, #0xa]
	cmp r0, r2
	movls r3, #1
_020B7DF0:
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r7]
	ldr r2, _020B7EB4 ; =_021D8518
	mov r1, #0x540
	mla r2, r0, r1, r2
	ldr r3, _020B7EB8 ; =_021109C8
	ldrh r5, [r7, #0xa]
	ldrh r1, [r7, #0xc]
	add r4, r2, #0x100
	mov r2, r5, lsl #0x15
	sub r1, r1, r5
	add r1, r1, #1
	mov r1, r1, lsl #0x10
	ldr r6, [r3, r0, lsl #2]
	add sl, r4, r5, lsl #5
	movs sb, r1, lsr #0x10
	mov r8, r2, lsr #0x10
	mov r7, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r5, #2
	mov r4, r5
	mov fp, r5
_020B7E4C:
	mov r2, r5
	add r0, sl, #6
	add r1, r8, #6
	blx r6
	mov r2, r4
	add r0, sl, #0xe
	add r1, r8, #0xe
	blx r6
	add r0, sl, #0x16
	add r1, r8, #0x16
	mov r2, fp
	blx r6
	add r0, sl, #0x1e
	add r1, r8, #0x1e
	mov r2, #2
	blx r6
	add r0, r8, #0x20
	mov r0, r0, lsl #0x10
	mov r8, r0, lsr #0x10
	add r0, r7, #1
	mov r0, r0, lsl #0x10
	add sl, sl, #0x20
	cmp sb, r0, lsr #16
	mov r7, r0, lsr #0x10
	bhi _020B7E4C
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B7EB4: .word _021D8518
_020B7EB8: .word _021109C8
	arm_func_end NNS_G2dApplyOamManagerToHW

	arm_func_start NNS_G2dResetOamManagerBuffer
NNS_G2dResetOamManagerBuffer: ; 0x020B7EBC
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r0
	ldr r0, [r5, #0x14]
	cmp r0, #0
	beq _020B7F74
	ldrh r3, [r5, #6]
	ldrh r1, [r5, #8]
	mov r2, #0
	add r0, r3, #1
	cmp r1, r0
	bgt _020B7EF4
	ldrh r0, [r5, #4]
	cmp r0, r3
	movls r2, #1
_020B7EF4:
	cmp r2, #0
	moveq r4, #0
	beq _020B7F14
	ldrh r0, [r5, #4]
	sub r0, r3, r0
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	mov r4, r0, lsr #0xd
_020B7F14:
	ldr r2, [r5]
	ldr r1, _020B7FDC ; =_021D8518
	mov r0, #0x540
	mla r1, r2, r0, r1
	add r2, r1, #0x100
	ldrh r0, [r5, #4]
	mov r1, r4
	add r6, r2, r0, lsl #3
	mov r0, r6
	bl DC_InvalidateRange
	ldr r0, _020B7FE0 ; =GXi_DmaId
	mvn r1, #0
	ldr r0, [r0]
	cmp r0, r1
	mov r1, r6
	beq _020B7F64
	mov r3, r4
	mov r2, #0xc0
	bl MI_DmaFill32
	b _020B7FC8
_020B7F64:
	mov r2, r4
	mov r0, #0xc0
	bl MIi_CpuClear32
	b _020B7FC8
_020B7F74:
	ldr r2, [r5]
	ldr r1, _020B7FDC ; =_021D8518
	mov r0, #0x540
	mla r1, r2, r0, r1
	ldrh r2, [r5, #4]
	ldrh r0, [r5, #6]
	add r1, r1, #0x100
	add r3, r1, r2, lsl #3
	sub r0, r0, r2
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	movs r2, r0, lsr #0x10
	mov r0, #0
	beq _020B7FC8
	mov r1, #0xc0
_020B7FB0:
	add r0, r0, #1
	mov r0, r0, lsl #0x10
	cmp r2, r0, lsr #16
	strh r1, [r3], #8
	mov r0, r0, lsr #0x10
	bhi _020B7FB0
_020B7FC8:
	ldrh r0, [r5, #4]
	strh r0, [r5, #8]
	ldrh r0, [r5, #0xa]
	strh r0, [r5, #0xe]
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020B7FDC: .word _021D8518
_020B7FE0: .word GXi_DmaId
	arm_func_end NNS_G2dResetOamManagerBuffer
