	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.public _d2f
	.public _d_add
	.public _d_div
	.public _d_dtof
	.public _d_dtoi
	.public _d_dtou
	.public _d_dtoull
	.public _d_feq
	.public _d_fge
	.public _d_fgt
	.public _d_fle
	.public _d_flt
	.public _d_fne
	.public _d_itod
	.public _d_mul
	.public _d_sub
	.public _d_utod
	.public _dadd
	.public _ddiv
	.public _deq
	.public _dfix
	.public _dfixu
	.public _dflt
	.public _dfltu
	.public _dgeq
	.public _dgr
	.public _dleq
	.public _dls
	.public _dmul
	.public _dneq
	.public _drdiv
	.public _drsb
	.public _dsqrt
	.public _dsub
	.public _f2d
	.public _f_add
	.public _f_div
	.public _f_feq
	.public _f_fgt
	.public _f_fle
	.public _f_flt
	.public _f_fne
	.public _f_ftod
	.public _f_ftoi
	.public _f_ftou
	.public _f_itof
	.public _f_lltof
	.public _f_mul
	.public _f_qtof
	.public _f_sub
	.public _f_ulltof
	.public _f_utof
	.public _fadd
	.public _fdiv
	.public _feq
	.public _ffix
	.public _ffixu
	.public _fflt
	.public _ffltu
	.public _fgr
	.public _fleq
	.public _fls
	.public _fmul
	.public _fneq
	.public _fp_init
	.public _frdiv
	.public _frsb
	.public _fsub
	.public _ll_div
	.public _ll_mod
	.public _ll_mul
	.public _ll_sdiv
	.public _ll_shl
	.public _ll_sll
	.public _ll_sto_f
	.public _ll_udiv
	.public _ll_ufrom_d
	.public _ll_uto_f
	.public _s32_div_f
	.public _u32_div_f
	.public _u32_div_not_0_f
	.public _ull_div
	.public _ull_mod
	.public _ull_mul
	.public _ull_sll
	.public sys_exit
	.public sys_readc
	.public sys_writec
	.extern errno
	.type _dadd, @function
	.type _d2f, @function
	.type _dfix, @function
	.type _dfixu, @function
	.type _dflt, @function
	.type _dfltu, @function
	.type _dmul, @function
	.type _dsub, @function
	.type _fadd, @function
	.type _dgeq, @function
	.type _dgr, @function
	.type _dleq, @function
	.type _dls, @function
	.type _dneq, @function
	.type _fgr, @function
	.type _fleq, @function
	.type _fls, @function
	.type _feq, @function
	.type _fneq, @function
	.type _fdiv, @function
	.type _f2d, @function
	.type _ffix, @function
	.type _ffixu, @function
	.type _fflt, @function
	.type _ffltu, @function
	.type _f_lltof, @function
	.type _f_ulltof, @function
	.type _fmul, @function
	.type _fsub, @function
	.type _ll_mod, @function
	.type _ll_sdiv, @function
	.type _ll_udiv, @function
	.type _ull_mod, @function
	.type _ll_mul, @function
	.type _s32_div_f, @function
	.type _u32_div_f, @function
	.type _ddiv, @function
	.type _fp_init, @function
	.type _d_add, @function
	.type _d_dtof, @function
	.type _f_qtof, @function
	.type _d_dtoi, @function
	.type _d_dtou, @function
	.type _ll_ufrom_d, @function
	.type _d_dtoull, @function
	.type _d_itod, @function
	.type _d_utod, @function
	.type _d_mul, @function
	.type _dsqrt, @function
	.type _drsb, @function
	.type _d_sub, @function
	.type _f_add, @function
	.type _d_fge, @function
	.type _d_fgt, @function
	.type _d_fle, @function
	.type _d_flt, @function
	.type _deq, @function
	.type _d_feq, @function
	.type _d_fne, @function
	.type _f_fgt, @function
	.type _f_fle, @function
	.type _f_flt, @function
	.type _f_feq, @function
	.type _f_fne, @function
	.type _frdiv, @function
	.type _f_div, @function
	.type _f_ftod, @function
	.type _f_ftoi, @function
	.type _f_ftou, @function
	.type _f_itof, @function
	.type _f_utof, @function
	.type _ll_sto_f, @function
	.type _ll_uto_f, @function
	.type _f_mul, @function
	.type _frsb, @function
	.type _f_sub, @function
	.type _ll_div, @function
	.type _ull_div, @function
	.type _ull_mul, @function
	.type _ll_shl, @function
	.type _ll_sll, @function
	.type _ull_sll, @function
	.type _u32_div_not_0_f, @function
	.type _drdiv, @function
	.type _d_div, @function
	.type sys_writec, @function
	.type sys_readc, @function
	.type sys_exit, @function

.text

	; FP_fastI_v5t_LE.a
_dadd: ; 0x020F068C
_d_add:
	stmdb sp!, {r4, lr}
	eors ip, r1, r3
	eormi r3, r3, #0x80000000
	bmi __dsub_start
__dadd_start:
	subs ip, r0, r2
	sbcs lr, r1, r3
	bhs _020F06B8
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
_020F06B8:
	mov lr, #0x80000000
	mov ip, r1, lsr #0x14
	orr r1, lr, r1, lsl #11
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #0xb
	movs r4, ip, lsl #0x15
	cmnne r4, #0x200000
	beq _020F07B4
	mov r4, r3, lsr #0x14
	orr r3, lr, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs lr, r4, lsl #0x15
	beq _020F07FC
_020F06F0:
	subs r4, ip, r4
	beq _020F0748
	cmp r4, #0x20
	ble _020F072C
	cmp r4, #0x38
	movge r4, #0x3f
	sub r4, r4, #0x20
	rsb lr, r4, #0x20
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #1
	adds r0, r0, r2
	adcs r1, r1, #0
	blo _020F0770
	b _020F0754
_020F072C:
	rsb lr, r4, #0x20
	movs lr, r2, lsl lr
	rsb lr, r4, #0x20
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #1
_020F0748:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo _020F0770
_020F0754:
	add ip, ip, #1
	and r4, r0, #1
	movs r1, r1, rrx
	orr r0, r4, r0, rrx
	mov lr, ip, lsl #0x15
	cmn lr, #0x200000
	beq _020F0980
_020F0770:
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, ip, lsl #20
	tst r2, #0x80000000
	ldmeqia sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqia sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmia sp!, {r4, lr}
	bx lr
_020F07B4:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bics ip, ip, #0x800
	beq _020F0820
	orrs r4, r0, r1, lsl #1
	bne _020F095C
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	beq _020F0948
	cmn r4, #0x200000
	bne _020F0948
	orrs r4, r2, r3, lsl #1
	beq _020F0948
	b _020F095C
_020F07FC:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq _020F088C
	orrs r4, r2, r3, lsl #1
	bne _020F095C
	b _020F0948
_020F0820:
	orrs r4, r0, r1, lsl #1
	beq _020F0860
	mov ip, #1
	bic r1, r1, #0x80000000
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	cmnne r4, #0x200000
	mov r4, r4, lsr #0x15
	orr r4, r4, lr, lsr #20
	beq _020F07FC
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	b _020F06F0
_020F0860:
	mov ip, r3, lsr #0x14
	mov r1, r3, lsl #0xb
	orr r1, r1, r2, lsr #21
	mov r0, r2, lsl #0xb
	movs r4, ip, lsl #0x15
	beq _020F0914
	cmn r4, #0x200000
	bne _020F0914
	orrs r4, r0, r1, lsl #1
	beq _020F0948
	b _020F0960
_020F088C:
	orrs r4, r2, r3, lsl #1
	beq _020F0924
	mov r4, #1
	bic r3, r3, #0x80000000
	cmp r1, #0
	bpl _020F08B0
	orr ip, ip, lr, lsr #20
	orr r4, r4, lr, lsr #20
	b _020F06F0
_020F08B0:
	adds r0, r0, r2
	adcs r1, r1, r3
	blo _020F08D0
	add ip, ip, #1
	and r4, r0, #1
	movs r1, r1, rrx
	mov r0, r0, rrx
	orr r0, r0, r4
_020F08D0:
	cmp r1, #0
	subges ip, ip, #1
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmeqia sp!, {r4, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqia sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqia sp!, {r4, lr}
	bxeq lr
_020F0914:
	mov r1, r3
	mov r0, r2
	ldmia sp!, {r4, lr}
	bx lr
_020F0924:
	cmp r1, #0
	subges ip, ip, #1
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmia sp!, {r4, lr}
	bx lr
_020F0948:
	ldr r1, _020F09A0 ; =0x7FF00000
	orr r1, lr, r1
	mov r0, #0
	ldmia sp!, {r4, lr}
	bx lr
_020F095C:
	mov r1, r3
_020F0960:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, lr}
	bx lr
_020F0970: ; 0x020F0970
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, lr}
	bx lr
_020F0980:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	ldr r1, _020F09A0 ; =0x7FF00000
	orr r1, lr, r1
	mov r0, #0
	ldmia sp!, {r4, lr}
	bx lr
	.align 2, 0
_020F09A0: .word 0x7FF00000
_d2f: ; 0x020F09A4
_d_dtof:
_f_qtof:
	and r2, r1, #0x80000000
	mov ip, r1, lsr #0x14
	bics ip, ip, #0x800
	beq _020F0A1C
	mov r3, ip, lsl #0x15
	cmn r3, #0x200000
	bhs _020F0A00
	subs ip, ip, #0x380
	bls _020F0A2C
	cmp ip, #0xff
	bge _020F0A9C
	mov r1, r1, lsl #0xc
	orr r3, r2, r1, lsr #9
	orr r3, r3, r0, lsr #29
	movs r1, r0, lsl #3
	orr r0, r3, ip, lsl #23
	bxeq lr
	tst r1, #0x80000000
	bxeq lr
	movs r1, r1, lsl #1
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F0A00:
	orrs r3, r0, r1, lsl #12
	bne _020F0A14
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_020F0A14:
	mvn r0, #0x80000000
	bx lr
