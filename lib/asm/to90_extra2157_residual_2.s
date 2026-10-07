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
	.extern calc_checksum
	.extern calc_checksum_do
	.extern invert_checksum
	.extern is_multicast
	.extern put_in_buffer
	.extern send_ether
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.extern send_ip

.text
	arm_func_start send_ping
send_ping: ; 0x020A4A78
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	mov r5, r2
	ldr r4, [r5, #0x4c]
	mov r6, r1
	mov r1, #8
	ldr r3, _020A4B20 ; =_021D4620
	ldr r2, _020A4B24 ; =OSi_ThreadInfo
	strh r1, [r4, #0x22]
	ldr r2, [r2, #4]
	ldrh lr, [r3, #4]
	mov r7, r0
	strh r2, [r4, #0x26]
	mov r2, #0
	strh r2, [r4, #0x24]
	strh lr, [r5, #0xa]
	add ip, lr, #1
	add r0, r4, #0x22
	strh ip, [r3, #4]
	strh lr, [r4, #0x28]
	bl calc_checksum_do
	mov r2, r0
	mov r0, r7
	mov r1, r6
	bl calc_checksum_do
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bl invert_checksum
	mov r2, r7
	mov r3, r6
	mov r1, r0, lsl #8
	orr r0, r1, r0, asr #8
	strh r0, [r4, #0x24]
	ldr r1, [r5, #0x1c]
	add r0, r4, #0x22
	str r1, [sp]
	mov r1, #1
	str r1, [sp, #4]
	mov r1, #8
	bl send_ip
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020A4B20: .word _021D4620
_020A4B24: .word OSi_ThreadInfo
	arm_func_end send_ping
