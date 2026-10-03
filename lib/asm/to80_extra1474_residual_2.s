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
	.extern vct_prepare_send_buffer

.text
	arm_func_start vct_flush_send_buffer
vct_flush_send_buffer: ; 0x020B22E4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r1, _020B23E4 ; =_021D6040
	ldr r0, [r1, #0x20]
	cmp r0, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, [r1, #0xf08]
	cmp r0, #0
	bne _020B232C
	bl vct_prepare_send_buffer
	ldr r1, _020B23E4 ; =_021D6040
	cmp r0, #0
	str r0, [r1, #0xf08]
	ldrne r0, [r1, #0x18]
	cmpne r0, #0
	movne r0, #0
	moveq r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B232C:
	ldr r0, _020B23E8 ; =__vct
	mov r5, #1
	ldr r0, [r0, #0x10]
	cmp r0, #2
	bne _020B2360
	ldr r3, [r1, #0xf10]
	ldr r0, _020B23EC ; =sTransceiverBalance
	add r2, r3, #1
	str r2, [r1, #0xf10]
	ldr r1, [r1, #0xf14]
	add r1, r1, r1, lsl #1
	add r0, r0, r1
	ldrb r5, [r3, r0]
_020B2360:
	mov r4, #0
	mov r7, #0x80000000
	ldr r6, _020B23E4 ; =_021D6040
	b _020B23D4
_020B2370:
	ldr r1, [r6, #0xf0c]
	mov r2, r1
	clz r2, r2
	cmp r2, #0x20
	beq _020B23DC
	mvn r0, r7, lsr r2
	and r0, r1, r0
	str r0, [r6, #0xf0c]
	rsb r0, r2, #0x1f
	ldr r1, [r6, #0xf08]
	ldr r2, [r6, #0xc]
	and r0, r0, #0xff
	bl DWC_SendUnreliable
	ldr r0, [r6, #0xf0c]
	cmp r0, #0
	bne _020B23D0
	ldr r0, _020B23E4 ; =_021D6040
	mov r1, #0
	ldr r2, [r0, #0xf00]
	add r2, r2, #1
	str r2, [r0, #0xf00]
	str r1, [r0, #0xf08]
	str r1, [r0, #0xf0c]
	b _020B23DC
_020B23D0:
	add r4, r4, #1
_020B23D4:
	cmp r4, r5
	blt _020B2370
_020B23DC:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B23E4: .word _021D6040
_020B23E8: .word __vct
_020B23EC: .word sTransceiverBalance
	arm_func_end vct_flush_send_buffer
