	.extern MBi_CheckWmErrcode
	.extern MBi_CommEnd
	.extern MBi_EndCommon
	.extern MBi_IsSendEnabled
	.extern MBi_OnInitializeDone
	.extern _start_AutoloadDoneCallback
	.extern changeScanChannel
	.extern ov13_0224598C
	.extern ov13_0224CF40
	.extern ov13_0224CF60
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.extern MBi_GetBeaconPeriodDispersion

.text
	arm_func_start MB_Init
MB_Init: ; 0x0221F6B0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldr r4, _0221F888 ; =ov13_0224CF40
	mov sb, r1
	ldr r1, [r4, #0xc]
	mov r8, r2
	cmp r1, #0
	addne r1, r1, #0x1300
	ldrneh r1, [r1, #0x16]
	mov r7, r3
	cmpne r1, #0
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r0, r0, #0x1f
	bic r4, r0, #0x1f
	add r0, r4, #0x1f
	add r0, r0, #0x1e00
	cmp r7, #0x10000
	bic r5, r0, #0x1f
	bne _0221F704
	bl WM_GetNextTgid
	mov r7, r0
_0221F704:
	bl OS_DisableInterrupts
	ldr r3, _0221F88C ; =0x0000FFFF
	ldr r1, _0221F890 ; =ov13_0224598C
	mov r2, #5
	strh r3, [r1, #6]
	strh r2, [r1]
	mov r2, #0x28
	strh r2, [r1, #2]
	mov r6, r0
	strh r2, [r1, #4]
	mov r2, #1
	ldr r0, [sp, #0x20]
	ldr r3, _0221F888 ; =ov13_0224CF40
	str r2, [r1, #0xc]
	strh r0, [r3]
	str r4, [r3, #8]
	mov r1, r4
	mov r0, #0
	mov r2, #0x1e00
	str r5, [r3, #0xc]
	bl MIi_CpuClear32
	mov r1, r5
	mov r0, #0
	mov r2, #0x1340
	bl MIi_CpuClear16
	ldrb r0, [sb, #1]
	add r2, r4, #0x530
	mov r1, #0
	cmp r0, #0
	ble _0221F798
_0221F77C:
	add r0, sb, r1, lsl #1
	ldrh r0, [r0, #2]
	add r1, r1, #1
	strh r0, [r2], #2
	ldrb r0, [sb, #1]
	cmp r1, r0
	blt _0221F77C
_0221F798:
	add r0, r4, #0x138
	add r3, r0, #0x400
	ldr r0, _0221F890 ; =ov13_0224598C
	mov ip, #0
	ldr r1, [r0, #8]
_0221F7AC:
	ldrh r2, [r1]
	cmp r2, #0
	beq _0221F7D0
	add r1, r1, #2
	add ip, ip, #1
	str r1, [r0, #8]
	cmp ip, #0x10
	strh r2, [r3], #2
	blt _0221F7AC
_0221F7D0:
	mov r0, sb
	add r1, r5, #0x1300
	mov r2, #0x16
	bl MI_CpuCopy8
	ldrb r0, [sb, #1]
	cmp r0, #0xa
	bhs _0221F7FC
	add r0, r5, r0, lsl #1
	add r0, r0, #0x1300
	mov r1, #0
	strh r1, [r0, #2]
_0221F7FC:
	add r0, r4, #0x500
	mov r1, #0x100
	strh r1, [r0]
	mov r1, #8
	strh r1, [r0, #2]
	mov r2, #0
	strh r2, [r0, #0x18]
	strh r2, [r0, #0x1a]
	mov r1, #1
	strh r1, [r0, #0x2c]
	add r0, r5, #0x400
	str r0, [r4, #0x504]
	strh r2, [r4, #0xe]
	strh r2, [r4, #0x12]
	strh r1, [r4, #0x16]
	strh r2, [r4, #0x14]
	str r8, [r4, #8]
	strh r7, [r4, #0xc]
	bl MBi_GetBeaconPeriodDispersion
	add r0, r0, #0xc8
	strh r0, [r4, #0x18]
	mov r0, #0xf
	strh r0, [r4, #0x10]
	mov r3, #0
	strb r3, [r4, #0x50c]
	strb r3, [r4, #0x50d]
	add r1, r5, #0x1300
	mov r2, #1
	mov r0, r6
	strh r2, [r1, #0x16]
	add r1, r5, #0x1000
	str r3, [r1, #0x31c]
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0221F888: .word ov13_0224CF40
_0221F88C: .word 0x0000FFFF
_0221F890: .word ov13_0224598C
	arm_func_end MB_Init
