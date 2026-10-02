	.extern CTRDG_IsExisting
	.extern OS_GetCpsrIrq
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
	.extern PMi_CallCallbackAndUnlock
	.section .text.unified_23,4,1,6
	arm_func_start PM_Init
PM_Init: ; 0x020DADE8
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020DAE78 ; =_021E370C
	ldrh r1, [r0]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r1, #1
	strh r1, [r0]
	mov r1, #0
	str r1, [r0, #0x1c]
	str r1, [r0, #0x20]
	bl PXI_Init
	mov r5, #8
	mov r4, #1
_020DAE1C:
	mov r0, r5
	mov r1, r4
	bl PXI_IsCallbackReady
	cmp r0, #0
	beq _020DAE1C
	ldr r1, _020DAE7C ; =PMi_CommonCallback
	mov r0, #8
	bl PXI_SetFifoRecvCallback
	mov r3, #0
	ldr r0, _020DAE80 ; =_021E3750
	mov r2, r3
_020DAE48:
	mov r1, r3, lsl #3
	add r3, r3, #1
	strh r2, [r0, r1]
	cmp r3, #5
	blt _020DAE48
	ldr r0, _020DAE84 ; =_021E3738
	bl OS_InitMutex
	ldr r1, _020DAE88 ; =0x027FFC3C
	ldr r0, _020DAE78 ; =_021E370C
	ldr r1, [r1]
	str r1, [r0, #0x10]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DAE78: .word _021E370C
_020DAE7C: .word PMi_CommonCallback
_020DAE80: .word _021E3750
_020DAE84: .word _021E3738
_020DAE88: .word 0x027FFC3C
	arm_func_end PM_Init

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
