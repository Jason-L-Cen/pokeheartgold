	.extern InitCharCanvas
	.extern NNS_G2dCharCanvasDrawChar
	.extern NNS_G2dFindBinaryBlock
	.extern NNSi_G2dFontGetStringWidth
	.extern NNSi_G2dFontGetTextHeight
	.extern NNSi_G2dFontGetTextRect
	.extern OBJSizeToShape
	.extern _02109408
	.extern _02109414
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNSi_G2dUnpackNFT

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

	arm_func_start NNS_G2dMapScrToCharText
NNS_G2dMapScrToCharText: ; 0x020BD484
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	ldr r5, [sp, #0x24]
	ldr lr, [sp, #0x20]
	cmp r5, #0x20
	ldr ip, [sp, #0x28]
	bgt _020BD4C0
	mla r4, r5, lr, r3
	ldr lr, [sp, #0x2c]
	mov r3, r5
	add r0, r0, r4, lsl #1
	stmia sp, {ip, lr}
	bl NNS_G2dMapScrToChar256x16Pltt
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020BD4C0:
	add r6, lr, r2
	ldr r4, [sp, #0x2c]
	cmp lr, r6
	add r5, r3, r1
	addge sp, sp, #8
	mov r1, r4, lsl #0x1c
	ldmgeia sp!, {r4, r5, r6, r7, r8, pc}
_020BD4DC:
	cmp lr, #0x20
	movlt r2, lr
	addge r2, lr, #0x20
	mov r7, r3
	cmp r3, r5
	add r8, r0, r2, lsl #6
	bge _020BD520
_020BD4F8:
	cmp r7, #0x20
	movlt r2, r7
	addge r2, r7, #0x3e0
	orr r4, ip, r1, lsr #16
	mov r2, r2, lsl #1
	add r7, r7, #1
	strh r4, [r8, r2]
	cmp r7, r5
	add ip, ip, #1
	blt _020BD4F8
_020BD520:
	add lr, lr, #1
	cmp lr, r6
	blt _020BD4DC
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNS_G2dMapScrToCharText

	arm_func_start NNS_G2dMapScrToChar256x16Pltt
NNS_G2dMapScrToChar256x16Pltt: ; 0x020BD534
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r4, [sp, #0x1c]
	ldr r8, [sp, #0x18]
	mov r6, #0
	cmp r2, #0
	mov ip, r4, lsl #0x1c
	ldmleia sp!, {r4, r5, r6, r7, r8, pc}
	mov r4, r6
_020BD554:
	mov r7, r0
	mov r5, r4
	cmp r1, #0
	ble _020BD57C
_020BD564:
	orr lr, r8, ip, lsr #16
	add r5, r5, #1
	cmp r5, r1
	add r8, r8, #1
	strh lr, [r7], #2
	blt _020BD564
_020BD57C:
	add r6, r6, #1
	cmp r6, r2
	add r0, r0, r3, lsl #1
	blt _020BD554
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNS_G2dMapScrToChar256x16Pltt

	arm_func_start NNSi_G2dCalcRequiredOBJ
NNSi_G2dCalcRequiredOBJ: ; 0x020BD590
	stmdb sp!, {r3, r4, r5, lr}
	mov r3, r0, lsr #3
	mov r5, r1, lsr #3
	mov r2, #0
	mla lr, r3, r5, r2
	and r4, r0, #2
	and r2, r0, #1
	add r2, r2, r4, lsr #1
	and r0, r0, #4
	mov ip, r2, lsl #1
	add ip, ip, r0, lsr #2
	mla ip, r5, ip, lr
	and r4, r1, #2
	and lr, r1, #1
	add r4, lr, r4, lsr #1
	and lr, r1, #4
	mov r1, r4, lsl #1
	add r1, r1, lr, lsr #2
	mla ip, r3, r1, ip
	add r1, r2, r0, lsr #2
	add r0, r4, lr, lsr #2
	mla r0, r1, r0, ip
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNSi_G2dCalcRequiredOBJ

	arm_func_start NNS_G2dArrangeOBJ1D
NNS_G2dArrangeOBJ1D: ; 0x020BD5EC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x38
	mov r7, r0
	mov r0, r1
	cmp r0, #8
	ldr r0, [sp, #0x60]
	str r1, [sp, #0x10]
	str r0, [sp, #0x60]
	str r2, [sp, #0x14]
	ldrlt r0, [sp, #0x10]
	str r3, [sp, #0x34]
	ldr r6, [sp, #0x64]
	ldr r5, [sp, #0x68]
	ldr r4, [sp, #0x6c]
	movge r2, #3
	clzlt r0, r0
	rsblt r2, r0, #0x1f
	ldr r0, [sp, #0x14]
	cmp r0, #8
	ldrlt r0, [sp, #0x14]
	movge r1, #3
	clzlt r0, r0
	rsblt r1, r0, #0x1f
	ldr r0, _020BD8D8 ; =_02109414
	mvn r3, #0
	add r1, r0, r1, lsl #3
	add r0, r1, r2, lsl #1
	ldrb fp, [r1, r2, lsl #1]
	ldrb r1, [r0, #1]
	ldr r2, [sp, #0x14]
	cmp r6, #0
	str r1, [sp, #0x30]
	ldr r1, [sp, #0x10]
	and r1, r1, r3, lsl fp
	str r1, [sp, #0x2c]
	ldr r1, [sp, #0x30]
	and r1, r2, r3, lsl r1
	str r1, [sp, #0x28]
	moveq r1, #1
	streq r1, [sp, #0x18]
	movne r1, #2
	strne r1, [sp, #0x18]
	mov r1, #0
	str r1, [sp, #0x24]
	bl OBJSizeToShape
	ldr r1, [sp, #0x10]
	ldr r2, [sp, #0x14]
	mov sb, r1, asr fp
	ldr r1, [sp, #0x30]
	mov r1, r2, asr r1
	str r1, [sp, #0x20]
	ldr r1, [sp, #0x18]
	mov r2, r1, lsl fp
	ldr r1, [sp, #0x30]
	mov r8, r2, lsl r1
	mov r1, #0
	str r1, [sp, #0x1c]
	b _020BD76C
_020BD6D4:
	ldr r2, [sp, #0x1c]
	ldr r1, [sp, #0x30]
	mov sl, #0
	mov r2, r2, lsl r1
	ldr r1, [sp, #0x60]
	add r1, r1, r2, lsl #3
	mov r2, #0x400
	and r3, r1, #0xff
	rsb r2, r2, #0
	b _020BD758
_020BD6FC:
	mov ip, sl, lsl fp
	ldr r1, [sp, #0x34]
	add sl, sl, #1
	add r1, r1, ip, lsl #3
	mov lr, r1, lsl #0x17
	ldr ip, [r7]
	ldr r1, _020BD8DC ; =0xFE00FF00
	and r1, ip, r1
	orr r1, r1, r3
	orr ip, r1, lr, lsr #7
	ldr r1, _020BD8E0 ; =0x3FFF3FFF
	and r1, ip, r1
	orr r1, r1, r0
	str r1, [r7]
	ldrh r1, [r7, #4]
	and r1, r1, r2
	orr r1, r1, r5
	strh r1, [r7, #4]
	ldr r1, [r7]
	add r5, r5, r8, asr r4
	bic r1, r1, #0x2000
	orr r1, r1, r6, lsl #13
	str r1, [r7], #8
_020BD758:
	cmp sl, sb
	blt _020BD6FC
	ldr r1, [sp, #0x1c]
	add r1, r1, #1
	str r1, [sp, #0x1c]
_020BD76C:
	ldr r2, [sp, #0x1c]
	ldr r1, [sp, #0x20]
	cmp r2, r1
	blt _020BD6D4
	ldr r0, [sp, #0x24]
	mla r0, sb, r1, r0
	str r0, [sp, #0x24]
	ldr r1, [sp, #0x2c]
	ldr r0, [sp, #0x10]
	cmp r1, r0
	bhs _020BD7F4
	ldr r0, [sp, #0x60]
	ldr r1, [sp, #0x10]
	stmia sp, {r0, r6}
	ldr r0, [sp, #0x2c]
	ldr r2, [sp, #0x2c]
	sub r8, r1, r0
	ldr r1, [sp, #0x34]
	str r5, [sp, #8]
	add r3, r1, r2, lsl #3
	ldr r2, [sp, #0x28]
	mov r0, r7
	mov r1, r8
	str r4, [sp, #0xc]
	bl NNS_G2dArrangeOBJ1D
	ldr r1, [sp, #0x18]
	add r7, r7, r0, lsl #3
	mul r2, r1, r8
	ldr r1, [sp, #0x28]
	mul r2, r1, r2
	ldr r1, [sp, #0x24]
	add r5, r5, r2, lsr r4
	add r0, r1, r0
	str r0, [sp, #0x24]
_020BD7F4:
	ldr r1, [sp, #0x28]
	ldr r0, [sp, #0x14]
	cmp r1, r0
	bhs _020BD860
	ldr r1, [sp, #0x60]
	ldr r0, [sp, #0x28]
	ldr r3, [sp, #0x34]
	add r0, r1, r0, lsl #3
	stmia sp, {r0, r6}
	str r5, [sp, #8]
	ldr r1, [sp, #0x14]
	ldr r0, [sp, #0x28]
	str r4, [sp, #0xc]
	sub r8, r1, r0
	ldr r1, [sp, #0x2c]
	mov r0, r7
	mov r2, r8
	bl NNS_G2dArrangeOBJ1D
	ldr r2, [sp, #0x18]
	ldr r1, [sp, #0x2c]
	add r7, r7, r0, lsl #3
	mul r1, r2, r1
	mul r1, r8, r1
	add r5, r5, r1, lsr r4
	ldr r1, [sp, #0x24]
	add r0, r1, r0
	str r0, [sp, #0x24]
_020BD860:
	ldr r1, [sp, #0x2c]
	ldr r0, [sp, #0x10]
	cmp r1, r0
	ldrlo r1, [sp, #0x28]
	ldrlo r0, [sp, #0x14]
	cmplo r1, r0
	bhs _020BD8CC
	ldr r1, [sp, #0x60]
	ldr r0, [sp, #0x28]
	ldr r2, [sp, #0x10]
	add r0, r1, r0, lsl #3
	stmia sp, {r0, r6}
	str r5, [sp, #8]
	ldr r1, [sp, #0x2c]
	ldr r3, [sp, #0x14]
	sub r1, r2, r1
	ldr r2, [sp, #0x28]
	ldr r5, [sp, #0x2c]
	sub r2, r3, r2
	ldr r3, [sp, #0x34]
	mov r0, r7
	add r3, r3, r5, lsl #3
	str r4, [sp, #0xc]
	bl NNS_G2dArrangeOBJ1D
	ldr r1, [sp, #0x24]
	add r0, r1, r0
	str r0, [sp, #0x24]
_020BD8CC:
	ldr r0, [sp, #0x24]
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BD8D8: .word _02109414
_020BD8DC: .word 0xFE00FF00
_020BD8E0: .word 0x3FFF3FFF
	arm_func_end NNS_G2dArrangeOBJ1D

	arm_func_start NNSi_G2dTextCanvasDrawString
NNSi_G2dTextCanvasDrawString: ; 0x020BD8E4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x10
	mov fp, r0
	ldr r4, [fp, #8]
	ldr r5, [fp, #4]
	ldr r6, [sp, #0x38]
	add r0, sp, #0xc
	str r6, [sp, #0xc]
	ldr r6, [r5, #4]
	mov sl, r1
	mov sb, r2
	str r3, [sp, #8]
	ldrsb r8, [sp, #0x41]
	blx r6
	cmp r0, #0
	beq _020BD96C
	ldrsb r7, [sp, #0x40]
_020BD928:
	cmp r0, #0xa
	beq _020BD96C
	ldr r1, [sp, #8]
	mov r2, sl
	str r1, [sp]
	str r0, [sp, #4]
	ldr r0, [fp]
	mov r1, r5
	mov r3, sb
	bl NNS_G2dCharCanvasDrawChar
	add r0, r4, r0
	mla sl, r0, r7, sl
	mla sb, r0, r8, sb
	add r0, sp, #0xc
	blx r6
	cmp r0, #0
	bne _020BD928
_020BD96C:
	ldr r1, [sp, #0x3c]
	cmp r1, #0
	addeq sp, sp, #0x10
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r0, #0xa
	ldreq r1, [sp, #0xc]
	ldr r0, [sp, #0x3c]
	movne r1, #0
	str r1, [r0]
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNSi_G2dTextCanvasDrawString

	arm_func_start NNSi_G2dTextCanvasDrawTextAlign
NNSi_G2dTextCanvasDrawTextAlign: ; 0x020BD998
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x34
	mov sl, r0
	ldr r0, [sl, #4]
	ldr r4, [sp, #0x60]
	ldr r0, [r0]
	ldrsb r8, [sp, #0x65]
	ldrsb r0, [r0, #1]
	ldr r5, [sl, #0xc]
	ldrsb sb, [sp, #0x64]
	add r5, r5, r0
	rsb r0, r8, #0
	mul r0, r5, r0
	str r0, [sp, #0x20]
	mul r0, r5, sb
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x58]
	str r1, [sp, #0xc]
	str r2, [sp, #0x10]
	mov fp, r3
	str r4, [sp, #0x30]
	cmp r4, #0
	str r0, [sp, #0x58]
	addeq sp, sp, #0x34
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r2, [sp, #0x5c]
	mov r6, #0
	and r0, r2, #0x800
	add r1, fp, #1
	str r0, [sp, #0x18]
	add r0, r1, r1, lsr #31
	str r0, [sp, #0x2c]
	and r0, r2, #0x400
	str r0, [sp, #0x14]
	ldrb r0, [sp, #0x64]
	mov r7, r6
	str r0, [sp, #0x24]
	ldrb r0, [sp, #0x65]
	str r0, [sp, #0x28]
_020BDA34:
	ldr r0, [sp, #0x18]
	cmp r0, #0
	ldr r0, [sp, #0xc]
	add r4, r0, r6
	ldr r0, [sp, #0x10]
	add r5, r0, r7
	beq _020BDA70
	ldr r2, [sp, #0x30]
	mov r3, #0
	ldmib sl, {r0, r1}
	bl NNSi_G2dFontGetStringWidth
	sub r0, fp, r0
	mla r4, r0, sb, r4
	mla r5, r0, r8, r5
	b _020BDAA8
_020BDA70:
	ldr r0, [sp, #0x14]
	cmp r0, #0
	beq _020BDAA8
	ldr r2, [sp, #0x30]
	mov r3, #0
	ldmib sl, {r0, r1}
	bl NNSi_G2dFontGetStringWidth
	add r0, r0, #1
	add r0, r0, r0, lsr #31
	mov r1, r0, asr #1
	ldr r0, [sp, #0x2c]
	rsb r0, r1, r0, asr #1
	mla r4, r0, sb, r4
	mla r5, r0, r8, r5
_020BDAA8:
	ldr r3, [sp, #0x30]
	mov r1, r4
	str r3, [sp]
	add r3, sp, #0x30
	str r3, [sp, #4]
	ldr r3, [sp, #0x24]
	mov r2, r5
	strb r3, [sp, #8]
	ldr r3, [sp, #0x28]
	mov r0, sl
	strb r3, [sp, #9]
	ldr r3, [sp, #0x58]
	bl NNSi_G2dTextCanvasDrawString
	ldr r0, [sp, #0x20]
	add r6, r6, r0
	ldr r0, [sp, #0x1c]
	add r7, r7, r0
	ldr r0, [sp, #0x30]
	cmp r0, #0
	bne _020BDA34
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNSi_G2dTextCanvasDrawTextAlign

	arm_func_start NNSi_G2dTextCanvasDrawText
NNSi_G2dTextCanvasDrawText: ; 0x020BDB00
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x28
	ldr r4, [sp, #0x44]
	mov r8, r0
	str r4, [sp]
	add r0, sp, #0x18
	mov r7, r1
	mov r6, r2
	mov r5, r3
	ldmib r8, {r1, r2, r3}
	ldr r4, [sp, #0x40]
	bl NNSi_G2dFontGetTextRect
	ldr r2, [sp, #0x18]
	ldr r0, [sp, #0x1c]
	str r2, [sp, #0x10]
	str r0, [sp, #0x14]
	str r2, [sp, #0x20]
	str r0, [sp, #0x24]
	tst r4, #0x10
	beq _020BDB74
	add r0, r2, #1
	rsb r0, r0, #0
	add r0, r0, r0, lsr #31
	ldrsb r1, [sp, #0x48]
	mov r2, r0, asr #1
	ldrsb r0, [sp, #0x49]
	mla r7, r2, r1, r7
	mla r6, r2, r0, r6
	b _020BDB90
_020BDB74:
	tst r4, #0x20
	beq _020BDB90
	ldrsb r1, [sp, #0x48]
	ldrsb r0, [sp, #0x49]
	rsb r2, r2, #0
	mla r7, r2, r1, r7
	mla r6, r2, r0, r6
_020BDB90:
	tst r4, #2
	beq _020BDBC4
	ldr r0, [sp, #0x24]
	ldrsb r2, [sp, #0x49]
	add r0, r0, #1
	rsb r0, r0, #0
	add r0, r0, r0, lsr #31
	ldrsb r1, [sp, #0x48]
	mov r3, r0, asr #1
	rsb r0, r2, #0
	mla r7, r3, r0, r7
	mla r6, r3, r1, r6
	b _020BDBE8
_020BDBC4:
	tst r4, #4
	beq _020BDBE8
	ldrsb r1, [sp, #0x49]
	ldr r2, [sp, #0x24]
	ldrsb r0, [sp, #0x48]
	rsb r2, r2, #0
	rsb r1, r1, #0
	mla r7, r2, r1, r7
	mla r6, r2, r0, r6
_020BDBE8:
	str r5, [sp]
	ldr r1, [sp, #0x44]
	str r4, [sp, #4]
	ldrb r0, [sp, #0x48]
	str r1, [sp, #8]
	ldrb r4, [sp, #0x49]
	strb r0, [sp, #0xc]
	ldr r3, [sp, #0x20]
	mov r0, r8
	mov r1, r7
	mov r2, r6
	strb r4, [sp, #0xd]
	bl NNSi_G2dTextCanvasDrawTextAlign
	add sp, sp, #0x28
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G2dTextCanvasDrawText

	arm_func_start NNSi_G2dTextCanvasDrawTextRect
NNSi_G2dTextCanvasDrawTextRect: ; 0x020BDC24
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x10
	ldr ip, [sp, #0x30]
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	tst ip, #0x100
	beq _020BDC78
	ldr r0, [r7, #4]
	ldr r1, [r7, #0xc]
	ldr r2, [sp, #0x34]
	bl NNSi_G2dFontGetTextHeight
	ldrsb r2, [sp, #0x39]
	ldr r3, [sp, #0x28]
	ldrsb r1, [sp, #0x38]
	sub r3, r3, r0
	rsb r0, r2, #0
	mla r6, r3, r0, r6
	mla r5, r3, r1, r5
	b _020BDCC0
_020BDC78:
	tst ip, #0x80
	beq _020BDCC0
	ldr r0, [r7, #4]
	ldr r1, [r7, #0xc]
	ldr r2, [sp, #0x34]
	bl NNSi_G2dFontGetTextHeight
	ldr r1, [sp, #0x28]
	add r0, r0, #1
	ldrsb r3, [sp, #0x39]
	add ip, r1, #1
	add r1, r0, r0, lsr #31
	ldrsb r2, [sp, #0x38]
	add r0, ip, ip, lsr #31
	mov r1, r1, asr #1
	rsb r1, r1, r0, asr #1
	rsb r0, r3, #0
	mla r6, r1, r0, r6
	mla r5, r1, r2, r5
_020BDCC0:
	ldr r1, [sp, #0x2c]
	ldr r0, [sp, #0x30]
	str r1, [sp]
	ldr r1, [sp, #0x34]
	str r0, [sp, #4]
	ldrb r0, [sp, #0x38]
	str r1, [sp, #8]
	ldrb ip, [sp, #0x39]
	strb r0, [sp, #0xc]
	mov r0, r7
	mov r1, r6
	mov r2, r5
	mov r3, r4
	strb ip, [sp, #0xd]
	bl NNSi_G2dTextCanvasDrawTextAlign
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNSi_G2dTextCanvasDrawTextRect

	arm_func_start NNSi_G2dGetUnpackedFont
NNSi_G2dGetUnpackedFont: ; 0x020BDD04
	stmdb sp!, {r4, r5, r6, lr}
	movs r6, r0
	mov r5, r1
	mov r4, #0
	beq _020BDD7C
	cmp r6, #0
	beq _020BDD34
	ldr r1, [r6]
	ldr r0, _020BDE4C ; =0x4E465452
	cmp r1, r0
	moveq r0, #1
	beq _020BDD38
_020BDD34:
	mov r0, #0
_020BDD38:
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _020BDD80
	cmp r6, #0
	beq _020BDD68
	ldrh r1, [r6, #6]
	ldr r0, _020BDE50 ; =0x00000101
	cmp r1, r0
	movhs r0, #1
	bhs _020BDD6C
_020BDD68:
	mov r0, #0
_020BDD6C:
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	b _020BDD80
_020BDD7C:
	mov r0, r4
_020BDD80:
	cmp r0, #0
	bne _020BDE00
	cmp r6, #0
	beq _020BDDEC
	beq _020BDDA8
	ldr r1, [r6]
	ldr r0, _020BDE4C ; =0x4E465452
	cmp r1, r0
	moveq r0, #1
	beq _020BDDAC
_020BDDA8:
	mov r0, #0
_020BDDAC:
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _020BDDF0
	cmp r6, #0
	beq _020BDDD8
	ldrh r0, [r6, #6]
	cmp r0, #0x100
	movhs r0, #1
	bhs _020BDDDC
_020BDDD8:
	mov r0, #0
_020BDDDC:
	cmp r0, #0
	movne r0, #1
	moveq r0, #0
	b _020BDDF0
_020BDDEC:
	mov r0, #0
_020BDDF0:
	cmp r0, #0
	bne _020BDDFC
	bl OS_Terminate
_020BDDFC:
	mov r4, #1
_020BDE00:
	mov r0, r6
	bl NNSi_G2dUnpackNFT
	ldr r1, _020BDE54 ; =0x46494E46
	mov r0, r6
	bl NNS_G2dFindBinaryBlock
	cmp r0, #0
	moveq r0, #0
	streq r0, [r5]
	ldmeqia sp!, {r4, r5, r6, pc}
	add r0, r0, #8
	str r0, [r5]
	cmp r4, #0
	beq _020BDE44
	ldr r0, [r5]
	mov r1, #0
	ldr r0, [r0, #8]
	strb r1, [r0, #7]
_020BDE44:
	mov r0, #1
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020BDE4C: .word 0x4E465452
_020BDE50: .word 0x00000101
_020BDE54: .word 0x46494E46
	arm_func_end NNSi_G2dGetUnpackedFont
