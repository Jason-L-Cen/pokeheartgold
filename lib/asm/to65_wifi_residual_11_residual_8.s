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
	.public reg_arpcache
	.extern ip_islocal
	.extern is_multicast

.text
	arm_func_start reg_arpcache
reg_arpcache: ; 0x020A454C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r3, _020A4678 ; =0x7F000001
	mov r6, r1
	mov r7, r0
	cmp r6, r3
	ldrne r0, _020A467C ; =_021D4620
	mov r5, r2
	ldrne r0, [r0, #0x50]
	cmpne r6, r0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, r6
	bl ip_islocal
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, r6
	bl is_multicast
	cmp r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	bl OS_GetTick
	mov r0, r0, lsr #0x10
	orr r0, r0, r1, lsl #16
	mov r0, r0, lsl #0x10
	ldr r2, _020A4680 ; =_021D46A0
	mov r4, r0, lsr #0x10
	mov r1, #0
_020A45B0:
	ldr r0, [r2]
	cmp r6, r0
	bne _020A45E8
	mov r0, #0xc
	mul r5, r1, r0
	ldr r0, _020A4680 ; =_021D46A0
	ldr r3, _020A4684 ; =_021D46AA
	add r1, r0, r5
	mov r0, r7
	add r1, r1, #4
	mov r2, #6
	strh r4, [r3, r5]
	bl MI_CpuCopy8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020A45E8:
	add r1, r1, #1
	cmp r1, #8
	add r2, r2, #0xc
	blo _020A45B0
	cmp r5, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r2, #0
	ldr r3, _020A4680 ; =_021D46A0
	mov r1, r2
	mov r5, r2
_020A4610:
	ldr r0, [r3]
	cmp r0, #0
	moveq r1, r5
	beq _020A4648
	ldrh r0, [r3, #0xa]
	add r3, r3, #0xc
	sub r0, r4, r0
	mov r0, r0, lsl #0x10
	cmp r2, r0, asr #16
	movlt r1, r5
	add r5, r5, #1
	movlt r2, r0, lsr #0x10
	cmp r5, #8
	blo _020A4610
_020A4648:
	mov r0, #0xc
	mul r5, r1, r0
	ldr r3, _020A4680 ; =_021D46A0
	mov r0, r7
	add r1, r3, r5
	add r1, r1, #4
	mov r2, #6
	str r6, [r3, r5]
	bl MI_CpuCopy8
	ldr r0, _020A4684 ; =_021D46AA
	strh r4, [r0, r5]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020A4678: .word 0x7F000001
_020A467C: .word _021D4620
_020A4680: .word _021D46A0
_020A4684: .word _021D46AA
	arm_func_end reg_arpcache
