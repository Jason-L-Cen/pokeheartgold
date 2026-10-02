	.extern NNS_FndAllocFromAllocator
	.extern NNS_FndFreeToAllocator
	.extern NNS_G3dAnmObjCalcSizeRequired
	.extern NNS_G3dBindMdlSet
	.extern NNS_G3dGetMdlSet
	.extern NNS_G3dGetTex
	.extern NNS_G3dGlb
	.extern NNS_G3dGlbGetInvV
	.extern NNS_G3dGlbGetInvVP
	.extern NNS_G3dGlbGetInvWV
	.extern NNS_G3dGlbGetViewPort
	.extern NNS_G3dGlbGetWV
	.extern NNS_G3dGlbInit
	.extern NNS_G3dPlttGetRequiredSize
	.extern NNS_G3dPlttLoad
	.extern NNS_G3dPlttSetPlttKey
	.extern NNS_G3dTex4x4GetRequiredSize
	.extern NNS_G3dTexGetRequiredSize
	.extern NNS_G3dTexLoad
	.extern NNS_G3dTexSetTexKey
	.extern NNS_GfdDefaultFuncAllocPlttVram
	.extern NNS_GfdDefaultFuncAllocTexVram
	.extern NNS_GfdDefaultFuncFreePlttVram
	.extern NNS_GfdDefaultFuncFreeTexVram
	.extern _02109494
	.extern _02110A8C
	.extern _02110A9C
	.extern _02110AA0
	.extern _02110AA8
	.extern _02110ACC
	.extern _02110B0C
	.extern _02110B10
	.extern _02110B18
	.extern _02110B3C
	.extern _02110B48
	.extern _02110B54
	.extern _02110B58
	.extern _02110B60
	.extern _02110B84
	.extern _02110B90
	.extern _021DBB04
	.extern _021DBB44
	.extern _021DD404
	.extern _021DD408
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_G3dResDefaultSetup
NNS_G3dResDefaultSetup: ; 0x020C26C0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	ldr r2, [sl]
	ldr r1, _020C28DC ; =0x30415642
	cmp r2, r1
	bhi _020C270C
	bhs _020C28CC
	sub r0, r1, #0x900
	cmp r2, r0
	bhi _020C26FC
	bhs _020C28CC
	sub r0, r1, #0x1300
	cmp r2, r0
	beq _020C28CC
	b _020C28D4
_020C26FC:
	sub r0, r1, #0x200
	cmp r2, r0
	beq _020C28CC
	b _020C28D4
_020C270C:
	ldr r0, _020C28E0 ; =0x30505442
	cmp r2, r0
	bhi _020C272C
	bhs _020C28CC
	ldr r0, _020C28E4 ; =0x30444D42
	cmp r2, r0
	beq _020C2738
	b _020C28D4
_020C272C:
	add r0, r0, #0x80000
	cmp r2, r0
	bne _020C28D4
_020C2738:
	mov r5, #1
	mov r0, sl
	mov r6, r5
	mov r7, r5
	bl NNS_G3dGetTex
	movs r4, r0
	beq _020C289C
	bl NNS_G3dTexGetRequiredSize
	mov sb, r0
	mov r0, r4
	bl NNS_G3dTex4x4GetRequiredSize
	mov r8, r0
	mov r0, r4
	bl NNS_G3dPlttGetRequiredSize
	str r0, [sp]
	cmp sb, #0
	beq _020C27A0
	ldr r0, _020C28E8 ; =NNS_GfdDefaultFuncAllocTexVram
	mov r1, #0
	ldr r3, [r0]
	mov r0, sb
	mov r2, r1
	blx r3
	movs fp, r0
	moveq r5, #0
	b _020C27A4
_020C27A0:
	mov fp, #0
_020C27A4:
	cmp r8, #0
	beq _020C27D0
	ldr r1, _020C28E8 ; =NNS_GfdDefaultFuncAllocTexVram
	mov r0, r8
	ldr r3, [r1]
	mov r1, #1
	mov r2, #0
	blx r3
	movs r8, r0
	moveq r6, #0
	b _020C27D4
_020C27D0:
	mov r8, #0
