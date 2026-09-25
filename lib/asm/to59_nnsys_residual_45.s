	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_SndWaveOutStart
NNS_SndWaveOutStart: ; 0x020C7CEC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x18
	movs r8, r1
	mov sb, r0
	mov r7, r2
	mov r6, r3
	beq _020C7D14
	cmp r8, #1
	beq _020C7D28
	b _020C7D38
_020C7D14:
	ldr r1, [sp, #0x38]
	ldr r0, [sp, #0x3c]
	mov r4, r1, asr #2
	rsb r5, r4, r0, asr #2
	b _020C7D38
_020C7D28:
	ldr r1, [sp, #0x38]
	ldr r0, [sp, #0x3c]
	mov r4, r1, asr #1
	rsb r5, r4, r0, asr #1
_020C7D38:
	ldr r2, [sp, #0x40]
	ldr r0, _020C7E04 ; =0xD87F8000
	mov r3, r2, asr #0x1f
	mov r1, #0x7f
	bl _ll_udiv
	ldr r2, [sp, #0x48]
	mov r3, r2, asr #0x1f
	bl _ll_udiv
	cmp r1, #0
	mov r2, #0x10
	cmpeq r0, #0x10
	mov r3, #0
	movlo r0, r2
	blo _020C7D80
	ldr r2, _020C7E08 ; =0x0000FFFF
	cmp r1, r3
	cmpeq r0, r2
	movhi r0, r2
_020C7D80:
	ldr r1, [sp, #0x44]
	stmia sp, {r4, r5}
	str r1, [sp, #8]
	mov r1, #0
	str r1, [sp, #0xc]
	cmp r6, #0
	movne r3, #1
	ldr r1, [sp, #0x4c]
	str r0, [sp, #0x10]
	str r1, [sp, #0x14]
	ldr r0, [sb]
	moveq r3, #2
	mov r1, r8
	mov r2, r7
	bl SND_SetupChannelPcm
	mov r1, #0
	ldr r0, [sb]
	mov r2, #1
	mov r0, r2, lsl r0
	mov r2, r1
	mov r3, r1
	bl SND_StartTimer
	mov r0, #1
	str r0, [sb, #8]
	mov r0, #0
	str r0, [sb, #0xc]
	bl SND_GetCurrentCommandTag
	str r0, [sb, #0x10]
	ldr r1, [sp, #0x40]
	mov r0, #1
	str r1, [sb, #4]
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020C7E04: .word 0xD87F8000
_020C7E08: .word 0x0000FFFF
	arm_func_end NNS_SndWaveOutStart
