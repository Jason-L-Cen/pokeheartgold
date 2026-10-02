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
	arm_func_start HPFFilter
HPFFilter: ; 0x020B43D0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldrsh r2, [r0]
	ldr r3, _020B44A8 ; =_021D7008
	ldr r5, _020B44AC ; =0x00000E9F
	ldr r4, [r3, #4]
	smulbb r7, r2, r5
	ldr r2, [r3, #0x10]
	ldr r6, _020B44B0 ; =0x00000D3E
	add r4, r7, r4
	mla r4, r2, r6, r4
	mov r4, r4, asr #0xc
	mul lr, r4, r4
	mov r1, r1, lsr #1
	str r4, [r3, #0x10]
	rsb r2, r7, #0
	str r2, [r3, #4]
	cmp r1, #1
	mov ip, lr, asr #0x1f
	mov r3, #1
	bls _020B4468
_020B4420:
	mov r7, r3, lsl #1
	ldrsh r8, [r0, r7]
	add r7, r0, r3, lsl #1
	add r3, r3, #1
	smulbb r8, r8, r5
	add r2, r8, r2
	mla r2, r4, r6, r2
	strh r4, [r7, #-2]
	mov r4, r2, asr #0xc
	mul r2, r4, r4
	adds lr, lr, r2
	adc ip, ip, r2, asr #31
	rsb r2, r8, #0
	cmp r3, r1
	blo _020B4420
	ldr r3, _020B44A8 ; =_021D7008
	str r2, [r3, #4]
	str r4, [r3, #0x10]
_020B4468:
	add r0, r0, r1, lsl #1
	strh r4, [r0, #-2]
	ldr r2, _020B44B4 ; =0x04000280
	mov r0, #1
	strh r0, [r2]
	str lr, [r2, #0x10]
	str ip, [r2, #0x14]
	str r1, [r2, #0x18]
	mov r0, #0
	str r0, [r2, #0x1c]
_020B4490:
	ldrh r0, [r2]
	tst r0, #0x8000
	bne _020B4490
	ldr r0, _020B44B8 ; =0x040002A0
	ldr r0, [r0]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020B44A8: .word _021D7008
_020B44AC: .word 0x00000E9F
_020B44B0: .word 0x00000D3E
_020B44B4: .word 0x04000280
_020B44B8: .word 0x040002A0
	arm_func_end HPFFilter

	arm_func_start InitFIRFilter
InitFIRFilter: ; 0x020B44BC
	stmdb sp!, {r3, lr}
	ldr r0, _020B4520 ; =_021D7008
	mov r2, #0
	ldr r1, _020B4524 ; =0x0000019D
	str r2, [r0, #0xc]
	str r1, [r0, #8]
	ldr ip, _020B4528 ; =sImpulseResponse
	ldr r0, _020B452C ; =sDelayLine
	mov lr, r2
_020B44E0:
	mov r1, r2, lsl #1
	add r2, r2, #1
	strh lr, [r0, r1]
	cmp r2, #0x800
	blt _020B44E0
	ldr r2, _020B4530 ; =sTap
	ldr r0, _020B4534 ; =sH
	mov r3, #0
_020B4500:
	mov r1, lr, lsl #1
	ldrsh r1, [ip, r1]
	str r3, [r2, lr, lsl #2]
	str r1, [r0, lr, lsl #2]
	add lr, lr, #1
	cmp lr, #6
	blt _020B4500
	ldmia sp!, {r3, pc}
	.align 2, 0
_020B4520: .word _021D7008
_020B4524: .word 0x0000019D
_020B4528: .word sImpulseResponse
_020B452C: .word sDelayLine
_020B4530: .word sTap
_020B4534: .word sH
	arm_func_end InitFIRFilter
