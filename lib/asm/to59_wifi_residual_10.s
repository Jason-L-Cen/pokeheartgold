	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_GetStatus
	.extern SOCL_SocketIsInvalid
	.extern SOCLi_GetReadBufferOccpiedSize
	.extern SOCLi_GetWriteBufferFreeSize

.text
	arm_func_start SOCL_GetStatus
SOCL_GetStatus: ; 0x020A2C78
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, #0
	mov r5, r0
	bl SOCL_SocketIsInvalid
	cmp r0, #0
	orrne r4, r4, #0x80
	bne _020A2D40
	ldrsh r0, [r5, #0x70]
	tst r0, #0x40
	ldrsb r0, [r5, #0x73]
	orrne r4, r4, #0x20
	cmp r0, #1
	beq _020A2CB8
	ldrsh r0, [r5, #0x70]
	tst r0, #4
	beq _020A2CE8
_020A2CB8:
	bl OS_DisableInterrupts
	mov r6, r0
	mov r0, r5
	bl SOCLi_GetReadBufferOccpiedSize
	cmp r0, #0
	mov r0, r5
	orrgt r4, r4, #1
	bl SOCLi_GetWriteBufferFreeSize
	cmp r0, #0
	mov r0, r6
	orrgt r4, r4, #8
	bl OS_RestoreInterrupts
_020A2CE8:
	ldrsb r0, [r5, #0x73]
	mov r1, #1
	cmp r0, #0
	cmpne r0, #4
	movne r1, #0
	cmp r1, #0
	beq _020A2D40
	ldrsh r0, [r5, #0x70]
	tst r0, #4
	ldrneb r0, [r5, #8]
	cmpne r0, #4
	beq _020A2D28
	tst r4, #1
	ldreqsh r0, [r5, #0x70]
	biceq r0, r0, #6
	streqh r0, [r5, #0x70]
_020A2D28:
	ldrsh r0, [r5, #0x70]
	tst r0, #2
	bne _020A2D40
	ldrsh r0, [r5, #0x70]
	tst r0, #4
	orreq r4, r4, #0x40
_020A2D40:
	mov r0, r4
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end SOCL_GetStatus
