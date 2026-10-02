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
	.public scavenger
	.extern CPS_SocRegister
	.extern CPS_SocUnRegister
	.extern CPSi_SslPeriodical
	.extern dhcp_discover_server
	.extern dhcp_release_server
	.extern dhcp_request_server
	.extern reset_network_vars
	.extern send_arprequest
	.extern set_fixed_ip

.text
	arm_func_start scavenger
scavenger: ; 0x020A7448
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r3, _020A7798 ; =_021D4620
	mov r1, #0
	ldr r0, _020A779C ; =_021D4700
	mov r2, #0x64
	str r1, [r3, #0x44]
	bl MI_CpuFill8
	ldr r1, _020A7798 ; =_021D4620
	mov r3, #0x180
	ldr r0, _020A77A0 ; =_021D4CCC
	str r3, [r1, #0x11c]
	str r0, [r1, #0x120]
	ldr r2, _020A77A4 ; =_021D4B4C
	str r3, [r1, #0x128]
	ldr r0, _020A779C ; =_021D4700
	str r2, [r1, #0x12c]
	bl CPS_SocRegister
	mov sl, #1
	ldr r0, _020A7798 ; =_021D4620
	mov fp, sl
	str sl, [sp]
	str sl, [r0, #0xc]
	mov r4, #0
_020A74A4:
	mov r0, #0x3e8
	bl OS_Sleep
	ldr r0, _020A7798 ; =_021D4620
	ldr r1, [r0, #0x44]
	cmp r1, #0
	bne _020A7778
	bl OS_GetTick
	ldr r2, _020A7798 ; =_021D4620
	mov r5, r0, lsr #0x10
	ldr r0, [r2, #0x48]
	orr r5, r5, r1, lsl #16
	blx r0
	cmp r0, #0
	beq _020A75D0
	ldr r0, [sp]
	subs r0, r0, #1
	str r0, [sp]
	bne _020A75E4
	ldr r0, _020A7798 ; =_021D4620
	ldr r1, [r0, #0x10]
	tst r1, #1
	beq _020A7510
	cmp r4, #0
	bne _020A75E4
	bl set_fixed_ip
	mov r4, #1
	b _020A75E4
_020A7510:
	cmp r4, #3
	addls pc, pc, r4, lsl #2
	b _020A75E4
_020A751C: ; jump table
	b _020A752C ; case 0
	b _020A7570 ; case 1
	b _020A7594 ; case 2
	b _020A75E4 ; case 3
_020A752C:
	cmp fp, #0
	movne r1, #2
	strne r1, [r0, #0xc]
	movne fp, #0
	bl dhcp_discover_server
	cmp r0, #0
	beq _020A755C
	add r0, sp, #0
	mov r1, #0
	bl dhcp_request_server
	cmp r0, #0
	bne _020A7568
_020A755C:
	bl set_fixed_ip
	mov r4, #3
	b _020A75E4
_020A7568:
	mov r4, #1
	b _020A75E4
_020A7570:
	add r0, sp, #0
	mov r1, #1
	bl dhcp_request_server
	cmp r0, #0
	bne _020A75E4
	ldr r0, [sp]
	cmp r0, #0x3c
	movlo r4, #2
	b _020A75E4
_020A7594:
	add r0, sp, #0
	mov r1, #2
	bl dhcp_request_server
	cmp r0, #0
	movne r4, #1
	bne _020A75E4
	ldr r0, [sp]
	cmp r0, #0x3c
	bhs _020A75E4
	mov r0, #3
	bl reset_network_vars
	mov sl, #1
	str sl, [sp]
	mov r4, #0
	b _020A75E4
_020A75D0:
	mov r0, #1
	bl reset_network_vars
	mov sl, #1
	str sl, [sp]
	mov r4, #0
_020A75E4:
	mov r3, #0
	ldr r6, _020A77A8 ; =_021D46A0
	ldr r1, _020A77AC ; =0x000003BD
	mov r2, r3
_020A75F4:
	ldr r0, [r6]
	cmp r0, #0
	beq _020A7614
	ldrh r0, [r6, #0xa]
	sub r0, r5, r0
	mov r0, r0, lsl #0x10
	cmp r1, r0, asr #16
	strlt r2, [r6]
_020A7614:
	add r3, r3, #1
	cmp r3, #8
	add r6, r6, #0xc
	blt _020A75F4
	ldr r0, _020A7798 ; =_021D4620
	ldr r0, [r0, #0x2c]
	cmp r0, #0
	beq _020A7644
	subs sl, sl, #1
	bne _020A7644
	bl send_arprequest
	mov sl, #0x69
_020A7644:
	ldr r0, _020A77B0 ; =OSi_ThreadInfo
	ldr sb, [r0, #8]
	cmp sb, #0
	beq _020A770C
	mov r6, #0
	mov r7, r6
	mov r8, #1
_020A7660:
	ldr r0, [sb, #0xa4]
	cmp r0, #0
	ldrne r1, [r0]
	cmpne r1, #0
	beq _020A7700
	ldrb r1, [r0, #8]
	cmp r1, #3
	bne _020A76A8
	ldr r2, [r0, #0x10]
	sub r2, r5, r2
	cmp r2, #0x27
	ble _020A76A8
	strb r8, [r0, #8]
	ldrh r1, [r0, #0x1a]
	strh r1, [r0, #0x18]
	ldr r1, [r0, #0x20]
	str r1, [r0, #0x1c]
	b _020A7700
_020A76A8:
	cmp r1, #2
	bne _020A76E0
	ldr r2, [r0, #0x10]
	sub r2, r5, r2
	cmp r2, #0x27
	ble _020A76E0
	ldr r1, [r0, #4]
	cmp r1, #1
	bne _020A7700
	strb r7, [r0, #8]
	str r7, [r0, #4]
	ldr r0, [r0]
	bl OS_WakeupThreadDirect
	b _020A7700
_020A76E0:
	cmp r1, #4
	beq _020A7700
	ldr r1, [r0, #4]
	cmp r1, #2
	bne _020A7700
	str r6, [r0, #4]
	ldr r0, [r0]
	bl OS_WakeupThreadDirect
_020A7700:
	ldr sb, [sb, #0x68]
	cmp sb, #0
	bne _020A7660
_020A770C:
	mov sb, #0
	ldr r8, _020A77B4 ; =_021D4948
	ldr r6, _020A7798 ; =_021D4620
	mov r7, sb
_020A771C:
	ldrh r0, [r8, #4]
	cmp r0, #0
	beq _020A7748
	ldr r0, [r8, #0x2c]
	sub r0, r5, r0
	cmp r0, #0xef
	ble _020A7748
	ldr r0, [r8, #0x34]
	ldr r1, [r6, #0x40]
	blx r1
	strh r7, [r8, #4]
_020A7748:
	add sb, sb, #1
	cmp sb, #8
	add r8, r8, #0x38
	blt _020A771C
	mov r0, r5
	bl CPSi_SslPeriodical
	ldr r0, _020A7798 ; =_021D4620
	ldr r0, [r0, #0x3c]
	cmp r0, #0
	beq _020A74A4
	blx r0
	b _020A74A4
_020A7778:
	ldr r0, [r0, #0x10]
	tst r0, #1
	bne _020A7790
	cmp r4, #3
	beq _020A7790
	bl dhcp_release_server
_020A7790:
	bl CPS_SocUnRegister
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020A7798: .word _021D4620
_020A779C: .word _021D4700
_020A77A0: .word _021D4CCC
_020A77A4: .word _021D4B4C
_020A77A8: .word _021D46A0
_020A77AC: .word 0x000003BD
_020A77B0: .word OSi_ThreadInfo
_020A77B4: .word _021D4948
	arm_func_end scavenger
