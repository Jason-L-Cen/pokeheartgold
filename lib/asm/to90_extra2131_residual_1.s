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
	.extern RtcBCD2HEX

.text
	arm_func_start RtcCommonCallback
RtcCommonCallback: ; 0x020DBC34
	stmdb sp!, {r3, r4, r5, lr}
	cmp r2, #0
	beq _020DBC90
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0x24]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0x24]
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0xc]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0xc]
	ldr r0, _020DC174 ; =_021E3778
	ldr r2, [r0, #0x10]
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r0, #0x10]
	ldr r1, [r0, #0x1c]
	mov r0, #6
	blx r2
	ldmia sp!, {r3, r4, r5, pc}
_020DBC90:
	and r0, r1, #0x7f00
	mov r0, r0, lsr #8
	and r0, r0, #0xff
	cmp r0, #0x30
	and r2, r1, #0xff
	bne _020DBCC0
	ldr r0, _020DC174 ; =_021E3778
	ldr r0, [r0, #0x28]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	blx r0
	ldmia sp!, {r3, r4, r5, pc}
_020DBCC0:
	cmp r2, #0
	bne _020DC0E4
	ldr r0, _020DC174 ; =_021E3778
	mov r4, #0
	ldr r1, [r0, #0x20]
	cmp r1, #0xf
	addls pc, pc, r1, lsl #2
	b _020DC0D0
_020DBCE0: ; jump table
	b _020DBD20 ; case 0
	b _020DBD7C ; case 1
	b _020DBDCC ; case 2
	b _020DC12C ; case 3
	b _020DC12C ; case 4
	b _020DC12C ; case 5
	b _020DBE74 ; case 6
	b _020DBEA4 ; case 7
	b _020DBEC8 ; case 8
	b _020DBF70 ; case 9
	b _020DC024 ; case 10
	b _020DC12C ; case 11
	b _020DC12C ; case 12
	b _020DC12C ; case 13
	b _020DC12C ; case 14
	b _020DC12C ; case 15
_020DBD20:
	ldr r1, _020DC178 ; =0x027FFDE8
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x18
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x13
	mov r0, r0, lsr #0x1b
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #0xa
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r0, r5
	bl RTC_GetDayOfWeek
	str r0, [r5, #0xc]
	b _020DC12C
_020DBD7C:
	ldr r1, _020DC17C ; =0x027FFDEC
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1a
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x11
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	b _020DC12C
_020DBDCC:
	ldr r1, _020DC178 ; =0x027FFDE8
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	and r0, r0, #0xff
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x13
	mov r0, r0, lsr #0x1b
	bl RtcBCD2HEX
	ldr r1, _020DC178 ; =0x027FFDE8
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #0xa
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r0, r5
	bl RTC_GetDayOfWeek
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #0xc]
	ldr r0, [r1]
	ldr r1, _020DC174 ; =_021E3778
	mov r0, r0, lsl #0x1a
	mov r0, r0, lsr #0x1a
	ldr r5, [r1, #0x18]
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x11
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	b _020DC12C
_020DBE74:
	ldr r1, _020DC180 ; =0x027FFDEA
	ldr r2, [r0, #0x14]
	ldrh r0, [r1]
	mov r0, r0, lsl #0x1c
	mov r0, r0, lsr #0x1c
	cmp r0, #4
	bne _020DBE9C
	mov r0, #1
	str r0, [r2]
	b _020DC12C
_020DBE9C:
	str r4, [r2]
	b _020DC12C
_020DBEA4:
	ldr r1, _020DC180 ; =0x027FFDEA
	ldr r2, [r0, #0x14]
	ldrh r0, [r1]
	mov r0, r0, lsl #0x19
	movs r0, r0, lsr #0x1f
	movne r0, #1
	strne r0, [r2]
	streq r4, [r2]
	b _020DC12C
_020DBEC8:
	ldr r1, _020DC17C ; =0x027FFDEC
	ldr r5, [r0, #0x14]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1d
	mov r0, r0, lsr #0x1d
	str r0, [r5]
	ldr r0, [r1]
	mov r0, r0, lsl #0x12
	mov r0, r0, lsr #0x1a
	bl RtcBCD2HEX
	ldr r1, _020DC17C ; =0x027FFDEC
	str r0, [r5, #4]
	ldr r0, [r1]
	mov r0, r0, lsl #9
	mov r0, r0, lsr #0x19
	bl RtcBCD2HEX
	str r0, [r5, #8]
	mov r1, r4
	ldr r0, _020DC17C ; =0x027FFDEC
	str r1, [r5, #0xc]
	ldr r0, [r0]
	mov r0, r0, lsl #0x18
	movs r0, r0, lsr #0x1f
	ldrne r0, [r5, #0xc]
	addne r0, r0, #1
	strne r0, [r5, #0xc]
	ldr r0, _020DC17C ; =0x027FFDEC
	ldr r0, [r0]
	mov r0, r0, lsl #0x10
	movs r0, r0, lsr #0x1f
	ldrne r0, [r5, #0xc]
	addne r0, r0, #2
	strne r0, [r5, #0xc]
	ldr r0, _020DC17C ; =0x027FFDEC
	ldr r0, [r0]
	mov r0, r0, lsl #8
	movs r0, r0, lsr #0x1f
	beq _020DC12C
	ldr r0, [r5, #0xc]
	add r0, r0, #4
	str r0, [r5, #0xc]
	b _020DC12C
_020DBF70:
	ldr r3, [r0, #0x24]
	cmp r3, #0
	bne _020DC01C
	ldr r1, [r0, #0x14]
	ldr r2, _020DC180 ; =0x027FFDEA
	ldr r1, [r1]
	cmp r1, #1
	ldrh r1, [r2]
	bne _020DBFDC
	mov r1, r1, lsl #0x1c
	mov r1, r1, lsr #0x1c
	cmp r1, #4
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0xf
	orr r0, r0, #4
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DBFDC:
	mov r1, r1, lsl #0x1c
	movs r1, r1, lsr #0x1c
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0xf
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC01C:
	str r4, [r0, #0x24]
	b _020DC12C
_020DC024:
	ldr r3, [r0, #0x24]
	cmp r3, #0
	bne _020DC0C8
	ldr r1, [r0, #0x14]
	ldr r2, _020DC180 ; =0x027FFDEA
	ldr r1, [r1]
	cmp r1, #1
	ldrh r1, [r2]
	bne _020DC088
	mov r1, r1, lsl #0x19
	movs r1, r1, lsr #0x1f
	bne _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	orr r0, r0, #0x40
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC088:
	mov r1, r1, lsl #0x19
	movs r1, r1, lsr #0x1f
	beq _020DC12C
	add r1, r3, #1
	str r1, [r0, #0x24]
	ldrh r0, [r2]
	bic r0, r0, #0x40
	strh r0, [r2]
	bl RTCi_WriteRawStatus2Async
	cmp r0, #0
	bne _020DC12C
	ldr r0, _020DC174 ; =_021E3778
	mov r1, r4
	str r1, [r0, #0x24]
	mov r4, #3
	b _020DC12C
_020DC0C8:
	str r4, [r0, #0x24]
	b _020DC12C
_020DC0D0:
	ldr r0, _020DC174 ; =_021E3778
	mov r1, #0
	str r1, [r0, #0x24]
	mov r4, #4
	b _020DC12C
_020DC0E4:
	ldr r0, _020DC174 ; =_021E3778
	mov r1, #0
	str r1, [r0, #0x24]
	cmp r2, #4
	addls pc, pc, r2, lsl #2
	b _020DC128
_020DC0FC: ; jump table
	b _020DC128 ; case 0
	b _020DC110 ; case 1
	b _020DC118 ; case 2
	b _020DC120 ; case 3
	b _020DC128 ; case 4
_020DC110:
	mov r4, #4
	b _020DC12C
_020DC118:
	mov r4, #5
	b _020DC12C
_020DC120:
	mov r4, #1
	b _020DC12C
_020DC128:
	mov r4, #6
_020DC12C:
	ldr r0, _020DC174 ; =_021E3778
	ldr r1, [r0, #0x24]
	cmp r1, #0
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r1, [r0, #0xc]
	cmp r1, #0
	movne r1, #0
	strne r1, [r0, #0xc]
	ldr r0, _020DC174 ; =_021E3778
	ldr r2, [r0, #0x10]
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r1, #0
	str r1, [r0, #0x10]
	ldr r1, [r0, #0x1c]
	mov r0, r4
	blx r2
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DC174: .word _021E3778
_020DC178: .word 0x027FFDE8
_020DC17C: .word 0x027FFDEC
_020DC180: .word 0x027FFDEA
	arm_func_end RtcCommonCallback
