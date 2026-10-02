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
	.public CPS_SocGetEport

.text
	arm_func_start CPS_SocGetEport
CPS_SocGetEport: ; 0x020A678C
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r0, _020A6828 ; =OSi_ThreadInfo
	ldr r2, _020A682C ; =_021D4620
	ldr r0, [r0, #8]
	ldr r1, _020A6830 ; =0x00001388
	mov r4, #0x400
	mov r3, #1
	mov r5, #0
_020A67AC:
	ldrh ip, [r2, #8]
	mov r6, r5
	add ip, ip, #1
	strh ip, [r2, #8]
	ldrh ip, [r2, #8]
	cmp ip, #0x400
	blo _020A67D0
	cmp ip, r1
	blo _020A67D4
_020A67D0:
	strh r4, [r2, #8]
_020A67D4:
	mov r7, r0
	cmp r0, #0
	beq _020A6814
	ldrh r8, [r2, #8]
_020A67E4:
	ldr lr, [r7, #0xa4]
	cmp lr, #0
	ldrne ip, [lr]
	cmpne ip, #0
	beq _020A6808
	ldrh ip, [lr, #0xa]
	cmp ip, r8
	moveq r6, r3
	beq _020A6814
_020A6808:
	ldr r7, [r7, #0x68]
	cmp r7, #0
	bne _020A67E4
_020A6814:
	cmp r6, #0
	bne _020A67AC
	ldr r0, _020A682C ; =_021D4620
	ldrh r0, [r0, #8]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020A6828: .word OSi_ThreadInfo
_020A682C: .word _021D4620
_020A6830: .word 0x00001388
	arm_func_end CPS_SocGetEport
