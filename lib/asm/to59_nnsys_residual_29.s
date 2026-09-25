	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNSi_G2dGetVramLocation
	.extern _02109328
	.extern _021DA470

.text
	arm_func_start NNS_G2dUpdateCellTransferStateManager
NNS_G2dUpdateCellTransferStateManager: ; 0x020BAD9C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	ldr r1, _020BAEB8 ; =_021DA470
	mov r0, #0
	str r0, [sp, #4]
	ldr r0, [r1, #4]
	cmp r0, #0
	addls sp, sp, #8
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp, #4]
	str r0, [sp]
_020BADC8:
	ldr r4, _020BAEB8 ; =_021DA470
	ldr r0, [sp]
	ldr r1, [r4, #8]
	add r8, r1, r0
	ldr r0, [r8, #0x1c]
	cmp r0, #0
	beq _020BAE84
	mov r7, #0
	cmp r7, #3
	bge _020BAE7C
	ldr r5, _020BAEBC ; =_02109328
	mov fp, r7
	mov r6, #1
_020BADFC:
	ldr r0, [r8, #0x24]
	mov r1, fp
	tst r0, r6, lsl r7
	beq _020BAE18
	ldr r0, [r8, #0x20]
	tst r0, r6, lsl r7
	movne r1, r6
_020BAE18:
	cmp r1, #0
	beq _020BAE70
	cmp r7, #0
	ldreq sl, [r8, #0x14]
	ldr sb, [r5, r7, lsl #2]
	mov r0, r8
	mov r1, r7
	ldrne sl, [r8, #0x10]
	bl NNSi_G2dGetVramLocation
	ldr r2, [r8, #0x28]
	mov r1, r0
	mov r0, sb
	ldr r3, [r8, #0x2c]
	ldr sb, [r4]
	add r2, sl, r2
	blx sb
	cmp r0, #0
	beq _020BAE70
	mvn r0, r6, lsl r7
	ldr r1, [r8, #0x24]
	and r0, r1, r0
	str r0, [r8, #0x24]
_020BAE70:
	add r7, r7, #1
	cmp r7, #3
	blt _020BADFC
_020BAE7C:
	mov r0, #0
	str r0, [r8, #0x20]
_020BAE84:
	ldr r0, [sp, #4]
	ldr r1, _020BAEB8 ; =_021DA470
	add r0, r0, #1
	str r0, [sp, #4]
	ldr r0, [sp]
	ldr r1, [r1, #4]
	add r0, r0, #0x30
	str r0, [sp]
	ldr r0, [sp, #4]
	cmp r0, r1
	blo _020BADC8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BAEB8: .word _021DA470
_020BAEBC: .word _02109328
	arm_func_end NNS_G2dUpdateCellTransferStateManager
