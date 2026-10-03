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
	.extern rawip
	.extern resolve_sub
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
	arm_func_start find_session_from_id
find_session_from_id: ; 0x020A8598
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r7, r0
	bl OS_DisableInterrupts
	mov r4, #0
	ldr r6, _020A8628 ; =_021D5E68
	mov r5, r0
	strb r4, [r7, #0x30]
	mov r8, #0x20
_020A85B8:
	ldrb r0, [r6, #0x5a]
	cmp r0, #0
	beq _020A860C
	ldr r0, [r6, #0x54]
	cmp r0, #0
	ldreqh r0, [r6, #0x58]
	cmpeq r0, #0
	bne _020A860C
	mov r0, r6
	mov r2, r8
	add r1, r7, #0x74
	bl memcmp
	cmp r0, #0
	bne _020A860C
	mov r1, r7
	add r0, r6, #0x20
	mov r2, #0x30
	bl MI_CpuCopy8
	mov r0, #1
	strb r0, [r7, #0x30]
	b _020A861C
_020A860C:
	add r4, r4, #1
	cmp r4, #4
	add r6, r6, #0x5c
	blt _020A85B8
_020A861C:
	mov r0, r5
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020A8628: .word _021D5E68
	arm_func_end find_session_from_id
