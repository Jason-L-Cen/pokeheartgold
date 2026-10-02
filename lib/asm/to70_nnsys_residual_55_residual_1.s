	.extern NNS_FndRemoveListObject
	.extern NNS_SndFreeAlarm
	.extern _021E0248
	.extern _021E0294
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

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
