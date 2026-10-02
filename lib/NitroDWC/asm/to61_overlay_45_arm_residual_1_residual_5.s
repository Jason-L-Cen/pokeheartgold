	.extern _02254D04
	.extern _02254F58
	.extern _022577C4
	.extern ov45_0223CAB4
	.extern ov45_0223CB04
	.extern ov45_0223CB6C
	.extern ov45_0223CC78
	.extern ov45_0223CD78
	.extern ov45_0223CDBC
	.extern ov45_0223CE30
	.extern ov45_0223CE60
	.extern ov45_0223CE90
	.extern ov45_0223D27C
	.extern ov45_0223D7D4
	.extern ov45_0223DD3C
	.extern ov45_0223DE38
	.extern ov45_0223DEC4
	.extern ov45_0223DF4C
	.extern ov45_0223DFC8
	.extern ov45_0223E100
	.extern ov45_0223E4A8
	.extern ov45_0223E558
	.extern ov45_02240874
	.extern ov45_02240A58
	.extern ov45_02240A74
	.extern ov45_02240A80
	.extern ov45_02240B10
	.extern ov45_02240F9C
	.extern ov45_02241048
	.extern ov45_022412CC
	.extern ov45_022413D4
	.extern ov45_022414C0
	.extern ov45_022415CC
	.extern ov45_022417B4
	.extern ov45_02241A20
	.extern ov45_02241B84
	.extern ov45_02241D58
	.extern ov45_02241E24
	.extern ov45_02242394
	.extern ov45_0225443C
	.extern ov45_022545F0
	.extern ov45_022546F8
	.extern ov45_02254774
	.extern ov45_022549E0
	.extern ov45_02254A04
	.extern ov45_02254D1C
	.extern ov45_02254D3C
	.extern ov45_02254D5C
	.extern ov45_02254D7C
	.extern ov45_02254D9C
	.extern ov45_02254F88
	.extern ov45_02254FA0
	.extern ov45_02254FB0
	.extern ov45_02254FC0
	.extern ov45_022550B0
	.extern ov45_022550B4
	.extern ov45_022550B8
	.extern ov45_022550BC
	.extern ov45_022550C4
	.extern ov45_022550D4
	.extern ov45_022550E4
	.extern ov45_022550F4
	.extern ov45_02255104
	.extern ov45_02255120
	.extern ov45_02255144
	.extern ov45_02255160
	.extern ov45_0225517C
	.extern ov45_02255198
	.extern ov45_022551A4
	.extern ov45_022551A8
	.extern ov45_022551AC
	.extern ov45_022551C4
	.extern ov45_022577C8
	.extern ov45_022577D0
	.extern ov45_022577D4
	.extern ov45_022577D8
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223795C
	.public ov45_022379D4
	.public ov45_02237A24
	.public ov45_02237AA0
	.public ov45_02237B58
	.public ov45_02237BC8
	.public ov45_02237BFC
	.public ov45_02237D48
	.public ov45_02237EB0
	.public ov45_02237F7C
	.public ov45_02237FD8
	.public ov45_02238104
	.public ov45_022383E0
	.public ov45_02238450
	.public ov45_02238508
	.public ov45_02238558
	.public ov45_022385A4
	.public ov45_02238674
	.public ov45_022387E4
	.public ov45_02238B08
	.public ov45_02238B70
	.public ov45_02238C1C
	.public ov45_02238C74
	.public ov45_0223AC78
	.public ov45_0223ACA4
	.public ov45_0223AE84
	.public ov45_0223AEDC
	.public ov45_0223AF04
	.public ov45_0223AF28
	.public ov45_0223B1C8
	.extern ov45_022310B0
	.extern ov45_02231C78
	.extern ov45_022320B4
	.extern ov45_0223325C
	.extern ov45_022377AC
	.extern ov45_02237800
	.extern ov45_02237890

.text
	arm_func_start ov45_0223795C
