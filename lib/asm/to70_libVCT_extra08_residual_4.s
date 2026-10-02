	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.extern _021D6040
	.extern _021D6234
	.extern sAudioSession
	.extern sSendInfo
	.extern sStream
	.extern vct_count_audio_queue
	.extern vct_free_packet_buffer
	.extern vct_mix_audio
	.extern vct_save_previous_buffer
	.extern vct_set_speaker_samples
	.extern vct_top_audio_queue

.text
	arm_func_start VCT_SendAudio
VCT_SendAudio: ; 0x020B1880
	stmdb sp!, {r4, lr}
	ldr r3, _020B1980 ; =_021D6040
	mov r2, r1
	ldr r1, [r3, #0x24]
	mov ip, #0
	cmp r2, r1
	movne r0, ip
	ldmneia sp!, {r4, pc}
	ldr r1, [r3, #0x20]
	cmp r1, #0
	moveq r0, ip
	ldmeqia sp!, {r4, pc}
	mov r4, ip
	ldr r3, _020B1984 ; =sAudioSession
	b _020B18DC
_020B18BC:
	ldr r1, [r3, r4, lsl #2]
	cmp r1, #0
	beq _020B18D8
	ldr r1, [r1, #0xc]
	cmp r1, #2
	moveq ip, #1
	beq _020B18E4
_020B18D8:
	add r4, r4, #1
_020B18DC:
	cmp r4, #3
	blo _020B18BC
_020B18E4:
	cmp ip, #0
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	ldr r1, _020B1980 ; =_021D6040
	ldr ip, _020B1988 ; =sSendInfo
	ldr r1, [r1, #0xf04]
	ldr r3, _020B198C ; =0x5F564354
	tst r1, #1
	ldr r1, _020B1980 ; =_021D6040
	movne r4, #0
	moveq r4, #0x460
	ldr r1, [r1, #0x1c]
	str r3, [ip, r4]
	cmp r1, #2
	add r4, ip, r4
	blt _020B1930
	add r1, r4, #0x10
	bl MIi_CpuCopyFast
	b _020B1938
_020B1930:
	add r1, r4, #0xc
	bl MIi_CpuCopyFast
_020B1938:
	ldr r0, _020B1980 ; =_021D6040
	mov r1, #0x40
	ldrh r2, [r0]
	strb r1, [r4, #4]
	add r1, r2, #1
	strh r1, [r0]
	strh r2, [r4, #6]
	bl OS_GetTick
	mov r2, #0x40
	mov r3, #0
	bl _ll_udiv
	str r0, [r4, #8]
	ldr r1, _020B1980 ; =_021D6040
	mov r0, #1
	ldr r2, [r1, #0xf04]
	add r2, r2, #1
	str r2, [r1, #0xf04]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B1980: .word _021D6040
_020B1984: .word sAudioSession
_020B1988: .word sSendInfo
_020B198C: .word 0x5F564354
	arm_func_end VCT_SendAudio

	arm_func_start VCTi_ReceiveAudioData
VCTi_ReceiveAudioData: ; 0x020B1990
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	ldr r4, _020B1BD4 ; =sStream
	mov sl, r2
	mov r2, #0x94
	mla r6, sl, r2, r4
	str r0, [sp]
	mov r0, sl
	str r1, [sp, #4]
	mov fp, r3
	mov r7, #1
	bl vct_count_audio_queue
	movs r4, r0
	beq _020B1BB0
_020B19C8:
	bl OS_GetTick
	mov r8, r0
	mov r0, sl
	mov sb, r1
	bl vct_top_audio_queue
	movs r5, r0
	beq _020B1B98
	ldr r1, [r5, #0x46c]
	adds r3, r8, #0x34
	ldr r0, [r5, #0x470]
	adc r2, sb, #0
	cmp r0, r2
	cmpeq r1, r3
	bls _020B1A7C
	ldr r0, [r5, #0x45c]
	cmp r0, #0
	bne _020B1A70
	ldr r0, [r6, #0x3c]
	cmp r0, #0
	beq _020B1A70
	ldr r0, [r6, #0x20]
	ldr r1, [sp]
	add r0, r0, #1
	str r0, [r6, #0x20]
	ldr r0, [r6, #0x3c]
	ldr r2, [sp, #4]
	ldr r0, [r0, #0x474]
	bl MIi_CpuCopyFast
	ldr r0, [r6, #0x3c]
	bl vct_free_packet_buffer
	mov r0, #0
	str r0, [r6, #0x3c]
	cmp fp, #0
	beq _020B1A64
	ldrb r0, [r5, #0x10]
	ldr r2, [fp]
	mov r1, #1
	orr r0, r2, r1, lsl r0
	str r0, [fp]
_020B1A64:
	add sp, sp, #8
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B1A70:
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B1A7C:
	ldr r2, _020B1BD8 ; =0x00008B4C
	adds r2, r1, r2
	adc r0, r0, #0
	cmp sb, r0
	cmpeq r8, r2
	bls _020B1ABC
	ldr r2, [r5, #0x18]
	mov r0, r6
	mov r1, r5
	str r2, [r6, #0x58]
	bl vct_save_previous_buffer
	mov r4, r0
	mov r0, sl
	bl vct_top_audio_queue
	mov r7, #0
	b _020B1BA8
_020B1ABC:
	ldr r1, [r6, #0x58]
	ldr r2, [r5, #0x18]
	add r0, r1, #1
	cmp r0, r2
	cmpne r1, #0
	beq _020B1AF0
	cmp r1, r2
	bhi _020B1AF0
	ldr r0, [r5, #0x45c]
	cmp r0, #0
	ldreq r0, [r6, #0x14]
	addeq r0, r0, #1
	streq r0, [r6, #0x14]
_020B1AF0:
	ldr r0, [r5, #0x18]
	cmp r7, #0
	str r0, [r6, #0x58]
	ldrne r0, [r6, #0x70]
	cmpne r0, #0
	beq _020B1B30
	mov r0, r6
	mov r1, r5
	bl vct_save_previous_buffer
	mov r4, r0
	mov r0, sl
	bl vct_top_audio_queue
	ldr r0, [r6, #0x70]
	sub r0, r0, #1
	str r0, [r6, #0x70]
	b _020B1BA8
_020B1B30:
	ldr r1, [r5, #0x464]
	ldr r0, [r6, #0x28]
	subs r1, r8, r1
	sub r0, r1, r0
	sub r0, r0, r1
	str r1, [r6, #0x28]
	add r0, r1, r0, lsr #4
	str r0, [r6, #0x2c]
	str r0, [r6]
	ldr r0, [r5, #0x474]
	ldr r1, [sp]
	ldr r2, [sp, #4]
	bl MIi_CpuCopyFast
	mov r0, r6
	mov r1, r5
	bl vct_save_previous_buffer
	cmp fp, #0
	beq _020B1B8C
	ldrb r0, [r5, #0x10]
	ldr r2, [fp]
	mov r1, #1
	orr r0, r2, r1, lsl r0
	str r0, [fp]
_020B1B8C:
	add sp, sp, #8
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B1B98:
	add sp, sp, #8
	str r4, [r6, #8]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B1BA8:
	cmp r4, #0
	bne _020B19C8
_020B1BB0:
	ldr r0, [r6, #0x3c]
	cmp r0, #0
	beq _020B1BC8
	bl vct_free_packet_buffer
	mov r0, #0
	str r0, [r6, #0x3c]
_020B1BC8:
	mov r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B1BD4: .word sStream
_020B1BD8: .word 0x00008B4C
	arm_func_end VCTi_ReceiveAudioData

	arm_func_start VCT_ReceiveAudio
VCT_ReceiveAudio: ; 0x020B1BDC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	movs r5, r2
	mov r7, r0
	movne r0, #0
	strne r0, [r5]
	ldr r0, _020B1CF0 ; =_021D6040
	mov r6, r1
	ldr r1, [r0, #0x24]
	mov r4, #0
	cmp r6, r1
	bne _020B1CB8
	ldr r0, [r0, #0x20]
	cmp r0, #0
	beq _020B1CB8
	mov r8, r4
_020B1C18:
	mov r0, r7
	mov r1, r6
	mov r2, r8
	mov r3, r5
	bl VCTi_ReceiveAudioData
	cmp r0, #1
	moveq r4, #1
	beq _020B1C44
	add r8, r8, #1
	cmp r8, #3
	blo _020B1C18
_020B1C44:
	cmp r4, #0
	beq _020B1CA4
	add sb, r8, #1
	cmp sb, #3
	moveq r4, #1
	beq _020B1CCC
	bhs _020B1CCC
	ldr r8, _020B1CF4 ; =_021D6234
_020B1C64:
	mov r0, r8
	mov r1, r6
	mov r2, sb
	mov r3, r5
	bl VCTi_ReceiveAudioData
	cmp r0, #1
	bne _020B1C94
	mov r0, r7
	mov r1, r8
	mov r2, r7
	mov r3, r6
	bl vct_mix_audio
_020B1C94:
	add sb, sb, #1
	cmp sb, #3
	blo _020B1C64
	b _020B1CCC
_020B1CA4:
	mov r1, r7
	mov r2, r6
	mov r0, #0
	bl MIi_CpuClearFast
	b _020B1CCC
_020B1CB8:
	mov r1, r7
	mov r2, r6
	mov r0, #0
	bl MIi_CpuClearFast
	mov r4, #0
_020B1CCC:
	ldr r0, _020B1CF0 ; =_021D6040
	ldr r0, [r0, #0x18]
	cmp r0, #0
	beq _020B1CE8
	mov r0, r7
	mov r1, r6
	bl vct_set_speaker_samples
_020B1CE8:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020B1CF0: .word _021D6040
_020B1CF4: .word _021D6234
	arm_func_end VCT_ReceiveAudio
