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
	.public send_ip_frag

.text
	arm_func_start send_ip_frag
send_ip_frag: ; 0x020A4758
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	mov r7, r1
	mov r5, r3
	add r1, r7, #0x14
	add r1, r1, r5
	mov r1, r1, lsl #0x10
	ldr r3, [sp, #0x24]
	mov r4, r1, lsr #0x10
	mov r1, r3, lsl #0x10
	mov r3, r4, lsl #8
	orr r3, r3, r4, asr #8
	mov r8, r0
	mov r1, r1, lsr #0x10
	mov r0, r1, lsl #8
	strh r3, [r8, #-0x12]
	orr r0, r0, r1, asr #8
	strh r0, [r8, #-0xe]
	mov r3, #0
	sub r0, r8, #0x14
	mov r1, #0x14
	mov r6, r2
	strh r3, [r8, #-0xa]
	ldr r4, [sp, #0x20]
	bl calc_checksum
	mov r2, r0, lsl #8
	ldr r1, _020A4870 ; =0x7F000001
	orr r0, r2, r0, asr #8
	strh r0, [r8, #-0xa]
	cmp r4, r1
	ldrne r0, _020A4874 ; =_021D4620
	ldrne r0, [r0, #0x50]
	cmpne r4, r0
	beq _020A4800
	mov r2, r6
	mov r3, r5
	str r4, [sp]
	mov ip, #0x800
	sub r0, r8, #0x14
	add r1, r7, #0x14
	str ip, [sp, #4]
	bl send_ether
_020A4800:
	ldr r0, _020A4870 ; =0x7F000001
	cmp r4, r0
	ldrne r0, _020A4874 ; =_021D4620
	ldrne r0, [r0, #0x50]
	cmpne r4, r0
	beq _020A482C
	mov r0, r4
	bl is_multicast
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
_020A482C:
	ldr r0, _020A4878 ; =_02110690
	sub r1, r8, #0x1c
	mov r2, #8
	bl MI_CpuCopy8
	bl OS_DisableInterrupts
	mov r4, r0
	ldr r0, _020A487C ; =_021D4B08
	str r6, [sp]
	mov r1, r0
	str r5, [sp, #4]
	sub r2, r8, #0x1c
	add r3, r7, #0x1c
	bl put_in_buffer
	mov r0, r4
	bl OS_RestoreInterrupts
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020A4870: .word 0x7F000001
_020A4874: .word _021D4620
_020A4878: .word _02110690
_020A487C: .word _021D4B08
	arm_func_end send_ip_frag
