	.extern CARDi_OnReadCard
	.extern CARDi_ReadFromCache
	.extern CARDi_SetCardDma
	.extern CARDi_SetRomOp
	.extern DC_FlushAll
	.extern DC_InvalidateRange
	.extern DC_StoreRange
	.extern IC_InvalidateAll
	.extern OS_GetDTCMAddress
	.extern cardi_common
	.extern cardi_rom_header_addr
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"

.text
	arm_func_start CARDi_TryReadCardDma
CARDi_TryReadCardDma: ; 0x020DD3E4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r4, _020DD55C ; =cardi_common
	mov r6, #0
	ldr sb, [r4, #0x20]
	mov fp, r0
	mov r7, r6
	mov r8, r6
	mov r1, r6
	ands sl, sb, #0x1f
	ldr r5, [r4, #0x24]
	bne _020DD41C
	ldr r0, [r4, #0x28]
	cmp r0, #3
	movls r1, #1
_020DD41C:
	cmp r1, #0
	beq _020DD470
	bl OS_GetDTCMAddress
	ldr r1, _020DD560 ; =OS_IrqHandler
	add r3, sb, r5
	cmp r3, r1
	mov r1, #1
	mov r2, #0
	bls _020DD448
	cmp sb, #0x2000000
	movlo r2, r1
_020DD448:
	cmp r2, #0
	bne _020DD468
	cmp r0, r3
	bhs _020DD464
	add r0, r0, #0x4000
	cmp r0, sb
	bhi _020DD468
_020DD464:
	mov r1, #0
_020DD468:
	cmp r1, #0
	moveq r8, #1
_020DD470:
	cmp r8, #0
	beq _020DD48C
	ldr r1, [r4, #0x1c]
	ldr r0, _020DD564 ; =0x000001FF
	orr r1, r1, r5
	tst r1, r0
	moveq r7, #1
_020DD48C:
	ldr r0, _020DD568 ; =cardi_rom_header_addr
	cmp r7, #0
	ldr r0, [r0]
	cmpne r5, #0
	ldr r0, [r0, #0x60]
	movne r6, #1
	bic r0, r0, #0x7000000
	orr r0, r0, #0xa1000000
	cmp r6, #0
	str r0, [fp, #4]
	beq _020DD554
	bl OS_DisableInterrupts
	ldr r1, [r4, #0x118]
	mov r7, r0
	cmp r5, r1
	bhs _020DD4DC
	mov r0, sb
	mov r1, r5
	bl IC_InvalidateRange
	b _020DD4E0
_020DD4DC:
	bl IC_InvalidateAll
_020DD4E0:
	ldr r0, [r4, #0x11c]
	cmp r5, r0
	bhs _020DD528
	cmp sl, #0
	beq _020DD514
	sub sb, sb, sl
	mov r0, sb
	mov r1, #0x20
	bl DC_StoreRange
	add r0, sb, r5
	mov r1, #0x20
	bl DC_StoreRange
	add r5, r5, #0x20
_020DD514:
	mov r0, sb
	mov r1, r5
	bl DC_InvalidateRange
	bl DC_WaitWriteBufferEmpty
	b _020DD52C
_020DD528:
	bl DC_FlushAll
_020DD52C:
	ldr r1, _020DD56C ; =CARDi_OnReadCard
	mov r0, #0x80000
	bl OS_SetIrqFunction
	mov r0, #0x80000
	bl OS_ResetRequestIrqMask
	mov r0, #0x80000
	bl OS_EnableIrqMask
	mov r0, r7
	bl OS_RestoreInterrupts
	bl CARDi_SetCardDma
_020DD554:
	mov r0, r6
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DD55C: .word cardi_common
_020DD560: .word OS_IrqHandler
_020DD564: .word 0x000001FF
_020DD568: .word cardi_rom_header_addr
_020DD56C: .word CARDi_OnReadCard
	arm_func_end CARDi_TryReadCardDma
