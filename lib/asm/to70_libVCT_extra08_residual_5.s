	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_decode_audio_buffer
	.public vct_flush_send_buffer
	.public vct_handle_audio
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

	arm_func_start vct_handle_audio
vct_handle_audio: ; 0x020B2028
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r4, _020B2204 ; =_021D6040
	mov sl, r0
	ldr r0, [r4, #0x20]
	ldr r7, [sp, #0x28]
	mov sb, r1
	mov r8, r2
	mov fp, r3
	mov r5, #0
	cmp r0, #0
	moveq r0, r5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, _020B2208 ; =sStream
	mov r2, r5
_020B2060:
	ldr r0, [r1, #0x38]
	cmp sl, r0
	bne _020B207C
	ldr r1, _020B2208 ; =sStream
	mov r0, #0x94
	mla r5, r2, r0, r1
	b _020B208C
_020B207C:
	add r2, r2, #1
	cmp r2, #3
	add r1, r1, #0x94
	blo _020B2060
_020B208C:
	cmp r5, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrb r0, [sb, #4]
	cmp r0, #0x41
	bne _020B20C0
	ldrh r2, [sb, #6]
	mov r0, #0
	mov r1, #1
	str r2, [r5, #0x30]
	str r0, [r5, #0x4c]
	str r1, [r5, #0x6c]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B20C0:
	cmp r0, #0x40
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldrb r0, [sb, #5]
	and r6, r0, #0x7f
	cmp r6, #5
	str r6, [r5, #0x24]
	movge r0, #0
	ldmgeia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, _020B220C ; =sCodecInfo + 1
	ldr r0, _020B2210 ; =sCodecInfo
	ldrb r2, [r1, r6, lsl #1]
	ldrb r1, [r0, r6, lsl #1]
	mov r0, #0x44
	mla r0, r1, r0, r2
	add r0, r0, #0xc
	cmp r8, r0
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	movne r0, #1
	movne r1, r0, lsl sl
	ldr r0, _020B2204 ; =_021D6040
	moveq r1, #1
	ldr r0, [r0, #4]
	tst r0, r1
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl vct_alloc_packet_buffer
	movs r4, r0
	bne _020B2174
	ldr r0, [r5, #0x34]
	bl vct_flush_audio_queue
	ldr r0, [r5, #0x34]
	bl vct_flush_decoder
	bl vct_alloc_packet_buffer
	movs r4, r0
	bne _020B2168
	bl vct_flush_all_audio_queue
	bl vct_flush_all_decoder
	bl vct_alloc_packet_buffer
	mov r4, r0
_020B2168:
	cmp r4, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B2174:
	add r0, r4, #0x5c
	add r0, r0, #0x400
	sub r2, r8, #0xc
	sub r1, r0, r2
	add r0, sb, #0xc
	str r1, [sp]
	bl MI_CpuCopy8
	ldr r0, [r5, #0x34]
	sub r1, r8, #0xc
	strb r0, [r4, #0x11]
	ldr r0, [sp]
	str r6, [r4, #0xc]
	str r0, [r4, #0x474]
	str r1, [r4, #0x14]
	str fp, [r4, #0x464]
	str r7, [r4, #0x468]
	strb sl, [r4, #0x10]
	ldrb r2, [sb, #5]
	mov r0, r5
	mov r1, r4
	and r2, r2, #0x80
	str r2, [r4, #0x45c]
	ldr r2, [sb, #8]
	str r2, [r4, #0x460]
	ldrh r2, [sb, #6]
	str r2, [r4, #0x18]
	bl vct_calc_jitter_and_skew
	cmp r0, #0
	mov r0, r4
	bne _020B21F8
	bl vct_free_packet_buffer
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B21F8:
	bl vct_insert_decoder
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B2204: .word _021D6040
_020B2208: .word sStream
_020B220C: .word sCodecInfo + 1
_020B2210: .word sCodecInfo
	arm_func_end vct_handle_audio

	arm_func_start vct_decode_audio_buffer
vct_decode_audio_buffer: ; 0x020B2214
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	bl vct_top_decoder
	movs r4, r0
	moveq r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, pc}
	bl OS_DisableInterrupts
	ldrb r6, [r4, #0x11]
	mov r7, r0
	mov r0, r6
	bl vct_count_decoder_queue
	mov r5, r0
	cmp r5, #0
	mov r4, #0
	ble _020B22D4
_020B224C:
	mov r0, r6
	bl vct_get_packet_from_decoder
	mov r8, r0
	ldr r3, [r8, #0xc]
	cmp r3, #2
	blt _020B2278
	ldr r0, [r8, #0x474]
	ldr r2, [r8, #0x14]
	add r1, r8, #0x1c
	bl vct_decode_adpcm
	b _020B229C
_020B2278:
	cmp r3, #1
	ldr r0, [r8, #0x474]
	ldr r2, [r8, #0x14]
	bne _020B2294
	add r1, r8, #0x1c
	bl vct_decode_g711_ulaw
	b _020B229C
_020B2294:
	add r1, r8, #0x1c
	bl vct_decode_8bit_raw
_020B229C:
	add r1, r8, #0x1c
	mov r0, r8
	str r1, [r8, #0x474]
	bl vct_remove_packet_from_decoder
	ldrb r1, [r8, #0x11]
	mov r0, r8
	bl vct_insert_audio_queue
	cmp r0, #0
	bge _020B22C8
	mov r0, r8
	bl vct_free_packet_buffer
_020B22C8:
	add r4, r4, #1
	cmp r4, r5
	blt _020B224C
_020B22D4:
	mov r0, r7
	bl OS_RestoreInterrupts
	mov r0, r5
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end vct_decode_audio_buffer

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
