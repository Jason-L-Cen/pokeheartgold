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
	.extern default_link_is_on
	.extern empty_func
	.extern scavenger
	.extern tcpip

.text
	arm_func_start CPS_Startup
CPS_Startup: ; 0x020A39AC
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	mov r4, r0
	ldr r0, _020A3BA4 ; =_version_UBIQUITOUS_CPS
	bl OSi_ReferSymbol
	ldr r3, [r4, #0x18]
	ldr r5, [r4, #0x14]
	cmp r3, #0
	cmpeq r5, #0
	mov r1, #0
	beq _020A3A04
	ldr r0, _020A3BA8 ; =_021D4620
	ldr r2, _020A3BAC ; =0x6C078965
	str r5, [r0, #0x68]
	str r3, [r0, #0x6c]
	ldr r3, _020A3BB0 ; =0x5D588B65
	str r2, [r0, #0x70]
	ldr r2, _020A3BB4 ; =0x00269EC3
	str r3, [r0, #0x74]
	str r2, [r0, #0x78]
	str r1, [r0, #0x7c]
	b _020A3A34
_020A3A04:
	bl OS_GetTick
	ldr r2, _020A3BA8 ; =_021D4620
	ldr r3, _020A3BAC ; =0x6C078965
	str r0, [r2, #0x68]
	str r1, [r2, #0x6c]
	ldr r1, _020A3BB0 ; =0x5D588B65
	str r3, [r2, #0x70]
	ldr r0, _020A3BB4 ; =0x00269EC3
	str r1, [r2, #0x74]
	str r0, [r2, #0x78]
	mov r0, #0
	str r0, [r2, #0x7c]
_020A3A34:
	ldr r1, [r4, #4]
	cmp r1, #0
	ldrne r0, [r4, #8]
	cmpne r0, #0
	beq _020A3A58
	ldr r0, _020A3BA8 ; =_021D4620
	str r1, [r0, #0x14]
	ldr r1, [r4, #8]
	b _020A3A64
_020A3A58:
	ldr r1, _020A3BB8 ; =empty_func
	ldr r0, _020A3BA8 ; =_021D4620
	str r1, [r0, #0x14]
_020A3A64:
	str r1, [r0, #0x40]
	ldr r1, [r4]
	ldr r0, _020A3BA8 ; =_021D4620
	ldr r2, _020A3BA8 ; =_021D4620
	str r1, [r0, #0x10]
	ldr r1, [r4, #0x24]
	cmp r1, #0
	ldreq r1, _020A3BBC ; =0x000005B4
	strh r1, [r0, #2]
	ldr r1, [r4, #0x28]
	ldr r0, _020A3BA8 ; =_021D4620
	str r1, [r0, #0x34]
	ldr r1, [r4, #0x2c]
	str r1, [r0, #0x24]
	ldr r1, [r4, #0xc]
	cmp r1, #0
	ldreq r1, _020A3BB8 ; =empty_func
	str r1, [r0, #0x18]
	ldr r1, [r4, #0x10]
	cmp r1, #0
	ldrne r0, _020A3BA8 ; =_021D4620
	ldreq r1, _020A3BC0 ; =default_link_is_on
	ldreq r0, _020A3BA8 ; =_021D4620
	str r1, [r0, #0x48]
	ldr r1, [r4, #0x1c]
	mov r0, #0
	str r1, [r2, #0x58]
	ldr r3, [r4, #0x20]
	ldr r1, _020A3BC4 ; =0x00000F88
	str r3, [r2, #0x5c]
	str r0, [r2, #0x30]
	str r0, [r2, #0x28]
	ldr ip, [r2, #0x70]
	ldr r4, [r2, #0x68]
	ldr r3, [r2, #0x6c]
	umull r5, lr, ip, r4
	mla lr, ip, r3, lr
	ldr r3, [r2, #0x74]
	ldr ip, [r2, #0x78]
	mla lr, r3, r4, lr
	ldr r3, [r2, #0x7c]
	adds ip, ip, r5
	adc r5, r3, lr
	umull r3, r4, r5, r1
	mla r4, r5, r0, r4
	mla r4, r0, r1, r4
	str ip, [r2, #0x68]
	ldr r0, _020A3BC8 ; =_021D4B08
	str r5, [r2, #0x6c]
	add r1, r4, #0x400
	strh r1, [r2, #8]
	bl OS_GetMacAddress
	ldr r0, _020A3BA8 ; =_021D4620
	mov r2, #0
	strb r2, [r0, #1]
	mov r1, #0x800
	str r1, [sp]
	ldr r0, _020A3BCC ; =_02110684
	ldr r1, _020A3BD0 ; =tcpip
	ldr r4, [r0]
	ldr r0, _020A3BD4 ; =_021D4888
	ldr r3, _020A3BD8 ; =_021D5E60
	str r4, [sp, #4]
	bl OS_CreateThread
	mov r1, #0x800
	ldr r0, _020A3BCC ; =_02110684
	str r1, [sp]
	ldr r1, [r0]
	ldr r0, _020A3BDC ; =_021D47C8
	str r1, [sp, #4]
	ldr r1, _020A3BE0 ; =scavenger
	ldr r3, _020A3BE4 ; =_021D5660
	mov r2, #0
	bl OS_CreateThread
	ldr r0, _020A3BD4 ; =_021D4888
	bl OS_WakeupThreadDirect
	ldr r0, _020A3BDC ; =_021D47C8
	bl OS_WakeupThreadDirect
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020A3BA4: .word _version_UBIQUITOUS_CPS
_020A3BA8: .word _021D4620
_020A3BAC: .word 0x6C078965
_020A3BB0: .word 0x5D588B65
_020A3BB4: .word 0x00269EC3
_020A3BB8: .word empty_func
_020A3BBC: .word 0x000005B4
_020A3BC0: .word default_link_is_on
_020A3BC4: .word 0x00000F88
_020A3BC8: .word _021D4B08
_020A3BCC: .word _02110684
_020A3BD0: .word tcpip
_020A3BD4: .word _021D4888
_020A3BD8: .word _021D5E60
_020A3BDC: .word _021D47C8
_020A3BE0: .word scavenger
_020A3BE4: .word _021D5660
	arm_func_end CPS_Startup
