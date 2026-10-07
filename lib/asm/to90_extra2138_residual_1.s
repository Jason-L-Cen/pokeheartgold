	.extern CARD_LockBackup
	.extern CARD_UnlockBackup
	.extern DC_InvalidateRange
	.extern DC_StoreRange
	.extern MATH_CountPopulation
	.extern OS_JamMessage
	.extern OS_LockCard
	.extern OS_UnLockCard
	.extern PXI_Init
	.extern PXI_IsCallbackReady
	.extern PXI_SendWordByFifo
	.extern PXI_SetFifoRecvCallback
	.extern RTC_GetTime
	.extern RTC_Init
	.extern WM_GetAID
	.extern WM_GetConnectedAIDs
	.extern WM_SetMPDataToPortEx
	.extern WMi_CheckStateEx
	.extern WMi_GetSystemWork
	.extern _02110FBC
	.extern _02110FC0
	.extern _021E4088
	.extern _021E408C
	.extern _021E409C
	.extern _021E4158
	.extern _021E4220
	.extern _021E4228
	.extern _021E4248
	.extern _021E4270
	.extern _021E4284
	.extern _021E4294
	.extern _021E42C0
	.extern _021E4CC0
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.extern WmDataSharingReceiveData
	.extern WmDataSharingSendDataSet
	.extern WmDataSharingSetDataCallback

.text
	arm_func_start WM_StepDataSharing
WM_StepDataSharing: ; 0x020E0390
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sl, r0
	mov sb, r1
	mov r8, r2
	mov r0, #2
	mov r1, #9
	mov r2, #0xa
	bl WMi_CheckStateEx
	cmp r0, #0
	addne sp, sp, #0xc
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sb, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r8, #0
	addeq sp, sp, #0xc
	moveq r0, #6
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl WM_GetAID
	movs r5, r0
	bne _020E0404
	bl WM_GetConnectedAIDs
	mov r4, r0
