	.extern CPS_SslAddRandomSeed
	.extern CPSi_SocConsumeRaw
	.extern CPSi_TcpReadRaw
	.extern CPSi_sha1_calc
	.extern CPSi_sha1_init
	.extern CPSi_sha1_result_prng
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
	.extern client_hello
	.extern client_hello_v2
	.extern make_plaintext
	.extern rcv_certificate
	.extern rcv_client_key_exchange
	.extern rcv_finished
	.extern rcv_server_hello
	.extern tcp_read_raw_nbytes
	.extern update_digest
	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"

.text
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
