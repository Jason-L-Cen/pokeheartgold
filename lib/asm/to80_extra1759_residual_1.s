	.extern InitCharCanvas
	.extern NNS_G2dCharCanvasDrawChar
	.extern NNS_G2dFindBinaryBlock
	.extern NNSi_G2dFontGetStringWidth
	.extern NNSi_G2dFontGetTextHeight
	.extern NNSi_G2dFontGetTextRect
	.extern NNSi_G2dUnpackNFT
	.extern OBJSizeToShape
	.extern _02109408
	.extern _02109414
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_G2dCharCanvasInitForOBJ1D
NNS_G2dCharCanvasInitForOBJ1D: ; 0x020BD3FC
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x10
	cmp r2, #8
	movlt r4, r2
	movge ip, #3
	clzlt r4, r4
	rsblt ip, r4, #0x1f
	cmp r3, #8
	movlt r4, r3
	movge r5, #3
	clzlt r4, r4
	rsblt r5, r4, #0x1f
	ldr r4, _020BD47C ; =_02109414
	ldr lr, [sp, #0xc]
	add r5, r4, r5, lsl #3
	add r4, r5, ip, lsl #1
	ldrb ip, [r5, ip, lsl #1]
	bic r5, lr, #0xff
	ldrb r4, [r4, #1]
	ldr lr, [sp, #0x20]
	orr r5, r5, ip
	str lr, [sp]
	ldr ip, _020BD480 ; =_02109408
	bic lr, r5, #0xff00
	mov r4, r4, lsl #0x18
	orr r4, lr, r4, lsr #16
	str ip, [sp, #4]
	str r4, [sp, #0xc]
	str r4, [sp, #8]
	bl InitCharCanvas
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BD47C: .word _02109414
_020BD480: .word _02109408
	arm_func_end NNS_G2dCharCanvasInitForOBJ1D
