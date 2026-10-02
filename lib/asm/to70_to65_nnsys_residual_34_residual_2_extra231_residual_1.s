	.extern DrawCellToSurface2D_
	.extern DrawCellToSurface3D_
	.extern NNS_G2dFontFindGlyphIndex
	.extern NNSi_G2dBitReaderRead
	.extern NNSi_G2dGetCellTransferState
	.extern NNSi_G2dGetUnpackedFont
	.extern NNSi_G2dSplitCharUTF16
	.extern _02109414
	.extern _02109434
	.extern _021DA498
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
