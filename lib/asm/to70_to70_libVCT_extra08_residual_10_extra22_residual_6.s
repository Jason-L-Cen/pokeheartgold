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
	.public DoFIRFilter

.text
	arm_func_start DoFIRFilter
DoFIRFilter: ; 0x020B4550
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x10
	mov r4, #0
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	mov r8, r4
	movs r2, r2, lsr #1
	beq _020B4678
	ldr lr, _020B4754 ; =0x3FFF8000
	ldr ip, _020B4758 ; =_021D7008
_020B4578:
	mov r4, r8, lsl #1
	ldr r7, [ip, #0xc]
	ldrsh r6, [r1, r4]
	ldr r4, _020B475C ; =sDelayLine
	mov r5, r7, lsl #1
	strh r6, [r4, r5]
	ldr r4, [ip, #8]
	str r4, [sp]
	mov r5, r4, lsl #1
	ldr r4, _020B475C ; =sDelayLine
	ldrsh r6, [r4, r5]
	str r6, [ip, #0x2c]
	ldr sb, [ip, #0x3c]
	ldr sl, [ip, #0x24]
	ldr r5, [ip, #0x40]
	mul sl, sb, sl
	ldr r4, [ip, #0x28]
	str sb, [sp, #4]
	mla r4, r5, r4, sl
	mov r5, sb
	str r5, [ip, #0x40]
	ldr sb, [ip, #0x38]
	ldr r5, [ip, #0x20]
	mla r5, sb, r5, r4
	str sb, [ip, #0x3c]
	ldr sb, [ip, #0x34]
	ldr r4, [ip, #0x1c]
	mla r5, sb, r4, r5
	str sb, [ip, #0x38]
	ldr sb, [ip, #0x30]
	ldr r4, [ip, #0x18]
	mla r5, sb, r4, r5
	str sb, [ip, #0x34]
	ldr r4, [ip, #0x14]
	mla r4, r6, r4, r5
	cmp r4, lr
	str r6, [ip, #0x30]
	movgt r4, lr
	bgt _020B461C
	cmp r4, #0xc0000000
	movlt r4, #0xc0000000
_020B461C:
	add r5, r7, #1
	and r5, r5, lr, lsr #19
	str r5, [ip, #0xc]
	ldr r5, [sp]
	mov r6, r8, lsl #1
	add r5, r5, #1
	and r5, r5, lr, lsr #19
	str r5, [ip, #8]
	mov r4, r4, asr #0xf
	strh r4, [r1, r6]
	ldrsh r5, [r1, r6]
	ldrsh r4, [r0, r6]
	add r8, r8, #1
	smulbb r5, r5, r5
	strh r4, [r1, r6]
	ldr r4, [sp, #8]
	adds r4, r4, r5
	str r4, [sp, #8]
	ldr r4, [sp, #0xc]
	adc r4, r4, r5, asr #31
	str r4, [sp, #0xc]
	cmp r8, r2
	blo _020B4578
_020B4678:
	ldr r4, _020B4760 ; =0x04000280
	mov r1, #1
	strh r1, [r4]
	ldr r1, [sp, #8]
	str r1, [r4, #0x10]
	ldr r1, [sp, #0xc]
	str r1, [r4, #0x14]
	str r2, [r4, #0x18]
	mov r1, #0
	str r1, [r4, #0x1c]
_020B46A0:
	ldrh r1, [r4]
	tst r1, #0x8000
	bne _020B46A0
	ldr r5, _020B4764 ; =0x040002A0
	mov r1, #0
	ldr r4, [r5]
	strh r1, [r5, #0x10]
	str r4, [r5, #0x18]
_020B46C0:
	ldrh r1, [r5, #0x10]
	tst r1, #0x8000
	bne _020B46C0
	ldr r1, _020B4768 ; =0x040002B4
	ldr r4, [r1]
	add r1, r4, r4, lsl #1
	cmp r1, r3, lsl #1
	bhs _020B46F4
	ldr r0, _020B4758 ; =_021D7008
	mov r1, #0
	add sp, sp, #0x10
	strh r1, [r0]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020B46F4:
	cmp r3, r4
	ldrlo r1, _020B4758 ; =_021D7008
	movlo r3, #4
	strloh r3, [r1]
	ldr r1, _020B4758 ; =_021D7008
	mov r5, #0
	ldrsh r3, [r1]
	cmp r3, #4
	addlt r3, r3, #1
	strlth r3, [r1]
	cmp r2, #0
	addls sp, sp, #0x10
	ldmlsia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, _020B4758 ; =_021D7008
	ldrsh r4, [r1]
_020B4730:
	mov r3, r5, lsl #1
	ldrsh r1, [r0, r3]
	add r5, r5, #1
	cmp r5, r2
	mov r1, r1, asr r4
	strh r1, [r0, r3]
	blo _020B4730
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020B4754: .word 0x3FFF8000
_020B4758: .word _021D7008
_020B475C: .word sDelayLine
_020B4760: .word 0x04000280
_020B4764: .word 0x040002A0
_020B4768: .word 0x040002B4
	arm_func_end DoFIRFilter
