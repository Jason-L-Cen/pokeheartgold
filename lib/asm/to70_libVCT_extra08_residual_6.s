	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_encode_g711_ulaw
	.extern ulaw_segment

.text
	arm_func_start vct_encode_g711_ulaw
vct_encode_g711_ulaw: ; 0x020B2684
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r7, #0
	cmp r2, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr ip, _020B2728 ; =0x00001FDF
	ldr r3, _020B272C ; =ulaw_segment
	mov lr, #0xff
	mov r4, #0x7f
_020B26A4:
	mov r5, r7, lsl #1
	ldrsh r5, [r1, r5]
	mov fp, r3
	movs sl, r5, asr #2
	movmi r6, r4
	ldr r5, _020B2728 ; =0x00001FDF
	rsbmi sl, sl, #0
	movpl r6, lr
	cmp sl, r5
	movgt sl, ip
	mov r5, #0
	add sb, sl, #0x21
_020B26D4:
	ldr r8, [fp], #4
	cmp sb, r8
	ble _020B26F0
	add r5, r5, #1
	cmp r5, #8
	blt _020B26D4
	mov r5, #8
_020B26F0:
	cmp r5, #8
	eorge r5, r6, #0x7f
	bge _020B2714
	add sb, sl, #0x21
	add r8, r5, #1
	mov r8, sb, asr r8
	and r8, r8, #0xf
	orr r5, r8, r5, lsl #4
	eor r5, r5, r6
_020B2714:
	strb r5, [r0, r7]
	add r7, r7, #1
	cmp r7, r2
	blo _020B26A4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B2728: .word 0x00001FDF
_020B272C: .word ulaw_segment
	arm_func_end vct_encode_g711_ulaw
