	.extern string_put_char
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.extern string_fill_char
	.extern string_put_string

.text
	arm_func_start STD_TVSNPrintf
STD_TVSNPrintf: ; 0x020E3F44
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sb, r2
	str r1, [sp, #0xc]
	str r0, [sp, #0x14]
	str r0, [sp, #0x10]
	ldrsb r0, [sb]
	str r1, [sp]
	mov fp, r3
	cmp r0, #0
	beq _020E4770
_020E3F70:
	ldrsb r1, [sb]
	and r0, r1, #0xff
	eor r0, r0, #0x20
	sub r0, r0, #0xa1
	cmp r0, #0x3c
	bhs _020E3FAC
	add r0, sp, #0xc
	bl string_put_char
	ldrsb r1, [sb, #1]!
	cmp r1, #0
	beq _020E4764
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E3FAC:
	cmp r1, #0x25
	beq _020E3FC4
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E3FC4:
	mov r5, #0
	mov sl, r5
	mov r2, sb
	sub r6, r5, #1
	mov r0, #0xa
	mov r3, #0x57
_020E3FDC:
	ldrsb r4, [sb, #1]!
	cmp r4, #0x20
	bgt _020E3FF0
	beq _020E402C
	b _020E4044
_020E3FF0:
	cmp r4, #0x30
	bgt _020E4044
	cmp r4, #0x2b
	blt _020E4044
	beq _020E4018
	cmp r4, #0x2d
	beq _020E4034
	cmp r4, #0x30
	beq _020E403C
	b _020E4044
_020E4018:
	ldrsb r1, [sb, #-1]
	cmp r1, #0x20
	bne _020E4044
	orr r5, r5, #2
	b _020E3FDC
_020E402C:
	orr r5, r5, #1
	b _020E3FDC
_020E4034:
	orr r5, r5, #8
	b _020E3FDC
_020E403C:
	orr r5, r5, #0x10
	b _020E3FDC
_020E4044:
	cmp r4, #0x2a
	bne _020E4068
	add fp, fp, #4
	ldr sl, [fp, #-4]
	add sb, sb, #1
	cmp sl, #0
	rsblt sl, sl, #0
	orrlt r5, r5, #8
	b _020E4090
_020E4068:
	mov r1, #0xa
	b _020E407C
_020E4070:
	ldrsb r4, [sb], #1
	mla r4, sl, r1, r4
	sub sl, r4, #0x30
_020E407C:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020E4090
	cmp r4, #0x39
	ble _020E4070
_020E4090:
	ldrsb r1, [sb]
	cmp r1, #0x2e
	bne _020E40EC
	ldrsb r1, [sb, #1]!
	mov r6, #0
	cmp r1, #0x2a
	bne _020E40C4
	add fp, fp, #4
	ldr r6, [fp, #-4]
	add sb, sb, #1
	cmp r6, #0
	mvnlt r6, #0
	b _020E40EC
_020E40C4:
	mov r1, #0xa
	b _020E40D8
_020E40CC:
	ldrsb r4, [sb], #1
	mla r4, r6, r1, r4
	sub r6, r4, #0x30
_020E40D8:
	ldrsb r4, [sb]
	cmp r4, #0x30
	blt _020E40EC
	cmp r4, #0x39
	ble _020E40CC
_020E40EC:
	ldrsb r1, [sb]
	cmp r1, #0x68
	beq _020E4104
	cmp r1, #0x6c
	beq _020E411C
	b _020E4130
_020E4104:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x68
	orrne r5, r5, #0x40
	addeq sb, sb, #1
	orreq r5, r5, #0x100
	b _020E4130
_020E411C:
	ldrsb r1, [sb, #1]!
	cmp r1, #0x6c
	orrne r5, r5, #0x20
	addeq sb, sb, #1
	orreq r5, r5, #0x80
_020E4130:
	ldrsb r1, [sb]
	cmp r1, #0x69
	bgt _020E4174
	cmp r1, #0x63
	blt _020E4158
	beq _020E41E4
	cmp r1, #0x64
	cmpne r1, #0x69
	beq _020E439C
	b _020E4380
_020E4158:
	cmp r1, #0x25
	bgt _020E4168
	beq _020E4364
	b _020E4380
_020E4168:
	cmp r1, #0x58
	beq _020E41D0
	b _020E4380
_020E4174:
	cmp r1, #0x6e
	bgt _020E4184
	beq _020E430C
	b _020E4380
_020E4184:
	sub r1, r1, #0x6f
	cmp r1, #9
	addls pc, pc, r1, lsl #2
	b _020E4380
_020E4194: ; jump table
	b _020E41BC ; case 0
	b _020E41D8 ; case 1
	b _020E4380 ; case 2
	b _020E4380 ; case 3
	b _020E4258 ; case 4
	b _020E4380 ; case 5
	b _020E41C8 ; case 6
	b _020E4380 ; case 7
	b _020E4380 ; case 8
	b _020E4394 ; case 9
_020E41BC:
	orr r5, r5, #0x1000
	mov r0, #8
	b _020E439C
_020E41C8:
	orr r5, r5, #0x1000
	b _020E439C
_020E41D0:
	mov r3, #0x37
	b _020E4394
_020E41D8:
	orr r5, r5, #4
	mov r6, #8
	b _020E4394
_020E41E4:
	cmp r6, #0
	bge _020E4380
	add fp, fp, #4
	tst r5, #8
	ldr r4, [fp, #-4]
	beq _020E4220
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
	add r0, sp, #0xc
	sub r2, sl, #1
	mov r1, #0x20
	bl string_fill_char
	b _020E4250
_020E4220:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	sub r2, sl, #1
	bl string_fill_char
	mov r1, r4, lsl #0x18
	add r0, sp, #0xc
	mov r1, r1, asr #0x18
	bl string_put_char
_020E4250:
	add sb, sb, #1
	b _020E4764
_020E4258:
	add fp, fp, #4
	cmp r6, #0
	ldr r7, [fp, #-4]
	mov r4, #0
	bge _020E4290
	ldrsb r0, [r7]
	cmp r0, #0
	beq _020E42A4
_020E4278:
	add r4, r4, #1
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020E4278
	b _020E42A4
_020E428C:
	add r4, r4, #1
_020E4290:
	cmp r4, r6
	bge _020E42A4
	ldrsb r0, [r7, r4]
	cmp r0, #0
	bne _020E428C
_020E42A4:
	tst r5, #8
	sub sl, sl, r4
	beq _020E42D4
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
	b _020E4304
_020E42D4:
	tst r5, #0x10
	movne r0, #0x30
	moveq r0, #0x20
	mov r1, r0, lsl #0x18
	add r0, sp, #0xc
	mov r2, sl
	mov r1, r1, asr #0x18
	bl string_fill_char
	add r0, sp, #0xc
	mov r1, r7
	mov r2, r4
	bl string_put_string
_020E4304:
	add sb, sb, #1
	b _020E4764
_020E430C:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	tst r5, #0x100
	sub r2, r1, r0
	bne _020E435C
	tst r5, #0x40
	beq _020E4338
	add fp, fp, #4
	ldr r0, [fp, #-4]
	strh r2, [r0]
	b _020E435C
_020E4338:
	add fp, fp, #4
	tst r5, #0x80
	ldreq r0, [fp, #-4]
	streq r2, [r0]
	beq _020E435C
	ldr r0, [fp, #-4]
	mov r1, r2, asr #0x1f
	str r2, [r0]
	str r1, [r0, #4]
_020E435C:
	add sb, sb, #1
	b _020E4764
_020E4364:
	add r0, r2, #1
	cmp r0, sb
	bne _020E4380
	add r0, sp, #0xc
	add sb, sb, #1
	bl string_put_char
	b _020E4764
_020E4380:
	mov r1, r2
	add r0, sp, #0xc
	sub r2, sb, r2
	bl string_put_string
	b _020E4764
_020E4394:
	orr r5, r5, #0x1000
	mov r0, #0x10
_020E439C:
	tst r5, #8
	bicne r5, r5, #0x10
	cmp r6, #0
	bicge r5, r5, #0x10
	movlt r6, #1
	mov r4, #0
	tst r5, #0x1000
	beq _020E445C
	tst r5, #0x100
	beq _020E43D4
	add fp, fp, #4
	ldrb r7, [fp, #-4]
	mov r1, #0
	b _020E440C
_020E43D4:
	tst r5, #0x40
	beq _020E43EC
	add fp, fp, #4
	ldrh r7, [fp, #-4]
	mov r1, #0
	b _020E440C
_020E43EC:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, #0
	beq _020E440C
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020E440C:
	bic r5, r5, #3
	tst r5, #4
	beq _020E451C
	cmp r0, #0x10
	bne _020E4444
	cmp r1, #0
	cmpeq r7, #0
	beq _020E451C
	add r4, r3, #0x21
	mov r2, #0x30
	strb r4, [sp, #8]
	strb r2, [sp, #9]
	mov r4, #2
	b _020E451C
_020E4444:
	cmp r0, #8
	bne _020E451C
	mov r2, #0x30
	strb r2, [sp, #8]
	mov r4, #1
	b _020E451C
_020E445C:
	tst r5, #0x100
	beq _020E4474
	add fp, fp, #4
	ldrsb r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020E44AC
_020E4474:
	tst r5, #0x40
	beq _020E448C
	add fp, fp, #4
	ldrsh r7, [fp, #-4]
	mov r1, r7, asr #0x1f
	b _020E44AC
_020E448C:
	tst r5, #0x80
	addeq fp, fp, #4
	ldreq r7, [fp, #-4]
	moveq r1, r7, asr #0x1f
	beq _020E44AC
	add fp, fp, #8
	ldr r7, [fp, #-8]
	ldr r1, [fp, #-4]
_020E44AC:
	mov ip, #0
	and r2, ip, #0
	and r8, r1, #0x80000000
	cmp r2, #0
	cmpeq r8, #0
	beq _020E44E4
	mvn r4, r7
	mov r2, #0x2d
	mvn r1, r1
	strb r2, [sp, #8]
	adds r7, r4, #1
	adc r1, r1, ip
	mov r4, #1
	b _020E451C
_020E44E4:
	cmp r1, ip
	cmpeq r7, ip
	cmpeq r6, #0
	beq _020E451C
	tst r5, #2
	beq _020E450C
	mov r2, #0x2b
	strb r2, [sp, #8]
	mov r4, #1
	b _020E451C
_020E450C:
	tst r5, #1
	movne r2, #0x20
	strneb r2, [sp, #8]
	movne r4, #1
_020E451C:
	cmp r0, #8
	mov r8, #0
	beq _020E453C
	cmp r0, #0xa
	beq _020E4588
	cmp r0, #0x10
	beq _020E4620
	b _020E466C
_020E453C:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
	add r2, sp, #0x18
	mov r0, r8
	mov lr, #7
	mov ip, r8
_020E4558:
	and r3, r7, lr
	add r3, r3, #0x30
	strb r3, [r2, r8]
	mov r3, r1, lsr #3
	cmp r3, r0
	mov r7, r7, lsr #3
	orr r7, r7, r1, lsl #29
	mov r1, r3
	cmpeq r7, ip
	add r8, r8, #1
	bne _020E4558
	b _020E466C
_020E4588:
	mov r0, r8
	cmp r0, r8
	cmpeq r1, r8
	bne _020E45D4
	cmp r7, #0
	beq _020E466C
	ldr ip, _020E47BC ; =0xCCCCCCCD
	add r3, sp, #0x18
	mov r2, #0xa
_020E45AC:
	umull r1, r0, r7, ip
	movs r0, r0, lsr #3
	mul r1, r0, r2
	sub r1, r7, r1
	mov r7, r0
	add r0, r1, #0x30
	strb r0, [r3, r8]
	add r8, r8, #1
	bne _020E45AC
	b _020E466C
_020E45D4:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
_020E45E0:
	mov r0, r7
	mov r2, #0xa
	mov r3, #0
	bl _ll_udiv
	mov r2, #0xa
	umull r3, r2, r0, r2
	subs r2, r7, r3
	add r3, r2, #0x30
	add r2, sp, #0x18
	strb r3, [r2, r8]
	cmp r1, #0
	cmpeq r0, #0
	mov r7, r0
	add r8, r8, #1
	bne _020E45E0
	b _020E466C
_020E4620:
	cmp r1, r8
	cmpeq r7, r8
	beq _020E466C
	add ip, sp, #0x18
	mov lr, #0xf
_020E4634:
	and r2, r7, lr
	mov r7, r7, lsr #4
	mov r0, r1, lsr #4
	orr r7, r7, r1, lsl #28
	cmp r2, #0xa
	mov r1, r0
	addlt r0, r2, #0x30
	addge r0, r2, r3
	strb r0, [ip, r8]
	mov r0, #0
	cmp r1, r0
	cmpeq r7, r0
	add r8, r8, #1
	bne _020E4634
_020E466C:
	cmp r4, #0
	ble _020E4694
	ldrsb r0, [sp, #8]
	cmp r0, #0x30
	bne _020E4694
	add r0, sp, #0x18
	mov r1, #0x30
	strb r1, [r0, r8]
	add r8, r8, #1
	mov r4, #0
_020E4694:
	tst r5, #0x10
	sub r6, r6, r8
	beq _020E46B0
	sub r0, sl, r8
	sub r0, r0, r4
	cmp r6, r0
	movlt r6, r0
_020E46B0:
	cmp r6, #0
	subgt sl, sl, r6
	add r0, r4, r8
	sub sl, sl, r0
	ands r0, r5, #8
	str r0, [sp, #4]
	bne _020E46DC
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020E46DC:
	cmp r4, #0
	ble _020E4708
	add r0, sp, #8
	add r5, r0, r4
	add r7, sp, #0xc
_020E46F0:
	ldrsb r1, [r5, #-1]!
	mov r0, r7
	sub r4, r4, #1
	bl string_put_char
	cmp r4, #0
	bgt _020E46F0
_020E4708:
	add r0, sp, #0xc
	mov r2, r6
	mov r1, #0x30
	bl string_fill_char
	cmp r8, #0
	ble _020E4744
	add r0, sp, #0x18
	add r5, r0, r8
	add r4, sp, #0xc
_020E472C:
	ldrsb r1, [r5, #-1]!
	mov r0, r4
	sub r8, r8, #1
	bl string_put_char
	cmp r8, #0
	bgt _020E472C
_020E4744:
	ldr r0, [sp, #4]
	cmp r0, #0
	beq _020E4760
	add r0, sp, #0xc
	mov r2, sl
	mov r1, #0x20
	bl string_fill_char
_020E4760:
	add sb, sb, #1
_020E4764:
	ldrsb r0, [sb]
	cmp r0, #0
	bne _020E3F70
_020E4770:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _020E478C
	ldr r0, [sp, #0x10]
	mov r1, #0
	strb r1, [r0]
	b _020E47A8
_020E478C:
	ldr r0, [sp]
	cmp r0, #0
	beq _020E47A8
	ldr r1, [sp, #0x14]
	mov r2, #0
	add r0, r1, r0
	strb r2, [r0, #-1]
_020E47A8:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	sub r0, r1, r0
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020E47BC: .word 0xCCCCCCCD
	arm_func_end STD_TVSNPrintf
