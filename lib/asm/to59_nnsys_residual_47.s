	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_SndPlayerCreateHeap
	.extern NNS_FndAppendListObject
	.extern NNS_SndHeapAlloc
	.extern NNS_SndHeapCreate
	.extern PlayerHeapDisposeCallback
	.extern _021DFDC4

.text
	arm_func_start NNS_SndPlayerCreateHeap
NNS_SndPlayerCreateHeap: ; 0x020C7FC4
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r2
	mov r6, r0
	mov r0, r1
	mov r3, #0
	ldr r2, _020C8050 ; =PlayerHeapDisposeCallback
	add r1, r5, #0x14
	str r3, [sp]
	bl NNS_SndHeapAlloc
	movs r4, r0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r2, #0
	str r2, [r4, #0xc]
	str r6, [r4, #0x10]
	mov r1, r5
	add r0, r4, #0x14
	str r2, [r4, #8]
	bl NNS_SndHeapCreate
	cmp r0, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r2, _020C8054 ; =_021DFDC4
	mov r1, #0x24
	mla r2, r6, r1, r2
	str r0, [r4, #8]
	mov r1, r4
	add r0, r2, #0xc
	bl NNS_FndAppendListObject
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020C8050: .word PlayerHeapDisposeCallback
_020C8054: .word _021DFDC4
	arm_func_end NNS_SndPlayerCreateHeap