_020F0A1C:
	orrs r3, r0, r1, lsl #12
	bne __f_underflow
__f_result_zero:
	mov r0, r2
	bx lr
_020F0A2C:
	cmn ip, #0x17
	beq _020F0A80
	bmi __f_underflow
	mov r1, r1, lsl #0xb
	orr r1, r1, #0x80000000
	mov r3, r1, lsr #8
	orr r3, r3, r0, lsr #29
	rsb ip, ip, #1
	movs r1, r0, lsl #3
	orr r0, r2, r3, lsr ip
	rsb ip, ip, #0x20
	mov r3, r3, lsl ip
	orrne r3, r3, #1
	movs r1, r3
	bxeq lr
	tst r1, #0x80000000
	bxeq lr
	movs r1, r1, lsl #1
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F0A80:
	orr r0, r0, r1, lsl #12
__f_very_tiny_result:
	movs r1, r0
	mov r0, r2
	addne r0, r0, #1
	bx lr
__f_underflow:
	mov r0, r2
	bx lr
_020F0A9C:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_dfix: ; 0x020F0AA8
_d_dtoi:
	bic r3, r1, #0x80000000
	ldr r2, _020F0AF0 ; =0x0000041E
	subs r2, r2, r3, lsr #20
	ble _020F0AE4
	cmp r2, #0x20
	bge _020F0ADC
	mov r3, r1, lsl #0xb
	orr r3, r3, #0x80000000
	orr r3, r3, r0, lsr #21
	cmp r1, #0
	mov r0, r3, lsr r2
	rsbmi r0, r0, #0
	bx lr
_020F0ADC:
	mov r0, #0
	bx lr
_020F0AE4:
	mvn r0, r1, asr #31
	add r0, r0, #0x80000000
	bx lr
	.align 2, 0
_020F0AF0: .word 0x0000041E
_dfixu: ; 0x020F0AF4
_d_dtou:
	tst r1, #0x80000000
	bne _020F0B2C
	ldr r2, _020F0B48 ; =0x0000041E
	subs r2, r2, r1, lsr #20
	blt _020F0B40
	cmp r2, #0x20
	bge _020F0B24
	mov r3, r1, lsl #0xb
	orr r3, r3, #0x80000000
	orr r3, r3, r0, lsr #21
	mov r0, r3, lsr r2
	bx lr
_020F0B24:
	mov r0, #0
	bx lr
_020F0B2C:
	cmn r1, #0x100000
	cmpeq r0, #0
	movls r0, #0
	mvnhi r0, #0
	bx lr
_020F0B40:
	mvn r0, #0
	bx lr
	.align 2, 0
_020F0B48: .word 0x0000041E
_ll_ufrom_d: ; 0x020F0B4C
_d_dtoull:
	tst r1, #0x80000000
	bne _020F0BB0
	ldr r2, _020F0BD4 ; =0x0000043E
	subs r2, r2, r1, lsr #20
	blt _020F0BC8
	cmp r2, #0x40
	bge _020F0BA4
	mov ip, r1, lsl #0xb
	orr ip, ip, #0x80000000
	orr ip, ip, r0, lsr #21
	cmp r2, #0x20
	ble _020F0B8C
	sub r2, r2, #0x20
	mov r1, #0
	mov r0, ip, lsr r2
	bx lr
_020F0B8C:
	mov r3, r0, lsl #0xb
	mov r1, ip, lsr r2
	mov r0, r3, lsr r2
	rsb r2, r2, #0x20
	orr r0, r0, ip, lsl r2
	bx lr
_020F0BA4:
	mov r1, #0
	mov r0, #0
	bx lr
_020F0BB0:
	cmn r1, #0x100000
	cmpeq r0, #0
	bhi _020F0BC8
	mov r1, #0
	mov r0, #0
	bx lr
_020F0BC8:
	mvn r1, #0
	mvn r0, #0
	bx lr
	.align 2, 0
_020F0BD4: .word 0x0000043E
_dflt: ; 0x020F0BD8
_d_itod:
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r0, #0
__d_itod_common:
	mov r1, #0
	bxeq lr
	mov r3, #0x400
	add r3, r3, #0x1e
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	movs r1, r0
	mov r0, r1, lsl #0x15
	add r1, r1, r1
	orr r1, r2, r1, lsr #12
	orr r1, r1, r3, lsl #20
	bx lr
_dfltu: ; 0x020F0C18
_d_utod:
	cmp r0, #0
__d_utod_common:
	mov r1, #0
	bxeq lr
	mov r3, #0x400
	add r3, r3, #0x1e
	bmi _020F0C3C
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
_020F0C3C:
	mov r1, r0
	mov r0, r1, lsl #0x15
	add r1, r1, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, r3, lsl #20
	bx lr
_dmul: ; 0x020F0C54
_d_mul:
	stmdb sp!, {r4, r5, r6, r7, lr}
	eor lr, r1, r3
	and lr, lr, #0x80000000
	mov ip, r1, lsr #0x14
	mov r1, r1, lsl #0xb
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #0xb
	movs r6, ip, lsl #0x15
	cmnne r6, #0x200000
	beq _020F0D5C
	orr r1, r1, #0x80000000
	bic ip, ip, #0x800
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	cmnne r5, #0x200000
	beq _020F0DA4
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
_020F0CA8:
	add ip, r4, ip
	umull r5, r4, r0, r2
	umull r7, r6, r0, r3
	adds r4, r7, r4
	adc r6, r6, #0
	umull r7, r0, r1, r2
	adds r4, r7, r4
	adcs r0, r0, r6
	umull r7, r2, r1, r3
	adc r1, r2, #0
	adds r0, r0, r7
	adc r1, r1, #0
	orrs r4, r4, r5
	orrne r0, r0, #1
	cmp r1, #0
	blt _020F0CF4
	sub ip, ip, #1
	adds r0, r0, r0
	adc r1, r1, r1
_020F0CF4:
	add ip, ip, #2
	subs ip, ip, #0x400
	bmi _020F0E90
	beq _020F0E90
	mov r6, ip, lsl #0x14
	cmn r6, #0x100000
	bmi _020F0F90
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0D5C:
	bics ip, ip, #0x800
	beq _020F0DB8
	orrs r6, r0, r1, lsl #1
	bne _020F0F44
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	beq _020F0D98
	cmn r5, #0x200000
	bne _020F0F30
	orrs r5, r2, r3, lsl #1
	beq _020F0F30
	b _020F0F44
_020F0D98:
	orrs r5, r3, r2
	beq _020F0F58
	b _020F0F30
_020F0DA4:
	bics r4, r4, #0x800
	beq _020F0E4C
	orrs r6, r2, r3, lsl #1
	bne _020F0F44
	b _020F0F30
_020F0DB8:
	orrs r6, r0, r1, lsl #1
	beq _020F0E20
	mov ip, #1
	cmp r1, #0
	bne _020F0DDC
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0
	bmi _020F0DF8
_020F0DDC:
	clz r6, r1
	movs r1, r1, lsl r6
	rsb r6, r6, #0x20
	orr r1, r1, r0, lsr r6
	rsb r6, r6, #0x20
	mov r0, r0, lsl r6
	sub ip, ip, r6
_020F0DF8:
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	cmnne r5, #0x200000
	beq _020F0DA4
	orr r3, r3, #0x80000000
	bic r4, r4, #0x800
	b _020F0CA8
_020F0E20:
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r5, r4, lsl #0x15
	beq _020F0FA4
	cmn r5, #0x200000
	bne _020F0FA4
	orrs r6, r2, r3, lsl #1
	beq _020F0F58
	b _020F0F44
_020F0E4C:
	orrs r5, r2, r3, lsl #1
	beq _020F0FA4
	mov r4, #1
	cmp r3, #0
	bne _020F0E70
	sub r4, r4, #0x20
	movs r3, r2
	mov r2, #0
	bmi _020F0CA8
_020F0E70:
	clz r6, r3
	movs r3, r3, lsl r6
	rsb r6, r6, #0x20
	orr r3, r3, r2, lsr r6
	rsb r6, r6, #0x20
	mov r2, r2, lsl r6
	sub r4, r4, r6
	b _020F0CA8
_020F0E90:
	cmn ip, #0x34
	beq _020F0F28
	bmi _020F0F80
	mov r2, r1
	mov r3, r0
	add r4, ip, #0x34
	cmp r4, #0x20
	movge r2, r3
	movge r3, #0
	subge r4, r4, #0x20
	rsb r5, r4, #0x20
	mov r2, r2, lsl r4
	orr r2, r2, r3, lsr r5
	movs r3, r3, lsl r4
	orrne r2, r2, #1
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	cmp r2, #0
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	tst r2, #0x80000000
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqia sp!, {r4, r5, r6, r7, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F28:
	orr r0, r0, r1, lsl #1
	b _020F0F68
_020F0F30:
	ldr r1, _020F0FB4 ; =0x7FF00000
	orr r1, lr, r1
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F44:
	mov r1, r3
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F58:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F68:
	movs r2, r0
	mov r1, lr
	mov r0, #0
	addne r0, r0, #1
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F80:
	mov r1, lr
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0F90:
	ldr r1, _020F0FB4 ; =0x7FF00000
	orr r1, lr, r1
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
_020F0FA4:
	mov r1, lr
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, lr}
	bx lr
	.align 2, 0
_020F0FB4: .word 0x7FF00000
_dsqrt: ; 0x020F0FB8
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020F1148 ; =0x7FF00000
	cmp r1, r2
	bhs _020F1104
	movs ip, r1, lsr #0x14
	beq _020F10B0
	bic r1, r1, r2
	orr r1, r1, #0x100000
_020F0FD8:
	movs ip, ip, asr #1
	bhs _020F0FEC
	sub ip, ip, #1
	movs r0, r0, lsl #1
	adc r1, r1, r1
_020F0FEC:
	movs r3, r0, lsl #1
	adc r1, r1, r1
	mov r2, #0
	mov r4, #0
	mov lr, #0x200000