_020E0404:
	add r0, sl, #0x800
	ldrh r0, [r0, #0x1c]
	cmp r0, #5
	addeq sp, sp, #0xc
	moveq r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #1
	cmpne r0, #4
	addne sp, sp, #0xc
	movne r0, #3
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r5, #0
	mov r7, #5
	bne _020E05C8
	mov r5, #0
	mov fp, r5
	cmp r0, #4
	bne _020E04EC
	add ip, sl, #0x800
	mov r2, #1
	strh r2, [ip, #0x1c]
	ldrh r3, [ip, #0xe]
	ldrh r1, [ip, #8]
	ldr r0, _020E06D0 ; =WmDataSharingSetDataCallback
	and r3, r3, r4
	mov r3, r3, lsl #0x10
	mov r3, r3, lsr #0x10
	str r3, [sp]
	ldrh r3, [ip, #0x16]
	add r1, r1, #3
	and r6, r1, #3
	str r3, [sp, #4]
	str r2, [sp, #8]
	ldrh r3, [ip, #0x14]
	mov r1, sl
	add r2, sl, r6, lsl #9
	bl WM_SetMPDataToPortEx
	cmp r0, #7
	bne _020E04C8
	add r0, sl, r6, lsl #1
	ldr r1, _020E06D4 ; =0x0000FFFF
	add r0, r0, #0x800
	strh r1, [r0]
	add r0, sl, #0x800
	ldrh r1, [r0, #0xa]
	add r1, r1, #1
	and r1, r1, #3
	strh r1, [r0, #0xa]
	b _020E04EC
_020E04C8:
	cmp r0, #0
	cmpne r0, #2
	beq _020E04EC
	add r0, sl, #0x800
	mov r1, r7
	strh r1, [r0, #0x1c]
	add sp, sp, #0xc
	mov r0, #1
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020E04EC:
	add r0, sl, #0x800
	ldrh r2, [r0, #0xc]
	ldrh r1, [r0, #0xa]
	cmp r2, r1
	beq _020E0584
	mov r5, r2, lsl #9
	ldrh r3, [sl, r5]
	mov r1, r8
	mov r2, #0x200
	orr r3, r3, #1
	strh r3, [sl, r5]
	ldrh r0, [r0, #0xc]
	add r0, sl, r0, lsl #9
	bl MIi_CpuCopy16
	add r1, sl, #0x800
	ldrh r0, [r1, #0xc]
	mov r5, #1
	mov r7, #0
	add r0, sl, r0, lsl #1
	add r0, r0, #0x800
	ldrh r0, [r0]
	strh r0, [r1, #0x1a]
	ldrh r0, [r1, #0xc]
	add r0, r0, #1
	and r0, r0, #3
	strh r0, [r1, #0xc]
	ldrh r0, [r1, #0x18]
	cmp r0, #0
	bne _020E0580
	cmp r4, #0
	beq _020E0580
	ldrh r0, [r1, #8]
	mov r0, r0, lsl #9
	ldrh r0, [sl, r0]
	cmp r0, #1
	moveq fp, r5
	beq _020E0584
_020E0580:
	mov fp, #0
_020E0584:
	mov r0, sl
	mov r1, #0
	bl WmDataSharingSendDataSet
	cmp r5, #0
	beq _020E06C4
	mov r0, sl
	mov r2, sb
	mov r1, #0
	bl WmDataSharingReceiveData
	add r0, sl, #0x800
	ldrh r0, [r0, #0x18]
	cmp r0, #0
	bne _020E06C4
	mov r0, sl
	mov r1, fp
	bl WmDataSharingSendDataSet
	b _020E06C4
_020E05C8:
	cmp r0, #4
	mov r0, #0
	add r1, sl, #0x800
	moveq r0, #1
	streqh r0, [r1, #0x1c]
	beq _020E0648
	ldrh r2, [r1, #0xc]
	ldrh r1, [r1, #8]
	cmp r2, r1
	beq _020E0648
	mov r2, r2, lsl #9
	ldrh r1, [sl, r2]
	tst r1, #1
	orreq r1, r1, #1
	streqh r1, [sl, r2]
	beq _020E0648
	mov r1, r8
	add r0, sl, r2
	mov r2, #0x200
	bl MIi_CpuCopy16
	add r2, sl, #0x800
	ldrh r1, [r2, #0xc]
	mov r0, #1
	mov r7, #0
	add r1, sl, r1, lsl #1
	add r1, r1, #0x800
	ldrh r1, [r1]
	strh r1, [r2, #0x1a]
	ldrh r1, [r2, #0xc]
	add r1, r1, #1
	and r1, r1, #3
	strh r1, [r2, #0xc]
_020E0648:
	cmp r0, #0
	beq _020E06C4
	add r0, sl, #0x800
	ldrh r1, [r0, #0xa]
	ldrh r2, [r0, #0x10]
	mov r0, sb
	add r6, sl, r1, lsl #9
	add r1, r6, #0x20
	bl MIi_CpuCopy16
	add r3, sl, #0x800
	ldrh r1, [r3, #0xe]
	mov r4, #1
	ldr r0, _020E06D0 ; =WmDataSharingSetDataCallback
	str r1, [sp]
	ldrh r5, [r3, #0x16]
	mov r1, sl
	add r2, r6, #0x20
	str r5, [sp, #4]
	str r4, [sp, #8]
	ldrh r3, [r3, #0x10]
	bl WM_SetMPDataToPortEx
	add r1, sl, #0x800
	ldrh r2, [r1, #0xa]
	cmp r0, #2
	cmpne r0, #0
	add r2, r2, #1
	and r2, r2, #3
	strh r2, [r1, #0xa]
	movne r0, #5
	strneh r0, [r1, #0x1c]
	movne r7, r4
_020E06C4:
	mov r0, r7
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E06D0: .word WmDataSharingSetDataCallback
_020E06D4: .word 0x0000FFFF
	arm_func_end WM_StepDataSharing
