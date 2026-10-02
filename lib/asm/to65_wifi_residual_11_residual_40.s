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
	.public CPSi_big_sqr
	.extern CPSi_big_add_part
	.extern count_digits

.text
	arm_func_start CPSi_big_sqr
CPSi_big_sqr: ; 0x020ACE3C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	mov r8, r2
	mov sl, r0
	mov r0, sb
	mov r1, r8
	bl count_digits
	mov fp, r0
	cmp r8, fp, lsl #1
	mov r0, fp, lsl #1
	ble _020ACE7C
	sub r1, r8, r0
	add r0, sl, r0, lsl #1
	mov r2, r1, lsl #1
	mov r1, #0
	bl MI_CpuFill8
_020ACE7C:
	cmp fp, #0
	mov r4, #0
	ble _020ACED0
	mov r5, r4
	sub r2, r8, #1
_020ACE90:
	cmp r5, r8
	bge _020ACED0
	mov r0, r4, lsl #1
	ldrh r3, [sb, r0]
	mov r0, r5, lsl #1
	cmp r5, r2
	mul r1, r3, r3
	strh r1, [sl, r0]
	beq _020ACED0
	add r4, r4, #1
	mov r1, r1, lsr #0x10
	add r0, sl, r0
	strh r1, [r0, #2]
	cmp r4, fp
	add r5, r5, #2
	blt _020ACE90
_020ACED0:
	mov r6, #0
	cmp fp, #0
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020ACEDC:
	mov r0, r6, lsl #1
	add r4, r6, #1
	str r0, [sp]
	b _020ACF4C
_020ACEEC:
	mov r0, r4, lsl #1
	ldrh r1, [sb, r0]
	ldr r0, [sp]
	ldrh r0, [sb, r0]
	mul r7, r1, r0
	ldr r0, _020ACF6C ; =0x7FFF8000
	cmp r7, r0
	mov r0, sl
	bhi _020ACF24
	mov r2, r5
	mov r3, r8
	mov r1, r7, lsl #1
	bl CPSi_big_add_part
	b _020ACF48
_020ACF24:
	mov r1, r7
	mov r2, r5
	mov r3, r8
	bl CPSi_big_add_part
	mov r1, r7
	mov r2, r5
	mov r0, sl
	mov r3, r8
	bl CPSi_big_add_part
_020ACF48:
	add r4, r4, #1
_020ACF4C:
	cmp r4, fp
	addlt r5, r6, r4
	cmplt r5, r8
	blt _020ACEEC
	add r6, r6, #1
	cmp r6, fp
	blt _020ACEDC
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020ACF6C: .word 0x7FFF8000
	arm_func_end CPSi_big_sqr
