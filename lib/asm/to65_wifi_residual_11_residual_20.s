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
	.public dhcp_analyze_response
	.extern CPS_SocConsume
	.extern CPS_SocGetLength
	.extern CPS_SocRead
	.extern OS_YieldThread__
	.extern maccmp

.text
	arm_func_start dhcp_analyze_response
dhcp_analyze_response: ; 0x020A7B44
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	add r1, r1, #1
	str r0, [sp]
	rsb fp, r1, r1, lsl #4
	bl OS_GetTick
	mov r4, r0, lsr #0x10
	orr r4, r4, r1, lsl #16
	mov r5, #0
	b _020A7E4C
_020A7B6C:
	bl CPS_SocGetLength
	cmp r0, #0
	bne _020A7B80
	bl OS_YieldThread__
	b _020A7E4C
_020A7B80:
	add r0, sp, #4
	bl CPS_SocRead
	ldr r1, [sp, #4]
	mov r6, r0
	cmp r1, #0xf0
	bls _020A7E44
	ldrb r0, [r6]
	cmp r0, #2
	bne _020A7E44
	ldrh r3, [r6, #6]
	ldrh r2, [r6, #4]
	mov r0, r3, lsl #8
	mov r1, r2, lsl #8
	orr r2, r1, r2, asr #8
	orr r1, r0, r3, asr #8
	mov r0, r2, lsl #0x10
	mov r1, r1, lsl #0x10
	mov r2, r0, lsr #0x10
	mov r0, r1, lsr #0x10
	orr r1, r0, r2, lsl #16
	ldr r0, [sp]
	cmp r0, r1
	bne _020A7E44
	ldr r1, _020A7E8C ; =_021D4B08
	add r0, r6, #0x1c
	bl maccmp
	cmp r0, #0
	bne _020A7E44
	ldrb r3, [r6, #0x10]
	ldrb r2, [r6, #0x11]
	ldrb r1, [r6, #0x12]
	ldrb r0, [r6, #0x13]
	orr r3, r2, r3, lsl #8
	ldrb r2, [r6, #0xec]
	orr r1, r0, r1, lsl #8
	mov r0, r3, lsl #0x10
	mov r1, r1, lsl #0x10
	mov r5, r0, lsr #0x10
	mov r0, r1, lsr #0x10
	cmp r2, #0x63
	orr r2, r0, r5, lsl #16
	ldreqb r0, [r6, #0xed]
	ldr r3, [sp, #4]
	mov r5, #3
	cmpeq r0, #0x82
	ldreqb r0, [r6, #0xee]
	add r1, r6, r3
	cmpeq r0, #0x53
	ldreqb r0, [r6, #0xef]
	addeq r3, r6, #0xf0
	cmpeq r0, #0x63
	bne _020A7E44
	mov r0, #0
	mov sb, #2
	mov sl, #1
	ldr ip, _020A7E90 ; =_021D4620
	b _020A7E30
_020A7C64:
	cmp r6, #0
	beq _020A7E30
	cmp r6, #0x33
	bgt _020A7CA0
	bge _020A7D98
	cmp r6, #6
	bgt _020A7E24
	cmp r6, #1
	blt _020A7E24
	beq _020A7CBC
	cmp r6, #3
	beq _020A7CF0
	cmp r6, #6
	beq _020A7D24
	b _020A7E24
_020A7CA0:
	cmp r6, #0x35
	bgt _020A7CB0
	beq _020A7DCC
	b _020A7E24
_020A7CB0:
	cmp r6, #0x36
	beq _020A7DF4
	b _020A7E24
_020A7CBC:
	ldrb lr, [r3, #1]
	ldrb r8, [r3, #2]
	ldrb r7, [r3, #3]
	ldrb r6, [r3, #4]
	orr r8, r8, lr, lsl #8
	mov r8, r8, lsl #0x10
	orr r6, r6, r7, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r8, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x1c]
	b _020A7E24
_020A7CF0:
	ldrb lr, [r3, #1]
	ldrb r8, [r3, #2]
	ldrb r7, [r3, #3]
	ldrb r6, [r3, #4]
	orr r8, r8, lr, lsl #8
	mov r8, r8, lsl #0x10
	orr r6, r6, r7, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r8, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x2c]
	b _020A7E24
_020A7D24:
	ldrb r6, [r3]
	cmp r6, #8
	strlo r0, [ip, #0x64]
	blo _020A7D64
	ldrb lr, [r3, #5]
	ldrb r8, [r3, #6]
	ldrb r7, [r3, #7]
	ldrb r6, [r3, #8]
	orr r8, r8, lr, lsl #8
	mov r8, r8, lsl #0x10
	orr r6, r6, r7, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r8, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x64]
_020A7D64:
	ldrb lr, [r3, #1]
	ldrb r8, [r3, #2]
	ldrb r7, [r3, #3]
	ldrb r6, [r3, #4]
	orr r8, r8, lr, lsl #8
	mov r8, r8, lsl #0x10
	orr r6, r6, r7, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r8, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x60]
	b _020A7E24
_020A7D98:
	ldrb lr, [r3, #1]
	ldrb r8, [r3, #2]
	ldrb r7, [r3, #3]
	ldrb r6, [r3, #4]
	orr r8, r8, lr, lsl #8
	mov r8, r8, lsl #0x10
	orr r6, r6, r7, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r8, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x4c]
	b _020A7E24
_020A7DCC:
	ldrb r6, [r3, #1]
	cmp r6, #2
	beq _020A7DE8
	cmp r6, #5
	moveq r5, sb
	streq r2, [ip, #0x50]
	b _020A7E24
_020A7DE8:
	mov r5, sl
	str r2, [ip, #0x34]
	b _020A7E24
_020A7DF4:
	ldrb r8, [r3, #1]
	ldrb r7, [r3, #2]
	ldrb r6, [r3, #3]
	ldrb lr, [r3, #4]
	orr r7, r7, r8, lsl #8
	mov r7, r7, lsl #0x10
	orr r6, lr, r6, lsl #8
	mov r6, r6, lsl #0x10
	mov r7, r7, lsr #0x10
	mov r6, r6, lsr #0x10
	orr r6, r6, r7, lsl #16
	str r6, [ip, #0x20]
_020A7E24:
	ldrb r6, [r3]
	add r6, r6, #1
	add r3, r3, r6
_020A7E30:
	cmp r3, r1
	bhs _020A7E44
	ldrb r6, [r3], #1
	cmp r6, #0xff
	bne _020A7C64
_020A7E44:
	ldr r0, [sp, #4]
	bl CPS_SocConsume
_020A7E4C:
	ldr r0, _020A7E90 ; =_021D4620
	ldr r0, [r0, #0x48]
	blx r0
	cmp r0, #0
	beq _020A7E80
	cmp r5, #0
	bne _020A7E80
	bl OS_GetTick
	mov r0, r0, lsr #0x10
	orr r0, r0, r1, lsl #16
	sub r0, r0, r4
	cmp r0, fp
	blt _020A7B6C
_020A7E80:
	mov r0, r5
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020A7E8C: .word _021D4B08
_020A7E90: .word _021D4620
	arm_func_end dhcp_analyze_response
