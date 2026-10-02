	.extern E2scales
	.extern VCT_DeleteSession
	.extern _021D6F90
	.extern _021D6FB4
	.extern _021D6FCC
	.extern _021D6FE4
	.extern _021D6FEC
	.extern _021D7008
	.extern _021D848C
	.extern __vct
	.extern sDelayLine
	.extern sH
	.extern sImpulseResponse
	.extern sRecvState
	.extern sRequestEvent
	.extern sResponseEvent
	.extern sSendState
	.extern sSpkBuffer
	.extern sTap
	.extern sTransRecvState
	.extern sTransSendState
	.extern sVADInfo
	.extern vct_check_timeout
	.extern vct_create_session_impl
	.extern vct_decode_audio_buffer
	.extern vct_finish_audio
	.extern vct_finish_ssp
	.extern vct_flush_send_buffer
	.extern vct_handle_audio
	.extern vct_handle_ssp
	.extern vct_init_audio
	.extern vct_init_ssp
	.extern vct_response_impl
	.extern vct_send_notify
	.extern vct_send_notify_impl
	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_init_packet_buffer

.text
	arm_func_start vct_init_packet_buffer
vct_init_packet_buffer: ; 0x020B4014
	stmdb sp!, {r4, r5, r6, lr}
	ldr r3, _020B40D0 ; =0xE525982B
	mov r2, r1
	umull r1, r4, r2, r3
	ldr r1, _020B40D4 ; =_021D6FE4
	mov r4, r4, lsr #0xa
	str r0, [r1, #4]
	cmp r4, #4
	blo _020B4040
	cmp r4, #0x48
	bls _020B4048
_020B4040:
	mov r0, #0
	ldmia sp!, {r4, r5, r6, pc}
_020B4048:
	cmp r0, #0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	tst r0, #0x1f
	movne r0, #0
	strne r0, [r1, #4]
	ldmneia sp!, {r4, r5, r6, pc}
	mov r1, r0
	mov r0, #0
	bl MIi_CpuClearFast
	subs lr, r4, #1
	mov r5, #0
	beq _020B40A8
	ldr r2, _020B40D4 ; =_021D6FE4
	ldr r0, _020B40D8 ; =0x00000478
	mov r6, r5
_020B4088:
	ldr ip, [r2, #4]
	add r5, r5, #1
	mla r3, r5, r0, ip
	add r1, r6, #0x78
	str r3, [ip, r6]
	cmp r5, lr
	add r6, r1, #0x400
	blo _020B4088
_020B40A8:
	ldr r1, _020B40D4 ; =_021D6FE4
	ldr r0, _020B40D8 ; =0x00000478
	ldr r2, [r1, #4]
	mov r3, #0
	mla r0, r4, r0, r2
	str r3, [r0, #-0x478]
	ldr r2, [r1, #4]
	mov r0, #1
	str r2, [r1]
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020B40D0: .word 0xE525982B
_020B40D4: .word _021D6FE4
_020B40D8: .word 0x00000478
	arm_func_end vct_init_packet_buffer
