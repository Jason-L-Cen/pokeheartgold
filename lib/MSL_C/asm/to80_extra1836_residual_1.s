	.extern buff_ptr$6E40
	.extern strptr$6E44
	.extern wstr$6DE4
	.extern wstr$6DF0
	.extern wstr$6DFC
	.extern wstr$6E08
	.extern wstr$6E10
	.extern wstr$6E18
	.extern wstr$6E24
	.extern wstr$6E30
	.extern wstr$6E38
	.include "asm/macros.inc"
	.include "MSL_Common_wprintf.inc"
	.include "global.inc"
	.public long2str__wide

.text
	arm_func_start long2str__wide
long2str__wide: ; 0x020EBE64
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x10
	movs sl, r0
	mov r0, #0
	mov r5, r1
	str r0, [sp, #0xc]
	ldr r7, [sp, #0x4c]
	mov r6, r0
	strh r0, [r5, #-2]!
	ldrb r0, [sp, #0x43]
	str r1, [sp]
	ldrh r8, [sp, #0x46]
	str r0, [sp, #4]
	ldr r0, [sp, #0x48]
	ldrb fp, [sp, #0x41]
	str r0, [sp, #8]
	cmpeq r7, #0
	bne _020EBED8
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020EBEC4
	cmp r8, #0x6f
	beq _020EBED8
_020EBEC4:
	add sp, sp, #0x10
	mov r0, r5
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
_020EBED8:
	cmp r8, #0x69
	bgt _020EBF00
	bge _020EBF34
	cmp r8, #0x58
	bgt _020EBEF4
	beq _020EBF60
	b _020EBF68
_020EBEF4:
	cmp r8, #0x64
	beq _020EBF34
	b _020EBF68
_020EBF00:
	cmp r8, #0x6f
	bgt _020EBF14
	moveq r4, #8
	moveq fp, #0
	b _020EBF68
_020EBF14:
	cmp r8, #0x78
	bgt _020EBF68
	cmp r8, #0x75
	blt _020EBF68
	beq _020EBF54
	cmp r8, #0x78
	beq _020EBF60
	b _020EBF68
_020EBF34:
	cmp sl, #0
	mov r4, #0xa
	bge _020EBF68
	mov r0, #1
	cmp sl, #0x80000000
	rsbne sl, sl, #0
	str r0, [sp, #0xc]
	b _020EBF68
_020EBF54:
	mov r4, #0xa
	mov fp, #0
	b _020EBF68
_020EBF60:
	mov r4, #0x10
	mov fp, #0
_020EBF68:
	mov r0, sl
	mov r1, r4
	bl _u32_div_f
	mov sb, r1
	mov r0, sl
	mov r1, r4
	bl _u32_div_f
	cmp sb, #0xa
	mov sl, r0
	addlt sb, sb, #0x30
	blt _020EBFA0
	cmp r8, #0x78
	addeq sb, sb, #0x57
	addne sb, sb, #0x37
_020EBFA0:
	cmp sl, #0
	strh sb, [r5, #-2]!
	add r6, r6, #1
	bne _020EBF68
	cmp r4, #8
	bne _020EBFD4
	ldr r0, [sp, #4]
	cmp r0, #0
	ldrneh r0, [r5]
	cmpne r0, #0x30
	movne r0, #0x30
	strneh r0, [r5, #-2]!
	addne r6, r6, #1
_020EBFD4:
	ldrb r0, [sp, #0x40]
	cmp r0, #2
	bne _020EC008
	ldr r0, [sp, #0xc]
	ldr r7, [sp, #8]
	cmp r0, #0
	cmpeq fp, #0
	subne r7, r7, #1
	cmp r4, #0x10
	bne _020EC008
	ldr r0, [sp, #4]
	cmp r0, #0
	subne r7, r7, #2
_020EC008:
	ldr r0, [sp]
	ldr r1, _020EC0B0 ; =0x000001FD
	sub r0, r0, r5
	add r0, r0, r0, lsr #31
	add r0, r7, r0, asr #1
	cmp r0, r1
	addgt sp, sp, #0x10
	movgt r0, #0
	ldmgtia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	addgt sp, sp, #0x10
	bxgt lr
	cmp r6, r7
	bge _020EC050
	mov r0, #0x30
_020EC040:
	add r6, r6, #1
	cmp r6, r7
	strh r0, [r5, #-2]!
	blt _020EC040
_020EC050:
	cmp r4, #0x10
	bne _020EC06C
	ldr r0, [sp, #4]
	cmp r0, #0
	movne r0, #0x30
	strneh r8, [r5, #-2]
	strneh r0, [r5, #-4]!
_020EC06C:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	movne r0, #0x2d
	strneh r0, [r5, #-2]!
	bne _020EC09C
	cmp fp, #1
	moveq r0, #0x2b
	streqh r0, [r5, #-2]!
	beq _020EC09C
	cmp fp, #2
	moveq r0, #0x20
	streqh r0, [r5, #-2]!
_020EC09C:
	mov r0, r5
	add sp, sp, #0x10
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020EC0B0: .word 0x000001FD
	arm_func_end long2str__wide
