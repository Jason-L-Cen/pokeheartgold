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
	.extern is_broadcast
	.extern is_multicast
	.extern send_packet
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
	arm_func_start send_arprequest
send_arprequest: ; 0x020A43E0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x2c
	mov r4, r0
	add r0, sp, #0
	mov r1, #0
	mov r2, #0x2a
	bl MI_CpuFill8
	add r0, sp, #0
	mov r1, #0xff
	mov r2, #6
	bl MI_CpuFill8
	ldr r0, _020A44CC ; =_021D4B08
	add r1, sp, #6
	mov r2, #6
	bl MI_CpuCopy8
	mov r0, #1
	ldr r1, _020A44D0 ; =0x00000608
	strb r0, [sp, #0xf]
	strh r1, [sp, #0xc]
	strb r0, [sp, #0x15]
	mov r0, #8
	ldr r1, _020A44D4 ; =0x00000406
	strb r0, [sp, #0x10]
	strh r1, [sp, #0x12]
	ldr r0, _020A44CC ; =_021D4B08
	add r1, sp, #0x16
	mov r2, #6
	bl MI_CpuCopy8
	ldr r0, _020A44D8 ; =_021D4620
	mov r1, r4, lsr #0x10
	ldr r3, [r0, #0x50]
	mov r0, r1, lsl #0x10
	mov r2, r0, lsr #0x10
	mov r0, r4, lsl #0x10
	mov r1, r0, lsr #0x10
	mov r0, r3, lsr #0x10
	mov r0, r0, lsl #0x10
	mov r4, r0, lsr #0x10
	mov r0, r3, lsl #0x10
	mov r3, r0, lsr #0x10
	mov r0, r4, lsl #8
	orr r0, r0, r4, asr #8
	strh r0, [sp, #0x1c]
	mov r0, r3, lsl #8
	orr r0, r0, r3, asr #8
	strh r0, [sp, #0x1e]
	mov r0, r2, lsl #8
	orr r0, r0, r2, asr #8
	strh r0, [sp, #0x26]
	mov r0, r1, lsl #8
	orr r0, r0, r1, asr #8
	mov r2, #0
	strh r0, [sp, #0x28]
	add r0, sp, #0
	mov r1, #0x2a
	mov r3, r2
	bl send_packet
	add sp, sp, #0x2c
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020A44CC: .word _021D4B08
_020A44D0: .word 0x00000608
_020A44D4: .word 0x00000406
_020A44D8: .word _021D4620
	arm_func_end send_arprequest
