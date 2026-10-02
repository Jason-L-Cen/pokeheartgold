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
	.public send_change_cipher_spec_and_finished
	.public send_client_hello
	.public send_client_key_exchange
	.public send_server_hello
	.extern CPSi_TcpWrite2Raw
	.extern CPSi_big_from_char
	.extern CPSi_big_power
	.extern date2sec
	.extern enter_computebound
	.extern exit_computebound
	.extern find_session_from_IP
	.extern finished_md5
	.extern finished_sha1
	.extern make_ciphertext
	.extern set_random
	.extern update_digest

.text
	arm_func_start send_server_hello
send_server_hello: ; 0x020AAA20
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov sb, r0
	ldr r4, [sb, #0xc]
	ldr r7, [r4, #0x820]
	cmp r7, #0
	ldrne r8, [r7]
	moveq r8, #0
	bl date2sec
	mov r1, r0, lsr #0x18
	strb r1, [r4, #0x54]
	mov r1, r0, lsr #0x10
	strb r1, [r4, #0x55]
	mov r1, r0, lsr #8
	strb r1, [r4, #0x56]
	strb r0, [r4, #0x57]
	add r0, r4, #0x58
	mov r1, #0x1c
	bl set_random
	ldr r1, _020AAC74 ; =_021D4634
	add r0, r8, #0x9d
	ldr r1, [r1]
	blx r1
	movs r5, r0
	bne _020AAA90
	mov r0, #9
	strb r0, [r4, #0x455]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020AAA90:
	mov r0, #2
	strb r0, [r5, #5]
	mov r3, #0
	strb r3, [r5, #6]
	strb r3, [r5, #7]
	mov r0, #0x46
	strb r0, [r5, #8]
	mov r0, #3
	strb r0, [r5, #9]
	add r0, r4, #0x54
	add r1, r5, #0xb
	mov r2, #0x20
	strb r3, [r5, #0xa]
	bl MI_CpuCopy8
	mov r2, #0x20
	strb r2, [r5, #0x2b]
	ldrb r0, [r4, #0x30]
	cmp r0, #0
	beq _020AAAF8
	add r0, r4, #0x74
	add r1, r5, #0x2c
	bl MI_CpuCopy8
	mov r0, #1
	strb r0, [r4, #0x31]
	add r6, r5, #0x4c
	b _020AAB54
_020AAAF8:
	add r0, r5, #0x2c
	mov r1, #0x1c
	bl set_random
	ldr r0, _020AAC78 ; =_021D5E60
	add r6, r5, #0x4c
	ldr r3, [r0, #4]
	sub r0, r6, #0x20
	mov r1, r3, lsr #0x18
	strb r1, [r5, #0x48]
	mov r1, r3, lsr #0x10
	strb r1, [r5, #0x49]
	mov r1, r3, lsr #8
	strb r1, [r5, #0x4a]
	add r1, r4, #0x74
	mov r2, #0x20
	strb r3, [r5, #0x4b]
	bl MI_CpuCopy8
	ldr r0, _020AAC78 ; =_021D5E60
	mov r1, #0
	ldr r2, [r0, #4]
	strb r1, [r4, #0x31]
	add r1, r2, #1
	str r1, [r0, #4]
_020AAB54:
	ldrh r1, [r4, #0x32]
	mov r0, #0
	mov r1, r1, asr #8
	strb r1, [r6]
	ldrh r1, [r4, #0x32]
	strb r1, [r6, #1]
	strb r0, [r6, #2]
	ldrb r0, [r4, #0x31]
	add r6, r6, #3
	cmp r0, #0
	bne _020AAC08
	cmp r8, #0
	beq _020AABEC
	add r2, r8, #6
	mov r0, #0xb
	strb r0, [r6]
	add r1, r8, #3
	mov r0, r2, asr #0x10
	strb r0, [r6, #1]
	mov r0, r2, asr #8
	strb r0, [r6, #2]
	strb r2, [r6, #3]
	mov r0, r1, asr #0x10
	strb r0, [r6, #4]
	mov r0, r1, asr #8
	strb r0, [r6, #5]
	strb r1, [r6, #6]
	mov r0, r8, asr #0x10
	strb r0, [r6, #7]
	mov r0, r8, asr #8
	strb r0, [r6, #8]
	strb r8, [r6, #9]
	add r6, r6, #0xa
	ldr r0, [r7, #4]
	mov r1, r6
	mov r2, r8
	bl MI_CpuCopy8
	add r6, r6, r8
_020AABEC:
	mov r0, #0xe
	strb r0, [r6]
	mov r0, #0
	strb r0, [r6, #1]
	strb r0, [r6, #2]
	strb r0, [r6, #3]
	add r6, r6, #4
_020AAC08:
	mov r0, #0x16
	sub r1, r6, r5
	sub r6, r1, #5
	strb r0, [r5]
	mov r0, #3
	strb r0, [r5, #1]
	mov r0, #0
	strb r0, [r5, #2]
	mov r0, r6, asr #8
	strb r0, [r5, #3]
	mov r0, r4
	mov r2, r6
	add r1, r5, #5
	strb r6, [r5, #4]
	bl update_digest
	mov r2, #0
	mov r0, r5
	mov r3, r2
	add r1, r6, #5
	str sb, [sp]
	bl CPSi_TcpWrite2Raw
	ldr r1, _020AAC7C ; =_021D4660
	mov r0, r5
	ldr r1, [r1]
	blx r1
	ldrb r0, [r4, #0x31]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020AAC74: .word _021D4634
_020AAC78: .word _021D5E60
_020AAC7C: .word _021D4660
	arm_func_end send_server_hello

	arm_func_start send_change_cipher_spec_and_finished
send_change_cipher_spec_and_finished: ; 0x020AAC80
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	ldr r1, _020AADD0 ; =_021D4634
	mov r6, r0
	ldr r1, [r1]
	mov r0, #0x83
	ldr r5, [r6, #0xc]
	blx r1
	movs r4, r0
	moveq r0, #9
	addeq sp, sp, #4
	streqb r0, [r5, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r0, #0x14
	strb r0, [r4]
	mov r0, #3
	strb r0, [r4, #1]
	mov r1, #0
	strb r1, [r4, #2]
	strb r1, [r4, #3]
	mov r3, #1
	strb r3, [r4, #4]
	add r0, r5, #0x1cc
	mov r2, #8
	strb r3, [r4, #5]
	bl MI_CpuFill8
	mov r0, #0x16
	strb r0, [r4, #6]
	mov r0, #3
	strb r0, [r4, #7]
	mov r1, #0
	strb r1, [r4, #8]
	strb r1, [r4, #9]
	mov r0, #0x28
	strb r0, [r4, #0xa]
	mov r0, #0x14
	strb r0, [r4, #0xb]
	strb r1, [r4, #0xc]
	strb r1, [r4, #0xd]
	mov r3, #0x24
	add r0, r5, #0x3a4
	add r1, r5, #0x3fc
	mov r2, #0x58
	strb r3, [r4, #0xe]
	bl MI_CpuCopy8
	mov r0, r5
	add r1, r4, #0xf
	mov r2, #0
	bl finished_md5
	add r0, r5, #0x3fc
	add r1, r5, #0x3a4
	mov r2, #0x58
	bl MI_CpuCopy8
	add r0, r5, #0x2ec
	add r1, r5, #0x348
	mov r2, #0x5c
	bl MI_CpuCopy8
	mov r0, r5
	add r1, r4, #0x1f
	mov r2, #0
	bl finished_sha1
	add r0, r5, #0x348
	add r1, r5, #0x2ec
	mov r2, #0x5c
	bl MI_CpuCopy8
	mov r0, r5
	add r1, r4, #0xb
	mov r2, #0x28
	bl update_digest
	mov r0, r5
	add r1, r4, #6
	bl make_ciphertext
	mov r2, #0
	add r1, r0, #6
	mov r0, r4
	mov r3, r2
	str r6, [sp]
	bl CPSi_TcpWrite2Raw
	ldr r1, _020AADD4 ; =_021D4660
	mov r0, r4
	ldr r1, [r1]
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020AADD0: .word _021D4634
_020AADD4: .word _021D4660
	arm_func_end send_change_cipher_spec_and_finished

	arm_func_start send_client_hello
send_client_hello: ; 0x020AADD8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r1, _020AAF60 ; =_021D4634
	mov r7, r0
	ldr r1, [r1]
	mov r0, #0x98
	ldr r4, [r7, #0xc]
	blx r1
	movs r5, r0
	moveq r0, #9
	streqb r0, [r4, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #3
	strb r0, [r5, #9]
	mov r0, #0
	strb r0, [r5, #0xa]
	bl date2sec
	mov r1, r0, lsr #0x18
	strb r1, [r4, #0x34]
	mov r1, r0, lsr #0x10
	strb r1, [r4, #0x35]
	mov r1, r0, lsr #8
	strb r1, [r4, #0x36]
	strb r0, [r4, #0x37]
	add r0, r4, #0x38
	mov r1, #0x1c
	bl set_random
	add r0, r4, #0x34
	add r1, r5, #0xb
	mov r2, #0x20
	bl MI_CpuCopy8
	ldrh r2, [r7, #0x18]
	ldr r1, [r7, #0x1c]
	mov r0, r4
	bl find_session_from_IP
	ldrb r0, [r4, #0x30]
	cmp r0, #0
	moveq r0, #0
	streqb r0, [r5, #0x2b]
	addeq r0, r5, #0x2c
	beq _020AAE90
	mov r2, #0x20
	add r0, r4, #0x74
	add r1, r5, #0x2c
	strb r2, [r5, #0x2b]
	bl MI_CpuCopy8
	add r0, r5, #0x4c
_020AAE90:
	mov r6, #0
	strb r6, [r0]
	mov r1, #4
	strb r1, [r0, #1]
	ldr r2, _020AAF64 ; =_021106B0
	add r0, r0, #2
_020AAEA8:
	mov r3, r6, lsl #1
	ldrh r1, [r2, r3]
	add r6, r6, #1
	cmp r6, #2
	mov r1, r1, asr #8
	strb r1, [r0]
	ldrh r1, [r2, r3]
	strb r1, [r0, #1]
	add r0, r0, #2
	blo _020AAEA8
	mov r3, #1
	mov r2, #0
	strb r3, [r0]
	add r1, r0, #2
	sub r1, r1, r5
	sub r6, r1, #5
	strb r2, [r0, #1]
	sub r1, r6, #4
	mov r0, #0x16
	strb r0, [r5]
	mov r0, #3
	strb r0, [r5, #1]
	strb r2, [r5, #2]
	mov r0, r6, asr #8
	strb r0, [r5, #3]
	strb r6, [r5, #4]
	strb r3, [r5, #5]
	mov r0, r1, asr #0x10
	strb r0, [r5, #6]
	mov r0, r1, asr #8
	strb r0, [r5, #7]
	strb r1, [r5, #8]
	mov r0, r5
	mov r3, r2
	add r1, r6, #5
	str r7, [sp]
	bl CPSi_TcpWrite2Raw
	mov r0, r4
	mov r2, r6
	add r1, r5, #5
	bl update_digest
	ldr r1, _020AAF68 ; =_021D4660
	mov r0, r5
	ldr r1, [r1]
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020AAF60: .word _021D4634
_020AAF64: .word _021106B0
_020AAF68: .word _021D4660
	arm_func_end send_client_hello

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
