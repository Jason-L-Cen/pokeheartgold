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
	.public WcmEditScanExParam
	.extern WCM_UpdateOption
	.extern WcmGetNextScanChannel

.text
	arm_func_start WcmEditScanExParam
WcmEditScanExParam: ; 0x020AE61C
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r0, r2
	mov r4, r1
	bl WCM_UpdateOption
	ldr r2, _020AE7A8 ; =_021D5FEC
	mov r3, #0x400
	ldr r1, [r2]
	mov r0, #0
	add ip, r1, #0x1500
	add r1, r1, #0x2000
	str ip, [r1, #0x288]
	ldr r1, [r2]
	add r1, r1, #0x2200
	strh r3, [r1, #0x8c]
	bl WcmGetNextScanChannel
	ldr r1, _020AE7A8 ; =_021D5FEC
	mov r2, #1
	mov r2, r2, lsl r0
	ldr r0, [r1]
	mov r2, r2, asr #1
	add r0, r0, #0x2200
	strh r2, [r0, #0x8e]
	ldr r0, [r1]
	add r0, r0, #0x2200
	ldrh r0, [r0, #0x68]
	cmp r0, #0
	bne _020AE690
	bl WM_GetDispersionScanPeriod
_020AE690:
	ldr r2, _020AE7A8 ; =_021D5FEC
	ldr r1, [r2]
	add r1, r1, #0x2200
	strh r0, [r1, #0x90]
	ldr r1, [r2]
	add r0, r1, #0x2000
	ldr r0, [r0, #0x264]
	and r0, r0, #0x300000
	cmp r0, #0x300000
	movne r2, #1
	moveq r2, #0
	add r0, r1, #0x2200
	strh r2, [r0, #0x98]
	cmp r5, #0
	mov r2, #6
	bne _020AE6EC
	ldr r1, _020AE7A8 ; =_021D5FEC
	ldr r0, _020AE7AC ; =_02108FC0
	ldr r1, [r1]
	add r1, r1, #0x92
	add r1, r1, #0x2200
	bl MI_CpuCopy8
	b _020AE704
_020AE6EC:
	ldr r1, _020AE7A8 ; =_021D5FEC
	mov r0, r5
	ldr r1, [r1]
	add r1, r1, #0x92
	add r1, r1, #0x2200
	bl MI_CpuCopy8
_020AE704:
	cmp r4, #0
	ldrne r0, _020AE7B0 ; =_02108FC8
	mov r2, #0x20
	cmpne r4, r0
	bne _020AE748
	ldr r1, _020AE7A8 ; =_021D5FEC
	ldr r0, _020AE7B0 ; =_02108FC8
	ldr r1, [r1]
	add r1, r1, #0x29c
	add r1, r1, #0x2000
	bl MI_CpuCopy8
	ldr r0, _020AE7A8 ; =_021D5FEC
	mov r1, #0
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x9a]
	b _020AE790
_020AE748:
	ldr r1, _020AE7A8 ; =_021D5FEC
	mov r0, r4
	ldr r1, [r1]
	add r1, r1, #0x29c
	add r1, r1, #0x2000
	bl MI_CpuCopy8
	mov r1, #0
_020AE764:
	ldrb r0, [r4]
	cmp r0, #0
	beq _020AE780
	add r1, r1, #1
	cmp r1, #0x20
	add r4, r4, #1
	blt _020AE764
_020AE780:
	ldr r0, _020AE7A8 ; =_021D5FEC
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x9a]
_020AE790:
	ldr r0, _020AE7A8 ; =_021D5FEC
	mov r1, #0
	ldr r0, [r0]
	add r0, r0, #0x2000
	str r1, [r0, #0x284]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020AE7A8: .word _021D5FEC
_020AE7AC: .word _02108FC0
_020AE7B0: .word _02108FC8
	arm_func_end WcmEditScanExParam
