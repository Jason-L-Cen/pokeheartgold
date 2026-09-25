	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public BeginSleep__Strm
	.public EndSleep__Strm
	.public StrmCallback
	.extern NNSi_SndCaptureStop
	.extern NNSi_SndFaderSet
	.extern _021E0254
	.extern _021E0294
	.extern _021E0314
	.extern _021E035C

.text
	arm_func_start StrmCallback
StrmCallback: ; 0x020C8EA8
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r5, r0
	mov r4, r1
	ldr r0, [r5, #0x28]
	ldr r1, [r5, #0x2c]
	bl _u32_div_f
	ldr r1, [r5, #0x48]
	ldr r2, [r5, #0x38]
	mov r3, r0
	mul ip, r3, r2
	cmp r1, #0
	mov lr, #0
	ble _020C8F0C
	ldr r2, _020C8F50 ; =_021E0294
	ldr r0, _020C8F54 ; =_021E0254
_020C8EE8:
	add r1, r5, lr
	ldrb r1, [r1, #0x4c]
	ldr r1, [r2, r1, lsl #3]
	add r1, r1, ip
	str r1, [r0, lr, lsl #2]
	ldr r1, [r5, #0x48]
	add lr, lr, #1
	cmp lr, r1
	blt _020C8EE8
_020C8F0C:
	ldr r0, [r5, #0x20]
	ldr r2, _020C8F54 ; =_021E0254
	str r0, [sp]
	ldr ip, [r5, #0x34]
	mov r0, r4
	str ip, [sp, #4]
	ldr r4, [r5, #0x30]
	blx r4
	ldr r0, [r5, #0x38]
	add r1, r0, #1
	str r1, [r5, #0x38]
	ldr r0, [r5, #0x2c]
	cmp r1, r0
	movge r0, #0
	strge r0, [r5, #0x38]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C8F50: .word _021E0294
_020C8F54: .word _021E0254
	arm_func_end StrmCallback

	arm_func_start BeginSleep__Strm
BeginSleep__Strm: ; 0x020C8F58
	stmdb sp!, {r4, lr}
	ldr r1, [r0, #0x24]
	mov r1, r1, lsl #0x1e
	movs r1, r1, asr #0x1f
	ldmeqia sp!, {r4, pc}
	ldr r2, [r0, #0x40]
	mov r3, #1
	mov r1, #0
	mov r2, r3, lsl r2
	ldr r0, [r0, #0x44]
	mov r3, r1
	bl SND_StopTimer
	bl SND_GetCurrentCommandTag
	mov r4, r0
	mov r0, #1
	bl SND_FlushCommand
	mov r0, r4
	bl SND_WaitForCommandProc
	ldmia sp!, {r4, pc}
	arm_func_end BeginSleep__Strm

	arm_func_start EndSleep__Strm
EndSleep__Strm: ; 0x020C8FA4
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, r0
	ldr r0, [r4, #0x24]
	mov r0, r0, lsl #0x1e
	movs r0, r0, asr #0x1f
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, [r4, #0x38]
	cmp r0, #0
	beq _020C8FF4
	mov r5, #1
_020C8FCC:
	bl OS_DisableInterrupts
	mov r6, r0
	mov r0, r4
	mov r1, r5
	bl StrmCallback
	mov r0, r6
	bl OS_RestoreInterrupts
	ldr r0, [r4, #0x38]
	cmp r0, #0
	bne _020C8FCC
_020C8FF4:
	ldr r0, [r4, #0x40]
	mov r2, #1
	mov r1, #0
	mov r2, r2, lsl r0
	ldr r0, [r4, #0x44]
	mov r3, r1
	bl SND_StartTimer
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end EndSleep__Strm

	arm_func_start NNS_SndCaptureStopReverb
NNS_SndCaptureStopReverb: ; 0x020C9014
	stmdb sp!, {r4, lr}
	ldr r1, _020C9064 ; =_021E0314
	mov r2, r0
	ldr r0, [r1, #0x48]
	ldr r4, _020C9068 ; =_021E035C
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r0, [r4, #4]
	cmp r0, #0
	ldmneia sp!, {r4, pc}
	cmp r2, #0
	bne _020C904C
	bl NNSi_SndCaptureStop
	ldmia sp!, {r4, pc}
_020C904C:
	add r0, r4, #0x3c
	mov r1, #0
	bl NNSi_SndFaderSet
	mov r0, #1
	str r0, [r4, #0x4c]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C9064: .word _021E0314
_020C9068: .word _021E035C
	arm_func_end NNS_SndCaptureStopReverb
