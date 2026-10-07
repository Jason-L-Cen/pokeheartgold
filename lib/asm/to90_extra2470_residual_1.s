	.extern _021109C8
	.extern _021D8514
	.extern _021D8518
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
