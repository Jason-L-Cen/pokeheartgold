	.extern cAdpcmIndexTable2
	.extern cAdpcmIndexTable3
	.extern cAdpcmIndexTable4
	.extern cAdpcmStepSizeTable
	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"

.text
	arm_func_start vct_encode_adpcm_32
vct_encode_adpcm_32: ; 0x020B0544
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	movs r8, r2, lsr #1
	mov r4, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov ip, #0x8000
	rsb ip, ip, #0
	ldr lr, _020B06C8 ; =cAdpcmStepSizeTable
	mov r2, ip, lsr #0x11
_020B0564:
	ldrb r6, [r3, #2]
	ldrsh r7, [r3]
	ldrsh r5, [r1]
	mov sb, r6, lsl #1
	ldrsh sb, [lr, sb]
	subs sl, r5, r7
	movmi r5, #8
	rsbmi sl, sl, #0
	movpl r5, #0
	cmp sl, sb
	orrge r5, r5, #4
	subge sl, sl, sb
	cmp sl, sb, asr #1
	orrge r5, r5, #2
	subge sl, sl, sb, asr #1
	cmp sl, sb, asr #2
	orrge r5, r5, #1
	mov sl, sb, asr #3
	tst r5, #4
	addne sl, sl, sb
	tst r5, #2
	addne sl, sl, sb, asr #1
	tst r5, #1
	addne sl, sl, sb, asr #2
	tst r5, #8
	rsbne sl, sl, #0
	add sb, r7, sl
	ldr r7, _020B06CC ; =cAdpcmIndexTable4
	cmp sb, ip
	movlt sb, ip
	cmp sb, ip, lsr #17
	ldrsb r7, [r7, r5]
	movgt sb, r2
	adds r6, r6, r7
	movmi r6, #0
	bmi _020B05FC
	cmp r6, #0x58
	movgt r6, #0x58
_020B05FC:
	strh sb, [r3]
	strb r6, [r3, #2]
	and r7, r6, #0xff
	ldrsh r6, [r1, #2]
	ldrsh sb, [r3]
	mov sl, r7, lsl #1
	ldrsh sl, [lr, sl]
	subs fp, r6, sb
	movmi r6, #8
	rsbmi fp, fp, #0
	movpl r6, #0
	cmp fp, sl
	orrge r6, r6, #4
	subge fp, fp, sl
	cmp fp, sl, asr #1
	orrge r6, r6, #2
	subge fp, fp, sl, asr #1
	cmp fp, sl, asr #2
	orrge r6, r6, #1
	mov fp, sl, asr #3
	tst r6, #4
	addne fp, fp, sl
	tst r6, #2
	addne fp, fp, sl, asr #1
	tst r6, #1
	addne fp, fp, sl, asr #2
	tst r6, #8
	rsbne fp, fp, #0
	add sl, sb, fp
	ldr sb, _020B06CC ; =cAdpcmIndexTable4
	cmp sl, ip
	movlt sl, ip
	cmp sl, ip, lsr #17
	ldrsb sb, [sb, r6]
	movgt sl, r2
	and r5, r5, #0xff
	adds r7, r7, sb
	add r1, r1, #4
	movmi r7, #0
	bmi _020B06A4
	cmp r7, #0x58
	movgt r7, #0x58
_020B06A4:
	strh sl, [r3]
	strb r7, [r3, #2]
	and r6, r6, #0xff
	orr r5, r5, r6, lsl #4
	strb r5, [r0], #1
	add r4, r4, #1
	cmp r4, r8
	blo _020B0564
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B06C8: .word cAdpcmStepSizeTable
_020B06CC: .word cAdpcmIndexTable4
	arm_func_end vct_encode_adpcm_32
