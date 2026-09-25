	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_GfdResetLnkPlttVramState
	.extern NNSi_GfdAddNewFreeBlock
	.extern NNSi_GfdInitLnkVramBlockPool
	.extern NNSi_GfdInitLnkVramMan
	.extern NNSi_GfdMergeAllFreeBlocks
	.extern _021D8500
	.extern _021D8504

.text
	arm_func_start NNS_GfdResetLnkPlttVramState
NNS_GfdResetLnkPlttVramState: ; 0x020B6C80
	stmdb sp!, {r3, lr}
	ldr r0, _020B6CD0 ; =_021D8500
	ldr r1, [r0, #0x10]
	ldr r0, [r0, #0xc]
	mov r1, r1, lsr #4
	bl NNSi_GfdInitLnkVramBlockPool
	ldr r1, _020B6CD0 ; =_021D8500
	str r0, [r1, #4]
	ldr r0, _020B6CD4 ; =_021D8500
	bl NNSi_GfdInitLnkVramMan
	ldr r1, _020B6CD0 ; =_021D8500
	ldr r0, _020B6CD4 ; =_021D8500
	ldr r3, [r1, #8]
	ldr r1, _020B6CD8 ; =_021D8504
	mov r2, #0
	bl NNSi_GfdAddNewFreeBlock
	ldr r0, _020B6CD4 ; =_021D8500
	ldr r1, _020B6CD8 ; =_021D8504
	bl NNSi_GfdMergeAllFreeBlocks
	ldmia sp!, {r3, pc}
	.align 2, 0
_020B6CD0: .word _021D8500
_020B6CD4: .word _021D8500
_020B6CD8: .word _021D8504
	arm_func_end NNS_GfdResetLnkPlttVramState
