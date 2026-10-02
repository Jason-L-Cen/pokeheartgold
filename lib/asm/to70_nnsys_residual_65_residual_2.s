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
	.extern ForceStopStrm__SndArc

.text
	arm_func_start NNSi_SndArcStrmMain
NNSi_SndArcStrmMain: ; 0x020CAB94
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r7, _020CAC9C ; =_021E0FEC
	ldr r4, _020CACA0 ; =_0210D750
	mov r5, #0
_020CABA4:
	ldr r1, [r7, #0x110]
	mov r0, r1, lsl #0x1f
	movs r0, r0, asr #0x1f
	beq _020CAC88
	ldr r0, [r7, #0x114]
	cmp r0, #0
	bne _020CABCC
	mov r0, r7
	bl ForceStopStrm__SndArc
	b _020CAC88
_020CABCC:
	mov r0, r1, lsl #0x1d
	movs r0, r0, asr #0x1f
	ldrne r0, [r7, #0x118]
	cmpne r0, #0
	beq _020CABF8
	mov r0, r7
	bl NNS_SndStrmStart
	ldr r0, [r7, #0x110]
	orr r0, r0, #2
	bic r0, r0, #4
	str r0, [r7, #0x110]
_020CABF8:
	ldr r0, [r7, #0x110]
	mov r0, r0, lsl #0x1e
	movs r0, r0, asr #0x1f
	beq _020CAC88
	add r0, r7, #0xe8
	bl NNSi_SndFaderUpdate
	ldr r1, [r7, #0x154]
	add r0, r7, #0xe8
	mov r1, r1, lsl #1
	ldrsh r6, [r4, r1]
	bl NNSi_SndFaderGet
	mov r0, r0, asr #8
	ldr r1, [r7, #0x158]
	mov r0, r0, lsl #1
	mov r2, r1, lsl #1
	ldrsh r1, [r4, r0]
	ldrsh r2, [r4, r2]
	ldr r0, [r7, #0x15c]
	add r1, r1, r6
	add r6, r2, r1
	cmp r6, r0
	beq _020CAC60
	mov r0, r7
	mov r1, r6
	bl NNS_SndStrmSetVolume
	str r6, [r7, #0x15c]
_020CAC60:
	ldr r0, [r7, #0x110]
	mov r0, r0, lsl #0x1c
	movs r0, r0, asr #0x1f
	beq _020CAC88
	add r0, r7, #0xe8
	bl NNSi_SndFaderIsFinished
	cmp r0, #0
	beq _020CAC88
	mov r0, r7
	bl ForceStopStrm__SndArc
_020CAC88:
	add r5, r5, #1
	cmp r5, #4
	add r7, r7, #0x174
	blt _020CABA4
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020CAC9C: .word _021E0FEC
_020CACA0: .word _0210D750
	arm_func_end NNSi_SndArcStrmMain
