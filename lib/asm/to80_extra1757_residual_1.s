	.extern HPFFilter
	.extern VCTi_GetVADStatus
	.extern _021D6040
	.extern __vct
	.extern sAdpcmState
	.extern sCodecInfo
	.extern sEnableVAD
	.extern sSendInfo
	.extern sStream
	.extern sTransceiverBalance
	.extern vct_alloc_packet_buffer
	.extern vct_calc_jitter_and_skew
	.extern vct_count_decoder_queue
	.extern vct_decode_8bit_raw
	.extern vct_decode_adpcm
	.extern vct_decode_g711_ulaw
	.extern vct_encode_8bit_raw
	.extern vct_encode_adpcm
	.extern vct_encode_g711_ulaw
	.extern vct_flush_all_audio_queue
	.extern vct_flush_all_decoder
	.extern vct_flush_audio_queue
	.extern vct_flush_decoder
	.extern vct_free_packet_buffer
	.extern vct_get_packet_from_decoder
	.extern vct_insert_audio_queue
	.extern vct_insert_decoder
	.extern vct_process_fir
	.extern vct_remove_packet_from_decoder
	.extern vct_top_decoder
	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"

.text
	arm_func_start vct_prepare_send_buffer
vct_prepare_send_buffer: ; 0x020B1DAC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #4
	ldr r0, _020B200C ; =_021D6040
	ldr r2, [r0, #0xf04]
	ldr r1, [r0, #0xf00]
	ldr r8, [r0, #0x1c]
	sub r1, r2, r1
	cmp r1, #1
	subhi r1, r2, #1
	strhi r1, [r0, #0xf00]
	movhi r1, #1
	cmp r1, #0
	addeq sp, sp, #4
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _020B200C ; =_021D6040
	ldr r0, [r0, #0xf00]
	tst r0, #1
	movne r1, #0
	moveq r1, #0x460
	ldr r0, _020B2010 ; =sSendInfo
	cmp r8, #2
	add r5, r0, r1
	ldr r1, _020B200C ; =_021D6040
	add r4, r5, #0xc
	addge r4, r4, #4
	ldr r1, [r1, #0x24]
	mov r0, r4
	bl HPFFilter
	ldr r1, _020B2014 ; =0x040002B0
	mov r2, #0
	mov r6, r0
	strh r2, [r1]
	str r6, [r1, #8]
_020B1E34:
	ldrh r0, [r1]
	tst r0, #0x8000
	bne _020B1E34
	ldr r0, _020B200C ; =_021D6040
	ldr r2, _020B2018 ; =0x040002B4
	ldr r1, [r0, #0x18]
	ldr r7, [r2]
	cmp r1, #0
	beq _020B1E68
	ldr r1, [r0, #0x24]
	mov r0, r4
	mov r2, r7
	bl vct_process_fir
_020B1E68:
	ldr r0, _020B201C ; =sEnableVAD
	strb r8, [r5, #5]
	ldr r0, [r0]
	cmp r0, #0
	beq _020B1ED8
	mov r0, r6
	mov r1, r7
	mov r2, #0
	bl VCTi_GetVADStatus
	ldr r1, _020B200C ; =_021D6040
	cmp r0, #0
	str r0, [r1, #0x28]
	beq _020B1EB4
	cmp r0, #1
	beq _020B1ECC
	cmp r0, #3
	moveq r0, #0x41
	streqb r0, [r5, #4]
	b _020B1ED8
_020B1EB4:
	ldr r2, [r1, #0xf00]
	add sp, sp, #4
	add r2, r2, #1
	str r2, [r1, #0xf00]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_020B1ECC:
	ldrb r0, [r5, #5]
	orr r0, r0, #0x80
	strb r0, [r5, #5]
_020B1ED8:
	ldr r0, _020B201C ; =sEnableVAD
	ldr r0, [r0]
	cmp r0, #0
	bne _020B1F0C
	ldr r0, _020B200C ; =_021D6040
	ldr r1, [r0, #8]
	cmp r1, #0
	beq _020B1F0C
	ldrb r2, [r5, #5]
	mov r1, #0
	str r1, [r0, #8]
	orr r0, r2, #0x80
	strb r0, [r5, #5]
_020B1F0C:
	ldr r0, _020B200C ; =_021D6040
	ldr r1, [r0, #0x14]
	cmp r1, #0
	beq _020B1F30
	ldrb r2, [r5, #5]
	mov r1, #0
	str r1, [r0, #0x14]
	orr r0, r2, #0x80
	strb r0, [r5, #5]
_020B1F30:
	cmp r8, #2
	blt _020B1F78
	ldrb r0, [r5, #5]
	tst r0, #0x80
	beq _020B1F54
	ldr r0, _020B200C ; =_021D6040
	mov r1, #0
	strh r1, [r0, #0x10]
	strb r1, [r0, #0x12]
_020B1F54:
	ldr r0, _020B200C ; =_021D6040
	str r8, [sp]
	ldr r2, [r0, #0x24]
	ldr r3, _020B2020 ; =sAdpcmState
	mov r1, r4
	sub r0, r4, #4
	mov r2, r2, lsr #1
	bl vct_encode_adpcm
	b _020B1FBC
_020B1F78:
	cmp r8, #1
	bne _020B1F9C
	ldr r1, _020B200C ; =_021D6040
	mov r0, r4
	ldr r2, [r1, #0x24]
	mov r1, r4
	mov r2, r2, lsr #1
	bl vct_encode_g711_ulaw
	b _020B1FBC
_020B1F9C:
	cmp r8, #0
	bne _020B1FBC
	ldr r1, _020B200C ; =_021D6040
	mov r0, r4
	ldr r2, [r1, #0x24]
	mov r1, r4
	mov r2, r2, lsr #1
	bl vct_encode_8bit_raw
_020B1FBC:
	ldr r1, _020B200C ; =_021D6040
	ldr r0, _020B2024 ; =__vct
	ldr r3, [r1, #4]
	mov r2, #1
	str r3, [r1, #0xf0c]
	ldrb r0, [r0]
	mvn r0, r2, lsl r0
	and r0, r3, r0
	str r0, [r1, #0xf0c]
	bl MATH_CountPopulation
	sub r0, r0, #1
	ldr r1, _020B200C ; =_021D6040
	cmp r0, #7
	str r0, [r1, #0xf14]
	movgt r0, #0
	movle r2, #0
	movle r0, r5
	strle r2, [r1, #0xf10]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020B200C: .word _021D6040
_020B2010: .word sSendInfo
_020B2014: .word 0x040002B0
_020B2018: .word 0x040002B4
_020B201C: .word sEnableVAD
_020B2020: .word sAdpcmState
_020B2024: .word __vct
	arm_func_end vct_prepare_send_buffer
