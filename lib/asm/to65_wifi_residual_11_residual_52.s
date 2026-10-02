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
	.public WcmWmcbReset
	.extern WcmNotify
	.extern WcmSetPhase
	.extern WcmWmcbCommon
	.extern WcmWmcbConnect

.text
	arm_func_start WcmWmcbReset
WcmWmcbReset: ; 0x020AF690
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldrh r0, [r0, #2]
	cmp r0, #0
	bne _020AF968
	ldr r2, _020AF98C ; =_021D5FEC
	mov r0, #0
	ldr r1, [r2]
	add r1, r1, #0x2000
	strb r0, [r1, #0x26b]
	ldr r1, [r2]
	add r1, r1, #0x2200
	strh r0, [r1, #0x82]
	ldr r3, [r2]
	add r1, r3, #0x2000
	ldr r1, [r1, #0x260]
	cmp r1, #0xd
	addls pc, pc, r1, lsl #2
	b _020AF938
_020AF6DC: ; jump table
	b _020AF938 ; case 0
	b _020AF938 ; case 1
	b _020AF938 ; case 2
	b _020AF938 ; case 3
	b _020AF938 ; case 4
	b _020AF714 ; case 5
	b _020AF714 ; case 6
	b _020AF738 ; case 7
	b _020AF75C ; case 8
	b _020AF874 ; case 9
	b _020AF8A0 ; case 10
	b _020AF938 ; case 11
	b _020AF874 ; case 12
	b _020AF8CC ; case 13
_020AF714:
	mov r0, #3
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF990 ; =0x000008F5
	mov r2, r1
	mov r0, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF738:
	mov r0, #3
	bl WcmSetPhase
	mov r0, #0
	ldr r3, _020AF994 ; =0x000008FB
	mov r1, r0
	mov r2, r0
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF75C:
	add r1, r3, #0x2200
	ldrh r4, [r1, #0xf8]
	strh r0, [r1, #0xf8]
	cmp r4, #0x12
	bne _020AF848
	ldr r1, [r2]
	add r1, r1, #0x2100
	ldrh r3, [r1, #0x70]
	and ip, r3, #0x24
	cmp ip, #0x24
	beq _020AF848
	orr r3, r3, #0x24
	strh r3, [r1, #0x70]
	ldr lr, [r2]
	add r1, lr, #0x2000
	ldr r2, [r1, #0x264]
	and r1, r2, #0xc0000
	cmp r1, #0xc0000
	moveq r0, #1
	and r1, r2, #0x30000
	cmp r1, #0x30000
	mov r0, r0, lsl #0x10
	mov ip, r0, lsr #0x10
	movne r3, #1
	ldr r0, _020AF998 ; =WcmWmcbConnect
	moveq r3, #0
	add r1, lr, #0x2140
	mov r2, #0
	str ip, [sp]
	bl WM_StartConnectEx
	cmp r0, #2
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	cmp r0, #3
	beq _020AF81C
	cmp r0, #8
	bne _020AF81C
	mov r0, #0xc
	bl WcmSetPhase
	ldr r0, _020AF98C ; =_021D5FEC
	ldr r3, _020AF99C ; =0x0000091C
	ldr r0, [r0]
	mov r2, r4
	add r1, r0, #0x2140
	mov r0, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF81C:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AF98C ; =_021D5FEC
	ldr r3, _020AF9A0 ; =0x00000925
	ldr r0, [r0]
	mov r2, r4
	add r1, r0, #0x2140
	mov r0, #7
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF848:
	mov r0, #3
	bl WcmSetPhase
	ldr r0, _020AF98C ; =_021D5FEC
	ldr r3, _020AF9A4 ; =0x0000092D
	ldr r0, [r0]
	mov r2, r4
	add r1, r0, #0x2140
	mov r0, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF874:
	mov r0, #3
	bl WcmSetPhase
	ldr r0, _020AF98C ; =_021D5FEC
	ldr r3, _020AF9A8 ; =0x00000935
	ldr r1, [r0]
	mov r0, #0
	add r1, r1, #0x2140
	mov r2, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF8A0:
	mov r0, #3
	bl WcmSetPhase
	ldr r1, _020AF98C ; =_021D5FEC
	mov r0, #0
	ldr r1, [r1]
	ldr r3, _020AF9AC ; =0x0000093C
	mov r2, r0
	add r1, r1, #0x2140
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF8CC:
	ldr r0, _020AF9B0 ; =WcmWmcbCommon
	bl WM_PowerOff
	cmp r0, #2
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	cmp r0, #3
	beq _020AF914
	cmp r0, #8
	bne _020AF914
	mov r0, #0xc
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF9B4 ; =0x0000094A
	mov r2, r1
	mov r0, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF914:
	mov r0, #0xb
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF9B8 ; =0x00000953
	mov r2, r1
	mov r0, #7
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF938:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r1, _020AF98C ; =_021D5FEC
	mov r0, #7
	ldr r1, [r1]
	rsb r3, r0, #0x960
	add r1, r1, #0x2000
	ldr r2, [r1, #0x260]
	mov r1, #0
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_020AF968:
	mov r0, #0xb
	bl WcmSetPhase
	mov r1, #0
	mov r2, r1
	mov r0, #7
	mov r3, #0x960
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020AF98C: .word _021D5FEC
_020AF990: .word 0x000008F5
_020AF994: .word 0x000008FB
_020AF998: .word WcmWmcbConnect
_020AF99C: .word 0x0000091C
_020AF9A0: .word 0x00000925
_020AF9A4: .word 0x0000092D
_020AF9A8: .word 0x00000935
_020AF9AC: .word 0x0000093C
_020AF9B0: .word WcmWmcbCommon
_020AF9B4: .word 0x0000094A
_020AF9B8: .word 0x00000953
	arm_func_end WcmWmcbReset
