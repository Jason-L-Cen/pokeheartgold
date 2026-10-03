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
	.extern long2str__wide

.text
	arm_func_start double2hex__wide
double2hex__wide: ; 0x020EC3B4
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x44
	ldr r7, [sp, #0x80]
	ldr r0, _020EC720 ; =0x000001FD
	mov r8, r2
	cmp r7, r0
	ldrh r6, [sp, #0x7a]
	ldrb r5, [sp, #0x77]
	ldrb r4, [sp, #0x75]
	ldr r1, [sp, #0x68]
	ldr r2, [sp, #0x6c]
	addgt sp, sp, #0x44
	movgt r0, #0
	ldmgtia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	addgt sp, sp, #0x10
	bxgt lr
	mov sl, #0
	mov sb, #0x20
	add r0, sp, #8
	add r3, sp, #0xc
	strb sl, [sp, #8]
	strh sb, [sp, #0xa]
	bl __num2dec
	ldr r0, [sp, #0x68]
	ldr r1, [sp, #0x6c]
	bl fabs
	mov r2, r0
	mov r0, sl
	mov r3, r1
	mov r1, r0
	bl _deq
	bne _020EC45C
	sub r4, r8, #0xc
	ldr r1, _020EC724 ; =wstr$6DE4
	mov r0, r4
	bl wcscpy
	add sp, sp, #0x44
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	add sp, sp, #0x10
	bx lr
_020EC45C:
	ldrb r0, [sp, #0x11]
	cmp r0, #0x49
	bne _020EC4DC
	ldrsb r0, [sp, #0xc]
	cmp r0, #0
	beq _020EC4A0
	cmp r6, #0x41
	sub r4, r8, #0xa
	bne _020EC490
	ldr r1, _020EC728 ; =wstr$6DF0
	mov r0, r4
	bl wcscpy
	b _020EC4C8
_020EC490:
	ldr r1, _020EC72C ; =wstr$6DFC
	mov r0, r4
	bl wcscpy
	b _020EC4C8
_020EC4A0:
	cmp r6, #0x41
	sub r4, r8, #8
	bne _020EC4BC
	ldr r1, _020EC730 ; =wstr$6E08
	mov r0, r4
	bl wcscpy
	b _020EC4C8
_020EC4BC:
	ldr r1, _020EC734 ; =wstr$6E10
	mov r0, r4
	bl wcscpy
_020EC4C8:
	add sp, sp, #0x44
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	add sp, sp, #0x10
	bx lr
_020EC4DC:
	cmp r0, #0x4e
	bne _020EC558
	ldrsb r0, [sp, #0xc]
	cmp r0, #0
	beq _020EC51C
	cmp r6, #0x41
	sub r4, r8, #0xa
	bne _020EC50C
	ldr r1, _020EC738 ; =wstr$6E18
	mov r0, r4
	bl wcscpy
	b _020EC544
_020EC50C:
	ldr r1, _020EC73C ; =wstr$6E24
	mov r0, r4
	bl wcscpy
	b _020EC544
_020EC51C:
	cmp r6, #0x41
	sub r4, r8, #8
	bne _020EC538
	ldr r1, _020EC740 ; =wstr$6E30
	mov r0, r4
	bl wcscpy
	b _020EC544
_020EC538:
	ldr r1, _020EC744 ; =wstr$6E38
	mov r0, r4
	bl wcscpy
_020EC544:
	add sp, sp, #0x44
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	add sp, sp, #0x10
	bx lr
_020EC558:
	mov r3, sl
	mov r1, #1
	mov r0, #0x64
	add sb, sp, #0x68
	strb r1, [sp, #0x34]
	strb r1, [sp, #0x35]
	strb r3, [sp, #0x36]
	strb r3, [sp, #0x37]
	strb r3, [sp, #0x38]
	str r3, [sp, #0x3c]
	str r1, [sp, #0x40]
	strh r0, [sp, #0x3a]
_020EC588:
	rsb r1, r3, #7
	ldrsb r2, [sb, r3]
	ldrsb r0, [sb, r1]
	strb r0, [sb, r3]
	add r3, r3, #1
	strb r2, [sb, r1]
	cmp r3, #4
	blt _020EC588
	ldrb r0, [sp, #0x69]
	ldrb r1, [sp, #0x68]
	ldr sb, _020EC748 ; =0x000007FF
	mov r0, r0, lsl #0x11
	orr r1, r0, r1, lsl #25
	add r0, sp, #0x34
	and lr, sb, r1, lsr #21
	sub ip, sp, #8
	ldmia r0, {r0, r1, r2, r3}
	stmia ip, {r0, r1, r2, r3}
	rsb r0, sb, #0x400
	mov r1, r8
	add r0, lr, r0
	ldmia ip, {r2, r3}
	bl long2str__wide
	cmp r6, #0x61
	moveq r1, #0x70
	movne r1, #0x50
	strh r1, [r0, #-2]!
	mov r1, r7, lsl #2
	mov lr, r7
	cmp r7, #1
	add r8, r1, #0xb
	add ip, sp, #0x68
	blt _020EC698
	mov sb, #0x30
_020EC610:
	cmp r8, #0x40
	bge _020EC680
	ldrb r1, [ip, r8, asr #3]
	and r2, r8, #7
	rsb r3, r2, #7
	mov r2, r1, asr r3
	sub sl, r8, #4
	bic r1, r8, #7
	bic sl, sl, #7
	cmp r1, sl
	add sl, ip, r8, asr #3
	and r1, r2, #0xff
	beq _020EC654
	ldrb r2, [sl, #-1]
	mov r2, r2, lsl #8
	orr r1, r1, r2, asr r3
	and r1, r1, #0xff
_020EC654:
	and r1, r1, #0xf
	cmp r1, #0xa
	addlo r1, r1, #0x30
	andlo r1, r1, #0xff
	blo _020EC684
	cmp r6, #0x61
	addeq r1, r1, #0x57
	andeq r1, r1, #0xff
	addne r1, r1, #0x37
	andne r1, r1, #0xff
	b _020EC684
_020EC680:
	mov r1, sb
_020EC684:
	sub lr, lr, #1
	cmp lr, #1
	strh r1, [r0, #-2]!
	sub r8, r8, #4
	bge _020EC610
_020EC698:
	cmp r7, #0
	cmpeq r5, #0
	movne r1, #0x2e
	strneh r1, [r0, #-2]!
	mov r1, #0x31
	strh r1, [r0, #-2]
	cmp r6, #0x61
	moveq r1, #0x78
	movne r1, #0x58
	strh r1, [r0, #-4]!
	mov r1, #0x30
	strh r1, [r0, #-2]!
	ldrsb r1, [sp, #0xc]
	cmp r1, #0
	movne r1, #0x2d
	strneh r1, [r0, #-2]!
	addne sp, sp, #0x44
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	addne sp, sp, #0x10
	bxne lr
	cmp r4, #1
	moveq r1, #0x2b
	streqh r1, [r0, #-2]!
	addeq sp, sp, #0x44
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	addeq sp, sp, #0x10
	bxeq lr
	cmp r4, #2
	moveq r1, #0x20
	streqh r1, [r0, #-2]!
	add sp, sp, #0x44
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	add sp, sp, #0x10
	bx lr
	.align 2, 0
_020EC720: .word 0x000001FD
_020EC724: .word wstr$6DE4
_020EC728: .word wstr$6DF0
_020EC72C: .word wstr$6DFC
_020EC730: .word wstr$6E08
_020EC734: .word wstr$6E10
_020EC738: .word wstr$6E18
_020EC73C: .word wstr$6E24
_020EC740: .word wstr$6E30
_020EC744: .word wstr$6E38
_020EC748: .word 0x000007FF
	arm_func_end double2hex__wide
