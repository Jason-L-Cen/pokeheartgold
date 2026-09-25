	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern EraseSync
	.extern NNS_FndFreeByStateToFrmHeap
	.extern NNS_FndGetPrevListObject
	.extern NNS_FndRecordStateForFrmHeap
	.extern NNS_FndRemoveListObject
	.extern NNS_SndHeapClear
	.extern NewSection

.text
	arm_func_start NNS_SndHeapLoadState
NNS_SndHeapLoadState: ; 0x020C9C04
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, #0
	movs sb, r1
	mov sl, r0
	mov r7, r8
	bne _020C9C24
	bl NNS_SndHeapClear
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C9C24:
	ldrh r0, [sl, #0xc]
	cmp sb, r0
	bge _020C9CA8
	mov r4, #1
	mov r5, r8
_020C9C38:
	mov r1, r5
	add r0, sl, #4
	bl NNS_FndGetPrevListObject
	mov r1, r8
	mov r6, r0
	bl NNS_FndGetPrevListObject
	movs r8, r0
	beq _020C9C90
_020C9C58:
	ldr ip, [r8, #0xc]
	cmp ip, #0
	beq _020C9C7C
	ldr r1, [r8, #8]
	ldr r2, [r8, #0x10]
	ldr r3, [r8, #0x14]
	add r0, r8, #0x20
	blx ip
	mov r7, r4
_020C9C7C:
	mov r0, r6
	mov r1, r8
	bl NNS_FndGetPrevListObject
	movs r8, r0
	bne _020C9C58
_020C9C90:
	mov r1, r6
	add r0, sl, #4
	bl NNS_FndRemoveListObject
	ldrh r0, [sl, #0xc]
	cmp sb, r0
	blt _020C9C38
_020C9CA8:
	ldr r0, [sl]
	mov r1, sb
	bl NNS_FndFreeByStateToFrmHeap
	cmp r7, #0
	beq _020C9CC0
	bl EraseSync
_020C9CC0:
	ldrh r1, [sl, #0xc]
	ldr r0, [sl]
	bl NNS_FndRecordStateForFrmHeap
	mov r0, sl
	bl NewSection
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end NNS_SndHeapLoadState
