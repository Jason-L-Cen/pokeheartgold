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
	.extern ClearChar

.text
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
