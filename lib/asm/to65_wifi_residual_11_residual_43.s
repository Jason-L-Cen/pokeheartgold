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
	.extern WcmConfigure
	.extern WcmSetPhase
	.extern WcmWmcbCommon
	.extern WcmWmcbIndication

.text
	arm_func_start WCM_StartupAsync
WCM_StartupAsync: ; 0x020AD9C0
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r5, r1
	bl OS_DisableInterrupts
	ldr r1, _020ADBB8 ; =_021D5FEC
	mov r4, r0
	ldr r1, [r1]
	cmp r1, #0
	bne _020AD9F0
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
_020AD9F0:
	add r1, r1, #0x2000
	ldr r1, [r1, #0x260]
	cmp r1, #1
	beq _020ADA14
	cmp r1, #2
	beq _020ADA24
	cmp r1, #3
	beq _020ADA30
	b _020ADA3C
_020ADA14:
	mov r0, r6
	mov r1, r5
	bl WcmConfigure
	b _020ADA48
_020ADA24:
	bl OS_RestoreInterrupts
	mov r0, #2
	ldmia sp!, {r4, r5, r6, pc}
_020ADA30:
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
_020ADA3C:
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
_020ADA48:
	ldr r0, _020ADBB8 ; =_021D5FEC
	ldr r0, [r0]
	add r1, r0, #0x2000
	ldr r1, [r1, #0x26c]
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	bl WM_Init
	cmp r0, #6
	addls pc, pc, r0, lsl #2
	b _020ADAB4
_020ADA70: ; jump table
	b _020ADACC ; case 0
	b _020ADAB4 ; case 1
	b _020ADAB4 ; case 2
	b _020ADA8C ; case 3
	b _020ADAA4 ; case 4
	b _020ADAB4 ; case 5
	b _020ADAB4 ; case 6
_020ADA8C:
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020ADAA4:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #5
	ldmia sp!, {r4, r5, r6, pc}
_020ADAB4:
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020ADACC:
	bl WM_GetAllowedChannel
	cmp r0, #0
	bne _020ADB0C
	bl WM_Finish
	cmp r0, #0
	beq _020ADAFC
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020ADAFC:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #5
	ldmia sp!, {r4, r5, r6, pc}
_020ADB0C:
	ldr r0, _020ADBBC ; =WcmWmcbIndication
	bl WM_SetIndCallback
	cmp r0, #0
	beq _020ADB34
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020ADB34:
	ldr r0, _020ADBC0 ; =WcmWmcbCommon
	bl WM_Enable
	cmp r0, #2
	beq _020ADB58
	cmp r0, #3
	beq _020ADB90
	cmp r0, #8
	beq _020ADB78
	b _020ADB90
_020ADB58:
	mov r0, #2
	bl WcmSetPhase
	ldr r0, _020ADBB8 ; =_021D5FEC
	mov r1, #1
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x80]
	b _020ADBA8
_020ADB78:
	mov r0, #0xc
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
_020ADB90:
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r4, r5, r6, pc}
_020ADBA8:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020ADBB8: .word _021D5FEC
_020ADBBC: .word WcmWmcbIndication
_020ADBC0: .word WcmWmcbCommon
	arm_func_end WCM_StartupAsync
