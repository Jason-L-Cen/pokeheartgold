	.extern ov13_02227228
	.extern ov13_02227258
	.extern ov13_0222726C
	.extern ov13_022272A0
	.extern ov13_022272CC
	.extern ov13_022272E4
	.extern ov13_022277B8
	.extern ov13_0222788C
	.extern ov13_022278C4
	.extern ov13_02227924
	.extern ov13_02227964
	.extern ov13_022279B4
	.extern ov13_02227B1C
	.extern ov13_02227B6C
	.extern ov13_02227D18
	.extern ov13_02227DE4
	.extern ov13_02227E58
	.extern ov13_02228054
	.extern ov13_022283B8
	.extern ov13_022283EC
	.extern ov13_02228400
	.extern ov13_02228414
	.extern ov13_02228428
	.extern ov13_02229D00
	.extern ov13_0222A03C
	.extern ov13_0222A070
	.extern ov13_0222A084
	.extern ov13_0222A098
	.extern ov13_0222A0AC
	.extern ov13_0222A0C0
	.extern ov13_0222B8E8
	.extern ov13_0222B8F8
	.extern ov13_0222B934
	.extern ov13_0222B948
	.extern ov13_0222B970
	.extern ov13_0222BADC
	.extern ov13_0222FA3C
	.extern ov13_0222FF3C
	.extern ov13_02234974
	.extern ov13_022349B8
	.extern ov13_022349DC
	.extern ov13_02234AA4
	.extern ov13_02234B48
	.extern ov13_02234BCC
	.extern ov13_02234CA4
	.extern ov13_02234D54
	.extern ov13_02234D84
	.extern ov13_02235074
	.extern ov13_022350A8
	.extern ov13_022350FC
	.extern ov13_022351C0
	.extern ov13_02235260
	.extern ov13_022352C0
	.extern ov13_022352D0
	.extern ov13_022363F4
	.extern ov13_022368D8
	.extern ov13_02237618
	.extern ov13_02237DD0
	.extern ov13_02238440
	.extern ov13_02239228
	.extern ov13_02239288
	.extern ov13_022392B4
	.extern ov13_022392CC
	.extern ov13_02239378
	.extern ov13_0223943C
	.extern ov13_02239514
	.extern ov13_02239550
	.extern ov13_02239564
	.extern ov13_02239578
	.extern ov13_02239584
	.extern ov13_0223959C
	.extern ov13_02239B34
	.extern ov13_02239FF0
	.extern ov13_0223A004
	.extern ov13_0223A018
	.extern ov13_0223A02C
	.extern ov13_0223A28C
	.extern ov13_0223A414
	.extern ov13_0223AA58
	.extern ov13_0223ABB8
	.extern ov13_0223BE6C
	.extern ov13_0223C198
	.extern ov13_0223C1DC
	.extern ov13_0223C1F0
	.extern ov13_0223F95C
	.extern ov13_0223FA50
	.extern ov13_022400F0
	.extern ov13_02240184
	.extern ov13_02240394
	.extern ov13_022403B8
	.extern ov13_022405B8
	.extern ov13_022408A0
	.extern ov13_02240AA0
	.extern ov13_02240B04
	.extern ov13_02240C14
	.extern ov13_02240CC0
	.extern ov13_02240D80
	.extern ov13_02240E2C
	.extern ov13_02240EA8
	.extern ov13_02240EFC
	.extern ov13_0224128C
	.extern ov13_022412B0
	.extern ov13_02241694
	.extern ov13_022416D4
	.extern ov13_02241714
	.extern ov13_022417DC
	.extern ov13_022418EC
	.extern ov13_02241974
	.extern ov13_02241D98
	.extern ov13_02242074
	.extern ov13_02242528
	.extern ov13_022425D0
	.extern ov13_022425F0
	.extern ov13_022453CC
	.extern ov13_022453FC
	.extern ov13_02245424
	.extern ov13_02245426
	.extern ov13_02245428
	.extern ov13_02245454
	.extern ov13_0224546C
	.extern ov13_02245470
	.extern ov13_02245480
	.extern ov13_02245498
	.extern ov13_022454AC
	.extern ov13_022454C0
	.extern ov13_022454CC
	.extern ov13_022454DC
	.extern ov13_022454DE
	.extern ov13_022454FC
	.extern ov13_02245526
	.extern ov13_02245536
	.extern ov13_02245548
	.extern ov13_0224555D
	.extern ov13_02245573
	.extern ov13_0224558A
	.extern ov13_022455D4
	.extern ov13_022455DC
	.extern ov13_022455F4
	.extern ov13_02245958
	.extern ov13_02246370
	.extern ov13_02246384
	.extern ov13_0224639C
	.extern ov13_022463B4
	.extern ov13_022463C8
	.extern ov13_022463E0
	.extern ov13_022463E4
	.extern ov13_022463F8
	.extern ov13_02246410
	.extern ov13_02246428
	.extern ov13_02246440
	.extern ov13_02246458
	.extern ov13_0224646C
	.extern ov13_02246474
	.extern ov13_02246488
	.extern ov13_022464A0
	.extern ov13_022464B8
	.extern ov13_022464CC
	.extern ov13_022464E4
	.extern ov13_0224F4F8
	.extern ov13_0224F538
	.extern ov13_0224F53C
	.extern ov13_0224F540
	.extern ov13_0224F544
	.extern ov13_0224F548
	.extern ov13_0224F54C
	.extern ov13_0224F554
	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_022316B4

