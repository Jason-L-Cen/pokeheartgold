	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_FndGetAllocatableSizeForFrmHeapEx

.text
	arm_func_start NNS_FndGetAllocatableSizeForFrmHeapEx
NNS_FndGetAllocatableSizeForFrmHeapEx: ; 0x020B5798
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, r1
	bl abs
	sub r1, r0, #1
	ldr r0, [r4, #0x24]
	mvn r2, r1
	add r0, r1, r0
	ldr r1, [r4, #0x28]
	and r0, r2, r0
	cmp r0, r1
	movhi r0, #0
	subls r0, r1, r0
	ldmia sp!, {r4, pc}
	arm_func_end NNS_FndGetAllocatableSizeForFrmHeapEx
