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
	.public cache_session
	.public purge_session

.text
	arm_func_start cache_session
cache_session: ; 0x020A86CC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r8, r0
	mov r7, r1
	mov r6, r2
	bl OS_DisableInterrupts
	mov r4, r0
	bl OS_GetTick
	ldr sb, _020A87B4 ; =_021D5E68
	mov r5, r0, lsr #0x10
	mov r2, #0
	mov r0, r2
	mov r3, sb
	orr r5, r5, r1, lsl #16
	mvn ip, #0
_020A8704:
	ldrb lr, [r3, #0x5a]
	cmp lr, #0
	cmpne r7, #0
	beq _020A8738
	ldr r1, [r3, #0x54]
	cmp r7, r1
	bne _020A8738
	cmp r6, #0
	beq _020A8738
	ldrh r1, [r3, #0x58]
	cmp r6, r1
	moveq sb, r3
	beq _020A8774
_020A8738:
	cmp r2, ip
	beq _020A8764
	cmp lr, #0
	moveq r2, ip
	moveq sb, r3
	beq _020A8764
	ldr r1, [r3, #0x50]
	sub r1, r5, r1
	cmp r1, r2
	movhi r2, r1
	movhi sb, r3
_020A8764:
	add r0, r0, #1
	cmp r0, #4
	add r3, r3, #0x5c
	blt _020A8704
_020A8774:
	mov r1, sb
	add r0, r8, #0x74
	mov r2, #0x20
	bl MI_CpuCopy8
	mov r0, r8
	add r1, sb, #0x20
	mov r2, #0x30
	bl MI_CpuCopy8
	str r5, [sb, #0x50]
	mov r0, #1
	strb r0, [sb, #0x5a]
	str r7, [sb, #0x54]
	mov r0, r4
	strh r6, [sb, #0x58]
	bl OS_RestoreInterrupts
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020A87B4: .word _021D5E68
	arm_func_end cache_session

	arm_func_start purge_session
purge_session: ; 0x020A87B8
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r8, r0
	bl OS_DisableInterrupts
	ldr r7, _020A881C ; =_021D5E68
	mov r6, r0
	mov r5, #0
	mov r4, #0x20
_020A87D4:
	ldrb r0, [r7, #0x5a]
	cmp r0, #0
	beq _020A8800
	mov r0, r7
	mov r2, r4
	add r1, r8, #0x74
	bl memcmp
	cmp r0, #0
	moveq r0, #0
	streqb r0, [r7, #0x5a]
	beq _020A8810
_020A8800:
	add r5, r5, #1
	cmp r5, #4
	add r7, r7, #0x5c
	blt _020A87D4
_020A8810:
	mov r0, r6
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020A881C: .word _021D5E68
	arm_func_end purge_session
