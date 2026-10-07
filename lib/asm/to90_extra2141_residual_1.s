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
	.extern WmGetCommandBuffer4Arm7

.text
	arm_func_start WMi_SendCommand
WMi_SendCommand: ; 0x020DE538
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	bl WmGetCommandBuffer4Arm7
	movs r4, r0
	moveq r0, #8
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
	strh r5, [r4]
	ldrh r5, [sp, #0x14]
	add r0, sp, #0x14
	bic r0, r0, #3
	mov r3, #0
	cmp r5, #0
	add r2, r0, #4
	ble _020DE598
_020DE57C:
	add r2, r2, #4
	ldr r1, [r2, #-4]
	add r0, r4, r3, lsl #2
	add r3, r3, #1
	str r1, [r0, #4]
	cmp r3, r5
	blt _020DE57C
_020DE598:
	mov r0, r4
	mov r1, #0x100
	bl DC_StoreRange
	mov r1, r4
	mov r0, #0xa
	mov r2, #0
	bl PXI_SendWordByFifo
	mov r5, r0
	ldr r0, _020DE5E0 ; =_021E4228
	mov r1, r4
	mov r2, #1
	bl OS_SendMessage
	cmp r5, #0
	movlt r0, #8
	movge r0, #2
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020DE5E0: .word _021E4228
	arm_func_end WMi_SendCommand
