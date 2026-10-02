	.extern SOCL_Accept
	.extern SOCL_Bind
	.extern SOCL_Cleanup
	.extern SOCL_Close
	.extern SOCL_Connect
	.extern SOCL_CreateSocket
	.extern SOCL_GetHostID
	.extern SOCL_GetStatus
	.extern SOCL_InetAtoH
	.extern SOCL_Listen
	.extern SOCL_ReadFrom
	.extern SOCL_Resolve
	.extern SOCL_SetResolver
	.extern SOCL_Shutdown
	.extern SOCL_SocketIsInvalid
	.extern SOCL_Startup
	.extern SOCL_WriteTo
	.extern SOCLi_CreateCommandPacket
	.extern SOCLi_ExecCommandPacketInRecvPipe
	.extern SOCLi_SocketGetNextPtr
	.extern _02108FB8
	.extern _02108FC0
	.extern _02108FC8
	.extern _02110630
	.extern _02110648
	.extern _02110678
	.extern _02110684
	.extern _02110688
	.extern _02110690
	.extern _0211069C
	.extern _021106B0
	.extern _021106B8
	.extern _021106FC
	.extern _02110700
	.extern _02110704
	.extern _02110708
	.extern _02110710
	.extern _02110718
	.extern _02110758
	.extern _02110798
	.extern _02110898
	.extern _02110899
	.extern _021D4490
	.extern _021D4494
	.extern _021D44AC
	.extern _021D44B4
	.extern _021D44D4
	.extern _021D450C
	.extern _021D451C
	.extern _021D4620
	.extern _021D4634
	.extern _021D4660
	.extern _021D4680
	.extern _021D4688
	.extern _021D46A0
	.extern _021D46AA
	.extern _021D4700
	.extern _021D4720
	.extern _021D4764
	.extern _021D47C8
	.extern _021D4888
	.extern _021D4948
	.extern _021D4B08
	.extern _021D4B32
	.extern _021D4B4C
	.extern _021D4B76
	.extern _021D4CCC
	.extern _021D5660
	.extern _021D5E60
	.extern _021D5E68
	.extern _021D5FD8
	.extern _021D5FEB
	.extern _021D5FEC
	.extern _021D5FF0
	.extern _021D5FF4
	.extern _021D5FFC
	.extern _021D601C
	.extern _021D6020
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.extern WcmSetPhase
	.extern WcmWmcbCommon
	.extern WcmWmcbReset

.text
	arm_func_start WCM_TerminateAsync
WCM_TerminateAsync: ; 0x020AE210
	stmdb sp!, {r3, r4, r5, lr}
	bl OS_DisableInterrupts
	ldr r1, _020AE414 ; =_021D5FEC
	mov r4, r0
	ldr r1, [r1]
	cmp r1, #0
	bne _020AE238
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
_020AE238:
	add r1, r1, #0x2000
	ldr r2, [r1, #0x260]
	cmp r2, #0xd
	addls pc, pc, r2, lsl #2
	b _020AE2C8
_020AE24C: ; jump table
	b _020AE2C8 ; case 0
	b _020AE290 ; case 1
	b _020AE2C8 ; case 2
	b _020AE2D8 ; case 3
	b _020AE2C8 ; case 4
	b _020AE2C8 ; case 5
	b _020AE29C ; case 6
	b _020AE2C8 ; case 7
	b _020AE2C8 ; case 8
	b _020AE2D8 ; case 9
	b _020AE2C8 ; case 10
	b _020AE2C8 ; case 11
	b _020AE2D8 ; case 12
	b _020AE284 ; case 13
_020AE284:
	bl OS_RestoreInterrupts
	mov r0, #2
	ldmia sp!, {r3, r4, r5, pc}
_020AE290:
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_020AE29C:
	mov r0, #0xd
	bl WcmSetPhase
	ldr r1, _020AE414 ; =_021D5FEC
	mov r2, #9
	ldr r1, [r1]
	mov r0, r4
	add r1, r1, #0x2200
	strh r2, [r1, #0x80]
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r3, r4, r5, pc}
_020AE2C8:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
_020AE2D8:
	ldrb r0, [r1, #0x26b]
	cmp r0, #1
	bne _020AE304
	mov r0, #0xd
	bl WcmSetPhase
	ldr r0, _020AE414 ; =_021D5FEC
	mov r1, #9
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x80]
	b _020AE404
_020AE304:
	bl WMi_GetStatusAddress
	mov r5, r0
	mov r1, #2
	bl DC_InvalidateRange
	ldrh r0, [r5]
	cmp r0, #0
	beq _020AE334
	cmp r0, #1
	beq _020AE36C
	cmp r0, #2
	beq _020AE378
	b _020AE384
_020AE334:
	bl WM_Finish
	cmp r0, #0
	bne _020AE3A0
	mov r0, #1
	bl WcmSetPhase
	ldr r1, _020AE414 ; =_021D5FEC
	mov r2, #0
	ldr r1, [r1]
	mov r0, r4
	add r1, r1, #0x2200
	strh r2, [r1, #0x80]
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_020AE36C:
	ldr r0, _020AE418 ; =WcmWmcbCommon
	bl WM_Disable
	b _020AE3A0
_020AE378:
	ldr r0, _020AE418 ; =WcmWmcbCommon
	bl WM_PowerOff
	b _020AE3A0
_020AE384:
	ldr r1, _020AE414 ; =_021D5FEC
	ldr r0, _020AE41C ; =WcmWmcbReset
	ldr r1, [r1]
	mov r2, #1
	add r1, r1, #0x2000
	strb r2, [r1, #0x26b]
	bl WM_Reset
_020AE3A0:
	cmp r0, #2
	beq _020AE3BC
	cmp r0, #3
	beq _020AE3EC
	cmp r0, #8
	beq _020AE3DC
	b _020AE3EC
_020AE3BC:
	mov r0, #0xd
	bl WcmSetPhase
	ldr r0, _020AE414 ; =_021D5FEC
	mov r1, #9
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x80]
	b _020AE404
_020AE3DC:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #4
	ldmia sp!, {r3, r4, r5, pc}
_020AE3EC:
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r3, r4, r5, pc}
_020AE404:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020AE414: .word _021D5FEC
_020AE418: .word WcmWmcbCommon
_020AE41C: .word WcmWmcbReset
	arm_func_end WCM_TerminateAsync
