	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223EC64
	.public ov45_0223EC80
	.public ov45_0223F000
	.public ov45_0223F1E4
	.public ov45_0223F2D8
	.public ov45_0223F32C
	.public ov45_0223F5C8
	.public ov45_0223F6F4
	.public ov45_0223F768
	.public ov45_0223F83C
	.public ov45_0223F8F4
	.public ov45_0223F910
	.public ov45_0223FC5C
	.public ov45_0223FC6C
	.public ov45_0223FD6C
	.public ov45_0223FD88
	.public ov45_0223FD8C
	.public ov45_0223FE20
	.public ov45_0223FE28
	.public ov45_02240234
	.public ov45_02240494
	.public ov45_02240620
	.public ov45_02240718
	.public ov45_02240874
	.public ov45_02240A58
	.public ov45_02240A74
	.public ov45_02240A80
	.public ov45_02240B10
	.extern ov45_0223259C
	.extern ov45_02232AE8
	.extern ov45_02232D8C
	.extern ov45_02233ABC
	.extern ov45_02236680
	.extern ov45_02236BA8
	.extern ov45_02236C40
	.extern ov45_02236E58
	.extern ov45_02237634
	.extern ov45_02237664
	.extern ov45_022376BC
	.extern ov45_02237A24
	.extern ov45_02237B58
	.extern ov45_02237BC8
	.extern ov45_02237D48
	.extern ov45_0223D644
	.extern ov45_0223DAD4
	.extern ov45_0223EA34
	.extern ov45_02240F8C
	.extern ov45_022412CC
	.extern ov45_022421E4
	.extern ov45_02242254
	.extern ov45_02242394
	.extern ov45_0224246C
	.extern ov45_022424DC
	.extern ov45_02242510
	.extern ov45_0224268C
	.extern ov45_02242858
	.extern ov45_022428C8
	.extern ov45_022428D8
	.extern ov45_02242910
	.extern ov45_022429E0
	.extern ov45_02242A2C
	.extern ov45_02242C3C
	.extern ov45_02242D30
	.extern ov45_02242EF4
	.extern ov45_02243280
	.extern ov45_0224335C
	.extern ov45_022435F8
	.extern ov45_0224367C
	.extern ov45_022436E0
	.extern ov45_02243794
	.extern ov45_022437CC
	.extern ov45_02243808
	.extern ov45_022502D0
	.extern ov45_0225035C
	.extern ov45_02250424
	.extern ov45_02250590
	.extern ov45_02250E6C
	.extern ov45_02250F5C
	.extern ov45_02251024
	.extern ov45_02251208
	.extern ov45_02255238
	.extern ov45_022552C0
	.extern ov45_022552C8
	.extern ov45_022552D8
	.extern ov45_022552DC
	.extern ov45_022552E4
	.extern ov45_022552EC
	.extern ov45_022552F0
	.extern ov45_022552FC
	.extern ov45_02255304
	.extern ov45_02255308
	.extern ov45_02255310
	.extern ov45_02255318
	.extern ov45_0225531C
	.extern ov45_022577DC

.text
	arm_func_start ov45_0223EC64
