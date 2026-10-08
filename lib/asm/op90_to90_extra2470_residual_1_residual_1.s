	.extern _021109C8
	.extern _021D8514
	.extern _021D8518
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
