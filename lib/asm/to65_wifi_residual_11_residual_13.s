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
	.public dt_ack
	.extern find_socket
	.extern tcp_send_ack
	.extern tcp_send_finack
	.extern tcp_send_rst

.text
	arm_func_start dt_ack
dt_ack: ; 0x020A5B40
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r4, r0
	mov sb, r1
	mov r8, r2
	bl find_socket
	movs r5, r0
	bne _020A5B74
	mov r0, r4
	mov r1, sb
	mov r2, r8
	mov r3, #0
	bl tcp_send_rst
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020A5B74:
	ldrh r6, [sb, #0xa]
	ldrh r3, [sb, #8]
	ldr r2, [r5, #0x30]
	mov r0, r6, lsl #8
	mov r1, r3, lsl #8
	orr r3, r1, r3, asr #8
	orr r1, r0, r6, asr #8
	mov r0, r3, lsl #0x10
	mov r1, r1, lsl #0x10
	mov r3, r0, lsr #0x10
	mov r0, r1, lsr #0x10
	orr r1, r0, r3, lsl #16
	sub r0, r1, r2
	ldrb r6, [sb, #0xd]
	cmp r0, #0
	strgt r1, [r5, #0x30]
	ldrh r7, [sb, #6]
	ldrh r3, [sb, #4]
	ldrb r2, [r5, #8]
	mov r0, r7, lsl #8
	mov r1, r3, lsl #8
	orr r3, r1, r3, asr #8
	orr r1, r0, r7, asr #8
	mov r0, r3, lsl #0x10
	mov r1, r1, lsl #0x10
	mov r3, r0, lsr #0x10
	mov r0, r1, lsr #0x10
	cmp r2, #4
	orr r1, r0, r3, lsl #16
	bne _020A5C08
	ldr r0, [r5, #0x24]
	cmp r0, r1
	beq _020A5C08
	mov r0, r5
	mov r1, #0
	bl tcp_send_ack
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020A5C08:
	ldrh r1, [sb, #0xe]
	mov r0, r1, lsl #8
	orr r0, r0, r1, asr #8
	strh r0, [r5, #0x2c]
	ldrb r0, [r5, #8]
	cmp r0, #9
	addls pc, pc, r0, lsl #2
	b _020A5E34
_020A5C28: ; jump table
	b _020A5C50 ; case 0
	b _020A5E34 ; case 1
	b _020A5C50 ; case 2
	b _020A5C68 ; case 3
	b _020A5C94 ; case 4
	b _020A5E34 ; case 5
	b _020A5E10 ; case 6
	b _020A5D9C ; case 7
	b _020A5D9C ; case 8
	b _020A5E10 ; case 9
_020A5C50:
	mov r0, r4
	mov r1, sb
	mov r2, r8
	mov r3, #0
	bl tcp_send_rst
	b _020A5E50
_020A5C68:
	mov r0, #4
	strb r0, [r5, #8]
	ldr r0, [r5, #4]
	cmp r0, #1
	bne _020A5C8C
	mov r0, #0
	str r0, [r5, #4]
	ldr r0, [r5]
	bl OS_WakeupThreadDirect
_020A5C8C:
	cmp r8, #0
	beq _020A5E50
_020A5C94:
	ldr r0, [r5, #0x34]
	add r0, r0, #1
	str r0, [r5, #0x34]
	ldr r1, [r5, #0x3c]
	ldr r0, [r5, #0x44]
	sub r0, r1, r0
	cmp r8, r0
	movhi r7, #0
	movhi r8, r0
	movls r7, #1
	cmp r8, #0
	beq _020A5D30
	bl OS_DisableInterrupts
	ldrb r1, [sb, #0xc]
	ldr ip, [r5, #0x40]
	ldr r3, [r5, #0x44]
	and r2, r1, #0xf0
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	mov r4, r0
	mov r2, r8
	add r0, sb, r1, asr #2
	add r1, ip, r3
	bl MI_CpuCopy8
	ldr r1, [r5, #0x44]
	mov r0, r4
	add r1, r1, r8
	str r1, [r5, #0x44]
	ldr r1, [r5, #0x24]
	add r1, r1, r8
	str r1, [r5, #0x24]
	bl OS_RestoreInterrupts
	ldr r0, [r5, #4]
	cmp r0, #2
	bne _020A5D30
	mov r0, #0
	str r0, [r5, #4]
	ldr r0, [r5]
	bl OS_WakeupThreadDirect
_020A5D30:
	cmp r7, #0
	beq _020A5D84
	tst r6, #1
	beq _020A5D84
	mov r0, #6
	strb r0, [r5, #8]
	ldr r1, [r5, #0x24]
	mov r0, r5
	add r2, r1, #1
	mov r1, #0
	str r2, [r5, #0x24]
	bl tcp_send_finack
	cmp r8, #0
	ldreq r0, [r5, #4]
	cmpeq r0, #2
	bne _020A5E50
	mov r0, #0
	str r0, [r5, #4]
	ldr r0, [r5]
	bl OS_WakeupThreadDirect
	b _020A5E50
_020A5D84:
	cmp r8, #0
	beq _020A5E50
	mov r0, r5
	mov r1, #0
	bl tcp_send_ack
	b _020A5E50
_020A5D9C:
	tst r6, #1
	beq _020A5DE4
	ldr r1, [r5, #0x24]
	add r0, r8, #1
	add r2, r1, r0
	mov r0, r5
	mov r1, #0
	str r2, [r5, #0x24]
	bl tcp_send_ack
	mov r1, #0
	strb r1, [r5, #8]
	ldr r0, [r5, #4]
	cmp r0, #2
	bne _020A5E50
	str r1, [r5, #4]
	ldr r0, [r5]
	bl OS_WakeupThreadDirect
	b _020A5E50
_020A5DE4:
	cmp r8, #0
	beq _020A5E04
	ldr r1, [r5, #0x24]
	mov r0, r5
	add r2, r1, r8
	mov r1, #0
	str r2, [r5, #0x24]
	bl tcp_send_ack
_020A5E04:
	mov r0, #8
	strb r0, [r5, #8]
	b _020A5E50
_020A5E10:
	mov r1, #0
	strb r1, [r5, #8]
	ldr r0, [r5, #4]
	cmp r0, #2
	bne _020A5E50
	str r1, [r5, #4]
	ldr r0, [r5]
	bl OS_WakeupThreadDirect
	b _020A5E50
_020A5E34:
	tst r6, #1
	ldrne r0, [r5, #0x24]
	mov r1, #0
	addne r0, r0, #1
	strne r0, [r5, #0x24]
	mov r0, r5
	bl tcp_send_ack
_020A5E50:
	bl OS_YieldThread
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end dt_ack
