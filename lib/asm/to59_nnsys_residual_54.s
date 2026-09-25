	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_SndStrmStart
	.extern AlarmCallback
	.extern NNS_FndAppendListObject
	.extern NNS_SndAllocAlarm
	.extern NNS_SndStrmStop
	.extern StrmCallback
	.extern _021E0248
	.extern _021E0294

.text
	arm_func_start NNS_SndStrmSetup
NNS_SndStrmSetup: ; 0x020C8B78
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1c
	mov sl, r0
	ldr r4, [sl, #0x24]
	mov sb, r1
	mov r1, r4, lsl #0x1f
	mov r8, r2
	mov r4, r3
	movs r1, r1, asr #0x1f
	ldr fp, [sp, #0x44]
	beq _020C8BA8
	bl NNS_SndStrmStop
_020C8BA8:
	ldr r2, [sl, #0x48]
	mov r0, fp, lsl #5
	mul r1, r2, r0
	mov r0, r4
	bl _u32_div_f
	mul r1, r0, fp
	mov r0, r1, lsl #5
	ldr r2, [sp, #0x40]
	str r0, [sl, #0x28]
	cmp sb, #1
	moveq r0, r0, lsr #1
	mul r0, r2, r0
	mov r1, fp
	bl _u32_div_f
	str r0, [sp, #0x18]
	bl NNS_SndAllocAlarm
	cmp r0, #0
	str r0, [sl, #0x40]
	addlt sp, sp, #0x1c
	movlt r0, #0
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sl, #0x48]
	mov r6, #0
	cmp r0, #0
	ble _020C8C80
	ldr r0, [sp, #0x40]
	ldr r5, _020C8D0C ; =_021E0294
	mov r7, r0, lsl #5
	mov r4, r6
_020C8C1C:
	ldr r1, [sl, #0x28]
	add r0, sl, r6
	mla r2, r1, r6, r8
	ldrb r0, [r0, #0x4c]
	mov r3, #1
	str r2, [r5, r0, lsl #3]
	add r1, r5, r0, lsl #3
	str r4, [r1, #4]
	str r4, [sp]
	ldr r2, [sl, #0x28]
	mov r1, sb
	mov r2, r2, lsr #2
	str r2, [sp, #4]
	mov r2, #0x7f
	str r2, [sp, #8]
	str r4, [sp, #0xc]
	str r7, [sp, #0x10]
	mov r2, #0x40
	str r2, [sp, #0x14]
	ldr r2, [r5, r0, lsl #3]
	bl SND_SetupChannelPcm
	ldr r0, [sl, #0x48]
	add r6, r6, #1
	cmp r6, r0
	blt _020C8C1C
_020C8C80:
	str sl, [sp]
	ldr r1, [sp, #0x18]
	ldr r0, [sl, #0x40]
	ldr r3, _020C8D10 ; =AlarmCallback
	mov r2, r1
	bl SND_SetupAlarm
	ldr r0, _020C8D14 ; =_021E0248
	mov r1, sl
	bl NNS_FndAppendListObject
	str sb, [sl, #0x20]
	ldr r1, [sp, #0x48]
	str fp, [sl, #0x2c]
	ldr r0, [sp, #0x4c]
	str r1, [sl, #0x30]
	str r0, [sl, #0x34]
	mov r0, #0
	str r0, [sl, #0x38]
	str r0, [sl, #0x3c]
	ldr r0, [sl, #0x24]
	bic r0, r0, #1
	orr r0, r0, #1
	str r0, [sl, #0x24]
	bl OS_DisableInterrupts
	mov r4, r0
	mov r2, #1
	mov r0, sl
	mov r1, #0
	str r2, [sl, #0x2c]
	bl StrmCallback
	mov r0, r4
	str fp, [sl, #0x2c]
	bl OS_RestoreInterrupts
	mov r0, #1
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C8D0C: .word _021E0294
_020C8D10: .word AlarmCallback
_020C8D14: .word _021E0248
	arm_func_end NNS_SndStrmSetup

	arm_func_start NNS_SndStrmStart
NNS_SndStrmStart: ; 0x020C8D18
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4, #0x40]
	mov r2, #1
	mov r1, #0
	mov r2, r2, lsl r0
	ldr r0, [r4, #0x44]
	mov r3, r1
	bl SND_StartTimer
	ldr r0, [r4, #0x24]
	mov r0, r0, lsl #0x1e
	movs r0, r0, asr #0x1f
	ldmneia sp!, {r4, pc}
	add r0, r4, #8
	bl PM_PrependPreSleepCallback
	add r0, r4, #0x14
	bl PM_AppendPostSleepCallback
	ldr r0, [r4, #0x24]
	orr r0, r0, #2
	str r0, [r4, #0x24]
	ldmia sp!, {r4, pc}
	arm_func_end NNS_SndStrmStart
