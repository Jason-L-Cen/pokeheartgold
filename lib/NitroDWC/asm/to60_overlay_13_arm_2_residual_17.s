	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0223FF18

.text
	arm_func_start ov13_0223FF18
ov13_0223FF18: ; 0x0223FF18
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x10
	cmp r1, #0
	ldr r5, [r0, #8]
	blt _0223FF54
	ldr r4, [r5, r1, lsl #3]
	ldr r0, _02240000 ; =0xFE00FF00
	and r3, r3, #0xff
	and r0, r4, r0
	mov r2, r2, lsl #0x17
	orr r0, r0, r3
	orr r0, r0, r2, lsr #7
	add sp, sp, #0x10
	str r0, [r5, r1, lsl #3]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223FF54:
	ldr r1, [r5]
	ldr ip, _02240000 ; =0xFE00FF00
	and r6, r3, #0xff
	and r7, r1, ip
	mov sb, r1
	ldr r1, _02240004 ; =0x01FF0000
	mov r8, r2, lsl #0x17
	orr r6, r7, r6
	orr r6, r6, r8, lsr #7
	str r6, [r5]
	ldrb r6, [r0, #0xc]
	and r4, sb, r1
	and lr, sb, #0xff
	mov r7, r4, lsr #0x10
	str r7, [sp, #0xc]
	cmp r6, #1
	str lr, [sp, #8]
	sub r6, r2, r4, lsr #16
	sub r7, r3, lr
	mov r4, #1
	addle sp, sp, #0x10
	ldmleia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223FFAC:
	ldr r2, [r5, r4, lsl #3]
	and r3, r2, #0xff
	mov sb, r2
	and lr, r2, r1
	add r8, r3, r7
	add r2, r6, lr, lsr #16
	and sb, sb, ip
	and r8, r8, #0xff
	mov sl, r2, lsl #0x17
	orr r2, sb, r8
	orr r2, r2, sl, lsr #7
	str r2, [r5, r4, lsl #3]
	ldrb r2, [r0, #0xc]
	add r4, r4, #1
	mov r8, lr, lsr #0x10
	cmp r4, r2
	blt _0223FFAC
	str r3, [sp]
	str r8, [sp, #4]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02240000: .word 0xFE00FF00
_02240004: .word 0x01FF0000
	arm_func_end ov13_0223FF18
