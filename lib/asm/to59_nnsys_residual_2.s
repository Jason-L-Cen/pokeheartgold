	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_FndInitHeapHead
	.extern FindListContainHeap
	.extern NNS_FndAppendListObject
	.extern NNS_FndInitList
	.extern _021D84B0
	.extern _021D84B4

.text
	arm_func_start NNSi_FndInitHeapHead
NNSi_FndInitHeapHead: ; 0x020B4DD0
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldrh r0, [sp, #8]
	str r1, [r4]
	mov ip, #0
	str r2, [r4, #0x18]
	bic r1, ip, #0xff
	and r0, r0, #0xff
	orr r2, r1, r0
	str r3, [r4, #0x1c]
	add r0, r4, #0xc
	mov r1, #4
	str r2, [r4, #0x20]
	bl NNS_FndInitList
	ldr r0, _020B4E44 ; =_021D84B0
	ldr r0, [r0]
	cmp r0, #0
	bne _020B4E30
	ldr r0, _020B4E48 ; =_021D84B4
	mov r1, #4
	bl NNS_FndInitList
	ldr r0, _020B4E44 ; =_021D84B0
	mov r1, #1
	str r1, [r0]
_020B4E30:
	mov r0, r4
	bl FindListContainHeap
	mov r1, r4
	bl NNS_FndAppendListObject
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B4E44: .word _021D84B0
_020B4E48: .word _021D84B4
	arm_func_end NNSi_FndInitHeapHead