_020F1000:
	add r6, r4, lr
	cmp r6, r1
	addle r4, r6, lr
	suble r1, r1, r6
	addle r2, r2, lr
	movs r3, r3, lsl #1
	adc r1, r1, r1
	movs lr, lr, lsr #1
	bne _020F1000
	mov r0, #0
	mov r5, #0
	cmp r1, r4
	cmpeq r3, #0x80000000
	blo _020F1048
	subs r3, r3, #0x80000000
	sbc r1, r1, r4
	add r4, r4, #1
	mov r0, #0x80000000
_020F1048:
	movs r3, r3, lsl #1
	adc r1, r1, r1
	mov lr, #0x40000000
_020F1054:
	add r6, r5, lr
	cmp r4, r1
	cmpeq r6, r3
	bhi _020F1074
	add r5, r6, lr
	subs r3, r3, r6
	sbc r1, r1, r4
	add r0, r0, lr
_020F1074:
	movs r3, r3, lsl #1
	adc r1, r1, r1
	movs lr, lr, lsr #1
	bne _020F1054
	orrs r1, r1, r3
	biceq r0, r0, #1
	movs r1, r2, lsr #1
	movs r0, r0, rrx
	adcs r0, r0, #0
	adc r1, r1, #0
	add r1, r1, #0x20000000
	sub r1, r1, #0x100000
	add r1, r1, ip, lsl #20
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F10B0:
	cmp r1, #0
	bne _020F10E0
	cmp r0, #0
	ldmeqia sp!, {r4, r5, r6, lr}
	bxeq lr
	mvn ip, #0x13
	clz r5, r0
	movs r0, r0, lsl r5
	sub ip, ip, r5
	mov r1, r0, lsr #0xb
	mov r0, r0, lsl #0x15
	b _020F0FD8
_020F10E0:
	clz r2, r1
	movs r1, r1, lsl r2
	rsb r2, r2, #0x2b
	mov r1, r1, lsr #0xb
	orr r1, r1, r0, lsr r2
	rsb r2, r2, #0x20
	mov r0, r0, lsl r2
	rsb ip, r2, #1
	b _020F0FD8
_020F1104:
	tst r1, #0x80000000
	beq _020F1120
	bics r3, r1, #0x80000000
	cmpeq r0, #0
	ldmeqia sp!, {r4, r5, r6, lr}
	bxeq lr
	b _020F112C
_020F1120:
	orrs r2, r0, r1, lsl #12
	ldmeqia sp!, {r4, r5, r6, lr}
	bxeq lr
_020F112C:
	ldr r2, _020F114C ; =0x7FF80000
	orr r1, r1, r2
	ldr r3, _020F1150 ; =errno
	mov r4, #0x21
	str r4, [r3]
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
	.align 2, 0
_020F1148: .word 0x7FF00000
_020F114C: .word 0x7FF80000
_020F1150: .word errno
_drsb: ; 0x020F1154
	eor r1, r1, r3
	eor r3, r1, r3
	eor r1, r1, r3
	eor r0, r0, r2
	eor r2, r0, r2
	eor r0, r0, r2
_dsub: ; 0x020F116C
_d_sub: ; 0x020F116C
	stmdb sp!, {r4, lr}
	eors ip, r1, r3
	eormi r3, r3, #0x80000000
	bmi __dadd_start
__dsub_start:
	subs ip, r0, r2
	sbcs lr, r1, r3
	bhs _020F119C
	eor lr, lr, #0x80000000
	adds r2, r2, ip
	adc r3, r3, lr
	subs r0, r0, ip
	sbc r1, r1, lr
_020F119C:
	mov lr, #0x80000000
	mov ip, r1, lsr #0x14
	orr r1, lr, r1, lsl #11
	orr r1, r1, r0, lsr #21
	mov r0, r0, lsl #0xb
	movs r4, ip, lsl #0x15
	cmnne r4, #0x200000
	beq _020F13A0
	mov r4, r3, lsr #0x14
	orr r3, lr, r3, lsl #11
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs lr, r4, lsl #0x15
	beq _020F13E8
_020F11D4:
	subs r4, ip, r4
	beq _020F127C
	cmp r4, #0x20
	ble _020F1210
	cmp r4, #0x38
	movge r4, #0x3f
	sub r4, r4, #0x20
	rsb lr, r4, #0x20
	orrs lr, r2, r3, lsl lr
	mov r2, r3, lsr r4
	orrne r2, r2, #1
	subs r0, r0, r2
	sbcs r1, r1, #0
	bmi _020F1238
	b _020F1328
_020F1210:
	rsb lr, r4, #0x20
	movs lr, r2, lsl lr
	rsb lr, r4, #0x20
	mov r2, r2, lsr r4
	orr r2, r2, r3, lsl lr
	mov r3, r3, lsr r4
	orrne r2, r2, #1
	subs r0, r0, r2
	sbcs r1, r1, r3
	bpl _020F1328
_020F1238:
	movs r2, r0, lsl #0x15
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, ip, lsl #20
	tst r2, #0x80000000
	ldmeqia sp!, {r4, lr}
	bxeq lr
	movs r2, r2, lsl #1
	andeqs r2, r0, #1
	ldmeqia sp!, {r4, lr}
	bxeq lr
	adds r0, r0, #1
	adc r1, r1, #0
	ldmia sp!, {r4, lr}
	bx lr
_020F127C:
	subs r0, r0, r2
	sbc r1, r1, r3
	orrs lr, r1, r0
	beq _020F150C
	mov lr, ip, lsl #0x14
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0
	bmi _020F1304
	bne _020F12B4
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0
	bmi _020F12D0
_020F12B4:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #0x20
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #0x20
	mov r0, r0, lsl r4
	sub ip, ip, r4
_020F12D0:
	cmp ip, #0
	bgt _020F130C
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmia sp!, {r4, lr}
	bx lr
_020F1304:
	cmp r1, #0
	subges ip, ip, #1
_020F130C:
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmia sp!, {r4, lr}
	bx lr
_020F1328:
	mov lr, ip, lsl #0x14
	and lr, lr, #0x80000000
	bic ip, ip, #0x800
	cmp r1, #0
	bne _020F134C
	sub ip, ip, #0x20
	movs r1, r0
	mov r0, #0
	bmi _020F1368
_020F134C:
	clz r4, r1
	movs r1, r1, lsl r4
	rsb r4, r4, #0x20
	orr r1, r1, r0, lsr r4
	rsb r4, r4, #0x20
	mov r0, r0, lsl r4
	sub ip, ip, r4
_020F1368:
	cmp ip, #0
	orrgt ip, ip, lr, lsr #20
	bgt _020F1238
	rsb ip, ip, #0xc
	cmp ip, #0x20
	movge r0, r1
	movge r1, #0
	subge ip, ip, #0x20
	rsb r4, ip, #0x20
	mov r0, r0, lsr ip
	orr r0, r0, r1, lsl r4
	orr r1, lr, r1, lsr ip
	ldmia sp!, {r4, lr}
	bx lr
_020F13A0:
	cmp ip, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bics ip, ip, #0x800
	beq _020F140C
	orrs r4, r0, r1, lsl #1
	bne _020F14E8
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	beq _020F14D4
	cmn r4, #0x200000
	bne _020F14D4
	orrs r4, r2, r3, lsl #1
	beq _020F14FC
	b _020F14E8
_020F13E8:
	cmp r4, #0x800
	movge lr, #0x80000000
	movlt lr, #0
	bic ip, ip, #0x800
	bics r4, r4, #0x800
	beq _020F1484
	orrs r4, r2, r3, lsl #1
	bne _020F14E8
	b _020F14D4
_020F140C:
	orrs r4, r0, r1, lsl #1
	beq _020F144C
	mov ip, #1
	bic r1, r1, #0x80000000
	mov r4, r3, lsr #0x14
	mov r3, r3, lsl #0xb
	orr r3, r3, r2, lsr #21
	mov r2, r2, lsl #0xb
	movs r4, r4, lsl #0x15
	cmnne r4, #0x200000
	mov r4, r4, lsr #0x15
	orr r4, r4, lr, lsr #20
	beq _020F13E8
	orr r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	b _020F11D4
_020F144C:
	mov ip, r3, lsr #0x14
	mov r1, r3, lsl #0xb
	orr r1, r1, r2, lsr #21
	mov r0, r2, lsl #0xb
	movs r4, ip, lsl #0x15
	beq _020F1478
	cmn r4, #0x200000
	bne _020F14A0
	orrs r4, r0, r1, lsl #1
	bne _020F14EC
	b _020F14D4
_020F1478:
	orrs r4, r0, r1, lsl #1
	beq _020F150C
	b _020F14A0
_020F1484:
	orrs r4, r2, r3, lsl #1
	beq _020F14B0
	mov r4, #1
	bic r3, r3, #0x80000000
	orr ip, ip, lr, lsr #20
	orr r4, r4, lr, lsr #20
	b _020F11D4
_020F14A0:
	mov r1, r3
	mov r0, r2
	ldmia sp!, {r4, lr}
	bx lr
_020F14B0:
	cmp r1, #0
	subges ip, ip, #1
	mov r0, r0, lsr #0xb
	orr r0, r0, r1, lsl #21
	add r1, r1, r1
	orr r1, lr, r1, lsr #12
	orr r1, r1, ip, lsl #20
	ldmia sp!, {r4, lr}
	bx lr
_020F14D4:
	ldr r1, _020F151C ; =0x7FF00000
	orr r1, lr, r1
	mov r0, #0
	ldmia sp!, {r4, lr}
	bx lr
_020F14E8:
	mov r1, r3
_020F14EC:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, lr}
	bx lr
_020F14FC:
	mvn r0, #0
	bic r1, r0, #0x80000000
	ldmia sp!, {r4, lr}
	bx lr
_020F150C:
	mov r1, #0
	mov r0, #0
	ldmia sp!, {r4, lr}
	bx lr
	.align 2, 0
_020F151C: .word 0x7FF00000

	arm_func_start _fadd
	arm_func_start _f_add