ov45_0223EC64: ; 0x0223EC64
	stmdb sp!, {r4, lr}
	mov r4, r0
	mov r1, #0
	str r1, [r4, #4]
	bl ov45_0223ECB0
	mov r0, r4
	ldmia sp!, {r4, pc}
	arm_func_end ov45_0223EC64

	arm_func_start ov45_0223EC80
ov45_0223EC80: ; 0x0223EC80
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r1
	mvn r1, #0xc0000000
	mov r5, r0
	cmp r4, r1
	bls _0223EC9C
	bl _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_0223EC9C:
	mov r0, r4, lsl #2
	bl ov45_02242394
	str r0, [r5]
	str r4, [r5, #8]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_0223EC80

	arm_func_start ov45_0223ECB0
ov45_0223ECB0: ; 0x0223ECB0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	beq _0223ECF4
	ldr r2, [r4, #4]
	mov r0, #0
	strb r0, [sp]
	ldr r1, [r4, #4]
	sub r1, r1, r2
	str r1, [r4, #4]
	ldr r1, [r4]
	cmp r1, #0
	beq _0223ECF4
	mov r2, r0
	bl ov00_021EC2EC
_0223ECF4:
	mov r0, r4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	arm_func_end ov45_0223ECB0

	arm_func_start ov45_0223ED00
ov45_0223ED00: ; 0x0223ED00
	ldr r0, _0223ED18 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.balign 4, 0
_0223ED18: .word ov45_022577DC
	arm_func_end ov45_0223ED00

	arm_func_start ov45_0223ED1C
ov45_0223ED1C: ; 0x0223ED1C
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr r0, _0223EDD4 ; =ov45_022577DC
	ldr r3, [r0]
	cmp r3, #0
	beq _0223ED44
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_0223ED44:
	mov r2, #0
	add r1, sp, #0
	add r0, r3, #0x9c
	str r2, [sp]
	bl ov45_02232D8C
	ldr r0, _0223EDD4 ; =ov45_022577DC
	ldr r4, [r0]
	ldr r1, [r4, #4]
	cmp r1, #4
	bne _0223EDB0
	cmp r4, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	beq _0223ED9C
	mov r0, r4
	bl ov45_02242858
	cmp r4, #0
	beq _0223ED9C
	mov r0, #0
	mov r1, r4
	mov r2, r0
	bl ov00_021EC2EC
_0223ED9C:
	ldr r0, _0223EDD4 ; =ov45_022577DC
	mov r1, #0
	str r1, [r0]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0223EDB0:
	mov r1, #3
	str r1, [r4, #0x58]
	mov r1, #5
	str r1, [r4, #4]
	ldr r0, [r0]
	mov r1, #1
	str r1, [r0, #0x60]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0223EDD4: .word ov45_022577DC
	arm_func_end ov45_0223ED1C

	arm_func_start ov45_0223EDD8
ov45_0223EDD8: ; 0x0223EDD8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x78
	ldr r0, _0223EFF0 ; =ov45_022577DC
	mov r4, r1
	ldr r0, [r0]
	mov r5, r2
	mov r7, r3
	cmp r0, #0
	beq _0223EE0C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x78
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223EE0C:
	ldr r6, _0223EFF4 ; =ov45_022552C0
	add r0, sp, #0x18
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0x24
	mov r1, r6
	bl ov45_02237B58
	add r0, sp, #0x18
	add r1, sp, #0x24
	mov r2, #6
	bl ov45_0223D644
	mov r6, r0
	add r0, sp, #0x24
	bl ov45_02237BC8
	add r0, sp, #0x18
	bl ov45_02237BC8
	cmp r6, #0
	bne _0223EF78
	ldr r4, _0223EFF8 ; =ov45_022552C8
	add r0, sp, #0
	mov r1, r5
	bl ov45_02237B58
	add r0, sp, #0xc
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0
	add r1, sp, #0xc
	mov r2, #0xc
	bl ov45_0223D644
	mov r4, r0
	add r0, sp, #0xc
	bl ov45_02237BC8
	add r0, sp, #0
	bl ov45_02237BC8
	cmp r4, #0
	addne sp, sp, #0x78
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x60
	mov r1, r5
	bl ov45_02237B58
	ldr r1, _0223EFFC ; =ov45_022552D8
	add r0, sp, #0x54
	bl ov45_02237B58
	add r0, sp, #0x6c
	add r1, sp, #0x60
	add r2, sp, #0x54
	mov r3, #0
	bl ov45_0223F000
	add r0, sp, #0x54
	bl ov45_02237BC8
	add r0, sp, #0x60
	bl ov45_02237BC8
	ldr r0, [sp, #0x70]
	cmp r0, #0
	bne _0223EEF8
	add r0, sp, #0x6c
	bl ov45_02237664
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223EEF8:
	ldr r1, [sp, #0x70]
	mov r0, #0xc
	sub r1, r1, #1
	mul r2, r1, r0
	ldr r1, [sp, #0x6c]
	ldr r0, [r1, r2]
	add r1, r1, r2
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, r1, #1
	ldrne r0, [r1, #8]
	bl atol
	movs r4, r0
	bne _0223EF40
	add r0, sp, #0x6c
	bl ov45_02237664
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223EF40:
	ldr r0, _0223EFF0 ; =ov45_022577DC
	add r3, sp, #0x48
	ldr r0, [r0]
	add r0, r0, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r1, [sp, #0x90]
	ldr r2, [sp, #0x4c]
	mov r0, r4
	blx r2
	add r0, sp, #0x6c
	bl ov45_02237664
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223EF78:
	cmp r7, #3
	addne sp, sp, #0x78
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x3c
	mov r1, r4
	bl ov45_02237B58
	ldr r0, _0223EFF0 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	bl ov45_022435F8
	mov r4, r0
	add r0, sp, #0x3c
	bl ov45_02237BC8
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x78
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _0223EFF0 ; =ov45_022577DC
	add r3, sp, #0x30
	ldr r0, [r0]
	add r0, r0, #0x64
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r2, [sp, #0x90]
	ldr r3, [sp, #0x30]
	mov r0, r4
	mov r1, r5
	blx r3
	add sp, sp, #0x78
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223EFF0: .word ov45_022577DC
_0223EFF4: .word ov45_022552C0
_0223EFF8: .word ov45_022552C8
_0223EFFC: .word ov45_022552D8
	arm_func_end ov45_0223EDD8

	arm_func_start ov45_0223F000
ov45_0223F000: ; 0x0223F000
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sl, r0
	mov r4, #0
	str r4, [sl]
	str r4, [sl, #4]
	add r0, sp, #0x24
	str r2, [sp]
	str r3, [sp, #4]
	str r4, [sl, #8]
	mov r8, #1
	bl ov45_02237A24
	sub r5, sp, #4
	add r6, sp, #0x24
	add r7, sp, #0x18
	add fp, sp, #0xc
	mvn r4, #0
	b _0223F104
_0223F048:
	cmp sb, #0
	beq _0223F0DC
	mov r0, r7
	mov r1, r6
	mov r2, #0
	mov r3, sb
	bl ov45_02236680
	mov r1, #0
	add r0, sp, #0xb
	strb r1, [r0]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223F0AC
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223F09C
	mov r1, r7
	bl ov45_02237A24
_0223F09C:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223F0D0
_0223F0AC:
	mov r0, #0
	mov r2, #0
	strb r0, [sp, #0xa]
	mov r2, r2
	strb r2, [r5]
	ldr r2, [r5]
	mov r0, sl
	mov r1, r7
	bl ov45_02237634
_0223F0D0:
	mov r0, r7
	bl ov45_02237BC8
	add r8, r8, #1
_0223F0DC:
	add r2, sb, #1
	mov r0, fp
	mov r1, r6
	mov r3, r4
	bl ov45_02236680
	mov r0, r6
	mov r1, fp
	bl ov45_02237D48
	mov r0, fp
	bl ov45_02237BC8
_0223F104:
	ldr r0, [sp, #4]
	cmp r8, r0
	beq _0223F12C
	ldr r1, [sp]
	mov r0, r6
	mov r2, #0
	bl ov45_02242254
	mov sb, r0
	cmp sb, r4
	bne _0223F048
_0223F12C:
	ldr r0, [sp, #0x24]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldrne r0, [sp, #0x28]
	bne _0223F14C
	ldrb r0, [sp, #0x24]
	mov r0, r0, lsl #0x18
	mov r0, r0, lsr #0x19
_0223F14C:
	cmp r0, #0
	beq _0223F1B8
	mov r2, #0
	strb r2, [sp, #9]
	ldr r1, [sl, #4]
	ldr r0, [sl, #8]
	cmp r1, r0
	bhs _0223F198
	mov r0, #0xc
	mul r0, r1, r0
	ldr r1, [sl]
	adds r0, r1, r0
	beq _0223F188
	add r1, sp, #0x24
	bl ov45_02237A24
_0223F188:
	ldr r0, [sl, #4]
	add r0, r0, #1
	str r0, [sl, #4]
	b _0223F1B8
_0223F198:
	strb r2, [sp, #8]
	sub r1, sp, #4
	and r0, r2, #0xff
	strb r0, [r1]
	ldr r2, [r1]
	add r1, sp, #0x24
	mov r0, sl
	bl ov45_02237634
_0223F1B8:
	add r0, sp, #0x24
	bl ov45_02237BC8
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end ov45_0223F000

	arm_func_start ov45_0223F1C8
ov45_0223F1C8: ; 0x0223F1C8
	ldr r0, _0223F1E0 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.balign 4, 0
_0223F1E0: .word ov45_022577DC
	arm_func_end ov45_0223F1C8

	arm_func_start ov45_0223F1E4
ov45_0223F1E4: ; 0x0223F1E4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x30
	ldr r0, _0223F2D4 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r3
	cmp r0, #0
	beq _0223F214
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x30
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223F214:
	ldr r0, [sp, #0x48]
	cmp r0, #3
	addne sp, sp, #0x30
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x24
	mov r1, r2
	bl ov45_02237B58
	ldr r0, _0223F2D4 ; =ov45_022577DC
	add r1, sp, #0x24
	ldr r0, [r0]
	bl ov45_022435F8
	mov r4, r0
	add r0, sp, #0x24
	bl ov45_02237BC8
	add r0, sp, #0x18
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _0223F2D4 ; =ov45_022577DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r5, r0
	add r0, sp, #0x18
	bl ov45_02237BC8
	mov r1, r7
	add r0, sp, #0xc
	bl ov45_02237B58
	ldr r1, _0223F2D4 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl ov45_022428D8
	ldr r0, [sp]
	ldr r3, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r5, [r5, #8]
	mov r1, r4
	mov r2, r6
	blx r5
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223F2D4: .word ov45_022577DC
	arm_func_end ov45_0223F1E4

	arm_func_start ov45_0223F2D8
ov45_0223F2D8: ; 0x0223F2D8
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _0223F328 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	beq _0223F300
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {pc}
_0223F300:
	add r0, sp, #0
	bl ov45_02237B58
	ldr r0, _0223F328 ; =ov45_022577DC
	add r1, sp, #0
	ldr r0, [r0]
	bl ov45_0224335C
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_0223F328: .word ov45_022577DC
	arm_func_end ov45_0223F2D8

	arm_func_start ov45_0223F32C
ov45_0223F32C: ; 0x0223F32C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0xa8
	ldr r0, _0223F5C4 ; =ov45_022577DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	cmp r0, #0
	beq _0223F35C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xa8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223F35C:
	add r0, sp, #0x90
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x90
	ldr r0, [r0]
	bl ov45_02243808
	mov r6, r0
	add r0, sp, #0x90
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F3AC
	ldr r0, _0223F5C4 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov45_02250F5C
	cmp r0, r6
	addgt sp, sp, #0xa8
	ldmgtia sp!, {r3, r4, r5, r6, r7, pc}
_0223F3AC:
	add r0, sp, #0x84
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0x84
	bl ov45_02242A2C
	ldr r1, _0223F5C4 ; =ov45_022577DC
	mov r2, r0
	ldr r1, [r1]
	add r0, sp, #0x9c
	bl ov45_0224367C
	add r0, sp, #0x84
	bl ov45_02237BC8
	add r0, sp, #0x78
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x9c
	ldr r0, [r0]
	add r2, sp, #0x78
	bl ov45_02243280
	mov r6, r0
	add r0, sp, #0x78
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F4AC
	add r0, sp, #0x6c
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x6c
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r6, r0
	add r0, sp, #0x6c
	bl ov45_02237BC8
	add r0, sp, #0x60
	mov r1, r5
	bl ov45_02237B58
	ldr r1, _0223F5C4 ; =ov45_022577DC
	add r0, sp, #0x54
	ldr r1, [r1]
	add r2, sp, #0x60
	bl ov45_022428D8
	add r0, sp, #0x48
	mov r1, r4
	bl ov45_02237B58
	ldr r1, [sp, #0x54]
	add r0, sp, #0x48
	mov r1, r1, lsl #0x1f
	movs r1, r1, lsr #0x1f
	addeq r7, sp, #0x55
	ldrne r7, [sp, #0x5c]
	bl ov45_02242A2C
	mov r1, r0
	ldr r2, [r6, #0x14]
	ldr r3, [r6, #4]
	mov r0, r7
	blx r3
	add r0, sp, #0x48
	bl ov45_02237BC8
	add r0, sp, #0x54
	bl ov45_02237BC8
	add r0, sp, #0x60
	bl ov45_02237BC8
_0223F4AC:
	add r0, sp, #0x3c
	mov r1, r4
	bl ov45_02237B58
	add r0, sp, #0x30
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r2, sp, #0x30
	bl ov45_02242D30
	mov r4, r0
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	mvn r0, #0
	cmp r4, r0
	bne _0223F508
	add r0, sp, #0x9c
	bl ov45_02237BC8
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0223F508:
	add r0, sp, #0x24
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x24
	ldr r0, [r0]
	bl ov45_02243794
	mov r6, r0
	add r0, sp, #0x24
	bl ov45_02237BC8
	cmp r6, #0
	beq _0223F5B4
	add r0, sp, #0x18
	mov r1, r5
	bl ov45_02237B58
	ldr r0, _0223F5C4 ; =ov45_022577DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r6, r0
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0xc
	mov r1, r5
	bl ov45_02237B58
	ldr r1, _0223F5C4 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl ov45_022428D8
	ldr r0, [sp]
	ldr r2, [r6, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r3, [r6]
	mov r1, r4
	blx r3
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
_0223F5B4:
	add r0, sp, #0x9c
	bl ov45_02237BC8
	add sp, sp, #0xa8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223F5C4: .word ov45_022577DC
	arm_func_end ov45_0223F32C

	arm_func_start ov45_0223F5C8
ov45_0223F5C8: ; 0x0223F5C8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #0x48
	ldr r0, _0223F6F0 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r2
	cmp r0, #0
	beq _0223F5F8
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
_0223F5F8:
	add r0, sp, #0x3c
	mov r1, r6
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	bl ov45_022435F8
	mov r4, r0
	add r0, sp, #0x3c
	bl ov45_02237BC8
	mvn r0, #0
	cmp r4, r0
	addeq sp, sp, #0x48
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	add r0, sp, #0x30
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0x30
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r5, r0
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x24
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _0223F6F0 ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428D8
	ldr r0, [sp, #0x18]
	ldr r2, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	ldr r3, [r5, #4]
	mov r1, r4
	blx r3
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	add r0, sp, #0xc
	mov r1, r6
	bl ov45_02237B58
	add r0, sp, #0
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _0223F6F0 ; =ov45_022577DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r2, sp, #0
	bl ov45_02243280
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0223F6F0: .word ov45_022577DC
	arm_func_end ov45_0223F5C8

	arm_func_start ov45_0223F6F4
ov45_0223F6F4: ; 0x0223F6F4
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r0, _0223F764 ; =ov45_022577DC
	mov r4, r3
	ldr r0, [r0]
	cmp r0, #0
	beq _0223F720
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x18
	ldmeqia sp!, {r4, pc}
_0223F720:
	add r0, sp, #0xc
	mov r1, r2
	bl ov45_02237B58
	add r0, sp, #0
	mov r1, r4
	bl ov45_02237B58
	ldr r0, _0223F764 ; =ov45_022577DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r2, sp, #0
	bl ov45_022436E0
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223F764: .word ov45_022577DC
	arm_func_end ov45_0223F6F4

	arm_func_start ov45_0223F768
ov45_0223F768: ; 0x0223F768
	stmdb sp!, {lr}
	sub sp, sp, #0xc
	ldr r0, _0223F824 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	beq _0223F790
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {pc}
_0223F790:
	mov r2, #0
	add r1, sp, #0
	mov r0, r2
_0223F79C:
	str r0, [r1, r2, lsl #2]
	add r2, r2, #1
	cmp r2, #3
	blo _0223F79C
	cmp r3, #3
	addls pc, pc, r3, lsl #2
	b _0223F808
_0223F7B8: ; jump table
	b _0223F7C8 ; case 0
	b _0223F7D8 ; case 1
	b _0223F7E8 ; case 2
	b _0223F7F8 ; case 3
_0223F7C8:
	ldr r1, _0223F828 ; =ov45_022552DC
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7D8:
	ldr r1, _0223F82C ; =ov45_022552E4
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7E8:
	ldr r1, _0223F830 ; =ov45_022552EC
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F7F8:
	ldr r1, _0223F834 ; =ov45_022552F0
	add r0, sp, #0
	bl ov45_0223DAD4
	b _0223F814
_0223F808:
	ldr r1, _0223F838 ; =ov45_022552FC
	add r0, sp, #0
	bl ov45_0223DAD4
_0223F814:
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0xc
	ldmia sp!, {pc}
	.balign 4, 0
_0223F824: .word ov45_022577DC
_0223F828: .word ov45_022552DC
_0223F82C: .word ov45_022552E4
_0223F830: .word ov45_022552EC
_0223F834: .word ov45_022552F0
_0223F838: .word ov45_022552FC
	arm_func_end ov45_0223F768

	arm_func_start ov45_0223F83C
ov45_0223F83C: ; 0x0223F83C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0x24
	ldr r0, _0223F8F0 ; =ov45_022577DC
	mov r6, r1
	ldr r0, [r0]
	mov r4, r2
	cmp r0, #0
	beq _0223F86C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x24
	ldmeqia sp!, {r3, r4, r5, r6, pc}
_0223F86C:
	add r0, sp, #0x18
	mov r1, r6
	bl ov45_02237B58
	ldr r0, _0223F8F0 ; =ov45_022577DC
	add r1, sp, #0x18
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r5, r0
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0xc
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _0223F8F0 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl ov45_022428D8
	ldr r0, [sp]
	ldr r2, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #1
	ldrne r0, [sp, #8]
	ldr r3, [r5, #0x10]
	mov r1, r4
	blx r3
	add r0, sp, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, pc}
	.balign 4, 0
_0223F8F0: .word ov45_022577DC
	arm_func_end ov45_0223F83C

	arm_func_start ov45_0223F8F4
ov45_0223F8F4: ; 0x0223F8F4
	ldr r0, _0223F90C ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.balign 4, 0
_0223F90C: .word ov45_022577DC
	arm_func_end ov45_0223F8F4

	arm_func_start ov45_0223F910
ov45_0223F910: ; 0x0223F910
	ldr r0, _0223F928 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #4]
	cmpne r0, #5
	bx lr
	.balign 4, 0
_0223F928: .word ov45_022577DC
	arm_func_end ov45_0223F910

	arm_func_start ov45_0223F92C
ov45_0223F92C: ; 0x0223F92C
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	ldr r0, _0223F9C0 ; =ov45_022577DC
	mov r4, r1
	ldr r3, [r0]
	cmp r3, #0
	beq _0223F958
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
_0223F958:
	mov r2, #0
	add r1, sp, #0
	add r0, r3, #0x9c
	str r2, [sp]
	bl ov45_02232D8C
	cmp r4, #0
	bne _0223F9A0
	ldr r0, _0223F9C0 ; =ov45_022577DC
	mov r2, #3
	ldr r3, [r0]
	mov r1, #5
	str r2, [r3, #0x58]
	str r1, [r3, #4]
	ldr r0, [r0]
	mov r1, #1
	str r1, [r0, #0x60]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0223F9A0:
	ldr r0, _0223F9C0 ; =ov45_022577DC
	ldr r1, [r0]
	ldr r0, [r1, #4]
	cmp r0, #5
	movne r0, #2
	strne r0, [r1, #4]
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0223F9C0: .word ov45_022577DC
	arm_func_end ov45_0223F92C

	arm_func_start ov45_0223F9C4
ov45_0223F9C4: ; 0x0223F9C4
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldr r0, _0223FA28 ; =ov45_022577DC
	mov r4, r2
	ldr r1, [r0]
	cmp r1, #0
	beq _0223F9F0
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0xc
	ldmeqia sp!, {r3, r4, pc}
_0223F9F0:
	add r0, sp, #0
	bl ov45_0223FA2C
	ldr r0, [sp]
	mov r2, #0x7f
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #1
	ldrne r1, [sp, #8]
	mov r0, r4
	bl strncpy
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0223FA28: .word ov45_022577DC
	arm_func_end ov45_0223F9C4

	arm_func_start ov45_0223FA2C
ov45_0223FA2C: ; 0x0223FA2C
	ldr ip, _0223FA38 ; =ov45_02237A24
	add r1, r1, #0x18
	bx ip
	.balign 4, 0
_0223FA38: .word ov45_02237A24
	arm_func_end ov45_0223FA2C

	arm_func_start ov45_0223FA3C
ov45_0223FA3C: ; 0x0223FA3C
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x60
	ldr r2, _0223FC54 ; =ov45_022577DC
	mov r4, r0
	ldr r0, [r2]
	cmp r0, #0
	beq _0223FA68
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
_0223FA68:
	cmp r1, #1
	bne _0223FADC
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x54
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x54
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x54
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x48
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x48]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x49
	ldrne r1, [sp, #0x50]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0x48
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FADC:
	cmp r1, #2
	bne _0223FB50
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x3c
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x3c
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x3c
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x30
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #0x31
	ldrne r1, [sp, #0x38]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0x30
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FB50:
	sub r0, r1, #3
	cmp r0, #1
	bhi _0223FBD0
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x24
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0x24
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0x24
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp, #0x18]
	ldr r3, _0223FC58 ; =ov45_02255304
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r2, sp, #0x19
	ldrne r2, [sp, #0x20]
	mov r0, r4
	mov r1, #2
	bl ov45_02250424
	add r0, sp, #0x18
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
_0223FBD0:
	cmp r1, #5
	addeq sp, sp, #0x60
	ldmeqia sp!, {r4, pc}
	cmp r1, #6
	addne sp, sp, #0x60
	ldmneia sp!, {r4, pc}
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0xc
	ldr r1, [r1]
	ldr r1, [r1, #8]
	bl ov45_022429E0
	ldr r0, _0223FC54 ; =ov45_022577DC
	add r1, sp, #0xc
	ldr r0, [r0]
	add r0, r0, #0xc
	bl ov45_02237D48
	add r0, sp, #0xc
	bl ov45_02237BC8
	ldr r1, _0223FC54 ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r1, sp, #1
	ldrne r1, [sp, #8]
	mov r0, r4
	bl ov45_0225035C
	add r0, sp, #0
	bl ov45_02237BC8
	add sp, sp, #0x60
	ldmia sp!, {r4, pc}
	.balign 4, 0
_0223FC54: .word ov45_022577DC
_0223FC58: .word ov45_02255304
	arm_func_end ov45_0223FA3C

	arm_func_start ov45_0223FC5C
ov45_0223FC5C: ; 0x0223FC5C
	ldr ip, _0223FC68 ; =ov45_02237A24
	add r1, r1, #0xc
	bx ip
	.balign 4, 0
_0223FC68: .word ov45_02237A24
	arm_func_end ov45_0223FC5C

	arm_func_start ov45_0223FC6C
ov45_0223FC6C: ; 0x0223FC6C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x28
	ldr r0, _0223FD64 ; =ov45_022577DC
	mov r6, r1
	ldr r1, [r0]
	mov r7, r2
	cmp r1, #0
	beq _0223FC9C
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
_0223FC9C:
	add r0, sp, #0x1c
	mov r2, r7
	bl ov45_02242910
	ldr r0, _0223FD64 ; =ov45_022577DC
	ldr r5, [sp, #0x50]
	ldr r4, [r0]
	ldr ip, _0223FD68 ; =ov45_02255238
	add r3, sp, #0x10
	mov r8, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	str ip, [sp, #0x10]
	str r5, [sp, #0x14]
	str r8, [sp, #0x18]
	str r3, [sp, #8]
	bl ov45_0223FD8C
	ldr r1, [sp, #0xc]
	add r0, r4, #0x94
	cmp r1, r0
	moveq sb, r8
	ldrne sb, [r1, #0xc]
	cmp r6, #1
	moveq r6, #1
	add r0, sp, #0x1c
	ldr r4, [sb, #0xc]
	movne r6, #0
	bl ov45_0223FD74
	mov r8, r0
	mov r0, sb
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	mov r1, r7
	ldr r7, [sp, #0x4c]
	ldr r3, [sp, #0x48]
	str r7, [sp]
	str r0, [sp, #4]
	mov r2, r8
	mov r0, r6
	blx r4
	ldr r0, _0223FD64 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add r0, sp, #0x1c
	bl ov45_022376BC
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_0223FD64: .word ov45_022577DC
_0223FD68: .word ov45_02255238
	arm_func_end ov45_0223FC6C

	arm_func_start ov45_0223FD6C
ov45_0223FD6C: ; 0x0223FD6C
	ldr r0, [r0, #8]
	bx lr
	arm_func_end ov45_0223FD6C

	arm_func_start ov45_0223FD74
ov45_0223FD74: ; 0x0223FD74
	ldr r1, [r0, #4]
	cmp r1, #0
	moveq r0, #0
	ldrne r0, [r0]
	bx lr
	arm_func_end ov45_0223FD74

	arm_func_start ov45_0223FD88
ov45_0223FD88: ; 0x0223FD88
	bx lr
	arm_func_end ov45_0223FD88

	arm_func_start ov45_0223FD8C
ov45_0223FD8C: ; 0x0223FD8C
	stmdb sp!, {r4, r5, r6, lr}
	mov r4, r1
	mov r6, r2
	mov r5, r0
	ldr r2, [r4, #4]
	mov r0, r4
	mov r1, r6
	add r3, r4, #4
	bl ov45_0223FDE8
	add r1, r4, #4
	cmp r0, r1
	beq _0223FDD4
	ldr r2, [r6]
	ldr r1, [r0, #0xc]
	ldr r2, [r2, #4]
	ldr r1, [r1, #4]
	cmp r2, r1
	bhs _0223FDE0
_0223FDD4:
	add r0, r4, #4
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, pc}
_0223FDE0:
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end ov45_0223FD8C

	arm_func_start ov45_0223FDE8
ov45_0223FDE8: ; 0x0223FDE8
	cmp r2, #0
	beq _0223FE18
	ldr r0, [r1]
	ldr r1, [r0, #4]
_0223FDF8:
	ldr r0, [r2, #0xc]
	ldr r0, [r0, #4]
	cmp r0, r1
	movhs r3, r2
	ldrhs r2, [r2]
	ldrlo r2, [r2, #4]
	cmp r2, #0
	bne _0223FDF8
_0223FE18:
	mov r0, r3
	bx lr
	arm_func_end ov45_0223FDE8

	arm_func_start ov45_0223FE20
ov45_0223FE20: ; 0x0223FE20
	cmp r1, #0
	bx lr
	arm_func_end ov45_0223FE20

	arm_func_start ov45_0223FE28
ov45_0223FE28: ; 0x0223FE28
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x58
	ldr r0, _0223FFE0 ; =ov45_022577DC
	mov r4, r1
	ldr r0, [r0]
	mov r6, r2
	mov r5, r3
	cmp r0, #0
	beq _0223FE5C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x58
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_0223FE5C:
	mov r0, #0
	str r0, [sp, #0x4c]
	str r0, [sp, #0x50]
	str r0, [sp, #0x54]
	cmp r5, #0
	beq _0223FEE4
	add r0, sp, #0x40
	mov r1, r6
	bl ov45_02237B58
	add r1, sp, #0x40
	ldr r0, _0223FFE0 ; =ov45_022577DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x78]
	add r0, sp, #0x34
	mov r2, r5
	bl ov45_02242EF4
	add r0, sp, #0x4c
	add r1, sp, #0x34
	cmp r0, r1
	beq _0223FED4
	mov r3, #0
	ldr r1, [sp, #0x34]
	ldr r5, [sp, #0x38]
	strb r3, [sp, #4]
	sub r2, sp, #4
	strb r3, [r2]
	ldr r3, [r2]
	add r2, r1, r5, lsl #2
	bl ov45_0223FFE8
_0223FED4:
	add r0, sp, #0x34
	bl ov45_022376BC
	add r0, sp, #0x40
	bl ov45_02237BC8
_0223FEE4:
	ldr r0, _0223FFE0 ; =ov45_022577DC
	ldr r8, [sp, #0x80]
	ldr r5, [r0]
	ldr sb, _0223FFE4 ; =ov45_02255238
	add r3, sp, #0x10
	mov r7, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r5, #0x90
	str sb, [sp, #0x10]
	str r8, [sp, #0x14]
	str r7, [sp, #0x18]
	str r3, [sp, #8]
	bl ov45_0223FD8C
	ldr r1, [sp, #0xc]
	add r0, r5, #0x94
	cmp r1, r0
	moveq r5, r7
	ldrne r5, [r1, #0xc]
	add r0, sp, #0x28
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _0223FFE0 ; =ov45_022577DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl ov45_022428D8
	ldr r0, [sp, #0x1c]
	ldr r6, [sp, #0x50]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r4, #1
	moveq sl, #1
	add r0, sp, #0x4c
	ldr r7, [r5, #0xc]
	movne sl, #0
	bl ov45_0223259C
	mov r4, r0
	mov r0, r5
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	str r0, [sp]
	mov r1, sb
	mov r2, r6
	mov r3, r4
	mov r0, sl
	blx r7
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x28
	bl ov45_02237BC8
	ldr r0, _0223FFE0 ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add r0, sp, #0x4c
	bl ov45_022376BC
	add sp, sp, #0x58
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0223FFE0: .word ov45_022577DC
_0223FFE4: .word ov45_02255238
	arm_func_end ov45_0223FE28

	arm_func_start ov45_0223FFE8
ov45_0223FFE8: ; 0x0223FFE8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r1
	mov r5, r2
	mov r7, r0
	sub r2, r5, r6
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	ldr r3, [r7, #8]
	mov r4, r1, asr #2
	cmp r3, r1, asr #2
	bhs _02240034
	mov r2, #0
	sub r1, r4, r3
	strb r2, [sp]
	bl ov45_0223EA34
	mov r2, r0
	mov r0, r7
	mov r1, r4
	bl ov45_022421E4
_02240034:
	sub r1, r5, r6
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	mov r2, r0, asr #2
	ldr r0, [r7]
	mov r1, r6
	mov r2, r2, lsl #2
	bl memmove
	str r4, [r7, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end ov45_0223FFE8

	arm_func_start ov45_0224005C
ov45_0224005C: ; 0x0224005C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x6c
	ldr r0, _0224022C ; =ov45_022577DC
	mov r5, r1
	ldr r0, [r0]
	mov r4, r2
	mov r6, r3
	cmp r0, #0
	beq _02240090
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240090:
	cmp r5, #1
	bne _022400D4
	add r0, sp, #0x60
	mov r1, r4
	bl ov45_02237B58
	add r1, sp, #0x60
	ldr r0, _0224022C ; =ov45_022577DC
	str r1, [sp]
	ldr r1, [r0]
	ldr r3, [sp, #0x90]
	add r0, sp, #0x54
	mov r2, r6
	bl ov45_02242EF4
	add r0, sp, #0x54
	bl ov45_022376BC
	add r0, sp, #0x60
	bl ov45_02237BC8
_022400D4:
	add r0, sp, #0x48
	mov r1, r4
	bl ov45_02237B58
	ldr r0, _0224022C ; =ov45_022577DC
	add r1, sp, #0x48
	ldr r0, [r0]
	bl ov45_022437CC
	add r0, sp, #0x48
	bl ov45_02237BC8
	ldr r0, _0224022C ; =ov45_022577DC
	ldr r8, [sp, #0x98]
	ldr r6, [r0]
	ldr r0, _02240230 ; =ov45_02255238
	mov r1, #0
	str r0, [sp, #0xc]
	add r0, sp, #0xc
	str r1, [sp, #0x14]
	str r0, [sp, #4]
	add r0, sp, #8
	add r1, r6, #0x90
	add r2, sp, #4
	str r8, [sp, #0x10]
	bl ov45_0223FD8C
	ldr r1, [sp, #8]
	add r0, r6, #0x94
	cmp r1, r0
	moveq r6, #0
	ldrne r6, [r1, #0xc]
	add r0, sp, #0x3c
	mov r1, r4
	bl ov45_02237B58
	ldr r1, _0224022C ; =ov45_022577DC
	add r0, sp, #0x30
	ldr r1, [r1]
	add r2, sp, #0x3c
	bl ov45_022428D8
	ldr r0, [sp, #0x30]
	ldr r7, [r6, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x31
	ldrne sb, [sp, #0x38]
	cmp r5, #1
	moveq sl, #1
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	mov r3, r0
	mov r0, sl
	mov r2, sb
	mov r1, #0
	blx r7
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	ldr r0, _0224022C ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	cmp r5, #0
	addne sp, sp, #0x6c
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	add r0, sp, #0x24
	mov r1, r4
	bl ov45_02237B58
	ldr r1, _0224022C ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428D8
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x19
	ldrne r0, [sp, #0x20]
	bl ov45_022412CC
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_0224022C: .word ov45_022577DC
_02240230: .word ov45_02255238
	arm_func_end ov45_0224005C

	arm_func_start ov45_02240234
ov45_02240234: ; 0x02240234
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x6c
	ldr r4, _02240488 ; =ov45_022577DC
	mov r8, r0
	ldr r0, [r4]
	mov r5, r1
	mov r7, r2
	mov r6, r3
	cmp r0, #0
	beq _0224026C
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0224026C:
	cmp r5, #1
	bne _02240300
	add r0, sp, #0x60
	mov r1, r6
	bl ov45_02237B58
	ldr r0, _02240488 ; =ov45_022577DC
	add r1, sp, #0x60
	ldr r0, [r0]
	bl ov45_02243808
	mov r4, r0
	add r0, sp, #0x60
	bl ov45_02237BC8
	cmp r4, #0
	beq _02240300
	ldr r0, _02240488 ; =ov45_022577DC
	mov r1, r6
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov45_02250F5C
	mvn r1, #0
	cmp r4, r1
	moveq r5, #0
	moveq r7, #7
	beq _02240300
	cmp r0, r4
	bgt _022402DC
	cmp r0, r1
	bne _022402E8
_022402DC:
	mov r5, #0
	mov r7, #2
	b _02240300
_022402E8:
	ldr r0, _02240488 ; =ov45_022577DC
	mov r1, r6
	ldr r0, [r0]
	mov r2, r4
	ldr r0, [r0]
	bl ov45_02250E6C
_02240300:
	cmp r5, #1
	bne _0224036C
	ldr r1, _02240488 ; =ov45_022577DC
	add r0, sp, #0x54
	ldr r1, [r1]
	bl ov45_0223FC5C
	add r0, sp, #0x48
	mov r1, r6
	bl ov45_02237B58
	ldr r0, _02240488 ; =ov45_022577DC
	add r1, sp, #0x54
	ldr r0, [r0]
	add r2, sp, #0x48
	bl ov45_02242D30
	add r0, sp, #0x48
	bl ov45_02237BC8
	add r0, sp, #0x54
	bl ov45_02237BC8
	mov r2, #0
	str r2, [sp]
	ldr r2, _0224048C ; =ov45_0224005C
	ldr r3, [sp, #0x88]
	mov r0, r8
	mov r1, r6
	bl ov45_02251208
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
_0224036C:
	add r0, sp, #0x3c
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _02240488 ; =ov45_022577DC
	add r0, sp, #0x30
	ldr r1, [r1]
	add r2, sp, #0x3c
	bl ov45_022428D8
	ldr r0, [sp, #0x30]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0x31
	ldrne r0, [sp, #0x38]
	bl ov45_022412CC
	add r0, sp, #0x30
	bl ov45_02237BC8
	add r0, sp, #0x3c
	bl ov45_02237BC8
	ldr r0, _02240488 ; =ov45_022577DC
	ldr r5, [sp, #0x88]
	ldr r4, [r0]
	ldr r0, _02240490 ; =ov45_02255238
	mov r1, #0
	str r0, [sp, #0xc]
	add r0, sp, #0xc
	str r1, [sp, #0x14]
	str r0, [sp, #4]
	add r0, sp, #8
	add r1, r4, #0x90
	add r2, sp, #4
	str r5, [sp, #0x10]
	bl ov45_0223FD8C
	ldr r1, [sp, #8]
	add r0, r4, #0x94
	cmp r1, r0
	moveq r4, #0
	ldrne r4, [r1, #0xc]
	add r0, sp, #0x24
	mov r1, r6
	bl ov45_02237B58
	ldr r1, _02240488 ; =ov45_022577DC
	add r0, sp, #0x18
	ldr r1, [r1]
	add r2, sp, #0x24
	bl ov45_022428D8
	ldr r0, [sp, #0x18]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r4
	ldr r1, [r0]
	addeq r6, sp, #0x19
	ldrne r6, [sp, #0x20]
	ldr r1, [r1, #0xc]
	ldr r4, [r4, #0xc]
	blx r1
	mov r3, r0
	mov r1, r7
	mov r2, r6
	mov r0, #0
	blx r4
	add r0, sp, #0x18
	bl ov45_02237BC8
	add r0, sp, #0x24
	bl ov45_02237BC8
	ldr r0, _02240488 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add sp, sp, #0x6c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_02240488: .word ov45_022577DC
_0224048C: .word ov45_0224005C
_02240490: .word ov45_02255238
	arm_func_end ov45_02240234

	arm_func_start ov45_02240494
ov45_02240494: ; 0x02240494
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x40
	ldr r0, _02240618 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov sb, r2
	cmp r0, #0
	beq _022404C4
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022404C4:
	cmp sb, #0
	addeq sp, sp, #0x40
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r3, #0
	mvneq fp, #0
	beq _02240504
	add r0, sp, #0x34
	mov r1, r3
	bl ov45_02237B58
	ldr r0, _02240618 ; =ov45_022577DC
	add r1, sp, #0x34
	ldr r0, [r0]
	bl ov45_022435F8
	mov fp, r0
	add r0, sp, #0x34
	bl ov45_02237BC8
_02240504:
	ldr r0, _02240618 ; =ov45_022577DC
	ldr r8, [sp, #0x74]
	ldr r4, [r0]
	ldr r6, _0224061C ; =ov45_02255238
	add r3, sp, #0x10
	mov r5, #0
	add r0, sp, #0xc
	add r2, sp, #8
	add r1, r4, #0x90
	str r6, [sp, #0x10]
	str r8, [sp, #0x14]
	str r5, [sp, #0x18]
	str r3, [sp, #8]
	bl ov45_0223FD8C
	add r0, r4, #0x94
	ldr r1, [sp, #0xc]
	mov r4, #0
	cmp r1, r0
	moveq r6, r5
	ldrne r6, [r1, #0xc]
	mov r5, r4
	cmp r7, #1
	bne _0224057C
	ldr r0, [sp, #0x68]
	cmp r0, #0
	ble _0224057C
	ldr r1, [sp, #0x6c]
	ldr r0, [sp, #0x70]
	ldr r4, [r1]
	ldr r5, [r0]
_0224057C:
	add r0, sp, #0x28
	mov r1, sb
	bl ov45_02237B58
	ldr r1, _02240618 ; =ov45_022577DC
	add r0, sp, #0x1c
	ldr r1, [r1]
	add r2, sp, #0x28
	bl ov45_022428D8
	ldr r0, [sp, #0x1c]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r6
	ldr r1, [r0]
	addeq sb, sp, #0x1d
	ldrne sb, [sp, #0x24]
	cmp r7, #1
	moveq sl, #1
	ldr r7, [r6, #0xc]
	ldr r1, [r1, #0xc]
	movne sl, #0
	blx r1
	str r5, [sp]
	str r0, [sp, #4]
	mov r0, sl
	mov r1, sb
	mov r2, fp
	mov r3, r4
	blx r7
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x28
	bl ov45_02237BC8
	ldr r0, _02240618 ; =ov45_022577DC
	mov r1, r8
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02240618: .word ov45_022577DC
_0224061C: .word ov45_02255238
	arm_func_end ov45_02240494

	arm_func_start ov45_02240620
ov45_02240620: ; 0x02240620
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x34
	ldr r0, _02240714 ; =ov45_022577DC
	mov r7, r1
	ldr r0, [r0]
	mov r6, r3
	cmp r0, #0
	beq _02240650
	ldr r0, [r0, #4]
	cmp r0, #5
	addeq sp, sp, #0x34
	ldmeqia sp!, {r4, r5, r6, r7, pc}
_02240650:
	cmp r2, #0
	mvneq r4, #0
	beq _02240684
	add r0, sp, #0x28
	mov r1, r2
	bl ov45_02237B58
	ldr r0, _02240714 ; =ov45_022577DC
	add r1, sp, #0x28
	ldr r0, [r0]
	bl ov45_022435F8
	mov r4, r0
	add r0, sp, #0x28
	bl ov45_02237BC8
_02240684:
	add r0, sp, #0x1c
	mov r1, r7
	bl ov45_02237B58
	ldr r0, _02240714 ; =ov45_022577DC
	add r1, sp, #0x1c
	ldr r0, [r0]
	bl ov45_02242C3C
	mov r5, r0
	add r0, sp, #0x1c
	bl ov45_02237BC8
	add r0, sp, #0x10
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02240714 ; =ov45_022577DC
	add r0, sp, #4
	ldr r1, [r1]
	add r2, sp, #0x10
	bl ov45_022428D8
	ldr r0, [sp, #4]
	ldr r1, [r5, #0x14]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #5
	ldrne r0, [sp, #0xc]
	ldr r3, [sp, #0x48]
	str r1, [sp]
	ldr r5, [r5, #0xc]
	mov r1, r4
	mov r2, r6
	blx r5
	add r0, sp, #4
	bl ov45_02237BC8
	add r0, sp, #0x10
	bl ov45_02237BC8
	add sp, sp, #0x34
	ldmia sp!, {r4, r5, r6, r7, pc}
	.balign 4, 0
_02240714: .word ov45_022577DC
	arm_func_end ov45_02240620

	arm_func_start ov45_02240718
ov45_02240718: ; 0x02240718
	stmdb sp!, {r4, r5, r6, r7, r8, sb, lr}
	sub sp, sp, #0x2c
	ldr r4, _02240848 ; =ov45_022577DC
	mov r8, r1
	ldr r1, [r4]
	mov r7, r2
	mov r6, r3
	cmp r1, #0
	beq _0224074C
	ldr r1, [r1, #4]
	cmp r1, #5
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
_0224074C:
	cmp r8, #0
	addeq sp, sp, #0x2c
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, pc}
	ldr r3, [sp, #0x48]
	mov r1, r7
	mov r2, r6
	bl ov45_0223F8F4
	ldr r0, _02240848 ; =ov45_022577DC
	ldr r5, [sp, #0x48]
	ldr r4, [r0]
	ldr lr, _0224084C ; =ov45_02255238
	add r3, sp, #8
	mov ip, #0
	add r0, sp, #4
	add r2, sp, #0
	add r1, r4, #0x90
	str lr, [sp, #8]
	str r5, [sp, #0xc]
	str ip, [sp, #0x10]
	str r3, [sp]
	bl ov45_0223FD8C
	ldr r1, [sp, #4]
	add r0, r4, #0x94
	cmp r1, r0
	moveq r4, #0
	ldrne r4, [r1, #0xc]
	add r0, sp, #0x20
	mov r1, r7
	bl ov45_02237B58
	ldr r1, _02240848 ; =ov45_022577DC
	add r0, sp, #0x14
	ldr r1, [r1]
	add r2, sp, #0x20
	bl ov45_022428D8
	ldr r0, [sp, #0x14]
	ldr r7, [r4, #0xc]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	mov r0, r4
	ldr r1, [r0]
	addeq sb, sp, #0x15
	ldrne sb, [sp, #0x1c]
	cmp r8, #1
	moveq r8, #1
	ldr r1, [r1, #0xc]
	movne r8, #0
	blx r1
	mov r3, r0
	mov r0, r8
	mov r1, sb
	mov r2, r6
	blx r7
	add r0, sp, #0x14
	bl ov45_02237BC8
	add r0, sp, #0x20
	bl ov45_02237BC8
	ldr r0, _02240848 ; =ov45_022577DC
	mov r1, r5
	ldr r0, [r0]
	add r0, r0, #0x90
	bl ov45_02242510
	add sp, sp, #0x2c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, pc}
	.balign 4, 0
_02240848: .word ov45_022577DC
_0224084C: .word ov45_02255238
	arm_func_end ov45_02240718

	arm_func_start ov45_02240850
ov45_02240850: ; 0x02240850
	ldr r0, _02240870 ; =ov45_022577DC
	mov r1, #3
	ldr r2, [r0]
	mov r0, #5
	str r1, [r2, #0x58]
	str r0, [r2, #4]
	mov r0, #0
	bx lr
	.balign 4, 0
_02240870: .word ov45_022577DC
	arm_func_end ov45_02240850

	arm_func_start ov45_02240874
ov45_02240874: ; 0x02240874
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x28
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r1, [r0]
	cmp r1, #0
	addeq sp, sp, #0x28
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	beq _022408AC
	ldr r0, [r1, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022408AC:
	ldr r0, [r1]
	cmp r0, #0
	beq _022408BC
	bl ov45_02250590
_022408BC:
	ldr r2, _02240A54 ; =ov45_022577DC
	add r0, sp, #0xc
	add r1, sp, #0x1c
	ldr r5, [r2]
	bl ov00_021ECB94
	add r0, sp, #0xc
	add r1, sp, #0x1c
	bl RTC_ConvertDateTimeToSecond
	ldr r2, [r5, #0xa8]
	add r7, r5, #0xa0
	mov r6, r0
	mov sb, r1
	str r2, [sp, #8]
	cmp r2, r7
	beq _0224098C
	mov fp, #1
	mov sl, #0
	add r4, sp, #8
_02240904:
	ldr r1, [r2, #0x10]
	ldr r0, [r2, #0x14]
	subs r1, r6, r1
	ldr r8, [r2, #0x18]
	sbc r0, sb, r0
	ldr r3, [r2, #0x1c]
	subs r1, r8, r1
	sbcs r1, r3, r0
	bge _02240948
	str r6, [r2, #0x10]
	str sb, [r2, #0x14]
	ldr r0, [r2, #0x20]
	ldr r1, [r2, #0x24]
	blx r1
	cmp r0, #0
	moveq r0, sl
	beq _0224094C
_02240948:
	mov r0, fp
_0224094C:
	cmp r0, #0
	bne _02240978
	ldr r8, [sp, #8]
	mov r0, r4
	bl ov45_02236E58
	add r0, sp, #4
	mov r2, r8
	add r1, r5, #0x9c
	str r8, [sp]
	bl ov45_02236C40
	b _02240980
_02240978:
	mov r0, r4
	bl ov45_02236E58
_02240980:
	ldr r2, [sp, #8]
	cmp r2, r7
	bne _02240904
_0224098C:
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r3, [r0]
	cmp r3, #0
	beq _022409B0
	ldr r0, [r3, #4]
	cmp r0, #5
	addeq sp, sp, #0x28
	moveq r0, #5
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022409B0:
	ldr r1, [r3, #0x54]
	ldr r0, _02240A54 ; =ov45_022577DC
	cmp r1, #3
	ldr r2, [r0]
	blo _022409F0
	mov r1, #0
	str r1, [r3, #0x54]
	ldr r2, [r0]
	mov r1, #3
	str r1, [r2, #0x58]
	mov r1, #5
	str r1, [r2, #4]
	ldr r0, [r0]
	add sp, sp, #0x28
	ldr r0, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_022409F0:
	ldr r0, [r2, #4]
	cmp r0, #4
	addls pc, pc, r0, lsl #2
	b _02240A40
_02240A00: ; jump table
	b _02240A40 ; case 0
	b _02240A40 ; case 1
	b _02240A14 ; case 2
	b _02240A40 ; case 3
	b _02240A40 ; case 4
_02240A14:
	ldr r1, [r2, #0x5c]
	add r0, r1, #1
	str r0, [r2, #0x5c]
	cmp r1, #0x3c
	bne _02240A40
	mov r0, #0
	str r0, [r2, #0x5c]
	ldr r0, [r2, #4]
	cmp r0, #5
	movne r0, #3
	strne r0, [r2, #4]
_02240A40:
	ldr r0, _02240A54 ; =ov45_022577DC
	ldr r0, [r0]
	ldr r0, [r0, #4]
	add sp, sp, #0x28
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_02240A54: .word ov45_022577DC
	arm_func_end ov45_02240874

	arm_func_start ov45_02240A58
ov45_02240A58: ; 0x02240A58
	ldr r0, _02240A70 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	ldrne r0, [r0, #0x58]
	moveq r0, #0
	bx lr
	.balign 4, 0
_02240A70: .word ov45_022577DC
	arm_func_end ov45_02240A58

	arm_func_start ov45_02240A74
ov45_02240A74: ; 0x02240A74
	add r0, r0, #0x410
	add r0, r0, #0xa000
	bx lr
	arm_func_end ov45_02240A74

	arm_func_start ov45_02240A80
ov45_02240A80: ; 0x02240A80
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x18
	ldr r2, _02240B0C ; =ov45_022577DC
	mov r1, r0
	ldr r0, [r2]
	cmp r0, #0
	addeq sp, sp, #0x18
	moveq r0, #0
	ldmeqia sp!, {r4, pc}
	add r0, sp, #0xc
	bl ov45_02237B58
	ldr r1, _02240B0C ; =ov45_022577DC
	add r0, sp, #0
	ldr r1, [r1]
	add r2, sp, #0xc
	bl ov45_022428C8
	ldr r0, [sp]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, _02240B0C ; =ov45_022577DC
	addeq r1, sp, #1
	ldr r0, [r0]
	ldrne r1, [sp, #8]
	ldr r0, [r0]
	bl ov45_02251024
	cmp r0, #1
	moveq r4, #1
	add r0, sp, #0
	movne r4, #0
	bl ov45_02237BC8
	add r0, sp, #0xc
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x18
	ldmia sp!, {r4, pc}
	.balign 4, 0
_02240B0C: .word ov45_022577DC
	arm_func_end ov45_02240A80

	arm_func_start ov45_02240B10
ov45_02240B10: ; 0x02240B10
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x13c
	mov sl, r0
	ldr r0, _02240F30 ; =_version_NINTENDO_DWC_LOBBY
	mov sb, r1
	mov r8, r2
	mov r7, r3
	bl OSi_ReferSymbol
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	addne sp, sp, #0x13c
	movne r0, #4
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	bl ov45_022429E0
	ldr r1, _02240F38 ; =ov45_02255308
	add r0, sp, #0x124
	bl ov45_02237B58
	ldr r1, _02240F3C ; =ov45_02255310
	add r0, sp, #0x118
	bl ov45_02237B58
	mov r4, #0
	mov r0, #0xac
	mov r5, r4
	bl ov45_02242394
	movs r6, r0
	beq _02240BE0
	add r0, sp, #0xf8
	mov r1, sb
	bl ov45_02237B58
	add r0, sp, #0xec
	mov r1, sl
	mov r4, #1
	bl ov45_02237B58
	ldr r1, [sp, #0x160]
	add r0, sp, #0x130
	str r1, [sp]
	str r0, [sp, #4]
	add r1, sp, #0x124
	str r1, [sp, #8]
	add r0, sp, #0x118
	str r0, [sp, #0xc]
	add r1, sp, #0xec
	add r2, sp, #0xf8
	mov r3, r8
	mov r0, r6
	str r7, [sp, #0x10]
	mov r5, r4
	bl ov45_0224268C
	mov r6, r0
_02240BE0:
	ldr r0, _02240F34 ; =ov45_022577DC
	cmp r5, #0
	str r6, [r0]
	beq _02240BF8
	add r0, sp, #0xec
	bl ov45_02237BC8
_02240BF8:
	cmp r4, #0
	beq _02240C08
	add r0, sp, #0xf8
	bl ov45_02237BC8
_02240C08:
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r0, [r0]
	cmp r0, #0
	bne _02240C3C
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	add sp, sp, #0x13c
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240C3C:
	bl ov45_0224246C
	cmp r0, #0
	moveq r4, #1
	beq _02240EC8
	add r0, sp, #0x104
	mov r1, #0
	mov r2, #0x14
	bl MI_CpuFill8
	ldr r4, _02240F40 ; =ov45_0223ED00
	ldr r3, _02240F44 ; =ov45_0223ED1C
	ldr r1, _02240F48 ; =ov45_0223F1C8
	ldr r2, _02240F4C ; =ov45_0223EDD8
	ldr r0, _02240F34 ; =ov45_022577DC
	str r1, [sp, #0x110]
	ldr r1, [r0]
	str r4, [sp, #0x104]
	str r3, [sp, #0x108]
	str r2, [sp, #0x10c]
	ldr r2, [r7, #8]
	add r0, sp, #0xe0
	str r2, [sp, #0x114]
	bl ov45_02240F6C
	ldr r2, _02240F50 ; =ov45_02255318
	add r0, sp, #0xd4
	add r1, sp, #0xe0
	bl ov45_02233ABC
	ldr r2, _02240F54 ; =ov45_0225531C
	add r0, sp, #0xc8
	add r1, sp, #0xd4
	bl ov45_02233ABC
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xbc
	ldr r1, [r1]
	bl ov45_0223FC5C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xb0
	ldr r1, [r1]
	bl ov45_02240F7C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0xa4
	ldr r1, [r1]
	bl ov45_02240F6C
	ldr r1, _02240F34 ; =ov45_022577DC
	add r0, sp, #0x98
	ldr r1, [r1]
	bl ov45_02240F8C
	ldr r0, [sp, #0x98]
	mov r0, r0, lsl #0x1f
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xa4]
	addeq r1, sp, #0x99
	mov r0, r0, lsl #0x1f
	ldrne r1, [sp, #0xa0]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xb0]
	addeq r4, sp, #0xa5
	mov r0, r0, lsl #0x1f
	ldrne r4, [sp, #0xac]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xbc]
	addeq r3, sp, #0xb1
	mov r0, r0, lsl #0x1f
	ldrne r3, [sp, #0xb8]
	movs r0, r0, lsr #0x1f
	ldr r0, [sp, #0xc8]
	addeq r2, sp, #0xbd
	mov r0, r0, lsl #0x1f
	ldrne r2, [sp, #0xc4]
	movs r0, r0, lsr #0x1f
	addeq r0, sp, #0xc9
	ldrne r0, [sp, #0xd0]
	str r4, [sp]
	str r1, [sp, #4]
	add r4, sp, #0x104
	str r4, [sp, #8]
	ldr r1, _02240F58 ; =ov45_0223FA3C
	ldr r4, _02240F5C ; =ov45_0223F9C4
	str r1, [sp, #0xc]
	ldr r1, _02240F60 ; =ov45_0223F92C
	str r4, [sp, #0x10]
	str r1, [sp, #0x14]
	ldr r4, [r7, #8]
	ldr r1, _02240F64 ; =0x00001A0B
	str r4, [sp, #0x18]
	mov r4, #0
	str r4, [sp, #0x1c]
	bl ov45_022502D0
	mov r4, r0
	add r0, sp, #0x98
	bl ov45_02237BC8
	add r0, sp, #0xa4
	bl ov45_02237BC8
	add r0, sp, #0xb0
	bl ov45_02237BC8
	add r0, sp, #0xbc
	bl ov45_02237BC8
	add r0, sp, #0xc8
	bl ov45_02237BC8
	add r0, sp, #0xd4
	bl ov45_02237BC8
	add r0, sp, #0xe0
	bl ov45_02237BC8
	cmp r4, #0
	moveq r4, #2
	beq _02240EC8
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r2, _02240F34 ; =ov45_022577DC
	ldr r1, [r0]
	str r4, [r1]
	ldr r1, [r0]
	ldr r0, [r1, #4]
	cmp r0, #5
	movne r0, #1
	strne r0, [r1, #4]
	add r0, sp, #0x88
	add r1, sp, #0x7c
	ldr r5, [r2]
	bl ov00_021ECB94
	add r0, sp, #0x88
	add r1, sp, #0x7c
	bl RTC_ConvertDateTimeToSecond
	ldr r3, _02240F68 ; =ov45_02240850
	str r0, [sp, #0x2c]
	str r1, [sp, #0x30]
	mov r4, #0x1e
	mov r1, #0
	add r0, sp, #0x44
	add r2, sp, #0x2c
	str r4, [sp, #0x34]
	str r1, [sp, #0x38]
	str r5, [sp, #0x3c]
	str r3, [sp, #0x40]
	bl ov45_02232AE8
	mov r0, #0
	add r2, sp, #0x48
	strb r0, [sp, #0x21]
	strb r0, [sp, #0x20]
	ldr r0, [sp, #0x44]
	add r7, sp, #0x64
	str r0, [sp, #0x60]
	add r1, r5, #0x9c
	add r6, r2, #8
	ldr r4, [r2, #0x10]
	ldr r0, [r2, #0x14]
	ldmia r2, {r2, r3}
	stmia r7, {r2, r3}
	add r5, r7, #8
	ldmia r6, {r2, r3}
	stmia r5, {r2, r3}
	str r0, [r7, #0x14]
	add r0, sp, #0x24
	add r2, sp, #0x60
	str r4, [r7, #0x10]
	bl ov45_02236BA8
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	add sp, sp, #0x13c
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
_02240EC8:
	bl ov45_022424DC
	ldr r0, _02240F34 ; =ov45_022577DC
	ldr r5, [r0]
	cmp r5, #0
	beq _02240F0C
	beq _02240F00
	mov r0, r5
	bl ov45_02242858
	cmp r5, #0
	beq _02240F00
	mov r0, #0
	mov r1, r5
	mov r2, r0
	bl ov00_021EC2EC
_02240F00:
	ldr r0, _02240F34 ; =ov45_022577DC
	mov r1, #0
	str r1, [r0]
_02240F0C:
	add r0, sp, #0x118
	bl ov45_02237BC8
	add r0, sp, #0x124
	bl ov45_02237BC8
	add r0, sp, #0x130
	bl ov45_02237BC8
	mov r0, r4
	add sp, sp, #0x13c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02240F30: .word _version_NINTENDO_DWC_LOBBY
_02240F34: .word ov45_022577DC
_02240F38: .word ov45_02255308
_02240F3C: .word ov45_02255310
_02240F40: .word ov45_0223ED00
_02240F44: .word ov45_0223ED1C
_02240F48: .word ov45_0223F1C8
_02240F4C: .word ov45_0223EDD8
_02240F50: .word ov45_02255318
_02240F54: .word ov45_0225531C
_02240F58: .word ov45_0223FA3C
_02240F5C: .word ov45_0223F9C4
_02240F60: .word ov45_0223F92C
_02240F64: .word 0x00001A0B
_02240F68: .word ov45_02240850
	arm_func_end ov45_02240B10

	arm_func_start ov45_02240F6C
ov45_02240F6C: ; 0x02240F6C
	ldr ip, _02240F78 ; =ov45_02237A24
	add r1, r1, #0x30
	bx ip
	.balign 4, 0
_02240F78: .word ov45_02237A24
	arm_func_end ov45_02240F6C

	arm_func_start ov45_02240F7C
ov45_02240F7C: ; 0x02240F7C
	ldr ip, _02240F88 ; =ov45_02237A24
	add r1, r1, #0x24
	bx ip
	.balign 4, 0
_02240F88: .word ov45_02237A24
	arm_func_end ov45_02240F7C
