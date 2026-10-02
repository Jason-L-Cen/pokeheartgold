	.extern AllocSeqPlayer
	.extern ForceStopSeq
	.extern InsertPlayerList
	.extern NNS_FndAppendListObject
	.extern NNS_FndGetNextListObject
	.extern NNS_FndInitList
	.extern NNS_SndHandleReleaseSeq
	.extern NNSi_SndFaderGet
	.extern NNSi_SndFaderIsFinished
	.extern NNSi_SndFaderUpdate
	.extern ShutdownPlayer
	.extern _021DF96C
	.extern _021DF978
	.extern _021DF984
	.extern _021DFDC4
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNSi_SndPlayerMain
NNSi_SndPlayerMain: ; 0x020C8498
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	bl SND_GetPlayerStatus
	mov fp, r0
	ldr r0, _020C85F0 ; =_021DF978
	mov r1, #0
	bl NNS_FndGetNextListObject
	movs r6, r0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r4, #0x8000
	ldr r5, _020C85F4 ; =_0210D750
	rsb r4, r4, #0
_020C84C4:
	ldr r0, _020C85F0 ; =_021DF978
	mov r1, r6
	bl NNS_FndGetNextListObject
	ldrb r1, [r6, #0x2d]
	mov r7, r0
	cmp r1, #0
	bne _020C84F4
	ldr r0, [r6, #0x30]
	bl SND_IsFinishedCommandTag
	cmp r0, #0
	movne r0, #1
	strneb r0, [r6, #0x2d]
_020C84F4:
	ldrb r0, [r6, #0x2d]
	cmp r0, #0
	beq _020C851C
	ldrb r1, [r6, #0x3c]
	mov r0, #1
	tst fp, r0, lsl r1
	bne _020C851C
	mov r0, r6
	bl ShutdownPlayer
	b _020C85E0
_020C851C:
	add r0, r6, #0x1c
	bl NNSi_SndFaderUpdate
	ldr r0, [r6, #4]
	ldrb r2, [r6, #0x41]
	ldrb r0, [r0, #0x20]
	ldrb r1, [r6, #0x40]
	mov r2, r2, lsl #1
	mov r0, r0, lsl #1
	mov r1, r1, lsl #1
	ldrsh sl, [r5, r0]
	add r0, r6, #0x1c
	ldrsh r8, [r5, r2]
	ldrsh sb, [r5, r1]
	bl NNSi_SndFaderGet
	mov r0, r0, asr #8
	mov r0, r0, lsl #1
	add r1, sb, r8
	ldrsh r0, [r5, r0]
	add r1, sl, r1
	add r8, r0, r1
	cmp r8, r4
	movlt r8, r4
	blt _020C8584
	ldr r0, _020C85F8 ; =0x00007FFF
	cmp r8, r0
	movgt r8, r0
_020C8584:
	ldrsh r0, [r6, #0x3e]
	cmp r8, r0
	beq _020C85A0
	ldrb r0, [r6, #0x3c]
	mov r1, r8
	bl SND_SetPlayerVolume
	strh r8, [r6, #0x3e]
_020C85A0:
	ldrb r0, [r6, #0x2c]
	cmp r0, #2
	bne _020C85C4
	add r0, r6, #0x1c
	bl NNSi_SndFaderIsFinished
	cmp r0, #0
	beq _020C85C4
	mov r0, r6
	bl ForceStopSeq
_020C85C4:
	ldrb r0, [r6, #0x2f]
	cmp r0, #0
	beq _020C85E0
	ldrb r0, [r6, #0x3c]
	bl SND_StartPreparedSeq
	mov r0, #0
	strb r0, [r6, #0x2f]
_020C85E0:
	mov r6, r7
	cmp r7, #0
	bne _020C84C4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C85F0: .word _021DF978
_020C85F4: .word _0210D750
_020C85F8: .word 0x00007FFF
	arm_func_end NNSi_SndPlayerMain