_fadd: ; 0x020F1520
_f_add: ; 0x020F1520
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi __fsub_start
__fadd_start:
	subs ip, r0, r1
	sublo r0, r0, ip
	addlo r1, r1, ip
	mov r2, #0x80000000
	mov r3, r0, lsr #0x17
	orr r0, r2, r0, lsl #8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq _020F15C0
	mov ip, r1, lsr #0x17
	orr r1, r2, r1, lsl #8
	ands r2, ip, #0xff
	beq _020F1600
_020F1560:
	subs ip, r3, ip
	beq _020F1578
	rsb r2, ip, #0x20
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #1
_020F1578:
	adds r0, r0, r1
	blo _020F1598
	and r1, r0, #1
	orr r0, r1, r0, rrx
	add r3, r3, #1
	and r2, r3, #0xff
	cmp r2, #0xff
	beq _020F1708
_020F1598:
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F15C0:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	ands r3, r3, #0xff
	beq _020F1624
	movs r0, r0, lsl #1
	bne _020F1734
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F1728
	cmp ip, #0xff
	blt _020F1728
	cmp r1, #0
	beq _020F1728
	b _020F1734
_020F1600:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq _020F1680
_020F1618:
	movs r1, r1, lsl #1
	bne _020F1734
	b _020F1728
_020F1624:
	movs r0, r0, lsl #1
	beq _020F165C
	mov r3, #1
	mov r0, r0, lsr #1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq _020F1680
	cmp ip, #0xff
	beq _020F1618
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	b _020F1560
_020F165C:
	mov r3, r1, lsr #0x17
	mov r0, r1, lsl #9
	ands r3, r3, #0xff
	beq _020F16E8
	cmp r3, #0xff
	blt _020F16E8
	cmp r0, #0
	beq _020F1728
	b _020F1720
_020F1680:
	movs r1, r1, lsl #1
	beq _020F16F0
	mov r1, r1, lsr #1
	mov ip, #1
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	cmp r0, #0
	bmi _020F1560
	adds r0, r0, r1
	blo _020F16B4
	and r1, r0, #1
	orr r0, r1, r0, rrx
	add ip, ip, #1
_020F16B4:
	cmp r0, #0
	subge ip, ip, #1
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, ip, lsl #23
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F16E8:
	mov r0, r1
	bx lr
_020F16F0:
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
_020F1708:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_020F1720:
	mvn r0, #0x80000000
	bx lr
_020F1728:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_020F1734:
	mvn r0, #0x80000000
	bx lr
_020F173C: ; 0x020F173C
	mvn r0, #0x80000000
	bx lr
_dgeq: ; 0x020F1744
_d_fge:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F17B8
	cmn ip, r3, lsl #1
	bhs _020F17CC
_020F1758:
	orrs ip, r3, r1
	bmi _020F1788
	cmp r1, r3
	cmpeq r0, r2
	movhs r0, #1
	movlo r0, #0
	bx lr
_020F1774:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F1788:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #1
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	movhs r0, #1
	movlo r0, #0
	bx lr
_020F17B8:
	bne _020F1774
	cmp r0, #0
	bhi _020F1774
	cmn ip, r3, lsl #1
	blo _020F1758
_020F17CC:
	bne _020F1774
	cmp r2, #0
	bhi _020F1774
	b _020F1758
_dgr: ; 0x020F17DC
_d_fgt:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F1850
	cmn ip, r3, lsl #1
	bhs _020F1864
_020F17F0:
	orrs ip, r3, r1
	bmi _020F1820
	cmp r1, r3
	cmpeq r0, r2
	movhi r0, #1
	movls r0, #0
	bx lr
_020F180C:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F1820:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	msr cpsr_f, ip
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	movhi r0, #1
	movls r0, #0
	bx lr
_020F1850:
	bne _020F180C
	cmp r0, #0
	bhi _020F180C
	cmn ip, r3, lsl #1
	blo _020F17F0
_020F1864:
	bne _020F180C
	cmp r2, #0
	bhi _020F180C
	b _020F17F0
_dleq: ; 0x020F1874
_d_fle:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F18F4
	cmn ip, r3, lsl #1
	bhs _020F1908
_020F1888:
	orrs ip, r3, r1
	bmi _020F18BC
	cmp r1, r3
	cmpeq r0, r2
	movls r0, #1
	movhi r0, #0
	bx lr
_020F18A4:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F18BC:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #1
	bne _020F18E0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	orr ip, ip, #0x40000000
	msr cpsr_f, ip
	bxeq lr
_020F18E0:
	cmp r3, r1
	cmpeq r2, r0
	movls r0, #1
	movhi r0, #0
	bx lr
_020F18F4:
	bne _020F18A4
	cmp r0, #0
	bhi _020F18A4
	cmn ip, r3, lsl #1
	blo _020F1888
_020F1908:
	bne _020F18A4
	cmp r2, #0
	bhi _020F18A4
	b _020F1888
_dls: ; 0x020F1918
_d_flt:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F1990
	cmn ip, r3, lsl #1
	bhs _020F19A4
_020F192C:
	orrs ip, r3, r1
	bmi _020F195C
	cmp r1, r3
	cmpeq r0, r2
	movlo r0, #1
	movhs r0, #0
	bx lr
_020F1948:
	mov r0, #0
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F195C:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #0
	bne _020F197C
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bxeq lr
_020F197C:
	cmp r3, r1
	cmpeq r2, r0
	movlo r0, #1
	movhs r0, #0
	bx lr
_020F1990:
	bne _020F1948
	cmp r0, #0
	bhi _020F1948
	cmn ip, r3, lsl #1
	blo _020F192C
_020F19A4:
	bne _020F1948
	cmp r2, #0
	bhi _020F1948
	b _020F192C
_deq: ; 0x020F19B4
_d_feq:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F1A1C
	cmn ip, r3, lsl #1
	bhs _020F1A30
_020F19C8:
	orrs ip, r3, r1
	bmi _020F19F8
	cmp r1, r3
	cmpeq r0, r2
	moveq r0, #1
	movne r0, #0
	bx lr
_020F19E4:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F19F8:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #1
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	moveq r0, #1
	movne r0, #0
	bx lr
_020F1A1C:
	bne _020F19E4
	cmp r0, #0
	bhi _020F19E4
	cmn ip, r3, lsl #1
	blo _020F19C8
_020F1A30:
	bne _020F19E4
	cmp r2, #0
	bhi _020F19E4
	b _020F19C8
_dneq: ; 0x020F1A40
_d_fne:
	mov ip, #0x200000
	cmn ip, r1, lsl #1
	bhs _020F1AA8
	cmn ip, r3, lsl #1
	bhs _020F1ABC
_020F1A54:
	orrs ip, r3, r1
	bmi _020F1A84
	cmp r1, r3
	cmpeq r0, r2
	movne r0, #1
	moveq r0, #0
	bx lr
_020F1A70:
	mov r0, #1
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1A84:
	orr ip, r0, ip, lsl #1
	orrs ip, ip, r2
	moveq r0, #0
	bxeq lr
	cmp r3, r1
	cmpeq r2, r0
	movne r0, #1
	moveq r0, #0
	bx lr
_020F1AA8:
	bne _020F1A70
	cmp r0, #0
	bhi _020F1A70
	cmn ip, r3, lsl #1
	blo _020F1A54
_020F1ABC:
	bne _020F1A70
	cmp r2, #0
	bhi _020F1A70
	b _020F1A54
_fgr: ; 0x020F1ACC
_f_fgt:
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	cmphs r3, r1, lsl #1
	blo _020F1B14
	cmp r0, #0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r1, #0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0
	cmp r0, r1
	movgt r0, #1
	movle r0, #0
	mrs ip, cpsr
	bicle ip, ip, #0x20000000
	orrgt ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F1B14:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_fleq: ; 0x020F1B28
_f_fle:
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	cmphs r3, r1, lsl #1
	blo _020F1B78
	cmp r0, #0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r1, #0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0
	cmp r0, r1
	movle r0, #1
	movgt r0, #0
	mrs ip, cpsr
	orrgt ip, ip, #0x20000000
	bicgt ip, ip, #0x40000000
	bicle ip, ip, #0x20000000
	orrle ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1B78:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_fls: ; 0x020F1B90
_f_flt:
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	cmphs r3, r1, lsl #1
	blo _020F1BD8
	cmp r0, #0
	bicmi r0, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r1, #0
	bicmi r1, r1, #0x80000000
	rsbmi r1, r1, #0
	cmp r0, r1
	movlt r0, #1
	movge r0, #0
	mrs ip, cpsr
	orrge ip, ip, #0x20000000
	biclt ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_020F1BD8:
	mov r0, #0
	mrs ip, cpsr
	orr ip, ip, #0x20000000
	msr cpsr_f, ip
	bx lr
_feq: ; 0x020F1BEC
_f_feq:
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	blo _020F1C40
	cmp r3, r1, lsl #1
	blo _020F1C40
	orr r3, r0, r1
	movs r3, r3, lsl #1
	moveq r0, #1
	bne _020F1C20
	mrs ip, cpsr
	orr ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1C20:
	cmp r0, r1
	moveq r0, #1
	movne r0, #0
	mrs ip, cpsr
	orreq ip, ip, #0x40000000
	bicne ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1C40:
	mov r0, #0
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_fneq: ; 0x020F1C54
_f_fne:
	mov r3, #0xff000000
	cmp r3, r0, lsl #1
	blo _020F1CA8
	cmp r3, r1, lsl #1
	blo _020F1CA8
	orr r3, r0, r1
	movs r3, r3, lsl #1
	moveq r0, #0
	bne _020F1C88
	mrs ip, cpsr
	orr ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1C88:
	cmp r0, r1
	movne r0, #1
	moveq r0, #0
	mrs ip, cpsr
	bicne ip, ip, #0x40000000
	orreq ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_020F1CA8:
	mov r0, #1
	mrs ip, cpsr
	bic ip, ip, #0x40000000
	msr cpsr_f, ip
	bx lr
