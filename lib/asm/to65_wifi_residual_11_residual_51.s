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
	.public WcmWmcbConnect
	.extern WcmNotify
	.extern WcmSetPhase
	.extern WcmWmReset
	.extern WcmWmcbStartDCF

.text
	arm_func_start WcmWmcbConnect
WcmWmcbConnect: ; 0x020AF138
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldrh r0, [r4, #2]
	cmp r0, #0xc
	addls pc, pc, r0, lsl #2
	b _020AF358
_020AF150: ; jump table
	b _020AF184 ; case 0
	b _020AF334 ; case 1
	b _020AF358 ; case 2
	b _020AF358 ; case 3
	b _020AF358 ; case 4
	b _020AF358 ; case 5
	b _020AF348 ; case 6
	b _020AF358 ; case 7
	b _020AF358 ; case 8
	b _020AF358 ; case 9
	b _020AF358 ; case 10
	b _020AF348 ; case 11
	b _020AF348 ; case 12
_020AF184:
	ldrh r0, [r4, #8]
	cmp r0, #9
	bgt _020AF1C4
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _020AF30C
_020AF19C: ; jump table
	b _020AF30C ; case 0
	b _020AF30C ; case 1
	b _020AF30C ; case 2
	b _020AF30C ; case 3
	b _020AF30C ; case 4
	b _020AF30C ; case 5
	ldmia sp!, {r4, pc} ; case 6
	b _020AF240 ; case 7
	b _020AF1D0 ; case 8
	b _020AF1D0 ; case 9
_020AF1C4:
	cmp r0, #0x1a
	ldmeqia sp!, {r4, pc}
	b _020AF30C
_020AF1D0:
	ldr r1, _020AF380 ; =_021D5FEC
	ldr r2, [r1]
	add r0, r2, #0x2000
	ldr r0, [r0, #0x260]
	sub r0, r0, #8
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	ldmia sp!, {r4, pc}
_020AF1F0: ; jump table
	b _020AF210 ; case 0
	b _020AF21C ; case 1
	b _020AF204 ; case 2
	ldmia sp!, {r4, pc} ; case 3
	b _020AF238 ; case 4
_020AF204:
	add r0, r2, #0x2200
	mov r1, #0
	strh r1, [r0, #0x82]
_020AF210:
	mov r0, #0xc
	bl WcmSetPhase
	ldmia sp!, {r4, pc}
_020AF21C:
	add r0, r2, #0x2200
	mov r2, #0
	strh r2, [r0, #0x82]
	ldr r0, [r1]
	mov r1, #6
	add r0, r0, #0x2200
	strh r1, [r0, #0x80]
_020AF238:
	bl WcmWmReset
	ldmia sp!, {r4, pc}
_020AF240:
	ldr r1, _020AF380 ; =_021D5FEC
	ldr r3, [r1]
	add r0, r3, #0x2000
	ldr r0, [r0, #0x260]
	cmp r0, #0xc
	bne _020AF268
	mov r0, #8
	bl WcmSetPhase
	bl WcmWmReset
	ldmia sp!, {r4, pc}
_020AF268:
	ldrh r2, [r4, #0xa]
	cmp r2, #1
	blo _020AF304
	ldr r0, _020AF384 ; =0x000007D7
	cmp r2, r0
	bhi _020AF304
	add r0, r3, #0x2200
	strh r2, [r0, #0x82]
	ldr r1, [r1]
	ldr r0, _020AF388 ; =WcmWmcbStartDCF
	add r1, r1, #0x1500
	mov r2, #0x620
	bl WM_StartDCF
	cmp r0, #2
	ldmeqia sp!, {r4, pc}
	cmp r0, #3
	beq _020AF2DC
	cmp r0, #8
	bne _020AF2DC
	mov r0, #0xc
	bl WcmSetPhase
	ldr r0, _020AF380 ; =_021D5FEC
	ldr r3, _020AF38C ; =0x000007ED
	ldr r1, [r0]
	mov r0, #1
	add r1, r1, #0x2140
	mov r2, #0
	bl WcmNotify
	ldmia sp!, {r4, pc}
_020AF2DC:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AF380 ; =_021D5FEC
	ldr r3, _020AF390 ; =0x000007F6
	ldr r1, [r0]
	mov r0, #7
	add r1, r1, #0x2140
	mov r2, #0
	bl WcmNotify
	ldmia sp!, {r4, pc}
_020AF304:
	bl WcmWmReset
	ldmia sp!, {r4, pc}
_020AF30C:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AF380 ; =_021D5FEC
	ldrh r2, [r4, #8]
	ldr r0, [r0]
	ldr r3, _020AF394 ; =0x00000804
	add r1, r0, #0x2140
	mov r0, #7
	bl WcmNotify
	ldmia sp!, {r4, pc}
_020AF334:
	ldr r0, _020AF380 ; =_021D5FEC
	ldrh r1, [r4, #0xe]
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0xf8]
_020AF348:
	mov r0, #8
	bl WcmSetPhase
	bl WcmWmReset
	ldmia sp!, {r4, pc}
_020AF358:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AF380 ; =_021D5FEC
	ldr r3, _020AF398 ; =0x0000081B
	ldr r1, [r0]
	mov r0, #7
	add r1, r1, #0x2140
	mov r2, #0
	bl WcmNotify
	ldmia sp!, {r4, pc}
	.align 2, 0
_020AF380: .word _021D5FEC
_020AF384: .word 0x000007D7
_020AF388: .word WcmWmcbStartDCF
_020AF38C: .word 0x000007ED
_020AF390: .word 0x000007F6
_020AF394: .word 0x00000804
_020AF398: .word 0x0000081B
	arm_func_end WcmWmcbConnect
