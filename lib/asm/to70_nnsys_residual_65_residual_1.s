	.extern FreeChannel
	.extern NNS_SndArcGetCurrent
	.extern NNS_SndArcGetFileAddress
	.extern NNS_SndArcGetSeqArcInfo
	.extern NNS_SndArcGetSeqInfo
	.extern NNS_SndArcGetStrmInfo
	.extern NNS_SndArcReadFile
	.extern NNS_SndHeapAlloc
	.extern NNS_SndPlayerCreateHeap
	.extern NNS_SndPlayerSetAllocatableChannel
	.extern NNS_SndPlayerSetChannelPriority
	.extern NNS_SndPlayerSetInitialVolume
	.extern NNS_SndPlayerSetPlayableSeqCount
	.extern NNS_SndPlayerSetSeqArcNo
	.extern NNS_SndPlayerSetSeqNo
	.extern NNS_SndStrmSetVolume
	.extern NNS_SndStrmStart
	.extern NNS_SndStrmStop
	.extern NNSi_SndArcLoadBank
	.extern NNSi_SndArcLoadSeq
	.extern NNSi_SndFaderGet
	.extern NNSi_SndFaderIsFinished
	.extern NNSi_SndFaderUpdate
	.extern NNSi_SndPlayerAllocHeap
	.extern NNSi_SndPlayerAllocSeqPlayer
	.extern NNSi_SndPlayerFreeSeqPlayer
	.extern NNSi_SndPlayerStartSeq
	.extern NNSi_SndSeqArcGetSeqInfo
	.extern RemoveCommandByPlayer
	.extern SingleWaveDisposeCallback
	.extern _021E0950
	.extern _021E0FC8
	.extern _021E0FE0
	.extern _021E0FEC
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public LoadSingleWave

.text
	arm_func_start LoadSingleWave
LoadSingleWave: ; 0x020CA6C0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r8, r0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	bl SND_GetWaveDataAddress
	cmp r0, #0
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, r8
	bl SND_GetWaveDataCount
	ldr r1, [r8, #0x38]
	sub r0, r0, #1
	add r1, r1, r7
	add r1, r8, r1, lsl #2
	cmp r7, r0
	ldrlo r0, [r1, #0x40]
	ldr r4, [r1, #0x3c]
	ldrhs r0, [r8, #8]
	cmp r5, #0
	sub sb, r0, r4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	ldr r2, _020CA784 ; =SingleWaveDisposeCallback
	mov r0, r5
	mov r3, r8
	add r1, sb, #0x20
	str r7, [sp]
	bl NNS_SndHeapAlloc
	movs r5, r0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, r6
	mov r1, r5
	mov r2, sb
	mov r3, r4
	bl NNS_SndArcReadFile
	cmp sb, r0
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	mov r0, r5
	mov r1, sb
	bl DC_StoreRange
	mov r0, r8
	mov r1, r7
	mov r2, r5
	bl SND_SetWaveDataAddress
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020CA784: .word SingleWaveDisposeCallback
	arm_func_end LoadSingleWave
