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
	.public reset_network_vars
	.extern CPSi_SslCleanup

.text
	arm_func_start reset_network_vars
reset_network_vars: ; 0x020A3874
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r1, _020A3968 ; =_021D4620
	ldr r2, _020A3968 ; =_021D4620
	ldr r1, [r1, #0x50]
	str r0, [r2, #0xc]
	cmp r1, #0
	mov r1, #0
	str r1, [r2, #0x50]
	str r1, [r2, #0x1c]
	str r1, [r2, #0x2c]
	str r1, [r2, #0x60]
	movne r3, #1
	str r1, [r2, #0x64]
	moveq r3, #0
	str r1, [r2, #0x20]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _020A396C ; =_021D46A0
	mov r2, #0x60
	bl MI_CpuFill8
	ldr r0, _020A3970 ; =OSi_ThreadInfo
	ldr r6, [r0, #8]
	cmp r6, #0
	beq _020A3924
	mov r5, #0
	mov r4, r5
_020A38DC:
	ldr r1, [r6, #0xa4]
	cmp r1, #0
	ldrne r0, [r1]
	cmpne r0, #0
	beq _020A3918
	ldrb r0, [r1, #8]
	cmp r0, #0xa
	cmpne r0, #0xb
	strneb r5, [r1, #8]
	ldr r0, [r1, #4]
	cmp r0, #0
	beq _020A3918
	str r4, [r1, #4]
	ldr r0, [r1]
	bl OS_WakeupThreadDirect
_020A3918:
	ldr r6, [r6, #0x68]
	cmp r6, #0
	bne _020A38DC
_020A3924:
	mov r6, #0
	ldr r7, _020A3974 ; =_021D4948
	ldr r4, _020A3968 ; =_021D4620
	mov r5, r6
_020A3934:
	ldrh r0, [r7, #4]
	cmp r0, #0
	beq _020A3950
	ldr r0, [r7, #0x34]
	ldr r1, [r4, #0x40]
	blx r1
	strh r5, [r7, #4]
_020A3950:
	add r6, r6, #1
	cmp r6, #8
	add r7, r7, #0x38
	blt _020A3934
	bl CPSi_SslCleanup
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020A3968: .word _021D4620
_020A396C: .word _021D46A0
_020A3970: .word OSi_ThreadInfo
_020A3974: .word _021D4948
	arm_func_end reset_network_vars
