	.extern cAdpcmIndexTable2
	.extern cAdpcmIndexTable3
	.extern cAdpcmIndexTable4
	.extern cAdpcmStepSizeTable
	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"

.text
	arm_func_start vct_encode_adpcm_24
vct_encode_adpcm_24: ; 0x020B0B64
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #8
	movs r2, r2, lsr #3
	str r2, [sp]
	mov r2, #0
	str r2, [sp, #4]
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r6, #0x8000
	rsb r6, r6, #0
	ldr r7, _020B1068 ; =cAdpcmStepSizeTable
	ldr r4, _020B106C ; =cAdpcmIndexTable3
	mov r5, r6, lsr #0x11
_020B0B98:
	ldrsh r2, [r3]
	ldrsh sb, [r1]
	ldrb r8, [r3, #2]
	subs fp, sb, r2
	mov sb, r8, lsl #1
	movmi sl, #4
	ldrsh sb, [r7, sb]
	rsbmi fp, fp, #0
	movpl sl, #0
	cmp fp, sb
	orrge sl, sl, #2
	subge fp, fp, sb
	cmp fp, sb, asr #1
	orrge sl, sl, #1
	mov fp, sb, asr #2
	tst sl, #2
	addne fp, fp, sb
	tst sl, #1
	addne fp, fp, sb, asr #1
	tst sl, #4
	rsbne fp, fp, #0
	add sb, r2, fp
	cmp sb, r6
	movlt sb, r6
	cmp sb, r6, lsr #17
	ldrsb r2, [r4, sl]
	movgt sb, r5
	adds r8, r8, r2
	movmi r8, #0
	bmi _020B0C18
	cmp r8, #0x58
	movgt r8, #0x58
_020B0C18:
	strh sb, [r3]
	strb r8, [r3, #2]
	and r2, sl, #0xff
	and sb, r8, #0xff
	mov fp, sb, lsl #1
	ldrsh sl, [r3]
	ldrsh r8, [r1, #2]
	ldrsh fp, [r7, fp]
	mov r2, r2, lsl #5
	subs ip, r8, sl
	movmi r8, #4
	rsbmi ip, ip, #0
	movpl r8, #0
	cmp ip, fp
	orrge r8, r8, #2
	subge ip, ip, fp
	cmp ip, fp, asr #1
	orrge r8, r8, #1
	mov ip, fp, asr #2
	tst r8, #2
	addne ip, ip, fp
	tst r8, #1
	addne ip, ip, fp, asr #1
	tst r8, #4
	rsbne ip, ip, #0
	add fp, sl, ip
	cmp fp, r6
	movlt fp, r6
	cmp fp, r6, lsr #17
	ldrsb sl, [r4, r8]
	movgt fp, r5
	and r2, r2, #0xff
	adds sb, sb, sl
	movmi sb, #0
	bmi _020B0CAC
	cmp sb, #0x58
	movgt sb, #0x58
_020B0CAC:
	strh fp, [r3]
	strb sb, [r3, #2]
	and r8, r8, #0xff
	orr r2, r2, r8, lsl #2
	and sb, sb, #0xff
	mov fp, sb, lsl #1
	ldrsh sl, [r3]
	ldrsh r8, [r1, #4]
	ldrsh fp, [r7, fp]
	and r2, r2, #0xff
	subs ip, r8, sl
	movmi r8, #4
	rsbmi ip, ip, #0
	movpl r8, #0
	cmp ip, fp
	orrge r8, r8, #2
	subge ip, ip, fp
	cmp ip, fp, asr #1
	orrge r8, r8, #1
	mov ip, fp, asr #2
	tst r8, #2
	addne ip, ip, fp
	tst r8, #1
	addne ip, ip, fp, asr #1
	tst r8, #4
	rsbne ip, ip, #0
	add fp, sl, ip
	cmp fp, r6
	movlt fp, r6
	cmp fp, r6, lsr #17
	ldrsb sl, [r4, r8]
	movgt fp, r5
	adds sb, sb, sl
	movmi sb, #0
	bmi _020B0D40
	cmp sb, #0x58
	movgt sb, #0x58
_020B0D40:
	strh fp, [r3]
	and lr, r8, #0xff
	strb sb, [r3, #2]
	orr r2, r2, lr, asr #1
	strb r2, [r0]
	ldrb sb, [r3, #2]
	ldrsh r8, [r3]
	ldrsh r2, [r1, #6]
	mov sl, sb, lsl #1
	ldrsh sl, [r7, sl]
	subs fp, r2, r8
	movmi r2, #4
	rsbmi fp, fp, #0
	movpl r2, #0
	cmp fp, sl
	orrge r2, r2, #2
	subge fp, fp, sl
	cmp fp, sl, asr #1
	orrge r2, r2, #1
	mov fp, sl, asr #2
	tst r2, #2
	addne fp, fp, sl
	tst r2, #1
	addne fp, fp, sl, asr #1
	tst r2, #4
	rsbne fp, fp, #0
	add sl, r8, fp
	cmp sl, r6
	movlt sl, r6
	cmp sl, r6, lsr #17
	ldrsb r8, [r4, r2]
	movgt sl, r5
	adds sb, sb, r8
	movmi sb, #0
	bmi _020B0DD4
	cmp sb, #0x58
	movgt sb, #0x58
_020B0DD4:
	strh sl, [r3]
	and r2, r2, #0xff
	mov r2, r2, lsl #4
	strb sb, [r3, #2]
	and r8, r2, #0xff
	and ip, sb, #0xff
	ldrsh r2, [r3]
	ldrsh sb, [r1, #8]
	subs fp, sb, r2
	mov sb, ip, lsl #1
	movmi sl, #4
	ldrsh sb, [r7, sb]
	rsbmi fp, fp, #0
	movpl sl, #0
	cmp fp, sb
	orrge sl, sl, #2
	subge fp, fp, sb
	cmp fp, sb, asr #1
	orrge sl, sl, #1
	mov fp, sb, asr #2
	tst sl, #2
	addne fp, fp, sb
	tst sl, #1
	addne fp, fp, sb, asr #1
	tst sl, #4
	rsbne fp, fp, #0
	add sb, r2, fp
	cmp sb, r6
	movlt sb, r6
	cmp sb, r6, lsr #17
	ldrsb r2, [r4, sl]
	movgt sb, r5
	adds ip, ip, r2
	movmi ip, #0
	bmi _020B0E68
	cmp ip, #0x58
	movgt ip, #0x58
_020B0E68:
	strh sb, [r3]
	strb ip, [r3, #2]
	and r2, sl, #0xff
	orr r2, r8, r2, lsl #1
	and r8, r2, #0xff
	ldrsh r2, [r3]
	ldrsh sb, [r1, #0xa]
	and ip, ip, #0xff
	subs fp, sb, r2
	mov sb, ip, lsl #1
	movmi sl, #4
	ldrsh sb, [r7, sb]
	rsbmi fp, fp, #0
	movpl sl, #0
	cmp fp, sb
	orrge sl, sl, #2
	subge fp, fp, sb
	cmp fp, sb, asr #1
	orrge sl, sl, #1
	mov fp, sb, asr #2
	tst sl, #2
	addne fp, fp, sb
	tst sl, #1
	addne fp, fp, sb, asr #1
	tst sl, #4
	rsbne fp, fp, #0
	add r2, r2, fp
	cmp r2, r6
	movlt r2, r6
	cmp r2, r6, lsr #17
	ldrsb sb, [r4, sl]
	movgt r2, r5
	adds ip, ip, sb
	movmi ip, #0
	bmi _020B0EFC
	cmp ip, #0x58
	movgt ip, #0x58
_020B0EFC:
	strh r2, [r3]
	orr r8, r8, lr, lsl #7
	and r2, sl, #0xff
	strb ip, [r3, #2]
	orr r8, r8, r2, asr #2
	strb r8, [r0, #1]
	ldrb sb, [r3, #2]
	ldrsh sl, [r3]
	ldrsh r8, [r1, #0xc]
	mov fp, sb, lsl #1
	ldrsh fp, [r7, fp]
	subs ip, r8, sl
	movmi r8, #4
	rsbmi ip, ip, #0
	movpl r8, #0
	cmp ip, fp
	orrge r8, r8, #2
	subge ip, ip, fp
	cmp ip, fp, asr #1
	orrge r8, r8, #1
	mov ip, fp, asr #2
	tst r8, #2
	addne ip, ip, fp
	tst r8, #1
	addne ip, ip, fp, asr #1
	tst r8, #4
	rsbne ip, ip, #0
	add fp, sl, ip
	cmp fp, r6
	movlt fp, r6
	cmp fp, r6, lsr #17
	ldrsb sl, [r4, r8]
	movgt fp, r5
	adds sb, sb, sl
	movmi sb, #0
	bmi _020B0F94
	cmp sb, #0x58
	movgt sb, #0x58
_020B0F94:
	strh fp, [r3]
	and r8, r8, #0xff
	mov r8, r8, lsl #3
	strb sb, [r3, #2]
	and ip, r8, #0xff
	and sl, sb, #0xff
	ldrsh r8, [r1, #0xe]
	ldrsh fp, [r3]
	mov lr, sl, lsl #1
	ldrsh lr, [r7, lr]
	subs sb, r8, fp
	movmi r8, #4
	rsbmi sb, sb, #0
	movpl r8, #0
	cmp sb, lr
	orrge r8, r8, #2
	subge sb, sb, lr
	cmp sb, lr, asr #1
	orrge r8, r8, #1
	mov sb, lr, asr #2
	tst r8, #2
	addne sb, sb, lr
	tst r8, #1
	addne sb, sb, lr, asr #1
	tst r8, #4
	rsbne sb, sb, #0
	add fp, fp, sb
	cmp fp, r6
	movlt fp, r6
	cmp fp, r6, lsr #17
	ldrsb sb, [r4, r8]
	movgt fp, r5
	add r1, r1, #0x10
	adds sl, sl, sb
	movmi sl, #0
	bmi _020B102C
	cmp sl, #0x58
	movgt sl, #0x58
_020B102C:
	and r8, r8, #0xff
	strh fp, [r3]
	orr r8, ip, r8
	strb sl, [r3, #2]
	orr r2, r8, r2, lsl #6
	strb r2, [r0, #2]
	ldr r2, [sp, #4]
	add r0, r0, #3
	add r8, r2, #1
	ldr r2, [sp]
	str r8, [sp, #4]
	cmp r8, r2
	blo _020B0B98
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B1068: .word cAdpcmStepSizeTable
_020B106C: .word cAdpcmIndexTable3
	arm_func_end vct_encode_adpcm_24