_020C27D4:
	ldr r0, [sp]
	cmp r0, #0
	beq _020C2804
	ldr r1, _020C28EC ; =NNS_GfdDefaultFuncAllocPlttVram
	ldrh r2, [r4, #0x20]
	ldr r3, [r1]
	and r1, r2, #0x8000
	mov r2, #0
	blx r3
	movs sb, r0
	moveq r7, #0
	b _020C2808
_020C2804:
	mov sb, #0
_020C2808:
	cmp r5, #0
	cmpne r6, #0
	cmpne r7, #0
	bne _020C2868
	cmp r7, #0
	beq _020C2830
	ldr r1, _020C28F0 ; =NNS_GfdDefaultFuncFreePlttVram
	mov r0, sb
	ldr r1, [r1]
	blx r1
_020C2830:
	cmp r6, #0
	beq _020C2848
	ldr r1, _020C28F4 ; =NNS_GfdDefaultFuncFreeTexVram
	mov r0, r8
	ldr r1, [r1]
	blx r1
_020C2848:
	cmp r5, #0
	beq _020C2860
	ldr r1, _020C28F4 ; =NNS_GfdDefaultFuncFreeTexVram
	mov r0, fp
	ldr r1, [r1]
	blx r1
_020C2860:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C2868:
	mov r0, r4
	mov r1, fp
	mov r2, r8
	bl NNS_G3dTexSetTexKey
	mov r0, r4
	mov r1, sb
	bl NNS_G3dPlttSetPlttKey
	mov r0, r4
	mov r1, #1
	bl NNS_G3dTexLoad
	mov r0, r4
	mov r1, #1
	bl NNS_G3dPlttLoad
_020C289C:
	ldr r1, [sl]
	ldr r0, _020C28E4 ; =0x30444D42
	cmp r1, r0
	bne _020C28C4
	mov r0, sl
	bl NNS_G3dGetMdlSet
	cmp r4, #0
	beq _020C28C4
	mov r1, r4
	bl NNS_G3dBindMdlSet
_020C28C4:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C28CC:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C28D4:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C28DC: .word 0x30415642
_020C28E0: .word 0x30505442
_020C28E4: .word 0x30444D42
_020C28E8: .word NNS_GfdDefaultFuncAllocTexVram
_020C28EC: .word NNS_GfdDefaultFuncAllocPlttVram
_020C28F0: .word NNS_GfdDefaultFuncFreePlttVram
_020C28F4: .word NNS_GfdDefaultFuncFreeTexVram
	arm_func_end NNS_G3dResDefaultSetup

	arm_func_start NNS_G3dScrPosToWorldLine
NNS_G3dScrPosToWorldLine: ; 0x020C28F8
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x24
	mov r7, r0
	mov r6, r1
	mov fp, r2
	mov r4, r3
	add r0, sp, #0x20
	add r1, sp, #0x1c
	add r2, sp, #0x18
	add r3, sp, #0x14
	bl NNS_G3dGlbGetViewPort
	ldr r1, [sp, #0x14]
	ldr r0, [sp, #0x1c]
	ldr r2, [sp, #0x20]
	sub r5, r1, r0
	ldr r1, [sp, #0x18]
	sub r0, r7, r2
	sub r1, r1, r2
	mov r0, r0, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
	rsb r1, r5, #0
	mov r5, r0
	ldr r0, [sp, #0x1c]
	mov r1, r1, lsl #0xc
	add r0, r6, r0
	sub r0, r0, #0xbf
	mov r0, r0, lsl #0xc
	bl FX_Div
	cmp r5, #0
	cmpge r0, #0
	blt _020C2984
	cmp r5, #0x1000
	cmple r0, #0x1000
	ble _020C2990
_020C2984:
	mvn r1, #0
	str r1, [sp, #0x10]
	b _020C2998
_020C2990:
	mov r1, #0
	str r1, [sp, #0x10]
_020C2998:
	sub r1, r5, #0x800
	sub r0, r0, #0x800
	mov r8, r1, lsl #1
	mov sl, r0, lsl #1
	bl NNS_G3dGlbGetInvVP
	mov r5, r0
	ldr r0, [r5, #0x1c]
	ldr r1, [r5, #0xc]
	smull r2, r0, sl, r0
	smlal r2, r0, r8, r1
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	ldr r0, [r5, #0x3c]
	ldr r1, [r5, #0x2c]
	add r0, r0, r2
	str r0, [sp, #0xc]
	sub r0, r0, r1
	bl FX_InvAsync
	ldr r0, [r5, #0x10]
	ldr r2, [r5]
	smull r3, r0, sl, r0
	smlal r3, r0, r8, r2
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	ldr r6, [r5, #0x30]
	ldr r3, [r5, #4]
	add r6, r6, r2
	ldr r2, [r5, #0x14]
	ldr r1, [r5, #0x34]
	smull r7, r2, sl, r2
	smlal r7, r2, r8, r3
	mov r3, r7, lsr #0xc
	orr r3, r3, r2, lsl #20
	add r7, r1, r3
	ldr r1, [r5, #0x18]
	ldr r2, [r5, #8]
	smull r3, r1, sl, r1
	smlal r3, r1, r8, r2
	mov r2, r3, lsr #0xc
	ldr r0, [r5, #0x38]
	orr r2, r2, r1, lsl #20
	cmp r4, #0
	add r8, r0, r2
	beq _020C2A78
	ldr r0, [r5, #0x20]
	ldr r3, [r5, #0x24]
	add r0, r6, r0
	str r0, [sp, #4]
	add r0, r7, r3
	str r0, [sp]
	ldr r2, [r5, #0x28]
	ldr r1, [r5, #0x2c]
	ldr r0, [sp, #0xc]
	add sb, r8, r2
	add r0, r0, r1
	str r0, [sp, #8]
_020C2A78:
	ldr r2, [r5, #0x20]
	ldr r1, [r5, #0x24]
	ldr r0, [r5, #0x28]
	sub r6, r6, r2
	sub r7, r7, r1
	sub r8, r8, r0
	bl FX_GetDivResultFx64c
	mov r5, r0
	mov sl, r1
	cmp r4, #0
	beq _020C2AAC
	ldr r0, [sp, #8]
	bl FX_InvAsync
_020C2AAC:
	mov r0, r6, asr #0x1f
	umull r2, r1, r5, r6
	adds r2, r2, #0x80000000
	mla r1, r5, r0, r1
	mla r1, sl, r6, r1
	adc r2, r1, #0
	umull r0, r1, r5, r7
	str r2, [fp]
	adds r0, r0, #0x80000000
	mov r0, r7, asr #0x1f
	mla r1, r5, r0, r1
	mla r1, sl, r7, r1
	adc r0, r1, #0
	str r0, [fp, #4]
	umull r0, r1, r5, r8
	adds r0, r0, #0x80000000
	mov r0, r8, asr #0x1f
	mla r1, r5, r0, r1
	mla r1, sl, r8, r1
	adc r0, r1, #0
	str r0, [fp, #8]
	cmp r4, #0
	beq _020C2B70
	bl FX_GetDivResultFx64c
	ldr r2, [sp, #4]
	umull r6, r5, r0, r2
	mov r2, r2, asr #0x1f
	mla r5, r0, r2, r5
	ldr r2, [sp]
	adds r6, r6, #0x80000000
	mov r7, r2, asr #0x1f
	umull r3, r2, r0, r2
	mla r2, r0, r7, r2
	mov r7, sb, asr #0x1f
	umull r8, r6, r0, sb
	mla r6, r0, r7, r6
	ldr r0, [sp, #4]
	mla r6, r1, sb, r6
	mla r5, r1, r0, r5
	adc r0, r5, #0
	str r0, [r4]
	ldr r0, [sp]
	mla r2, r1, r0, r2
	adds r0, r3, #0x80000000
	adc r2, r2, #0
	adds r0, r8, #0x80000000
	str r2, [r4, #4]
	adc r0, r6, #0
	str r0, [r4, #8]
_020C2B70:
	ldr r0, [sp, #0x10]
	add sp, sp, #0x24
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G3dScrPosToWorldLine
