	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_SndCaptureBeginSleep
	.public NNSi_SndCaptureEndSleep
	.public NNSi_SndCaptureMain
	.public NNSi_SndCaptureStop
	.extern NNS_SndFreeAlarm
	.extern NNS_SndUnlockCapture
	.extern NNS_SndUnlockChannel
	.extern NNSi_SndFaderGet
	.extern NNSi_SndFaderIsFinished
	.extern NNSi_SndFaderUpdate
	.extern _021E0314
	.extern _021E031C
	.extern _021E035C

.text
	arm_func_start NNSi_SndCaptureMain
NNSi_SndCaptureMain: ; 0x020C90A4
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020C9124 ; =_021E0314
	ldr r4, _020C9128 ; =_021E035C
	ldr r0, [r0, #0x48]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #4]
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	add r0, r4, #0x3c
	bl NNSi_SndFaderUpdate
	ldr r0, [r4, #0x4c]
	cmp r0, #0
	beq _020C90F4
	add r0, r4, #0x3c
	bl NNSi_SndFaderIsFinished
	cmp r0, #0
	beq _020C90F4
	bl NNSi_SndCaptureStop
	ldmia sp!, {r3, r4, r5, pc}
_020C90F4:
	add r0, r4, #0x3c
	bl NNSi_SndFaderGet
	ldr r1, [r4, #0x50]
	mov r5, r0, asr #8
	cmp r5, r1
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r4, #0x24]
	mov r1, r5
	mov r2, #0
	bl SND_SetChannelVolume
	str r5, [r4, #0x50]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C9124: .word _021E0314
_020C9128: .word _021E035C
	arm_func_end NNSi_SndCaptureMain

	arm_func_start NNSi_SndCaptureStop
NNSi_SndCaptureStop: ; 0x020C912C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r0, _020C9210 ; =_021E0314
	ldr r4, _020C9214 ; =_021E035C
	ldr r0, [r0, #0x48]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, [r4, #0x2c]
	mov r3, #0
	cmp r1, #0
	movge r7, #1
	movlt r7, #0
	cmp r7, #0
	movne r0, #1
	movne r2, r0, lsl r1
	ldr r0, [r4, #0x24]
	ldr r1, [r4, #0x28]
	moveq r2, #0
	bl SND_StopTimer
	cmp r7, #0
	beq _020C91B4
	bl SND_GetCurrentCommandTag
	mov r5, r0
	mov r0, #1
	bl SND_FlushCommand
	mov r0, r5
	bl SND_WaitForCommandProc
	ldr r6, _020C9218 ; =_021E031C
	mov r5, #0
_020C919C:
	mov r0, r6
	mov r1, r5
	mov r2, r5
	bl OS_ReceiveMessage
	cmp r0, #0
	bne _020C919C
_020C91B4:
	ldr r0, [r4, #0x28]
	cmp r0, #0
	beq _020C91C4
	bl NNS_SndUnlockCapture
_020C91C4:
	ldr r0, [r4, #0x20]
	cmp r0, #0
	beq _020C91D4
	bl NNS_SndUnlockChannel
_020C91D4:
	cmp r7, #0
	beq _020C91E4
	ldr r0, [r4, #0x2c]
	bl NNS_SndFreeAlarm
_020C91E4:
	ldr r0, [r4, #4]
	cmp r0, #1
	bne _020C9204
	mov r0, #0
	mov r1, r0
	mov r2, r0
	mov r3, r0
	bl SND_SetOutputSelector
_020C9204:
	mov r0, #0
	str r0, [r4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020C9210: .word _021E0314
_020C9214: .word _021E035C
_020C9218: .word _021E031C
	arm_func_end NNSi_SndCaptureStop

	arm_func_start NNSi_SndCaptureBeginSleep
NNSi_SndCaptureBeginSleep: ; 0x020C921C
	stmdb sp!, {r4, lr}
	ldr r0, _020C9274 ; =_021E0314
	ldr r3, _020C9278 ; =_021E035C
	ldr r0, [r0, #0x48]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r1, [r3, #0x2c]
	cmp r1, #0
	movge r0, #1
	movge r2, r0, lsl r1
	ldr r0, [r3, #0x24]
	ldr r1, [r3, #0x28]
	movlt r2, #0
	mov r3, #0
	bl SND_StopTimer
	bl SND_GetCurrentCommandTag
	mov r4, r0
	mov r0, #1
	bl SND_FlushCommand
	mov r0, r4
	bl SND_WaitForCommandProc
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C9274: .word _021E0314
_020C9278: .word _021E035C
	arm_func_end NNSi_SndCaptureBeginSleep

	arm_func_start NNSi_SndCaptureEndSleep
NNSi_SndCaptureEndSleep: ; 0x020C927C
	stmdb sp!, {r4, lr}
	ldr r0, _020C92F8 ; =_021E0314
	ldr r4, _020C92FC ; =_021E035C
	ldr r0, [r0, #0x48]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r1, [r4, #0xc]
	mov r0, #0
	ldr r2, [r4, #0x14]
	str r0, [r4, #0x1c]
	bl MIi_CpuClear32
	ldr r1, [r4, #0x10]
	ldr r2, [r4, #0x14]
	mov r0, #0
	bl MIi_CpuClear32
	ldr r0, [r4, #0xc]
	ldr r1, [r4, #0x14]
	bl DC_FlushRange
	ldr r0, [r4, #0x10]
	ldr r1, [r4, #0x14]
	bl DC_FlushRange
	ldr r1, [r4, #0x2c]
	mov r3, #0
	cmp r1, #0
	movge r0, #1
	movge r2, r0, lsl r1
	ldr r0, [r4, #0x24]
	ldr r1, [r4, #0x28]
	movlt r2, #0
	bl SND_StartTimer
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C92F8: .word _021E0314
_020C92FC: .word _021E035C
	arm_func_end NNSi_SndCaptureEndSleep
