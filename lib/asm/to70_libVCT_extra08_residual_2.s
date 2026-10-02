	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_init_audio
	.extern InitFIRFilter
	.extern _021D6040
	.extern __vct
	.extern sAudioSession
	.extern sCodecInfo
	.extern sEnableVAD
	.extern sStream
	.extern vct_init_audio_queue
	.extern vct_init_decoder
	.extern vct_init_stream
	.extern vct_init_vad

.text
	arm_func_start vct_init_audio
vct_init_audio: ; 0x020B1584
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	bl OS_GetTickLo
	ldr r2, _020B1668 ; =_021D6040
	ldr r1, _020B166C ; =sEnableVAD
	mov r3, #1
	str r3, [r1]
	strh r0, [r2]
	mov r1, #0
	str r1, [r2, #0x20]
	ldr r0, _020B1670 ; =__vct
	str r1, [r2, #4]
	ldr r0, [r0, #0x10]
	ldr r3, _020B1674 ; =sCodecInfo
	cmp r0, #1
	streq r1, [r2, #0x1c]
	movne r0, #4
	strne r0, [r2, #0x1c]
	ldr r1, _020B1668 ; =_021D6040
	mov r0, #0x440
	str r0, [r1, #0x24]
	ldr r5, [r1, #0x1c]
	mov r0, #0x44
	add r2, r3, r5, lsl #1
	ldrb r4, [r2, #1]
	ldrb r3, [r3, r5, lsl #1]
	mov r2, #0
	mla r0, r3, r0, r4
	add r0, r0, #0xc
	str r0, [r1, #0xc]
	str r2, [r1, #0xf00]
	str r2, [r1, #0xf04]
	str r2, [r1, #0xf0c]
	str r2, [r1, #0xf08]
	str r2, [r1, #0xf10]
	str r2, [r1, #0xf14]
	bl vct_init_vad
	mov sb, #0
	mov r8, sb
	ldr r7, _020B1678 ; =sAudioSession
	ldr r6, _020B167C ; =sStream
	mvn r5, #0
	mov r4, #0x94
	b _020B1650
_020B1630:
	mov r0, sb
	str r8, [r7, sb, lsl #2]
	bl vct_init_audio_queue
	mla r0, sb, r4, r6
	mov r1, sb
	mov r2, r5
	bl vct_init_stream
	add sb, sb, #1
_020B1650:
	cmp sb, #3
	blo _020B1630
	bl vct_init_decoder
	bl InitFIRFilter
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020B1668: .word _021D6040
_020B166C: .word sEnableVAD
_020B1670: .word __vct
_020B1674: .word sCodecInfo
_020B1678: .word sAudioSession
_020B167C: .word sStream
	arm_func_end vct_init_audio
