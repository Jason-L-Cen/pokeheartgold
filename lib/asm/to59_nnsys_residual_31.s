	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public CalcSpriteParams_
	.extern _021092F8
	.extern _02109310
	.extern _0210933C
	.extern _02109344
	.extern _02109364
	.extern _02109394
	.extern _021DA47C

.text
	arm_func_start CalcSpriteParams_
CalcSpriteParams_: ; 0x020BAF88
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #8
	mov r6, r0
	ldr r0, _020BB1C8 ; =0xC000C000
	ldmia r6, {r4, r5}
	and r0, r4, r0
	and r4, r0, #0xc0000000
	mov lr, r4, lsr #0x1e
	and r0, r0, #0xc000
	mov r5, r5, lsl #0x16
	mov ip, r0, asr #0xe
	mov r5, r5, lsr #0x16
	ldr r4, _020BB1CC ; =_02109310
	mov r7, r5, lsl #0x10
	ldr r0, _020BB1D0 ; =_021092F8
	add r5, r4, ip, lsl #3
	mov r8, lr, lsl #1
	add r4, r0, ip, lsl #3
	ldrh r5, [r8, r5]
	ldr r0, [sp, #0x20]
	ldrh r4, [r8, r4]
	str r5, [r0, #0x10]
	mov r5, r1
	str r4, [r0, #0x14]
	ldr r8, [r5, #0x14]
	mov r4, r3
	cmp r8, #0
	mov r1, r7, lsr #0x10
	bne _020BB068
	ldr r3, [r5, #8]
	ldr r7, [r5]
	mov r3, r3, lsl #0x1a
	orr r2, r3, r2, lsr #3
	orr r2, r2, #0x40000000
	orr r2, r2, r7, lsl #20
	ldr r3, [r5, #4]
	ldr r7, [r5, #0x10]
	orr r3, r2, r3, lsl #23
	ldr r2, _020BB1D4 ; =0x040004A8
	orr r3, r3, r7, lsl #29
	str r3, [r2]
	ldr r2, [r5, #8]
	ldr r3, [r5]
	cmp r2, #4
	ldr r2, _020BB1D8 ; =_02109344
	moveq r1, r1, lsl #0xf
	ldr r2, [r2, r3, lsl #2]
	moveq r1, r1, lsr #0x10
	sub r2, r2, #1
	and r2, r1, r2
	mov r2, r2, lsl #0xf
	str r2, [r0]
	ldr r2, [r5]
	mov r1, r1, asr r2
	mov r1, r1, lsl #0xf
	b _020BB0D0
_020BB068:
	and r3, r8, #0x700000
	mov r3, r3, asr #0x14
	add r7, r3, #5
	mov r3, ip, lsl #0x10
	add ip, r2, r1, lsl r7
	mov r1, lr, lsl #0x10
	mov r8, r3, lsr #0x10
	ldr r7, _020BB1DC ; =_02109364
	ldr lr, [r5, #8]
	mov r3, ip, lsr #3
	orr ip, r3, lr, lsl #26
	ldr r2, _020BB1E0 ; =_02109394
	mov lr, r1, lsr #0x10
	add r1, r2, r8, lsl #4
	add r3, r7, r8, lsl #4
	ldr r2, [r3, lr, lsl #2]
	orr r3, ip, #0x40000000
	orr r2, r3, r2, lsl #20
	ldr r1, [r1, lr, lsl #2]
	ldr r3, [r5, #0x10]
	orr r2, r2, r1, lsl #23
	ldr r1, _020BB1D4 ; =0x040004A8
	orr r2, r2, r3, lsl #29
	str r2, [r1]
	mov r1, #0
	str r1, [r0]
_020BB0D0:
	str r1, [r0, #4]
	ldr r2, [r0]
	ldr r1, [r0, #0x10]
	add r1, r2, r1, lsl #12
	str r1, [r0, #8]
	ldr r2, [r0, #4]
	ldr r1, [r0, #0x14]
	add r1, r2, r1, lsl #12
	str r1, [r0, #0xc]
	ldr r1, [r6]
	mov r2, r1, lsl #2
	mov r1, r1, lsl #3
	mov ip, r2, lsr #0x1f
	movs r3, r1, lsr #0x1f
	beq _020BB11C
	ldr r2, [r0]
	ldr r1, [r0, #8]
	str r1, [r0]
	str r2, [r0, #8]
_020BB11C:
	cmp ip, #0
	beq _020BB134
	ldr r2, [r0, #4]
	ldr r1, [r0, #0xc]
	str r1, [r0, #4]
	str r2, [r0, #0xc]
_020BB134:
	ldr r1, _020BB1E4 ; =_021DA47C
	ldr r7, [r1]
	cmp r7, #0
	beq _020BB15C
	str r3, [sp]
	add r1, r0, #4
	add r2, r0, #8
	add r3, r0, #0xc
	str ip, [sp, #4]
	blx r7
_020BB15C:
	ldr r0, [r6, #4]
	ldr r1, [r6]
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x1c
	mov r3, r1, lsl #0x12
	ldr r1, [r5, #0xc]
	mov r0, r0, lsl #0x10
	ldr r2, _020BB1E8 ; =_0210933C
	mov r3, r3, lsr #0x1f
	cmp r1, #0
	mov r0, r0, lsr #0x10
	ldr r1, [r2, r3, lsl #2]
	movne r0, r0, lsl #9
	bne _020BB1A0
	cmp r1, #4
	moveq r0, #0
	movne r0, r0, lsl #5
_020BB1A0:
	cmp r1, #2
	moveq r2, #1
	movne r2, #0
	add r1, r4, r0
	rsb r0, r2, #4
	mov r1, r1, lsr r0
	ldr r0, _020BB1EC ; =0x040004AC
	str r1, [r0]
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020BB1C8: .word 0xC000C000
_020BB1CC: .word _02109310
_020BB1D0: .word _021092F8
_020BB1D4: .word 0x040004A8
_020BB1D8: .word _02109344
_020BB1DC: .word _02109364
_020BB1E0: .word _02109394
_020BB1E4: .word _021DA47C
_020BB1E8: .word _0210933C
_020BB1EC: .word 0x040004AC
	arm_func_end CalcSpriteParams_
