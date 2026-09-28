	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_021FBE14

.text
	arm_func_start ov00_021FBE14
ov00_021FBE14: ; 0x021FBE14
	ldrb r1, [r0, #0x22]
	ldrb r2, [r0, #0x16]
	cmp r1, #0
	ldreqb r1, [r0, #0x15]
	cmp r2, #0xa
	bhs _021FBE60
	ldrb r0, [r0, #0x14]
	cmp r0, #3
	ldreq r0, _021FBEFC ; =0xFFFF3864
	subeq r0, r0, r1
	bxeq lr
	cmp r0, #4
	ldrne r0, _021FBF00 ; =0xFFFF379C
	subne r0, r0, r1
	bxne lr
	mov r0, #0xc800
	rsb r0, r0, #0
	sub r0, r0, r1
	bx lr
_021FBE60:
	cmp r2, #0xd
	ldrlo r0, _021FBF04 ; =0xFFFF34E0
	sublo r0, r0, r1
	bxlo lr
	ldr r0, [r0, #0x10]
	cmp r0, #0
	ldreq r0, _021FBF08 ; =0xFFFF3CB0
	subeq r0, r0, r1
	bxeq lr
	mvn r3, #0
	cmp r0, r3
	ldreq r0, _021FBF0C ; =0xFFFF347C
	subeq r0, r0, r1
	bxeq lr
	sub r2, r3, #1
	cmp r0, r2
	ldreq r0, _021FBF10 ; =0xFFFF3418
	subeq r0, r0, r1
	bxeq lr
	sub r2, r3, #2
	cmp r0, r2
	ldreq r0, _021FBF14 ; =0xFFFF33B4
	subeq r0, r0, r1
	bxeq lr
	sub r2, r3, #3
	cmp r0, r2
	ldreq r0, _021FBF18 ; =0xFFFF30F8
	subeq r0, r0, r1
	bxeq lr
	sub r2, r3, #4
	cmp r0, r2
	ldreq r0, _021FBF1C ; =0xFFFF3094
	subeq r0, r0, r1
	bxeq lr
	sub r2, r3, #5
	cmp r0, r2
	ldreq r0, _021FBF20 ; =0xFFFF3030
	subeq r0, r0, r1
	bx lr
	.balign 4, 0
_021FBEFC: .word 0xFFFF3864
_021FBF00: .word 0xFFFF379C
_021FBF04: .word 0xFFFF34E0
_021FBF08: .word 0xFFFF3CB0
_021FBF0C: .word 0xFFFF347C
_021FBF10: .word 0xFFFF3418
_021FBF14: .word 0xFFFF33B4
_021FBF18: .word 0xFFFF30F8
_021FBF1C: .word 0xFFFF3094
_021FBF20: .word 0xFFFF3030
	arm_func_end ov00_021FBE14
