	.extern ov13_0222B898
	.extern ov13_0222B8E8
	.extern ov13_0222B90C
	.extern ov13_0222B970
	.extern ov13_0222B980
	.extern ov13_0222BADC
	.extern ov13_0222BAF4
	.extern ov13_0222C1BC
	.extern ov13_02239FF0
	.extern ov13_0223A6E4
	.extern ov13_0223AAFC
	.extern ov13_0223ABB8
	.extern ov13_0223AC4C
	.extern ov13_0223B1E0
	.extern ov13_0223F664
	.extern ov13_0223F95C
	.extern ov13_0223FA50
	.extern ov13_0223FADC
	.extern ov13_0223FC04
	.extern ov13_0223FD60
	.extern ov13_0223FDE0
	.extern ov13_0223FDF4
	.extern ov13_0223FE98
	.extern ov13_0223FF18
	.extern ov13_02240008
	.extern ov13_02240068
	.extern ov13_022400F0
	.extern ov13_02240184
	.extern ov13_022401B0
	.extern ov13_02240338
	.extern ov13_02240648
	.extern ov13_02240798
	.extern ov13_02240874
	.extern ov13_022408A0
	.extern ov13_02240B04
	.extern ov13_02240B28
	.extern ov13_02240CC0
	.extern ov13_02240D94
	.extern ov13_02240E2C
	.extern ov13_02240E74
	.extern ov13_022411DC
	.extern ov13_0224128C
	.extern ov13_022412B0
	.extern ov13_022412F4
	.extern ov13_02241330
	.extern ov13_022413E4
	.extern ov13_02241694
	.extern ov13_022416D4
	.extern ov13_02241714
	.extern ov13_02241754
	.extern ov13_022417DC
	.extern ov13_022418EC
	.extern ov13_02241A44
	.extern ov13_02241AC0
	.extern ov13_02241C68
	.extern ov13_02241D98
	.extern ov13_02241DFC
	.extern ov13_022420A0
	.extern ov13_022422E0
	.extern ov13_0224231C
	.extern ov13_02242478
	.extern ov13_02242528
	.extern ov13_022425C8
	.extern ov13_022425D0
	.extern ov13_022425F0
	.extern ov13_02244EB0
	.extern ov13_02244EB8
	.extern ov13_02244EBC
	.extern ov13_02244EBE
	.extern ov13_02244EC4
	.extern ov13_02244ED2
	.extern ov13_02244EE0
	.extern ov13_02244EF8
	.extern ov13_02244F14
	.extern ov13_02244F16
	.extern ov13_02244F30
	.extern ov13_02244F38
	.extern ov13_02244F44
	.extern ov13_02244F4C
	.extern ov13_02244F50
	.extern ov13_02244F54
	.extern ov13_02244F58
	.extern ov13_02244F5C
	.extern ov13_02244F60
	.extern ov13_02244F64
	.extern ov13_02244F68
	.extern ov13_02244F7C
	.extern ov13_02244F84
	.extern ov13_02244F88
	.extern ov13_02244F8C
	.extern ov13_02244F94
	.extern ov13_02244F98
	.extern ov13_02244F9C
	.extern ov13_02244FA4
	.extern ov13_02245060
	.extern ov13_02245062
	.extern ov13_02245134
	.extern ov13_02245208
	.extern ov13_02245210
	.extern ov13_02245214
	.extern ov13_02245218
	.extern ov13_0224521C
	.extern ov13_02245220
	.extern ov13_02245224
	.extern ov13_02245228
	.extern ov13_02245230
	.extern ov13_02245234
	.extern ov13_02245250
	.extern ov13_02245258
	.extern ov13_02245260
	.extern ov13_0224527A
	.extern ov13_022452A2
	.extern ov13_022452A4
	.extern ov13_022452DA
	.extern ov13_02245314
	.extern ov13_02245320
	.extern ov13_02245958
	.extern ov13_02245B20
	.extern ov13_02245D04
	.extern ov13_02245D54
	.extern ov13_02245D68
	.extern ov13_02245D74
	.extern ov13_02245D80
	.extern ov13_0224F464
	.extern ov13_0224F468
	.extern ov13_0224F46C
	.extern ov13_0224F470
	.extern ov13_0224F4B0
	.extern ov13_0224F4B4
	.extern ov13_0224F4B8
	.extern ov13_0224F4BC
	.extern ov13_0224F4C0
	.extern ov13_0224F4C4
	.extern ov13_0224F4C8
	.extern ov13_0224F4CC
	.extern ov13_0224F4F8
	.extern ov13_02228C08
	.extern ov13_02228C38
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0222A378
	.public ov13_0222A554
	.public ov13_0222A724
	.extern ov13_0222A914
	.extern ov13_0222AE24

