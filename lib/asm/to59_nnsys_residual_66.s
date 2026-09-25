	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public RemoveCommandByPlayer
	.extern FreeCommandBuffer
	.extern NNS_FndGetNextListObject
	.extern NNS_FndRemoveListObject

.text
	arm_func_start RemoveCommandByPlayer
RemoveCommandByPlayer: ; 0x020CADF0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r8, r0
	mov r7, r1
	bl OS_DisableInterrupts
	mov r4, r0
	mov r0, r8
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r5, r0
	beq _020CAE54
_020CAE18:
	mov r0, r8
	mov r1, r5
	bl NNS_FndGetNextListObject
	ldr r1, [r5, #8]
	mov r6, r0
	cmp r1, r7
	bne _020CAE48
	mov r0, r8
	mov r1, r5
	bl NNS_FndRemoveListObject
	mov r0, r5
	bl FreeCommandBuffer
_020CAE48:
	mov r5, r6
	cmp r6, #0
	bne _020CAE18
_020CAE54:
	mov r0, r4
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end RemoveCommandByPlayer
