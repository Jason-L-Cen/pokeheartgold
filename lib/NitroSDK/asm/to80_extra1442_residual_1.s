	.include "asm/macros.inc"
	.include "fx_mtx44.inc"
	.include "global.inc"

.text
	arm_func_start MTX_PerspectiveW
MTX_PerspectiveW: ; 0x020CC84C
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r4, r0
	mov r0, r1
	mov r1, r4
	mov r8, r2
	mov r7, r3
	ldr r6, [sp, #0x1c]
	ldr r5, [sp, #0x20]
	bl FX_Div
	ldr r1, [sp, #0x18]
	ldr r2, _020CC994 ; =0x04000290
	mov r3, #0
	str r3, [r2]
	mov r3, #0x1000
	str r3, [r2, #4]
	sub r1, r7, r1
	str r1, [r2, #8]
	mov r1, #0
	mov r4, r0
	str r1, [r2, #0xc]
	cmp r6, #0x1000
	beq _020CC8B4
	mul r1, r4, r6
	mov r0, r1, asr #0xb
	add r0, r1, r0, lsr #20
	mov r4, r0, asr #0xc
_020CC8B4:
	mov r1, #0
	str r1, [r5, #4]
	str r1, [r5, #8]
	str r1, [r5, #0xc]
	str r1, [r5, #0x10]
	str r4, [r5, #0x14]
	str r1, [r5, #0x18]
	str r1, [r5, #0x1c]
	str r1, [r5, #0x20]
	str r1, [r5, #0x24]
	rsb r0, r6, #0
	str r0, [r5, #0x2c]
	str r1, [r5, #0x30]
	str r1, [r5, #0x34]
	str r1, [r5, #0x3c]
	bl FX_GetDivResultFx64c
	ldr r2, _020CC994 ; =0x04000290
	mov r3, #0
	stmia r2, {r3, r4, r8}
	str r3, [r2, #0xc]
	cmp r6, #0x1000
	beq _020CC930
	mov r2, r6, asr #0x1f
	umull r4, r3, r0, r6
	mla r3, r0, r2, r3
	mla r3, r1, r6, r3
	mov r0, r4
	mov r1, r3
	mov r2, #0x1000
	mov r3, #0
	bl _ll_sdiv
_020CC930:
	ldr r4, [sp, #0x18]
	mov r2, r7, lsl #1
	add r6, r4, r7
	mov r3, r6, asr #0x1f
	umull r8, r7, r0, r6
	mla r7, r0, r3, r7
	smull r4, r3, r2, r4
	mla r7, r1, r6, r7
	adds r2, r8, #0x80000000
	adc r7, r7, #0
	adds r4, r4, #0x800
	adc r2, r3, #0
	mov r3, r4, lsr #0xc
	orr r3, r3, r2, lsl #20
	umull r6, r4, r0, r3
	mov r2, r3, asr #0x1f
	mla r4, r0, r2, r4
	mla r4, r1, r3, r4
	adds r0, r6, #0x80000000
	str r7, [r5, #0x28]
	adc r0, r4, #0
	str r0, [r5, #0x38]
	bl FX_GetDivResult
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020CC994: .word 0x04000290
	arm_func_end MTX_PerspectiveW
