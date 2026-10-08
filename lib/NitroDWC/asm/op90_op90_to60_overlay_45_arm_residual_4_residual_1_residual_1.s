	.extern ov45_022327B8
	.extern ov45_02233FE4
	.extern ov45_0223649C
	.extern ov45_02237664
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_0223D7B4
	.extern ov45_0223DA3C
	.extern ov45_0223DB54
	.extern ov45_022551D8
	.extern ov45_022551D9
	.extern ov45_022551DC
	.extern ov45_022551E0
	.extern ov45_022551E4
	.extern ov45_022551E8
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.extern ov45_0223D644

.text
	arm_func_start ov45_0223D27C
ov45_0223D27C: ; 0x0223D27C
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x84
	mov r4, r1
	mov r5, r0
	ldr r1, _0223D62C ; =ov45_022551DC
	add r0, sp, #0x60
	bl ov45_02237B58
	add r0, sp, #0x78
	add r2, sp, #0x60
	mov r1, r5
	mov r3, #2
	bl ov45_0223649C
	add r0, sp, #0x60
	bl ov45_02237BC8
	ldr r0, [sp, #0x7c]
	cmp r0, #2
	beq _0223D2D4
	add r0, sp, #0x78
	bl ov45_02237664
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0223D2D4:
	ldr r1, [sp, #0x78]
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [r1, #4]
	bne _0223D2F8
	ldrb r0, [r1]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223D2F8:
	cmp r0, #0x28
	bls _0223D314
	add r0, sp, #0x78
	bl ov45_02237664
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0223D314:
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [r1, #0x10]
	bne _0223D334
	ldrb r0, [r1, #0xc]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223D334:
	cmp r0, #0x190
	bls _0223D350
	add r0, sp, #0x78
	bl ov45_02237664
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0223D350:
	ldr r1, _0223D630 ; =ov45_022551E0
	add r0, sp, #0x54
	bl ov45_02237B58
	ldr r1, [sp, #0x78]
	add r0, sp, #0x6c
	add r2, sp, #0x54
	mov r3, #0
	bl ov45_0223649C
	add r0, sp, #0x54
	bl ov45_02237BC8
	ldr r0, [sp, #0x70]
	cmp r0, #6
	bhs _0223D3A0
	add r0, sp, #0x6c
	bl ov45_02237664
	add r0, sp, #0x78
	bl ov45_02237664
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0223D3A0:
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #1
	ldrne r0, [r1, #8]
	add r1, sp, #0x20
	bl strtoul
	str r0, [r4]
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0xd
	ldrne r0, [r1, #0x14]
	add r1, sp, #0x20
	bl strtoul
	str r0, [r4, #4]
	ldr r1, [sp, #0x6c]
	ldr r5, _0223D634 ; =ov45_022551E4
	ldr r0, [r1, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0x19
	ldrne r1, [r1, #0x20]
	add r0, sp, #0x3c
	bl ov45_02237B58
	add r0, sp, #0x48
	mov r1, r5
	bl ov45_02237B58
	add r0, sp, #0x3c
	add r1, sp, #0x48
	mov r2, #1
	bl ov45_0223D644
	mov r5, r0
	add r0, sp, #0x48
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	cmp r5, #0
	moveq r0, #1
	movne r0, #0
	str r0, [r4, #8]
	ldr r1, [sp, #0x6c]
	ldr r5, _0223D638 ; =ov45_022551E8
	ldr r0, [r1, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, r1, #0x25
	ldrne r1, [r1, #0x2c]
	add r0, sp, #0x24
	bl ov45_02237B58
	add r0, sp, #0x30
	mov r1, r5
	bl ov45_02237B58
	add r0, sp, #0x24
	add r1, sp, #0x30
	mov r2, #1
	bl ov45_0223D644
	mov r5, r0
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	cmp r5, #0
	moveq r0, #0
	movne r0, #1
	str r0, [r4, #0xc]
	ldr r1, [sp, #0x6c]
	mov r2, #0x10
	ldr r0, [r1, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0x31
	ldrne r0, [r1, #0x38]
	add r1, sp, #0x20
	bl strtol
	str r0, [r4, #0x10]
	add r0, sp, #0x6c
	bl ov45_02237664
	ldr r0, [r4, #8]
	cmp r0, #1
	bne _0223D530
	ldr r1, [sp, #0x78]
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #0xd
	ldrne r0, [r1, #0x14]
	add r1, r4, #0x18
	bl ov45_02233FE4
	cmp r0, #0
	bne _0223D618
	add r0, sp, #0x78
	bl ov45_02237664
	add sp, sp, #0x84
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0223D530:
	ldr r0, [sp, #0x78]
	ldr r1, [r0, #0xc]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r0, #0x10]
	bne _0223D554
	ldrb r1, [r0, #0xc]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_0223D554:
	ldr r1, [r0, #0xc]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldr r1, [r0, #0xc]
	addeq r3, r0, #0xd
	mov r1, r1, lsl #0x1f
	ldrne r3, [r0, #0x14]
	movs r1, r1, lsr #0x1f
	addeq r5, r0, #0xd
	ldrne r5, [r0, #0x14]
	mov r0, #0
	add r2, r3, r2
	strb r0, [sp, #0xb]
	strb r0, [sp, #0xc]
	strb r0, [sp, #0xa]
	strb r0, [sp, #4]
	sub r1, sp, #4
	ldr r0, [sp, #0x1c]
	str r2, [r1]
	str r0, [r1, #4]
	ldr r3, [r1]
	add r1, sp, #0x10
	str r5, [sp, #0x10]
	str r2, [sp, #0x18]
	add r0, r4, #0x18
	ldmia r1, {r1, r2}
	bl ov45_022327B8
	mov r2, #0
	strb r2, [sp, #8]
	ldr r1, [r4, #0x1c]
	ldr r0, [r4, #0x20]
	cmp r1, r0
	bhs _0223D5F8
	add r3, r1, #1
	str r3, [r4, #0x1c]
	ldr r0, _0223D63C ; =ov45_022551D8
	ldr r1, [r4, #0x18]
	ldrsb r2, [r0, #1]
	add r0, r1, r3
	strb r2, [r0, #-1]
	b _0223D618
_0223D5F8:
	strb r2, [sp, #9]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	ldr r1, _0223D640 ; =ov45_022551D9
	add r0, r4, #0x18
	bl ov45_0223DA3C
_0223D618:
	add r0, sp, #0x78
	bl ov45_02237664
	mov r0, #1
	add sp, sp, #0x84
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_0223D62C: .word ov45_022551DC
_0223D630: .word ov45_022551E0
_0223D634: .word ov45_022551E4
_0223D638: .word ov45_022551E8
_0223D63C: .word ov45_022551D8
_0223D640: .word ov45_022551D9
	arm_func_end ov45_0223D27C
