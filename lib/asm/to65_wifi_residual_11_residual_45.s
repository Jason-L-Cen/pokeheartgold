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
	.extern WCM_UpdateOption
	.extern WcmSetPhase
	.extern WcmWmcbCommon

.text
	arm_func_start WCM_ConnectAsync
WCM_ConnectAsync: ; 0x020ADEF0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	bl OS_DisableInterrupts
	ldr r2, _020AE0E8 ; =_021D5FEC
	mov r4, r0
	ldr ip, [r2]
	cmp ip, #0
	bne _020ADF24
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020ADF24:
	add r1, ip, #0x2000
	ldr r3, [r1, #0x260]
	cmp r3, #3
	beq _020ADF48
	cmp r3, #8
	beq _020AE038
	cmp r3, #9
	beq _020AE044
	b _020AE050
_020ADF48:
	cmp r7, #0
	bne _020ADF5C
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020ADF5C:
	ldrh r3, [r7, #0x3c]
	cmp r3, #0
	beq _020ADF74
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020ADF74:
	cmp r6, #0
	beq _020ADFEC
	ldrb r3, [r6]
	cmp r3, #4
	ldrlob r0, [r6, #1]
	cmplo r0, #4
	blo _020ADFA0
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020ADFA0:
	strb r3, [r1, #0x250]
	ldr r0, [r2]
	ldrb r1, [r6, #1]
	add r0, r0, #0x2000
	strb r1, [r0, #0x251]
	ldr r1, [r2]
	mov r2, #0x50
	add r0, r1, #0x2000
	ldrb r0, [r0, #0x250]
	cmp r0, #0
	bne _020ADFDC
	add r0, r1, #0x2200
	mov r1, #0
	bl MI_CpuFill8
	b _020ADFFC
_020ADFDC:
	add r0, r6, #2
	add r1, r1, #0x2200
	bl MI_CpuCopy8
	b _020ADFFC
_020ADFEC:
	add r0, ip, #0x2200
	mov r1, #0
	mov r2, #0x52
	bl MI_CpuFill8
_020ADFFC:
	ldr r1, _020AE0E8 ; =_021D5FEC
	mov r0, r7
	ldr r1, [r1]
	mov r2, #0xc0
	add r1, r1, #0x2140
	bl MI_CpuCopy8
	ldr r1, _020AE0E8 ; =_021D5FEC
	mov r0, r5
	ldr r1, [r1]
	add r1, r1, #0x2100
	ldrh r2, [r1, #0x6e]
	orr r2, r2, #3
	strh r2, [r1, #0x70]
	bl WCM_UpdateOption
	b _020AE05C
_020AE038:
	bl OS_RestoreInterrupts
	mov r0, #2
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020AE044:
	bl OS_RestoreInterrupts
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020AE050:
	bl OS_RestoreInterrupts
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020AE05C:
	ldr r1, _020AE0EC ; =0x0000FFFF
	ldr r0, _020AE0F0 ; =WcmWmcbCommon
	mov r3, r1
	mov r2, #0x50
	str r1, [sp]
	bl WM_SetLifeTime
	cmp r0, #2
	beq _020AE090
	cmp r0, #3
	beq _020AE0C0
	cmp r0, #8
	beq _020AE0B0
	b _020AE0C0
_020AE090:
	mov r0, #8
	bl WcmSetPhase
	ldr r0, _020AE0E8 ; =_021D5FEC
	mov r1, #5
	ldr r0, [r0]
	add r0, r0, #0x2200
	strh r1, [r0, #0x80]
	b _020AE0D8
_020AE0B0:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #4
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020AE0C0:
	mov r0, #0xb
	bl WcmSetPhase
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #7
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020AE0D8:
	mov r0, r4
	bl OS_RestoreInterrupts
	mov r0, #3
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020AE0E8: .word _021D5FEC
_020AE0EC: .word 0x0000FFFF
_020AE0F0: .word WcmWmcbCommon
	arm_func_end WCM_ConnectAsync
