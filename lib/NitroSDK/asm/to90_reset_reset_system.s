	.include "asm/macros.inc"
	.public OS_ResetSystem
	.extern CARD_LockRom
	.extern MI_StopDma
	.extern OS_GetLockID
	.extern OS_ResetRequestIrqMask
	.extern OS_SetIrqMask
	.extern OS_Terminate
	.extern OSi_DoResetSystem
	.extern OSi_SendToPxi
	.extern SDK_AUTOLOAD_DTCM_START
	.extern SDK_IRQ_STACKSIZE

.text
	.type OS_ResetSystem, @function
	arm_func_start OS_ResetSystem
OS_ResetSystem:
	stmdb sp!, {r4, lr}
	ldr	r1, [pc, #132]
	mov	r4, r0
	ldrh	r0, [r1]
	cmp	r0, #2
	moveq	r0, #1
	movne	r0, #0
	cmp	r0, #0
	beq . + 0x8
	bl OS_Terminate
	bl OS_GetLockID
	mov r0, r0, lsl #16
	mov r0, r0, lsr #16
	bl CARD_LockRom
	ldr	r0, [pc, #84]
	bl MI_StopDma
	ldr	r0, [pc, #80]
	bl MI_StopDma
	ldr	r0, [pc, #76]
	bl MI_StopDma
	ldr	r0, [pc, #72]
	bl MI_StopDma
	ldr	r0, [pc, #68]
	bl OS_SetIrqMask
	ldr	r0, [pc, #64]
	bl OS_ResetRequestIrqMask
	ldr	r1, [pc, #60]
	ldr	r0, [pc, #60]
	str	r4, [r1]
	bl OSi_SendToPxi
	ldr	r0, [pc, #52]
	ldr	r1, [pc, #52]
	sub	r0, r0, r1
	mov	sp, r0
	bl OSi_DoResetSystem
	ldmia sp!, {r4, pc}
	.word 0x027ffc40
	.word 0x00000000
	.word 0x00000001
	.word 0x00000002
	.word 0x00000003
	.word 0x00040000
	.word 0xfffbffff
	.word 0x027ffc20
	.word 0x00000010
	.word SDK_AUTOLOAD_DTCM_START+0x3f80
	.word SDK_IRQ_STACKSIZE
	arm_func_end OS_ResetSystem
	.type $d, @function
