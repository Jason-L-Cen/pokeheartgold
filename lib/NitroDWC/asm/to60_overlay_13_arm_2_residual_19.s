	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_022408A0
	.extern ov13_02240A18
	.extern ov13_02240D80
	.extern ov13_0224128C
	.extern ov13_02242528
	.extern ov13_0224594C
	.extern ov13_02245950
	.extern ov13_02245952
	.extern ov13_0224F5E0

.text
	arm_func_start ov13_022408A0
ov13_022408A0: ; 0x022408A0
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	mov r8, r0
	ldr r0, _022409FC ; =ov13_0224F5E0
	ldr r3, _02240A00 ; =ov13_02245952
	mov r4, r8, lsl #2
	ldr r2, _02240A04 ; =ov13_02245950
	ldr r0, [r0]
	ldrh r5, [r3, r4]
	ldrh r6, [r2, r4]
	add r3, r0, #0x610
	mov r0, #0x30
	mul r2, r6, r5
	mov r7, r1
	mla r4, r8, r0, r3
	mov r0, r2, lsl #5
	mov r1, #0x20
	bl ov13_0224128C
	str r0, [r4, #0x28]
	cmp r8, #1
	bne _02240918
	ldr r1, _02240A08 ; =0x04001008
	ldrh r0, [r1]
	bic r0, r0, #0x40
	strh r0, [r1]
	ldrh r0, [r1]
	and r0, r0, #0x43
	orr r0, r0, #0xc00
	strh r0, [r1]
	b _02240938
_02240918:
	ldr r1, _02240A0C ; =0x04000008
	ldrh r0, [r1]
	bic r0, r0, #0x40
	strh r0, [r1]
	ldrh r0, [r1]
	and r0, r0, #0x43
	orr r0, r0, #0xc00
	strh r0, [r1]
_02240938:
	mov r0, #4
	str r0, [sp]
	ldr r1, [r4, #0x28]
	mov r0, r4
	mov r2, r6
	mov r3, r5
	bl NNS_G2dCharCanvasInitForBG
	ldr r0, _022409FC ; =ov13_0224F5E0
	mov r1, #1
	ldr r0, [r0]
	cmp r8, #1
	str r4, [r4, #0x18]
	add r0, r0, r7, lsl #3
	str r0, [r4, #0x1c]
	str r1, [r4, #0x20]
	str r1, [r4, #0x24]
	bne _02240984
	bl G2S_GetBG0ScrPtr
	b _02240988
_02240984:
	bl G2_GetBG0ScrPtr
_02240988:
	ldr r1, _02240A10 ; =ov13_0224594C
	mov r2, r8, lsl #1
	mov r3, #0
	ldrh r7, [r1, r2]
	str r3, [sp]
	mov r1, #0x20
	str r1, [sp, #4]
	mov r1, r6
	mov r2, r5
	str r7, [sp, #8]
	mov r5, #0xf
	str r5, [sp, #0xc]
	bl NNS_G2dMapScrToCharText
	mov r0, r4
	mov r1, #0
	bl ov13_02240D80
	ldr r0, _022409FC ; =ov13_0224F5E0
	ldr r1, _02240A14 ; =ov13_02240A18
	ldr r2, [r0]
	mov r0, #1
	add r2, r2, #0x27c
	add r2, r2, #0x400
	add r2, r2, r8
	mov r3, #0xc8
	bl ov13_02242528
	str r0, [r4, #0x2c]
	mov r0, r4
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_022409FC: .word ov13_0224F5E0
_02240A00: .word ov13_02245952
_02240A04: .word ov13_02245950
_02240A08: .word 0x04001008
_02240A0C: .word 0x04000008
_02240A10: .word ov13_0224594C
_02240A14: .word ov13_02240A18
	arm_func_end ov13_022408A0