.text
	arm_func_start ov13_0222A378
ov13_0222A378: ; 0x0222A378
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r0, _0222A534 ; =ov13_02245958
	bl ov13_022417DC
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r0, _0222A538 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	ldr r7, _0222A53C ; =ov13_0224527A
	ldr r6, _0222A540 ; =ov13_02245228
	strb r1, [r0, #0x61]
	mov r4, #0
	add r5, sp, #0
_0222A3B4:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A414
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x67]
	cmp r0, #0
	bne _0222A3F8
	mov r0, #9
	bl ov13_0223ABB8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A3F8:
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add sp, sp, #8
	ldr r0, [r0]
	strb r4, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A414:
	add r4, r4, #1
	cmp r4, #0xa
	add r7, r7, #4
	blt _0222A3B4
	ldr r7, _0222A544 ; =ov13_02245250
	ldr r6, _0222A548 ; =ov13_02245230
	mov r4, #0
	add r5, sp, #0
_0222A434:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A4BC
	cmp r4, #0
	bne _0222A470
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x66]
	cmp r0, #0
	beq _0222A48C
_0222A470:
	cmp r4, #1
	bne _0222A49C
	ldr r0, _0222A538 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrb r0, [r0, #0x68]
	cmp r0, #0
	bne _0222A49C
_0222A48C:
	mov r0, #9
	bl ov13_0223ABB8
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A49C:
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add r1, r4, #0xa
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A4BC:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A434
	ldr r6, _0222A54C ; =ov13_02245258
	ldr r5, _0222A550 ; =ov13_02245234
	mov r7, #0
	add r4, sp, #0
_0222A4DC:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_022417DC
	cmp r0, #0
	beq _0222A51C
	mov r0, #0
	bl ov13_0223ABB8
	ldr r0, _0222A538 ; =ov13_0224F4C0
	add r1, r7, #0xc
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A51C:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A4DC
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A534: .word ov13_02245958
_0222A538: .word ov13_0224F4C0
_0222A53C: .word ov13_0224527A
_0222A540: .word ov13_02245228
_0222A544: .word ov13_02245250
_0222A548: .word ov13_02245230
_0222A54C: .word ov13_02245258
_0222A550: .word ov13_02245234
	arm_func_end ov13_0222A378

	arm_func_start ov13_0222A554
ov13_0222A554: ; 0x0222A554
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r1, _0222A6F8 ; =ov13_0224F4C0
	ldr r0, _0222A6FC ; =ov13_02245958
	ldr r1, [r1]
	mov r2, #0
	strb r2, [r1, #0x60]
	bl ov13_022418EC
	cmp r0, #0
	addeq sp, sp, #8
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r7, _0222A700 ; =ov13_0224527A
	ldr r6, _0222A704 ; =ov13_02245228
	mov r4, #0
	add r5, sp, #0
_0222A590:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A5E8
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	ldr r2, [r0]
	ldrsb r1, [r2, #0x61]
	cmp r1, r4
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A708 ; =ov13_02245314
	ldrb r1, [r1, r4]
	strb r1, [r2, #0x60]
	ldr r0, [r0]
	strb r4, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A5E8:
	add r4, r4, #1
	cmp r4, #0xa
	add r7, r7, #4
	blt _0222A590
	ldr r7, _0222A70C ; =ov13_02245250
	ldr r6, _0222A710 ; =ov13_02245230
	mov r4, #0
	add r5, sp, #0
_0222A608:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A664
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	add r2, r4, #0xa
	ldr r3, [r0]
	ldrsb r1, [r3, #0x61]
	cmp r1, r2
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A714 ; =ov13_02245224
	ldrb r1, [r1, r4]
	strb r1, [r3, #0x60]
	ldr r0, [r0]
	strb r2, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A664:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A608
	ldr r7, _0222A718 ; =ov13_02245258
	ldr r6, _0222A71C ; =ov13_02245234
	mov r4, #0
	add r5, sp, #0
_0222A684:
	mov r0, r7
	mov r1, r6
	mov r2, r5
	bl ov13_02240E74
	mov r0, r5
	bl ov13_022418EC
	cmp r0, #0
	beq _0222A6E0
	ldr r0, _0222A6F8 ; =ov13_0224F4C0
	add r2, r4, #0xc
	ldr r3, [r0]
	ldrsb r1, [r3, #0x61]
	cmp r1, r2
	addne sp, sp, #8
	ldmneia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r1, _0222A720 ; =ov13_02245220
	ldrb r1, [r1, r4]
	strb r1, [r3, #0x60]
	ldr r0, [r0]
	strb r2, [r0, #0x63]
	bl ov13_0222AE24
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A6E0:
	add r4, r4, #1
	cmp r4, #2
	add r7, r7, #4
	blt _0222A684
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A6F8: .word ov13_0224F4C0
_0222A6FC: .word ov13_02245958
_0222A700: .word ov13_0224527A
_0222A704: .word ov13_02245228
_0222A708: .word ov13_02245314
_0222A70C: .word ov13_02245250
_0222A710: .word ov13_02245230
_0222A714: .word ov13_02245224
_0222A718: .word ov13_02245258
_0222A71C: .word ov13_02245234
_0222A720: .word ov13_02245220
	arm_func_end ov13_0222A554

	arm_func_start ov13_0222A724
ov13_0222A724: ; 0x0222A724
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r0, _0222A8F4 ; =ov13_02245958
	bl ov13_02241754
	cmp r0, #0
	beq _0222A8D4
	ldr r7, _0222A8F8 ; =ov13_0224527A
	ldr r5, _0222A8FC ; =ov13_02245228
	mov r6, #0
	add r4, sp, #0
_0222A74C:
	mov r0, r7
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A78C
	ldr r0, _0222A900 ; =ov13_0224F4C0
	ldr r0, [r0]
	ldrsb r0, [r0, #0x61]
	cmp r0, r6
	bne _0222A8D4
	mov r0, r6
	bl ov13_0222A914
	b _0222A8DC
_0222A78C:
	add r6, r6, #1
	cmp r6, #0xa
	add r7, r7, #4
	blt _0222A74C
	ldr r6, _0222A904 ; =ov13_02245250
	ldr r5, _0222A908 ; =ov13_02245230
	mov r7, #0
	add r4, sp, #0
_0222A7AC:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A864
	ldr r1, _0222A900 ; =ov13_0224F4C0
	add r0, r7, #0xa
	ldr r1, [r1]
	ldrsb r1, [r1, #0x61]
	cmp r1, r0
	bne _0222A8D4
	bl ov13_0222A914
	cmp r7, #0
	bne _0222A8DC
	ldr r0, _0222A900 ; =ov13_0224F4C0
	ldr r2, [r0]
	ldrb r1, [r2, #0x65]
	add r1, r1, #1
	strb r1, [r2, #0x65]
	ldr r2, [r0]
	ldrb r1, [r2, #0x65]
	cmp r1, #0x28
	addlo sp, sp, #8
	ldmloia sp!, {r3, r4, r5, r6, r7, pc}
	ldrb r1, [r2, #0x66]
	cmp r1, #0
	bne _0222A844
	mov r0, #9
	bl ov13_0223ABB8
	ldr r0, _0222A900 ; =ov13_0224F4C0
	mvn r1, #0
	ldr r0, [r0]
	add sp, sp, #8
	strb r1, [r0, #0x61]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A844:
	mov r1, #0x10
	strb r1, [r2, #0x60]
	ldr r1, [r0]
	add sp, sp, #8
	ldrb r0, [r1, #0x65]
	sub r0, r0, #7
	strb r0, [r1, #0x65]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_0222A864:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A7AC
	ldr r6, _0222A90C ; =ov13_02245258
	ldr r5, _0222A910 ; =ov13_02245234
	mov r7, #0
	add r4, sp, #0
_0222A884:
	mov r0, r6
	mov r1, r5
	mov r2, r4
	bl ov13_02240E74
	mov r0, r4
	bl ov13_02241754
	cmp r0, #0
	beq _0222A8C4
	ldr r1, _0222A900 ; =ov13_0224F4C0
	add r0, r7, #0xc
	ldr r1, [r1]
	ldrsb r1, [r1, #0x61]
	cmp r1, r0
	bne _0222A8D4
	bl ov13_0222A914
	b _0222A8DC
_0222A8C4:
	add r7, r7, #1
	cmp r7, #2
	add r6, r6, #4
	blt _0222A884
_0222A8D4:
	mvn r0, #0
	bl ov13_0222A914
_0222A8DC:
	ldr r0, _0222A900 ; =ov13_0224F4C0
	mov r1, #0
	ldr r0, [r0]
	strb r1, [r0, #0x65]
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0222A8F4: .word ov13_02245958
_0222A8F8: .word ov13_0224527A
_0222A8FC: .word ov13_02245228
_0222A900: .word ov13_0224F4C0
_0222A904: .word ov13_02245250
_0222A908: .word ov13_02245230
_0222A90C: .word ov13_02245258
_0222A910: .word ov13_02245234
	arm_func_end ov13_0222A724