ov45_0223795C: ; 0x0223795C
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #8]
	mov r4, r1
	cmp r4, r3
	ldrls r1, [r5, #4]
	subls r0, r3, r4
	cmpls r1, r0
	bls _022379A0
	ldr r0, [r5, #4]
	mov r2, #0
	add r1, r0, r4
	mov r0, r5
	sub r1, r1, r3
	strb r2, [sp]
	bl ov45_02237890
	b _022379C4
_022379A0:
	ldr r0, [r5]
	mov r2, r4
	add r0, r0, r1
	mov r1, #0
	bl memset
	ldr r0, [r5, #4]
	add r0, r0, r4
	str r0, [r5, #4]
	ldmia sp!, {r3, r4, r5, pc}
_022379C4:
	mov r0, r5
	mov r1, r4
	bl ov45_02238820
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223795C

	arm_func_start ov45_022379D4
ov45_022379D4: ; 0x022379D4
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _02237A18
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _02237A18
	mov r2, r0
	bl ov00_021EC2EC
_02237A18:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_022379D4

	arm_func_start ov45_02237A24
ov45_02237A24: ; 0x02237A24
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0xc
	mov r5, r1
	ldr r1, [r5]
	mov r4, r0
	mov r0, r1, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldmeqia r5, {r0, r1, r2}
	stmeqia r4, {r0, r1, r2}
	beq _02237A94
	mov r1, #0
	mov r0, r1
_02237A54:
	str r0, [r4, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02237A54
	ldr r1, [r5, #4]
	mov r0, r4
	bl ov45_02237BFC
	ldmib r5, {r0, r3}
	mov r1, #0
	add r0, r3, r0
	str r0, [sp]
	ldrb ip, [sp, #8]
	mov r0, r4
	mov r2, r1
	strb ip, [sp, #4]
	bl ov45_02238104
_02237A94:
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, pc}
	arm_func_end ov45_02237A24

	arm_func_start ov45_02237AA0
ov45_02237AA0: ; 0x02237AA0
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov r5, r1
	mov r1, #0
	mov r6, r0
	mov r4, r2
	mov r0, r1
_02237AC0:
	str r0, [r6, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02237AC0
	ldr r0, [r5]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r1, [r5, #4]
	bne _02237AF0
	ldrb r0, [r5]
	mov r0, r0, lsl #0x18
	mov r1, r0, lsr #0x19
_02237AF0:
	cmp r4, r1
	bhi _02237B18
	ldr r0, [sp, #0x2c]
	sub r1, r1, r4
	cmp r1, r0
	addlo r0, sp, #8
	addhs r0, sp, #0x2c
	str r1, [sp, #8]
	ldr r1, [r0]
	b _02237B1C
_02237B18:
	mov r1, #0
_02237B1C:
	mov r0, r6
	bl ov45_02237BFC
	str r4, [sp]
	ldr r4, [sp, #0x2c]
	mov r1, #0
	mov r0, r6
	mov r2, r1
	mov r3, r5
	str r4, [sp, #4]
	bl ov45_02237E04
	mov r0, r6
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02237AA0

	arm_func_start ov45_02237B58
ov45_02237B58: ; 0x02237B58
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov r5, r1
	mov r1, #0
	mov r6, r0
	mov r0, r1
_02237B70:
	str r0, [r6, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _02237B70
	mov r0, r5
	bl strlen
	mov r4, r0
	mov r0, r6
	mov r1, r4
	bl ov45_02237BFC
	add r0, r5, r4
	mov r1, #0
	ldrb r3, [sp, #8]
	str r0, [sp]
	mov r0, r6
	mov r2, r1
	strb r3, [sp, #4]
	mov r3, r5
	bl ov45_02238104
	mov r0, r6
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_02237B58

	arm_func_start ov45_02237BC8
ov45_02237BC8: ; 0x02237BC8
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r1, [r4, #8]
	cmpne r1, #0
	beq _02237BF4
	mov r0, #0
	mov r2, r0
	bl ov00_021EC2EC
_02237BF4:
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_02237BC8

	arm_func_start ov45_02237BFC
ov45_02237BFC: ; 0x02237BFC
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	mov r8, r1
	mvn r1, #0x80000001
	mov sb, r0
	cmp r8, r1
	bls _02237C20
	ldr r0, _02237D44 ; =ov45_02255120
	bl printf
	bl abort
_02237C20:
	ldr r0, [sb]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r7, #1
	moveq r7, #0
	cmp r7, #0
	ldmneia sb, {r0, r4}
	movne r1, r0, lsr #1
	bne _02237C54
	ldrb r0, [sb]
	mov r1, #0xb
	mov r0, r0, lsl #0x18
	mov r4, r0, lsr #0x19
_02237C54:
	cmp r8, r4
	movlo r8, r4
	add r0, r8, #1
	cmp r0, #0xb
	movls r8, #0xb
	addhi r0, r8, #0x10
	bichi r8, r0, #0xf
	cmp r8, r1
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	cmp r8, #0xb
	bne _02237C90
	add r5, sb, #1
	ldr sl, [sb, #8]
	mov r6, #0
	b _02237CC4
_02237C90:
	cmp r8, r1
	mov r0, r8
	bls _02237CA8
	bl ov45_02242394
	mov r5, r0
	b _02237CB4
_02237CA8:
	bl ov45_02242394
	movs r5, r0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02237CB4:
	cmp r7, #0
	ldrne sl, [sb, #8]
	mov r6, #1
	addeq sl, sb, #1
_02237CC4:
	mov r0, r5
	mov r1, sl
	mov r2, r4
	bl memmove
	mov r0, #0
	cmp r7, #0
	strb r0, [r5, r4]
	cmpne sl, #0
	beq _02237CF4
	mov r1, sl
	mov r2, r0
	bl ov00_021EC2EC
_02237CF4:
	ldr r1, [sb]
	and r0, r6, #1
	bic r1, r1, #1
	orr r0, r1, r0
	str r0, [sb]
	cmp r6, #0
	bne _02237D2C
	ldrb r1, [sb]
	and r0, r4, #0xff
	mov r0, r0, lsl #0x19
	bic r1, r1, #0xfe
	orr r0, r1, r0, lsr #24
	strb r0, [sb]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02237D2C:
	stmib sb, {r4, r5}
	ldr r0, [sb]
	and r0, r0, #1
	orr r0, r0, r8, lsl #1
	str r0, [sb]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02237D44: .word ov45_02255120
	arm_func_end ov45_02237BFC

	arm_func_start ov45_02237D48
ov45_02237D48: ; 0x02237D48
	stmdb sp!, {r3, lr}
	mov ip, r0
	ldr r0, [ip]
	mov r3, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r1, #1
	moveq r1, #0
	cmp r1, #0
	bne _02237D90
	ldr r0, [r3]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	bne _02237D90
	ldmia r3, {r0, r1, r2}
	stmia ip, {r0, r1, r2}
	mov r0, ip
	ldmia sp!, {r3, pc}
_02237D90:
	cmp r1, #0
	ldreqb r0, [ip]
	ldrne r2, [ip, #4]
	mov r1, #0
	moveq r0, r0, lsl #0x18
	moveq r2, r0, lsr #0x19
	mov r0, ip
	bl ov45_02237DB4
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02237D48

	arm_func_start ov45_02237DB4
ov45_02237DB4: ; 0x02237DB4
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, [r3]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldrne lr, [r3, #8]
	ldrne r3, [r3, #4]
	bne _02237DE4
	ldrb ip, [r3]
	add lr, r3, #1
	mov r3, ip, lsl #0x18
	mov r3, r3, lsr #0x19
_02237DE4:
	ldrb ip, [sp, #8]
	add r3, lr, r3
	str r3, [sp]
	mov r3, lr
	strb ip, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0xc
	ldmia sp!, {pc}
	arm_func_end ov45_02237DB4

	arm_func_start ov45_02237E04
ov45_02237E04: ; 0x02237E04
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x10
	ldr r4, [r3]
	mov r8, r0
	mov r0, r4, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r7, r1
	mov r6, r2
	ldrne r4, [r3, #8]
	ldrne r5, [r3, #4]
	bne _02237E40
	ldrb r0, [r3]
	add r4, r3, #1
	mov r0, r0, lsl #0x18
	mov r5, r0, lsr #0x19
_02237E40:
	ldr r0, [sp, #0x28]
	cmp r0, r5
	bls _02237E58
	ldr r0, _02237EAC ; =ov45_02255144
	bl printf
	bl abort
_02237E58:
	ldr r2, [sp, #0x28]
	ldr r0, [sp, #0x2c]
	sub r1, r5, r2
	cmp r1, r0
	addlo r0, sp, #0xc
	str r1, [sp, #0xc]
	add r4, r4, r2
	addhs r0, sp, #0x2c
	ldr r0, [r0]
	ldrb r5, [sp, #8]
	str r0, [sp, #0x2c]
	add r0, r4, r0
	str r0, [sp]
	mov r0, r8
	mov r1, r7
	mov r2, r6
	mov r3, r4
	strb r5, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02237EAC: .word ov45_02255144
	arm_func_end ov45_02237E04

	arm_func_start ov45_02237EB0
ov45_02237EB0: ; 0x02237EB0
	stmdb sp!, {r3, lr}
	ldr r3, [r1]
	mov r3, r3, lsl #0x1f
	movs r3, r3, lsr #0x1f
	ldmneib r1, {r3, ip}
	bne _02237ED8
	ldrb r3, [r1]
	add ip, r1, #1
	mov r1, r3, lsl #0x18
	mov r3, r1, lsr #0x19
_02237ED8:
	mov r1, ip
	bl ov45_02237EE4
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02237EB0

	arm_func_start ov45_02237EE4
ov45_02237EE4: ; 0x02237EE4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr ip, [r0]
	mov ip, ip, lsl #0x1f
	movs ip, ip, lsr #0x1f
	ldrne lr, [r0, #8]
	ldrne r0, [r0, #4]
	bne _02237F10
	ldrb ip, [r0]
	add lr, r0, #1
	mov r0, ip, lsl #0x18
	mov r0, r0, lsr #0x19
_02237F10:
	cmp r2, r0
	bhi _02237F74
	sub ip, r0, r2
	cmp ip, r3
	add r4, r1, r3
	add r5, lr, r2
	blo _02237F74
_02237F2C:
	mov r6, r5
	mov r7, r1
	cmp r1, r4
	bhs _02237F5C
_02237F3C:
	ldrsb r2, [r7]
	ldrsb r0, [r6]
	cmp r2, r0
	bne _02237F64
	add r7, r7, #1
	cmp r7, r4
	add r6, r6, #1
	blo _02237F3C
_02237F5C:
	sub r0, r5, lr
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_02237F64:
	sub ip, ip, #1
	cmp ip, r3
	add r5, r5, #1
	bhs _02237F2C
_02237F74:
	mvn r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov45_02237EE4

	arm_func_start ov45_02237F7C
ov45_02237F7C: ; 0x02237F7C
	stmdb sp!, {r3, lr}
	ldr r2, [r1]
	mov r2, r2, lsl #0x1f
	movs r2, r2, lsr #0x1f
	ldrne r3, [r1, #8]
	ldrne ip, [r1, #4]
	bne _02237FA8
	ldrb r2, [r1]
	add r3, r1, #1
	mov r1, r2, lsl #0x18
	mov ip, r1, lsr #0x19
_02237FA8:
	ldr r1, [r0]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r0, #4]
	bne _02237FC8
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_02237FC8:
	mov r1, #0
	str ip, [sp]
	bl ov45_02238024
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02237F7C

	arm_func_start ov45_02237FD8
ov45_02237FD8: ; 0x02237FD8
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl strlen
	ldr r1, [r5]
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r2, [r5, #4]
	bne _0223800C
	ldrb r1, [r5]
	mov r1, r1, lsl #0x18
	mov r2, r1, lsr #0x19
_0223800C:
	str r0, [sp]
	mov r0, r5
	mov r3, r4
	mov r1, #0
	bl ov45_02238024
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02237FD8

	arm_func_start ov45_02238024
ov45_02238024: ; 0x02238024
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r2, [r0]
	mov r6, r1
	mov r1, r2, lsl #0x1f
	movs r1, r1, lsr #0x1f
	mov r5, r3
	ldrne r4, [r0, #8]
	ldrne r7, [r0, #4]
	bne _02238060
	ldrb r1, [r0]
	add r4, r0, #1
	mov r0, r1, lsl #0x18
	mov r7, r0, lsr #0x19
_02238060:
	cmp r6, r7
	bls _02238074
	ldr r0, _02238100 ; =ov45_02255160
	bl printf
	bl abort
_02238074:
	ldr r0, [sp, #0x28]
	sub r1, r7, r6
	cmp r0, r1
	addlo r0, sp, #0x28
	addhs r0, sp, #0
	str r1, [sp]
	ldr r1, [r0]
	ldr r0, [sp, #0x30]
	str r1, [sp, #4]
	cmp r0, r1
	addlo r0, sp, #0x30
	addhs r0, sp, #4
	ldr r2, [r0]
	mov r1, r5
	add r0, r4, r6
	bl memcmp
	cmp r0, #0
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, lr}
	addne sp, sp, #0x10
	bxne lr
	ldr r1, [sp, #0x30]
	ldr r0, [sp, #4]
	cmp r0, r1
	addlo sp, sp, #8
	mvnlo r0, #0
	ldmloia sp!, {r3, r4, r5, r6, r7, lr}
	addlo sp, sp, #0x10
	bxlo lr
	movne r0, #1
	moveq r0, #0
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_02238100: .word ov45_02255160
	arm_func_end ov45_02238024

	arm_func_start ov45_02238104
ov45_02238104: ; 0x02238104
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	mov r6, r0
	ldr r0, [r6]
	mov r5, r1
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	movne r0, #1
	strne r0, [sp]
	moveq r0, #0
	streq r0, [sp]
	ldr r0, [sp]
	mov fp, r3
	ldr r4, [sp, #0x60]
	cmp r0, #0
	bne _02238164
	ldrb r1, [r6]
	mov r0, #0xb
	str r0, [sp, #0x14]
	mov r0, r1, lsl #0x18
	add r7, r6, #1
	mov r8, r0, lsr #0x19
	b _02238174
_02238164:
	ldmia r6, {r0, r8}
	mov r0, r0, lsr #1
	ldr r7, [r6, #8]
	str r0, [sp, #0x14]
_02238174:
	cmp r5, r8
	bls _02238188
	ldr r0, _022383D4 ; =ov45_02255144
	bl printf
	bl abort
_02238188:
	ldr r0, [sp, #0x58]
	sub r1, r8, r5
	cmp r1, r0
	addlo r0, sp, #0x18
	str r1, [sp, #0x18]
	addhs r0, sp, #0x58
	ldr r0, [r0]
	sub sb, r4, fp
	mvn r1, #0x80000001
	cmp sb, r1
	str r0, [sp, #0x10]
	subls sl, r8, r0
	subls r0, r1, sb
	cmpls sl, r0
	bls _022381D0
	ldr r0, _022383D8 ; =ov45_0225517C
	bl printf
	bl abort
_022381D0:
	ldr r0, [sp, #0x10]
	add sl, sb, sl
	add r1, r5, r0
	ldr r0, [sp, #0x14]
	cmp sl, r0
	sub r0, r8, r1
	str r0, [sp, #0xc]
	blo _022382E4
	ldr r0, [sp, #0x14]
	add r1, sl, #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	bhs _0223821C
_02238208:
	mov r0, r8, lsl #1
	add r0, r0, #0xf
	bic r8, r0, #0xf
	cmp r8, r1
	blo _02238208
_0223821C:
	mov r0, r8
	bl ov45_02242394
	str r0, [sp, #8]
	cmp r5, #0
	beq _0223823C
	mov r1, r7
	mov r2, r5
	bl memcpy
_0223823C:
	ldr r0, [sp, #8]
	sub r2, r4, fp
	add r0, r0, r5
	mov r1, fp
	str r0, [sp, #4]
	bl memmove
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223827C
	mov r2, r0
	ldr r0, [sp, #4]
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	add r0, r0, sb
	add r1, r3, r1
	bl memcpy
_0223827C:
	ldr r1, _022383DC ; =_02254F58
	ldr r0, [sp]
	ldrsb r1, [r1, #3]
	cmp r0, #0
	ldr r0, [sp, #8]
	strb r1, [r0, sl]
	beq _022382B4
	cmp r7, #0
	beq _022382C4
	mov r0, #0
	mov r1, r7
	mov r2, r0
	bl ov00_021EC2EC
	b _022382C4
_022382B4:
	ldr r0, [r6]
	bic r0, r0, #1
	orr r0, r0, #1
	str r0, [r6]
_022382C4:
	ldr r0, [sp, #8]
	str r0, [r6, #8]
	str sl, [r6, #4]
	ldr r0, [r6]
	and r0, r0, #1
	orr r0, r0, r8, lsl #1
	str r0, [r6]
	b _022383C0
_022382E4:
	mov r1, #0
	add r0, sp, #0x1c
	mov r3, r1
_022382F0:
	str r3, [r0, r1, lsl #2]
	add r1, r1, #1
	cmp r1, #3
	blo _022382F0
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _02238350
	add r0, r7, r5
	add r0, r0, sb
	cmp r0, r4
	bhs _02238350
	add r0, r7, r8
	cmp r4, r0
	bhi _02238350
	add r0, sp, #0x1c
	mov r1, fp
	mov r2, r4
	bl ov45_022384BC
	ldr r0, [sp, #0x1c]
	add r1, sp, #0x1c
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq fp, r1, #1
	ldrne fp, [r1, #8]
_02238350:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _02238374
	ldr r1, [sp, #0x10]
	add r3, r7, r5
	mov r2, r0
	add r0, r3, sb
	add r1, r3, r1
	bl memmove
_02238374:
	mov r1, fp
	mov r2, sb
	add r0, r7, r5
	bl memmove
	ldr r0, [sp]
	ldr r1, _022383DC ; =_02254F58
	cmp r0, #0
	ldrsb r0, [r1, #1]
	strb r0, [r7, sl]
	strne sl, [r6, #4]
	bne _022383B8
	ldrb r1, [r6]
	and r0, sl, #0xff
	mov r0, r0, lsl #0x19
	bic r1, r1, #0xfe
	orr r0, r1, r0, lsr #24
	strb r0, [r6]
_022383B8:
	add r0, sp, #0x1c
	bl ov45_02237BC8
_022383C0:
	mov r0, r6
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_022383D4: .word ov45_02255144
_022383D8: .word ov45_0225517C
_022383DC: .word _02254F58
	arm_func_end ov45_02238104

	arm_func_start ov45_022383E0
ov45_022383E0: ; 0x022383E0
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r5, r1
	mov r4, r2
	subs r1, r4, r5
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	mov r3, #0
	strb r3, [sp, #1]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov45_02237800
	mov r0, #0
	strb r0, [sp]
	ldr r2, [r6]
	ldr r0, [r6, #4]
	mov r1, r5
	add r0, r2, r0
	sub r2, r4, r5
	bl memmove
	ldr r1, [r6, #4]
	sub r0, r4, r5
	add r0, r1, r0
	str r0, [r6, #4]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_022383E0

	arm_func_start ov45_02238450
ov45_02238450: ; 0x02238450
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r4, #0
	str r4, [r2]
	ldr r6, [r0, #4]
	mov r4, #1
	ldr r7, [sp, #0x18]
	strb r4, [r3]
	strb r4, [r7]
	cmp r6, #0
	add r0, r0, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov ip, #0
	mov lr, r4
_02238484:
	ldr r5, [r1]
	ldr r4, [r6, #0xc]
	mov r0, r6
	cmp r5, r4
	ldrlo r6, [r6]
	strlob lr, [r3]
	blo _022384B0
	str r6, [r2]
	ldr r6, [r6, #4]
	strb ip, [r3]
	strb ip, [r7]
_022384B0:
	cmp r6, #0
	bne _02238484
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov45_02238450

	arm_func_start ov45_022384BC
ov45_022384BC: ; 0x022384BC
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr ip, [r0]
	mov r3, r1
	mov r1, ip, lsl #0x1f
	movs r1, r1, lsr #0x1f
	ldrne r1, [r0, #4]
	bne _022384E8
	ldrb r1, [r0]
	mov r1, r1, lsl #0x18
	mov r1, r1, lsr #0x19
_022384E8:
	ldrb ip, [sp, #8]
	str r2, [sp]
	mov r2, r1
	mov r1, #0
	strb ip, [sp, #4]
	bl ov45_02238104
	add sp, sp, #0xc
	ldmia sp!, {pc}
	arm_func_end ov45_022384BC

	arm_func_start ov45_02238508
ov45_02238508: ; 0x02238508
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223854C
	ldr r2, [r4, #8]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #8]
	sub r1, r1, r2
	str r1, [r4, #8]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223854C
	mov r2, r0
	bl ov00_021EC2EC
_0223854C:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_02238508

	arm_func_start ov45_02238558
ov45_02238558: ; 0x02238558
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	ldr r1, [r4]
	mov r5, r0
	cmp r1, #0
	beq _02238574
	bl ov45_02238558
_02238574:
	ldr r1, [r4, #4]
	cmp r1, #0
	beq _02238588
	mov r0, r5
	bl ov45_02238558
_02238588:
	cmp r4, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02238558

	arm_func_start ov45_022385A4
ov45_022385A4: ; 0x022385A4
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	ldr r0, _02238668 ; =0x15555555
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _022385CC
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_022385CC:
	ldr r0, _0223866C ; =0x071C71C7
	cmp r4, r0
	bhs _02238618
	add r1, r4, #1
	ldr r0, _02238670 ; =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_02238618:
	cmp r4, r0, lsl #1
	bhs _02238654
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_02238654:
	ldr r0, _02238668 ; =0x15555555
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_02238668: .word 0x15555555
_0223866C: .word 0x071C71C7
_02238670: .word 0xCCCCCCCD
	arm_func_end ov45_022385A4

	arm_func_start ov45_02238674
ov45_02238674: ; 0x02238674
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x1c
	mov r4, #0
	mov r7, r0
	add r3, r7, #8
	str r4, [sp, #8]
	str r4, [sp, #0xc]
	str r4, [sp, #0x10]
	str r4, [sp, #0x18]
	str r3, [sp, #0x14]
	mov r6, r1
	ldr r3, [r7, #4]
	ldr r1, [r7, #8]
	add r3, r3, r6
	sub r1, r3, r1
	mov r5, r2
	bl ov45_022385A4
	mov r3, r4
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov45_02238A3C
	ldr r4, [r7, #4]
	mov r3, #0
	ldr r1, [sp, #8]
	mov r0, #0xc
	mla r2, r4, r0, r1
	ldr r1, [sp, #0xc]
	str r4, [sp, #0x18]
	mla r4, r1, r0, r2
	strb r3, [sp, #2]
	cmp r6, #0
	beq _02238734
_02238708:
	cmp r4, #0
	beq _0223871C
	mov r0, r4
	mov r1, r5
	bl ov45_02237A24
_0223871C:
	ldr r0, [sp, #0xc]
	subs r6, r6, #1
	add r0, r0, #1
	str r0, [sp, #0xc]
	add r4, r4, #0xc
	bne _02238708
_02238734:
	ldr r6, [r7]
	ldr r1, [r7, #4]
	mov r0, #0xc
	mla r5, r1, r0, r6
	mov r3, #0
	ldr r2, [sp, #8]
	ldr r1, [sp, #0x18]
	strb r3, [sp, #1]
	mla r4, r1, r0, r2
	cmp r5, r6
	bls _02238798
_02238760:
	sub r5, r5, #0xc
	subs r4, r4, #0xc
	beq _02238778
	mov r0, r4
	mov r1, r5
	bl ov45_02237A24
_02238778:
	ldr r1, [sp, #0x18]
	ldr r0, [sp, #0xc]
	sub r1, r1, #1
	add r0, r0, #1
	str r1, [sp, #0x18]
	str r0, [sp, #0xc]
	cmp r5, r6
	bhi _02238760
_02238798:
	mov r0, #0
	strb r0, [sp]
	add r3, sp, #0x10
	ldr r2, [r7, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r7, #8]
	str r2, [r3]
	ldr r2, [r7]
	ldr r1, [sp, #8]
	str r1, [r7]
	str r2, [sp, #8]
	ldr r2, [r7, #4]
	ldr r1, [sp, #0xc]
	str r1, [r7, #4]
	str r2, [sp, #0xc]
	bl ov45_0223895C
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, pc}
	arm_func_end ov45_02238674

	arm_func_start ov45_022387E4
ov45_022387E4: ; 0x022387E4
	stmdb sp!, {r3, r4, r5, lr}
	ldr ip, [r0]
	ldr r3, [r0, #4]
	mov r2, #0xc
	mla r4, r3, r2, ip
	movs r5, r1
	sub r1, r3, r5
	str r1, [r0, #4]
	ldmeqia sp!, {r3, r4, r5, pc}
_02238808:
	sub r4, r4, #0xc
	mov r0, r4
	bl ov45_02237BC8
	subs r5, r5, #1
	bne _02238808
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_022387E4

	arm_func_start ov45_02238820
ov45_02238820: ; 0x02238820
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x1c
	mov r3, #0
	mov r6, r0
	add r2, r6, #8
	str r3, [sp, #8]
	str r3, [sp, #0xc]
	str r3, [sp, #0x10]
	str r3, [sp, #0x18]
	str r2, [sp, #0x14]
	mov r4, r1
	ldr r2, [r6, #4]
	ldr r1, [r6, #8]
	add r2, r2, r4
	sub r1, r2, r1
	bl ov45_02237890
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #3]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #8
	strb r3, [sp, #4]
	bl ov45_022389CC
	ldr r5, [r6, #4]
	ldr r2, [sp, #8]
	mov r1, #0
	add r3, r2, r5
	ldr r0, [sp, #0xc]
	mov r2, r4
	add r0, r3, r0
	str r5, [sp, #0x18]
	strb r1, [sp, #2]
	bl memset
	ldr r1, [sp, #0xc]
	ldr r0, [sp, #0x18]
	add r1, r1, r4
	str r1, [sp, #0xc]
	ldr r5, [r6]
	ldr r2, [r6, #4]
	mov r1, #0
	strb r1, [sp, #1]
	add r1, r5, r2
	sub r4, r1, r5
	sub r2, r0, r4
	ldr r0, [sp, #8]
	str r2, [sp, #0x18]
	add r0, r0, r2
	mov r1, r5
	mov r2, r4
	bl memcpy
	mov r0, r5
	mov r1, #0
	mov r2, r4
	bl memset
	ldr r1, [sp, #0xc]
	mov r0, #0
	add r1, r1, r4
	str r1, [sp, #0xc]
	strb r0, [sp]
	str r0, [r6, #4]
	add r3, sp, #0x10
	ldr r2, [r6, #8]
	ldr r1, [r3]
	add r0, sp, #8
	str r1, [r6, #8]
	str r2, [r3]
	ldr r2, [r6]
	ldr r1, [sp, #8]
	str r1, [r6]
	str r2, [sp, #8]
	ldr r2, [r6, #4]
	ldr r1, [sp, #0xc]
	str r1, [r6, #4]
	str r2, [sp, #0xc]
	bl ov45_022389B0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_02238820

	arm_func_start ov45_0223895C
ov45_0223895C: ; 0x0223895C
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, r0
	ldr r2, [r4]
	ldr r1, [r4, #0x10]
	mov r0, #0xc
	mla r5, r1, r0, r2
	ldr r1, [r4, #4]
	mla r6, r1, r0, r5
	cmp r6, r5
	bls _02238998
_02238984:
	sub r6, r6, #0xc
	mov r0, r6
	bl ov45_02237BC8
	cmp r6, r5
	bhi _02238984
_02238998:
	mov r1, #0
	mov r0, r4
	str r1, [r4, #4]
	bl ov45_02238A74
	mov r0, r4
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_0223895C

	arm_func_start ov45_022389B0
ov45_022389B0: ; 0x022389B0
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r1, #0
	str r1, [r4, #4]
	bl ov45_022389EC
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_022389B0

	arm_func_start ov45_022389CC
ov45_022389CC: ; 0x022389CC
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r5, r0
	mov r0, r4
	bl ov45_02242394
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_022389CC

	arm_func_start ov45_022389EC
ov45_022389EC: ; 0x022389EC
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _02238A30
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _02238A30
	mov r2, r0
	bl ov00_021EC2EC
_02238A30:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_022389EC

	arm_func_start ov45_02238A3C
ov45_02238A3C: ; 0x02238A3C
	stmdb sp!, {r3, r4, r5, lr}
	ldr r2, _02238A70 ; =0x15555555
	mov r4, r1
	mov r5, r0
	cmp r4, r2
	bls _02238A58
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_02238A58:
	mov r0, #0xc
	mul r0, r4, r0
	bl ov45_02242394
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_02238A70: .word 0x15555555
	arm_func_end ov45_02238A3C

	arm_func_start ov45_02238A74
ov45_02238A74: ; 0x02238A74
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #0
	beq _02238AC0
	ldr r1, [r4, #4]
	mov r3, #0
	strb r3, [sp]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r2, [r2]
	bl ov45_02238ACC
	ldr r1, [r4]
	cmp r1, #0
	beq _02238AC0
	mov r0, #0
	mov r2, r0
	bl ov00_021EC2EC
_02238AC0:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_02238A74

	arm_func_start ov45_02238ACC
ov45_02238ACC: ; 0x02238ACC
	stmdb sp!, {r3, r4, r5, lr}
	ldr ip, [r0]
	ldr r3, [r0, #4]
	mov r2, #0xc
	mla r4, r3, r2, ip
	movs r5, r1
	sub r1, r3, r5
	str r1, [r0, #4]
	ldmeqia sp!, {r3, r4, r5, pc}
_02238AF0:
	sub r4, r4, #0xc
	mov r0, r4
	bl ov45_02237BC8
	subs r5, r5, #1
	bne _02238AF0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02238ACC

	arm_func_start ov45_02238B08
ov45_02238B08: ; 0x02238B08
	mov r1, #0
	str r1, [r0]
	str r1, [r0, #4]
	str r1, [r0, #8]
	str r1, [r0, #0xc]
	str r1, [r0, #0x10]
	str r1, [r0, #0x14]
	str r1, [r0, #0x18]
	str r1, [r0, #0x1c]
	str r1, [r0, #0x20]
	str r1, [r0, #0x24]
	strb r1, [r0, #0x28]
	strb r1, [r0, #0x29]
	str r1, [r0, #0x2c]
	str r1, [r0, #0x30]
	str r1, [r0, #0x34]
	str r1, [r0, #0x38]
	str r1, [r0, #0x3c]
	str r1, [r0, #0x40]
	str r1, [r0, #0x44]
	str r1, [r0, #0x48]
	str r1, [r0, #0x4c]
	str r1, [r0, #0x50]
	str r1, [r0, #0x54]
	str r1, [r0, #0x58]
	bx lr
	arm_func_end ov45_02238B08

	arm_func_start ov45_02238B70
ov45_02238B70: ; 0x02238B70
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #0x30]
	ldr r0, [r2]
	sub r3, r3, #1
	mov r3, r3, lsl #3
	add r3, r3, #0x18
	mov r4, r1
	str r3, [r2]
	cmp r0, r3
	blo _02238BA4
	cmp r4, #0
	bne _02238BAC
_02238BA4:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_02238BAC:
	ldr r0, [r5, #0x1c]
	str r0, [r4]
	ldr r0, [r5, #0x20]
	str r0, [r4, #4]
	ldr r0, [r5, #0x24]
	str r0, [r4, #8]
	ldrb r0, [r5, #0x28]
	strb r0, [r4, #0xc]
	ldrb r0, [r5, #0x29]
	strb r0, [r4, #0xd]
	ldr r0, [r5, #0x30]
	strh r0, [r4, #0xe]
	ldrh r0, [r4, #0xe]
	cmp r0, #0
	beq _02238C00
	add r0, r5, #0x2c
	bl ov45_02238C08
	ldr r2, [r5, #0x30]
	add r1, r4, #0x10
	mov r2, r2, lsl #3
	bl MI_CpuCopy8
_02238C00:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02238B70

	arm_func_start ov45_02238C08
ov45_02238C08: ; 0x02238C08
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov45_02238C08

	arm_func_start ov45_02238C1C
ov45_02238C1C: ; 0x02238C1C
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mov r1, #0
	mov r5, r0
	add r2, sp, #0
	mov r0, r4
	str r1, [sp]
	bl ov45_02238B70
	ldr r1, [sp]
	mov r2, #0
	str r2, [r5]
	str r2, [r5, #4]
	mov r0, r5
	str r2, [r5, #8]
	bl ov45_022377AC
	mov r0, r5
	bl ov45_02231C78
	mov r1, r0
	add r2, sp, #0
	mov r0, r4
	bl ov45_02238B70
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02238C1C

	arm_func_start ov45_02238C74
ov45_02238C74: ; 0x02238C74
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	mov r5, r0
	mov r4, r1
	bl ov45_0223AC78
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrb r0, [r4, #0xc]
	add r1, r4, #0x10
	mov r3, #0
	strb r0, [r5, #0x28]
	ldrb r0, [r4, #0xd]
	sub r2, sp, #4
	strb r0, [r5, #0x29]
	ldr ip, [r4]
	add r0, r5, #0x2c
	str ip, [r5, #0x1c]
	ldr ip, [r4, #4]
	str ip, [r5, #0x20]
	ldr ip, [r4, #8]
	str ip, [r5, #0x24]
	ldrh r4, [r4, #0xe]
	strb r3, [sp, #1]
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r4, lsl #3
	bl ov45_0223AAE4
	ldr r0, [r5, #0x2c]
	ldr r1, [r5, #0x30]
	ldrb r2, [sp, #2]
	sub r3, sp, #4
	add r1, r0, r1, lsl #3
	strb r2, [r3]
	ldr r2, [r3]
	str r1, [sp, #0x10]
	str r0, [sp, #0x14]
	bl ov45_022391EC
	ldr r2, [r5, #0x2c]
	ldr r1, [r5, #0x30]
	ldrb r0, [sp]
	add r4, r2, r1, lsl #3
	str r4, [sp, #4]
	ldr r1, [r5, #0x2c]
	sub r2, sp, #4
	strb r0, [r2]
	ldr r3, [r2]
	add r2, sp, #4
	add r0, r5, #0x38
	str r4, [sp, #8]
	str r1, [sp, #0xc]
	bl ov45_02238D54
	mov r0, #1
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_02238C74

	arm_func_start ov45_02238D54
ov45_02238D54: ; 0x02238D54
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x3c
	mov r5, r2
	ldr r3, [r5]
	ldr r2, [sp, #0x54]
	mov r6, r0
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r1, r0, asr #3
	str r1, [sp, #0x10]
	ldr r1, [r6, #4]
	str r2, [sp, #8]
	cmp r1, r0, asr #3
	addlo r0, r6, #4
	addhs r0, sp, #0x10
	ldr r4, [r0]
	ldr r1, [sp, #0x54]
	str r3, [sp, #4]
	add r3, r1, r4, lsl #3
	add r0, sp, #0x14
	mov r1, r6
	mov r2, #0
	str r3, [sp, #0xc]
	bl ov45_0223B250
	add r1, sp, #0x54
	add r0, sp, #0xc
	ldr r2, [r0]
	ldr r1, [r1]
	add r0, sp, #0x28
	add r3, sp, #0x14
	bl ov45_02239090
	ldr r0, [sp, #0x10]
	subs r0, r0, r4
	str r0, [sp, #0x10]
	beq _02238E18
	ldrb r0, [sp, #1]
	sub r2, sp, #4
	add r1, sp, #0xc
	strb r0, [r2]
	ldr r3, [r2]
	mov r0, r6
	mov r2, r5
	bl ov45_02238E60
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
_02238E18:
	ldr r3, [r6, #4]
	cmp r4, r3
	addhs sp, sp, #0x3c
	ldmhsia sp!, {r3, r4, r5, r6, lr}
	addhs sp, sp, #0x10
	bxhs lr
	mov r1, #0
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r2, [r0]
	mov r0, r6
	sub r1, r3, r4
	bl ov45_0223B764
	add sp, sp, #0x3c
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02238D54

	arm_func_start ov45_02238E60
ov45_02238E60: ; 0x02238E60
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x38
	mov r5, r1
	mov r4, r2
	ldr r1, [r5]
	ldr ip, [r4]
	mov r6, r0
	cmp r1, ip
	addeq sp, sp, #0x38
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r3, [r5]
	ldr r1, [r6, #0x10]
	sub r2, ip, r3
	mov r0, r2, asr #2
	add r0, r2, r0, lsr #29
	str r3, [sp, #0xc]
	cmp r1, #0
	mov r2, r0, asr #3
	subne r0, r1, #1
	moveq r3, #0
	movne r3, r0, lsl #7
	ldr r1, [r6]
	ldr r0, [r6, #4]
	str ip, [sp, #8]
	add r0, r1, r0
	sub r1, r3, r0
	cmp r2, r1
	bls _02238EDC
	mov r0, r6
	sub r1, r2, r1
	bl ov45_0223B2DC
_02238EDC:
	ldr r2, [r6, #4]
	add r0, sp, #0x10
	mov r1, r6
	bl ov45_0223B250
	add r0, r6, #8
	str r0, [sp]
	add r0, r6, #4
	str r0, [sp, #4]
	ldr r1, [r5]
	ldr r2, [r4]
	add r0, sp, #0x24
	add r3, sp, #0x10
	bl ov45_02238F18
	add sp, sp, #0x38
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_02238E60

	arm_func_start ov45_02238F18
ov45_02238F18: ; 0x02238F18
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x14
	ldr r4, [sp, #0x2c]
	ldr r2, [sp, #0x30]
	ldr r1, [sp, #0x3c]
	cmp r4, r2
	beq _02239058
	sub r4, r2, r4
	mov r2, r4, asr #2
	add r2, r4, r2, lsr #29
	mov r2, r2, asr #3
	str r2, [sp, #0x10]
_02238F4C:
	ldr r4, [r3, #0x10]
	cmp r4, #0
	moveq r2, r4
	ldrne r2, [r3]
	ldrne r2, [r2]
	addne r2, r2, #0x400
	sub r4, r2, r4
	mov r2, r4, asr #2
	add r2, r4, r2, lsr #29
	ldr r4, [sp, #0x10]
	mov ip, r2, asr #3
	cmp r4, r2, asr #3
	addgt r2, sp, #8
	str ip, [sp, #8]
	addle r2, sp, #0x10
	ldr r4, [sp, #0x2c]
	ldr r5, [r2]
	str r4, [sp, #4]
	add lr, r4, r5, lsl #3
	str lr, [sp, #0xc]
	str lr, [sp]
	cmp r4, lr
	ldr r6, [r3, #0x10]
	beq _02238FE4
_02238FAC:
	cmp r6, #0
	beq _02238FC4
	ldr ip, [r4]
	ldr r2, [r4, #4]
	str ip, [r6]
	str r2, [r6, #4]
_02238FC4:
	ldr r2, [r1]
	add r4, r4, #8
	add r2, r2, #1
	str r2, [r1]
	cmp r4, lr
	add r6, r6, #8
	bne _02238FAC
	str r4, [sp, #4]
_02238FE4:
	ldr ip, [sp, #0x10]
	sub r2, r5, #1
	sub ip, ip, r5
	str ip, [sp, #0x10]
	ldr ip, [r3, #0x10]
	add r2, ip, r2, lsl #3
	str r2, [r3, #0x10]
	add ip, r2, #8
	ldr r2, [r3]
	str ip, [r3, #0x10]
	ldr r2, [r2]
	add r2, r2, #0x400
	cmp ip, r2
	bne _02239044
	ldr r2, [r3]
	add ip, r2, #4
	str ip, [r3]
	ldr r2, [r3, #0xc]
	cmp ip, r2
	ldreq r2, [r3, #4]
	streq r2, [r3]
	ldr r2, [r3]
	ldr r2, [r2]
	str r2, [r3, #0x10]
_02239044:
	ldr r2, [sp, #0x10]
	cmp r2, #0
	ldrne r2, [sp, #0xc]
	strne r2, [sp, #0x2c]
	bne _02238F4C
_02239058:
	ldr r1, [r3]
	str r1, [r0]
	ldr r1, [r3, #4]
	str r1, [r0, #4]
	ldr r1, [r3, #8]
	str r1, [r0, #8]
	ldr r1, [r3, #0xc]
	str r1, [r0, #0xc]
	ldr r1, [r3, #0x10]
	str r1, [r0, #0x10]
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02238F18

	arm_func_start ov45_02239090
ov45_02239090: ; 0x02239090
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x14
	ldr r2, [sp, #0x24]
	ldr r1, [sp, #0x28]
	cmp r2, r1
	beq _022391B4
	sub r2, r1, r2
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov r1, r1, asr #3
	str r1, [sp, #0x10]
_022390C0:
	ldr r2, [r3, #0x10]
	cmp r2, #0
	moveq r1, r2
	ldrne r1, [r3]
	ldrne r1, [r1]
	addne r1, r1, #0x400
	sub r2, r1, r2
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	ldr r2, [sp, #0x10]
	mov ip, r1, asr #3
	cmp r2, r1, asr #3
	addgt r1, sp, #8
	str ip, [sp, #8]
	addle r1, sp, #0x10
	ldr r2, [sp, #0x24]
	ldr ip, [r1]
	str r2, [sp, #4]
	add r4, r2, ip, lsl #3
	str r4, [sp, #0xc]
	str r4, [sp]
	cmp r2, r4
	ldr lr, [r3, #0x10]
	bhs _02239140
_02239120:
	ldr r1, [r2]
	str r1, [lr]
	ldr r1, [r2, #4]
	add r2, r2, #8
	str r1, [lr, #4]
	cmp r2, r4
	add lr, lr, #8
	blo _02239120
_02239140:
	ldr r2, [sp, #0x10]
	sub r1, ip, #1
	sub r2, r2, ip
	str r2, [sp, #0x10]
	ldr r2, [r3, #0x10]
	add r1, r2, r1, lsl #3
	str r1, [r3, #0x10]
	add r2, r1, #8
	ldr r1, [r3]
	str r2, [r3, #0x10]
	ldr r1, [r1]
	add r1, r1, #0x400
	cmp r2, r1
	bne _022391A0
	ldr r1, [r3]
	add r2, r1, #4
	str r2, [r3]
	ldr r1, [r3, #0xc]
	cmp r2, r1
	ldreq r1, [r3, #4]
	streq r1, [r3]
	ldr r1, [r3]
	ldr r1, [r1]
	str r1, [r3, #0x10]
_022391A0:
	ldr r1, [sp, #0x10]
	cmp r1, #0
	ldrne r1, [sp, #0xc]
	strne r1, [sp, #0x24]
	bne _022390C0
_022391B4:
	ldr r1, [r3]
	str r1, [r0]
	ldr r1, [r3, #4]
	str r1, [r0, #4]
	ldr r1, [r3, #8]
	str r1, [r0, #8]
	ldr r1, [r3, #0xc]
	str r1, [r0, #0xc]
	ldr r1, [r3, #0x10]
	str r1, [r0, #0x10]
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239090

	arm_func_start ov45_022391EC
ov45_022391EC: ; 0x022391EC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x30
	ldr r1, [sp, #0x3c]
	ldr r0, [sp, #0x38]
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r1, r0, asr #3
	cmp r1, #0x14
	movle r1, #0
	add r0, sp, #0xc
	bl ov45_0223AA80
	ldr r4, [sp, #0xc]
	ldr r3, [sp, #0x10]
	add ip, sp, #0x24
	mov r2, #0
	add lr, r4, r3, lsl #3
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	str r4, [sp, #0x2c]
	add r0, sp, #0x38
	add r1, sp, #0x3c
	str lr, [sp]
	add r3, sp, #0x40
	str ip, [sp, #4]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x1c
	str r4, [sp, #0x1c]
	str ip, [sp, #0x20]
	str r3, [sp, #8]
	str lr, [sp, #0x14]
	str ip, [sp, #0x18]
	ldmia r2, {r2, r3}
	bl ov45_02239294
	ldr r0, [sp, #0x2c]
	bl _ZdaPv
	add sp, sp, #0x30
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_022391EC

	arm_func_start ov45_02239294
ov45_02239294: ; 0x02239294
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x4c
	ldr r0, [sp, #0x64]
	ldr r6, [sp, #0x60]
	ldr r7, [sp, #0x78]
	sub r1, r0, r6
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r5, r0, asr #3
	cmp r5, #1
	addle sp, sp, #0x4c
	ldmleia sp!, {r4, r5, r6, r7, lr}
	addle sp, sp, #0x10
	bxle lr
	cmp r5, #2
	bne _02239338
	ldr r0, [sp, #0x64]
	ldr r1, [sp, #0x60]
	sub r4, r0, #8
	ldr r3, [r4]
	ldr r0, [r1]
	str r4, [sp, #0x64]
	cmp r3, r0
	addge sp, sp, #0x4c
	ldmgeia sp!, {r4, r5, r6, r7, lr}
	addge sp, sp, #0x10
	bxge lr
	ldr r2, [r1]
	ldr r1, [r1, #4]
	str r2, [sp, #0x1c]
	str r3, [r6]
	ldr r0, [r4, #4]
	str r1, [sp, #0x20]
	str r0, [r6, #4]
	str r2, [r4]
	add sp, sp, #0x4c
	str r1, [r4, #4]
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_02239338:
	cmp r5, #0x14
	bgt _02239368
	add r0, sp, #0x60
	add r1, sp, #0x64
	ldr r0, [r0]
	ldr r1, [r1]
	mov r2, r7
	bl ov45_0223A9C0
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_02239368:
	ldr r2, [sp, #0x70]
	ldr r1, [sp, #0x68]
	add r0, r5, r5, lsr #31
	sub r2, r2, r1
	mov r1, r2, asr #2
	add r2, r2, r1, lsr #29
	sub r4, r5, r0, asr #1
	ldr r1, [sp, #0x60]
	cmp r5, r2, asr #3
	add r1, r1, r4, lsl #3
	str r1, [sp, #0x48]
	add r0, sp, #0x60
	add r2, sp, #0x68
	bgt _0223944C
	ldr r0, [r0]
	str r7, [sp]
	ldmia r2, {r2, r3}
	bl ov45_0223A0CC
	ldr r2, [sp, #0x68]
	add r0, sp, #0x48
	add r3, r2, r4, lsl #3
	add r1, sp, #0x64
	ldr r6, [sp, #0x6c]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x40
	str r6, [sp, #0x44]
	str r3, [sp, #0x40]
	str r7, [sp]
	ldmia r2, {r2, r3}
	bl ov45_0223A0CC
	ldr r0, [sp, #0x68]
	add r1, sp, #0x68
	add r2, r0, r4, lsl #3
	add r3, r0, r5, lsl #3
	stmib sp, {r2, r6}
	str r3, [sp, #0xc]
	ldr r0, [sp, #0x60]
	str r6, [sp, #0x10]
	str r0, [sp, #0x14]
	str r7, [sp, #0x18]
	sub r0, sp, #4
	stmia r0, {r2, r6}
	str r3, [sp, #0x24]
	ldr r3, [r0]
	add r0, sp, #0x3c
	str r6, [sp, #0x28]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x34]
	str r6, [sp, #0x30]
	str r6, [sp, #0x38]
	ldmia r1, {r1, r2}
	bl ov45_02239F48
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223944C:
	ldr r4, [sp, #0x70]
	ldr r3, [sp, #0x74]
	str r4, [sp]
	stmib sp, {r3, r7}
	ldr r0, [r0]
	ldmia r2, {r2, r3}
	bl ov45_02239294
	ldr r4, [sp, #0x70]
	add r0, sp, #0x48
	add r1, sp, #0x64
	ldr r3, [sp, #0x74]
	str r4, [sp]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x68
	stmib sp, {r3, r7}
	ldmia r2, {r2, r3}
	bl ov45_02239294
	add r0, sp, #0x60
	add r1, sp, #0x48
	add r2, sp, #0x64
	ldr ip, [sp, #0x48]
	ldr r3, [sp, #0x60]
	ldr r0, [r0]
	sub r5, ip, r3
	mov r4, r5, asr #2
	add r4, r5, r4, lsr #29
	mov r4, r4, asr #3
	str r4, [sp]
	ldr r4, [sp, #0x64]
	ldr r5, [sp, #0x68]
	sub r6, r4, ip
	mov r4, r6, asr #2
	add r4, r6, r4, lsr #29
	mov r4, r4, asr #3
	str r4, [sp, #4]
	str r5, [sp, #8]
	ldr r4, [sp, #0x6c]
	ldr r5, [sp, #0x70]
	str r4, [sp, #0xc]
	ldr r1, [r1]
	ldr r2, [r2]
	ldr r4, [sp, #0x74]
	str r5, [sp, #0x10]
	mov r3, r7
	str r4, [sp, #0x14]
	bl ov45_02239518
	add sp, sp, #0x4c
	ldmia sp!, {r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239294

	arm_func_start ov45_02239518
ov45_02239518: ; 0x02239518
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x88
	ldr r8, [sp, #0xb0]
	ldr r7, [sp, #0xb4]
	cmp r8, #0
	cmpne r7, #0
	mov r4, r3
	addeq sp, sp, #0x88
	ldmeqia sp!, {r4, r5, r6, r7, r8, lr}
	addeq sp, sp, #0x10
	bxeq lr
	add r0, r8, r7
	cmp r0, #2
	bne _022395B4
	ldr r6, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	ldr r5, [r6]
	ldr r0, [r1]
	cmp r5, r0
	addge sp, sp, #0x88
	ldmgeia sp!, {r4, r5, r6, r7, r8, lr}
	addge sp, sp, #0x10
	bxge lr
	ldr r4, [r1]
	ldr r3, [r1, #4]
	ldr r2, [sp, #0xa0]
	ldr r0, [sp, #0xa4]
	str r5, [r2]
	ldr r1, [r6, #4]
	str r4, [sp, #0x54]
	str r1, [r2, #4]
	str r4, [r0]
	str r3, [sp, #0x58]
	str r3, [r0, #4]
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_022395B4:
	ldr r1, [sp, #0xc0]
	ldr r0, [sp, #0xb8]
	cmp r8, r7
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	cmple r8, r0, asr #3
	bgt _022396FC
	ldr r1, [sp, #0xa4]
	ldr r0, [sp, #0xa0]
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	str r0, [sp, #0x50]
	cmp r0, r1
	movne r0, #1
	moveq r0, #0
	str r3, [sp, #0x44]
	str r2, [sp, #0x48]
	str r1, [sp, #0x4c]
	cmp r0, #0
	beq _022396A0
	cmp r0, #0
	add r1, sp, #0x3c
	beq _022396A0
	ldr r5, [sp, #0x50]
	ldr r3, [sp, #0x4c]
_0223961C:
	ldr r0, [sp, #0x44]
	ldr r6, [sp, #0x48]
	str r0, [sp, #0x3c]
	str r6, [sp, #0x40]
	ldr r2, [r6, #8]
	ldr r6, [r6, #4]
	sub r7, r0, r2
	mov r2, r7, asr #2
	add r2, r7, r2, lsr #29
	cmp r6, r2, asr #3
	ble _0223965C
	ldr r2, [r5]
	str r2, [r0]
	ldr r2, [r5, #4]
	str r2, [r0, #4]
	b _02239684
_0223965C:
	cmp r0, #0
	beq _02239674
	ldr r6, [r5]
	ldr r2, [r5, #4]
	str r6, [r0]
	str r2, [r0, #4]
_02239674:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_02239684:
	ldr r0, [sp, #0x44]
	add r5, r5, #8
	add r0, r0, #8
	str r0, [sp, #0x44]
	cmp r5, r3
	bne _0223961C
	str r5, [sp, #0x50]
_022396A0:
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa8]
	str r0, [sp, #4]
	str r1, [sp, #8]
	ldr r0, [sp, #0xa0]
	ldr r5, [sp, #0x44]
	str r0, [sp, #0xc]
	ldr r2, [sp, #0x48]
	str r4, [sp, #0x10]
	sub r0, sp, #4
	str r5, [r0]
	str r2, [r0, #4]
	ldr r3, [r0]
	add r1, sp, #0xb8
	str r2, [sp, #0x78]
	add r0, sp, #0x7c
	str r5, [sp, #0x74]
	ldmia r1, {r1, r2}
	bl ov45_02239DF0
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_022396FC:
	cmp r7, r0, asr #3
	bgt _02239830
	ldr r1, [sp, #0xa4]
	ldr r0, [sp, #0xa8]
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	cmp r1, r0
	movne r0, #1
	moveq r0, #0
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	str r1, [sp, #0x38]
	cmp r0, #0
	beq _022397CC
	cmp r0, #0
	add r1, sp, #0x28
	beq _022397CC
	ldr r5, [sp, #0x38]
	ldr r3, [sp, #0xa8]
_02239748:
	ldr r0, [sp, #0x30]
	ldr r6, [sp, #0x34]
	str r0, [sp, #0x28]
	str r6, [sp, #0x2c]
	ldr r2, [r6, #8]
	ldr r6, [r6, #4]
	sub r7, r0, r2
	mov r2, r7, asr #2
	add r2, r7, r2, lsr #29
	cmp r6, r2, asr #3
	ble _02239788
	ldr r2, [r5]
	str r2, [r0]
	ldr r2, [r5, #4]
	str r2, [r0, #4]
	b _022397B0
_02239788:
	cmp r0, #0
	beq _022397A0
	ldr r6, [r5]
	ldr r2, [r5, #4]
	str r6, [r0]
	str r2, [r0, #4]
_022397A0:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_022397B0:
	ldr r0, [sp, #0x30]
	add r5, r5, #8
	add r0, r0, #8
	str r0, [sp, #0x30]
	cmp r5, r3
	bne _02239748
	str r5, [sp, #0x38]
_022397CC:
	ldr r6, [sp, #0x30]
	ldr r5, [sp, #0x34]
	str r6, [sp, #4]
	ldr r0, [sp, #0xa8]
	str r5, [sp, #8]
	str r0, [sp, #0xc]
	str r4, [sp, #0x10]
	add r1, sp, #0xa0
	ldr r3, [sp, #0xb8]
	sub r4, sp, #4
	ldr r2, [sp, #0xbc]
	str r3, [r4]
	str r2, [r4, #4]
	add r0, sp, #0xa4
	ldr r2, [r0]
	ldr r3, [r4]
	ldr r1, [r1]
	add r0, sp, #0x70
	str r6, [sp, #0x68]
	str r5, [sp, #0x6c]
	bl ov45_02239BBC
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_02239830:
	cmp r8, r7
	bgt _02239890
	add r1, sp, #0xa0
	add r2, sp, #0xa4
	add r0, r7, r7, lsr #31
	mov r6, r0, asr #1
	ldr r3, [sp, #0xa4]
	ldr r1, [r1]
	add r3, r3, r6, lsl #3
	ldr r2, [r2]
	add r0, sp, #0x64
	str r4, [sp]
	str r3, [sp, #0x80]
	bl ov45_02239B3C
	ldr r3, [sp, #0x64]
	ldr r2, [sp, #0xa0]
	str r3, [sp, #0x84]
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	mov r5, r0, asr #3
	b _022398E4
_02239890:
	add r1, sp, #0xa4
	add r2, sp, #0xa8
	add r0, r8, r8, lsr #31
	mov r5, r0, asr #1
	ldr r3, [sp, #0xa0]
	ldr r1, [r1]
	add r3, r3, r5, lsl #3
	ldr r2, [r2]
	add r0, sp, #0x60
	str r4, [sp]
	str r3, [sp, #0x84]
	bl ov45_02239ABC
	ldr r3, [sp, #0x60]
	ldr r2, [sp, #0xa4]
	str r3, [sp, #0x80]
	sub r1, r3, r2
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	str r3, [sp, #0x18]
	str r2, [sp, #0x1c]
	mov r6, r0, asr #3
_022398E4:
	add r1, sp, #0x84
	add r2, sp, #0xa4
	add r0, sp, #0x80
	ldr r3, [r0]
	ldr r1, [r1]
	ldr r2, [r2]
	add r0, sp, #0x5c
	bl ov45_022399A8
	ldr r2, [sp, #0x5c]
	add r0, sp, #0xa0
	add r1, sp, #0x84
	ldr r3, [sp, #0xb8]
	stmia sp, {r5, r6}
	str r3, [sp, #8]
	ldr ip, [sp, #0xbc]
	ldr r3, [sp, #0xc0]
	str ip, [sp, #0xc]
	str r3, [sp, #0x10]
	ldr ip, [sp, #0xc4]
	ldr r0, [r0]
	ldr r1, [r1]
	mov r3, r4
	str ip, [sp, #0x14]
	str r2, [sp, #0xa4]
	bl ov45_02239518
	sub r2, r8, r5
	str r2, [sp]
	sub r5, r7, r6
	str r5, [sp, #4]
	ldr r3, [sp, #0xb8]
	add r0, sp, #0xa4
	add r1, sp, #0x80
	add r2, sp, #0xa8
	str r3, [sp, #8]
	ldr r5, [sp, #0xbc]
	ldr r3, [sp, #0xc0]
	str r5, [sp, #0xc]
	str r3, [sp, #0x10]
	ldr r5, [sp, #0xc4]
	ldr r0, [r0]
	ldr r1, [r1]
	ldr r2, [r2]
	mov r3, r4
	str r5, [sp, #0x14]
	bl ov45_02239518
	add sp, sp, #0x88
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239518

	arm_func_start ov45_022399A8
ov45_022399A8: ; 0x022399A8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	ldr r4, [sp, #0x24]
	ldr lr, [sp, #0x28]
	cmp r4, lr
	ldreq r1, [sp, #0x2c]
	addeq sp, sp, #0x10
	streq r1, [r0]
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr ip, [sp, #0x2c]
	cmp lr, ip
	ldreq r1, [sp, #0x24]
	addeq sp, sp, #0x10
	streq r1, [r0]
	ldmeqia sp!, {r4, r5, r6, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r5, [sp, #0x28]
_022399FC:
	ldr r3, [r4]
	ldr r2, [r4, #4]
	ldr r1, [r5]
	str r3, [sp, #8]
	str r1, [r4]
	ldr r1, [r5, #4]
	str r2, [sp, #0xc]
	str r1, [r4, #4]
	str r3, [r5]
	str r2, [r5, #4]
	add r5, r5, #8
	cmp r5, ip
	add r4, r4, #8
	beq _02239A40
	cmp r4, lr
	moveq lr, r5
	b _022399FC
_02239A40:
	mov r5, r4
	cmp r4, lr
	beq _02239AA8
	ldr r1, [sp, #0x2c]
	mov r6, lr
_02239A54:
	ldr ip, [r4]
	ldr r3, [r4, #4]
	ldr r2, [r6]
	str ip, [sp]
	str r2, [r4]
	ldr r2, [r6, #4]
	str r3, [sp, #4]
	str r2, [r4, #4]
	str ip, [r6]
	str r3, [r6, #4]
	add r6, r6, #8
	cmp r6, r1
	add r4, r4, #8
	bne _02239A9C
	cmp r4, lr
	beq _02239AA8
	mov r6, lr
	b _02239A54
_02239A9C:
	cmp r4, lr
	moveq lr, r6
	b _02239A54
_02239AA8:
	str r5, [r0]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_022399A8

	arm_func_start ov45_02239ABC
ov45_02239ABC: ; 0x02239ABC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr ip, [sp, #0x14]
	ldr r1, [sp, #0x18]
	str ip, [sp]
	sub r2, r1, ip
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov lr, r1, asr #3
	cmp lr, #0
	ble _02239B24
	ldr r4, [r3]
_02239AF0:
	add r1, lr, lr, lsr #31
	mov r3, r1, asr #1
	ldr r1, [ip, r3, lsl #3]
	add r2, ip, r3, lsl #3
	cmp r1, r4
	movge lr, r3
	bge _02239B1C
	add ip, r2, #8
	add r1, r3, #1
	str ip, [sp, #0x14]
	sub lr, lr, r1
_02239B1C:
	cmp lr, #0
	bgt _02239AF0
_02239B24:
	ldr r1, [sp, #0x14]
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239ABC

	arm_func_start ov45_02239B3C
ov45_02239B3C: ; 0x02239B3C
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr ip, [sp, #0x14]
	ldr r1, [sp, #0x18]
	str ip, [sp]
	sub r2, r1, ip
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov lr, r1, asr #3
	cmp lr, #0
	ble _02239BA4
	ldr r4, [r3]
_02239B70:
	add r1, lr, lr, lsr #31
	mov r3, r1, asr #1
	ldr r1, [ip, r3, lsl #3]
	add r2, ip, r3, lsl #3
	cmp r4, r1
	movlt lr, r3
	blt _02239B9C
	add ip, r2, #8
	add r1, r3, #1
	str ip, [sp, #0x14]
	sub lr, lr, r1
_02239B9C:
	cmp lr, #0
	bgt _02239B70
_02239BA4:
	ldr r1, [sp, #0x14]
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239B3C

	arm_func_start ov45_02239BBC
ov45_02239BBC: ; 0x02239BBC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x40
	ldr r3, [sp, #0x54]
	ldr r1, [sp, #0x58]
	cmp r3, r1
	bne _02239C48
	ldr r5, [sp, #0x6c]
	ldr r4, [sp, #0x64]
	ldr r3, [sp, #0x5c]
	ldr r2, [sp, #0x68]
	ldr r1, [sp, #0x60]
	str r5, [sp, #0x2c]
	str r4, [sp, #0x30]
	str r2, [sp, #0x34]
	str r3, [sp, #0x38]
	str r1, [sp, #0x3c]
	cmp r4, r3
	beq _02239C30
_02239C08:
	ldr r1, [sp, #0x30]
	sub r2, r1, #8
	ldr r1, [r2]
	str r2, [sp, #0x30]
	str r1, [r5, #-8]!
	ldr r1, [r2, #4]
	cmp r2, r3
	str r1, [r5, #4]
	bne _02239C08
	str r5, [sp, #0x2c]
_02239C30:
	ldr r1, [sp, #0x2c]
	add sp, sp, #0x40
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02239C48:
	ldr r2, [sp, #0x5c]
	ldr r1, [sp, #0x64]
	cmp r2, r1
	bne _02239CA4
	ldr r3, [sp, #0x6c]
	ldr r2, [sp, #0x58]
	ldr r4, [sp, #0x54]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	str r4, [sp, #0x28]
	cmp r2, r4
	bls _02239C90
_02239C78:
	ldr r1, [r2, #-8]!
	cmp r2, r4
	str r1, [r3, #-8]!
	ldr r1, [r2, #4]
	str r1, [r3, #4]
	bhi _02239C78
_02239C90:
	add sp, sp, #0x40
	str r3, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02239CA4:
	ldr r1, [sp, #0x58]
	ldr r4, [sp, #0x64]
	sub r1, r1, #8
	sub r4, r4, #8
	ldr ip, [sp, #0x6c]
	str r1, [sp, #0x58]
	str r4, [sp, #0x64]
_02239CC0:
	ldr r5, [sp, #0x64]
	ldr lr, [r1]
	ldr r4, [r5]
	cmp r4, lr
	bge _02239D6C
	str lr, [ip, #-8]!
	ldr lr, [r1, #4]
	str ip, [sp, #0x6c]
	str lr, [ip, #4]
	cmp r3, r1
	bne _02239D60
	ldr r1, [sp, #0x64]
	ldr r2, [sp, #0x68]
	add lr, r1, #8
	ldr r3, [sp, #0x5c]
	ldr r1, [sp, #0x60]
	str ip, [sp, #0xc]
	str lr, [sp, #0x64]
	str lr, [sp, #0x10]
	str r2, [sp, #0x14]
	str r3, [sp, #0x18]
	str r1, [sp, #0x1c]
	cmp lr, r3
	beq _02239D48
_02239D20:
	ldr r1, [sp, #0x10]
	sub r2, r1, #8
	ldr r1, [r2]
	str r2, [sp, #0x10]
	str r1, [ip, #-8]!
	ldr r1, [r2, #4]
	cmp r2, r3
	str r1, [ip, #4]
	bne _02239D20
	str ip, [sp, #0xc]
_02239D48:
	ldr r1, [sp, #0xc]
	add sp, sp, #0x40
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02239D60:
	sub r1, r1, #8
	str r1, [sp, #0x58]
	b _02239CC0
_02239D6C:
	str r4, [ip, #-8]!
	ldr lr, [r5, #4]
	str ip, [sp, #0x6c]
	str lr, [ip, #4]
	cmp r2, r5
	bne _02239DD0
	ldr r3, [sp, #0x54]
	add r2, r1, #8
	str ip, [sp]
	str r2, [sp, #0x58]
	str r2, [sp, #4]
	str r3, [sp, #8]
	cmp r2, r3
	bls _02239DBC
_02239DA4:
	ldr r1, [r2, #-8]!
	cmp r2, r3
	str r1, [ip, #-8]!
	ldr r1, [r2, #4]
	str r1, [ip, #4]
	bhi _02239DA4
_02239DBC:
	str ip, [r0]
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02239DD0:
	ldr lr, [sp, #0x64]
	sub lr, lr, #8
	str lr, [sp, #0x64]
	b _02239CC0
	arm_func_end ov45_02239BBC

	arm_func_start ov45_02239DE0
ov45_02239DE0: ; 0x02239DE0
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239DE0

	arm_func_start ov45_02239DF0
ov45_02239DF0: ; 0x02239DF0
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x20
	ldr r4, [sp, #0x44]
	ldr r1, [sp, #0x48]
	ldr ip, [sp, #0x4c]
	ldr r2, [sp, #0x3c]
	b _02239E5C
_02239E10:
	ldr lr, [sp, #0x34]
	ldr r5, [r4]
	ldr r3, [lr]
	cmp r5, r3
	bge _02239E3C
	str r5, [ip]
	ldr r3, [r4, #4]
	add r4, r4, #8
	str r3, [ip, #4]
	str r4, [sp, #0x44]
	b _02239E54
_02239E3C:
	str r3, [ip]
	ldr r3, [sp, #0x34]
	ldr lr, [lr, #4]
	add r3, r3, #8
	str lr, [ip, #4]
	str r3, [sp, #0x34]
_02239E54:
	add ip, ip, #8
	str ip, [sp, #0x4c]
_02239E5C:
	ldr r3, [sp, #0x34]
	cmp r3, r2
	cmpne r4, r1
	bne _02239E10
	ldr r2, [sp, #0x34]
	ldr r1, [sp, #0x3c]
	cmp r2, r1
	bne _02239ED0
	ldr r3, [sp, #0x4c]
	ldr r4, [sp, #0x48]
	ldr r2, [sp, #0x44]
	str r3, [sp, #0x14]
	str r4, [sp, #0x18]
	str r2, [sp, #0x1c]
	cmp r2, r4
	bhs _02239EBC
_02239E9C:
	ldr r1, [r2]
	str r1, [r3]
	ldr r1, [r2, #4]
	add r2, r2, #8
	str r1, [r3, #4]
	cmp r2, r4
	add r3, r3, #8
	blo _02239E9C
_02239EBC:
	add sp, sp, #0x20
	str r3, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_02239ED0:
	ldr lr, [sp, #0x4c]
	ldr ip, [sp, #0x3c]
	ldr r2, [sp, #0x34]
	ldr r3, [sp, #0x40]
	ldr r1, [sp, #0x38]
	str lr, [sp]
	str ip, [sp, #4]
	str r3, [sp, #8]
	str r2, [sp, #0xc]
	str r1, [sp, #0x10]
	cmp r2, ip
	beq _02239F30
_02239F00:
	ldr r3, [sp, #0xc]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0xc]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _02239F00
	str lr, [sp]
_02239F30:
	ldr r1, [sp]
	str r1, [r0]
	add sp, sp, #0x20
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239DF0

	arm_func_start ov45_02239F48
ov45_02239F48: ; 0x02239F48
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x28
	ldr r1, [sp, #0x54]
	ldr ip, [sp, #0x5c]
	ldr r3, [sp, #0x44]
	b _02239FB8
_02239F64:
	ldr r5, [sp, #0x4c]
	ldr lr, [sp, #0x3c]
	ldr r4, [r5]
	ldr r2, [lr]
	cmp r4, r2
	bge _02239F98
	str r4, [ip]
	ldr r2, [sp, #0x4c]
	ldr lr, [r5, #4]
	add r2, r2, #8
	str lr, [ip, #4]
	str r2, [sp, #0x4c]
	b _02239FB0
_02239F98:
	str r2, [ip]
	ldr r2, [sp, #0x3c]
	ldr lr, [lr, #4]
	add r2, r2, #8
	str lr, [ip, #4]
	str r2, [sp, #0x3c]
_02239FB0:
	add ip, ip, #8
	str ip, [sp, #0x5c]
_02239FB8:
	ldr r2, [sp, #0x3c]
	cmp r2, r3
	ldrne r2, [sp, #0x4c]
	cmpne r2, r1
	bne _02239F64
	ldr r2, [sp, #0x3c]
	ldr r1, [sp, #0x44]
	cmp r2, r1
	bne _0223A054
	ldr lr, [sp, #0x5c]
	ldr ip, [sp, #0x54]
	ldr r2, [sp, #0x4c]
	ldr r3, [sp, #0x58]
	ldr r1, [sp, #0x50]
	str lr, [sp, #0x14]
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	str r2, [sp, #0x20]
	str r1, [sp, #0x24]
	cmp r2, ip
	beq _0223A03C
_0223A00C:
	ldr r3, [sp, #0x20]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0x20]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _0223A00C
	str lr, [sp, #0x14]
_0223A03C:
	ldr r1, [sp, #0x14]
	add sp, sp, #0x28
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223A054:
	ldr lr, [sp, #0x5c]
	ldr ip, [sp, #0x44]
	ldr r2, [sp, #0x3c]
	ldr r3, [sp, #0x48]
	ldr r1, [sp, #0x40]
	str lr, [sp]
	str ip, [sp, #4]
	str r3, [sp, #8]
	str r2, [sp, #0xc]
	str r1, [sp, #0x10]
	cmp r2, ip
	beq _0223A0B4
_0223A084:
	ldr r3, [sp, #0xc]
	ldr r2, [r3]
	mov r1, r3
	str r2, [lr]
	add r1, r1, #8
	ldr r2, [r3, #4]
	str r1, [sp, #0xc]
	str r2, [lr, #4]
	cmp r1, ip
	add lr, lr, #8
	bne _0223A084
	str lr, [sp]
_0223A0B4:
	ldr r1, [sp]
	str r1, [r0]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_02239F48

	arm_func_start ov45_0223A0CC
ov45_0223A0CC: ; 0x0223A0CC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x80
	ldr r1, [sp, #0x9c]
	ldr r0, [sp, #0x98]
	ldr r7, [sp, #0xa8]
	sub r1, r1, r0
	mov r0, r1, asr #2
	add r0, r1, r0, lsr #29
	mov r5, r0, asr #3
	cmp r5, #0
	addle sp, sp, #0x80
	ldmleia sp!, {r3, r4, r5, r6, r7, lr}
	addle sp, sp, #0x10
	bxle lr
	cmp r5, #1
	bne _0223A1A0
	ldr r3, [sp, #0xa0]
	ldr r1, [sp, #0xa4]
	str r3, [sp, #0x74]
	str r1, [sp, #0x78]
	ldr r0, [r1, #8]
	ldr r1, [r1, #4]
	sub r2, r3, r0
	mov r0, r2, asr #2
	add r0, r2, r0, lsr #29
	cmp r1, r0, asr #3
	add r2, sp, #0x74
	ble _0223A164
	ldr r1, [sp, #0x98]
	add sp, sp, #0x80
	ldr r0, [r1]
	str r0, [r3]
	ldr r0, [r1, #4]
	str r0, [r3, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223A164:
	cmp r3, #0
	beq _0223A180
	ldr r0, [sp, #0x98]
	ldr r1, [r0]
	ldr r0, [r0, #4]
	str r1, [r3]
	str r0, [r3, #4]
_0223A180:
	ldr r1, [r2, #4]
	add sp, sp, #0x80
	ldr r0, [r1, #4]
	add r0, r0, #1
	str r0, [r1, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223A1A0:
	cmp r5, #2
	bne _0223A39C
	ldr r3, [sp, #0x9c]
	ldr r2, [sp, #0x98]
	ldr r1, [r3, #-8]
	ldr r0, [r2]
	cmp r1, r0
	bge _0223A2B0
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	str r0, [sp, #0x34]
	cmp r2, r3
	movne r0, #1
	moveq r0, #0
	str r1, [sp, #0x30]
	str r3, [sp, #0x38]
	str r2, [sp, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	cmp r0, #0
	add r2, sp, #0x28
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r4, [sp, #0x3c]
	ldr r1, [sp, #0x38]
_0223A218:
	ldr r0, [sp, #0x30]
	ldr r5, [sp, #0x34]
	str r0, [sp, #0x28]
	str r5, [sp, #0x2c]
	ldr r3, [r5, #8]
	sub r1, r1, #8
	sub r6, r0, r3
	mov r3, r6, asr #2
	ldr r5, [r5, #4]
	add r3, r6, r3, lsr #29
	str r1, [sp, #0x38]
	cmp r5, r3, asr #3
	ble _0223A260
	ldr r3, [r1]
	str r3, [r0]
	ldr r3, [r1, #4]
	str r3, [r0, #4]
	b _0223A288
_0223A260:
	cmp r0, #0
	beq _0223A278
	ldr r5, [r1]
	ldr r3, [r1, #4]
	str r5, [r0]
	str r3, [r0, #4]
_0223A278:
	ldr r3, [r2, #4]
	ldr r0, [r3, #4]
	add r0, r0, #1
	str r0, [r3, #4]
_0223A288:
	ldr r3, [sp, #0x30]
	ldr r0, [sp, #0x38]
	add r3, r3, #8
	str r3, [sp, #0x30]
	cmp r4, r0
	bne _0223A218
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223A2B0:
	ldr r0, [sp, #0xa4]
	ldr r1, [sp, #0xa0]
	str r0, [sp, #0x1c]
	cmp r2, r3
	movne r0, #1
	moveq r0, #0
	str r1, [sp, #0x18]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	cmp r0, #0
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	cmp r0, #0
	add r1, sp, #0x10
	addeq sp, sp, #0x80
	ldmeqia sp!, {r3, r4, r5, r6, r7, lr}
	addeq sp, sp, #0x10
	bxeq lr
	ldr r4, [sp, #0x24]
	ldr r3, [sp, #0x20]
_0223A308:
	ldr r0, [sp, #0x18]
	ldr r5, [sp, #0x1c]
	str r0, [sp, #0x10]
	str r5, [sp, #0x14]
	ldr r2, [r5, #8]
	ldr r5, [r5, #4]
	sub r6, r0, r2
	mov r2, r6, asr #2
	add r2, r6, r2, lsr #29
	cmp r5, r2, asr #3
	ble _0223A348
	ldr r2, [r4]
	str r2, [r0]
	ldr r2, [r4, #4]
	str r2, [r0, #4]
	b _0223A370
_0223A348:
	cmp r0, #0
	beq _0223A360
	ldr r5, [r4]
	ldr r2, [r4, #4]
	str r5, [r0]
	str r2, [r0, #4]
_0223A360:
	ldr r2, [r1, #4]
	ldr r0, [r2, #4]
	add r0, r0, #1
	str r0, [r2, #4]
_0223A370:
	ldr r0, [sp, #0x18]
	add r4, r4, #8
	add r0, r0, #8
	str r0, [sp, #0x18]
	cmp r4, r3
	bne _0223A308
	str r4, [sp, #0x24]
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223A39C:
	cmp r5, #0x14
	bgt _0223A3E8
	ldr r3, [sp, #0xa0]
	str r7, [sp, #4]
	sub r4, sp, #4
	add r1, sp, #0x98
	ldr r2, [sp, #0xa4]
	str r3, [r4]
	str r2, [r4, #4]
	add r0, sp, #0x9c
	ldr r2, [r0]
	ldr r3, [r4]
	ldr r1, [r1]
	add r0, sp, #0x6c
	bl ov45_0223A7E4
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
_0223A3E8:
	add r0, r5, r5, lsr #31
	sub r4, r5, r0, asr #1
	ldr r1, [sp, #0xa0]
	ldr r6, [sp, #0xa4]
	add ip, r1, r4, lsl #3
	ldr r0, [sp, #0x98]
	str ip, [sp]
	add r1, r0, r4, lsl #3
	add r3, sp, #0x98
	str r6, [sp, #4]
	ldr r0, [r3]
	add r2, sp, #0xa0
	str r1, [sp, #0x7c]
	str ip, [sp, #0x64]
	str r6, [sp, #0x68]
	str r7, [sp, #8]
	ldmia r2, {r2, r3}
	bl ov45_02239294
	ldr r2, [sp, #0xa0]
	add r0, sp, #0x7c
	add ip, r2, r5, lsl #3
	add r3, r2, r4, lsl #3
	add r1, sp, #0x9c
	str ip, [sp]
	str r6, [sp, #4]
	ldr r0, [r0]
	ldr r1, [r1]
	add r2, sp, #0x5c
	str r6, [sp, #0x60]
	str r3, [sp, #0x5c]
	str ip, [sp, #0x54]
	str r6, [sp, #0x58]
	str r7, [sp, #8]
	ldmia r2, {r2, r3}
	bl ov45_02239294
	ldr r1, [sp, #0x98]
	ldr r3, [sp, #0xa0]
	add r2, r1, r4, lsl #3
	add r4, r1, r5, lsl #3
	str r4, [sp]
	str r3, [sp, #4]
	ldr r1, [sp, #0xa4]
	add r0, sp, #0x98
	str r1, [sp, #8]
	ldr r1, [r0]
	add r0, sp, #0x4c
	mov r3, r2
	str r4, [sp, #0x40]
	str r2, [sp, #0x44]
	str r2, [sp, #0x48]
	str r7, [sp, #0xc]
	bl ov45_0223A4C8
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, r6, r7, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0223A0CC

	arm_func_start ov45_0223A4C8
ov45_0223A4C8: ; 0x0223A4C8
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x40
	ldr r3, [sp, #0x64]
	ldr r5, [sp, #0x68]
	ldr r1, [sp, #0x5c]
	ldr r6, [sp, #0x60]
	add ip, sp, #0x38
	add lr, sp, #0x30
	b _0223A5F0
_0223A4F0:
	ldr r4, [r3]
	ldr r2, [r1]
	cmp r4, r2
	bge _0223A574
	ldr r2, [sp, #0x6c]
	ldr r7, [sp, #0x70]
	str r2, [sp, #0x38]
	str r7, [sp, #0x3c]
	ldr r4, [r7, #8]
	ldr r7, [r7, #4]
	sub r8, r2, r4
	mov r4, r8, asr #2
	add r4, r8, r4, lsr #29
	cmp r7, r4, asr #3
	ble _0223A540
	ldr r4, [r3]
	str r4, [r2]
	ldr r4, [r3, #4]
	str r4, [r2, #4]
	b _0223A568
_0223A540:
	cmp r2, #0
	beq _0223A558
	ldr r7, [r3]
	ldr r4, [r3, #4]
	str r7, [r2]
	str r4, [r2, #4]
_0223A558:
	ldr r4, [ip, #4]
	ldr r2, [r4, #4]
	add r2, r2, #1
	str r2, [r4, #4]
_0223A568:
	add r3, r3, #8
	str r3, [sp, #0x64]
	b _0223A5E4
_0223A574:
	ldr r2, [sp, #0x6c]
	ldr r7, [sp, #0x70]
	str r2, [sp, #0x30]
	str r7, [sp, #0x34]
	ldr r4, [r7, #8]
	ldr r7, [r7, #4]
	sub r8, r2, r4
	mov r4, r8, asr #2
	add r4, r8, r4, lsr #29
	cmp r7, r4, asr #3
	ble _0223A5B4
	ldr r4, [r1]
	str r4, [r2]
	ldr r4, [r1, #4]
	str r4, [r2, #4]
	b _0223A5DC
_0223A5B4:
	cmp r2, #0
	beq _0223A5CC
	ldr r7, [r1]
	ldr r4, [r1, #4]
	str r7, [r2]
	str r4, [r2, #4]
_0223A5CC:
	ldr r4, [lr, #4]
	ldr r2, [r4, #4]
	add r2, r2, #1
	str r2, [r4, #4]
_0223A5DC:
	add r1, r1, #8
	str r1, [sp, #0x5c]
_0223A5E4:
	ldr r2, [sp, #0x6c]
	add r2, r2, #8
	str r2, [sp, #0x6c]
_0223A5F0:
	cmp r1, r6
	cmpne r3, r5
	bne _0223A4F0
	ldr r2, [sp, #0x5c]
	ldr r1, [sp, #0x60]
	cmp r2, r1
	bne _0223A6F8
	ldr r2, [sp, #0x68]
	ldr r1, [sp, #0x64]
	ldr r4, [sp, #0x6c]
	ldr r3, [sp, #0x70]
	str r1, [sp, #0x2c]
	cmp r1, r2
	movne r1, #1
	moveq r1, #0
	str r4, [sp, #0x20]
	str r3, [sp, #0x24]
	str r2, [sp, #0x28]
	cmp r1, #0
	beq _0223A6D8
	cmp r1, #0
	add r2, sp, #0x18
	beq _0223A6D8
	ldr r5, [sp, #0x2c]
	ldr r4, [sp, #0x28]
_0223A654:
	ldr r1, [sp, #0x20]
	ldr ip, [sp, #0x24]
	str r1, [sp, #0x18]
	str ip, [sp, #0x1c]
	ldr r3, [ip, #8]
	ldr ip, [ip, #4]
	sub lr, r1, r3
	mov r3, lr, asr #2
	add r3, lr, r3, lsr #29
	cmp ip, r3, asr #3
	ble _0223A694
	ldr r3, [r5]
	str r3, [r1]
	ldr r3, [r5, #4]
	str r3, [r1, #4]
	b _0223A6BC
_0223A694:
	cmp r1, #0
	beq _0223A6AC
	ldr ip, [r5]
	ldr r3, [r5, #4]
	str ip, [r1]
	str r3, [r1, #4]
_0223A6AC:
	ldr r3, [r2, #4]
	ldr r1, [r3, #4]
	add r1, r1, #1
	str r1, [r3, #4]
_0223A6BC:
	ldr r1, [sp, #0x20]
	add r5, r5, #8
	add r1, r1, #8
	str r1, [sp, #0x20]
	cmp r5, r4
	bne _0223A654
	str r5, [sp, #0x2c]
_0223A6D8:
	ldr r2, [sp, #0x20]
	ldr r1, [sp, #0x24]
	str r2, [r0]
	str r1, [r0, #4]
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
_0223A6F8:
	ldr r2, [sp, #0x60]
	ldr r1, [sp, #0x5c]
	ldr r4, [sp, #0x6c]
	ldr r3, [sp, #0x70]
	str r1, [sp, #0x14]
	cmp r1, r2
	movne r1, #1
	moveq r1, #0
	str r4, [sp, #8]
	str r3, [sp, #0xc]
	str r2, [sp, #0x10]
	cmp r1, #0
	beq _0223A7C4
	cmp r1, #0
	add r2, sp, #0
	beq _0223A7C4
	ldr ip, [sp, #0x14]
	ldr r4, [sp, #0x10]
_0223A740:
	ldr r1, [sp, #8]
	ldr r5, [sp, #0xc]
	str r1, [sp]
	str r5, [sp, #4]
	ldr r3, [r5, #8]
	ldr lr, [r5, #4]
	sub r5, r1, r3
	mov r3, r5, asr #2
	add r3, r5, r3, lsr #29
	cmp lr, r3, asr #3
	ble _0223A780
	ldr r3, [ip]
	str r3, [r1]
	ldr r3, [ip, #4]
	str r3, [r1, #4]
	b _0223A7A8
_0223A780:
	cmp r1, #0
	beq _0223A798
	ldr lr, [ip]
	ldr r3, [ip, #4]
	str lr, [r1]
	str r3, [r1, #4]
_0223A798:
	ldr r3, [r2, #4]
	ldr r1, [r3, #4]
	add r1, r1, #1
	str r1, [r3, #4]
_0223A7A8:
	ldr r1, [sp, #8]
	add ip, ip, #8
	add r1, r1, #8
	str r1, [sp, #8]
	cmp ip, r4
	bne _0223A740
	str ip, [sp, #0x14]
_0223A7C4:
	ldr r2, [sp, #8]
	ldr r1, [sp, #0xc]
	str r2, [r0]
	str r1, [r0, #4]
	add sp, sp, #0x40
	ldmia sp!, {r4, r5, r6, r7, r8, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0223A4C8

	arm_func_start ov45_0223A7E4
ov45_0223A7E4: ; 0x0223A7E4
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x18
	ldr r3, [sp, #0x3c]
	ldr r1, [sp, #0x40]
	ldr r4, [sp, #0x48]
	cmp r3, r1
	ldr ip, [sp, #0x44]
	beq _0223A9A8
	ldr r1, [sp, #0x44]
	str r4, [sp, #0x14]
	str r1, [sp, #0x10]
	ldr r2, [r4, #8]
	ldr r5, [r4, #4]
	sub r6, r1, r2
	mov r2, r6, asr #2
	add r2, r6, r2, lsr #29
	cmp r5, r2, asr #3
	add r6, sp, #0x10
	ble _0223A84C
	ldr r5, [sp, #0x3c]
	ldr r2, [r5]
	str r2, [r1]
	ldr r2, [r5, #4]
	str r2, [r1, #4]
	b _0223A878
_0223A84C:
	cmp r1, #0
	beq _0223A868
	ldr r2, [sp, #0x3c]
	ldr r5, [r2]
	ldr r2, [r2, #4]
	str r5, [r1]
	str r2, [r1, #4]
_0223A868:
	ldr r2, [r6, #4]
	ldr r1, [r2, #4]
	add r1, r1, #1
	str r1, [r2, #4]
_0223A878:
	ldr r1, [sp, #0x40]
	add r3, r3, #8
	cmp r3, r1
	movne r1, #1
	moveq r1, #0
	add ip, ip, #8
	cmp r1, #0
	beq _0223A9A8
	cmp r1, #0
	add r2, sp, #0
	beq _0223A9A8
_0223A8A4:
	mov lr, ip
	mov r5, ip
	ldr r8, [sp, #0x44]
	add r1, sp, #8
	b _0223A91C
_0223A8B8:
	str lr, [sp, #8]
	str r4, [sp, #0xc]
	ldr r6, [r4, #8]
	ldr r7, [r4, #4]
	sub sb, lr, r6
	mov r6, sb, asr #2
	add r6, sb, r6, lsr #29
	cmp r7, r6, asr #3
	ble _0223A8F0
	ldr r6, [r5]
	str r6, [lr]
	ldr r6, [r5, #4]
	str r6, [lr, #4]
	b _0223A918
_0223A8F0:
	cmp lr, #0
	beq _0223A908
	ldr r7, [r5]
	ldr r6, [r5, #4]
	str r7, [lr]
	str r6, [lr, #4]
_0223A908:
	ldr r7, [r1, #4]
	ldr r6, [r7, #4]
	add r6, r6, #1
	str r6, [r7, #4]
_0223A918:
	sub lr, lr, #8
_0223A91C:
	cmp r5, r8
	beq _0223A934
	ldr r7, [r3]
	ldr r6, [r5, #-8]!
	cmp r7, r6
	blt _0223A8B8
_0223A934:
	str lr, [sp]
	str r4, [sp, #4]
	ldr r1, [r4, #8]
	ldr r5, [r4, #4]
	sub r6, lr, r1
	mov r1, r6, asr #2
	add r1, r6, r1, lsr #29
	cmp r5, r1, asr #3
	ble _0223A96C
	ldr r1, [r3]
	str r1, [lr]
	ldr r1, [r3, #4]
	str r1, [lr, #4]
	b _0223A994
_0223A96C:
	cmp lr, #0
	beq _0223A984
	ldr r5, [r3]
	ldr r1, [r3, #4]
	str r5, [lr]
	str r1, [lr, #4]
_0223A984:
	ldr r5, [r2, #4]
	ldr r1, [r5, #4]
	add r1, r1, #1
	str r1, [r5, #4]
_0223A994:
	ldr r1, [sp, #0x40]
	add r3, r3, #8
	cmp r3, r1
	add ip, ip, #8
	bne _0223A8A4
_0223A9A8:
	str ip, [r0]
	str r4, [r0, #4]
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0223A7E4

	arm_func_start ov45_0223A9C0
ov45_0223A9C0: ; 0x0223A9C0
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	ldr ip, [sp, #0x18]
	ldr r3, [sp, #0x1c]
	cmp ip, r3
	ldrne r0, [sp, #0x18]
	addne lr, r0, #8
	cmpne lr, r3
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, lr}
	addeq sp, sp, #0x10
	bxeq lr
_0223A9F4:
	sub r5, lr, #8
	ldr r1, [lr]
	ldr r0, [r5]
	cmp r1, r0
	bge _0223AA64
	ldr r2, [lr]
	ldr r1, [lr, #4]
	mov r4, r5
	str r0, [lr]
	ldr r0, [r5, #4]
	str r2, [sp]
	str r1, [sp, #4]
	str r0, [lr, #4]
	b _0223AA40
_0223AA2C:
	ldr r0, [r5]
	str r0, [r4]
	ldr r0, [r5, #4]
	str r0, [r4, #4]
	sub r4, r4, #8
_0223AA40:
	cmp r5, ip
	beq _0223AA54
	ldr r0, [r5, #-8]!
	cmp r2, r0
	blt _0223AA2C
_0223AA54:
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [r4]
	str r0, [r4, #4]
_0223AA64:
	add lr, lr, #8
	cmp lr, r3
	bne _0223A9F4
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	arm_func_end ov45_0223A9C0

	arm_func_start ov45_0223AA80
ov45_0223AA80: ; 0x0223AA80
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	mov r2, #0
	ldr r0, _0223AADC ; =ov45_02255198
	str r2, [r6]
	ldr r0, [r0, #4]
	mov r5, r1
	str r0, [r6, #4]
	cmp r5, #0
	ldmleia sp!, {r4, r5, r6, pc}
	ldr r4, _0223AAE0 ; =_ZSt7nothrow
_0223AAAC:
	mov r1, r4
	mov r0, r5, lsl #3
	bl _ZnamRKSt9nothrow_t
	str r0, [r6]
	cmp r0, #0
	strne r5, [r6, #4]
	ldmneia sp!, {r4, r5, r6, pc}
	add r0, r5, r5, lsr #31
	mov r5, r0, asr #1
	cmp r5, #0
	bgt _0223AAAC
	ldmia sp!, {r4, r5, r6, pc}
	.balign 4, 0
_0223AADC: .word ov45_02255198
_0223AAE0: .word _ZSt7nothrow
	arm_func_end ov45_0223AA80

	arm_func_start ov45_0223AAE4
ov45_0223AAE4: ; 0x0223AAE4
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r1
	mov r4, r2
	sub r2, r4, r5
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov ip, r1, asr #3
	mov r6, r0
	str ip, [sp, #4]
	ldr r3, [r6, #8]
	cmp r3, r1, asr #3
	bhs _0223AB30
	mov r2, #0
	sub r1, ip, r3
	strb r2, [sp, #3]
	bl ov45_0223BED0
	mov r2, r0
	b _0223AC10
_0223AB30:
	ldr r1, [r6, #4]
	mov r0, r5
	cmp r1, ip
	addlo r1, r6, #4
	addhs r1, sp, #4
	ldr r1, [r1]
	ldr r2, [r6]
	add r0, r0, r1, lsl #3
	cmp r5, r0
	bhs _0223AB78
_0223AB58:
	ldr r1, [r5]
	str r1, [r2]
	ldr r1, [r5, #4]
	add r5, r5, #8
	str r1, [r2, #4]
	cmp r5, r0
	add r2, r2, #8
	blo _0223AB58
_0223AB78:
	ldr r3, [r6, #4]
	ldr r2, [sp, #4]
	cmp r2, r3
	bhs _0223ABA8
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r6, #4]
	sub r0, r3, r2
	sub r0, r1, r0
	add sp, sp, #8
	str r0, [r6, #4]
	ldmia sp!, {r4, r5, r6, pc}
_0223ABA8:
	cmp r3, r2
	addhs sp, sp, #8
	ldmhsia sp!, {r4, r5, r6, pc}
	mov r1, #0
	strb r1, [sp, #1]
	ldr r2, [r6]
	ldr r1, [r6, #4]
	cmp r0, r4
	addeq sp, sp, #8
	add r3, r2, r1, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223ABD4:
	cmp r3, #0
	beq _0223ABEC
	ldr r2, [r0]
	ldr r1, [r0, #4]
	str r2, [r3]
	str r1, [r3, #4]
_0223ABEC:
	ldr r1, [r6, #4]
	add r0, r0, #8
	add r1, r1, #1
	str r1, [r6, #4]
	cmp r0, r4
	add r3, r3, #8
	bne _0223ABD4
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
_0223AC10:
	ldr r1, [sp, #4]
	mov r0, r6
	bl ov45_0223BE60
	mov r0, #0
	strb r0, [sp]
	cmp r5, r4
	ldr r1, [r6]
	ldr r0, [r6, #4]
	addeq sp, sp, #8
	add r2, r1, r0, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223AC3C:
	cmp r2, #0
	beq _0223AC54
	ldr r1, [r5]
	ldr r0, [r5, #4]
	str r1, [r2]
	str r0, [r2, #4]
_0223AC54:
	ldr r0, [r6, #4]
	add r5, r5, #8
	add r0, r0, #1
	str r0, [r6, #4]
	cmp r5, r4
	add r2, r2, #8
	bne _0223AC3C
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_0223AAE4

	arm_func_start ov45_0223AC78
ov45_0223AC78: ; 0x0223AC78
	cmp r2, #0x10
	movlo r0, #0
	bxlo lr
	ldrh r0, [r1, #0xe]
	sub r0, r0, #1
	mov r0, r0, lsl #3
	add r0, r0, #0x18
	cmp r2, r0
	movhs r0, #1
	movlo r0, #0
	bx lr
	arm_func_end ov45_0223AC78

	arm_func_start ov45_0223ACA4
ov45_0223ACA4: ; 0x0223ACA4
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	mov r6, r0
	mov r5, r1
	mov r4, r2
	bl ov45_0223AE84
	mov r1, #0
	bic r2, r4, #7
	strb r1, [sp]
	sub r0, sp, #4
	strb r1, [r0]
	ldr r3, [r0]
	mov r1, r5
	add r0, r6, #0x50
	add r2, r5, r2
	bl ov45_0223ACF0
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_0223ACA4

	arm_func_start ov45_0223ACF0
ov45_0223ACF0: ; 0x0223ACF0
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #8
	mov r5, r1
	mov r4, r2
	sub r2, r4, r5
	mov r1, r2, asr #2
	add r1, r2, r1, lsr #29
	mov ip, r1, asr #3
	mov r6, r0
	str ip, [sp, #4]
	ldr r3, [r6, #8]
	cmp r3, r1, asr #3
	bhs _0223AD3C
	mov r2, #0
	sub r1, ip, r3
	strb r2, [sp, #3]
	bl ov45_0223BD94
	mov r2, r0
	b _0223AE1C
_0223AD3C:
	ldr r1, [r6, #4]
	mov r0, r5
	cmp r1, ip
	addlo r1, r6, #4
	addhs r1, sp, #4
	ldr r1, [r1]
	ldr r2, [r6]
	add r0, r0, r1, lsl #3
	cmp r5, r0
	bhs _0223AD84
_0223AD64:
	ldr r1, [r5]
	str r1, [r2]
	ldr r1, [r5, #4]
	add r5, r5, #8
	str r1, [r2, #4]
	cmp r5, r0
	add r2, r2, #8
	blo _0223AD64
_0223AD84:
	ldr r3, [r6, #4]
	ldr r2, [sp, #4]
	cmp r2, r3
	bhs _0223ADB4
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r6, #4]
	sub r0, r3, r2
	sub r0, r1, r0
	add sp, sp, #8
	str r0, [r6, #4]
	ldmia sp!, {r4, r5, r6, pc}
_0223ADB4:
	cmp r3, r2
	addhs sp, sp, #8
	ldmhsia sp!, {r4, r5, r6, pc}
	mov r1, #0
	strb r1, [sp, #1]
	ldr r2, [r6]
	ldr r1, [r6, #4]
	cmp r0, r4
	addeq sp, sp, #8
	add r3, r2, r1, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223ADE0:
	cmp r3, #0
	beq _0223ADF8
	ldr r2, [r0]
	ldr r1, [r0, #4]
	str r2, [r3]
	str r1, [r3, #4]
_0223ADF8:
	ldr r1, [r6, #4]
	add r0, r0, #8
	add r1, r1, #1
	str r1, [r6, #4]
	cmp r0, r4
	add r3, r3, #8
	bne _0223ADE0
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
_0223AE1C:
	ldr r1, [sp, #4]
	mov r0, r6
	bl ov45_0223BD24
	mov r0, #0
	strb r0, [sp]
	cmp r5, r4
	ldr r1, [r6]
	ldr r0, [r6, #4]
	addeq sp, sp, #8
	add r2, r1, r0, lsl #3
	ldmeqia sp!, {r4, r5, r6, pc}
_0223AE48:
	cmp r2, #0
	beq _0223AE60
	ldr r1, [r5]
	ldr r0, [r5, #4]
	str r1, [r2]
	str r0, [r2, #4]
_0223AE60:
	ldr r0, [r6, #4]
	add r5, r5, #8
	add r0, r0, #1
	str r0, [r6, #4]
	cmp r5, r4
	add r2, r2, #8
	bne _0223AE48
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_0223ACF0

	arm_func_start ov45_0223AE84
ov45_0223AE84: ; 0x0223AE84
	stmdb sp!, {r3}
	sub sp, sp, #4
	tst r2, #7
	addne sp, sp, #4
	movne r0, #0
	ldmneia sp!, {r3}
	bxne lr
	cmp r2, #0
	addne sp, sp, #4
	movne r0, #1
	ldmneia sp!, {r3}
	bxne lr
	ldr r2, [r0, #0x54]
	mov r1, #0
	strb r1, [sp]
	ldr r1, [r0, #0x54]
	sub r1, r1, r2
	str r1, [r0, #0x54]
	mov r0, #1
	add sp, sp, #4
	ldmia sp!, {r3}
	bx lr
	arm_func_end ov45_0223AE84

	arm_func_start ov45_0223AEDC
ov45_0223AEDC: ; 0x0223AEDC
	stmdb sp!, {r3, lr}
	ldr r2, [r0, #0x334]
	cmp r2, #0
	moveq r0, #0
	ldmeqia sp!, {r3, pc}
	add r0, r0, #0x5c
	mov r2, #0x2d8
	bl MI_CpuCopy8
	mov r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end ov45_0223AEDC

	arm_func_start ov45_0223AF04
ov45_0223AF04: ; 0x0223AF04
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r0, r1
	add r1, r4, #0x5c
	mov r2, #0x2d8
	bl MI_CpuCopy8
	mov r0, #1
	str r0, [r4, #0x334]
	ldmia sp!, {r4, pc}
	arm_func_end ov45_0223AF04

	arm_func_start ov45_0223AF28
ov45_0223AF28: ; 0x0223AF28
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x3c
	mov r4, r0
	ldr r1, [r4]
	cmp r1, #5
	addls pc, pc, r1, lsl #2
	b _0223B0F8
_0223AF44: ; jump table
	b _0223B0F8 ; case 0
	b _0223B0F8 ; case 1
	b _0223B0F8 ; case 2
	b _0223B0F8 ; case 3
	b _0223AF5C ; case 4
	b _0223AFE0 ; case 5
_0223AF5C:
	bl ov45_0223B104
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	bl ov45_022310B0
	mov r1, r0
	add r0, sp, #0x30
	add r1, r1, #0xb8
	bl ov45_022320B4
	ldr r0, [sp, #0x30]
	ldr r1, _0223B100 ; =ov45_02254D9C
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	bl ov45_02241D58
	add r0, sp, #0x30
	bl ov45_02237BC8
	mov r1, #0
	str r1, [sp, #0x10]
	mov r5, #4
	add r0, sp, #0x10
	str r1, [sp]
	str r0, [sp, #4]
	mov r3, r1
	sub r0, r5, #5
	mov r2, #1
	str r5, [sp, #8]
	bl ov45_0223325C
	mov r0, r4
	bl ov45_0223B1C8
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223AFE0:
	ldr r0, [r4, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r5, [r4, #0x38]
	ldr r0, [r4, #0x44]
	ldr r1, [r4, #0x4c]
	add r0, r0, r5, lsr #7
	bl _u32_div_f
	ldr r2, [r4, #0x40]
	add r0, sp, #0x14
	ldr r6, [r2, r1, lsl #2]
	mov r5, r5, lsl #0x19
	add r1, sp, #0x24
	add r7, r6, r5, lsr #22
	bl ov00_021ECB94
	add r0, sp, #0x14
	add r1, sp, #0x24
	bl RTC_ConvertDateTimeToSecond
	mov r8, r0
	ldr r2, [r4, #0x10]
	ldr r3, [r6, r5, lsr #22]
	ldr r0, [r4, #0x14]
	mov sl, r1
	subs r2, r8, r2
	sbc r1, sl, r0
	subs r0, r2, r3
	sbcs r0, r1, r3, asr #31
	addlt sp, sp, #0x3c
	ldmltia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	sub r5, sp, #4
	add r6, sp, #0xc
	mov fp, #0
_0223B064:
	bl ov45_022310B0
	mov r1, r0
	ldr r0, [r7, #4]
	ldr r1, [r1, #0x30]
	blx r1
	strb fp, [r6]
	and r2, fp, #0xff
	strb r2, [r5]
	ldr r2, [r5]
	add r0, r4, #0x38
	mov r1, #1
	bl ov45_0223B81C
	ldr r0, [r4, #0x3c]
	cmp r0, #0
	addeq sp, sp, #0x3c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr sb, [r4, #0x38]
	ldr r0, [r4, #0x44]
	ldr r1, [r4, #0x4c]
	add r0, r0, sb, lsr #7
	bl _u32_div_f
	ldr r0, [r4, #0x40]
	mov r2, sb, lsl #0x19
	ldr r1, [r0, r1, lsl #2]
	ldr r0, [r1, r2, lsr #22]
	add r1, r1, r2, lsr #22
	str r0, [r7]
	ldr r0, [r1, #4]
	str r0, [r7, #4]
	ldr r1, [r4, #0x10]
	ldr r3, [r7]
	ldr r0, [r4, #0x14]
	subs r2, r8, r1
	sbc r1, sl, r0
	subs r0, r2, r3
	sbcs r0, r1, r3, asr #31
	bge _0223B064
_0223B0F8:
	add sp, sp, #0x3c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0223B100: .word ov45_02254D9C
	arm_func_end ov45_0223AF28

	arm_func_start ov45_0223B104
ov45_0223B104: ; 0x0223B104
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #5
	beq _0223B12C
	bl ov45_022310B0
	ldr r0, [r0]
	cmp r0, #8
	beq _0223B138
_0223B12C:
	add sp, sp, #0x1c
	mov r0, #0
	ldmia sp!, {r3, r4, pc}
_0223B138:
	ldr r2, [r4, #0x10]
	ldr r1, [r4, #0x14]
	mov r0, #0
	cmp r1, r0
	cmpeq r2, r0
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, pc}
	ldr r0, [r4, #0x18]
	cmp r0, #0
	addne sp, sp, #0x1c
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	bl ov45_022310B0
	add r0, r0, #0xe8
	bl ov45_0223DFC8
	cmp r0, #0
	addne sp, sp, #0x1c
	movne r0, #1
	ldmneia sp!, {r3, r4, pc}
	add r0, sp, #0
	add r1, sp, #0x10
	bl ov00_021ECB94
	add r0, sp, #0
	add r1, sp, #0x10
	bl RTC_ConvertDateTimeToSecond
	ldr ip, [r4, #8]
	ldr r2, [r4, #0x1c]
	ldr r3, [r4, #0xc]
	adds r4, ip, r2
	adc r2, r3, #0
	subs r0, r4, r0
	sbcs r0, r2, r1
	movlt r0, #1
	movge r0, #0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_0223B104

	arm_func_start ov45_0223B1C8
ov45_0223B1C8: ; 0x0223B1C8
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x1c
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #5
	beq _0223B23C
	bl ov45_022310B0
	ldr r0, [r0]
	cmp r0, #8
	bne _0223B23C
	add r0, sp, #0
	add r1, sp, #0x10
	bl ov00_021ECB94
	add r0, sp, #0
	add r1, sp, #0x10
	bl RTC_ConvertDateTimeToSecond
	str r0, [r4, #0x10]
	str r1, [r4, #0x14]
	mov r0, #5
	str r0, [r4]
	mov r0, #0
	str r0, [r4, #0x18]
	bl ov45_022310B0
	ldr r1, [r0, #0x30]
	mov r0, #0
	blx r1
	add sp, sp, #0x1c
	mov r0, #1
	ldmia sp!, {r3, r4, pc}
_0223B23C:
	mov r0, #1
	str r0, [r4, #0x18]
	mov r0, #0
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_0223B1C8

	arm_func_start ov45_0223B250
ov45_0223B250: ; 0x0223B250
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	mov r5, r1
	ldr r1, [r5]
	mov r4, r2
	mov r6, r0
	add r4, r4, r1
	ldr r3, [r5, #8]
	ldr r1, [r5, #0xc]
	ldr r0, [r5, #0x14]
	add r2, r3, r1, lsl #2
	add r1, r3, r0, lsl #2
	str r1, [sp, #0xc]
	add r0, sp, #0
	str r2, [sp]
	str r3, [sp, #4]
	str r2, [sp, #8]
	mov r1, r4, lsr #7
	bl ov45_0223CD78
	ldr r0, [r5, #0x10]
	ldr r1, [sp]
	cmp r0, #0
	moveq r5, #0
	ldr r0, [sp, #4]
	ldr r2, [sp, #8]
	ldr r3, [sp, #0xc]
	ldrne r5, [r1]
	str r1, [r6]
	stmib r6, {r0, r2}
	mov r0, r4, lsl #0x19
	str r3, [r6, #0xc]
	add r0, r5, r0, lsr #22
	str r0, [r6, #0x10]
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_0223B250

	arm_func_start ov45_0223B2DC
ov45_0223B2DC: ; 0x0223B2DC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xbc
	mov r5, r0
	tst r1, #0x7f
	movne r0, #1
	moveq r0, #0
	mov r8, #0
	add r4, r0, r1, lsr #7
	mov r7, r8
	add r6, sp, #0x28
	b _0223B378
_0223B308:
	ldr r1, [r5, #8]
	ldr r0, [r5, #0xc]
	ldr sb, [r1, r0, lsl #2]
	strb r8, [sp, #0x13]
	ldr r0, [r5, #0xc]
	add r1, r0, #1
	str r1, [r5, #0xc]
	ldr r0, [r5, #0x14]
	cmp r1, r0
	streq r8, [r5, #0xc]
	ldr r0, [r5, #0x10]
	sub r1, r0, #1
	str r1, [r5, #0x10]
	ldr r0, [r5, #0xc]
	add r0, r0, r1
	ldr r1, [r5, #0x14]
	sub r0, r0, #1
	bl _u32_div_f
	ldr r2, [r5, #8]
	add r0, r5, #8
	str sb, [r2, r1, lsl #2]
	mov r1, r6
	str r7, [sp, #0x28]
	bl ov45_0223BA3C
	ldr r0, [r5]
	sub r4, r4, #1
	sub r0, r0, #0x80
	str r0, [r5]
_0223B378:
	cmp r4, #0
	beq _0223B38C
	ldr r0, [r5]
	cmp r0, #0x80
	bhs _0223B308
_0223B38C:
	cmp r4, #0
	addeq sp, sp, #0xbc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, _0223B758 ; =ov45_022577C8
	ldr r2, [r1]
	tst r2, #1
	bne _0223B3BC
	ldr r0, _0223B75C ; =ov45_022551A8
	orr r2, r2, #1
	mvn r3, #0xff
	str r3, [r0]
	str r2, [r1]
_0223B3BC:
	ldr r1, _0223B75C ; =ov45_022551A8
	mov r6, r4, lsl #7
	ldr r0, [r1]
	cmp r0, r4, lsl #7
	blo _0223B418
	ldr r0, _0223B758 ; =ov45_022577C8
	ldr r2, [r0]
	tst r2, #1
	bne _0223B3F0
	orr r2, r2, #1
	mvn r3, #0xff
	str r3, [r1]
	str r2, [r0]
_0223B3F0:
	ldr r1, [r5, #0x10]
	ldr r0, _0223B75C ; =ov45_022551A8
	cmp r1, #0
	ldr r2, [r0]
	moveq r1, #0
	subne r0, r1, #1
	movne r1, r0, lsl #7
	sub r0, r2, r6
	cmp r1, r0
	bls _0223B420
_0223B418:
	ldr r0, _0223B760 ; =ov45_022551AC
	bl ov45_0223B8D0
_0223B420:
	ldr r0, [r5, #0x14]
	cmp r0, #0
	moveq r1, #0
	subne r1, r0, #1
	ldr r0, [r5, #0x10]
	add r0, r4, r0
	cmp r0, r1
	bhi _0223B4A4
	cmp r4, #0
	addeq sp, sp, #0xbc
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	mov r7, #0x400
	mov sb, #0
	add r8, sp, #0x24
_0223B458:
	mov r0, r7
	bl ov45_02242394
	ldr r3, [r5, #0xc]
	ldr r2, [r5, #0x10]
	ldr r1, [r5, #0x14]
	add r2, r3, r2
	mov r6, r0
	sub r0, r2, #1
	bl _u32_div_f
	ldr r2, [r5, #8]
	add r0, r5, #8
	str r6, [r2, r1, lsl #2]
	mov r1, r8
	str sb, [sp, #0x24]
	bl ov45_0223BA3C
	subs r4, r4, #1
	bne _0223B458
	add sp, sp, #0xbc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_0223B4A4:
	movs r1, #0
	add r6, sp, #0xac
	str r1, [sp, #0xa4]
	str r1, [sp, #0xa8]
	str r1, [sp, #0x74]
	str r1, [sp, #0x78]
	str r1, [sp, #0x7c]
	str r1, [sp, #0x80]
	str r1, [r6]
	str r1, [r6, #4]
	str r1, [r6, #8]
	str r1, [r6, #0xc]
	add r8, sp, #0x74
	beq _0223B4E4
	add r0, r1, r1
	bl _u32_div_f
_0223B4E4:
	ldr r7, [r8]
	ldr r2, [r8, #4]
	ldr r0, [r8, #0xc]
	add r3, r7, r2, lsl #2
	add r2, r7, r0, lsl #2
	add r8, r7, r1, lsl #2
	ldrb r1, [sp, #0x12]
	add r0, sp, #0x84
	str r8, [sp, #0x84]
	str r7, [sp, #0x88]
	str r7, [sp, #0x98]
	str r3, [sp, #0x8c]
	str r2, [sp, #0x90]
	str r0, [sp, #4]
	add r0, sp, #0x94
	strb r1, [sp, #8]
	sub r7, sp, #0xc
	str r3, [sp, #0x94]
	str r3, [sp, #0x9c]
	str r2, [sp, #0xa0]
	ldmia r0, {r0, r1, r2, r3}
	stmia r7, {r0, r1, r2, r3}
	mov r0, r6
	ldmia r7, {r1, r2, r3}
	bl ov45_0223CE90
	add r0, sp, #0x74
	bl ov45_02238508
	ldr r0, [r5, #0x10]
	add r8, sp, #0xac
	add r0, r4, r0
	add r0, r0, #1
	str r0, [sp, #0x1c]
	ldr r0, [r5, #0x14]
	cmp r0, #0
	moveq r2, #0
	subne r2, r0, #1
	ldr r0, [sp, #0x1c]
	mov r1, r2, lsl #1
	cmp r0, r2, lsl #1
	addhi r0, sp, #0x1c
	addls r0, sp, #0x20
	str r1, [sp, #0x20]
	ldr r1, [r0]
	mov r0, r8
	bl ov45_0223B8E0
	mov r2, #0
	add r1, sp, #0x18
	mov r0, r8
	str r2, [sp, #0x18]
	bl ov45_0223BA3C
	cmp r4, #0
	beq _0223B604
	mov r7, #0x400
	mov sl, #0
	add sb, sp, #0x14
_0223B5C0:
	mov r0, r7
	bl ov45_02242394
	ldr r3, [r8, #4]
	ldr r2, [r8, #8]
	ldr r1, [r8, #0xc]
	add r2, r3, r2
	mov r6, r0
	sub r0, r2, #1
	bl _u32_div_f
	ldr r2, [r8]
	mov r0, r8
	str r6, [r2, r1, lsl #2]
	mov r1, sb
	str sl, [sp, #0x14]
	bl ov45_0223BA3C
	subs r4, r4, #1
	bne _0223B5C0
_0223B604:
	ldr r0, [r5, #0x10]
	cmp r0, #0
	beq _0223B6F4
	ldr r1, [r5, #0x14]
	cmp r1, #0
	moveq r1, #0
	beq _0223B630
	ldr r2, [r5, #0xc]
	ldr r0, [r5, #0x10]
	add r0, r2, r0
	bl _u32_div_f
_0223B630:
	ldr r4, [r5, #8]
	ldr r2, [r5, #0xc]
	ldr r0, [r5, #0x14]
	add r3, r4, r2, lsl #2
	add r2, r4, r0, lsl #2
	add r1, r4, r1, lsl #2
	str r1, [sp, #0x64]
	add r0, sp, #0x64
	str r4, [sp, #0x68]
	str r3, [sp, #0x6c]
	str r2, [sp, #0x70]
	mvn r1, #0
	bl ov45_0223CD78
	ldr r0, [r5, #8]
	ldr r2, [r5, #0xc]
	ldr r1, [r5, #0x14]
	add sb, r0, r2, lsl #2
	mov r6, #0
	ldr r7, [r8]
	ldr r2, [r8, #0xc]
	ldr r3, [r8, #4]
	add r2, r7, r2, lsl #2
	add r4, r7, r3, lsl #2
	ldr r3, [sp, #0x64]
	str r2, [sp, #0x38]
	ldr r2, [sp, #0x68]
	str r3, [sp, #0x50]
	ldr r3, [sp, #0x6c]
	str r2, [sp, #0x54]
	ldr r2, [sp, #0x70]
	str r7, [sp, #0x30]
	add r7, r0, r1, lsl #2
	str r3, [sp, #0x58]
	str r2, [sp, #0x5c]
	str r0, [sp, #0x40]
	add r1, sp, #0x2c
	add r2, sp, #0x3c
	add r3, sp, #0x50
	str r7, [sp, #0x48]
	strb r6, [sp, #0xe]
	strb r6, [sp, #0xf]
	strb r6, [sp, #0xd]
	mov r0, r8
	str sb, [sp, #0x3c]
	str sb, [sp, #0x44]
	str r4, [sp, #0x2c]
	str r4, [sp, #0x34]
	strb r6, [sp]
	bl ov45_0223BF9C
_0223B6F4:
	ldr r2, [r5, #8]
	ldr r1, [r8]
	add r0, sp, #0xa4
	str r1, [r5, #8]
	str r2, [r8]
	ldr r3, [r5, #0xc]
	ldr r1, [r8, #4]
	mov r2, #0
	str r1, [r5, #0xc]
	str r3, [r8, #4]
	ldr r3, [r5, #0x10]
	ldr r1, [r8, #8]
	str r1, [r5, #0x10]
	str r3, [r8, #8]
	ldr r3, [r5, #0x14]
	ldr r1, [r8, #0xc]
	str r1, [r5, #0x14]
	ldr r1, [r8, #8]
	str r3, [r8, #0xc]
	sub r1, r1, r1
	strb r2, [sp, #0xc]
	str r1, [r8, #8]
	bl ov45_0223CA14
	add sp, sp, #0xbc
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0223B758: .word ov45_022577C8
_0223B75C: .word ov45_022551A8
_0223B760: .word ov45_022551AC
	arm_func_end ov45_0223B2DC

	arm_func_start ov45_0223B764
ov45_0223B764: ; 0x0223B764
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #4
	cmp r1, #0
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r0, [r6, #4]
	sub r0, r0, r1
	str r0, [r6, #4]
	ldr r0, [r6, #0x10]
	cmp r0, #0
	moveq r2, #0
	subne r0, r0, #1
	movne r2, r0, lsl #7
	ldmia r6, {r0, r1}
	add r0, r0, r1
	sub r5, r2, r0
	cmp r5, #0x100
	addlo sp, sp, #4
	ldmloia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	mov r8, #0
	mov r7, r8
	mov r4, r8
_0223B7C0:
	strb r4, [sp]
	ldr r0, [r6, #0x10]
	sub r1, r0, #1
	str r1, [r6, #0x10]
	ldr r0, [r6, #0xc]
	add r0, r0, r1
	ldr r1, [r6, #0x14]
	sub r0, r0, #1
	bl _u32_div_f
	ldr sb, [r6, #8]
	mov sl, r1
	ldr r1, [sb, sl, lsl #2]
	cmp r1, #0
	beq _0223B804
	mov r0, r8
	mov r2, r8
	bl ov00_021EC2EC
_0223B804:
	sub r5, r5, #0x80
	str r7, [sb, sl, lsl #2]
	cmp r5, #0x100
	bhs _0223B7C0
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	arm_func_end ov45_0223B764

	arm_func_start ov45_0223B81C
ov45_0223B81C: ; 0x0223B81C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	cmp r1, #0
	mov r6, r0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r0, [r6, #4]
	sub r0, r0, r1
	str r0, [r6, #4]
	ldr r0, [r6]
	add r0, r0, r1
	str r0, [r6]
	cmp r0, #0x100
	blo _0223B8B8
	mov r5, #0
	mov r4, r5
_0223B85C:
	ldr r1, [r6, #8]
	ldr r0, [r6, #0xc]
	ldr r1, [r1, r0, lsl #2]
	cmp r1, #0
	beq _0223B87C
	mov r0, r5
	mov r2, r5
	bl ov00_021EC2EC
_0223B87C:
	strb r4, [sp]
	ldr r0, [r6, #0xc]
	add r1, r0, #1
	str r1, [r6, #0xc]
	ldr r0, [r6, #0x14]
	cmp r1, r0
	streq r4, [r6, #0xc]
	ldr r0, [r6, #0x10]
	sub r0, r0, #1
	str r0, [r6, #0x10]
	ldr r0, [r6]
	sub r0, r0, #0x80
	str r0, [r6]
	cmp r0, #0x100
	bhs _0223B85C
_0223B8B8:
	ldr r0, [r6, #4]
	cmp r0, #0
	moveq r0, #0x40
	streq r0, [r6]
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov45_0223B81C

	arm_func_start ov45_0223B8D0
ov45_0223B8D0: ; 0x0223B8D0
	stmdb sp!, {r3, lr}
	bl printf
	bl abort
	ldmia sp!, {r3, pc}
	arm_func_end ov45_0223B8D0

	arm_func_start ov45_0223B8E0
ov45_0223B8E0: ; 0x0223B8E0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	mov r4, r0
	ldr r0, [r4, #0xc]
	cmp r0, #0
	moveq r0, #0
	subne r0, r0, #1
	cmp r1, r0
	addls sp, sp, #0x38
	ldmlsia sp!, {r3, r4, r5, pc}
	mov r2, #0
	add r0, r4, #0xc
	str r0, [sp, #0x34]
	str r2, [sp, #0x24]
	str r2, [sp, #0x28]
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	strb r2, [sp, #2]
	sub r0, sp, #4
	strb r2, [r0]
	ldr r2, [r0]
	add r0, sp, #0x24
	bl ov45_0223CB04
	ldr r1, [r4, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223B95C
	ldr r2, [r4, #4]
	ldr r0, [r4, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223B95C:
	ldr r0, [r4]
	mov ip, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #4]
	ldr r2, [r4]
	sub r3, sp, #4
	str r2, [sp, #8]
	ldr r0, [r4, #4]
	add r0, r2, r0, lsl #2
	str r0, [sp, #0xc]
	ldr r1, [r4, #0xc]
	add r0, sp, #0x24
	add r1, r2, r1, lsl #2
	str r1, [sp, #0x10]
	ldr r5, [r4]
	ldr r2, [r4, #4]
	add r1, sp, #0x14
	add r2, r5, r2, lsl #2
	str r2, [sp, #0x14]
	ldr r5, [r4]
	add r2, sp, #4
	str r5, [sp, #0x18]
	ldr lr, [r4, #4]
	add lr, r5, lr, lsl #2
	str lr, [sp, #0x1c]
	ldr lr, [r4, #0xc]
	add lr, r5, lr, lsl #2
	str lr, [sp, #0x20]
	strb ip, [sp, #1]
	strb ip, [r3]
	ldr r3, [r3]
	bl ov45_0223CC78
	mov r0, #0
	strb r0, [sp]
	str r0, [r4, #8]
	ldr r1, [r4]
	ldr r0, [sp, #0x24]
	add r3, sp, #0x30
	str r0, [r4]
	str r1, [sp, #0x24]
	ldr r2, [r4, #4]
	ldr r1, [sp, #0x28]
	add r0, sp, #0x24
	str r1, [r4, #4]
	str r2, [sp, #0x28]
	ldr r2, [r4, #8]
	ldr r1, [sp, #0x2c]
	str r1, [r4, #8]
	str r2, [sp, #0x2c]
	ldr r2, [r4, #0xc]
	ldr r1, [r3]
	str r1, [r4, #0xc]
	str r2, [r3]
	bl ov45_0223CAB4
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223B8E0

	arm_func_start ov45_0223BA3C
ov45_0223BA3C: ; 0x0223BA3C
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x38
	mov r5, r0
	ldr r3, [r5, #8]
	mov r4, r1
	ldr r1, [r5, #0xc]
	add r2, r3, #1
	cmp r2, r1
	bhs _0223BA90
	ldr r0, [r5, #4]
	add r0, r0, r3
	bl _u32_div_f
	ldr r0, [r5]
	add sp, sp, #0x38
	adds r1, r0, r1, lsl #2
	ldrne r0, [r4]
	strne r0, [r1]
	ldr r0, [r5, #8]
	add r0, r0, #1
	str r0, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
_0223BA90:
	mov r3, #0
	add r2, r5, #0xc
	mov r1, #1
	str r3, [sp, #0x24]
	str r3, [sp, #0x28]
	str r3, [sp, #0x2c]
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	bl ov45_0223BBFC
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #2]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #0x24
	strb r3, [sp, #3]
	bl ov45_0223CB04
	ldr r1, [r5, #8]
	ldr r0, [sp, #0x24]
	str r1, [sp, #0x28]
	adds r1, r0, r1, lsl #2
	ldrne r0, [r4]
	strne r0, [r1]
	ldr r0, [sp, #0x2c]
	add r0, r0, #1
	str r0, [sp, #0x2c]
	ldr r1, [r5, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223BB1C
	ldr r2, [r5, #4]
	ldr r0, [r5, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223BB1C:
	ldr r0, [r5]
	mov r4, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #4]
	ldr r2, [r5]
	sub r3, sp, #4
	str r2, [sp, #8]
	ldr r0, [r5, #4]
	add r0, r2, r0, lsl #2
	str r0, [sp, #0xc]
	ldr r1, [r5, #0xc]
	add r0, sp, #0x24
	add r1, r2, r1, lsl #2
	str r1, [sp, #0x10]
	ldr ip, [r5]
	ldr r2, [r5, #4]
	add r1, sp, #0x14
	add r2, ip, r2, lsl #2
	str r2, [sp, #0x14]
	ldr lr, [r5]
	add r2, sp, #4
	str lr, [sp, #0x18]
	ldr ip, [r5, #4]
	add ip, lr, ip, lsl #2
	str ip, [sp, #0x1c]
	ldr ip, [r5, #0xc]
	add ip, lr, ip, lsl #2
	str ip, [sp, #0x20]
	strb r4, [sp, #1]
	strb r4, [r3]
	ldr r3, [r3]
	bl ov45_0223CB6C
	mov r0, r4
	strb r0, [sp]
	str r0, [r5, #8]
	ldr r1, [r5]
	ldr r0, [sp, #0x24]
	add r3, sp, #0x30
	str r0, [r5]
	str r1, [sp, #0x24]
	ldr r2, [r5, #4]
	ldr r1, [sp, #0x28]
	add r0, sp, #0x24
	str r1, [r5, #4]
	str r2, [sp, #0x28]
	ldr r2, [r5, #8]
	ldr r1, [sp, #0x2c]
	str r1, [r5, #8]
	str r2, [sp, #0x2c]
	ldr r2, [r5, #0xc]
	ldr r1, [r3]
	str r1, [r5, #0xc]
	str r2, [r3]
	bl ov45_0223CAB4
	add sp, sp, #0x38
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223BA3C

	arm_func_start ov45_0223BBFC
ov45_0223BBFC: ; 0x0223BBFC
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #8
	ldr r2, _0223BCFC ; =ov45_022577D0
	ldr r3, [r2]
	tst r3, #1
	bne _0223BC2C
	ldr r1, _0223BD00 ; =ov45_022551A4
	orr r3, r3, #1
	mvn r4, #0xc0000001
	str r4, [r1]
	str r3, [r2]
_0223BC2C:
	ldr r1, [r0, #0xc]
	ldr r0, _0223BD00 ; =ov45_022551A4
	cmp r1, #0
	moveq r5, #0
	subne r5, r1, #1
	ldr r4, [r0]
	ldr r1, [sp, #0x1c]
	sub r0, r4, r5
	cmp r1, r0
	bls _0223BC58
	bl ov45_0223BD0C
_0223BC58:
	ldr r0, _0223BD04 ; =0xAAAAAAAB
	umull r0, r1, r4, r0
	mov r1, r1, lsr #1
	cmp r5, r1
	bhs _0223BCAC
	add r1, r5, #1
	ldr r0, _0223BD08 ; =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x1c]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x1c
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r5, r0
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223BCAC:
	cmp r5, r1, lsl #1
	bhs _0223BCE8
	ldr r0, [sp, #0x1c]
	add r1, r5, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x1c
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r5, r0
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
_0223BCE8:
	mov r0, r4
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_0223BCFC: .word ov45_022577D0
_0223BD00: .word ov45_022551A4
_0223BD04: .word 0xAAAAAAAB
_0223BD08: .word 0xCCCCCCCD
	arm_func_end ov45_0223BBFC

	arm_func_start ov45_0223BD0C
ov45_0223BD0C: ; 0x0223BD0C
	stmdb sp!, {r3, lr}
	ldr r0, _0223BD20 ; =ov45_022551C4
	bl printf
	bl abort
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0223BD20: .word ov45_022551C4
	arm_func_end ov45_0223BD0C

	arm_func_start ov45_0223BD24
ov45_0223BD24: ; 0x0223BD24
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0223BD6C
	beq _0223BD60
	mov r2, r0
	bl ov00_021EC2EC
_0223BD60:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0223BD6C:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov45_0223CE30
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223BD24

	arm_func_start ov45_0223BD94
ov45_0223BD94: ; 0x0223BD94
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0xe0000000
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223BDBC
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223BDBC:
	ldr r0, _0223BE58 ; =0x0AAAAAAA
	cmp r4, r0
	bhs _0223BE08
	add r1, r4, #1
	ldr r0, _0223BE5C ; =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223BE08:
	cmp r4, r0, lsl #1
	bhs _0223BE44
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223BE44:
	mvn r0, #0xe0000000
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_0223BE58: .word 0x0AAAAAAA
_0223BE5C: .word 0xCCCCCCCD
	arm_func_end ov45_0223BD94

	arm_func_start ov45_0223BE60
ov45_0223BE60: ; 0x0223BE60
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0223BEA8
	beq _0223BE9C
	mov r2, r0
	bl ov00_021EC2EC
_0223BE9C:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0223BEA8:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov45_0223CE60
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223BE60

	arm_func_start ov45_0223BED0
ov45_0223BED0: ; 0x0223BED0
	stmdb sp!, {r0, r1, r2, r3}
	stmdb sp!, {r4, lr}
	sub sp, sp, #8
	ldr r4, [r0, #8]
	mvn r0, #0xe0000000
	ldr r1, [sp, #0x14]
	sub r0, r0, r4
	cmp r1, r0
	bls _0223BEF8
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223BEF8:
	ldr r0, _0223BF94 ; =0x0AAAAAAA
	cmp r4, r0
	bhs _0223BF44
	add r1, r4, #1
	ldr r0, _0223BF98 ; =0xCCCCCCCD
	add r1, r1, r1, lsl #1
	umull r0, r2, r1, r0
	mov r2, r2, lsr #2
	ldr r0, [sp, #0x14]
	str r2, [sp, #4]
	cmp r0, r2
	addhi r0, sp, #0x14
	addls r0, sp, #4
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223BF44:
	cmp r4, r0, lsl #1
	bhs _0223BF80
	ldr r0, [sp, #0x14]
	add r1, r4, #1
	mov r2, r1, lsr #1
	cmp r0, r1, lsr #1
	addhi r0, sp, #0x14
	str r2, [sp]
	addls r0, sp, #0
	ldr r0, [r0]
	add sp, sp, #8
	add r0, r4, r0
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
_0223BF80:
	mvn r0, #0xe0000000
	add sp, sp, #8
	ldmia sp!, {r4, lr}
	add sp, sp, #0x10
	bx lr
	.balign 4, 0
_0223BF94: .word 0x0AAAAAAA
_0223BF98: .word 0xCCCCCCCD
	arm_func_end ov45_0223BED0

	arm_func_start ov45_0223BF9C
ov45_0223BF9C: ; 0x0223BF9C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1a4
	mov sb, r2
	mov r8, r3
	ldrb r6, [r8, #0x10]
	ldrb r2, [sb, #0x10]
	str r1, [sp]
	mov sl, r0
	ldr ip, [r8, #4]
	ldr fp, [r8, #8]
	ldr r7, [r8, #0xc]
	ldr r5, [sb]
	ldr r4, [sb, #4]
	ldr r3, [sb, #8]
	ldr lr, [sb, #0xc]
	ldr r1, [r8]
	add r0, sp, #0x5c
	str r1, [sp, #0x5c]
	add r1, sp, #0x48
	strb r6, [sp, #0x168]
	strb r2, [sp, #0x17c]
	str ip, [sp, #0x60]
	str fp, [sp, #0x64]
	str r7, [sp, #0x68]
	strb r6, [sp, #0x6c]
	str r5, [sp, #0x48]
	str r4, [sp, #0x4c]
	str r3, [sp, #0x50]
	str lr, [sp, #0x54]
	strb r2, [sp, #0x58]
	bl ov45_0223CDBC
	movs r4, r0
	addeq sp, sp, #0x1a4
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [sp]
	ldr r3, [sl]
	ldr r1, [sl, #4]
	ldr r2, [r0]
	add r7, r3, r1, lsl #2
	sub r1, r2, r7
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	cmp r2, r7
	mov r5, r0, asr #2
	ldrlo r0, [sl, #0xc]
	addlo r5, r5, r0
	ldr r0, [sl, #0xc]
	cmp r0, #0
	moveq r0, #0
	subne r0, r0, #1
	cmp r4, r0
	ldrls r1, [sl, #8]
	subls r0, r0, r4
	cmpls r1, r0
	bhi _0223C364
	str r7, [sp, #0x194]
	ldr r6, [sl]
	str r6, [sp, #0x198]
	ldr r0, [sl, #4]
	add r0, r6, r0, lsl #2
	str r0, [sp, #4]
	str r0, [sp, #0x19c]
	ldr r0, [sl, #0xc]
	add fp, r6, r0, lsl #2
	str fp, [sp, #0x1a0]
	ldr r1, [sl, #8]
	sub r0, r1, r5
	str r0, [sp, #8]
	cmp r5, r0
	bhs _0223C19C
	add r0, sp, #0x194
	rsb r1, r4, #0
	bl ov45_0223CD78
	ldr r1, [sp, #0x194]
	ldr r0, [sl]
	cmp r5, #0
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r0, r4
	str r0, [sl, #8]
	beq _0223C2D8
	ldr r2, [sp, #0x1a0]
	ldr sl, [sp, #0x194]
	ldr r4, [sp, #0x198]
	ldr r3, [sp, #0x19c]
	str r2, [sp, #0x124]
	ldr r2, [sp, #4]
	add r0, sp, #0x38
	mov r1, r5
	str sl, [sp, #0x118]
	str r4, [sp, #0x11c]
	str r3, [sp, #0x120]
	str r7, [sp, #0x38]
	str r6, [sp, #0x3c]
	str r2, [sp, #0x40]
	str fp, [sp, #0x44]
	bl ov45_0223CD78
	ldr r2, [r0]
	add r1, sp, #0x138
	str r2, [sp, #0x128]
	ldr r3, [r0, #4]
	add r2, sp, #0x128
	str r3, [sp, #0x12c]
	ldr r4, [r0, #8]
	add r3, sp, #0x118
	str r4, [sp, #0x130]
	ldr r4, [r0, #0xc]
	add r0, sp, #0x148
	str r4, [sp, #0x134]
	ldr r4, [sp, #4]
	str r7, [sp, #0x138]
	str r6, [sp, #0x13c]
	str r4, [sp, #0x140]
	str fp, [sp, #0x144]
	bl ov45_0223C8A0
	ldr r3, [sp, #0x148]
	ldr r2, [sp, #0x14c]
	ldr r1, [sp, #0x150]
	ldr r0, [sp, #0x154]
	str r3, [sp, #0x194]
	str r2, [sp, #0x198]
	str r1, [sp, #0x19c]
	str r0, [sp, #0x1a0]
	b _0223C2D8
_0223C19C:
	add r0, r1, r4
	str r0, [sl, #8]
	ldr r1, [sl, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223C1C4
	ldr r2, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223C1C4:
	ldr r5, [sl]
	ldr r2, [sl, #4]
	ldr r0, [sl, #0xc]
	add r3, r5, r2, lsl #2
	add r2, r5, r0, lsl #2
	ldr r0, [sp, #8]
	add r1, r5, r1, lsl #2
	str r1, [sp, #0x194]
	str r5, [sp, #0x198]
	str r3, [sp, #0x19c]
	str r2, [sp, #0x1a0]
	cmp r0, #0
	beq _0223C2CC
	add r0, sp, #0x18
	str r1, [sp, #0xd8]
	str r1, [sp, #0x18]
	str r5, [sp, #0xdc]
	str r3, [sp, #0xe0]
	str r2, [sp, #0xe4]
	str r5, [sp, #0x1c]
	str r3, [sp, #0x20]
	str r2, [sp, #0x24]
	rsb r1, r4, #0
	bl ov45_0223CD78
	ldr r2, [r0]
	ldr r1, [sp, #8]
	str r2, [sp, #0xe8]
	ldr r2, [r0, #4]
	add r1, r4, r1
	str r2, [sp, #0xec]
	ldr r2, [r0, #8]
	ldr r6, [sp, #0x194]
	str r2, [sp, #0xf0]
	ldr r7, [r0, #0xc]
	ldr r5, [sp, #0x198]
	ldr r3, [sp, #0x19c]
	ldr r2, [sp, #0x1a0]
	add r0, sp, #0x28
	str r7, [sp, #0xf4]
	str r6, [sp, #0x28]
	str r5, [sp, #0x2c]
	str r3, [sp, #0x30]
	str r2, [sp, #0x34]
	rsb r1, r1, #0
	bl ov45_0223CD78
	ldr r2, [r0]
	add r1, sp, #0xf8
	str r2, [sp, #0xf8]
	ldr r3, [r0, #4]
	add r2, sp, #0xe8
	str r3, [sp, #0xfc]
	ldr r5, [r0, #8]
	add r3, sp, #0xd8
	str r5, [sp, #0x100]
	ldr r5, [r0, #0xc]
	add r0, sp, #0x108
	str r5, [sp, #0x104]
	bl ov45_0223C70C
	ldr r0, [sp, #0x108]
	ldr r2, [sp, #0x10c]
	ldr r1, [sp, #0x110]
	str r0, [sp, #0x194]
	ldr r0, [sp, #0x114]
	str r2, [sp, #0x198]
	str r1, [sp, #0x19c]
	str r0, [sp, #0x1a0]
_0223C2CC:
	add r0, sp, #0x194
	rsb r1, r4, #0
	bl ov45_0223CD78
_0223C2D8:
	ldr r3, [sp, #0x194]
	ldr r2, [sp, #0x198]
	ldr r0, [sp, #0x1a0]
	ldr r1, [sp, #0x19c]
	str r0, [sp, #0x9c]
	str r3, [sp, #0x90]
	str r2, [sp, #0x94]
	str r1, [sp, #0x98]
	ldr r1, [r8]
	add r0, sp, #0xc8
	str r1, [sp, #0xa0]
	ldr r2, [r8, #4]
	add r1, sp, #0xb4
	str r2, [sp, #0xa4]
	ldr r3, [r8, #8]
	add r2, sp, #0xa0
	str r3, [sp, #0xa8]
	ldr r4, [r8, #0xc]
	add r3, sp, #0x90
	str r4, [sp, #0xac]
	ldrb r4, [r8, #0x10]
	strb r4, [sp, #0xb0]
	ldr r4, [sb]
	str r4, [sp, #0xb4]
	ldr r4, [sb, #4]
	str r4, [sp, #0xb8]
	ldr r4, [sb, #8]
	str r4, [sp, #0xbc]
	ldr r4, [sb, #0xc]
	str r4, [sp, #0xc0]
	ldrb r4, [sb, #0x10]
	strb r4, [sp, #0xc4]
	bl ov45_0223C53C
	add sp, sp, #0x1a4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0223C364:
	mov r3, #0
	add r2, sl, #0xc
	mov r0, sl
	mov r1, r4
	str r3, [sp, #0x180]
	str r3, [sp, #0x184]
	str r3, [sp, #0x188]
	str r3, [sp, #0x18c]
	str r2, [sp, #0x190]
	bl ov45_0223BBFC
	mov r3, #0
	mov r1, r0
	strb r3, [sp, #0x10]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	add r0, sp, #0x180
	strb r3, [sp, #0x11]
	bl ov45_0223CB04
	ldr r0, [sp, #0x180]
	str r5, [sp, #0x184]
	cmp r4, #0
	add r5, r0, r5, lsl #2
	beq _0223C418
	mov r3, #0
_0223C3C8:
	ldr r0, [sb]
	cmp r5, #0
	strb r3, [sp, #0xf]
	ldr r2, [r0]
	sub r4, r4, #1
	strne r2, [r5]
	ldr r0, [sp, #0x188]
	add r5, r5, #4
	add r0, r0, #1
	str r0, [sp, #0x188]
	ldr r0, [sb]
	add r1, r0, #4
	str r1, [sb]
	ldr r0, [sb, #0xc]
	cmp r1, r0
	ldreq r0, [sb, #4]
	streq r0, [sb]
	cmp r4, #0
	bne _0223C3C8
	str r2, [sp, #0x14]
_0223C418:
	ldr r1, [sl, #0xc]
	cmp r1, #0
	moveq r1, #0
	beq _0223C438
	ldr r2, [sl, #4]
	ldr r0, [sl, #8]
	add r0, r2, r0
	bl _u32_div_f
_0223C438:
	ldr r0, [sl]
	mov r2, #0
	add r0, r0, r1, lsl #2
	str r0, [sp, #0x80]
	ldr r4, [sl]
	sub r1, sp, #4
	str r4, [sp, #0x84]
	ldr r0, [sl, #4]
	add r0, r4, r0, lsl #2
	str r0, [sp, #0x88]
	ldr r3, [sl, #0xc]
	add r0, sp, #0x180
	add r3, r4, r3, lsl #2
	str r3, [sp, #0x8c]
	strb r2, [sp, #0xe]
	strb r2, [r1]
	ldr r3, [r1]
	ldr r1, [sp]
	add r2, sp, #0x80
	bl ov45_0223CC78
	ldr r1, [sl]
	ldr r0, [sl, #4]
	mov r2, #0
	add r0, r1, r0, lsl #2
	str r0, [sp, #0x70]
	ldr r4, [sl]
	sub r1, sp, #4
	str r4, [sp, #0x74]
	ldr r0, [sl, #4]
	add r0, r4, r0, lsl #2
	str r0, [sp, #0x78]
	ldr r3, [sl, #0xc]
	add r0, sp, #0x180
	add r3, r4, r3, lsl #2
	str r3, [sp, #0x7c]
	strb r2, [sp, #0xd]
	strb r2, [r1]
	ldr r3, [r1]
	ldr r2, [sp]
	add r1, sp, #0x70
	bl ov45_0223CB6C
	mov r0, #0
	strb r0, [sp, #0xc]
	str r0, [sl, #8]
	ldr r1, [sl]
	ldr r0, [sp, #0x180]
	add r3, sp, #0x18c
	str r0, [sl]
	str r1, [sp, #0x180]
	ldr r2, [sl, #4]
	ldr r1, [sp, #0x184]
	add r0, sp, #0x180
	str r1, [sl, #4]
	str r2, [sp, #0x184]
	ldr r2, [sl, #8]
	ldr r1, [sp, #0x188]
	str r1, [sl, #8]
	str r2, [sp, #0x188]
	ldr r2, [sl, #0xc]
	ldr r1, [r3]
	str r1, [sl, #0xc]
	str r2, [r3]
	bl ov45_0223CAB4
	add sp, sp, #0x1a4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223BF9C

	arm_func_start ov45_0223C53C
ov45_0223C53C: ; 0x0223C53C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x60
	mov r5, r1
	ldr r8, [r5]
	ldr r7, [r2]
	mov r6, r0
	mov r4, r3
	cmp r8, r7
	beq _0223C6E4
	mov r0, r2
	bl ov45_0223CDBC
	str r0, [sp, #0xc]
_0223C56C:
	ldr r2, [r4, #0xc]
	ldr r0, [r4]
	ldr r1, [sp, #0xc]
	sub r2, r2, r0
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	addle r0, sp, #0xc
	str r2, [sp, #8]
	addgt r0, sp, #8
	ldr r7, [r0]
	ldr r1, [r5]
	add r0, sp, #0x10
	str r1, [sp, #0x10]
	ldr r2, [r5, #4]
	mov r1, r7
	str r2, [sp, #0x14]
	ldr r2, [r5, #8]
	str r2, [sp, #0x18]
	ldr r2, [r5, #0xc]
	str r2, [sp, #0x1c]
	ldrb r2, [r5, #0x10]
	strb r2, [sp, #0x20]
	bl ov45_0223CD78
	ldrb r2, [sp, #0x20]
	ldr r0, [sp, #0x10]
	ldr ip, [r4]
	ldr sb, [sp, #0x14]
	ldr r8, [sp, #0x18]
	ldr r3, [sp, #0x1c]
	str r0, [sp, #0x24]
	str sb, [sp, #0x28]
	str r8, [sp, #0x2c]
	str r3, [sp, #0x30]
	strb r2, [sp, #0x34]
	ldr lr, [r5]
	str r3, [sp, #0x58]
	str lr, [sp, #0x38]
	ldr r1, [r5, #4]
	str r0, [sp, #0x4c]
	str r1, [sp, #0x3c]
	ldr r1, [r5, #8]
	str sb, [sp, #0x50]
	str r1, [sp, #0x40]
	ldr r1, [r5, #0xc]
	str r8, [sp, #0x54]
	str r1, [sp, #0x44]
	ldrb r1, [r5, #0x10]
	strb r2, [sp, #0x5c]
	strb r2, [sp]
	strb r1, [sp, #0x48]
	cmp lr, r0
	add r3, sp, #0x24
	add r2, sp, #0x38
	beq _0223C690
	mov r1, #0
_0223C650:
	strb r1, [sp, #1]
	ldr r0, [lr]
	str r0, [ip]
	ldr lr, [r2]
	ldr r8, [r2, #0xc]
	add lr, lr, #4
	cmp lr, r8
	ldreq r8, [r2, #4]
	str lr, [r2]
	streq r8, [r2]
	ldr lr, [r2]
	ldr r8, [r3]
	add ip, ip, #4
	cmp lr, r8
	bne _0223C650
	str r0, [sp, #4]
_0223C690:
	ldr r1, [sp, #0xc]
	sub r0, r7, #1
	sub r1, r1, r7
	str r1, [sp, #0xc]
	ldr r1, [r4]
	add r0, r1, r0, lsl #2
	add r1, r0, #4
	str r1, [r4]
	ldr r0, [r4, #0xc]
	cmp r1, r0
	ldreq r0, [r4, #4]
	streq r0, [r4]
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _0223C6E4
	add r0, sp, #0x4c
	ldmia r0, {r0, r1, r2, r3}
	stmia r5, {r0, r1, r2, r3}
	ldrb r7, [sp]
	strb r7, [r5, #0x10]
	b _0223C56C
_0223C6E4:
	ldr r0, [r4]
	str r0, [r6]
	ldr r0, [r4, #4]
	str r0, [r6, #4]
	ldr r0, [r4, #8]
	str r0, [r6, #8]
	ldr r0, [r4, #0xc]
	str r0, [r6, #0xc]
	add sp, sp, #0x60
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov45_0223C53C

	arm_func_start ov45_0223C70C
ov45_0223C70C: ; 0x0223C70C
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sb, r1
	mov r8, r2
	ldr r2, [r8]
	ldr r1, [sb]
	mov sl, r0
	mov r7, r3
	cmp r1, r2
	beq _0223C878
	add r4, sp, #8
	mov fp, #0
	mov r5, #1
_0223C740:
	ldr r0, [r8, #4]
	cmp r2, r0
	ldreq r0, [r8, #0xc]
	streq r0, [r8]
	ldr r0, [r8]
	sub r0, r0, #4
	str r0, [r8]
	ldr r1, [r7]
	ldr r0, [r7, #4]
	cmp r1, r0
	ldreq r0, [r7, #0xc]
	streq r0, [r7]
	ldr r0, [r7]
	sub r0, r0, #4
	str r0, [r7]
	ldr r1, [r7, #8]
	cmp r0, r1
	ldrlo r1, [r7, #4]
	sub r1, r0, r1
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sp, #8]
	ldr r1, [sb]
	ldr r0, [sb, #8]
	ldr r6, [r8, #8]
	cmp r1, r0
	movhs r2, r5
	ldr r0, [r8]
	movlo r2, fp
	cmp r0, r6
	movhs r3, #1
	movlo r3, #0
	cmp r2, r3
	bne _0223C7F4
	sub r1, r0, r1
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp, #4]
	addge r0, sp, #4
	b _0223C820
_0223C7F4:
	cmp r0, r6
	ldrlo r6, [r8, #4]
	sub r1, r0, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp]
	addge r0, sp, #0
_0223C820:
	ldr r6, [r0]
	ldr r1, [r8]
	add r0, r6, #1
	sub r1, r1, r6, lsl #2
	add r0, r1, r0, lsl #2
	str r1, [r8]
	sub r2, r0, r1
	ldr r3, [r7]
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	add r3, r3, #4
	mov r2, r0, asr #2
	sub r0, r3, r2, lsl #2
	mov r2, r2, lsl #2
	bl memmove
	ldr r0, [r7]
	sub r0, r0, r6, lsl #2
	str r0, [r7]
	ldr r2, [r8]
	ldr r0, [sb]
	cmp r0, r2
	bne _0223C740
_0223C878:
	ldr r0, [r7]
	str r0, [sl]
	ldr r0, [r7, #4]
	str r0, [sl, #4]
	ldr r0, [r7, #8]
	str r0, [sl, #8]
	ldr r0, [r7, #0xc]
	str r0, [sl, #0xc]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223C70C

	arm_func_start ov45_0223C8A0
ov45_0223C8A0: ; 0x0223C8A0
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc
	mov sb, r1
	mov r8, r2
	ldr r2, [sb]
	ldr r1, [r8]
	mov sl, r0
	mov r7, r3
	cmp r2, r1
	beq _0223C9EC
	add r4, sp, #8
	mov fp, #0
	mov r5, #1
_0223C8D4:
	ldr r1, [r7, #0xc]
	ldr r0, [r7]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r0, r0, asr #2
	str r0, [sp, #8]
	ldr r2, [sb]
	ldr r0, [sb, #8]
	ldr r1, [r8]
	cmp r2, r0
	movhs r3, r5
	ldr r0, [r8, #8]
	movlo r3, fp
	cmp r1, r0
	movhs r0, #1
	movlo r0, #0
	cmp r3, r0
	bne _0223C948
	sub r1, r1, r2
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	ldr r1, [sp, #8]
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp, #4]
	addge r0, sp, #4
	b _0223C970
_0223C948:
	ldr r0, [sb, #0xc]
	ldr r1, [sp, #8]
	sub r2, r0, r2
	mov r0, r2, asr #1
	add r0, r2, r0, lsr #30
	mov r2, r0, asr #2
	cmp r1, r0, asr #2
	movlt r0, r4
	str r2, [sp]
	addge r0, sp, #0
_0223C970:
	ldr r6, [r0]
	ldr r1, [sb]
	ldr r0, [r7]
	add r2, r1, r6, lsl #2
	sub r3, r2, r1
	mov r2, r3, asr #1
	add r2, r3, r2, lsr #30
	mov r2, r2, asr #2
	mov r2, r2, lsl #2
	bl memmove
	ldr r1, [sb]
	sub r0, r6, #1
	add r1, r1, r0, lsl #2
	add r2, r1, #4
	str r2, [sb]
	ldr r1, [sb, #0xc]
	cmp r2, r1
	ldreq r1, [sb, #4]
	streq r1, [sb]
	ldr r1, [r7]
	add r0, r1, r0, lsl #2
	add r1, r0, #4
	str r1, [r7]
	ldr r0, [r7, #0xc]
	cmp r1, r0
	ldreq r0, [r7, #4]
	streq r0, [r7]
	ldr r1, [sb]
	ldr r0, [r8]
	cmp r1, r0
	bne _0223C8D4
_0223C9EC:
	ldr r0, [r7]
	str r0, [sl]
	ldr r0, [r7, #4]
	str r0, [sl, #4]
	ldr r0, [r7, #8]
	str r0, [sl, #8]
	ldr r0, [r7, #0xc]
	str r0, [sl, #0xc]
	add sp, sp, #0xc
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223C8A0

	arm_func_start ov45_0223CA14
ov45_0223CA14: ; 0x0223CA14
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r2, #0
	mov r4, r0
	strb r2, [sp]
	str r2, [r4, #4]
	ldr r0, [r4, #0x10]
	cmp r0, #0
	beq _0223CAA4
	ldr r1, [r4, #0x14]
	cmp r1, #0
	beq _0223CA54
	ldr r2, [r4, #0xc]
	ldr r0, [r4, #0x10]
	add r0, r2, r0
	bl _u32_div_f
	mov r2, r1
_0223CA54:
	ldr r8, [r4, #8]
	ldr r0, [r4, #0xc]
	ldr r1, [r4, #0x14]
	add r6, r8, r2, lsl #2
	add r7, r8, r0, lsl #2
	cmp r7, r6
	add sb, r8, r1, lsl #2
	beq _0223CAA4
	mov r5, #0
_0223CA78:
	ldr r1, [r7]
	cmp r1, #0
	beq _0223CA90
	mov r0, r5
	mov r2, r5
	bl ov00_021EC2EC
_0223CA90:
	add r7, r7, #4
	cmp r7, sb
	moveq r7, r8
	cmp r7, r6
	bne _0223CA78
_0223CAA4:
	add r0, r4, #8
	bl ov45_02238508
	mov r0, r4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end ov45_0223CA14
