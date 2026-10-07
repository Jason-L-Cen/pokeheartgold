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
	.extern PM_ForceToPowerOff
	.extern PM_GetBackLight
	.extern PM_GetLCDPower
	.extern PM_SetBackLight
	.extern PMi_ExecuteList
	.extern PMi_SendSleepStart
	.extern PMi_SetLCDPower
	.extern PMi_SetLED
.text
	arm_func_start PM_GoSleepMode
PM_GoSleepMode: ; 0x020DB43C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x18
	ldr r3, _020DB660 ; =_021E370C
	mov sl, r0
	ldr r0, [r3, #0xc]
	mov sb, r1
	mov fp, r2
	mov r4, #0
	bl PMi_ExecuteList
	ldr r1, _020DB664 ; =0x04000208
	mov r0, r4
	ldrh r8, [r1]
	strh r0, [r1]
	bl OS_DisableInterrupts
	str r0, [sp, #8]
	ldr r0, _020DB668 ; =0x003FFFFF
	bl OS_DisableIrqMask
	str r0, [sp, #4]
	bl OS_IsTickAvailable
	cmp r0, #0
	movne r0, #8
	moveq r0, r4
	orr r0, r0, #0x40000
	bl OS_SetIrqMask
	ldr r0, [sp, #8]
	bl OS_RestoreInterrupts
	ldr r2, _020DB664 ; =0x04000208
	mov r0, #1
	ldrh r1, [r2]
	tst sl, #8
	strh r0, [r2]
	beq _020DB4CC
	ldr r0, _020DB66C ; =0x027FFC40
	ldrh r0, [r0]
	cmp r0, #2
	biceq sl, sl, #8
_020DB4CC:
	tst sl, #0x10
	beq _020DB4E0
	bl CTRDG_IsExisting
	cmp r0, #0
	biceq sl, sl, #0x10
_020DB4E0:
	mov r1, #0x4000000
	add r0, r1, #0x1000
	ldr r5, [r1]
	ldr r6, [r0]
	bl PM_GetLCDPower
	str r0, [sp]
	add r0, sp, #0x14
	add r1, sp, #0x10
	bl PM_GetBackLight
	mov r0, #2
	mov r1, #0
	bl PM_SetBackLight
	ldr r2, _020DB670 ; =0x027FFC3C
	ldr r0, [r2]
	str r0, [sp, #0xc]
_020DB51C:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB51C
	ldr r0, [r2]
	mov r2, #0x4000000
	str r0, [sp, #0xc]
	ldr r0, [r2]
	add r1, r2, #0x1000
	bic r0, r0, #0x30000
	str r0, [r2]
	ldr r0, [r1]
	bic r0, r0, #0x10000
	str r0, [r1]
	ldr r2, _020DB670 ; =0x027FFC3C
_020DB558:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB558
	ldr r0, [r2]
	str r0, [sp, #0xc]
	ldr r2, _020DB670 ; =0x027FFC3C
_020DB574:
	ldr r1, [r2]
	ldr r0, [sp, #0xc]
	cmp r0, r1
	beq _020DB574
	ldr r0, [sp, #0x14]
	ldr r1, [sp, #0x10]
	orr r0, sl, r0, lsl #5
	orr r0, r0, r1, lsl #6
	mov r0, r0, lsl #0x10
	mov r7, r0, lsr #0x10
	orr r0, sb, fp
	mov r0, r0, lsl #0x10
	mov sb, r0, lsr #0x10
_020DB5A8:
	mov r0, r7
	mov r1, sb
	bl PMi_SendSleepStart
	cmp r0, #0
	bne _020DB5A8
	bl OS_Halt
	tst sl, #8
	beq _020DB5D8
	ldr r0, _020DB674 ; =0x04000214
	ldr r0, [r0]
	tst r0, #0x100000
	movne r4, #1
_020DB5D8:
	cmp r4, #0
	bne _020DB618
	ldr r0, [sp]
	cmp r0, #1
	mov r0, #1
	bne _020DB604
	mov r1, r0
	mov r2, r0
	mov r3, r0
	bl PMi_SetLCDPower
	b _020DB608
_020DB604:
	bl PMi_SetLED
_020DB608:
	mov r0, #0x4000000
	str r5, [r0]
	add r0, r0, #0x1000
	str r6, [r0]
_020DB618:
	ldr r0, _020DB678 ; =0x00708100
	bl OS_SpinWait
	bl OS_DisableInterrupts
	ldr r0, [sp, #4]
	bl OS_SetIrqMask
	ldr r0, [sp, #8]
	bl OS_RestoreInterrupts
	ldr r1, _020DB664 ; =0x04000208
	cmp r4, #0
	ldrh r0, [r1]
	strh r8, [r1]
	beq _020DB64C
	bl PM_ForceToPowerOff
_020DB64C:
	ldr r0, _020DB660 ; =_021E370C
	ldr r0, [r0, #0x18]
	bl PMi_ExecuteList
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020DB660: .word _021E370C
_020DB664: .word 0x04000208
_020DB668: .word 0x003FFFFF
_020DB66C: .word 0x027FFC40
_020DB670: .word 0x027FFC3C
_020DB674: .word 0x04000214
_020DB678: .word 0x00708100
	arm_func_end PM_GoSleepMode
