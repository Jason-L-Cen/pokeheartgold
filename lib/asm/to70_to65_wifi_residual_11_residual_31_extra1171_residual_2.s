	.extern CPSi_TcpWrite2Raw
	.extern CPSi_big_from_char
	.extern CPSi_big_power
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
	.extern date2sec
	.extern enter_computebound
	.extern exit_computebound
	.extern find_session_from_IP
	.extern finished_md5
	.extern finished_sha1
	.extern make_ciphertext
	.extern set_random
	.extern update_digest
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
	arm_func_start send_client_key_exchange
send_client_key_exchange: ; 0x020AAF6C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	mov sl, r0
	ldr r4, [sl, #0xc]
	ldrb r0, [r4, #0x5ac]
	cmp r0, #0
	beq _020AAFA0
	mov r2, #0
	ldr r0, _020AB220 ; =_02108FB8
	mov r3, r2
	mov r1, #7
	str sl, [sp]
	bl CPSi_TcpWrite2Raw
_020AAFA0:
	mov r0, #3
	strb r0, [r4]
	mov r0, #0
	strb r0, [r4, #1]
	add r0, r4, #2
	mov r1, #0x2e
	bl set_random
	ldr sb, [r4, #0x594]
	ldr r0, _020AB224 ; =_021D4634
	mov r1, sb, lsl #1
	ldr r2, [r0]
	add r1, r1, r1, lsr #31
	mov r0, sb
	mov r7, r1, asr #1
	blx r2
	movs r8, r0
	moveq r0, #9
	addeq sp, sp, #8
	streqb r0, [r4, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, #0
	strb r0, [r8]
	mov r2, #2
	add r0, r8, #2
	sub r1, sb, #0x33
	strb r2, [r8, #1]
	bl set_random
	add r1, r8, sb
	mov r0, r4
	sub r3, sb, #0x31
	mov r5, #0
	sub r1, r1, #0x30
	mov r2, #0x30
	strb r5, [r8, r3]
	bl MI_CpuCopy8
	ldr r1, _020AB224 ; =_021D4634
	mov r0, r7, lsl #3
	ldr r1, [r1]
	blx r1
	movs r5, r0
	bne _020AB064
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	mov r0, #9
	add sp, sp, #8
	strb r0, [r4, #0x455]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020AB064:
	add r0, r5, r7, lsl #1
	add fp, r0, r7, lsl #1
	mov r1, r8
	mov r2, sb
	mov r3, r7
	str r0, [sp, #4]
	add r6, fp, r7, lsl #1
	bl CPSi_big_from_char
	add r0, r4, #0x198
	add r1, r0, #0x400
	ldr r2, [r4, #0x5a0]
	mov r0, fp
	mov r3, r7
	bl CPSi_big_from_char
	add r1, r4, #0x94
	mov r0, r6
	add r1, r1, #0x400
	mov r2, sb
	mov r3, r7
	bl CPSi_big_from_char
	bl enter_computebound
	str r6, [sp]
	mov r6, r0
	ldr r1, [sp, #4]
	mov r2, fp
	mov r3, r7
	mov r0, r5
	bl CPSi_big_power
	mov r0, r6
	bl exit_computebound
	ldr r1, _020AB224 ; =_021D4634
	add r0, sb, #0x49
	ldr r1, [r1]
	blx r1
	movs r6, r0
	bne _020AB124
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r5
	ldr r1, [r1]
	blx r1
	mov r0, #9
	add sp, sp, #8
	strb r0, [r4, #0x455]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020AB124:
	mov r0, #0x16
	strb r0, [r6]
	mov r1, #3
	add r0, sb, #4
	strb r1, [r6, #1]
	mov r1, #0
	strb r1, [r6, #2]
	mov r1, r0, asr #8
	strb r1, [r6, #3]
	strb r0, [r6, #4]
	mov r0, #0x10
	strb r0, [r6, #5]
	mov r0, sb, asr #0x10
	strb r0, [r6, #6]
	mov r0, sb, asr #8
	strb r0, [r6, #7]
	strb sb, [r6, #8]
	tst sb, #1
	add r0, r6, #9
	beq _020AB18C
	add r0, sb, sb, lsr #31
	mov r0, r0, asr #1
	mov r0, r0, lsl #1
	ldrh r1, [r5, r0]
	add r0, r6, #0xa
	strb r1, [r6, #9]
_020AB18C:
	add r1, sb, sb, lsr #31
	mov r1, r1, asr #1
	subs r3, r1, #1
	bmi _020AB1C0
_020AB19C:
	mov r2, r3, lsl #1
	ldrh r1, [r5, r2]
	subs r3, r3, #1
	mov r1, r1, asr #8
	strb r1, [r0]
	ldrh r1, [r5, r2]
	strb r1, [r0, #1]
	add r0, r0, #2
	bpl _020AB19C
_020AB1C0:
	mov r2, #0
	mov r0, r6
	mov r3, r2
	add r1, sb, #9
	str sl, [sp]
	bl CPSi_TcpWrite2Raw
	mov r0, r4
	add r1, r6, #5
	add r2, sb, #4
	bl update_digest
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r6
	ldr r1, [r1]
	blx r1
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r5
	ldr r1, [r1]
	blx r1
	ldr r1, _020AB228 ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020AB220: .word _02108FB8
_020AB224: .word _021D4634
_020AB228: .word _021D4660
	arm_func_end send_client_key_exchange
