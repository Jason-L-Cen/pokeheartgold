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

.text
	arm_func_start vct_check_send_state
vct_check_send_state: ; 0x020B3AA8
	ldrb r2, [r1, #4]
	ldrb ip, [r1, #6]
	cmp r2, #0
	addeq ip, ip, #5
	cmp ip, #0
	blt _020B3AC8
	cmp ip, #0xb
	blt _020B3ACC
_020B3AC8:
	mvn ip, #0
_020B3ACC:
	cmp ip, #0
	mvnlt r0, #0
	bxlt lr
	ldr r1, _020B3B24 ; =__vct
	ldr r3, [r0, #0xc]
	ldr r1, [r1, #0x10]
	cmp r1, #1
	ldreq r2, _020B3B28 ; =sSendState
	mov r1, #0xb
	ldrne r2, _020B3B2C ; =sTransSendState
	mla r1, r3, r1, r2
	ldrsb r2, [ip, r1]
	mvn r1, #0
	cmp r2, r1
	moveq r0, #1
	bxeq lr
	sub r1, r1, #1
	cmp r2, r1
	moveq r0, #0
	strne r2, [r0, #0xc]
	movne r0, #1
	bx lr
	.align 2, 0
_020B3B24: .word __vct
_020B3B28: .word sSendState
_020B3B2C: .word sTransSendState
	arm_func_end vct_check_send_state
