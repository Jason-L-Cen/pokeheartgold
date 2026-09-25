	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_SndPlayerStartSeq
	.public NNSi_SndPlayerStopSeq
	.extern ForceStopSeq
	.extern InitPlayer
	.extern NNSi_SndFaderSet
	.extern SetPlayerPriority

.text
	arm_func_start NNSi_SndPlayerStartSeq
NNSi_SndPlayerStartSeq: ; 0x020C86A0
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldrb r0, [r5, #0x3c]
	ldr r4, [r5, #4]
	bl SND_PrepareSeq
	ldr r2, [r4, #0x1c]
	cmp r2, #0
	beq _020C86CC
	ldrb r0, [r5, #0x3c]
	ldr r1, _020C86EC ; =0x0000FFFF
	bl SND_SetTrackAllocatableChannel
_020C86CC:
	mov r0, r5
	bl InitPlayer
	bl SND_GetCurrentCommandTag
	str r0, [r5, #0x30]
	mov r0, #1
	strb r0, [r5, #0x2f]
	strb r0, [r5, #0x2c]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C86EC: .word 0x0000FFFF
	arm_func_end NNSi_SndPlayerStartSeq

	arm_func_start NNSi_SndPlayerStopSeq
NNSi_SndPlayerStopSeq: ; 0x020C86F0
	stmdb sp!, {r4, lr}
	movs r4, r0
	mov r2, r1
	ldrneb r1, [r4, #0x2c]
	cmpne r1, #0
	ldmeqia sp!, {r4, pc}
	cmp r2, #0
	bne _020C8718
	bl ForceStopSeq
	ldmia sp!, {r4, pc}
_020C8718:
	add r0, r4, #0x1c
	mov r1, #0
	bl NNSi_SndFaderSet
	mov r0, r4
	mov r1, #0
	bl SetPlayerPriority
	mov r0, #2
	strb r0, [r4, #0x2c]
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_SndPlayerStopSeq