_frdiv: ; 0x020F1CBC
	eor r0, r0, r1
	eor r1, r0, r1
	eor r0, r0, r1
_fdiv: ; 0x020F1CC8
_f_div:
	stmdb sp!, {lr}
	mov ip, #0xff
	ands r3, ip, r0, lsr #23
	cmpne r3, #0xff
	beq _020F1E9C
	ands ip, ip, r1, lsr #23
	cmpne ip, #0xff
	beq _020F1ED8
	orr r1, r1, #0x800000
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	bic lr, r1, #0xff000000
_020F1CF8:
	cmp r2, lr
	movlo r2, r2, lsl #1
	sublo r3, r3, #1
	teq r0, r1
	adr r0, UNK_020F1D7C - 0x100
	ldrb r1, [r0, lr, lsr #15]
	rsb lr, lr, #0
	mov r0, lr, asr #1
	mul r0, r1, r0
	add r0, r0, #0x80000000
	mov r0, r0, lsr #6
	mul r0, r1, r0
	mov r0, r0, lsr #0xe
	mul r1, lr, r0
	sub ip, r3, ip
	mov r1, r1, lsr #0xc
	mul r1, r0, r1
	mov r0, r0, lsl #0xe
	add r0, r0, r1, lsr #15
	umull r1, r0, r2, r0
	mov r3, r0
	orrmi r0, r0, #0x80000000
	adds ip, ip, #0x7e
	bmi _020F1FA0
	cmp ip, #0xfe
	bge _020F2054
	add r0, r0, ip, lsl #23
	mov ip, r1, lsr #0x1c
	cmp ip, #7
	beq _020F1E7C
	add r0, r0, r1, lsr #31
	ldmia sp!, {lr}
	bx lr
UNK_020F1D7C: ; 0x020F1D7C
	.byte 0xFF, 0xFF, 0xFE, 0xFD
	.byte 0xFC, 0xFB, 0xFA, 0xF9, 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0, 0xF0, 0xEF, 0xEE
	.byte 0xED, 0xEC, 0xEB, 0xEA, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE2, 0xE1
	.byte 0xE0, 0xDF, 0xDF, 0xDE, 0xDD, 0xDC, 0xDC, 0xDB, 0xDA, 0xD9, 0xD9, 0xD8, 0xD7, 0xD7, 0xD6, 0xD5
	.byte 0xD4, 0xD4, 0xD3, 0xD2, 0xD2, 0xD1, 0xD0, 0xD0, 0xCF, 0xCE, 0xCE, 0xCD, 0xCC, 0xCC, 0xCB, 0xCB
	.byte 0xCA, 0xC9, 0xC9, 0xC8, 0xC8, 0xC7, 0xC6, 0xC6, 0xC5, 0xC5, 0xC4, 0xC3, 0xC3, 0xC2, 0xC2, 0xC1
	.byte 0xC0, 0xC0, 0xBF, 0xBF, 0xBE, 0xBE, 0xBD, 0xBD, 0xBC, 0xBC, 0xBB, 0xBA, 0xBA, 0xB9, 0xB9, 0xB8
	.byte 0xB8, 0xB7, 0xB7, 0xB6, 0xB6, 0xB5, 0xB5, 0xB4, 0xB4, 0xB3, 0xB3, 0xB2, 0xB2, 0xB1, 0xB1, 0xB0
	.byte 0xB0, 0xAF, 0xAF, 0xAF, 0xAE, 0xAE, 0xAD, 0xAD, 0xAC, 0xAC, 0xAB, 0xAB, 0xAA, 0xAA, 0xAA, 0xA9
	.byte 0xA9, 0xA8, 0xA8, 0xA7, 0xA7, 0xA7, 0xA6, 0xA6, 0xA5, 0xA5, 0xA4, 0xA4, 0xA4, 0xA3, 0xA3, 0xA2
	.byte 0xA2, 0xA2, 0xA1, 0xA1, 0xA0, 0xA0, 0xA0, 0x9F, 0x9F, 0x9E, 0x9E, 0x9E, 0x9D, 0x9D, 0x9D, 0x9C
	.byte 0x9C, 0x9B, 0x9B, 0x9B, 0x9A, 0x9A, 0x9A, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x97, 0x97, 0x96
	.byte 0x96, 0x96, 0x95, 0x95, 0x95, 0x94, 0x94, 0x94, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x91, 0x91
	.byte 0x91, 0x91, 0x90, 0x90, 0x90, 0x8F, 0x8F, 0x8F, 0x8E, 0x8E, 0x8E, 0x8D, 0x8D, 0x8D, 0x8C, 0x8C
	.byte 0x8C, 0x8C, 0x8B, 0x8B, 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x89, 0x89, 0x89, 0x88, 0x88, 0x88, 0x88
	.byte 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x83, 0x83
	.byte 0x83, 0x83, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80
_020F1E7C:
	mov r1, r3, lsl #1
	add r1, r1, #1
	rsb lr, lr, #0
	mul r1, lr, r1
	cmp r1, r2, lsl #24
	addmi r0, r0, #1
	ldmia sp!, {lr}
	bx lr
_020F1E9C:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp r3, #0
	beq _020F1EF4
	movs r0, r0, lsl #9
	bne _020F203C
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F202C
	cmp ip, #0xff
	blt _020F202C
	cmp r1, #0
	beq _020F2048
	b _020F2024
_020F1ED8:
	eor lr, r0, r1
	and lr, lr, #0x80000000
	cmp ip, #0
	beq _020F1F58
_020F1EE8:
	movs r1, r1, lsl #9
	bne _020F2024
	b _020F2074
_020F1EF4:
	movs r2, r0, lsl #9
	beq _020F1F28
	clz r3, r2
	movs r2, r2, lsl r3
	rsb r3, r3, #0
	mov r2, r2, lsr #8
	ands ip, ip, r1, lsr #23
	beq _020F1F80
	cmp ip, #0xff
	beq _020F1EE8
	orr r1, r1, #0x800000
	bic lr, r1, #0xff000000
	b _020F1CF8
_020F1F28:
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F1F4C
	cmp ip, #0xff
	blt _020F2074
	cmp r1, #0
	beq _020F2074
	b _020F2024
_020F1F4C:
	cmp r1, #0
	beq _020F2048
	b _020F2074
_020F1F58:
	movs ip, r1, lsl #9
	beq _020F202C
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0
	mov lr, lr, lsr #8
	orr r0, r0, #0x800000
	bic r2, r0, #0xff000000
	b _020F1CF8
_020F1F80:
	movs ip, r1, lsl #9
	beq _020F202C
	mov lr, ip
	clz ip, lr
	movs lr, lr, lsl ip
	rsb ip, ip, #0
	mov lr, lr, lsr #8
	b _020F1CF8
_020F1FA0:
	and r0, r0, #0x80000000
	cmn ip, #0x18
	beq _020F2014
	bmi _020F206C
	add r1, ip, #0x17
	mov r2, r2, lsl r1
	rsb ip, ip, #0
	mov r3, r3, lsr ip
	orr r0, r0, r3
	rsb lr, lr, #0
	mul r1, lr, r3
	cmp r1, r2
	ldmeqia sp!, {lr}
	bxeq lr
	add r1, r1, lr
	cmp r1, r2
	beq _020F2008
	addmi r0, r0, #1
	subpl r1, r1, lr
	add r1, lr, r1, lsl #1
	cmp r1, r2, lsl #1
	and r3, r0, #1
	addmi r0, r0, #1
	addeq r0, r0, r3
	ldmia sp!, {lr}
	bx lr
_020F2008:
	add r0, r0, #1
	ldmia sp!, {lr}
	bx lr
_020F2014:
	cmn r2, lr
	addne r0, r0, #1
	ldmia sp!, {lr}
	bx lr
_020F2024:
	mov r0, r1
	b _020F203C
_020F202C:
	mov r0, #0xff000000
	orr r0, lr, r0, lsr #1
	ldmia sp!, {lr}
	bx lr
_020F203C:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
_020F2048:
	mvn r0, #0x80000000
	ldmia sp!, {lr}
	bx lr
_020F2054:
	tst r0, #0x80000000
	mov r0, #0xff000000
	movne r0, r0, asr #1
	moveq r0, r0, lsr #1
	ldmia sp!, {lr}
	bx lr
_020F206C:
	ldmia sp!, {lr}
	bx lr
_020F2074:
	mov r0, lr
	ldmia sp!, {lr}
	bx lr
_f2d: ; 0x020F2080
_f_ftod:
	and r2, r0, #0x80000000
	mov ip, r0, lsr #0x17
	mov r3, r0, lsl #9
	ands ip, ip, #0xff
	beq _020F20B0
	cmp ip, #0xff
	beq _020F20DC
_020F209C:
	add ip, ip, #0x380
	mov r0, r3, lsl #0x14
	orr r1, r2, r3, lsr #12
	orr r1, r1, ip, lsl #20
	bx lr
_020F20B0:
	cmp r3, #0
	bne _020F20C4
	mov r1, r2
	mov r0, #0
	bx lr
_020F20C4:
	mov r3, r3, lsr #1
	clz ip, r3
	movs r3, r3, lsl ip
	rsb ip, ip, #1
	add r3, r3, r3
	b _020F209C
_020F20DC:
	cmp r3, #0
	bhi _020F20F4
	ldr r1, _020F2100 ; =0x7FF00000
	orr r1, r1, r2
	mov r0, #0
	bx lr
_020F20F4:
	mvn r0, #0
	bic r1, r0, #0x80000000
	bx lr
	.align 2, 0
_020F2100: .word 0x7FF00000
_f_ftoi: ; 0x020F2104
_ffix:
	bic r1, r0, #0x80000000
	mov r2, #0x9e
	subs r2, r2, r1, lsr #23
	ble _020F212C
	mov r1, r1, lsl #8
	orr r1, r1, #0x80000000
	cmp r0, #0
	mov r0, r1, lsr r2
	rsbmi r0, r0, #0
	bx lr
_020F212C:
	mvn r0, r0, asr #31
	add r0, r0, #0x80000000
	bx lr
_ffixu: ; 0x020F2138
_f_ftou:
	tst r0, #0x80000000
	bne _020F215C
	mov r1, #0x9e
	subs r1, r1, r0, lsr #23
	blt _020F2170
	mov r2, r0, lsl #8
	orr r0, r2, #0x80000000
	mov r0, r0, lsr r1
	bx lr
_020F215C:
	mov r2, #0xff000000
	cmp r2, r0, lsl #1
	movhs r0, #0
	mvnlo r0, #0
	bx lr
_020F2170:
	mvn r0, #0
	bx lr
_fflt: ; 0x020F2178
_f_itof:
	ands r2, r0, #0x80000000
	rsbmi r0, r0, #0
	cmp r0, #0
__f_itof_common:
	bxeq lr
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #0x9e
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r3, r1, #0x7f
	andeqs r3, r0, #1
	addne r0, r0, #1
	bx lr
_ffltu: ; 0x020F21C0
_f_utof:
	cmp r0, #0
__f_utof_common:
	bxeq lr
	mov r3, #0x9e
	bmi _020F21DC
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
_020F21DC:
	ands r2, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst r2, #0x80
	bxeq lr
	ands r1, r2, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_f_lltof: ; 0x020F2208
_ll_sto_f:
	ands r2, r1, #0x80000000
	beq _020F2218
	rsbs r0, r0, #0
	rsc r1, r1, #0
_020F2218:
	cmp r1, #0
	bne _020F2228
	movs r0, r0
	b __f_itof_common
_020F2228:
	clz r3, r1
	movs r1, r1, lsl r3
	rsb r3, r3, #0x20
	orr r1, r1, r0, lsr r3
	rsb ip, r3, #0x20
	movs r0, r0, lsl ip
	orrne r1, r1, #1
	add r3, r3, #0x9e
	ands ip, r1, #0xff
	add r0, r1, r1
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst ip, #0x80
	bxeq lr
	ands r3, ip, #0x7f
	andeqs r3, r0, #1
	addne r0, r0, #1
	bx lr
_f_ulltof: ; 0x020F2274
_ll_uto_f:
	cmp r1, #0
	bne _020F2284
	movs r0, r0
	b __f_utof_common
_020F2284:
	mov r3, #0x20
	bmi _020F22A4
	clz ip, r1
	movs r1, r1, lsl ip
	sub r3, r3, ip
	orr r1, r1, r0, lsr r3
	rsb r2, r3, #0x20
	mov r0, r0, lsl r2
_020F22A4:
	cmp r0, #0
	orrne r1, r1, #1
	add r3, r3, #0x9e
	ands r2, r1, #0xff
	add r0, r1, r1
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst r2, #0x80
	bxeq lr
	ands r1, r2, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_fmul: ; 0x020F22DC
_f_mul:
	eor r2, r0, r1
	and r2, r2, #0x80000000
	mov ip, #0xff
	ands r3, ip, r0, lsr #23
	mov r0, r0, lsl #8
	cmpne r3, #0xff
	beq _020F2358
	orr r0, r0, #0x80000000
	ands ip, ip, r1, lsr #23
	mov r1, r1, lsl #8
	cmpne ip, #0xff
	beq _020F2398
	orr r1, r1, #0x80000000
_020F2310:
	add ip, r3, ip
	umull r1, r3, r0, r1
	movs r0, r3
	addpl r0, r0, r0
	subpl ip, ip, #1
	subs ip, ip, #0x7f
	bmi _020F2424
	cmp ip, #0xfe
	bge _020F2490
	ands r3, r0, #0xff
	orr r0, r2, r0, lsr #8
	add r0, r0, ip, lsl #23
	tst r3, #0x80
	bxeq lr
	orrs r1, r1, r3, lsl #25
	andeqs r3, r0, #1
	addne r0, r0, #1
	bx lr
_020F2358:
	cmp r3, #0
	beq _020F23AC
	movs r0, r0, lsl #1
	bne __f_result_x_NaN
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F238C
	cmp ip, #0xff
	blt __f_result_INF
	cmp r1, #0
	beq __f_result_INF
	b __f_result_x_NaN
_020F238C:
	cmp r1, #0
	beq __f_result_invalid
	b __f_result_INF
_020F2398:
	cmp ip, #0
	beq _020F2408
_020F23A0:
	movs r1, r1, lsl #1
	bne __f_result_x_NaN
	b __f_result_INF
_020F23AC:
	movs r0, r0, lsl #1
	beq _020F23E4
	mov r0, r0, lsr #1
	clz r3, r0
	movs r0, r0, lsl r3
	rsb r3, r3, #1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq _020F2408
	cmp ip, #0xff
	beq _020F23A0
	orr r1, r1, #0x80000000
	b _020F2310
_020F23E4:
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F24B4
	cmp ip, #0xff
	blt _020F24B4
	cmp r1, #0
	beq __f_result_invalid
	b __f_result_x_NaN
_020F2408:
	movs r1, r1, lsl #1
	beq _020F24B4
	mov r1, r1, lsr #1
	clz ip, r1
	movs r1, r1, lsl ip
	rsb ip, ip, #1
	b _020F2310
_020F2424:
	cmn ip, #0x18
	beq _020F246C
	bmi _020F24AC
	cmp r1, #0
	orrne r0, r0, #1
	mov r3, r0
	mov r0, r0, lsr #8
	rsb ip, ip, #0
	orr r0, r2, r0, lsr ip
	rsb ip, ip, #0x18
	movs r1, r3, lsl ip
	bxeq lr
	tst r1, #0x80000000
	bxeq lr
	movs r1, r1, lsl #1
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F246C:
	mov r0, r0, lsl #1
	b _020F249C
__f_result_INF:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
__f_result_x_NaN:
	mvn r0, #0x80000000
	bx lr
__f_result_invalid:
	mvn r0, #0x80000000
	bx lr
_020F2490:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_020F249C:
	movs r1, r0
	mov r0, r2
	addne r0, r0, #1
	bx lr
_020F24AC:
	mov r0, r2
	bx lr
_020F24B4:
	mov r0, r2
	bx lr
_frsb: ; 0x020F24BC
	eor r0, r0, r1
	eor r1, r0, r1
	eor r0, r0, r1
_fsub: ; 0x020F24C8
_f_sub: ; 0x020F24C8
	eors r2, r0, r1
	eormi r1, r1, #0x80000000
	bmi __fadd_start
__fsub_start:
	subs ip, r0, r1
	eorlo ip, ip, #0x80000000
	sublo r0, r0, ip
	addlo r1, r1, ip
	mov r2, #0x80000000
	mov r3, r0, lsr #0x17
	orr r0, r2, r0, lsl #8
	ands ip, r3, #0xff
	cmpne ip, #0xff
	beq _020F25F0
	mov ip, r1, lsr #0x17
	orr r1, r2, r1, lsl #8
	ands r2, ip, #0xff
	beq _020F2630
_020F250C:
	subs ip, r3, ip
	beq _020F2554
	rsb r2, ip, #0x20
	movs r2, r1, lsl r2
	mov r1, r1, lsr ip
	orrne r1, r1, #1
	subs r0, r0, r1
	bpl _020F2598
	ands r1, r0, #0xff
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F2554:
	subs r0, r0, r1
	beq _020F26FC
	mov r2, r3, lsl #0x17
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	cmp r3, #0
	bgt _020F2588
	rsb r3, r3, #9
	orr r0, r2, r0, lsr r3
	bx lr
_020F2588:
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
_020F2598:
	mov r2, r3, lsl #0x17
	and r2, r2, #0x80000000
	bic r3, r3, #0x100
	clz ip, r0
	movs r0, r0, lsl ip
	sub r3, r3, ip
	cmp r3, #0
	bgt _020F25C4
	rsb r3, r3, #9
	orr r0, r2, r0, lsr r3
	bx lr
_020F25C4:
	ands r1, r0, #0xff
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bxeq lr
	tst r1, #0x80
	bxeq lr
	ands r1, r1, #0x7f
	andeqs r1, r0, #1
	addne r0, r0, #1
	bx lr
_020F25F0:
	cmp r3, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	ands r3, r3, #0xff
	beq _020F2658
	movs r0, r0, lsl #1
	bne _020F2730
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #9
	ands ip, ip, #0xff
	beq _020F2724
	cmp ip, #0xff
	blt _020F2724
	cmp r1, #0
	beq _020F2738
	b _020F2730
_020F2630:
	cmp ip, #0x100
	movge r2, #0x80000000
	movlt r2, #0
	and r3, r3, #0xff
	ands ip, ip, #0xff
	beq _020F26C0
_020F2648:
	eor r2, r2, #0x80000000
	movs r1, r1, lsl #1
	bne _020F2730
	b _020F2724
_020F2658:
	movs r0, r0, lsl #1
	beq _020F2690
	mov r0, r0, lsr #1
	mov r3, #1
	mov ip, r1, lsr #0x17
	mov r1, r1, lsl #8
	ands ip, ip, #0xff
	beq _020F26C0
	cmp ip, #0xff
	beq _020F2648
	orr r1, r1, #0x80000000
	orr r3, r3, r2, lsr #23
	orr ip, ip, r2, lsr #23
	b _020F250C
_020F2690:
	mov r3, r1, lsr #0x17
	mov r0, r1, lsl #9
	ands r2, r3, #0xff
	beq _020F26B4
	cmp r2, #0xff
	blt _020F26DC
	cmp r0, #0
	bne _020F271C
	b _020F2724
_020F26B4:
	cmp r0, #0
	beq _020F26FC
	b _020F26DC
_020F26C0:
	movs r1, r1, lsl #1
	beq _020F26E4
	mov r1, r1, lsr #1
	mov ip, #1
	orr ip, ip, r2, lsr #23
	orr r3, r3, r2, lsr #23
	b _020F250C
_020F26DC:
	mov r0, r1
	bx lr
_020F26E4:
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	orr r0, r2, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
_020F26FC:
	mov r0, #0
	bx lr
_020F2704: ; 0x020F2704
	cmp r0, #0
	subges r3, r3, #1
	add r0, r0, r0
	mov r0, r0, lsr #9
	orr r0, r0, r3, lsl #23
	bx lr
_020F271C:
	mvn r0, #0x80000000
	bx lr
_020F2724:
	mov r0, #0xff000000
	orr r0, r2, r0, lsr #1
	bx lr
_020F2730:
	mvn r0, #0x80000000
	bx lr
_020F2738:
	mvn r0, #0x80000000
	bx lr
_ll_mod: ; 0x020F2740
	stmdb sp!, {r4, r5, r6, r7, fp, ip, lr}
	mov r4, r1
	orr r4, r4, #1
	b _020F2760
_ll_sdiv: ; 0x020F2750
_ll_div: ; 0x020F2750
	stmdb sp!, {r4, r5, r6, r7, fp, ip, lr}
	eor r4, r1, r3
	mov r4, r4, asr #1
	mov r4, r4, lsl #1
_020F2760:
	orrs r5, r3, r2
	bne _020F2770
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_020F2770:
	mov r5, r0, lsr #0x1f
	add r5, r5, r1
	mov r6, r2, lsr #0x1f
	add r6, r6, r3
	orrs r6, r5, r6
	bne _020F27A4
	mov r1, r2
	bl _s32_div_f
	ands r4, r4, #1
	movne r0, r1
	mov r1, r0, asr #0x1f
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_020F27A4:
	cmp r1, #0
	bge _020F27B4
	rsbs r0, r0, #0
	rsc r1, r1, #0
_020F27B4:
	cmp r3, #0
	bge _020F27C4
	rsbs r2, r2, #0
	rsc r3, r3, #0
_020F27C4:
	orrs r5, r1, r0
	beq _020F28E8
	mov r5, #0
	mov r6, #1
	cmp r3, #0
	bmi _020F27F0
_020F27DC:
	add r5, r5, #1
	adds r2, r2, r2
	adcs r3, r3, r3
	bpl _020F27DC
	add r6, r6, r5
_020F27F0:
	cmp r1, #0
	blt _020F2810
_020F27F8:
	cmp r6, #1
	beq _020F2810
	sub r6, r6, #1
	adds r0, r0, r0
	adcs r1, r1, r1
	bpl _020F27F8
_020F2810:
	mov r7, #0
	mov ip, #0
	mov fp, #0
	b _020F2838
_020F2820:
	orr ip, ip, #1
	subs r6, r6, #1
	beq _020F2890
	adds r0, r0, r0
	adcs r1, r1, r1
	adcs r7, r7, r7
_020F2838:
	subs r0, r0, r2
	sbcs r1, r1, r3
	sbcs r7, r7, #0
	adds ip, ip, ip
	adc fp, fp, fp
	cmp r7, #0
	bge _020F2820
_020F2854:
	subs r6, r6, #1
	beq _020F2888
	adds r0, r0, r0
	adcs r1, r1, r1
	adc r7, r7, r7
	adds r0, r0, r2
	adcs r1, r1, r3
	adc r7, r7, #0
	adds ip, ip, ip
	adc fp, fp, fp
	cmp r7, #0
	bge _020F2820
	b _020F2854
_020F2888:
	adds r0, r0, r2
	adc r1, r1, r3
_020F2890:
	ands r7, r4, #1
	moveq r0, ip
	moveq r1, fp
	beq _020F28C8
	subs r7, r5, #0x20
	movge r0, r1, lsr r7
	bge _020F28EC
	rsb r7, r5, #0x20
	mov r0, r0, lsr r5
	orr r0, r0, r1, lsl r7
	mov r1, r1, lsr r5
	b _020F28C8
_020F28C0: ; 0x020F28C0
	mov r0, r1, lsr r7
	mov r1, #0
_020F28C8:
	cmp r4, #0
	blt _020F28D8
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_020F28D8:
	rsbs r0, r0, #0
	rsc r1, r1, #0
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_020F28E8:
	mov r0, #0
_020F28EC:
	mov r1, #0
	cmp r4, #0
	blt _020F28D8
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_ll_udiv: ; 0x020F2900
_ull_div:
	stmdb sp!, {r4, r5, r6, r7, fp, ip, lr}
	mov r4, #0
	b _020F2914
_ull_mod: ; 0x020F290C
	stmdb sp!, {r4, r5, r6, r7, fp, ip, lr}
	mov r4, #1
_020F2914:
	orrs r5, r3, r2
	bne _020F2924
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_020F2924:
	orrs r5, r1, r3
	bne _020F27C4
	mov r1, r2
	bl _u32_div_not_0_f
	cmp r4, #0
	movne r0, r1
	mov r1, #0
	ldmia sp!, {r4, r5, r6, r7, fp, ip, lr}
	bx lr
_ll_mul: ; 0x020F2948
_ull_mul: ; 0x020F2948
	stmdb sp!, {r4, r5, lr}
	umull r5, r4, r0, r2
	mla r4, r0, r3, r4
	mla r4, r2, r1, r4
	mov r1, r4
	mov r0, r5
	ldmia sp!, {r4, r5, lr}
	bx lr
_ll_shl: ; 0x020F2968
_ll_sll:
_ull_sll:
	ands r2, r2, #0x3f
	bxeq lr
	subs r3, r2, #0x20
	bge _020F298C
	rsb r3, r2, #0x20
	mov r1, r1, lsl r2
	orr r1, r1, r0, lsr r3
	mov r0, r0, lsl r2
	bx lr
_020F298C:
	mov r1, r0, lsl r3
	mov r0, #0
	bx lr
_s32_div_f: ; 0x020F2998
	eor ip, r0, r1
	and ip, ip, #0x80000000
	cmp r0, #0
	rsblt r0, r0, #0
	addlt ip, ip, #1
	cmp r1, #0
	rsblt r1, r1, #0
	beq _020F2B90
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0
	blo _020F2B90
	mov r2, #0x1c
	mov r3, r0, lsr #4
	cmp r1, r3, lsr #12
	suble r2, r2, #0x10
	movle r3, r3, lsr #0x10
	cmp r1, r3, lsr #4
	suble r2, r2, #8
	movle r3, r3, lsr #8
	cmp r1, r3
	suble r2, r2, #4
	movle r3, r3, lsr #4
	mov r0, r0, lsl r2
	rsb r1, r1, #0
	adds r0, r0, r0
	add r2, r2, r2, lsl #1
	add pc, pc, r2, lsl #2
	nop
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3
_020F2B90:
	ands r3, ip, #0x80000000
	rsbne r0, r0, #0
	ands r3, ip, #1
	rsbne r1, r1, #0
	bx lr
_u32_div_f: ; 0x020F2BA4
	cmp r1, #0
	bxeq lr
_u32_div_not_0_f: ; 0x020F2BAC
	cmp r0, r1
	movlo r1, r0
	movlo r0, #0
	bxlo lr
	mov r2, #0x1c
	mov r3, r0, lsr #4
	cmp r1, r3, lsr #12
	suble r2, r2, #0x10
	movle r3, r3, lsr #0x10
	cmp r1, r3, lsr #4
	suble r2, r2, #8
	movle r3, r3, lsr #8
	cmp r1, r3
	suble r2, r2, #4
	movle r3, r3, lsr #4
	mov r0, r0, lsl r2
	rsb r1, r1, #0
	adds r0, r0, r0
	add r2, r2, r2, lsl #1
	add pc, pc, r2, lsl #2
	nop
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	adcs r3, r1, r3, lsl #1
	sublo r3, r3, r1
	adcs r0, r0, r0
	mov r1, r3
	bx lr
_drdiv: ; 0x020F2D88
	eor r1, r1, r3
	eor r3, r1, r3
	eor r1, r1, r3
	eor r0, r0, r2
	eor r2, r0, r2
	eor r0, r0, r2
_ddiv: ; 0x020F2DA0
_d_div:
	stmdb sp!, {r4, r5, r6, lr}
	ldr lr, _020F32E0 ; =0x00000FFE
	eor r4, r1, r3
	ands ip, lr, r1, lsr #19
	cmpne ip, lr
	beq _020F314C
	bic r1, r1, lr, lsl #20
	orr r1, r1, #0x100000
	add ip, ip, r4, lsr #31
_020F2DC4:
	ands r4, lr, r3, lsr #19
	cmpne r4, lr
	beq _020F31E4
	bic r3, r3, lr, lsl #20
	orr r3, r3, #0x100000
_020F2DD8:
	sub ip, ip, r4
	cmp r1, r3
	cmpeq r0, r2
	bhs _020F2DF4
	adds r0, r0, r0
	adc r1, r1, r1
	sub ip, ip, #2
_020F2DF4:
	adr r4, UNK_020F2ED8 - 0x100
	ldrb lr, [r4, r3, lsr #12]
	rsbs r2, r2, #0
	rsc r3, r3, #0
	mov r4, #0x20000000
	mla r5, lr, r3, r4
	mov r6, r3, lsl #0xa
	mov r5, r5, lsr #7
	mul lr, r5, lr
	orr r6, r6, r2, lsr #22
	mov lr, lr, lsr #0xd
	mul r5, lr, r6
	mov r6, r1, lsl #0xa
	orr r6, r6, r0, lsr #22
	mov r5, r5, lsr #0x10
	mul r5, lr, r5
	mov lr, lr, lsl #0xe
	add lr, lr, r5, lsr #16
	umull r5, r6, lr, r6
	umull r4, r5, r6, r2
	mla r5, r3, r6, r5
	mov r4, r4, lsr #0x1a
	orr r4, r4, r5, lsl #6
	add r4, r4, r0, lsl #2
	umull lr, r5, r4, lr
	mov r4, #0
	adds r5, r5, r6, lsl #24
	adc r4, r4, r6, lsr #8
	cmp ip, #0x800
	bge _020F2FD8
	add ip, ip, #0x7f0
	adds ip, ip, #0xc
	bmi _020F2FF0
	orr r1, r4, ip, lsl #31
	bic ip, ip, #1
	add r1, r1, ip, lsl #19
	tst lr, #0x80000000
	bne _020F2EC8
	rsbs r2, r2, #0
	mov r4, r4, lsl #1
	add r4, r4, r5, lsr #31
	mul lr, r2, r4
	mov r6, #0
	mov r4, r5, lsl #1
	orr r4, r4, #1
	umlal r6, lr, r4, r2
	rsc r3, r3, #0
	mla lr, r4, r3, lr
	cmp lr, r0, lsl #21
	bmi _020F2EC8
	mov r0, r5
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F2EC8:
	adds r0, r5, #1
	adc r1, r1, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
UNK_020F2ED8: ; 0x020F2ED8
	.byte 0xFF, 0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA, 0xF9
	.byte 0xF8, 0xF7, 0xF6, 0xF5, 0xF4, 0xF3, 0xF2, 0xF1, 0xF0, 0xF0, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA
	.byte 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE2, 0xE1, 0xE0, 0xDF, 0xDF, 0xDE
	.byte 0xDD, 0xDC, 0xDC, 0xDB, 0xDA, 0xD9, 0xD9, 0xD8, 0xD7, 0xD7, 0xD6, 0xD5, 0xD4, 0xD4, 0xD3, 0xD2
	.byte 0xD2, 0xD1, 0xD0, 0xD0, 0xCF, 0xCE, 0xCE, 0xCD, 0xCC, 0xCC, 0xCB, 0xCB, 0xCA, 0xC9, 0xC9, 0xC8
	.byte 0xC8, 0xC7, 0xC6, 0xC6, 0xC5, 0xC5, 0xC4, 0xC3, 0xC3, 0xC2, 0xC2, 0xC1, 0xC0, 0xC0, 0xBF, 0xBF
	.byte 0xBE, 0xBE, 0xBD, 0xBD, 0xBC, 0xBC, 0xBB, 0xBA, 0xBA, 0xB9, 0xB9, 0xB8, 0xB8, 0xB7, 0xB7, 0xB6
	.byte 0xB6, 0xB5, 0xB5, 0xB4, 0xB4, 0xB3, 0xB3, 0xB2, 0xB2, 0xB1, 0xB1, 0xB0, 0xB0, 0xAF, 0xAF, 0xAF
	.byte 0xAE, 0xAE, 0xAD, 0xAD, 0xAC, 0xAC, 0xAB, 0xAB, 0xAA, 0xAA, 0xAA, 0xA9, 0xA9, 0xA8, 0xA8, 0xA7
	.byte 0xA7, 0xA7, 0xA6, 0xA6, 0xA5, 0xA5, 0xA4, 0xA4, 0xA4, 0xA3, 0xA3, 0xA2, 0xA2, 0xA2, 0xA1, 0xA1
	.byte 0xA0, 0xA0, 0xA0, 0x9F, 0x9F, 0x9E, 0x9E, 0x9E, 0x9D, 0x9D, 0x9D, 0x9C, 0x9C, 0x9B, 0x9B, 0x9B
	.byte 0x9A, 0x9A, 0x9A, 0x99, 0x99, 0x99, 0x98, 0x98, 0x98, 0x97, 0x97, 0x96, 0x96, 0x96, 0x95, 0x95
	.byte 0x95, 0x94, 0x94, 0x94, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x91, 0x91, 0x91, 0x91, 0x90, 0x90
	.byte 0x90, 0x8F, 0x8F, 0x8F, 0x8E, 0x8E, 0x8E, 0x8D, 0x8D, 0x8D, 0x8C, 0x8C, 0x8C, 0x8C, 0x8B, 0x8B
	.byte 0x8B, 0x8A, 0x8A, 0x8A, 0x8A, 0x89, 0x89, 0x89, 0x88, 0x88, 0x88, 0x88, 0x87, 0x87, 0x87, 0x86
	.byte 0x86, 0x86, 0x86, 0x85, 0x85, 0x85, 0x85, 0x84, 0x84, 0x84, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82
	.byte 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x80, 0x80
_020F2FD8:
	movs r1, ip, lsl #0x1f
	orr r1, r1, #0x7f000000
	orr r1, r1, #0xf00000
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F2FF0:
	mvn r6, ip, asr #1
	cmp r6, #0x34
	bgt _020F313C
	beq _020F3118
	cmp r6, #0x14
	bge _020F3038
	rsb r6, r6, #0x13
	mov lr, r0, lsl r6
	rsb r6, r6, #0x14
	mov r0, r5, lsr r6
	rsb r6, r6, #0x20
	orr r0, r0, r4, lsl r6
	rsb r6, r6, #0x20
	mov r4, r4, lsr r6
	orr r1, r4, ip, lsl #31
	mov ip, lr
	mov lr, #0
	b _020F3068
_020F3038:
	rsb r6, r6, #0x33
	mov lr, r1, lsl r6
	mov r1, ip, lsl #0x1f
	rsb r6, r6, #0x20
	orr ip, lr, r0, lsr r6
	rsb r6, r6, #0x20
	mov lr, r0, lsl r6
	mov r5, r5, lsr #0x15
	orr r5, r5, r4, lsl #11
	rsb r6, r6, #0x1f
	mov r0, r5, lsr r6
	mov r4, #0
_020F3068:
	rsbs r2, r2, #0
	mul r4, r2, r4
	mov r5, #0
	umlal r5, r4, r2, r0
	rsc r3, r3, #0
	mla r4, r0, r3, r4
	cmp r4, ip
	cmpeq r5, lr
	ldmeqia sp!, {r4, r5, r6, lr}
	bxeq lr
	adds r5, r5, r2
	adc r4, r4, r3
	cmp r4, ip
	bmi _020F310C
	bne _020F30B0
	cmp r5, lr
	beq _020F30FC
	blo _020F310C
_020F30B0:
	subs r5, r5, r2
	sbc r4, r4, r3
_020F30B8:
	adds r5, r5, r5
	adc r4, r4, r4
	adds r5, r5, r2
	adc r4, r4, r3
	adds lr, lr, lr
	adc ip, ip, ip
	cmp r4, ip
	bmi _020F30FC
	ldmneia sp!, {r4, r5, r6, lr}
	bxne lr
	cmp r5, lr
	blo _020F30FC
	ldmneia sp!, {r4, r5, r6, lr}
	bxne lr
	tst r0, #1
	ldmeqia sp!, {r4, r5, r6, lr}
	bxeq lr
_020F30FC:
	adds r0, r0, #1
	adc r1, r1, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F310C:
	adds r0, r0, #1
	adc r1, r1, #0
	b _020F30B8
_020F3118:
	rsbs r2, r2, #0
	rsc r3, r3, #0
	cmp r1, r3
	cmpeq r0, r2
	mov r1, ip, lsl #0x1f
	mov r0, #0
	movne r0, #1
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F313C:
	mov r1, ip, lsl #0x1f
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F314C:
	orrs r5, r0, r1, lsl #1
	beq _020F3270
	cmp ip, lr
	beq _020F31B4
	movs r1, r1, lsl #0xc
	beq _020F3190
	clz r5, r1
	movs r1, r1, lsl r5
	sub ip, ip, r5
	add r5, ip, #0x1f
	mov r1, r1, lsr #0xb
	orr r1, r1, r0, lsr r5
	rsb r5, r5, #0x20
	mov r0, r0, lsl r5
	mov ip, ip, lsl #1
	orr ip, ip, r4, lsr #31
	b _020F2DC4
_020F3190:
	mvn ip, #0x13
	clz r5, r0
	movs r0, r0, lsl r5
	sub ip, ip, r5
	mov r1, r0, lsr #0xb
	mov r0, r0, lsl #0x15
	mov ip, ip, lsl #1
	orr ip, ip, r4, lsr #31
	b _020F2DC4
_020F31B4:
	orrs r5, r0, r1, lsl #12
	bne _020F3298
	bic r5, r3, #0x80000000
	cmp r5, lr, lsl #19
	bhs _020F31D8
	and r5, r3, #0x80000000
	eor r1, r5, r1
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F31D8:
	orrs r5, r2, r3, lsl #12
	bne _020F32B8
	b _020F32D0
_020F31E4:
	orrs r5, r2, r3, lsl #1
	beq _020F325C
	cmp r4, lr
	beq _020F3244
	movs r3, r3, lsl #0xc
	beq _020F3224
	clz r5, r3
	movs r3, r3, lsl r5
	sub r4, r4, r5
	add r5, r4, #0x1f
	mov r3, r3, lsr #0xb
	orr r3, r3, r2, lsr r5
	rsb r5, r5, #0x20
	mov r2, r2, lsl r5
	mov r4, r4, lsl #1
	b _020F2DD8
_020F3224:
	mvn r4, #0x13
	clz r5, r2
	movs r2, r2, lsl r5
	sub r4, r4, r5
	mov r3, r2, lsr #0xb
	mov r2, r2, lsl #0x15
	mov r4, r4, lsl #1
	b _020F2DD8
_020F3244:
	orrs r5, r2, r3, lsl #12
	bne _020F32B8
	mov r1, ip, lsl #0x1f
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F325C:
	mov r1, ip, lsl #0x1f
	orr r1, r1, lr, lsl #19
	mov r0, #0
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F3270:
	orrs r5, r2, r3, lsl #1
	beq _020F32D0
	bic r5, r3, #0x80000000
	cmp r5, lr, lsl #19
	cmpeq r2, #0
	bhi _020F32B8
	eor r1, r1, r3
	and r1, r1, #0x80000000
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F3298:
	tst r1, #0x80000
	beq _020F32D0
	bic r5, r3, #0x80000000
	cmp r5, lr, lsl #19
	cmpeq r2, #0
	bhi _020F32B8
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F32B8:
	tst r3, #0x80000
	beq _020F32D0
	mov r1, r3
	mov r0, r2
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
_020F32D0:
	orr r1, r1, #0x7f000000
	orr r1, r1, #0xf80000
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
	.align 2, 0
_020F32E0: .word 0x00000FFE
