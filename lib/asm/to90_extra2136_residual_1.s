	.extern CTRDG_IsExisting
	.extern OS_GetCpsrIrq
	.extern PMi_CallCallbackAndUnlock
	.extern PXI_Init
	.extern PXI_IsCallbackReady
	.extern PXI_SendWordByFifo
	.extern PXI_SetFifoRecvCallback
	.extern PXIi_HandlerRecvFifoNotEmpty
	.extern RTC_GetDayOfWeek
	.extern RTCi_ReadRawDateAsync
	.extern RTCi_ReadRawDateTimeAsync
	.extern RTCi_ReadRawTimeAsync
	.extern RTCi_WriteRawStatus2Async
	.extern _021E36B0
	.extern _021E36CC
	.extern _021E36EC
	.extern _021E36F0
	.extern _021E370C
	.extern _021E3718
	.extern _021E3724
	.extern _021E3728
	.extern _021E3738
	.extern _021E3750
	.extern _021E3754
	.extern _021E3778
	.extern _021E3784
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public PMi_CommonCallback

.text
	arm_func_start PMi_CommonCallback
PMi_CommonCallback: ; 0x020DAE8C
	stmdb sp!, {r3, lr}
	cmp r2, #0
	beq _020DAEA4
	mov r0, #2
	bl PMi_CallCallbackAndUnlock
	ldmia sp!, {r3, pc}
_020DAEA4:
	and r0, r1, #0x7f00
	mov r0, r0, lsl #8
	mov r2, r0, lsr #0x10
	and r0, r1, #0xff
	cmp r2, #0x70
	blo _020DAEF4
	cmp r2, #0x74
	bhi _020DAEF4
	ldr r1, _020DAF48 ; =_021E3754
	sub r2, r2, #0x70
	ldr r1, [r1, r2, lsl #3]
	and r0, r0, #0xff
	cmp r1, #0
	strneh r0, [r1]
	mov r1, r2, lsl #3
	ldr r0, _020DAF4C ; =_021E3750
	mov r2, #1
	strh r2, [r0, r1]
	mov r0, #0
	b _020DAF40
_020DAEF4:
	cmp r2, #0x60
	bne _020DAF0C
	ldr r1, _020DAF50 ; =_021E370C
	mov r2, #1
	str r2, [r1, #4]
	b _020DAF40
_020DAF0C:
	cmp r2, #0x62
	bne _020DAF24
	ldr r1, _020DAF50 ; =_021E370C
	mov r2, #1
	str r2, [r1, #8]
	b _020DAF40
_020DAF24:
	cmp r2, #0x67
	bne _020DAF40
	ldr r1, _020DAF50 ; =_021E370C
	ldr r1, [r1, #0x28]
	cmp r1, #0
	strne r0, [r1]
	mov r0, #0
_020DAF40:
	bl PMi_CallCallbackAndUnlock
	ldmia sp!, {r3, pc}
	.align 2, 0
_020DAF48: .word _021E3754
_020DAF4C: .word _021E3750
_020DAF50: .word _021E370C
	arm_func_end PMi_CommonCallback
