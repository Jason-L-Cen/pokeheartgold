	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_SndWaveOutSetSpeed
NNS_SndWaveOutSetSpeed: ; 0x020C7E70
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, [r5, #8]
	mov r4, r1
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r2, [r5, #4]
	ldr r0, _020C7EEC ; =0xD87F8000
	mov r3, r2, asr #0x1f
	mov r1, #0x7f
	bl _ll_udiv
	mov r2, r4
	mov r3, r4, asr #0x1f
	bl _ll_udiv
	mov r3, r0
	cmp r1, #0
	mov r0, #0x10
	cmpeq r3, #0x10
	mov r2, #0
	movlo r3, r0
	blo _020C7ED4
	ldr r0, _020C7EF0 ; =0x0000FFFF
	cmp r1, r2
	cmpeq r3, r0
	movhi r3, r0
_020C7ED4:
	ldr r0, [r5]
	mov r1, #1
	mov r0, r1, lsl r0
	mov r1, r3
	bl SND_SetChannelTimer
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C7EEC: .word 0xD87F8000
_020C7EF0: .word 0x0000FFFF
	arm_func_end NNS_SndWaveOutSetSpeed
