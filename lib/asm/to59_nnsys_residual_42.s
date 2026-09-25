	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_G3dGetTexPatAnmFV
	.extern NNSi_G3dGetTexPatAnmDataByIdx

.text
	arm_func_start NNSi_G3dGetTexPatAnmFV
NNSi_G3dGetTexPatAnmFV: ; 0x020C3C44
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r2
	bl NNSi_G3dGetTexPatAnmDataByIdx
	ldrsh r1, [r0, #4]
	ldrh r3, [r0, #6]
	mul r2, r1, r4
	add r3, r5, r3
	mov r2, r2, lsr #0xc
	b _020C3C70
_020C3C6C:
	sub r2, r2, #1
_020C3C70:
	cmp r2, #0
	beq _020C3C88
	mov r1, r2, lsl #2
	ldrh r1, [r3, r1]
	cmp r1, r4
	bhs _020C3C6C
_020C3C88:
	ldrh r1, [r0]
	b _020C3C94
_020C3C90:
	add r2, r2, #1
_020C3C94:
	add r0, r2, #1
	cmp r0, r1
	bhs _020C3CB0
	add r0, r3, r2, lsl #2
	ldrh r0, [r0, #4]
	cmp r0, r4
	bls _020C3C90
_020C3CB0:
	add r0, r3, r2, lsl #2
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNSi_G3dGetTexPatAnmFV
