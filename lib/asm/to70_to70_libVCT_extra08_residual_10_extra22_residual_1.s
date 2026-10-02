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
	.public vct_check_recv_state

.text
	arm_func_start vct_build_ssp_header
vct_build_ssp_header: ; 0x020B393C
	ldr ip, _020B397C ; =0x5F564354
	mov r3, #0x10
	str ip, [r1]
	strb r3, [r1, #5]
	ldr r3, _020B3980 ; =__vct
	strb r2, [r1, #6]
	ldrb r2, [r3]
	strb r2, [r1, #8]
	ldrb r2, [r0, #4]
	mov r0, #0
	strb r2, [r1, #9]
	strb r0, [r1, #0xa]
	ldrb r2, [r3]
	strb r2, [r1, #0xb]
	str r0, [r1, #0xc]
	bx lr
	.align 2, 0
_020B397C: .word 0x5F564354
_020B3980: .word __vct
	arm_func_end vct_build_ssp_header

	arm_func_start vct_check_recv_state
vct_check_recv_state: ; 0x020B3984
	stmdb sp!, {r3, r4, r5, lr}
	ldrb r2, [r1, #4]
	ldrb r3, [r1, #6]
	mov r4, r0
	cmp r2, #0
	addeq r3, r3, #5
	cmp r3, #0
	blt _020B39AC
	cmp r3, #0xb
	blt _020B39B0
_020B39AC:
	mvn r3, #0
_020B39B0:
	cmp r3, #0
	mvnlt r0, #0
	ldmltia sp!, {r3, r4, r5, pc}
	ldr r0, _020B3A50 ; =__vct
	ldr r2, [r4, #0xc]
	ldr r0, [r0, #0x10]
	cmp r0, #1
	ldreq r1, _020B3A54 ; =sRecvState
	mov r0, #0xb
	ldrne r1, _020B3A58 ; =sTransRecvState
	mla r0, r2, r0, r1
	ldrsb r5, [r3, r0]
	mvn r1, #2
	cmp r5, r1
	beq _020B3A2C
	add r0, r1, #1
	cmp r5, r0
	beq _020B3A0C
	add r0, r1, #2
	cmp r5, r0
	mov r0, r5
	bne _020B3A4C
	ldmia sp!, {r3, r4, r5, pc}
_020B3A0C:
	mov r0, r4
	mov r1, #1
	bl vct_response_impl
	mov r1, #0
	str r1, [r4, #0xc]
	mov r0, r5
	str r1, [r4]
	ldmia sp!, {r3, r4, r5, pc}
_020B3A2C:
	mov r0, r4
	mov r1, #3
	bl vct_response_impl
	mov r1, #0
	str r1, [r4, #0xc]
	mov r0, r5
	str r1, [r4]
	ldmia sp!, {r3, r4, r5, pc}
_020B3A4C:
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020B3A50: .word __vct
_020B3A54: .word sRecvState
_020B3A58: .word sTransRecvState
	arm_func_end vct_check_recv_state
