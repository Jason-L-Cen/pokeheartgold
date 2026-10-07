	.extern DrawCellToSurface2D_
	.extern DrawCellToSurface3D_
	.extern NNSi_G2dBitReaderRead
	.extern NNSi_G2dGetCellTransferState
	.extern NNSi_G2dGetUnpackedFont
	.extern NNSi_G2dSplitCharUTF16
	.extern _02109414
	.extern _02109434
	.extern _021DA498
	.extern GetCharIndex1D
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
