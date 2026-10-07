	.extern CARD_LockBackup
	.extern CARD_UnlockBackup
	.extern CARDi_SpiDummyWait
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
	.extern CARDi_SpiWaitReadByte
	.extern CARDi_SpiWaitWriteByte

.text
	arm_func_start CARD_SpiWaitReadRange
CARD_SpiWaitReadRange: ; 0x020DDE94
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r2, _020DDFC4 ; =_021E409C
	ldr r1, _020DDFC8 ; =_021E4088
	mov r6, r0
	str r2, [r1, #8]
	str r6, [r1, #0xc]
	mov r0, #1
	strb r0, [r2]
	mov r1, #0
	ldr r0, _020DDFCC ; =_02110FBC
	strb r1, [r6]
	ldrh r0, [r0]
	bl CARD_LockBackup
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl OS_LockCard
	ldr r2, _020DDFD0 ; =0x040001A0
_020DDED8:
	ldrh r0, [r2]
	tst r0, #0x80
	bne _020DDED8
	ldr r1, _020DDFD4 ; =0x0000A042
	ldr r0, _020DDFD8 ; =_021E408C
	strh r1, [r2]
	bl CARDi_SpiWaitWriteByte
	bl CARDi_SpiDummyWait
	ldr r0, _020DDFD8 ; =_021E408C
	bl CARDi_SpiWaitReadByte
	bl CARDi_SpiDummyWait
	ldrb r4, [r6]
	ldr r0, _020DDFC8 ; =_021E4088
	mov r5, #0
	str r5, [r0, #8]
	cmp r4, #0xb9
	movhi r4, #0
	str r6, [r0, #0xc]
	strb r5, [r6]
	cmp r4, #0
	beq _020DDF60
	sub sl, r4, #1
	ldr sb, _020DDFDC ; =0x0000A002
	ldr r8, _020DDFD0 ; =0x040001A0
	ldr r7, _020DDFD8 ; =_021E408C
	b _020DDF54
_020DDF40:
	mov r0, r7
	cmp r5, sl
	streqh sb, [r8]
	bl CARDi_SpiWaitReadByte
	add r5, r5, #1
_020DDF54:
	cmp r5, r4
	blt _020DDF40
	b _020DDF74
_020DDF60:
	ldr r2, _020DDFDC ; =0x0000A002
	ldr r1, _020DDFD0 ; =0x040001A0
	ldr r0, _020DDFD8 ; =_021E408C
	strh r2, [r1]
	bl CARDi_SpiWaitReadByte
_020DDF74:
	mov r5, #0
	b _020DDF8C
_020DDF7C:
	ldrb r0, [r6, r5]
	eor r0, r0, #0xaa
	strb r0, [r6, r5]
	add r5, r5, #1
_020DDF8C:
	cmp r5, r4
	blt _020DDF7C
	ldr r1, _020DDFD0 ; =0x040001A0
_020DDF98:
	ldrh r0, [r1]
	tst r0, #0x80
	bne _020DDF98
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl OS_UnLockCard
	ldr r0, _020DDFCC ; =_02110FBC
	ldrh r0, [r0]
	bl CARD_UnlockBackup
	mov r0, r4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020DDFC4: .word _021E409C
_020DDFC8: .word _021E4088
_020DDFCC: .word _02110FBC
_020DDFD0: .word 0x040001A0
_020DDFD4: .word 0x0000A042
_020DDFD8: .word _021E408C
_020DDFDC: .word 0x0000A002
	arm_func_end CARD_SpiWaitReadRange
