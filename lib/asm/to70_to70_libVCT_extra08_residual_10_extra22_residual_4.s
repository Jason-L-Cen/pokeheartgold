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
	arm_func_start VCTi_GetVADStatus
VCTi_GetVADStatus: ; 0x020B41C8
	stmdb sp!, {r4, lr}
	ldr r3, _020B4394 ; =sVADInfo
	mov ip, r1, lsr #8
	str ip, [r3, #4]
	cmp r2, #0
	strneb ip, [r2]
	ldr r2, _020B4394 ; =sVADInfo
	ldr r2, [r2]
	cmp r2, #0
	bne _020B42DC
	mov ip, #0
	ldr r3, _020B4398 ; =E2scales
	mov lr, ip
_020B41FC:
	ldr r2, [r3, lr, lsl #2]
	add lr, lr, #1
	cmp lr, #4
	add ip, ip, r2
	blt _020B41FC
	ldr r3, _020B439C ; =0x040002B0
	mov r2, #0
	strh r2, [r3]
	mov r2, ip, lsr #2
	str r2, [r3, #8]
_020B4224:
	ldrh r2, [r3]
	tst r2, #0x8000
	bne _020B4224
	ldr r2, _020B43A0 ; =0x040002B4
	cmp r1, #0
	ldr r2, [r2]
	beq _020B42B8
	cmp r1, r2, lsl #1
	blo _020B42B8
	mov r3, #0
	ldr r2, _020B4398 ; =E2scales
	mov ip, r3
_020B4254:
	ldr r1, [r2, ip, lsl #2]
	add ip, ip, #1
	cmp ip, #4
	add r3, r3, r1
	blo _020B4254
	ldr r2, _020B439C ; =0x040002B0
	mov r1, #0
	strh r1, [r2]
	mov r1, r3, lsr #2
	str r1, [r2, #8]
_020B427C:
	ldrh r1, [r2]
	tst r1, #0x8000
	bne _020B427C
	ldr r1, _020B43A0 ; =0x040002B4
	ldr r2, _020B43A4 ; =_021D6FEC
	ldr r1, [r1]
	mov r3, #0
	add r1, r1, r1, lsl #1
	mov r1, r1, lsr #1
	str r1, [r2]
	ldr r1, _020B4394 ; =sVADInfo
	str r3, [r2, #4]
	mov lr, #1
	str lr, [r1]
	b _020B42BC
_020B42B8:
	mov lr, #0
_020B42BC:
	ldr r1, _020B43A4 ; =_021D6FEC
	ldr r3, _020B4398 ; =E2scales
	ldr ip, [r1, #4]
	add r2, ip, #1
	and r2, r2, #3
	str r0, [r3, ip, lsl #2]
	str r2, [r1, #4]
	b _020B438C
_020B42DC:
	ldr r1, _020B43A4 ; =_021D6FEC
	mov lr, #0
	ldr ip, [r1, #4]
	ldr r3, _020B4398 ; =E2scales
	add r2, ip, #1
	and r2, r2, #3
	mov r4, lr
	str r0, [r3, ip, lsl #2]
	str r2, [r1, #4]
_020B4300:
	ldr r0, [r3, r4, lsl #2]
	add r4, r4, #1
	cmp r4, #4
	add lr, lr, r0
	blt _020B4300
	ldr r1, _020B439C ; =0x040002B0
	mov r0, #0
	strh r0, [r1]
	mov r0, lr, lsr #2
	str r0, [r1, #8]
_020B4328:
	ldrh r0, [r1]
	tst r0, #0x8000
	bne _020B4328
	ldr r0, _020B43A0 ; =0x040002B4
	ldr r1, _020B43A4 ; =_021D6FEC
	ldr r2, [r0]
	ldr r0, [r1]
	cmp r2, r0
	bhi _020B4380
	ldr r2, [r1, #8]
	ldr r0, _020B4394 ; =sVADInfo
	add r3, r2, #1
	str r3, [r1, #8]
	ldr r2, [r0, #8]
	cmp r3, r2
	ble _020B4388
	mov r2, #0
	str r2, [r0]
	str r2, [r1, #8]
	str r2, [r1, #4]
	mov r0, #3
	ldmia sp!, {r4, pc}
_020B4380:
	mov r0, #0
	str r0, [r1, #8]
_020B4388:
	mov lr, #2
_020B438C:
	mov r0, lr
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B4394: .word sVADInfo
_020B4398: .word E2scales
_020B439C: .word 0x040002B0
_020B43A0: .word 0x040002B4
_020B43A4: .word _021D6FEC
	arm_func_end VCTi_GetVADStatus
