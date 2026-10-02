	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_StartParentCore
	.extern MB_CommSetParentStateCallback
	.extern MB_InitSendGameInfoStatus
	.extern MBi_CommParentCallback
	.extern MBi_IsCommSizeValid
	.extern MBi_OnInitializeDone
	.extern MBi_ParentCallback
	.extern MBi_SetChildMPMaxSize
	.extern MBi_SetMaxScanTime
	.extern MBi_SetParentPieceBuffer
	.extern ov13_0224CF40

.text
	arm_func_start MB_SetParentCommParam
MB_SetParentCommParam: ; 0x0221F904
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r0
	mov r4, r1
	bl OS_DisableInterrupts
	ldr r1, _0221F998 ; =ov13_0224CF40
	mov r6, r0
	ldr r1, [r1, #8]
	ldrb r1, [r1, #0x50d]
	cmp r1, #0
	beq _0221F938
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
_0221F938:
	mov r0, r5
	mov r2, r4
	mov r1, #8
	bl MBi_IsCommSizeValid
	cmp r0, #0
	bne _0221F960
	mov r0, r6
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
_0221F960:
	ldr r2, _0221F998 ; =ov13_0224CF40
	mov r0, r6
	ldr r1, [r2, #8]
	mov r3, #8
	strh r4, [r1, #0x10]
	ldr r1, [r2, #8]
	add r1, r1, #0x500
	strh r5, [r1]
	ldr r1, [r2, #8]
	add r1, r1, #0x500
	strh r3, [r1, #2]
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0221F998: .word ov13_0224CF40
	arm_func_end MB_SetParentCommParam

	arm_func_start MBi_StartCommon
MBi_StartCommon: ; 0x0221F99C
	stmdb sp!, {r4, lr}
	ldr r2, _0221FA6C ; =ov13_0224CF40
	mov r3, #0
	ldr r1, [r2, #8]
	mov r0, #0xa
	add r1, r1, #0x500
	strh r3, [r1, #0x28]
	ldr r1, [r2, #8]
	add r1, r1, #0x500
	strh r3, [r1, #0x2a]
	ldr r1, [r2, #8]
	add r1, r1, #0x500
	strh r3, [r1, #0x26]
	ldr r1, [r2, #8]
	add r1, r1, #0x500
	strh r3, [r1, #0x48]
	bl MBi_SetMaxScanTime
	ldr r4, _0221FA6C ; =ov13_0224CF40
	ldr r0, [r4, #0xc]
	add r0, r0, #0x1000
	ldr r0, [r0, #0x320]
	cmp r0, #0
	bne _0221FA44
_0221F9F8:
	ldrh r2, [r4]
	ldmib r4, {r0, r1}
	ldr r1, [r1, #0x508]
	bl WM_Initialize
	cmp r0, #4
	beq _0221F9F8
	cmp r0, #2
	movne r0, #8
	ldmneia sp!, {r4, pc}
	ldr r0, _0221FA6C ; =ov13_0224CF40
	ldr r0, [r0, #8]
	ldr r0, [r0, #0x508]
	bl WM_SetIndCallback
	ldr r0, _0221FA6C ; =ov13_0224CF40
	mov r2, #1
	ldr r1, [r0, #8]
	mov r0, #0
	strb r2, [r1, #0x50d]
	ldmia sp!, {r4, pc}
_0221FA44:
	ldr r0, [r4, #8]
	ldr r0, [r0, #0x508]
	bl WM_SetIndCallback
	mov r0, r4
	ldr r0, [r0, #8]
	mov r1, #1
	strb r1, [r0, #0x50d]
	bl MBi_OnInitializeDone
	mov r0, #0
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0221FA6C: .word ov13_0224CF40
	arm_func_end MBi_StartCommon

	arm_func_start MBi_StartParentCore
MBi_StartParentCore: ; 0x0221FA70
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r0
	bl OS_DisableInterrupts
	ldr r3, _0221FC40 ; =ov13_0224CF40
	mov r4, r0
	ldr r0, [r3, #8]
	ldr r2, _0221FC44 ; =0x000069C0
	strh r5, [r0, #0x32]
	ldr r5, [r3, #0xc]
	mov r0, #0
	add r1, r5, #0x1f
	add r1, r1, #0x7d00
	bic r1, r1, #0x1f
	str r1, [r3, #4]
	add r1, r5, #0x1000
	ldr r6, [r1, #0x4e4]
	add r1, r5, #0x1340
	bl MIi_CpuClear16
	mov r0, r6
	bl MB_CommSetParentStateCallback
	ldr r1, _0221FC40 ; =ov13_0224CF40
	ldr r0, [r1, #8]
	ldr r2, [r1, #0xc]
	add r0, r0, #0x500
	ldrh r3, [r0]
	add r0, r2, #0x1000
	sub r2, r3, #6
	str r2, [r0, #0x318]
	ldr r0, [r1, #8]
	add r0, r0, #0x500
	ldrh r0, [r0, #2]
	bl MBi_SetChildMPMaxSize
	ldr r0, _0221FC40 ; =ov13_0224CF40
	ldr r0, [r0, #0xc]
	add r0, r0, #0x138
	add r0, r0, #0x1400
	bl MBi_SetParentPieceBuffer
	mov r5, #0
	ldr r2, _0221FC40 ; =ov13_0224CF40
	mov r0, r5
	mvn r3, #0
_0221FB14:
	ldr r1, [r2, #0xc]
	add r1, r1, r5, lsl #2
	add r1, r1, #0x1000
	str r0, [r1, #0x4e8]
	ldr r1, [r2, #0xc]
	add r1, r1, r5
	add r1, r1, #0x1000
	add r5, r5, #1
	strb r3, [r1, #0x526]
	cmp r5, #0xf
	blt _0221FB14
	ldr r3, _0221FC40 ; =ov13_0224CF40
	ldr r2, _0221FC48 ; =0x00005D40
	ldr r1, [r3, #0xc]
	add r1, r1, #0x1000
	strb r0, [r1, #0x524]
	ldr r1, [r3, #0xc]
	add r1, r1, #0x388
	add r1, r1, #0x1400
	bl MIi_CpuClear16
	ldr r0, _0221FC40 ; =ov13_0224CF40
	mov r1, #0
	ldr r0, [r0, #0xc]
	mov r2, #0x1e
	add r0, r0, #0x354
	add r0, r0, #0x1400
	bl MI_CpuFill8
	ldr r1, _0221FC40 ; =ov13_0224CF40
	mov r2, #1
	ldr r0, [r1, #8]
	ldr r3, _0221FC4C ; =MBi_CommParentCallback
	add r0, r0, #0x500
	strh r2, [r0, #0x24]
	ldr r0, [r1, #8]
	ldr r2, _0221FC50 ; =MBi_ParentCallback
	str r3, [r0, #0x51c]
	ldr r0, [r1, #8]
	str r2, [r0, #0x508]
	ldr r2, [r1, #8]
	add r0, r2, #0x500
	ldrh r0, [r0]
	strh r0, [r2, #0x34]
	ldr r0, [r1, #8]
	ldrh r2, [r0, #0x34]
	add r0, r0, #0x500
	add r2, r2, #0x23
	bic r2, r2, #0x1f
	strh r2, [r0, #0x18]
	ldr r2, [r1, #8]
	add r0, r2, #0x500
	ldrh r0, [r0, #2]
	strh r0, [r2, #0x36]
	ldr r0, [r1, #8]
	ldrh r1, [r0, #0x36]
	add r0, r0, #0x500
	add r1, r1, #0xe
	rsb r1, r1, r1, lsl #4
	add r1, r1, #0x29
	bic r1, r1, #0x1f
	mov r1, r1, lsl #1
	strh r1, [r0, #0x1a]
	bl MB_InitSendGameInfoStatus
	bl MBi_StartCommon
	mov r5, r0
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #0xf
	mov r1, #1
	bl PXI_IsCallbackReady
	ldr r1, _0221FC40 ; =ov13_0224CF40
	ldr r1, [r1, #0xc]
	add r1, r1, #0x7000
	str r0, [r1, #0x4c8]
	mov r0, r5
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0221FC40: .word ov13_0224CF40
_0221FC44: .word 0x000069C0
_0221FC48: .word 0x00005D40
_0221FC4C: .word MBi_CommParentCallback
_0221FC50: .word MBi_ParentCallback
	arm_func_end MBi_StartParentCore
