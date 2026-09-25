	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_SndArcReadFile
	.extern _021E0910

.text
	arm_func_start NNS_SndArcReadFile
NNS_SndArcReadFile: ; 0x020C98DC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r4, _020C99A0 ; =_021E0910
	mov sb, r2
	ldr r7, [r4]
	mov sl, r1
	ldr r2, [r7, #0x84]
	mov r8, r3
	ldr r1, [r2, #8]
	cmp r0, r1
	mvnhs r0, #0
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, [r7, #0x90]
	add r1, r2, #0xc
	cmp r5, #0
	moveq r5, sb
	add fp, r1, r0, lsl #4
	cmp sb, #0
	mov r4, #0
	ble _020C9998
_020C9928:
	ldr r0, [fp, #4]
	sub r6, sb, r4
	cmp r6, r5
	movgt r6, r5
	sub r0, r0, r8
	cmp r6, r0
	movhi r6, r0
	cmp r6, #0
	beq _020C9998
	ldr r1, [fp]
	mov r2, #0
	add r0, r7, #0x34
	add r1, r1, r8
	bl FS_SeekFile
	cmp r0, #0
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r1, sl
	mov r2, r6
	add r0, r7, #0x34
	bl FS_ReadFile
	cmp r0, #0
	ldmltia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r4, r4, r0
	cmp r4, sb
	add r8, r8, r0
	add sl, sl, r0
	blt _020C9928
_020C9998:
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C99A0: .word _021E0910
	arm_func_end NNS_SndArcReadFile
