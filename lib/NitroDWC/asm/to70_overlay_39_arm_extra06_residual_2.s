	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_02229474
	.public ov39_02229630
	.extern _0222AB84
	.extern ov39_02229408
	.extern ov39_022298CC
	.extern ov39_022298E0
	.extern ov39_022298FC
	.extern ov39_02229910
	.extern ov39_02229980
	.extern ov39_02229AD4
	.extern ov39_02229BE4
	.extern ov39_02229D2C
	.extern ov39_02229E40
	.extern ov39_02229FC0
	.extern ov39_02229FCC
	.extern ov39_0222A4EC
	.extern ov39_0222A59C
	.extern ov39_0222A6DC

.text
	arm_func_start ov39_02229474
ov39_02229474: ; 0x02229474
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r5, #0
	ldr r4, _0222962C ; =_0222AB84
	mov fp, #9
	mov sl, #7
	mov r6, #1
	mov r7, r5
_02229490:
	ldr r0, [r4, #0x13c]
	mov r8, r7
	cmp r0, #9
	addls pc, pc, r0, lsl #2
	b _022295E8
_022294A4: ; jump table
	b _022294CC ; case 0
	b _022294CC ; case 1
	b _022294D4 ; case 2
	b _02229508 ; case 3
	b _0222953C ; case 4
	b _02229570 ; case 5
	b _022295B8 ; case 6
	b _022294CC ; case 7
	b _022294CC ; case 8
	b _022294CC ; case 9
_022294CC:
	mov r8, r6
	b _022295E8
_022294D4:
	bl ov39_02229FCC
	cmp r0, #0
	bne _022294F8
	ldr r0, [r4, #0x250]
	mov r8, #1
	cmp r0, #0xd
	moveq sb, sl
	movne sb, fp
	b _022294FC
_022294F8:
	mov sb, #3
_022294FC:
	bl ov39_022298FC
	str sb, [r4, #0x13c]
	b _022295E8
_02229508:
	bl ov39_02229AD4
	cmp r0, #0
	bne _0222952C
	ldr r0, [r4, #0x250]
	mov r8, #1
	cmp r0, #0xd
	moveq sb, #7
	movne sb, #9
	b _02229530
_0222952C:
	mov sb, #4
_02229530:
	bl ov39_022298FC
	str sb, [r4, #0x13c]
	b _022295E8
_0222953C:
	bl ov39_02229BE4
	cmp r0, #0
	bne _02229560
	ldr r0, [r4, #0x250]
	mov r8, #1
	cmp r0, #0xd
	moveq sb, #7
	movne sb, #9
	b _02229564
_02229560:
	mov sb, #5
_02229564:
	bl ov39_022298FC
	str sb, [r4, #0x13c]
	b _022295E8
_02229570:
	bl ov39_02229D2C
	cmp r0, #0
	bne _02229594
	ldr r0, [r4, #0x250]
	mov r8, #1
	cmp r0, #0xd
	moveq sb, #7
	movne sb, #9
	b _02229598
_02229594:
	mov sb, #6
_02229598:
	ldr r0, [r4, #0x258]
	cmp r0, #0
	beq _022295AC
	bl ov39_022298E0
	str r5, [r4, #0x258]
_022295AC:
	bl ov39_022298FC
	str sb, [r4, #0x13c]
	b _022295E8
_022295B8:
	bl ov39_02229E40
	cmp r0, #0
	bne _022295D8
	ldr r0, [r4, #0x250]
	cmp r0, #0xd
	moveq sb, #7
	movne sb, #9
	b _022295DC
_022295D8:
	mov sb, #8
_022295DC:
	mov r8, #1
	bl ov39_022298FC
	str sb, [r4, #0x13c]
_022295E8:
	cmp r8, #0
	bne _02229624
	ldr r0, [r4, #0x268]
	cmp r0, #1
	bne _02229614
	ldr r0, _0222962C ; =_0222AB84
	mov r1, #7
	str r1, [r0, #0x13c]
	mov r1, #0xd
	str r1, [r0, #0x250]
	b _02229624
_02229614:
	bl ov39_02229910
	mov r0, #1
	bl ov39_02229FC0
	b _02229490
_02229624:
	bl ov39_02229910
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0222962C: .word _0222AB84
	arm_func_end ov39_02229474

	arm_func_start ov39_02229630
ov39_02229630: ; 0x02229630
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	bl ov39_022298FC
	ldr r0, _02229818 ; =_0222AB84
	ldr r0, [r0, #0x13c]
	cmp r0, #9
	addls pc, pc, r0, lsl #2
	b _022296A8
_0222965C: ; jump table
	b _02229684 ; case 0
	b _022296A8 ; case 1
	b _02229684 ; case 2
	b _02229684 ; case 3
	b _02229684 ; case 4
	b _02229684 ; case 5
	b _02229684 ; case 6
	b _022296A4 ; case 7
	b _022296A4 ; case 8
	b _022296A4 ; case 9
_02229684:
	bl ov39_02229910
	ldr r0, _02229818 ; =_0222AB84
	mov r1, #9
	str r1, [r0, #0x13c]
	mov r1, #1
	str r1, [r0, #0x250]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_022296A4:
	bl ov39_02229408
_022296A8:
	bl ov39_02229910
	mov r0, r7
	bl ov39_0222A6DC
	ldr r2, _02229818 ; =_0222AB84
	mvn r1, #0
	str r0, [r2, #0x25c]
	cmp r0, r1
	bne _022296E0
	mov r0, #9
	str r0, [r2, #0x13c]
	mov r0, #2
	str r0, [r2, #0x250]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_022296E0:
	cmp r4, r1
	str r5, [r2, #0x260]
	addne r0, r4, #0x140
	strne r0, [sp]
	bne _02229728
	mov r0, r7
	bl ov39_0222A59C
	mvn r1, #0
	str r0, [sp]
	cmp r0, r1
	bne _02229728
	ldr r0, _02229818 ; =_0222AB84
	mov r1, #9
	str r1, [r0, #0x13c]
	mov r1, #2
	str r1, [r0, #0x250]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02229728:
	ldr r0, [sp]
	bl ov39_022298CC
	ldr ip, _02229818 ; =_0222AB84
	cmp r0, #0
	str r0, [ip, #0x258]
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, [sp]
	add r3, sp, #0
	str r1, [ip, #0x254]
	ldrb r2, [r3]
	ldrb r1, [r3, #1]
	mov r4, #0x13
	strb r2, [r0]
	strb r1, [r0, #1]
	ldrb r2, [r3, #2]
	ldrb r1, [r3, #3]
	strb r2, [r0, #2]
	strb r1, [r0, #3]
	ldr lr, [ip, #0x258]
	strh r7, [lr, #4]
	add r5, lr, #8
_02229780:
	ldmia ip!, {r0, r1, r2, r3}
	stmia r5!, {r0, r1, r2, r3}
	subs r4, r4, #1
	bne _02229780
	ldmia ip, {r0, r1}
	stmia r5, {r0, r1}
	mov r0, #0
	strh r0, [lr, #6]
	ldr r2, [sp]
	mov r1, r6
	add r0, lr, #0x140
	sub r2, r2, #0x140
	bl memcpy
	ldr r0, _02229818 ; =_0222AB84
	ldr r2, [r0, #0x258]
	ldr r1, [r0, #0x254]
	add r0, r2, #4
	sub r1, r1, #4
	add r2, r2, #4
	bl ov39_0222A4EC
	ldr r0, _02229818 ; =_0222AB84
	mov r1, #2
	str r1, [r0, #0x13c]
	bl ov39_022298FC
	bl ov39_02229980
	cmp r0, #0
	bne _0222980C
	ldr r0, _02229818 ; =_0222AB84
	mov r1, #9
	str r1, [r0, #0x13c]
	mov r1, #6
	str r1, [r0, #0x250]
	bl ov39_02229910
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222980C:
	bl ov39_02229910
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_02229818: .word _0222AB84
	arm_func_end ov39_02229630
