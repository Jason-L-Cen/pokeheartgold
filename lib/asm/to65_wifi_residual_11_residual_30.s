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
	.public parse_record
	.public parse_record_in_buf
	.public set_random
	.extern CPS_SslAddRandomSeed
	.extern CPSi_SocConsumeRaw
	.extern CPSi_TcpReadRaw
	.extern CPSi_sha1_calc
	.extern CPSi_sha1_init
	.extern CPSi_sha1_result_prng
	.extern client_hello
	.extern client_hello_v2
	.extern make_plaintext
	.extern rcv_certificate
	.extern rcv_client_key_exchange
	.extern rcv_finished
	.extern rcv_server_hello
	.extern tcp_read_raw_nbytes
	.extern update_digest

.text
	arm_func_start parse_record_in_buf
parse_record_in_buf: ; 0x020AA460
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r0
	ldrb r2, [sb, #0x455]
	mov r8, r1
	cmp r2, #9
	bne _020AA48C
	ldr r1, _020AA6DC ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020AA48C:
	ldrb r3, [r8, #3]
	ldrb r1, [r8, #4]
	add r0, r2, #0xf9
	and r0, r0, #0xff
	add r1, r1, r3, lsl #8
	cmp r0, #1
	add r6, r1, #5
	ldrb r4, [r8]
	bhi _020AA4B8
	cmp r4, #0x15
	bne _020AA4C8
_020AA4B8:
	cmp r4, #0x15
	bne _020AA4F8
	cmp r6, #7
	bls _020AA4F8
_020AA4C8:
	mov r0, sb
	mov r1, r8
	bl make_plaintext
	ldrb r2, [sb, #0x455]
	mov r6, r0
	cmp r2, #9
	bne _020AA4F8
	ldr r1, _020AA6DC ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020AA4F8:
	sub r0, r4, #0x14
	cmp r0, #3
	add r5, r8, #5
	sub r6, r6, #5
	addls pc, pc, r0, lsl #2
	b _020AA6C0
_020AA510: ; jump table
	b _020AA520 ; case 0
	b _020AA550 ; case 1
	b _020AA564 ; case 2
	b _020AA6A0 ; case 3
_020AA520:
	ldr r0, [sb, #0x1d4]
	cmp r0, #0
	moveq r0, #9
	streqb r0, [sb, #0x455]
	beq _020AA6C8
	add r0, sb, #0x2e4
	mov r1, #0
	mov r2, #8
	bl MI_CpuFill8
	mov r0, #7
	strb r0, [sb, #0x455]
	b _020AA6C8
_020AA550:
	ldrb r0, [r5]
	cmp r0, #2
	moveq r0, #9
	streqb r0, [sb, #0x455]
	b _020AA6C8
_020AA564:
	mov fp, #4
	mov sl, #1
	mov r4, #0
_020AA570:
	ldrb r7, [r5, #2]
	ldrb r0, [r5, #1]
	ldrb r3, [r5]
	ldrb r1, [r5, #3]
	mov r7, r7, lsl #8
	add r0, r7, r0, lsl #16
	cmp r3, #0xb
	add r7, r1, r0
	add r5, r5, #4
	bgt _020AA5C0
	cmp r3, #0xb
	bge _020AA634
	cmp r3, #2
	bgt _020AA668
	cmp r3, #1
	blt _020AA668
	beq _020AA5F0
	cmp r3, #2
	beq _020AA624
	b _020AA668
_020AA5C0:
	sub r0, r3, #0xd
	cmp r0, #7
	addls pc, pc, r0, lsl #2
	b _020AA668
_020AA5D0: ; jump table
	b _020AA648 ; case 0
	b _020AA650 ; case 1
	b _020AA668 ; case 2
	b _020AA614 ; case 3
	b _020AA668 ; case 4
	b _020AA668 ; case 5
	b _020AA668 ; case 6
	b _020AA658 ; case 7
_020AA5F0:
	ldrb r0, [sb, #0x454]
	cmp r0, #0
	beq _020AA670
	cmp r2, #0
	bne _020AA670
	mov r0, sb
	mov r1, r5
	bl client_hello
	b _020AA670
_020AA614:
	mov r0, sb
	mov r1, r5
	bl rcv_client_key_exchange
	b _020AA670
_020AA624:
	mov r0, sb
	mov r1, r5
	bl rcv_server_hello
	b _020AA670
_020AA634:
	mov r0, sb
	mov r1, r5
	bl rcv_certificate
	strb r4, [sb, #0x5ac]
	b _020AA670
_020AA648:
	strb sl, [sb, #0x5ac]
	b _020AA670
_020AA650:
	strb fp, [sb, #0x455]
	b _020AA670
_020AA658:
	mov r0, sb
	mov r1, r5
	bl rcv_finished
	b _020AA670
_020AA668:
	mov r0, #9
	strb r0, [sb, #0x455]
_020AA670:
	mov r0, sb
	sub r1, r5, #4
	add r2, r7, #4
	bl update_digest
	add r0, r7, #4
	add r5, r5, r7
	subs r6, r6, r0
	beq _020AA6C8
	ldrb r2, [sb, #0x455]
	cmp r2, #9
	bne _020AA570
	b _020AA6C8
_020AA6A0:
	str r8, [sb, #0x824]
	mov r0, #5
	str r0, [sb, #0x82c]
	add r0, r6, #5
	str r0, [sb, #0x828]
	mov r0, #1
	strb r0, [sb, #0x456]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020AA6C0:
	mov r0, #9
	strb r0, [sb, #0x455]
_020AA6C8:
	ldr r1, _020AA6DC ; =_021D4660
	mov r0, r8
	ldr r1, [r1]
	blx r1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020AA6DC: .word _021D4660
	arm_func_end parse_record_in_buf

	arm_func_start parse_record
parse_record: ; 0x020AA6E0
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r0
	ldr r4, [r5, #0xc]
	add r6, sp, #0
_020AA6F4:
	mov r0, r6
	mov r1, r5
	bl CPSi_TcpReadRaw
	ldr r1, [sp]
	cmp r1, #0
	moveq r0, #9
	addeq sp, sp, #4
	streqb r0, [r4, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	cmp r1, #5
	blo _020AA6F4
	ldrb r1, [r0]
	cmp r1, #0x80
	bne _020AA7DC
	ldrb r1, [r4, #0x454]
	cmp r1, #0
	beq _020AA7D0
	ldrb r1, [r4, #0x455]
	cmp r1, #0
	bne _020AA7D0
	ldrb r2, [r0, #1]
	mov r1, r5
	mov r0, #2
	str r2, [sp]
	bl CPSi_SocConsumeRaw
	ldr r1, _020AA874 ; =_021D4634
	ldr r0, [sp]
	ldr r1, [r1]
	blx r1
	movs r6, r0
	moveq r0, #9
	addeq sp, sp, #4
	streqb r0, [r4, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r1, [sp]
	mov r2, r5
	bl tcp_read_raw_nbytes
	cmp r0, #0
	ldreqb r0, [r6]
	cmpeq r0, #1
	movne r0, #9
	strneb r0, [r4, #0x455]
	bne _020AA7AC
	mov r0, r4
	add r1, r6, #1
	bl client_hello_v2
_020AA7AC:
	ldr r2, [sp]
	mov r0, r4
	mov r1, r6
	bl update_digest
	ldr r1, _020AA878 ; =_021D4660
	mov r0, r6
	ldr r1, [r1]
	blx r1
	b _020AA868
_020AA7D0:
	mov r0, #9
	strb r0, [r4, #0x455]
	b _020AA868
_020AA7DC:
	ldrb r2, [r0, #3]
	ldrb r0, [r0, #4]
	ldr r1, _020AA87C ; =0x00004805
	add r0, r0, r2, lsl #8
	add r0, r0, #5
	str r0, [sp]
	cmp r0, r1
	movhi r0, #9
	addhi sp, sp, #4
	strhib r0, [r4, #0x455]
	ldmhiia sp!, {r3, r4, r5, r6, pc}
	ldr r1, _020AA874 ; =_021D4634
	ldr r1, [r1]
	blx r1
	movs r6, r0
	moveq r0, #9
	addeq sp, sp, #4
	streqb r0, [r4, #0x455]
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r1, [sp]
	mov r2, r5
	bl tcp_read_raw_nbytes
	cmp r0, #0
	beq _020AA85C
	ldr r1, _020AA878 ; =_021D4660
	mov r0, r6
	ldr r1, [r1]
	blx r1
	mov r0, #9
	add sp, sp, #4
	strb r0, [r4, #0x455]
	ldmia sp!, {r3, r4, r5, r6, pc}
_020AA85C:
	mov r0, r4
	mov r1, r6
	bl parse_record_in_buf
_020AA868:
	ldrb r0, [r4, #0x455]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020AA874: .word _021D4634
_020AA878: .word _021D4660
_020AA87C: .word 0x00004805
	arm_func_end parse_record

	arm_func_start set_random
set_random: ; 0x020AA880
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x74
	ldr r2, _020AA9A0 ; =_021D5E60
	mov sl, r0
	ldrb r0, [r2]
	mov sb, r1
	cmp r0, #0
	bne _020AA8E4
	ldr r2, _020AA9A4 ; =_021D4688
	add r0, sp, #0
	ldr r3, [r2]
	ldmib r2, {r1, r4}
	umull r6, r5, r4, r3
	mla r5, r4, r1, r5
	ldr r1, [r2, #0xc]
	ldr r4, [r2, #0x10]
	mla r5, r1, r3, r5
	ldr r1, [r2, #0x14]
	adds r4, r4, r6
	adc r3, r1, r5
	str r4, [r2]
	mov r1, #4
	str r3, [r2, #4]
	str r3, [sp]
	bl CPS_SslAddRandomSeed
_020AA8E4:
	cmp sb, #0
	mov r8, #0
	addle sp, sp, #0x74
	mov r1, #0x14
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, _020AA9A8 ; =_021D5FD8
	add r6, sp, #0x18
	mov fp, r1
	add r4, sp, #4
_020AA908:
	cmp r1, #0x14
	bne _020AA97C
	mov r0, r6
	bl CPSi_sha1_init
	bl OS_DisableInterrupts
	mov r7, r0
	mov r0, r6
	mov r1, r5
	mov r2, fp
	bl CPSi_sha1_calc
	mov r0, r6
	mov r1, r4
	bl CPSi_sha1_result_prng
	ldr lr, _020AA9AC ; =_021D5FEB
	mov r3, #1
	mov r2, #0x13
	add ip, sp, #0x17
_020AA94C:
	ldrb r1, [lr]
	ldrb r0, [ip], #-1
	subs r2, r2, #1
	add r0, r1, r0
	add r0, r3, r0
	strb r0, [lr], #-1
	mov r3, r0, lsr #8
	bpl _020AA94C
	str r0, [sp]
	mov r0, r7
	bl OS_RestoreInterrupts
	mov r1, #0
_020AA97C:
	ldrb r0, [r4, r1]
	add r1, r1, #1
	cmp r0, #0
	strneb r0, [sl, r8]
	addne r8, r8, #1
	cmp r8, sb
	blt _020AA908
	add sp, sp, #0x74
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020AA9A0: .word _021D5E60
_020AA9A4: .word _021D4688
_020AA9A8: .word _021D5FD8
_020AA9AC: .word _021D5FEB
	arm_func_end set_random
