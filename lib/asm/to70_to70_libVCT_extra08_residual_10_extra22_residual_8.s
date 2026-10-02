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
	.public VCTi_HandleData

.text
	arm_func_start VCTi_HandleData
VCTi_HandleData: ; 0x020B4A8C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	movs r6, r1
	mov r7, r0
	mov r5, r2
	mov r4, r3
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, [r6]
	ldr r0, _020B4B60 ; =0x5F564354
	cmp r1, r0
	movne r0, #2
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _020B4B64 ; =_021D848C
	ldr r1, [r0, #0x1c]
	cmp r1, #0
	ldrne r0, [r0, #8]
	cmpne r0, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, #0
	str r0, [r4]
	str r0, [r4, #4]
	bl OS_GetTick
	ldrb r2, [r6, #4]
	and r3, r2, #0xf0
	cmp r3, #0x40
	bne _020B4B28
	ldr r2, _020B4B64 ; =_021D848C
	mov r3, r0
	ldr r4, [r2, #4]
	mov r0, r7
	add r4, r4, #1
	str r4, [r2, #4]
	str r1, [sp]
	mov r1, r6
	mov r2, r5
	bl vct_handle_audio
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B4B28:
	cmp r2, #0
	cmpne r2, #0xff
	bne _020B4B58
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	bl vct_handle_ssp
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B4B58:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B4B60: .word 0x5F564354
_020B4B64: .word _021D848C
	arm_func_end VCTi_HandleData
