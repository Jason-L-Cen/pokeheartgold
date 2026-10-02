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
	.public WcmWmcbCommon
	.public WcmWmcbScanEx
	.extern WCMi_EntryApList
	.extern WcmCountBits
	.extern WcmCountLeadingZero
	.extern WcmGetNextScanChannel
	.extern WcmNotify
	.extern WcmNotifyEx
	.extern WcmSetPhase
	.extern WcmWmReset
	.extern WcmWmcbConnect
	.extern WcmWmcbEndScan

.text
	arm_func_start WcmWmcbCommon
WcmWmcbCommon: ; 0x020AEB20
	stmdb sp!, {r3, lr}
	ldrh r1, [r0, #2]
	mov r2, #0x14
	cmp r1, #4
	addls pc, pc, r1, lsl #2
	b _020AED80
_020AEB38: ; jump table
	b _020AEB4C ; case 0
	b _020AED48 ; case 1
	b _020AED80 ; case 2
	b _020AED80 ; case 3
	b _020AED80 ; case 4
_020AEB4C:
	ldrh r0, [r0]
	cmp r0, #0x19
	bgt _020AEB84
	bge _020AEC48
	cmp r0, #6
	addls pc, pc, r0, lsl #2
	b _020AECC0
_020AEB68: ; jump table
	b _020AECC0 ; case 0
	b _020AECC0 ; case 1
	b _020AECC0 ; case 2
	b _020AEBA0 ; case 3
	b _020AEBB0 ; case 4
	b _020AEC04 ; case 5
	b _020AEC24 ; case 6
_020AEB84:
	cmp r0, #0x1d
	bgt _020AEB94
	beq _020AEC34
	b _020AECC0
_020AEB94:
	cmp r0, #0x27
	beq _020AEC70
	b _020AECC0
_020AEBA0:
	ldr r0, _020AEDB8 ; =WcmWmcbCommon
	bl WM_PowerOn
	mov r2, r0
	b _020AECC0
_020AEBB0:
	bl WM_Finish
	cmp r0, #0
	beq _020AEBC4
	cmp r0, #4
	b _020AEBE4
_020AEBC4:
	mov r0, #1
	bl WcmSetPhase
	mov r0, #0
	ldr r3, _020AEDBC ; =0x00000663
	mov r1, r0
	mov r2, r0
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AEBE4:
	mov r0, #0xb
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AEDC0 ; =0x0000066C
	mov r2, r1
	mov r0, #7
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AEC04:
	mov r0, #3
	bl WcmSetPhase
	mov r0, #0
	ldr r3, _020AEDC4 ; =0x00000673
	mov r1, r0
	mov r2, r0
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AEC24:
	ldr r0, _020AEDB8 ; =WcmWmcbCommon
	bl WM_Disable
	mov r2, r0
	b _020AECC0
_020AEC34:
	ldr r0, _020AEDB8 ; =WcmWmcbCommon
	mov r1, #0
	bl WM_SetBeaconIndication
	mov r2, r0
	b _020AECC0
_020AEC48:
	ldr r1, _020AEDC8 ; =_021D5FEC
	ldr r0, _020AEDB8 ; =WcmWmcbCommon
	ldr r3, [r1]
	add r2, r3, #0x2000
	ldrb r1, [r2, #0x250]
	ldrb r2, [r2, #0x251]
	add r3, r3, #0x2200
	bl WM_SetWEPKeyEx
	mov r2, r0
	b _020AECC0
_020AEC70:
	ldr r0, _020AEDC8 ; =_021D5FEC
	ldr r2, [r0]
	add r0, r2, #0x2000
	ldr r1, [r0, #0x264]
	and r0, r1, #0xc0000
	cmp r0, #0xc0000
	moveq r0, #1
	movne r0, #0
	mov r0, r0, lsl #0x10
	mov ip, r0, lsr #0x10
	and r1, r1, #0x30000
	cmp r1, #0x30000
	movne r3, #1
	add r1, r2, #0x2140
	ldr r0, _020AEDCC ; =WcmWmcbConnect
	moveq r3, #0
	mov r2, #0
	str ip, [sp]
	bl WM_StartConnectEx
	mov r2, r0
_020AECC0:
	cmp r2, #2
	ldmeqia sp!, {r3, pc}
	cmp r2, #3
	beq _020AED10
	cmp r2, #8
	bne _020AED10
	mov r0, #0xc
	bl WcmSetPhase
	ldr r0, _020AEDC8 ; =_021D5FEC
	mov r2, #0
	ldr r1, [r0]
	add r0, r1, #0x2200
	ldrsh r0, [r0, #0x80]
	cmp r0, #5
	addeq r1, r1, #0x2140
	mov r0, #1
	movne r1, #0
	rsb r3, r0, #0x6b0
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AED10:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AEDC8 ; =_021D5FEC
	ldr r3, _020AEDD0 ; =0x000006B8
	ldr r1, [r0]
	mov r2, #0
	add r0, r1, #0x2200
	ldrsh r0, [r0, #0x80]
	cmp r0, #5
	addeq r1, r1, #0x2140
	movne r1, #0
	mov r0, #7
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AED48:
	mov r0, #0xc
	bl WcmSetPhase
	ldr r0, _020AEDC8 ; =_021D5FEC
	ldr r3, _020AEDD4 ; =0x000006DE
	ldr r1, [r0]
	mov r2, #0
	add r0, r1, #0x2200
	ldrsh r0, [r0, #0x80]
	cmp r0, #5
	addeq r1, r1, #0x2140
	movne r1, #0
	mov r0, #1
	bl WcmNotify
	ldmia sp!, {r3, pc}
_020AED80:
	mov r0, #0xb
	bl WcmSetPhase
	ldr r0, _020AEDC8 ; =_021D5FEC
	ldr r3, _020AEDD8 ; =0x000006E8
	ldr r1, [r0]
	mov r2, #0
	add r0, r1, #0x2200
	ldrsh r0, [r0, #0x80]
	cmp r0, #5
	addeq r1, r1, #0x2140
	movne r1, #0
	mov r0, #7
	bl WcmNotify
	ldmia sp!, {r3, pc}
	.align 2, 0
_020AEDB8: .word WcmWmcbCommon
_020AEDBC: .word 0x00000663
_020AEDC0: .word 0x0000066C
_020AEDC4: .word 0x00000673
_020AEDC8: .word _021D5FEC
_020AEDCC: .word WcmWmcbConnect
_020AEDD0: .word 0x000006B8
_020AEDD4: .word 0x000006DE
_020AEDD8: .word 0x000006E8
	arm_func_end WcmWmcbCommon

	arm_func_start WcmWmcbScanEx
WcmWmcbScanEx: ; 0x020AEDDC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #4
	mov r5, r0
	ldrh r0, [r5, #2]
	mov r4, #0x14
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	b _020AF074
_020AEDFC: ; jump table
	b _020AEE10 ; case 0
	b _020AF068 ; case 1
	b _020AF074 ; case 2
	b _020AF074 ; case 3
	b _020AF074 ; case 4
_020AEE10:
	ldr r0, _020AF098 ; =_021D5FEC
	ldr r0, [r0]
	add r0, r0, #0x2000
	ldr r0, [r0, #0x260]
	cmp r0, #5
	bne _020AEE44
	mov r0, #6
	bl WcmSetPhase
	mov r0, #0
	ldr r3, _020AF09C ; =0x00000704
	mov r1, r0
	mov r2, r0
	bl WcmNotify
_020AEE44:
	ldr r1, _020AF098 ; =_021D5FEC
	ldr r2, [r1]
	add r0, r2, #0x2000
	ldr r0, [r0, #0x260]
	cmp r0, #6
	beq _020AEE70
	cmp r0, #7
	beq _020AEFE8
	cmp r0, #0xd
	beq _020AEFF8
	b _020AF004
_020AEE70:
	add r0, r2, #0x2200
	mov r2, #7
	strh r2, [r0, #0x80]
	ldrh r0, [r5, #8]
	cmp r0, #5
	bne _020AEEFC
	ldr r0, [r1]
	add r1, r0, #0x2200
	add r0, r0, #0x2000
	ldrh r1, [r1, #0x8c]
	ldr r0, [r0, #0x288]
	bl DC_InvalidateRange
	ldrh r0, [r5, #0xe]
	mov r4, #0
	cmp r0, #0
	ble _020AEEFC
	ldr r8, _020AF0A0 ; =0x0000071A
	mov r7, #7
	mov r6, r4
_020AEEBC:
	add r0, r5, r4, lsl #1
	add r2, r5, r4, lsl #2
	ldrh r1, [r0, #0x50]
	ldr r0, [r2, #0x10]
	bl WCMi_EntryApList
	str r8, [sp]
	add r0, r5, r4, lsl #2
	ldr r2, [r0, #0x10]
	mov r0, r7
	mov r1, r6
	mov r3, r5
	bl WcmNotifyEx
	ldrh r0, [r5, #0xe]
	add r4, r4, #1
	cmp r4, r0
	blt _020AEEBC
_020AEEFC:
	ldr r0, _020AF098 ; =_021D5FEC
	ldr r0, [r0]
	add r0, r0, #0x2000
	ldr r1, [r0, #0x264]
	and r0, r1, #0xc00000
	cmp r0, #0xc00000
	bne _020AEF68
	ldr r0, _020AF0A4 ; =0x00003FFE
	and r0, r1, r0
	bl WcmCountBits
	movs r1, r0
	beq _020AEF68
	ldr r0, _020AF098 ; =_021D5FEC
	ldr r0, [r0]
	add r0, r0, #0x2000
	ldr r6, [r0, #0x284]
	mov r0, r6
	bl _u32_div_f
	cmp r1, #0
	bne _020AEF68
	ldr r4, _020AF0A8 ; =0x00000728
	mov r1, #0
	mov r2, r6
	mov r3, r1
	mov r0, #8
	str r4, [sp]
	bl WcmNotifyEx
_020AEF68:
	ldrh r0, [r5, #0xa]
	bl WcmCountLeadingZero
	rsb r0, r0, #0x20
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bl WcmGetNextScanChannel
	ldr r1, _020AF098 ; =_021D5FEC
	mov r2, #1
	mov r2, r2, lsl r0
	ldr r0, [r1]
	mov r2, r2, asr #1
	add r0, r0, #0x2200
	strh r2, [r0, #0x8e]
	ldr r0, [r1]
	add r1, r0, #0x2200
	add r0, r0, #0x2000
	ldrh r1, [r1, #0x8c]
	ldr r0, [r0, #0x288]
	bl DC_InvalidateRange
	ldr r2, _020AF098 ; =_021D5FEC
	ldr r0, _020AF0AC ; =WcmWmcbScanEx
	ldr r1, [r2]
	add r1, r1, #0x2000
	ldr r3, [r1, #0x284]
	add r3, r3, #1
	str r3, [r1, #0x284]
	ldr r1, [r2]
	add r1, r1, #0x288
	add r1, r1, #0x2000
	bl WM_StartScanEx
	mov r4, r0
	b _020AF004
_020AEFE8:
	ldr r0, _020AF0B0 ; =WcmWmcbEndScan
	bl WM_EndScan
	mov r4, r0
	b _020AF004
_020AEFF8:
	bl WcmWmReset
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020AF004:
	cmp r4, #2
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	cmp r4, #3
	beq _020AF044
	cmp r4, #8
	bne _020AF044
	mov r0, #0xc
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF0B4 ; =0x00000753
	mov r2, r1
	mov r0, #1
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020AF044:
	mov r0, #0xb
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF0B8 ; =0x0000075C
	mov r2, r1
	mov r0, #7
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020AF068:
	bl WcmWmReset
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020AF074:
	mov r0, #0xb
	bl WcmSetPhase
	mov r1, #0
	ldr r3, _020AF0BC ; =0x0000076D
	mov r2, r1
	mov r0, #7
	bl WcmNotify
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020AF098: .word _021D5FEC
_020AF09C: .word 0x00000704
_020AF0A0: .word 0x0000071A
_020AF0A4: .word 0x00003FFE
_020AF0A8: .word 0x00000728
_020AF0AC: .word WcmWmcbScanEx
_020AF0B0: .word WcmWmcbEndScan
_020AF0B4: .word 0x00000753
_020AF0B8: .word 0x0000075C
_020AF0BC: .word 0x0000076D
	arm_func_end WcmWmcbScanEx
