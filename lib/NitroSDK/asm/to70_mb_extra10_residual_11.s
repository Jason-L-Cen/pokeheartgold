	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_CommEnd
	.extern MBi_CallReset
	.extern MBi_EndCommon
	.extern MBi_EndTaskThread
	.extern MBi_IsTaskAvailable
	.extern MBi_OnReset
	.extern ov13_0224CF40

.text
	arm_func_start MBi_CommEnd
MBi_CommEnd: ; 0x0221FCBC
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, #1
	bl OS_DisableInterrupts
	ldr r1, _0221FD48 ; =ov13_0224CF40
	mov r4, r0
	ldr r2, [r1, #8]
	ldrb r0, [r2, #0x50d]
	cmp r0, #0
	bne _0221FCEC
	mov r0, #0
	bl MBi_EndCommon
	b _0221FD38
_0221FCEC:
	add r0, r2, #0x500
	ldrh r0, [r0, #0x26]
	cmp r0, #0
	bne _0221FD38
	mov r0, #0
	str r0, [r2, #0x5e4]
	ldr r0, [r1, #8]
	mov r1, r5
	add r0, r0, #0x500
	strh r1, [r0, #0x26]
	bl MBi_IsTaskAvailable
	cmp r0, #0
	beq _0221FD30
	ldr r0, _0221FD4C ; =MBi_OnReset
	bl MBi_EndTaskThread
	mov r5, #0
	b _0221FD38
_0221FD30:
	bl MBi_CallReset
	mov r5, r0
_0221FD38:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, r5
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0221FD48: .word ov13_0224CF40
_0221FD4C: .word MBi_OnReset
	arm_func_end MBi_CommEnd
