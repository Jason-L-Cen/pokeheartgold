	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dTickMCInstance
	.extern NNS_G2dTickCellAnimation

.text
	arm_func_start NNS_G2dTickMCInstance
NNS_G2dTickMCInstance: ; 0x020B8FA4
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	ldr r0, [r7, #8]
	mov r6, r1
	cmp r0, #1
	ldr r0, [r7]
	mov r4, #0
	ldr r5, [r7, #0xc]
	bne _020B9004
	ldrh r0, [r0, #2]
	cmp r0, #0
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
	mov r8, #0x5c
_020B8FD8:
	mla r0, r4, r8, r5
	mov r1, r6
	bl NNS_G2dTickCellAnimation
	ldr r1, [r7]
	add r0, r4, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r1, #2]
	mov r4, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020B8FD8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020B9004:
	ldrh r0, [r0]
	cmp r0, #0
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
	mov r8, #0x28
_020B9014:
	mul r0, r4, r8
	ldr r0, [r5, r0]
	mov r1, r6
	bl NNS_G2dTickCellAnimation
	ldr r1, [r7]
	add r0, r4, #1
	mov r0, r0, lsl #0x10
	ldrh r1, [r1]
	mov r4, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bhi _020B9014
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNS_G2dTickMCInstance
