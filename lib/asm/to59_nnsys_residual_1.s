	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_FndInsertListObject
	.extern NNS_FndAppendListObject
	.extern NNS_FndPrependListObject

.text
	arm_func_start NNS_FndInsertListObject
NNS_FndInsertListObject: ; 0x020B4C50
	stmdb sp!, {r3, lr}
	cmp r1, #0
	bne _020B4C68
	mov r1, r2
	bl NNS_FndAppendListObject
	ldmia sp!, {r3, pc}
_020B4C68:
	ldr r3, [r0]
	cmp r1, r3
	bne _020B4C80
	mov r1, r2
	bl NNS_FndPrependListObject
	ldmia sp!, {r3, pc}
_020B4C80:
	ldrh lr, [r0, #0xa]
	ldr r3, [r1, lr]
	add ip, r2, lr
	str r3, [r2, lr]
	str r1, [ip, #4]
	add r3, r3, lr
	str r2, [r3, #4]
	ldrh r3, [r0, #0xa]
	str r2, [r1, r3]
	ldrh r1, [r0, #8]
	add r1, r1, #1
	strh r1, [r0, #8]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_FndInsertListObject
