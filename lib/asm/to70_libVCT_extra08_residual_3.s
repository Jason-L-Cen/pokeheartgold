	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.extern VCT_ResetVAD
	.extern _021D6040
	.extern sAudioSession
	.extern sStream
	.extern vct_init_audio_queue
	.extern vct_init_stream

.text
	arm_func_start VCT_StartStreaming
VCT_StartStreaming: ; 0x020B1694
	stmdb sp!, {r3, r4, r5, lr}
	movs r5, r0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, _020B17C0 ; =_021D6040
	ldr r0, [r0, #0x20]
	cmp r0, #3
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, [r5, #0xc]
	cmp r0, #2
	cmpne r0, #3
	movne r0, #0
	ldmneia sp!, {r3, r4, r5, pc}
	mov r4, #0
	ldr r1, _020B17C4 ; =sAudioSession
	b _020B16EC
_020B16D8:
	ldr r0, [r1, r4, lsl #2]
	cmp r0, r5
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	add r4, r4, #1
_020B16EC:
	cmp r4, #3
	blo _020B16D8
	mov r4, #0
	ldr r1, _020B17C4 ; =sAudioSession
	b _020B1714
_020B1700:
	ldr r0, [r1, r4, lsl #2]
	cmp r0, #0
	streq r5, [r1, r4, lsl #2]
	beq _020B171C
	add r4, r4, #1
_020B1714:
	cmp r4, #3
	blo _020B1700
_020B171C:
	ldr r1, _020B17C0 ; =_021D6040
	ldr r2, _020B17C8 ; =sStream
	ldr r3, [r1, #0x20]
	mov r0, #0x94
	add r3, r3, #1
	str r3, [r1, #0x20]
	ldr ip, [r1, #4]
	ldr r3, [r5, #8]
	mla r0, r4, r0, r2
	orr r2, ip, r3
	str r2, [r1, #4]
	ldrb r2, [r5, #5]
	mov r1, r4
	bl vct_init_stream
	mov r0, r4
	bl vct_init_audio_queue
	ldr r0, [r5, #0xc]
	cmp r0, #2
	movne r0, #1
	ldmneia sp!, {r3, r4, r5, pc}
	ldr r0, _020B17C0 ; =_021D6040
	ldr r0, [r0, #0x20]
	cmp r0, #1
	bne _020B17B8
	bl VCT_ResetVAD
	bl OS_GetTickLo
	ldr r1, _020B17C0 ; =_021D6040
	mov r2, #0
	strh r0, [r1]
	strh r2, [r1, #0x10]
	strb r2, [r1, #0x12]
	mov r0, #1
	str r0, [r1, #0x14]
	str r2, [r1, #0xf00]
	str r2, [r1, #0xf04]
	str r2, [r1, #0xf0c]
	str r2, [r1, #0xf08]
	str r2, [r1, #0xf10]
	str r2, [r1, #0xf14]
_020B17B8:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020B17C0: .word _021D6040
_020B17C4: .word sAudioSession
_020B17C8: .word sStream
	arm_func_end VCT_StartStreaming
