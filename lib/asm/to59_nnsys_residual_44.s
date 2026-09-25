	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_SndReadDriverTrackInfo
	.extern _021DD420
	.extern _021DD460

.text
	arm_func_start NNS_SndUpdateDriverInfo
NNS_SndUpdateDriverInfo: ; 0x020C79B4
	stmdb sp!, {r4, lr}
	ldr r0, _020C7A88 ; =_021DD420
	ldr r0, [r0, #4]
	cmp r0, #0
	bne _020C7A68
	mov r4, #0
_020C79CC:
	mov r0, r4
	bl SND_RecvCommandReply
	cmp r0, #0
	bne _020C79CC
	ldr r0, _020C7A88 ; =_021DD420
	ldr r0, [r0, #8]
	bl SND_IsFinishedCommandTag
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r0, _020C7A88 ; =_021DD420
	ldr r2, _020C7A8C ; =_021DD460
	ldrsb r1, [r0]
	cmp r1, #0
	movlt r1, #1
	strltb r1, [r0]
	ldr r0, _020C7A88 ; =_021DD420
	ldrsb r1, [r0]
	ldr r0, _020C7A90 ; =0x000011E0
	mla r0, r1, r0, r2
	bl SND_ReadDriverInfo
	bl SND_GetCurrentCommandTag
	ldr r1, _020C7A88 ; =_021DD420
	ldr r3, _020C7A8C ; =_021DD460
	str r0, [r1, #8]
	ldrsb r0, [r1]
	cmp r0, #0
	moveq r0, #1
	movne r0, #0
	strb r0, [r1]
	ldr r0, _020C7A88 ; =_021DD420
	ldr r1, _020C7A90 ; =0x000011E0
	ldrsb r2, [r0]
	mla r0, r2, r1, r3
	bl DC_InvalidateRange
	mov r0, #0
	bl SND_FlushCommand
	mov r0, #1
	ldmia sp!, {r4, pc}
_020C7A68:
	ldr r0, _020C7A8C ; =_021DD460
	bl SND_ReadDriverInfo
	bl SND_GetCurrentCommandTag
	ldr r1, _020C7A88 ; =_021DD420
	str r0, [r1, #8]
	mov r0, #0
	str r0, [r1, #4]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C7A88: .word _021DD420
_020C7A8C: .word _021DD460
_020C7A90: .word 0x000011E0
	arm_func_end NNS_SndUpdateDriverInfo

	arm_func_start NNS_SndReadDriverChannelInfo
NNS_SndReadDriverChannelInfo: ; 0x020C7A94
	stmdb sp!, {r3, lr}
	ldr r2, _020C7AD4 ; =_021DD420
	mov ip, r0
	ldrsb r3, [r2]
	mov r2, r1
	cmp r3, #0
	movlt r0, #0
	ldrge r1, _020C7AD8 ; =_021DD460
	ldrge r0, _020C7ADC ; =0x000011E0
	mlage r0, r3, r0, r1
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	mov r1, ip
	bl SND_ReadChannelInfo
	ldmia sp!, {r3, pc}
	.align 2, 0
_020C7AD4: .word _021DD420
_020C7AD8: .word _021DD460
_020C7ADC: .word 0x000011E0
	arm_func_end NNS_SndReadDriverChannelInfo

	arm_func_start NNSi_SndReadDriverTrackInfo
NNSi_SndReadDriverTrackInfo: ; 0x020C7AE0
	stmdb sp!, {r4, lr}
	ldr r3, _020C7B28 ; =_021DD420
	mov r4, r0
	ldrsb ip, [r3]
	mov lr, r1
	mov r3, r2
	cmp ip, #0
	movlt r0, #0
	ldrge r1, _020C7B2C ; =_021DD460
	ldrge r0, _020C7B30 ; =0x000011E0
	mlage r0, ip, r0, r1
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	mov r1, r4
	mov r2, lr
	bl SND_ReadTrackInfo
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C7B28: .word _021DD420
_020C7B2C: .word _021DD460
_020C7B30: .word 0x000011E0
	arm_func_end NNSi_SndReadDriverTrackInfo
