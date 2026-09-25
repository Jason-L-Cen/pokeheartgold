	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public AllocSeqPlayer
	.public ForceStopSeq
	.public InitPlayer
	.public InsertPlayerList
	.public NNSi_SndPlayerAllocHeap
	.public PlayerHeapDisposeCallback
	.public SetPlayerPriority
	.public ShutdownPlayer
	.extern BeginSleep__Strm
	.extern EndSleep__Strm
	.extern NNS_FndAppendListObject
	.extern NNS_FndGetNextListObject
	.extern NNS_FndInitList
	.extern NNS_FndInsertListObject
	.extern NNS_FndRemoveListObject
	.extern NNS_SndHeapClear
	.extern NNS_SndHeapDestroy
	.extern NNS_SndLockChannel
	.extern NNSi_SndFaderInit
	.extern NNSi_SndFaderSet
	.extern _021DF96C
	.extern _021DF978
	.extern _021DFDC4
	.extern _021E0244
	.extern _021E0248

.text
	arm_func_start NNSi_SndPlayerAllocHeap
NNSi_SndPlayerAllocHeap: ; 0x020C8764
	stmdb sp!, {r4, r5, r6, lr}
	ldr r3, _020C87B4 ; =_021DFDC4
	mov r2, #0x24
	mla r5, r0, r2, r3
	mov r6, r1
	add r0, r5, #0xc
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r1, r4
	add r0, r5, #0xc
	bl NNS_FndRemoveListObject
	str r6, [r4, #0xc]
	str r4, [r6, #8]
	ldr r0, [r4, #8]
	bl NNS_SndHeapClear
	ldr r0, [r4, #8]
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020C87B4: .word _021DFDC4
	arm_func_end NNSi_SndPlayerAllocHeap

	arm_func_start InitPlayer
InitPlayer: ; 0x020C87B8
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, #0
	strb r0, [r4, #0x2e]
	strb r0, [r4, #0x2d]
	strb r0, [r4, #0x2f]
	strh r0, [r4, #0x34]
	strh r0, [r4, #0x3e]
	mov r1, #0x7f
	strb r1, [r4, #0x40]
	add r0, r4, #0x1c
	strb r1, [r4, #0x41]
	bl NNSi_SndFaderInit
	add r0, r4, #0x1c
	mov r1, #0x7f00
	mov r2, #1
	bl NNSi_SndFaderSet
	ldmia sp!, {r4, pc}
	arm_func_end InitPlayer

	arm_func_start InsertPlayerList
InsertPlayerList: ; 0x020C8800
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r1, #0
	mov r5, r0
	bl NNS_FndGetNextListObject
	movs r1, r0
	beq _020C883C
_020C881C:
	ldrb r2, [r4, #0x3d]
	ldrb r0, [r1, #0x3d]
	cmp r2, r0
	blo _020C883C
	mov r0, r5
	bl NNS_FndGetNextListObject
	movs r1, r0
	bne _020C881C
_020C883C:
	mov r0, r5
	mov r2, r4
	bl NNS_FndInsertListObject
	str r5, [r4, #4]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end InsertPlayerList

	arm_func_start InsertPrioList
InsertPrioList: ; 0x020C8850
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, _020C88A0 ; =_021DF978
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r1, r0
	beq _020C8890
	ldr r4, _020C88A0 ; =_021DF978
_020C8870:
	ldrb r2, [r5, #0x3d]
	ldrb r0, [r1, #0x3d]
	cmp r2, r0
	blo _020C8890
	mov r0, r4
	bl NNS_FndGetNextListObject
	movs r1, r0
	bne _020C8870
_020C8890:
	ldr r0, _020C88A0 ; =_021DF978
	mov r2, r5
	bl NNS_FndInsertListObject
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C88A0: .word _021DF978
	arm_func_end InsertPrioList

	arm_func_start ForceStopSeq
ForceStopSeq: ; 0x020C88A4
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldrb r0, [r4, #0x2c]
	cmp r0, #2
	bne _020C88C4
	ldrb r0, [r4, #0x3c]
	ldr r1, _020C88D8 ; =0xFFFFFD2D
	bl SND_SetPlayerVolume
_020C88C4:
	ldrb r0, [r4, #0x3c]
	bl SND_StopSeq
	mov r0, r4
	bl ShutdownPlayer
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C88D8: .word 0xFFFFFD2D
	arm_func_end ForceStopSeq

	arm_func_start AllocSeqPlayer
AllocSeqPlayer: ; 0x020C88DC
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, _020C893C ; =_021DF96C
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r4, r0
	bne _020C891C
	ldr r0, _020C8940 ; =_021DF978
	mov r1, #0
	bl NNS_FndGetNextListObject
	mov r4, r0
	ldrb r1, [r4, #0x3d]
	cmp r5, r1
	movlt r0, #0
	ldmltia sp!, {r3, r4, r5, pc}
	bl ForceStopSeq
_020C891C:
	ldr r0, _020C893C ; =_021DF96C
	mov r1, r4
	bl NNS_FndRemoveListObject
	mov r0, r4
	strb r5, [r4, #0x3d]
	bl InsertPrioList
	mov r0, r4
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C893C: .word _021DF96C
_020C8940: .word _021DF978
	arm_func_end AllocSeqPlayer

	arm_func_start ShutdownPlayer
ShutdownPlayer: ; 0x020C8944
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #0
	movne r0, #0
	strne r0, [r1]
	strne r0, [r4]
	ldr r5, [r4, #4]
	mov r1, r4
	mov r0, r5
	bl NNS_FndRemoveListObject
	mov r0, #0
	str r0, [r4, #4]
	ldr r1, [r4, #8]
	cmp r1, #0
	beq _020C899C
	add r0, r5, #0xc
	bl NNS_FndAppendListObject
	ldr r0, [r4, #8]
	mov r1, #0
	str r1, [r0, #0xc]
	str r1, [r4, #8]
_020C899C:
	ldr r0, _020C89C0 ; =_021DF978
	mov r1, r4
	bl NNS_FndRemoveListObject
	ldr r0, _020C89C4 ; =_021DF96C
	mov r1, r4
	bl NNS_FndAppendListObject
	mov r0, #0
	strb r0, [r4, #0x2c]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C89C0: .word _021DF978
_020C89C4: .word _021DF96C
	arm_func_end ShutdownPlayer

	arm_func_start PlayerHeapDisposeCallback
PlayerHeapDisposeCallback: ; 0x020C89C8
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4, #8]
	cmp r0, #0
	ldmeqia sp!, {r4, pc}
	bl NNS_SndHeapDestroy
	ldr r1, [r4, #0xc]
	cmp r1, #0
	movne r0, #0
	strne r0, [r1, #8]
	ldmneia sp!, {r4, pc}
	ldr r1, [r4, #0x10]
	ldr r2, _020C8A14 ; =_021DFDC4
	mov r0, #0x24
	mla r0, r1, r0, r2
	mov r1, r4
	add r0, r0, #0xc
	bl NNS_FndRemoveListObject
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C8A14: .word _021DFDC4
	arm_func_end PlayerHeapDisposeCallback

	arm_func_start SetPlayerPriority
SetPlayerPriority: ; 0x020C8A18
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	ldr r4, [r6, #4]
	mov r5, r1
	cmp r4, #0
	beq _020C8A44
	mov r0, r4
	mov r1, r6
	bl NNS_FndRemoveListObject
	mov r0, #0
	str r0, [r6, #4]
_020C8A44:
	ldr r0, _020C8A74 ; =_021DF978
	mov r1, r6
	bl NNS_FndRemoveListObject
	strb r5, [r6, #0x3d]
	cmp r4, #0
	beq _020C8A68
	mov r0, r4
	mov r1, r6
	bl InsertPlayerList
_020C8A68:
	mov r0, r6
	bl InsertPrioList
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020C8A74: .word _021DF978
	arm_func_end SetPlayerPriority

	arm_func_start NNS_SndStrmInit
NNS_SndStrmInit: ; 0x020C8A78
	stmdb sp!, {r4, lr}
	ldr r1, _020C8AE0 ; =_021E0244
	mov r4, r0
	ldr r0, [r1]
	cmp r0, #0
	bne _020C8AA8
	ldr r0, _020C8AE4 ; =_021E0248
	mov r1, #0
	bl NNS_FndInitList
	ldr r0, _020C8AE0 ; =_021E0244
	mov r1, #1
	str r1, [r0]
_020C8AA8:
	ldr r1, _020C8AE8 ; =BeginSleep__Strm
	ldr r0, _020C8AEC ; =EndSleep__Strm
	str r1, [r4, #8]
	str r4, [r4, #0xc]
	str r0, [r4, #0x14]
	str r4, [r4, #0x18]
	mov r0, #0
	str r0, [r4, #0x44]
	str r0, [r4, #0x48]
	ldr r0, [r4, #0x24]
	bic r0, r0, #1
	bic r0, r0, #2
	str r0, [r4, #0x24]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C8AE0: .word _021E0244
_020C8AE4: .word _021E0248
_020C8AE8: .word BeginSleep__Strm
_020C8AEC: .word EndSleep__Strm
	arm_func_end NNS_SndStrmInit

	arm_func_start NNS_SndStrmAllocChannel
NNS_SndStrmAllocChannel: ; 0x020C8AF0
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, r1
	mov r6, #0
	mov r5, r0
	mov ip, r6
	cmp r4, #0
	ble _020C8B30
	mov r1, #1
_020C8B10:
	ldrb r3, [r2, ip]
	add r0, r5, ip
	strb r3, [r0, #0x4c]
	ldrb r0, [r2, ip]
	add ip, ip, #1
	cmp ip, r4
	orr r6, r6, r1, lsl r0
	blt _020C8B10
_020C8B30:
	mov r0, r6
	bl NNS_SndLockChannel
	cmp r0, #0
	moveq r0, #0
	strne r4, [r5, #0x48]
	strne r6, [r5, #0x44]
	movne r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end NNS_SndStrmAllocChannel