.text
	arm_func_start ov13_022316B4
ov13_022316B4: ; 0x022316B4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x1c
	ldr r0, _0223179C ; =ov13_0224F540
	add r5, sp, #0x14
	mov r1, #0
	strh r1, [r5, #2]
	mov r4, #0x20
	strh r1, [r5, #4]
	mov r3, #0xe
	strh r1, [r5, #6]
	mov r2, #0x10
	ldr r0, [r0]
	strh r1, [r5]
	strh r4, [sp, #0x16]
	strh r3, [sp, #0x18]
	strh r2, [sp, #0x1a]
	ldr r0, [r0]
	bl ov13_02240D80
	mov sl, #0
	ldr r7, _022317A0 ; =0x0000E01D
	ldr r6, _022317A4 ; =ov13_02245470
	mov sb, sl
	strh sl, [sp, #0x12]
	mov r8, #0x31
	mov r5, #2
	mov r4, #0x480
	add fp, sp, #0x10
_02231720:
	ldr r0, _0223179C ; =ov13_0224F540
	cmp sl, #0x10
	ldr r0, [r0]
	moveq sb, #0
	streqh r8, [sp, #0x16]
	add r1, r0, sl
	ldrb r1, [r1, #8]
	ldrb r2, [r6, sb]
	cmp r1, #0x20
	streqh r7, [sp, #0x10]
	strneh r1, [sp, #0x10]
	ldrh r1, [sp, #0x1a]
	strh r2, [sp, #0x14]
	stmia sp, {r1, r5}
	str r4, [sp, #8]
	str fp, [sp, #0xc]
	ldrh r1, [sp, #0x14]
	ldrh r2, [sp, #0x16]
	ldrh r3, [sp, #0x18]
	ldr r0, [r0]
	bl ov13_02240CC0
	add sl, sl, #1
	cmp sl, #0x20
	add sb, sb, #1
	blt _02231720
	ldr r0, _0223179C ; =ov13_0224F540
	ldr r0, [r0]
	ldr r0, [r0]
	bl ov13_02240E2C
	add sp, sp, #0x1c
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0223179C: .word ov13_0224F540
_022317A0: .word 0x0000E01D
_022317A4: .word ov13_02245470
	arm_func_end ov13_022316B4
