	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.public _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
	.extern _Z15__PopStackFrameP12ThrowContextP13ExceptionInfo
	.extern _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo
	.extern _Z19FindExceptionRecordPcP13ExceptionInfo
	.extern _Z20__DecodeSignedNumberPcPl
	.extern _Z22__DecodeUnsignedNumberPcPm
	.extern _ZSt9terminatev
	.extern _u32_div_f
	.type _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc, @function

.text

	arm_func_start _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
_Z11UnwindStackP12ThrowContextP13ExceptionInfoPc: ; 0x020F39D4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xa4
	mov sb, r0
	mov r8, r1
	str r2, [sp]
_020F39E8:
	ldr r2, [r8, #8]
	cmp r2, #0
	bne _020F3A30
	mov r0, sb
	mov r1, r8
	bl _Z15__PopStackFrameP12ThrowContextP13ExceptionInfo
	mov r1, r8
	bl _Z19FindExceptionRecordPcP13ExceptionInfo
	ldr r0, [r8, #4]
	cmp r0, #0
	bne _020F3A18
	bl _ZSt9terminatev
_020F3A18:
	mov r0, sb
	mov r1, r8
	bl _Z16__SetupFrameInfoP12ThrowContextP13ExceptionInfo
	ldr r2, [r8, #8]
	cmp r2, #0
	beq _020F39E8
_020F3A30:
	ldrb r7, [r2]
	and r0, r7, #0x1f
	cmp r0, #0x13
	addls pc, pc, r0, lsl #2
	b _020F4268
_020F3A44: ; jump table
	b _020F4268 ; case 0
	b _020F3A94 ; case 1
	b _020F3AB4 ; case 2
	b _020F3B00 ; case 3
	b _020F3B80 ; case 4
	b _020F3BE0 ; case 5
	b _020F3C64 ; case 6
	b _020F3CD4 ; case 7
	b _020F3D44 ; case 8
	b _020F3DEC ; case 9
	b _020F3EA0 ; case 10
	b _020F3EFC ; case 11
	b _020F3F8C ; case 12
	b _020F3FB8 ; case 13
	b _020F4268 ; case 14
	b _020F4004 ; case 15
	b _020F4040 ; case 16
	b _020F40E4 ; case 17
	b _020F4190 ; case 18
	b _020F4254 ; case 19
_020F3A94:
	add r1, sp, #0xa0
	add r0, r2, #1
	bl _Z20__DecodeSignedNumberPcPl
	ldr r1, [r8, #8]
	ldr r0, [sp, #0xa0]
	add r0, r1, r0
	str r0, [r8, #8]
	b _020F426C
_020F3AB4:
	add r1, sp, #0x9c
	add r0, r2, #1
	bl _Z20__DecodeSignedNumberPcPl
	mov r4, r0
	ldrb r1, [r4]
	ldrb r0, [r4, #1]
	ldrb r2, [r4, #2]
	ldrb r3, [r4, #3]
	orr r0, r1, r0, lsl #8
	orr r0, r0, r2, lsl #16
	ldr r5, [sb, #0x18]
	ldr r1, [sp, #0x9c]
	orr r2, r0, r3, lsl #24
	add r0, r5, r1
	mvn r1, #0
	blx r2
	add r0, r4, #4
	str r0, [r8, #8]
	b _020F426C
_020F3B00:
	add r1, sp, #0x94
	add r0, r2, #1
	and r4, r7, #0x40
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x98
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x94]
	ldreqb r0, [r1, r0]
	beq _020F3B5C
	ldr r0, [sp, #0x94]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
	and r0, r0, #0xff
_020F3B5C:
	cmp r0, #0
	beq _020F3B78
	ldr r3, [sb, #0x18]
	ldr r0, [sp, #0x98]
	mvn r1, #0
	add r0, r3, r0
	blx r2
_020F3B78:
	str r4, [r8, #8]
	b _020F426C
_020F3B80:
	add r1, sp, #0x90
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x90]
	ldreq r0, [r1, r0]
	beq _020F3BD0
	ldr r0, [sp, #0x90]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
_020F3BD0:
	mvn r1, #0
	blx r2
	str r4, [r8, #8]
	b _020F426C
_020F3BE0:
	add r1, sp, #0x84
	add r0, r2, #1
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x88
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, sp, #0x8c
	bl _Z22__DecodeUnsignedNumberPcPm
	ldr r3, [sb, #0x18]
	ldr r2, [sp, #0x84]
	ldr r6, [sp, #0x88]
	ldrb r5, [r0]
	ldrb r4, [r0, #1]
	ldr r1, [sp, #0x8c]
	add r2, r3, r2
	ldrb sl, [r0, #2]
	orr r3, r5, r4, lsl #8
	mla r4, r6, r1, r2
	ldrb r2, [r0, #3]
	orr r1, r3, sl, lsl #16
	cmp r6, #0
	orr r5, r1, r2, lsl #24
	add fp, r0, #4
	beq _020F3C5C
	mvn sl, #0
_020F3C40:
	ldr r0, [sp, #0x8c]
	mov r1, sl
	sub r4, r4, r0
	mov r0, r4
	blx r5
	subs r6, r6, #1
	bne _020F3C40
_020F3C5C:
	str fp, [r8, #8]
	b _020F426C
_020F3C64:
	add r1, sp, #0x7c
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x80
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x7c]
	ldreq r3, [r1, r0]
	beq _020F3CBC
	ldr r0, [sp, #0x7c]
	add r0, sb, r0, lsl #2
	ldr r3, [r0, #0x1c]
_020F3CBC:
	ldr r0, [sp, #0x80]
	mov r1, #0
	add r0, r3, r0
	blx r2
	str r4, [r8, #8]
	b _020F426C
_020F3CD4:
	add r1, sp, #0x74
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x78
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x74]
	ldreq r3, [r1, r0]
	beq _020F3D2C
	ldr r0, [sp, #0x74]
	add r0, sb, r0, lsl #2
	ldr r3, [r0, #0x1c]
_020F3D2C:
	ldr r0, [sp, #0x78]
	mvn r1, #0
	add r0, r3, r0
	blx r2
	str r4, [r8, #8]
	b _020F426C
_020F3D44:
	add r1, sp, #0x68
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x6c
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x70
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	orr r2, r1, r5, lsl #24
	tst r7, #0x40
	add r5, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x68]
	ldreqsh r0, [r1, r0]
	beq _020F3DAC
	ldr r0, [sp, #0x68]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
	mov r0, r0, lsl #0x10
	mov r0, r0, asr #0x10
_020F3DAC:
	cmp r0, #0
	beq _020F3DE4
	cmp r4, #0
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x6c]
	ldreq r3, [r1, r0]
	beq _020F3DD4
	ldr r0, [sp, #0x6c]
	add r0, sb, r0, lsl #2
	ldr r3, [r0, #0x1c]
_020F3DD4:
	ldr r0, [sp, #0x70]
	mvn r1, #0
	add r0, r3, r0
	blx r2
_020F3DE4:
	str r5, [r8, #8]
	b _020F426C
_020F3DEC:
	add r1, sp, #0x58
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x5c
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x60
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, sp, #0x64
	bl _Z22__DecodeUnsignedNumberPcPm
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r5, r1, r5, lsl #24
	add fp, r0, #4
	beq _020F3E50
	ldr r1, [sp, #0x58]
	ldr r0, [sp, #0x5c]
	add r1, sb, r1, lsl #2
	ldr r1, [r1, #0x1c]
	b _020F3E60
_020F3E50:
	ldr r2, [sb, #0x18]
	ldr r1, [sp, #0x58]
	ldr r0, [sp, #0x5c]
	ldr r1, [r2, r1]
_020F3E60:
	add r4, r1, r0
	ldr r6, [sp, #0x60]
	ldr r0, [sp, #0x64]
	cmp r6, #0
	mla r4, r6, r0, r4
	beq _020F3E98
	mvn sl, #0
_020F3E7C:
	ldr r0, [sp, #0x64]
	mov r1, sl
	sub r4, r4, r0
	mov r0, r4
	blx r5
	subs r6, r6, #1
	bne _020F3E7C
_020F3E98:
	str fp, [r8, #8]
	b _020F426C
_020F3EA0:
	add r1, sp, #0x54
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x54]
	ldreq r0, [r1, r0]
	beq _020F3EF0
	ldr r0, [sp, #0x54]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
_020F3EF0:
	blx r2
	str r4, [r8, #8]
	b _020F426C
_020F3EFC:
	add r1, sp, #0x4c
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x50
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	tst r7, #0x40
	orr r1, r1, r5, lsl #24
	add r5, r0, #4
	ldreq r2, [sb, #0x18]
	ldreq r0, [sp, #0x4c]
	ldreqb r0, [r2, r0]
	beq _020F3F58
	ldr r0, [sp, #0x4c]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
	and r0, r0, #0xff
_020F3F58:
	cmp r0, #0
	beq _020F3F84
	cmp r4, #0
	ldreq r2, [sb, #0x18]
	ldreq r0, [sp, #0x50]
	ldreq r0, [r2, r0]
	beq _020F3F80
	ldr r0, [sp, #0x50]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
_020F3F80:
	blx r1
_020F3F84:
	str r5, [r8, #8]
	b _020F426C
_020F3F8C:
	ldr r0, [sp]
	cmp r0, r2
	addeq sp, sp, #0xa4
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r1, sp, #0x44
	add r0, r2, #5
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, sp, #0x48
	bl _Z20__DecodeSignedNumberPcPl
	str r0, [r8, #8]
	b _020F426C
_020F3FB8:
	add r1, sp, #0x40
	add r0, r2, #1
	bl _Z20__DecodeSignedNumberPcPl
	ldr r2, [sb, #0x18]
	ldr r1, [sp, #0x40]
	mov r4, r0
	add r0, r2, r1
	ldr r2, [r0, #8]
	cmp r2, #0
	beq _020F3FFC
	ldr r0, [r0]
	ldr r1, [sb, #4]
	cmp r1, r0
	streq r2, [sb, #8]
	beq _020F3FFC
	mvn r1, #0
	blx r2
_020F3FFC:
	str r4, [r8, #8]
	b _020F426C
_020F4004:
	ldr r0, [sp]
	cmp r0, r2
	addeq sp, sp, #0xa4
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	add r1, sp, #0x34
	add r0, r2, #1
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, sp, #0x38
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, sp, #0x3c
	bl _Z20__DecodeSignedNumberPcPl
	ldr r1, [sp, #0x34]
	add r0, r0, r1, lsl #2
	str r0, [r8, #8]
	b _020F426C
_020F4040:
	add r1, sp, #0x24
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x28
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	orr r2, r1, r5, lsl #24
	add r1, sp, #0x2c
	add r0, r0, #4
	str r2, [sp, #0x30]
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r2, r1, r5, lsl #24
	add r4, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x24]
	ldreq r5, [r1, r0]
	beq _020F40C4
	ldr r0, [sp, #0x24]
	add r0, sb, r0, lsl #2
	ldr r5, [r0, #0x1c]
_020F40C4:
	ldr r0, [sp, #0x28]
	ldr r3, [sp, #0x30]
	ldr r1, [sp, #0x2c]
	add r0, r5, r0
	add r1, r3, r1
	blx r2
	str r4, [r8, #8]
	b _020F426C
_020F40E4:
	add r1, sp, #0x14
	add r0, r2, #1
	and r5, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x1c
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0], #1
	add r1, sp, #0x18
	and r4, r2, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r6, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r5, #0
	orr r2, r1, r6, lsl #24
	add r5, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0x14]
	ldreq r0, [r1, r0]
	beq _020F4154
	ldr r0, [sp, #0x14]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
_020F4154:
	cmp r4, #0
	ldreq r3, [sb, #0x18]
	ldreq r1, [sp, #0x18]
	ldreq r4, [r3, r1]
	beq _020F4174
	ldr r1, [sp, #0x18]
	add r1, sb, r1, lsl #2
	ldr r4, [r1, #0x1c]
_020F4174:
	ldr r3, [sp, #0x1c]
	ldr r1, [sp, #0x20]
	add r0, r0, r3
	add r1, r4, r1
	blx r2
	str r5, [r8, #8]
	b _020F426C
_020F4190:
	add r1, sp, #8
	add r0, r2, #1
	and r4, r7, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	ldrb r2, [r0], #1
	add r1, sp, #0xc
	and r6, r2, #0x20
	bl _Z20__DecodeSignedNumberPcPl
	add r1, sp, #0x10
	bl _Z22__DecodeUnsignedNumberPcPm
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb r5, [r0, #3]
	orr r1, r2, r1, lsl #8
	orr r1, r1, r3, lsl #16
	cmp r4, #0
	orr r5, r1, r5, lsl #24
	add fp, r0, #4
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #8]
	ldreq r4, [r1, r0]
	beq _020F41F8
	ldr r0, [sp, #8]
	add r0, sb, r0, lsl #2
	ldr r4, [r0, #0x1c]
_020F41F8:
	cmp r6, #0
	ldreq r1, [sb, #0x18]
	ldreq r0, [sp, #0xc]
	ldreq r0, [r1, r0]
	beq _020F4218
	ldr r0, [sp, #0xc]
	add r0, sb, r0, lsl #2
	ldr r0, [r0, #0x1c]
_020F4218:
	ldr r1, [sp, #0x10]
	add r4, r4, r0
	bl _u32_div_f
	movs r6, r0
	beq _020F424C
	mvn sl, #0
_020F4230:
	ldr r0, [sp, #0x10]
	mov r1, sl
	sub r4, r4, r0
	mov r0, r4
	blx r5
	subs r6, r6, #1
	bne _020F4230
_020F424C:
	str fp, [r8, #8]
	b _020F426C
_020F4254:
	add r1, sp, #4
	add r0, r2, #1
	bl _Z20__DecodeSignedNumberPcPl
	str r0, [r8, #8]
	b _020F426C
_020F4268:
	bl _ZSt9terminatev
_020F426C:
	tst r7, #0x80
	movne r0, #0
	strne r0, [r8, #8]
	b _020F39E8
_020F427C:
	add sp, sp, #0xa4
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end _Z11UnwindStackP12ThrowContextP13ExceptionInfoPc
