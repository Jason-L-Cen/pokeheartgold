	.extern DrawCellToSurface2D_
	.extern DrawCellToSurface3D_
	.extern NNSi_G2dBitReaderRead
	.extern NNSi_G2dGetCellTransferState
	.extern NNSi_G2dGetUnpackedFont
	.extern NNSi_G2dSplitCharUTF16
	.extern _02109414
	.extern _02109434
	.extern _021DA498
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_G2dRndCoreDrawCellVramTransfer
NNS_G2dRndCoreDrawCellVramTransfer: ; 0x020BC1D4
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020BC2B8 ; =_021DA498
	mov r6, r0
	ldr r3, [r2]
	mov r4, r1
	ldr r5, [r3]
	ldr r0, [r5, #0x10]
	cmp r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, #1
	str r0, [r3, #0x30]
	ldr r3, [r5, #0x18]
	cmp r3, #0
	beq _020BC218
	ldr r0, [r2]
	mov r1, r6
	blx r3
_020BC218:
	ldr r0, _020BC2B8 ; =_021DA498
	ldr r0, [r0]
	ldr r0, [r0, #0x30]
	cmp r0, #0
	beq _020BC298
	mvn r0, #0
	cmp r4, r0
	beq _020BC25C
	mov r0, r4
	ldr r4, [r5, #0x14]
	bl NNSi_G2dGetCellTransferState
	mov r3, #1
	mvn r1, r3, lsl r4
	ldr r2, [r0, #0x20]
	and r1, r2, r1
	orr r1, r1, r3, lsl r4
	str r1, [r0, #0x20]
_020BC25C:
	ldr r0, [r5, #0x14]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _020BC298
_020BC26C: ; jump table
	b _020BC27C ; case 0
	b _020BC28C ; case 1
	b _020BC28C ; case 2
	b _020BC298 ; case 3
_020BC27C:
	mov r0, r5
	mov r1, r6
	bl DrawCellToSurface3D_
	b _020BC298
_020BC28C:
	mov r0, r5
	mov r1, r6
	bl DrawCellToSurface2D_
_020BC298:
	ldr r2, [r5, #0x1c]
	cmp r2, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, _020BC2B8 ; =_021DA498
	mov r1, r6
	ldr r0, [r0]
	blx r2
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020BC2B8: .word _021DA498
	arm_func_end NNS_G2dRndCoreDrawCellVramTransfer
