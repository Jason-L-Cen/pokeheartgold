	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MB_SendGameInfoBeacon
	.extern MBi_ClearSendStatus
	.extern MBi_GetGgid
	.extern MBi_GetTgid
	.extern MBi_InitSendFixedBeacon
	.extern MBi_InitSendVolatBeacon
	.extern MBi_SendFixedBeacon
	.extern MBi_SendVolatBeacon
	.extern bsendBuff
	.extern ov13_022474A0

.text
	arm_func_start MB_SendGameInfoBeacon
MB_SendGameInfoBeacon: ; 0x0221DE70
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r4, _0221DEF8 ; =ov13_022474A0
	mov r7, r0
	mov r6, r1
	mov r5, r2
_0221DE84:
	ldrb r0, [r4, #0x24]
	cmp r0, #6
	addls pc, pc, r0, lsl #2
	b _0221DE84
_0221DE94: ; jump table
	b _0221DEB0 ; case 0
	b _0221DEB0 ; case 1
	b _0221DEC0 ; case 2
	b _0221DEC8 ; case 3
	b _0221DEDC ; case 4
	b _0221DEE4 ; case 5
	b _0221DE84 ; case 6
_0221DEB0:
	bl MBi_ReadyBeaconSendStatus
	cmp r0, #0
	bne _0221DE84
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221DEC0:
	bl MBi_InitSendFixedBeacon
	b _0221DE84
_0221DEC8:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl MBi_SendFixedBeacon
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0221DEDC:
	bl MBi_InitSendVolatBeacon
	b _0221DE84
_0221DEE4:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl MBi_SendVolatBeacon
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0221DEF8: .word ov13_022474A0
	arm_func_end MB_SendGameInfoBeacon

	arm_func_start MBi_ReadyBeaconSendStatus
MBi_ReadyBeaconSendStatus: ; 0x0221DEFC
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r0, _0221DF94 ; =ov13_022474A0
	ldr r1, [r0, #0x18]
	cmp r1, #0
	bne _0221DF4C
	bl MBi_GetGgid
	mov r4, r0
	bl MBi_GetTgid
	str r0, [sp]
	mov ip, #8
	ldr r1, _0221DF98 ; =bsendBuff
	mov r3, r4
	mov r0, #0
	mov r2, #0x70
	str ip, [sp, #4]
	bl WM_SetGameInfo
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r4, pc}
_0221DF4C:
	ldr r0, [r0, #0x1c]
	cmp r0, #0
	ldrne r1, [r0, #0x4bc]
	cmpne r1, #0
	ldreq r0, _0221DF94 ; =ov13_022474A0
	ldreq r1, [r0, #0x18]
	ldr r0, _0221DF94 ; =ov13_022474A0
	str r1, [r0, #0x1c]
	bl MBi_ClearSendStatus
	ldr r1, _0221DF94 ; =ov13_022474A0
	mov r2, #2
	ldr r3, [r1, #0x1c]
	mov r0, #1
	ldrb r3, [r3, #0x4b4]
	strb r3, [r1, #0x26]
	strb r2, [r1, #0x24]
	add sp, sp, #8
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0221DF94: .word ov13_022474A0
_0221DF98: .word bsendBuff
	arm_func_end MBi_ReadyBeaconSendStatus
