	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public ForceStopStrm
	.public NNS_SndStrmSetVolume
	.extern NNS_FndRemoveListObject
	.extern NNS_SndFreeAlarm
	.extern _021E0248
	.extern _021E0294

.text
	arm_func_start NNS_SndStrmSetVolume
NNS_SndStrmSetVolume: ; 0x020C8D88
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	str r1, [r7, #0x3c]
	ldr r0, [r7, #0x48]
	mov r6, #0
	cmp r0, #0
	ldmleia sp!, {r4, r5, r6, r7, r8, pc}
	ldr r4, _020C8DF0 ; =_021E0294
	mov r8, #1
_020C8DAC:
	add r0, r7, r6
	ldrb r5, [r0, #0x4c]
	ldr r1, [r7, #0x3c]
	add r0, r4, r5, lsl #3
	ldr r0, [r0, #4]
	add r0, r1, r0
	bl SND_CalcChannelVolume
	mov r2, r0
	mov r0, r8, lsl r5
	and r1, r2, #0xff
	mov r2, r2, asr #8
	bl SND_SetChannelVolume
	ldr r0, [r7, #0x48]
	add r6, r6, #1
	cmp r6, r0
	blt _020C8DAC
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C8DF0: .word _021E0294
	arm_func_end NNS_SndStrmSetVolume

	arm_func_start ForceStopStrm
ForceStopStrm: ; 0x020C8DF4
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	ldr r0, [r4, #0x24]
	mov r0, r0, lsl #0x1e
	movs r0, r0, asr #0x1f
	beq _020C8E5C
	ldr r0, [r4, #0x40]
	mov r2, #1
	mov r1, #0
	mov r2, r2, lsl r0
	ldr r0, [r4, #0x44]
	mov r3, r1
	bl SND_StopTimer
	add r0, r4, #8
	bl PM_DeletePreSleepCallback
	add r0, r4, #0x14
	bl PM_DeletePostSleepCallback
	ldr r0, [r4, #0x24]
	bic r0, r0, #2
	str r0, [r4, #0x24]
	bl SND_GetCurrentCommandTag
	mov r5, r0
	mov r0, #1
	bl SND_FlushCommand
	mov r0, r5
	bl SND_WaitForCommandProc
_020C8E5C:
	mov r0, r4
	bl ShutdownStrm
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ForceStopStrm

	arm_func_start ShutdownStrm
ShutdownStrm: ; 0x020C8E68
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4, #0x40]
	bl NNS_SndFreeAlarm
	ldr r0, _020C8E94 ; =_021E0248
	mov r1, r4
	bl NNS_FndRemoveListObject
	ldr r0, [r4, #0x24]
	bic r0, r0, #1
	str r0, [r4, #0x24]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C8E94: .word _021E0248
	arm_func_end ShutdownStrm
