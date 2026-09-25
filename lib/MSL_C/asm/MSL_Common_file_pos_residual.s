	.include "asm/macros.inc"
	.include "MSL_Common_file_pos.inc"
	.include "global.inc"

	.text
	.public _ftell
	.public ftell
	.public fseek
	.public rewind
	arm_func_start _fseek
_fseek: ; 0x020E561C
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r1, [r5, #4]
	mov r4, r2
	mov r1, r1, lsl #0x16
	mov r1, r1, lsr #0x1d
	and r1, r1, #0xff
	cmp r1, #1
	ldreqb r1, [r5, #0xd]
	cmpeq r1, #0
	beq _020E5668
	ldr r0, _020E5800 ; =errno
	mov r1, #0x28
	str r1, [r0]
	sub r0, r1, #0x29
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_020E5668:
	ldr r1, [r5, #8]
	mov r1, r1, lsl #0x1d
	mov r1, r1, lsr #0x1d
	cmp r1, #1
	bne _020E56B8
	mov r1, #0
	bl __flush_buffer
	cmp r0, #0
	beq _020E56B8
	mov r0, #1
	strb r0, [r5, #0xd]
	mov r2, #0
	ldr r0, _020E5800 ; =errno
	mov r1, #0x28
	str r2, [r5, #0x28]
	str r1, [r0]
	sub r0, r1, #0x29
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_020E56B8:
	cmp r4, #1
	bne _020E56D8
	mov r0, r5
	mov r4, #0
	bl _ftell
	ldr r1, [sp, #0x14]
	add r0, r1, r0
	str r0, [sp, #0x14]
_020E56D8:
	cmp r4, #2
	beq _020E576C
	ldr r0, [r5, #4]
	mov r0, r0, lsl #0x1b
	mov r0, r0, lsr #0x1d
	cmp r0, #3
	beq _020E576C
	ldr r0, [r5, #8]
	mov r0, r0, lsl #0x1d
	mov r0, r0, lsr #0x1d
	sub r0, r0, #2
	cmp r0, #1
	bhi _020E576C
	ldr r2, [sp, #0x14]
	ldr r0, [r5, #0x18]
	cmp r2, r0
	bhs _020E5728
	ldr r0, [r5, #0x34]
	cmp r2, r0
	bhs _020E5738
_020E5728:
	ldr r0, [r5, #8]
	bic r0, r0, #7
	str r0, [r5, #8]
	b _020E5778
_020E5738:
	ldr r1, [r5, #0x1c]
	sub r0, r2, r0
	add r0, r1, r0
	str r0, [r5, #0x24]
	ldr r1, [r5, #0x18]
	ldr r0, [sp, #0x14]
	sub r0, r1, r0
	str r0, [r5, #0x28]
	ldr r0, [r5, #8]
	bic r0, r0, #7
	orr r0, r0, #2
	str r0, [r5, #8]
	b _020E5778
_020E576C:
	ldr r0, [r5, #8]
	bic r0, r0, #7
	str r0, [r5, #8]
_020E5778:
	ldr r0, [r5, #8]
	mov r0, r0, lsl #0x1d
	movs r0, r0, lsr #0x1d
	bne _020E57F0
	ldr ip, [r5, #0x38]
	cmp ip, #0
	beq _020E57DC
	ldr r0, [r5]
	ldr r3, [r5, #0x48]
	add r1, sp, #0x14
	mov r2, r4
	blx ip
	cmp r0, #0
	beq _020E57DC
	mov r0, #1
	strb r0, [r5, #0xd]
	mov r2, #0
	ldr r0, _020E5800 ; =errno
	mov r1, #0x28
	str r2, [r5, #0x28]
	str r1, [r0]
	sub r0, r1, #0x29
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_020E57DC:
	mov r1, #0
	strb r1, [r5, #0xc]
	ldr r0, [sp, #0x14]
	str r0, [r5, #0x18]
	str r1, [r5, #0x28]
_020E57F0:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020E5800: .word errno
	arm_func_end _fseek
	exception ftell, 0x011D, 0x00300F00
	exception _fseek, 0x01E9, 0x00400320
	exception fseek, 0x012D, 0x00403F00
	exception rewind, 0x0025, 0x00100100
