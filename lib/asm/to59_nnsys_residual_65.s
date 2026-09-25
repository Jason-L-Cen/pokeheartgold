	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public LoadSingleWaves
	.public NNSi_SndArcStrmMain
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

	arm_func_start LoadSingleWaves
LoadSingleWaves: ; 0x020CA788
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x1c
	mov r8, r0
	add r0, sp, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	ldr r4, [sp, #0x40]
	bl SND_GetFirstInstDataPos
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [sp, #8]
	str r0, [sp, #0xc]
	cmp r7, #0
	add r2, sp, #8
	addeq sp, sp, #0x1c
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	add r1, sp, #0x10
	mov r0, r7
	bl SND_GetNextInstData
	cmp r0, #0
	beq _020CA83C
	add sl, sp, #0x10
	add sb, sp, #8
_020CA7EC:
	ldrb r0, [sp, #0x10]
	cmp r0, #1
	ldreqh r0, [sp, #0x14]
	cmpeq r6, r0
	bne _020CA824
	ldrh r1, [sp, #0x12]
	mov r0, r8
	mov r2, r5
	mov r3, r4
	bl LoadSingleWave
	cmp r0, #0
	addeq sp, sp, #0x1c
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_020CA824:
	mov r0, r7
	mov r1, sl
	mov r2, sb
	bl SND_GetNextInstData
	cmp r0, #0
	bne _020CA7EC
_020CA83C:
	mov r0, #1
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end LoadSingleWaves

	arm_func_start NNS_SndArcPlayerSetup
NNS_SndArcPlayerSetup: ; 0x020CA848
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r4, r0
	bl NNS_SndArcGetCurrent
	mov r6, #0
	mov r5, r6
_020CA85C:
	mov r0, r6
	bl NNS_SndArcGetStrmInfo
	movs r7, r0
	beq _020CA8D0
	ldrb r1, [r7]
	mov r0, r6
	bl NNS_SndPlayerSetPlayableSeqCount
	ldrh r1, [r7, #2]
	mov r0, r6
	bl NNS_SndPlayerSetAllocatableChannel
	ldr r0, [r7, #4]
	cmp r0, #0
	cmpne r4, #0
	beq _020CA8D0
	ldrb r0, [r7]
	mov r8, r5
	cmp r0, #0
	ble _020CA8D0
_020CA8A4:
	ldr r2, [r7, #4]
	mov r0, r6
	mov r1, r4
	bl NNS_SndPlayerCreateHeap
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	ldrb r0, [r7]
	add r8, r8, #1
	cmp r8, r0
	blt _020CA8A4
_020CA8D0:
	add r6, r6, #1
	cmp r6, #0x20
	blt _020CA85C
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNS_SndArcPlayerSetup

	arm_func_start NNS_SndArcPlayerStartSeqEx
NNS_SndArcPlayerStartSeqEx: ; 0x020CA8E4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r7, r0
	ldr r0, [sp, #0x20]
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl NNS_SndArcGetSeqInfo
	cmp r0, #0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	cmp r4, #0
	ldrltb r4, [r0, #8]
	cmp r5, #0
	ldrlth r5, [r0, #4]
	cmp r6, #0
	ldrltb r6, [r0, #9]
	ldr ip, [sp, #0x20]
	mov r2, r5
	str r0, [sp]
	mov r0, r7
	mov r1, r6
	mov r3, r4
	str ip, [sp, #4]
	bl StartSeq
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNS_SndArcPlayerStartSeqEx

	arm_func_start NNS_SndArcPlayerStartSeqArc
NNS_SndArcPlayerStartSeqArc: ; 0x020CA954
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	mov r6, r1
	mov r7, r0
	mov r0, r6
	mov r5, r2
	bl NNS_SndArcGetSeqArcInfo
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r0]
	bl NNS_SndArcGetFileAddress
	movs r4, r0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r1, r5
	bl NNSi_SndSeqArcGetSeqInfo
	cmp r0, #0
	addeq sp, sp, #0x10
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	stmia sp, {r0, r4, r6}
	str r5, [sp, #0xc]
	ldrb r1, [r0, #9]
	ldrh r2, [r0, #4]
	ldrb r3, [r0, #8]
	mov r0, r7
	bl StartSeqArc
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNS_SndArcPlayerStartSeqArc

	arm_func_start StartSeq
StartSeq: ; 0x020CA9D4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	mov r8, r2
	mov r2, r3
	ldr r5, [sp, #0x28]
	mov r6, r0
	mov r7, r1
	bl NNSi_SndPlayerAllocSeqPlayer
	movs r4, r0
	addeq sp, sp, #0xc
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, r7
	mov r1, r4
	bl NNSi_SndPlayerAllocHeap
	mov r7, r0
	add ip, sp, #4
	mov r0, r8
	mov r2, r7
	mov r1, #6
	mov r3, #0
	str ip, [sp]
	bl NNSi_SndArcLoadBank
	cmp r0, #0
	beq _020CAA4C
	mov r0, r4
	bl NNSi_SndPlayerFreeSeqPlayer
	add sp, sp, #0xc
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020CAA4C:
	ldr r0, [sp, #0x2c]
	add ip, sp, #8
	mov r2, r7
	mov r1, #1
	mov r3, #0
	str ip, [sp]
	bl NNSi_SndArcLoadSeq
	cmp r0, #0
	beq _020CAA84
	mov r0, r4
	bl NNSi_SndPlayerFreeSeqPlayer
	add sp, sp, #0xc
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020CAA84:
	ldr r2, [sp, #8]
	ldr r3, [sp, #4]
	ldr r1, [r2, #0x18]
	mov r0, r4
	add r1, r2, r1
	mov r2, #0
	bl NNSi_SndPlayerStartSeq
	ldrb r1, [r5, #6]
	mov r0, r6
	bl NNS_SndPlayerSetInitialVolume
	ldrb r1, [r5, #7]
	mov r0, r6
	bl NNS_SndPlayerSetChannelPriority
	ldr r1, [sp, #0x2c]
	mov r0, r6
	bl NNS_SndPlayerSetSeqNo
	mov r0, #1
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end StartSeq

	arm_func_start StartSeqArc
StartSeqArc: ; 0x020CAAD0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	mov r7, r2
	mov r2, r3
	ldr r6, [sp, #0x20]
	mov r4, r0
	mov r8, r1
	bl NNSi_SndPlayerAllocSeqPlayer
	movs r5, r0
	addeq sp, sp, #8
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	mov r0, r8
	mov r1, r5
	bl NNSi_SndPlayerAllocHeap
	add ip, sp, #4
	mov r2, r0
	mov r0, r7
	mov r1, #6
	mov r3, #0
	str ip, [sp]
	bl NNSi_SndArcLoadBank
	cmp r0, #0
	beq _020CAB44
	mov r0, r5
	bl NNSi_SndPlayerFreeSeqPlayer
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020CAB44:
	ldr ip, [sp, #0x24]
	ldr r2, [r6]
	ldr r1, [ip, #0x18]
	ldr r3, [sp, #4]
	mov r0, r5
	add r1, ip, r1
	bl NNSi_SndPlayerStartSeq
	ldrb r1, [r6, #6]
	mov r0, r4
	bl NNS_SndPlayerSetInitialVolume
	ldrb r1, [r6, #7]
	mov r0, r4
	bl NNS_SndPlayerSetChannelPriority
	ldr r1, [sp, #0x28]
	ldr r2, [sp, #0x2c]
	mov r0, r4
	bl NNS_SndPlayerSetSeqArcNo
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end StartSeqArc

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

	arm_func_start FreePlayer
FreePlayer: ; 0x020CACA4
	ldr r2, [r0, #0x14c]
	cmp r2, #0
	movne r1, #0
	strne r1, [r2]
	strne r1, [r0, #0x14c]
	ldr r1, [r0, #0x110]
	bic r2, r1, #1
	bic r1, r2, #4
	bic r1, r1, #2
	str r1, [r0, #0x110]
	bx lr
	arm_func_end FreePlayer

	arm_func_start ForceStopStrm__SndArc
ForceStopStrm__SndArc: ; 0x020CACD0
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, _020CAD60 ; =_021E0FC8
	bl OS_LockMutex
	ldr r0, _020CAD64 ; =_021E0950
	ldr r0, [r0, #4]
	cmp r0, #0
	beq _020CACFC
	add r0, r0, #0xc8
	add r0, r0, #0x400
	bl OS_LockMutex
_020CACFC:
	ldr r0, [r4, #0x110]
	mov r0, r0, lsl #0x1e
	movs r0, r0, asr #0x1f
	beq _020CAD14
	mov r0, r4
	bl NNS_SndStrmStop
_020CAD14:
	ldr r0, [r4, #0x110]
	mov r0, r0, lsl #0x1f
	movs r0, r0, asr #0x1f
	beq _020CAD30
	ldr r1, [r4, #0x170]
	mov r0, r4
	blx r1
_020CAD30:
	mov r0, r4
	bl ShutdownPlayer__SndArc
	ldr r0, _020CAD60 ; =_021E0FC8
	bl OS_UnlockMutex
	ldr r0, _020CAD64 ; =_021E0950
	ldr r0, [r0, #4]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	add r0, r0, #0xc8
	add r0, r0, #0x400
	bl OS_UnlockMutex
	ldmia sp!, {r4, pc}
	.align 2, 0
_020CAD60: .word _021E0FC8
_020CAD64: .word _021E0950
	arm_func_end ForceStopStrm__SndArc

	arm_func_start ShutdownPlayer__SndArc
ShutdownPlayer__SndArc: ; 0x020CAD68
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #0x110]
	mov r1, r1, lsl #0x1f
	movs r1, r1, asr #0x1f
	ldmeqia sp!, {r4, pc}
	bl FreeChannel
	ldr r1, [r4, #0x168]
	mov r0, r4
	blx r1
	ldr r0, _020CADC4 ; =_021E0FE0
	mov r1, r4
	bl RemoveCommandByPlayer
	ldr r0, _020CADC8 ; =_021E0950
	ldr r0, [r0, #4]
	cmp r0, #0
	beq _020CADB8
	mov r1, r4
	add r0, r0, #0x4e0
	bl RemoveCommandByPlayer
_020CADB8:
	mov r0, r4
	bl FreePlayer
	ldmia sp!, {r4, pc}
	.align 2, 0
_020CADC4: .word _021E0FE0
_020CADC8: .word _021E0950
	arm_func_end ShutdownPlayer__SndArc
