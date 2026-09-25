	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public ClearArea1D
	.public ClearAreaLine
	.public ClearContinuous
	.public ClearLine
	.public DrawGlyph1D
	.public DrawGlyphLine
	.public InitCharCanvas
	.public NNS_G2dCharCanvasDrawChar
	.public NNS_G2dRndCoreDrawCellVramTransfer
	.public NNSi_G2dFontGetStringWidth
	.public NNSi_G2dFontGetTextHeight
	.public NNSi_G2dFontGetTextRect
	.public OBJSizeToShape
	.extern DrawCellToSurface2D_
	.extern DrawCellToSurface3D_
	.extern NNSi_G2dBitReaderRead
	.extern NNSi_G2dGetCellTransferState
	.extern NNSi_G2dGetUnpackedFont
	.extern NNSi_G2dSplitCharUTF16
	.extern _02109414
	.extern _02109434
	.extern _021DA498

.text
	arm_func_start NNS_G2dRndCoreDrawCellVramTransfer
NNS_G2dRndCoreDrawCellVramTransfer: ; 0x020BC1D4
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020BC2B8 ; =_021DA498
	mov r6, r0
	ldr r3, [r2]
	mov r4, r1
	ldr r5, [r3]
	ldr r0, [r5, #0x10]
	cmp r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, #1
	str r0, [r3, #0x30]
	ldr r3, [r5, #0x18]
	cmp r3, #0
	beq _020BC218
	ldr r0, [r2]
	mov r1, r6
	blx r3
_020BC218:
	ldr r0, _020BC2B8 ; =_021DA498
	ldr r0, [r0]
	ldr r0, [r0, #0x30]
	cmp r0, #0
	beq _020BC298
	mvn r0, #0
	cmp r4, r0
	beq _020BC25C
	mov r0, r4
	ldr r4, [r5, #0x14]
	bl NNSi_G2dGetCellTransferState
	mov r3, #1
	mvn r1, r3, lsl r4
	ldr r2, [r0, #0x20]
	and r1, r2, r1
	orr r1, r1, r3, lsl r4
	str r1, [r0, #0x20]
_020BC25C:
	ldr r0, [r5, #0x14]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _020BC298
_020BC26C: ; jump table
	b _020BC27C ; case 0
	b _020BC28C ; case 1
	b _020BC28C ; case 2
	b _020BC298 ; case 3
_020BC27C:
	mov r0, r5
	mov r1, r6
	bl DrawCellToSurface3D_
	b _020BC298
_020BC28C:
	mov r0, r5
	mov r1, r6
	bl DrawCellToSurface2D_
_020BC298:
	ldr r2, [r5, #0x1c]
	cmp r2, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, _020BC2B8 ; =_021DA498
	mov r1, r6
	ldr r0, [r0]
	blx r2
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020BC2B8: .word _021DA498
	arm_func_end NNS_G2dRndCoreDrawCellVramTransfer

	arm_func_start GetGlyphIndex
GetGlyphIndex: ; 0x020BC2BC
	stmdb sp!, {r3, lr}
	ldrh r3, [r0, #4]
	ldr r2, _020BC380 ; =0x0000FFFF
	cmp r3, #0
	beq _020BC2E4
	cmp r3, #1
	beq _020BC300
	cmp r3, #2
	beq _020BC314
	b _020BC378
_020BC2E4:
	ldrh r2, [r0]
	ldrh r3, [r0, #0xc]
	sub r0, r1, r2
	add r0, r3, r0
	mov r0, r0, lsl #0x10
	mov r2, r0, lsr #0x10
	b _020BC378
_020BC300:
	ldrh r2, [r0]
	sub r1, r1, r2
	add r0, r0, r1, lsl #1
	ldrh r2, [r0, #0xc]
	b _020BC378
_020BC314:
	ldrh r3, [r0, #0xc]
	add ip, r0, #0xe
	sub r0, r3, #1
	add lr, ip, r0, lsl #2
	cmp ip, lr
	bhi _020BC378
_020BC32C:
	sub r3, lr, ip
	mov r0, r3, asr #1
	add r0, r3, r0, lsr #30
	mov r0, r0, asr #2
	add r0, r0, r0, lsr #31
	mov r3, r0, asr #1
	mov r0, r3, lsl #2
	ldrh r0, [ip, r0]
	add r3, ip, r3, lsl #2
	cmp r0, r1
	addlo ip, r3, #4
	blo _020BC370
	cmp r1, r0
	sublo lr, r3, #4
	blo _020BC370
	ldrh r2, [r3, #2]
	b _020BC378
_020BC370:
	cmp ip, lr
	bls _020BC32C
_020BC378:
	mov r0, r2
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BC380: .word 0x0000FFFF
	arm_func_end GetGlyphIndex

	arm_func_start NNS_G2dFontInitUTF16
NNS_G2dFontInitUTF16: ; 0x020BC384
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, r1
	mov r1, r4
	bl NNSi_G2dGetUnpackedFont
	ldr r0, _020BC3A4 ; =NNSi_G2dSplitCharUTF16
	str r0, [r4, #4]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020BC3A4: .word NNSi_G2dSplitCharUTF16
	arm_func_end NNS_G2dFontInitUTF16

	arm_func_start NNS_G2dFontFindGlyphIndex
NNS_G2dFontFindGlyphIndex: ; 0x020BC3A8
	stmdb sp!, {r3, lr}
	ldr r0, [r0]
	ldr r0, [r0, #0x10]
	cmp r0, #0
	beq _020BC3E4
_020BC3BC:
	ldrh r2, [r0]
	cmp r2, r1
	ldrlsh r2, [r0, #2]
	cmpls r1, r2
	bhi _020BC3D8
	bl GetGlyphIndex
	ldmia sp!, {r3, pc}
_020BC3D8:
	ldr r0, [r0, #8]
	cmp r0, #0
	bne _020BC3BC
_020BC3E4:
	ldr r0, _020BC3EC ; =0x0000FFFF
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BC3EC: .word 0x0000FFFF
	arm_func_end NNS_G2dFontFindGlyphIndex

	arm_func_start NNS_G2dFontGetCharWidthsFromIndex
NNS_G2dFontGetCharWidthsFromIndex: ; 0x020BC3F0
	ldr ip, [r0]
	ldr r3, [ip, #0xc]
	cmp r3, #0
	beq _020BC434
_020BC400:
	ldrh r2, [r3]
	cmp r2, r1
	ldrlsh r0, [r3, #2]
	cmpls r1, r0
	bhi _020BC428
	sub r0, r1, r2
	add r1, r3, #8
	add r0, r0, r0, lsl #1
	add r0, r1, r0
	bx lr
_020BC428:
	ldr r3, [r3, #4]
	cmp r3, #0
	bne _020BC400
_020BC434:
	add r0, ip, #4
	bx lr
	arm_func_end NNS_G2dFontGetCharWidthsFromIndex

	arm_func_start NNSi_G2dFontGetStringWidth
NNSi_G2dFontGetStringWidth: ; 0x020BC43C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #4
	mov r8, r0
	str r2, [sp]
	ldr r5, [r8, #4]
	add r0, sp, #0
	mov r7, r1
	mov r6, r3
	mov r4, #0
	blx r5
	movs r1, r0
	beq _020BC4B8
	ldr sb, _020BC4E4 ; =0x0000FFFF
	add sl, sp, #0
_020BC474:
	cmp r1, #0xa
	beq _020BC4B8
	mov r0, r8
	bl NNS_G2dFontFindGlyphIndex
	mov r1, r0
	cmp r1, sb
	ldreq r0, [r8]
	ldreqh r1, [r0, #2]
	mov r0, r8
	bl NNS_G2dFontGetCharWidthsFromIndex
	ldrsb r1, [r0, #2]
	mov r0, sl
	add r1, r7, r1
	add r4, r4, r1
	blx r5
	movs r1, r0
	bne _020BC474
_020BC4B8:
	cmp r6, #0
	beq _020BC4D0
	cmp r1, #0xa
	ldreq r0, [sp]
	movne r0, #0
	str r0, [r6]
_020BC4D0:
	cmp r4, #0
	subgt r4, r4, r7
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020BC4E4: .word 0x0000FFFF
	arm_func_end NNSi_G2dFontGetStringWidth

	arm_func_start NNSi_G2dFontGetTextHeight
NNSi_G2dFontGetTextHeight: ; 0x020BC4E8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0xc
	add r4, sp, #0
	mov r3, #0
	mov r8, r0
	str r2, [sp, #8]
	str r3, [r4]
	str r3, [r4, #4]
	ldr r7, [r8, #4]
	add r0, sp, #8
	mov r4, r1
	mov r6, #1
	blx r7
	cmp r0, #0
	beq _020BC540
	add r5, sp, #8
_020BC528:
	cmp r0, #0xa
	mov r0, r5
	addeq r6, r6, #1
	blx r7
	cmp r0, #0
	bne _020BC528
_020BC540:
	ldr r0, [r8]
	ldrsb r0, [r0, #1]
	add r0, r4, r0
	mul r0, r6, r0
	sub r0, r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G2dFontGetTextHeight

	arm_func_start NNSi_G2dFontGetTextRect
NNSi_G2dFontGetTextRect: ; 0x020BC55C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #8
	mov r6, r2
	ldr r2, [sp, #0x28]
	add sb, sp, #0
	mov r4, #0
	mov r8, r0
	mov r7, r1
	mov r5, r3
	str r4, [sb]
	str r4, [sb, #4]
	cmp r2, #0
	mov r4, #1
	beq _020BC5C4
	add sb, sp, #0x28
_020BC598:
	mov r0, r7
	mov r1, r6
	mov r3, sb
	bl NNSi_G2dFontGetStringWidth
	ldr r1, [sp]
	ldr r2, [sp, #0x28]
	cmp r0, r1
	strgt r0, [sp]
	add r4, r4, #1
	cmp r2, #0
	bne _020BC598
_020BC5C4:
	ldr r0, [r7]
	sub r2, r4, #1
	ldrsb r1, [r0, #1]
	ldr r0, [sp]
	add r1, r5, r1
	mul r1, r2, r1
	sub r1, r1, r5
	str r1, [sp, #4]
	stmia r8, {r0, r1}
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end NNSi_G2dFontGetTextRect

	arm_func_start GetCharIndex1D
GetCharIndex1D: ; 0x020BC5F0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov sl, #3
	ldr r7, [sp, #0x20]
	ldr r6, [sp, #0x24]
	ldr r8, _020BC6E4 ; =_02109414
	mov r5, #0
	mov sb, sl
	mvn r4, #0
_020BC610:
	and ip, r3, r4, lsl r6
	cmp ip, r1
	and lr, r2, r4, lsl r7
	bhi _020BC654
	mla r5, r2, ip, r5
	cmp lr, r0
	bhi _020BC644
	sub r3, r3, ip
	mla r5, lr, r3, r5
	sub r0, r0, lr
	sub r1, r1, ip
	sub r2, r2, lr
	b _020BC69C
_020BC644:
	mov r2, lr
	sub r1, r1, ip
	sub r3, r3, ip
	b _020BC69C
_020BC654:
	cmp lr, r0
	mvn r3, r4, lsl r6
	bhi _020BC674
	mla r5, lr, ip, r5
	mov r3, ip
	sub r0, r0, lr
	sub r2, r2, lr
	b _020BC69C
_020BC674:
	and r2, r1, r4, lsl r6
	mla r5, lr, r2, r5
	and r2, r0, r4, lsl r7
	mvn r4, r4, lsl r7
	add r2, r5, r2, lsl r6
	and r1, r1, r3
	add r1, r2, r1, lsl r7
	and r0, r0, r4
	add r0, r1, r0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020BC69C:
	cmp r2, #8
	movge r6, sl
	movlt r6, r2
	clzlt r6, r6
	rsblt r6, r6, #0x1f
	cmp r3, #8
	movge r7, sb
	movlt r7, r3
	clzlt r7, r7
	rsblt r7, r7, #0x1f
	add r7, r8, r7, lsl #3
	add ip, r7, r6, lsl #1
	ldrb r7, [r7, r6, lsl #1]
	ldrb r6, [ip, #1]
	str r7, [sp, #0x20]
	str r6, [sp, #0x24]
	b _020BC610
_020BC6E0: ; 0x020BC6E0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020BC6E4: .word _02109414
	arm_func_end GetCharIndex1D

	arm_func_start OBJSizeToShape
OBJSizeToShape: ; 0x020BC6E8
	ldrb r3, [r0, #1]
	ldr r2, _020BC700 ; =_02109434
	ldrb r1, [r0]
	add r0, r2, r3, lsl #4
	ldr r0, [r0, r1, lsl #2]
	bx lr
	.align 2, 0
_020BC700: .word _02109434
	arm_func_end OBJSizeToShape

	arm_func_start ClearChar
ClearChar: ; 0x020BC704
	stmdb sp!, {r3, r4, r5, lr}
	cmp r3, #8
	ldreq r4, [sp, #0x10]
	mov ip, r0
	ldr r0, [sp, #0x14]
	cmpeq r4, #8
	bne _020BC734
	ldr r2, [sp, #0x18]
	mov r1, ip
	mov r2, r2, lsl #3
	bl MIi_CpuClearFast
	ldmia sp!, {r3, r4, r5, pc}
_020BC734:
	ldr r4, [sp, #0x18]
	cmp r4, #4
	bne _020BC794
	mov r5, r1, lsl #2
	add r4, r5, r3, lsl #2
	mvn r3, #0
	rsb r4, r4, #0x20
	mov r3, r3, lsr r5
	add r1, r4, r1, lsl #2
	mov r3, r3, lsl r1
	ldr r1, [sp, #0x10]
	add r2, ip, r2, lsl #2
	add r5, r2, r1, lsl #2
	and r1, r0, r3, lsr r4
	mvn r3, r3, lsr r4
	cmp r2, r5
	ldmhsia sp!, {r3, r4, r5, pc}
_020BC778:
	ldr r0, [r2]
	and r0, r0, r3
	orr r0, r1, r0
	str r0, [r2], #4
	cmp r2, r5
	blo _020BC778
	ldmia sp!, {r3, r4, r5, pc}
_020BC794:
	mov r1, r1, lsl #3
	mvn r4, #0
	add r3, r1, r3, lsl #3
	rsb r3, r3, #0x40
	mov r5, r4, lsr r1
	cmp r3, #0x20
	movlo r5, r5, lsl r1
	blo _020BC7C4
	sub lr, r3, #0x20
	add r4, r1, lr
	mov r4, r5, lsl r4
	mov r5, r4, lsr lr
_020BC7C4:
	mvn lr, #0
	mov r4, lr, lsl r3
	cmp r1, #0x20
	movlo r3, r4, lsr r3
	blo _020BC7E8
	sub lr, r1, #0x20
	add r1, lr, r3
	mov r1, r4, lsr r1
	mov r3, r1, lsl lr
_020BC7E8:
	ldr r1, [sp, #0x10]
	add r4, ip, r2, lsl #3
	add ip, r4, r1, lsl #3
	and r1, r0, r5
	and r2, r0, r3
	mvn r5, r5
	mvn r3, r3
	cmp r4, ip
	ldmhsia sp!, {r3, r4, r5, pc}
_020BC80C:
	ldr r0, [r4]
	and r0, r0, r5
	orr r0, r1, r0
	str r0, [r4]
	ldr r0, [r4, #4]
	and r0, r0, r3
	orr r0, r2, r0
	str r0, [r4, #4]
	add r4, r4, #8
	cmp r4, ip
	blo _020BC80C
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ClearChar

	arm_func_start LetterChar
LetterChar: ; 0x020BC83C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	ldr r5, [r0, #8]
	ldr r4, [r0, #0xc]
	cmp r5, #0
	strge r5, [sp]
	movlt r1, #0
	strlt r1, [sp]
	ldr r1, [r0, #0x10]
	cmp r4, #0
	add sl, r5, r1
	ldr r1, [r0, #0x14]
	movge r2, r4
	movlt r2, #0
	cmp sl, #8
	add r3, r4, r1
	movge sl, #8
	cmp r3, #8
	movge r3, #8
	cmp r4, #0
	movgt r4, #0
	cmp r5, #0
	ldr r7, [r0, #0x20]
	movgt r5, #0
	rsb r1, r4, #0
	mul r6, sl, r7
	ldr r8, [r0, #0x1c]
	rsb r4, r5, #0
	mul sb, r8, r4
	ldr r4, [r0, #0x18]
	mov sl, r6
	str r4, [sp, #0x10]
	ldr r4, [sp]
	cmp r7, #4
	mul r5, r4, r7
	ldr r4, [sp, #0x10]
	str r5, [sp]
	mla r4, r1, r4, sb
	ldr r1, [r0, #4]
	str r1, [sp, #0x14]
	ldr r1, [r0]
	bne _020BC9AC
	ldr r7, [r0, #0x24]
	add r0, r1, r2, lsl #2
	str r0, [sp, #0xc]
	add r0, r1, r3, lsl #2
	ldr r1, [sp, #0xc]
	str r0, [sp, #8]
	cmp r1, r0
	addhs sp, sp, #0x28
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov fp, #0xf
	add r5, sp, #0x20
_020BC910:
	ldr r0, [sp, #0xc]
	mov r1, r4, lsr #0x1f
	ldr sb, [r0]
	ldr r0, [sp, #0x14]
	rsb r2, r1, r4, lsl #29
	add r0, r0, r4, lsr #3
	str r0, [sp, #0x20]
	mov r0, #0
	strb r0, [sp, #0x24]
	strb r0, [sp, #0x25]
	mov r0, r5
	add r1, r1, r2, ror #29
	bl NNSi_G2dBitReaderRead
	ldr r6, [sp]
	mov r0, r6
	cmp r0, sl
	bhs _020BC984
_020BC954:
	mov r0, r5
	mov r1, r8
	bl NNSi_G2dBitReaderRead
	cmp r0, #0
	beq _020BC978
	add r0, r7, r0
	mvn r1, fp, lsl r6
	and r1, sb, r1
	orr sb, r1, r0, lsl r6
_020BC978:
	add r6, r6, #4
	cmp r6, sl
	blo _020BC954
_020BC984:
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #8]
	str sb, [r1], #4
	cmp r1, r0
	ldr r0, [sp, #0x10]
	str r1, [sp, #0xc]
	add r4, r4, r0
	blo _020BC910
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BC9AC:
	ldr r5, [r0, #0x24]
	add r0, r1, r3, lsl #3
	add fp, r1, r2, lsl #3
	str r0, [sp, #4]
	cmp fp, r0
	addhs sp, sp, #0x28
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BC9C8:
	mov r1, r4, lsr #0x1f
	rsb r0, r1, r4, lsl #29
	add r1, r1, r0, ror #29
	ldr r0, [sp, #0x14]
	ldmia fp, {r6, r7}
	mov r2, #0
	add r0, r0, r4, lsr #3
	str r0, [sp, #0x18]
	add r0, sp, #0x18
	strb r2, [sp, #0x1c]
	strb r2, [sp, #0x1d]
	bl NNSi_G2dBitReaderRead
	ldr sb, [sp]
	mov r0, sb
	cmp r0, sl
	bhs _020BCA58
_020BCA08:
	add r0, sp, #0x18
	mov r1, r8
	bl NNSi_G2dBitReaderRead
	cmp r0, #0
	beq _020BCA4C
	add r1, r5, r0
	cmp sb, #0x20
	mov r0, #0xff
	bhs _020BCA3C
	mvn r0, r0, lsl sb
	and r0, r6, r0
	orr r6, r0, r1, lsl sb
	b _020BCA4C
_020BCA3C:
	sub r2, sb, #0x20
	mvn r0, r0, lsl r2
	and r0, r7, r0
	orr r7, r0, r1, lsl r2
_020BCA4C:
	add sb, sb, #8
	cmp sb, sl
	blo _020BCA08
_020BCA58:
	ldr r0, [sp, #4]
	stmia fp, {r6, r7}
	add fp, fp, #8
	cmp fp, r0
	ldr r0, [sp, #0x10]
	add r4, r4, r0
	blo _020BC9C8
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end LetterChar

	arm_func_start DrawGlyphLine
DrawGlyphLine: ; 0x020BCA7C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x2c
	ldr r4, [sp, #0x54]
	ldrb r6, [r0, #0xc]
	str r4, [sp, #0x54]
	ldr r4, [r4]
	ldr r5, [r1]
	mov r7, r6, lsl #6
	ldrb r4, [r4, #1]
	mov r6, r7, asr #2
	add r6, r7, r6, lsr #29
	cmp r4, #0
	ldr r5, [r5, #8]
	mov sl, r2
	mov sb, r3
	mov r8, r6, asr #3
	addeq sp, sp, #0x2c
	ldmib r0, {fp, ip}
	ldr lr, [r0]
	ldrb r2, [r5, #1]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	adds r7, sl, r4
	addmi sp, sp, #0x2c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	adds r3, sb, r2
	addmi sp, sp, #0x2c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp sl, #0
	movle r5, #0
	movgt r5, sl, lsr #3
	cmp sb, #0
	movle r6, #0
	add r7, r7, #7
	movgt r6, sb, lsr #3
	cmp fp, r7, lsr #3
	mov r7, r7, lsr #3
	add r3, r3, #7
	movls r7, fp
	mov fp, r3, lsr #3
	cmp ip, r3, lsr #3
	movls fp, ip
	subs r3, r7, r5
	sub fp, fp, r6
	addmi sp, sp, #0x2c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp fp, #0
	addlt sp, sp, #0x2c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r7, [r0, #0x10]
	cmp sl, #0
	mla r6, r7, r6, r5
	sub r5, r7, r3
	mul r5, r8, r5
	str r5, [sp]
	ldr r5, [sp, #0x54]
	mla r7, r8, r6, lr
	ldr ip, [r5, #4]
	ldr r6, [sp, #0x50]
	str ip, [sp, #8]
	sub r5, r6, #1
	str r4, [sp, #0x14]
	str r5, [sp, #0x28]
	str r2, [sp, #0x18]
	ldr r2, [r1]
	andge sl, sl, #7
	ldr r2, [r2, #8]
	cmp sb, #0
	ldrb r2, [r2, #6]
	andge sb, sb, #7
	sub fp, sb, fp, lsl #3
	str r2, [sp, #0x20]
	ldrb r0, [r0, #0xc]
	cmp sb, fp
	sub r6, sl, r3, lsl #3
	str r0, [sp, #0x24]
	ldr r0, [r1]
	ldr r0, [r0, #8]
	ldrb r0, [r0]
	smulbb r0, r2, r0
	str r0, [sp, #0x1c]
	addle sp, sp, #0x2c
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r4, sp, #4
_020BCBC8:
	mov r5, sl
	str sb, [sp, #0x10]
	cmp sl, r6
	ble _020BCBF8
_020BCBD8:
	mov r0, r4
	str r7, [sp, #4]
	str r5, [sp, #0xc]
	bl LetterChar
	sub r5, r5, #8
	cmp r5, r6
	add r7, r7, r8
	bgt _020BCBD8
_020BCBF8:
	ldr r0, [sp]
	sub sb, sb, #8
	cmp sb, fp
	add r7, r7, r0
	bgt _020BCBC8
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end DrawGlyphLine

	arm_func_start DrawGlyph1D
DrawGlyph1D: ; 0x020BCC14
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x4c
	ldr r6, [sp, #0x74]
	ldrb r5, [r0, #0xc]
	ldr r4, [r6]
	ldr r7, [r1]
	mov r8, r5, lsl #6
	ldrb r4, [r4, #1]
	mov r5, r8, asr #2
	add r5, r8, r5, lsr #29
	ldr r7, [r7, #8]
	str r2, [sp, #8]
	str r3, [sp, #0xc]
	cmp r4, #0
	mov sb, r5, asr #3
	addeq sp, sp, #0x4c
	ldmib r0, {r3, r5}
	ldrb r2, [r7, #1]
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r7, [sp, #8]
	adds r7, r7, r4
	addmi sp, sp, #0x4c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r8, [sp, #0xc]
	adds r8, r8, r2
	addmi sp, sp, #0x4c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr sl, [sp, #8]
	add r7, r7, #7
	cmp sl, #0
	movle sl, #0
	strle sl, [sp, #0x10]
	movgt sl, sl, lsr #3
	strgt sl, [sp, #0x10]
	ldr sl, [sp, #0xc]
	cmp sl, #0
	movle sl, #0
	movgt sl, sl, lsr #3
	cmp r3, r7, lsr #3
	mov r7, r7, lsr #3
	movls r7, r3
	add r3, r8, #7
	cmp r5, r3, lsr #3
	mov r8, r3, lsr #3
	ldr r3, [sp, #0x10]
	movls r8, r5
	subs r5, r7, r3
	sub r7, r8, sl
	addmi sp, sp, #0x4c
	ldmmiia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r7, #0
	addlt sp, sp, #0x4c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r3, [sp, #8]
	ldr fp, [r0]
	cmp r3, #0
	andge r3, r3, #7
	strge r3, [sp, #8]
	ldr r3, [sp, #0xc]
	ldr r8, [sp, #8]
	ldr r6, [r6, #4]
	cmp r3, #0
	andge r3, r3, #7
	strge r3, [sp, #0xc]
	ldr r3, [sp, #0x70]
	sub r5, r8, r5, lsl #3
	ldr r8, [sp, #0xc]
	sub r3, r3, #1
	str r3, [sp, #0x48]
	sub r7, r8, r7, lsl #3
	str r2, [sp, #0x38]
	str r6, [sp, #0x28]
	str r4, [sp, #0x34]
	ldr r4, [r1]
	mov r3, r8
	mov r2, r7
	cmp r3, r2
	ldr r2, [r4, #8]
	str r7, [sp, #0x1c]
	ldrb r3, [r2, #6]
	str r3, [sp, #0x40]
	ldrb r2, [r0, #0xc]
	str r2, [sp, #0x44]
	ldr r1, [r1]
	ldr r1, [r1, #8]
	ldrb r1, [r1]
	smulbb r1, r3, r1
	str r1, [sp, #0x3c]
	ldr r3, [r0, #0x10]
	ldr r1, [r0, #4]
	ldr r0, [r0, #8]
	str r1, [sp, #0x14]
	mov r2, r3, lsl #0x18
	mov r1, r3, lsl #0x10
	str r3, [sp, #0x20]
	str r0, [sp, #0x18]
	mov r8, r2, lsr #0x18
	mov r7, r1, lsr #0x18
	addle sp, sp, #0x4c
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BCDA4:
	ldr r0, [sp, #0xc]
	ldr r6, [sp, #0x10]
	str r0, [sp, #0x30]
	ldr r0, [sp, #8]
	cmp r0, r5
	mov r4, r0
	ble _020BCE00
_020BCDC0:
	ldr r2, [sp, #0x14]
	str r8, [sp]
	ldr r3, [sp, #0x18]
	mov r0, r6
	mov r1, sl
	str r7, [sp, #4]
	bl GetCharIndex1D
	mla r1, r0, sb, fp
	add r0, sp, #0x24
	str r4, [sp, #0x2c]
	str r1, [sp, #0x24]
	bl LetterChar
	sub r4, r4, #8
	add r6, r6, #1
	cmp r4, r5
	bgt _020BCDC0
_020BCE00:
	ldr r0, [sp, #0xc]
	add sl, sl, #1
	sub r1, r0, #8
	ldr r0, [sp, #0x1c]
	str r1, [sp, #0xc]
	cmp r1, r0
	bgt _020BCDA4
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end DrawGlyph1D

	arm_func_start ClearContinuous
ClearContinuous: ; 0x020BCE24
	stmdb sp!, {r3, lr}
	mov r3, r0
	ldrb r2, [r3, #0xc]
	ldr ip, [r3, #4]
	cmp r2, #4
	orreq r0, r1, r1, lsl #4
	orreq r0, r0, r0, lsl #8
	orrne r0, r1, r1, lsl #8
	orr r1, r0, r0, lsl #16
	ldr r0, [r3, #8]
	mov r2, r2, lsl #6
	mul lr, ip, r0
	mov r0, r2, asr #2
	add r0, r2, r0, lsr #29
	mov r2, r0, asr #3
	mov r0, r1
	mul r2, lr, r2
	ldr r1, [r3]
	bl MIi_CpuClearFast
	ldmia sp!, {r3, pc}
	arm_func_end ClearContinuous

	arm_func_start ClearLine
ClearLine: ; 0x020BCE74
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r7, r0
	ldrb r2, [r7, #0xc]
	mov r6, r1
	ldr sb, [r7]
	cmp r2, #4
	orreq r0, r6, r6, lsl #4
	orreq r0, r0, r0, lsl #8
	orrne r0, r6, r6, lsl #8
	mov r1, r2, lsl #6
	orr r6, r0, r0, lsl #16
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r3, r0, asr #3
	ldr r2, [r7, #0x10]
	ldr r1, [r7, #4]
	ldr r0, [r7, #8]
	mul r5, r3, r2
	mul r4, r3, r1
	mov r8, #0
	cmp r0, #0
	ldmleia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_020BCECC:
	mov r0, r6
	mov r1, sb
	mov r2, r4
	bl MIi_CpuClearFast
	ldr r0, [r7, #8]
	add r8, r8, #1
	cmp r8, r0
	add sb, sb, r5
	blt _020BCECC
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ClearLine

	arm_func_start ClearAreaLine
ClearAreaLine: ; 0x020BCEF4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	ldr r4, [sp, #0x58]
	mov sb, r2
	ldrb fp, [r0, #0xc]
	mov sl, r1
	add r1, sb, r4
	str r1, [sp, #0x28]
	ldr r2, [sp, #0x5c]
	mov r1, r3
	add r1, r1, r2
	cmp fp, #4
	str r1, [sp, #0x24]
	orreq r1, sl, sl, lsl #4
	orreq r1, r1, r1, lsl #8
	orrne r1, sl, sl, lsl #8
	str r3, [sp, #0xc]
	orr sl, r1, r1, lsl #16
	ldr r1, [sp, #0xc]
	mov r6, fp, lsl #6
	bic r1, r1, #7
	mov r4, r6, asr #2
	mov r2, r1, asr #2
	str r1, [sp, #0x2c]
	add r1, r1, r2, lsr #29
	ldr r2, [r0, #0x10]
	mov r3, r1, asr #3
	mul r1, r3, r2
	bic r3, sb, #7
	mov r5, r3, asr #2
	add r4, r6, r4, lsr #29
	str r3, [sp, #0x18]
	add r5, r3, r5, lsr #29
	mov r3, r4, asr #3
	str r3, [sp, #0x14]
	add r4, r1, r5, asr #3
	ldr r3, [sp, #0x24]
	ldr r5, [r0]
	ldr r1, [sp, #0x14]
	ldr r0, [sp, #0x28]
	mla r4, r1, r4, r5
	add r3, r3, #7
	bic r1, r3, #7
	str r1, [sp, #0x20]
	ldr r1, [sp, #0x14]
	add r0, r0, #7
	mul r1, r2, r1
	str r1, [sp, #0x10]
	ldr r2, [sp, #0x2c]
	ldr r1, [sp, #0x20]
	str r4, [sp, #0x1c]
	cmp r2, r1
	bic r6, r0, #7
	addge sp, sp, #0x30
	ldmgeia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BCFD0:
	ldr r1, [sp, #0x2c]
	ldr r0, [sp, #0xc]
	cmp r1, r0
	movge r8, #0
	bge _020BCFF0
	mov r1, r0
	ldr r0, [sp, #0x2c]
	sub r8, r1, r0
_020BCFF0:
	ldr r1, [sp, #0x24]
	ldr r0, [sp, #0x2c]
	ldr r7, [sp, #0x1c]
	sub r0, r1, r0
	cmp r0, #8
	movgt r0, #8
	sub r5, r0, r8
	ldr r0, [sp, #0x18]
	cmp r0, r6
	mov r4, r0
	bge _020BD060
_020BD01C:
	ldr r0, [sp, #0x28]
	cmp r4, sb
	sublt r1, sb, r4
	sub r0, r0, r4
	movge r1, #0
	cmp r0, #8
	movgt r0, #8
	sub r3, r0, r1
	mov r0, r7
	mov r2, r8
	stmia sp, {r5, sl, fp}
	bl ClearChar
	ldr r0, [sp, #0x14]
	add r4, r4, #8
	add r7, r7, r0
	cmp r4, r6
	blt _020BD01C
_020BD060:
	ldr r1, [sp, #0x1c]
	ldr r0, [sp, #0x10]
	add r0, r1, r0
	str r0, [sp, #0x1c]
	ldr r0, [sp, #0x2c]
	add r1, r0, #8
	ldr r0, [sp, #0x20]
	str r1, [sp, #0x2c]
	cmp r1, r0
	blt _020BCFD0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ClearAreaLine

	arm_func_start ClearArea1D
ClearArea1D: ; 0x020BD090
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x48
	ldrb r4, [r0, #0xc]
	mov sl, r2
	mov fp, r1
	str r4, [sp, #0x10]
	ldr r1, [sp, #0x10]
	ldr r4, [sp, #0x70]
	cmp r1, #4
	add r1, sl, r4
	str r1, [sp, #0x3c]
	ldr r2, [sp, #0x74]
	mov r1, r3
	add r1, r1, r2
	bic r2, sl, #7
	str r1, [sp, #0x38]
	str r3, [sp, #0xc]
	str r2, [sp, #0x28]
	ldr r2, [sp, #0xc]
	orreq r1, fp, fp, lsl #4
	bic r2, r2, #7
	str r2, [sp, #0x40]
	ldr r2, [sp, #0x10]
	orreq r1, r1, r1, lsl #8
	mov r5, r2, lsl #6
	ldr r2, [sp, #0x38]
	mov r3, r5, asr #2
	add r6, r2, #7
	ldr r2, [sp, #0x28]
	add r3, r5, r3, lsr #29
	mov r4, r2, asr #2
	ldr r2, [sp, #0x40]
	orrne r1, fp, fp, lsl #8
	mov r5, r2, asr #2
	bic r2, r6, #7
	str r2, [sp, #0x34]
	mov r2, r3, asr #3
	str r2, [sp, #0x14]
	ldr r2, [sp, #0x28]
	orr fp, r1, r1, lsl #16
	add r2, r2, r4, lsr #29
	mov r2, r2, asr #3
	str r2, [sp, #0x2c]
	ldr r2, [sp, #0x40]
	ldr r1, [r0, #0x10]
	add r2, r2, r5, lsr #29
	mov r8, r2, asr #3
	ldr r3, [sp, #0x40]
	ldr r2, [sp, #0x34]
	str r1, [sp, #0x44]
	cmp r3, r2
	ldr r2, [sp, #0x3c]
	add r2, r2, #7
	bic r7, r2, #7
	ldr r2, [r0, #4]
	str r2, [sp, #0x18]
	ldr r2, [r0, #8]
	ldr r0, [r0]
	str r2, [sp, #0x1c]
	str r0, [sp, #0x30]
	mov r0, r1, lsl #0x18
	mov r0, r0, lsr #0x18
	str r0, [sp, #0x20]
	mov r0, r1, lsl #0x10
	mov r0, r0, lsr #0x18
	str r0, [sp, #0x24]
	addge sp, sp, #0x48
	ldmgeia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BD1A0:
	ldr r1, [sp, #0x40]
	ldr r0, [sp, #0xc]
	cmp r1, r0
	movge sb, #0
	bge _020BD1C0
	mov r1, r0
	ldr r0, [sp, #0x40]
	sub sb, r1, r0
_020BD1C0:
	ldr r1, [sp, #0x38]
	ldr r0, [sp, #0x40]
	ldr r6, [sp, #0x2c]
	sub r0, r1, r0
	cmp r0, #8
	movgt r0, #8
	sub r5, r0, sb
	ldr r0, [sp, #0x28]
	cmp r0, r7
	mov r4, r0
	bge _020BD260
_020BD1EC:
	ldr r0, [sp, #0x20]
	ldr r2, [sp, #0x18]
	str r0, [sp]
	ldr r0, [sp, #0x24]
	ldr r3, [sp, #0x1c]
	str r0, [sp, #4]
	mov r0, r6
	mov r1, r8
	bl GetCharIndex1D
	ldr r2, [sp, #0x3c]
	cmp r4, sl
	sublt r1, sl, r4
	sub r2, r2, r4
	movge r1, #0
	cmp r2, #8
	movgt r2, #8
	stmia sp, {r5, fp}
	sub r3, r2, r1
	ldr r2, [sp, #0x10]
	ldr ip, [sp, #0x14]
	str r2, [sp, #8]
	ldr r2, [sp, #0x30]
	mla r0, ip, r0, r2
	mov r2, sb
	bl ClearChar
	add r4, r4, #8
	add r6, r6, #1
	cmp r4, r7
	blt _020BD1EC
_020BD260:
	ldr r0, [sp, #0x40]
	add r8, r8, #1
	add r1, r0, #8
	ldr r0, [sp, #0x34]
	str r1, [sp, #0x40]
	cmp r1, r0
	blt _020BD1A0
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ClearArea1D

	arm_func_start InitCharCanvas
InitCharCanvas: ; 0x020BD284
	str r2, [r0, #4]
	ldr r2, [sp]
	str r3, [r0, #8]
	strb r2, [r0, #0xc]
	ldr r2, [sp, #4]
	str r1, [r0]
	ldr r1, [sp, #8]
	str r2, [r0, #0x14]
	str r1, [r0, #0x10]
	bx lr
	arm_func_end InitCharCanvas

	arm_func_start NNS_G2dCharCanvasDrawChar
NNS_G2dCharCanvasDrawChar: ; 0x020BD2AC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	mov r7, r1
	ldrh r1, [sp, #0x2c]
	mov r8, r0
	mov r0, r7
	mov r6, r2
	mov r5, r3
	bl NNS_G2dFontFindGlyphIndex
	ldr r1, _020BD3CC ; =0x0000FFFF
	mov r4, r0
	cmp r4, r1
	ldreq r0, [r7]
	ldreqh r4, [r0, #2]
	mov r0, r7
	mov r1, r4
	bl NNS_G2dFontGetCharWidthsFromIndex
	str r0, [sp, #8]
	ldr r1, [r7]
	ldr r2, [r1, #8]
	ldrh r1, [r2, #2]
	add r2, r2, #8
	mla r1, r4, r1, r2
	str r1, [sp, #0xc]
	ldr r1, [r7]
	ldr r3, [r1, #8]
	ldrb r1, [r3, #7]
	cmp r1, #7
	addls pc, pc, r1, lsl #2
	b _020BD390
_020BD324: ; jump table
	b _020BD344 ; case 0
	b _020BD350 ; case 1
	b _020BD350 ; case 2
	b _020BD364 ; case 3
	b _020BD364 ; case 4
	b _020BD380 ; case 5
	b _020BD380 ; case 6
	b _020BD344 ; case 7
_020BD344:
	ldrsb r0, [r0]
	add r6, r6, r0
	b _020BD390
_020BD350:
	ldrb r1, [r3]
	ldrsb r0, [r0]
	sub r6, r6, r1
	add r5, r5, r0
	b _020BD390
_020BD364:
	ldrsb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r0, [r3, #1]
	add r1, r2, r1
	sub r6, r6, r1
	sub r5, r5, r0
	b _020BD390
_020BD380:
	ldrsb r1, [r0]
	ldrb r0, [r3, #1]
	add r0, r1, r0
	sub r5, r5, r0
_020BD390:
	ldr r1, [sp, #0x28]
	add r0, sp, #8
	str r1, [sp]
	str r0, [sp, #4]
	ldr r1, [r8, #0x14]
	mov r0, r8
	ldr r4, [r1]
	mov r1, r7
	mov r2, r6
	mov r3, r5
	blx r4
	ldr r0, [sp, #8]
	ldrsb r0, [r0, #2]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020BD3CC: .word 0x0000FFFF
	arm_func_end NNS_G2dCharCanvasDrawChar
