	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public DrawCellAnimationImpl_
	.public DrawNode_
	.public NNSi_G2dIdentity
	.public RndCoreCBFuncAfterCell_
	.public RndCoreCBFuncAfterOBJ_
	.public RndCoreCBFuncBeforeCell_
	.public RndCoreCBFuncBeforeOBJ_
	.extern NNS_G2dGetAnimSequenceByIdx
	.extern NNS_G2dGetPaletteTableValue
	.extern NNS_G2dGetRendererPaletteTbl
	.extern NNS_G2dInitAnimCtrl
	.extern NNS_G2dInitCellAnimation
	.extern NNS_G2dIsRndCoreFlipH
	.extern NNS_G2dIsRndCoreFlipV
	.extern NNS_G2dPopMtx
	.extern NNS_G2dPushMtx
	.extern NNS_G2dRndCoreBeginRendering
	.extern NNS_G2dRndCoreDrawCell
	.extern NNS_G2dRndCoreDrawCellVramTransfer
	.extern NNS_G2dRndCoreEndRendering
	.extern NNS_G2dRotZ
	.extern NNS_G2dScale
	.extern NNS_G2dSetRndCore3DSoftSpriteZvalue
	.extern NNS_G2dSetRndCoreCurrentMtx2D
	.extern NNS_G2dSetRndCoreCurrentMtx3D
	.extern NNS_G2dSetRndCoreOamRegisterFunc
	.extern NNS_G2dSetRndCoreSurface
	.extern NNS_G2dTranslate
	.extern NNSi_G2dInitializeNode
	.extern NNSi_G2dSrtcInitControl
	.extern _021109DC
	.extern _021109E4
	.extern _021D94D8
	.extern _021D94E8
	.extern _021D9568
	.extern _021D9668
	.extern _021D9678
	.extern _021D967C
	.extern _021D9968
	.extern _021D9C68
	.extern _021D9FD8
	.extern _021DA068
	.extern _021DA06C

.text
	arm_func_start NNS_G2dInitMCAnimation
NNS_G2dInitMCAnimation: ; 0x020B91D8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r5, #0
	str r5, [r0, #0x3c]
	ldr r7, [sp, #0x28]
	str r5, [r0, #0x34]
	mov sl, r1
	str r7, [r0, #0x38]
	str r0, [sp]
	movs r8, r3
	mov sb, r2
	str sl, [r0, #0x40]
	ldr fp, [sp, #0x2c]
	beq _020B925C
_020B920C:
	mov r0, #0x28
	mul r4, r5, r0
	mov r1, #1
	add r0, sl, r4
	bl NNSi_G2dInitializeNode
	mov r0, #0x58
	mla r6, r5, r0, sb
	mov r0, r7
	mov r1, #0
	str r6, [sl, r4]
	bl NNS_G2dGetAnimSequenceByIdx
	mov r1, r0
	mov r0, r6
	mov r2, fp
	bl NNS_G2dInitCellAnimation
	add r0, r5, #1
	mov r0, r0, lsl #0x10
	cmp r8, r0, lsr #16
	mov r5, r0, lsr #0x10
	bhi _020B920C
_020B925C:
	ldr r0, [sp]
	bl NNS_G2dInitAnimCtrl
	ldr r0, [sp]
	ldr r3, [sp, #0x30]
	ldr r2, [sp]
	add r0, r0, #0x48
	mov r1, #1
	str r3, [r2, #0x44]
	bl NNSi_G2dSrtcInitControl
	ldr r0, [sp]
	mov r1, #0
	strh r1, [r0, #0x30]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G2dInitMCAnimation

	arm_func_start NNSi_G2dIdentity
NNSi_G2dIdentity: ; 0x020B9290
	stmdb sp!, {r4, lr}
	ldr r0, _020B9318 ; =_021D94D8
	ldr ip, [r0, #0xc]
	ldr r1, [r0, #4]
	mov r0, #0x18
	mul r3, ip, r0
	cmp r1, #0
	beq _020B92C8
	ldr r1, _020B931C ; =_021D9678
	mov r2, #0
	ldr r0, _020B9320 ; =_021D967C
	str r2, [r1, r3]
	str r2, [r0, r3]
	b _020B9308
_020B92C8:
	ldr r1, _020B9324 ; =_021D9668
	mov r2, #0x1000
	str r2, [r1, r3]
	add r4, r1, r3
	mov r1, #0
	str r1, [r4, #4]
	ldr r0, _020B9328 ; =_021D9968
	str r1, [r4, #8]
	str r2, [r4, #0xc]
	str r1, [r4, #0x10]
	add lr, r0, r3
	str r1, [r4, #0x14]
	ldmia r4!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	ldmia r4, {r0, r1}
	stmia lr, {r0, r1}
_020B9308:
	ldr r0, _020B932C ; =_021D94E8
	mov r1, #0
	str r1, [r0, ip, lsl #2]
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B9318: .word _021D94D8
_020B931C: .word _021D9678
_020B9320: .word _021D967C
_020B9324: .word _021D9668
_020B9328: .word _021D9968
_020B932C: .word _021D94E8
	arm_func_end NNSi_G2dIdentity

	arm_func_start NNSi_G2dMCMStoreCurrentMtxToMtxCache
NNSi_G2dMCMStoreCurrentMtxToMtxCache: ; 0x020B9330
	stmdb sp!, {r3, r4, r5, lr}
	ldr r0, _020B9454 ; =_021D94D8
	ldr r1, _020B9458 ; =_021D9568
	ldr r0, [r0, #0xc]
	mov r2, #1
	mov r0, r0, lsl #0x10
	add r3, r1, r0, lsr #13
	ldrh r0, [r3, #4]
	cmp r0, #1
	cmpne r0, #2
	movne r2, #0
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r0, _020B9454 ; =_021D94D8
	ldrh r4, [r3, #2]
	ldrh r5, [r0]
	mov r2, #0x10
	cmp r5, #0x1f
	addlo r1, r5, #1
	strloh r1, [r0]
	ldr r0, _020B945C ; =_021D9C68
	strh r5, [r3]
	add r0, r0, r5, lsl #5
	add r1, r0, #0x10
	mvn r0, #0
	bl MIi_CpuClearFast
	ldr r0, _020B9454 ; =_021D94D8
	ldr r1, [r0, #4]
	ldr ip, [r0, #0xc]
	cmp r1, #0
	ldreq r1, _020B9460 ; =_021D9968
	mov r0, #0x18
	mlaeq r1, ip, r0, r1
	beq _020B93DC
	mul r3, ip, r0
	ldr r1, _020B9464 ; =_021D9678
	ldr r0, _020B9468 ; =_021109DC
	ldr r2, [r1, r3]
	ldr r1, _020B946C ; =_021D967C
	str r2, [r0, #0x18]
	ldr r2, [r1, r3]
	ldr r1, _020B9470 ; =_021109E4
	str r2, [r0, #0x1c]
_020B93DC:
	mov r2, ip, lsl #0x10
	ldr r0, _020B9458 ; =_021D9568
	mov r2, r2, lsr #0xd
	ldrh ip, [r0, r2]
	ldr lr, _020B945C ; =_021D9C68
	ldr r3, [r1]
	ldr r2, _020B9454 ; =_021D94D8
	str r3, [lr, ip, lsl #5]
	ldr r3, [r1, #4]
	add ip, lr, ip, lsl #5
	str r3, [ip, #4]
	ldr r3, [r1, #8]
	ldr r2, [r2, #0xc]
	str r3, [ip, #8]
	ldr r3, [r1, #0xc]
	mov r1, r2, lsl #0x10
	str r3, [ip, #0xc]
	movs lr, r1, lsr #0x10
	ldmmiia sp!, {r3, r4, r5, pc}
	mov r1, #3
_020B942C:
	add r3, r0, lr, lsl #3
	ldrh r2, [r3, #2]
	mov ip, lr, lsl #3
	cmp r4, r2
	ldmneia sp!, {r3, r4, r5, pc}
	strh r1, [r3, #4]
	strh r5, [r0, ip]
	subs lr, lr, #1
	bpl _020B942C
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020B9454: .word _021D94D8
_020B9458: .word _021D9568
_020B945C: .word _021D9C68
_020B9460: .word _021D9968
_020B9464: .word _021D9678
_020B9468: .word _021109DC
_020B946C: .word _021D967C
_020B9470: .word _021109E4
	arm_func_end NNSi_G2dMCMStoreCurrentMtxToMtxCache

	arm_func_start RndCoreCBFuncBeforeCell_
RndCoreCBFuncBeforeCell_: ; 0x020B9474
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020B9570 ; =_021D94D8
	mov r6, r0
	ldr r0, [r2, #8]
	mov r5, r1
	ldr r4, [r0, #0x74]
	ldr r0, [r4, #0x34]
	cmp r0, #0
	beq _020B9504
	ldr r0, [r2, #4]
	cmp r0, #0
	ldreq r2, [r2, #0xc]
	ldreq r1, _020B9574 ; =_021D9668
	mov r0, #0x18
	mlaeq r1, r2, r0, r1
	beq _020B94DC
	ldr r1, [r2, #0xc]
	mul r3, r1, r0
	ldr r1, _020B9578 ; =_021D9678
	ldr r0, _020B957C ; =_021109DC
	ldr r2, [r1, r3]
	ldr r1, _020B9580 ; =_021D967C
	str r2, [r0, #0x18]
	ldr r2, [r1, r3]
	ldr r1, _020B9584 ; =_021109E4
	str r2, [r0, #0x1c]
_020B94DC:
	ldr r3, [r4, #0x34]
	mov r0, r5
	mov r2, r4
	blx r3
	cmp r0, #0
	moveq r0, #0
	streq r0, [r6, #0x30]
	ldmeqia sp!, {r4, r5, r6, pc}
	mov r0, #1
	str r0, [r6, #0x30]
_020B9504:
	ldr r0, [r4, #0x38]
	cmp r0, #0
	ldmeqia sp!, {r4, r5, r6, pc}
	ldr r0, _020B9570 ; =_021D94D8
	ldr r1, [r0, #4]
	cmp r1, #0
	ldr r1, [r0, #0xc]
	ldreq r2, _020B9574 ; =_021D9668
	mov r0, #0x18
	mlaeq r3, r1, r0, r2
	beq _020B9554
	mul r3, r1, r0
	ldr r1, _020B9578 ; =_021D9678
	ldr r0, _020B957C ; =_021109DC
	ldr r2, [r1, r3]
	ldr r1, _020B9580 ; =_021D967C
	str r2, [r0, #0x18]
	ldr r1, [r1, r3]
	ldr r3, _020B9584 ; =_021109E4
	str r1, [r0, #0x1c]
_020B9554:
	ldr r0, _020B9570 ; =_021D94D8
	ldr r6, [r4, #0x38]
	ldr r0, [r0, #8]
	mov r1, r4
	mov r2, r5
	blx r6
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020B9570: .word _021D94D8
_020B9574: .word _021D9668
_020B9578: .word _021D9678
_020B957C: .word _021109DC
_020B9580: .word _021D967C
_020B9584: .word _021109E4
	arm_func_end RndCoreCBFuncBeforeCell_

	arm_func_start RndCoreCBFuncAfterCell_
RndCoreCBFuncAfterCell_: ; 0x020B9588
	stmdb sp!, {r4, lr}
	ldr r3, _020B95F4 ; =_021D94D8
	mov r2, r1
	ldr r0, [r3, #8]
	ldr r1, [r0, #0x74]
	ldr r4, [r1, #0x3c]
	cmp r4, #0
	ldmeqia sp!, {r4, pc}
	ldr r4, [r3, #4]
	ldr ip, [r3, #0xc]
	cmp r4, #0
	ldreq lr, _020B95F8 ; =_021D9668
	mov r3, #0x18
	mlaeq r3, ip, r3, lr
	beq _020B95E8
	mul r4, ip, r3
	ldr r3, _020B95FC ; =_021D9678
	ldr ip, _020B9600 ; =_021109DC
	ldr lr, [r3, r4]
	ldr r3, _020B9604 ; =_021D967C
	str lr, [ip, #0x18]
	ldr lr, [r3, r4]
	ldr r3, _020B9608 ; =_021109E4
	str lr, [ip, #0x1c]
_020B95E8:
	ldr ip, [r1, #0x3c]
	blx ip
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B95F4: .word _021D94D8
_020B95F8: .word _021D9668
_020B95FC: .word _021D9678
_020B9600: .word _021109DC
_020B9604: .word _021D967C
_020B9608: .word _021109E4
	arm_func_end RndCoreCBFuncAfterCell_

	arm_func_start RndCoreCBFuncBeforeOBJ_
RndCoreCBFuncBeforeOBJ_: ; 0x020B960C
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #4
	ldr r3, _020B97E0 ; =_021D94D8
	mov r6, r0
	ldr r0, [r3, #8]
	mov r5, r1
	mov r4, r2
	bl NNS_G2dGetRendererPaletteTbl
	cmp r0, #0
	beq _020B965C
	ldrh r1, [r6, #0x3c]
	and r1, r1, #0xf000
	mov r1, r1, lsl #4
	mov r1, r1, lsr #0x10
	bl NNS_G2dGetPaletteTableValue
	ldr r1, [r6, #0x3c]
	mov r0, r0, lsl #0x1c
	bic r1, r1, #0xf000
	orr r0, r1, r0, lsr #16
	str r0, [r6, #0x3c]
_020B965C:
	ldr r0, _020B97E0 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r0, [r1, #0x84]
	cmp r0, #0
	beq _020B9768
	ldr r0, [r1, #0x84]
	tst r0, #1
	beq _020B9694
	ldrh r0, [r1, #0x88]
	ldr r1, [r6, #0x3c]
	bic r1, r1, #0xc00
	mov r0, r0, lsl #0x1e
	orr r0, r1, r0, lsr #20
	str r0, [r6, #0x3c]
_020B9694:
	ldr r0, _020B97E0 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r0, [r1, #0x84]
	tst r0, #2
	beq _020B96C0
	ldrh r0, [r1, #0x8a]
	ldr r1, [r6, #0x3c]
	bic r1, r1, #0xf000
	mov r0, r0, lsl #0x1c
	orr r0, r1, r0, lsr #16
	str r0, [r6, #0x3c]
_020B96C0:
	ldr r0, _020B97E0 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r0, [r1, #0x84]
	tst r0, #0x10
	beq _020B96F4
	ldr r2, [r6, #0x3c]
	ldrh r0, [r1, #0x94]
	mov r1, r2, lsl #0x10
	bic r2, r2, #0xf000
	add r0, r0, r1, lsr #28
	mov r0, r0, lsl #0x1c
	orr r0, r2, r0, lsr #16
	str r0, [r6, #0x3c]
_020B96F4:
	ldr r0, _020B97E0 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r0, [r1, #0x84]
	tst r0, #4
	beq _020B9724
	ldr r0, [r1, #0x90]
	cmp r0, #0
	ldr r0, [r6, #0x38]
	orrne r0, r0, #0x1000
	strne r0, [r6, #0x38]
	biceq r0, r0, #0x1000
	streq r0, [r6, #0x38]
_020B9724:
	ldr r0, _020B97E0 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r0, [r1, #0x84]
	tst r0, #8
	beq _020B9768
	ldr r0, [r6, #0x38]
	ldrh r2, [r6, #0x3c]
	ldr r1, [r1, #0x8c]
	bic r0, r0, #0xc00
	orr r0, r0, r1, lsl #10
	str r0, [r6, #0x38]
	ldrh r0, [r6, #0x3c]
	and r1, r2, #0xf000
	mov r1, r1, asr #0xc
	bic r0, r0, #0xf000
	orr r0, r0, r1, lsl #12
	strh r0, [r6, #0x3c]
_020B9768:
	ldr r2, _020B97E0 ; =_021D94D8
	ldr r0, [r2, #8]
	ldr r1, [r0, #0x74]
	ldr r3, [r1, #0x40]
	cmp r3, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, r5, r6, pc}
	ldr r3, [r2, #4]
	cmp r3, #0
	ldr r3, [r2, #0xc]
	ldreq r6, _020B97E4 ; =_021D9668
	mov r2, #0x18
	mlaeq r6, r3, r2, r6
	beq _020B97C4
	mul ip, r3, r2
	ldr r3, _020B97E8 ; =_021D9678
	ldr r2, _020B97EC ; =_021109DC
	ldr r6, [r3, ip]
	ldr r3, _020B97F0 ; =_021D967C
	str r6, [r2, #0x18]
	ldr r3, [r3, ip]
	ldr r6, _020B97F4 ; =_021109E4
	str r3, [r2, #0x1c]
_020B97C4:
	str r6, [sp]
	ldr r6, [r1, #0x40]
	mov r2, r5
	mov r3, r4
	blx r6
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, pc}
	.align 2, 0
_020B97E0: .word _021D94D8
_020B97E4: .word _021D9668
_020B97E8: .word _021D9678
_020B97EC: .word _021109DC
_020B97F0: .word _021D967C
_020B97F4: .word _021109E4
	arm_func_end RndCoreCBFuncBeforeOBJ_

	arm_func_start RndCoreCBFuncAfterOBJ_
RndCoreCBFuncAfterOBJ_: ; 0x020B97F8
	stmdb sp!, {r3, r4, r5, lr}
	ldr lr, _020B9874 ; =_021D94D8
	mov ip, r1
	ldr r0, [lr, #8]
	mov r3, r2
	ldr r1, [r0, #0x74]
	ldr r2, [r1, #0x44]
	cmp r2, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldr r2, [lr, #4]
	cmp r2, #0
	ldreq lr, [lr, #0xc]
	ldreq r4, _020B9878 ; =_021D9668
	mov r2, #0x18
	mlaeq r5, lr, r2, r4
	beq _020B9860
	ldr r4, [lr, #0xc]
	mul r5, r4, r2
	ldr r4, _020B987C ; =_021D9678
	ldr r2, _020B9880 ; =_021109DC
	ldr lr, [r4, r5]
	ldr r4, _020B9884 ; =_021D967C
	str lr, [r2, #0x18]
	ldr r4, [r4, r5]
	ldr r5, _020B9888 ; =_021109E4
	str r4, [r2, #0x1c]
_020B9860:
	str r5, [sp]
	ldr lr, [r1, #0x44]
	mov r2, ip
	blx lr
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020B9874: .word _021D94D8
_020B9878: .word _021D9668
_020B987C: .word _021D9678
_020B9880: .word _021109DC
_020B9884: .word _021D967C
_020B9888: .word _021109E4
	arm_func_end RndCoreCBFuncAfterOBJ_

	arm_func_start DrawCellImpl_
DrawCellImpl_: ; 0x020B988C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	ldr r7, _020B9CA8 ; =_021D94D8
	mov r4, r0
	ldr fp, [r7, #8]
	ldr r0, [fp, #0x7c]
	ldr sb, [fp, #0x70]
	tst r0, #2
	beq _020B9A7C
	ldr r0, [sb, #0x14]
	cmp r0, #0
	beq _020B99EC
	ldr r0, [r7, #4]
	mov r1, #0
	cmp r0, #0
	bne _020B98E0
	ldr r2, _020B9CAC ; =_021109DC
	ldr r0, _020B9CB0 ; =0x0000FFFE
	ldrh r2, [r2]
	cmp r2, r0
	movne r0, #1
	bne _020B98E4
_020B98E0:
	mov r0, #0
_020B98E4:
	cmp r0, #0
	beq _020B9970
	ldr r0, _020B9CA8 ; =_021D94D8
	ldr r0, [r0, #0xf94]
	cmp r0, #0
	beq _020B994C
	ldr r0, _020B9CB4 ; =_021D9FD8
	ldr r1, _020B9CB8 ; =_021DA06C
	ldrh r0, [r0, #0x90]
	ldr r1, [r1, r0, lsl #2]
	cmp r1, #0
	bne _020B9970
	bl NNSi_G2dMCMStoreCurrentMtxToMtxCache
	ldr r0, _020B9CA8 ; =_021D94D8
	ldr r2, _020B9CBC ; =_021D9568
	ldr r0, [r0, #0xc]
	ldr r1, _020B9CB4 ; =_021D9FD8
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0xd
	ldrh r3, [r2, r0]
	ldr r5, _020B9CC0 ; =_021D9C68
	ldrh r2, [r1, #0x90]
	ldr r0, _020B9CB8 ; =_021DA06C
	add r1, r5, r3, lsl #5
	str r1, [r0, r2, lsl #2]
	b _020B9970
_020B994C:
	bl NNSi_G2dMCMStoreCurrentMtxToMtxCache
	ldr r0, _020B9CA8 ; =_021D94D8
	ldr r1, _020B9CBC ; =_021D9568
	ldr r0, [r0, #0xc]
	ldr r2, _020B9CC0 ; =_021D9C68
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0xd
	ldrh r0, [r1, r0]
	add r1, r2, r0, lsl #5
_020B9970:
	ldr r0, _020B9CA8 ; =_021D94D8
	ldr r2, [r0, #4]
	cmp r2, #0
	ldr r2, [r0, #0xc]
	ldreq r3, _020B9CC4 ; =_021D9668
	mov r0, #0x18
	mlaeq r0, r2, r0, r3
	beq _020B99B4
	mul r5, r2, r0
	ldr r0, _020B9CC8 ; =_021D9678
	ldr r2, _020B9CAC ; =_021109DC
	ldr r3, [r0, r5]
	ldr r0, _020B9CCC ; =_021D967C
	str r3, [r2, #0x18]
	ldr r3, [r0, r5]
	ldr r0, _020B9CD0 ; =_021109E4
	str r3, [r2, #0x1c]
_020B99B4:
	bl NNS_G2dSetRndCoreCurrentMtx2D
	ldr r1, _020B9CAC ; =_021109DC
	mvn r0, #0
	ldr r1, [r1, #4]
	cmp r1, r0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	mov r0, r4
	beq _020B99E4
	bl NNS_G2dRndCoreDrawCellVramTransfer
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B99E4:
	bl NNS_G2dRndCoreDrawCell
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B99EC:
	ldr r2, [r7, #0xc]
	ldr r1, _020B9CD4 ; =_021D94E8
	mov r0, fp
	ldr r1, [r1, r2, lsl #2]
	bl NNS_G2dSetRndCore3DSoftSpriteZvalue
	mov r0, r7
	ldr r1, [r0, #4]
	cmp r1, #0
	ldr r1, [r0, #0xc]
	ldreq r2, _020B9CC4 ; =_021D9668
	mov r0, #0x18
	mlaeq r0, r1, r0, r2
	beq _020B9A44
	mul r3, r1, r0
	ldr r0, _020B9CC8 ; =_021D9678
	ldr r1, _020B9CAC ; =_021109DC
	ldr r2, [r0, r3]
	ldr r0, _020B9CCC ; =_021D967C
	str r2, [r1, #0x18]
	ldr r2, [r0, r3]
	ldr r0, _020B9CD0 ; =_021109E4
	str r2, [r1, #0x1c]
_020B9A44:
	bl NNS_G2dSetRndCoreCurrentMtx3D
	ldr r1, _020B9CAC ; =_021109DC
	mvn r0, #0
	ldr r1, [r1, #4]
	cmp r1, r0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	mov r0, r4
	beq _020B9A74
	bl NNS_G2dRndCoreDrawCellVramTransfer
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B9A74:
	bl NNS_G2dRndCoreDrawCell
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020B9A7C:
	cmp sb, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r8, _020B9CC4 ; =_021D9668
	ldr r6, _020B9CAC ; =_021109DC
	mvn r5, #0
_020B9A90:
	ldr r0, [sb, #0x10]
	cmp r0, #0
	beq _020B9C98
	ldr r0, [sb, #0x14]
	ldr sl, [r7, #8]
	cmp r0, #0
	mov r1, sb
	beq _020B9BFC
	str sb, [sl, #0x74]
	mov r0, sl
	bl NNS_G2dSetRndCoreSurface
	ldr r1, [sb, #0x28]
	ldr r2, [sb, #0x2c]
	mov r0, sl
	bl NNS_G2dSetRndCoreOamRegisterFunc
	mov r0, sl
	bl NNS_G2dRndCoreBeginRendering
	ldr r0, [r7, #4]
	mov r1, #0
	cmp r0, #0
	bne _020B9AF8
	ldrh r2, [r6]
	ldr r0, _020B9CB0 ; =0x0000FFFE
	cmp r2, r0
	movne r0, #1
	bne _020B9AFC
_020B9AF8:
	mov r0, #0
_020B9AFC:
	cmp r0, #0
	beq _020B9B84
	ldr r0, [r7, #0xf94]
	cmp r0, #0
	beq _020B9B64
	ldr r0, _020B9CB4 ; =_021D9FD8
	ldrh r1, [r0, #0x90]
	ldr r0, _020B9CD8 ; =_021DA068
	add r0, r0, r1, lsl #2
	ldr r1, [r0, #4]
	cmp r1, #0
	bne _020B9B84
	bl NNSi_G2dMCMStoreCurrentMtxToMtxCache
	ldr r0, _020B9CB4 ; =_021D9FD8
	ldr r2, [r7, #0xc]
	ldrh r1, [r0, #0x90]
	mov r0, r2, lsl #0x10
	mov r2, r0, lsr #0xd
	ldr r0, _020B9CBC ; =_021D9568
	ldrh r3, [r0, r2]
	ldr r0, _020B9CD8 ; =_021DA068
	add r2, r0, r1, lsl #2
	ldr r0, _020B9CC0 ; =_021D9C68
	add r1, r0, r3, lsl #5
	str r1, [r2, #4]
	b _020B9B84
_020B9B64:
	bl NNSi_G2dMCMStoreCurrentMtxToMtxCache
	ldr r0, [r7, #0xc]
	mov r0, r0, lsl #0x10
	mov r1, r0, lsr #0xd
	ldr r0, _020B9CBC ; =_021D9568
	ldrh r1, [r0, r1]
	ldr r0, _020B9CC0 ; =_021D9C68
	add r1, r0, r1, lsl #5
_020B9B84:
	ldr r0, [r7, #4]
	cmp r0, #0
	ldreq r2, [r7, #0xc]
	moveq r0, #0x18
	mlaeq r0, r2, r0, r8
	beq _020B9BBC
	ldr r3, [r7, #0xc]
	mov r2, #0x18
	mla sl, r3, r2, r8
	ldr r2, [sl, #0x10]
	ldr r0, _020B9CD0 ; =_021109E4
	str r2, [r6, #0x18]
	ldr r2, [sl, #0x14]
	str r2, [r6, #0x1c]
_020B9BBC:
	bl NNS_G2dSetRndCoreCurrentMtx2D
	ldr r1, [r6, #4]
	cmp r1, r5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	mov r0, r4
	beq _020B9BE4
	bl NNS_G2dRndCoreDrawCellVramTransfer
	b _020B9BE8
_020B9BE4:
	bl NNS_G2dRndCoreDrawCell
_020B9BE8:
	ldr r1, [r7, #8]
	mov r0, #0
	str r0, [r1, #0x74]
	bl NNS_G2dRndCoreEndRendering
	b _020B9C98
_020B9BFC:
	mov r0, sl
	str sb, [sl, #0x74]
	bl NNS_G2dSetRndCoreSurface
	mov r0, sl
	bl NNS_G2dRndCoreBeginRendering
	ldr r2, [r7, #0xc]
	ldr r1, _020B9CD4 ; =_021D94E8
	mov r0, fp
	ldr r1, [r1, r2, lsl #2]
	bl NNS_G2dSetRndCore3DSoftSpriteZvalue
	ldr r0, [r7, #4]
	cmp r0, #0
	ldreq r1, [r7, #0xc]
	moveq r0, #0x18
	mlaeq r0, r1, r0, r8
	beq _020B9C5C
	ldr r2, [r7, #0xc]
	mov r1, #0x18
	mla r3, r2, r1, r8
	ldr r1, [r3, #0x10]
	ldr r0, _020B9CD0 ; =_021109E4
	str r1, [r6, #0x18]
	ldr r1, [r3, #0x14]
	str r1, [r6, #0x1c]
_020B9C5C:
	bl NNS_G2dSetRndCoreCurrentMtx3D
	ldr r1, [r6, #4]
	cmp r1, r5
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	mov r0, r4
	beq _020B9C84
	bl NNS_G2dRndCoreDrawCellVramTransfer
	b _020B9C88
_020B9C84:
	bl NNS_G2dRndCoreDrawCell
_020B9C88:
	ldr r1, [r7, #8]
	mov r0, #0
	str r0, [r1, #0x74]
	bl NNS_G2dRndCoreEndRendering
_020B9C98:
	ldr sb, [sb, #0x30]
	cmp sb, #0
	bne _020B9A90
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020B9CA8: .word _021D94D8
_020B9CAC: .word _021109DC
_020B9CB0: .word 0x0000FFFE
_020B9CB4: .word _021D9FD8
_020B9CB8: .word _021DA06C
_020B9CBC: .word _021D9568
_020B9CC0: .word _021D9C68
_020B9CC4: .word _021D9668
_020B9CC8: .word _021D9678
_020B9CCC: .word _021D967C
_020B9CD0: .word _021109E4
_020B9CD4: .word _021D94E8
_020B9CD8: .word _021DA068
	arm_func_end DrawCellImpl_

	arm_func_start DrawCellAnimationImpl_
DrawCellAnimationImpl_: ; 0x020B9CDC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r0
	ldrh r0, [r5, #0x4e]
	ldr r4, [r5, #0x30]
	cmp r0, #0
	bne _020B9D3C
	ldr r2, [r5, #0x38]
	mvn r0, #0
	cmp r2, r0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _020B9D30
	ldr r1, _020B9E2C ; =_021109DC
	mov r0, r4
	str r2, [r1, #4]
	bl DrawCellImpl_
	ldr r0, _020B9E2C ; =_021109DC
	mvn r1, #0
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B9D30:
	mov r0, r4
	bl DrawCellImpl_
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020B9D3C:
	bl NNS_G2dPushMtx
	ldrh r0, [r5, #0x4e]
	tst r0, #8
	beq _020B9D8C
	ldr r0, _020B9E30 ; =_021D94D8
	ldrsh r7, [r5, #0x4a]
	ldr r0, [r0, #8]
	ldrsh r6, [r5, #0x48]
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	ldr r0, _020B9E30 ; =_021D94D8
	rsbne r6, r6, #0
	ldr r0, [r0, #8]
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	rsbne r7, r7, #0
	mov r0, r6, lsl #0xc
	mov r1, r7, lsl #0xc
	mov r2, #0
	bl NNS_G2dTranslate
_020B9D8C:
	ldrh r0, [r5, #0x4e]
	tst r0, #4
	beq _020B9DC0
	ldrh r0, [r5, #0x4c]
	ldr r2, _020B9E34 ; =FX_SinCosTable_
	mov r0, r0, asr #4
	mov r1, r0, lsl #1
	add r0, r1, #1
	mov r3, r1, lsl #1
	mov r1, r0, lsl #1
	ldrsh r0, [r2, r3]
	ldrsh r1, [r2, r1]
	bl NNS_G2dRotZ
_020B9DC0:
	ldrh r0, [r5, #0x4e]
	tst r0, #2
	beq _020B9DDC
	ldr r0, [r5, #0x40]
	ldr r1, [r5, #0x44]
	mov r2, #0x1000
	bl NNS_G2dScale
_020B9DDC:
	ldr r2, [r5, #0x38]
	mvn r0, #0
	cmp r2, r0
	movne r0, #1
	moveq r0, #0
	cmp r0, #0
	beq _020B9E18
	ldr r1, _020B9E2C ; =_021109DC
	mov r0, r4
	str r2, [r1, #4]
	bl DrawCellImpl_
	ldr r0, _020B9E2C ; =_021109DC
	mvn r1, #0
	str r1, [r0, #4]
	b _020B9E20
_020B9E18:
	mov r0, r4
	bl DrawCellImpl_
_020B9E20:
	mov r0, #1
	bl NNS_G2dPopMtx
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B9E2C: .word _021109DC
_020B9E30: .word _021D94D8
_020B9E34: .word FX_SinCosTable_
	arm_func_end DrawCellAnimationImpl_

	arm_func_start DrawNode_
DrawNode_: ; 0x020B9E38
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r0
	ldr r0, [r5, #8]
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	ldr r4, [r5]
	bl NNS_G2dPushMtx
	ldrh r0, [r5, #0x1e]
	tst r0, #8
	beq _020B9EA0
	ldr r0, _020B9F04 ; =_021D94D8
	ldrsh r7, [r5, #0x1a]
	ldr r0, [r0, #8]
	ldrsh r6, [r5, #0x18]
	bl NNS_G2dIsRndCoreFlipH
	cmp r0, #0
	ldr r0, _020B9F04 ; =_021D94D8
	rsbne r6, r6, #0
	ldr r0, [r0, #8]
	bl NNS_G2dIsRndCoreFlipV
	cmp r0, #0
	rsbne r7, r7, #0
	mov r0, r6, lsl #0xc
	mov r1, r7, lsl #0xc
	mov r2, #0
	bl NNS_G2dTranslate
_020B9EA0:
	ldrh r0, [r5, #0x1e]
	tst r0, #4
	beq _020B9ED4
	ldrh r0, [r5, #0x1c]
	ldr r2, _020B9F08 ; =FX_SinCosTable_
	mov r0, r0, asr #4
	mov r1, r0, lsl #1
	add r0, r1, #1
	mov r3, r1, lsl #1
	mov r1, r0, lsl #1
	ldrsh r0, [r2, r3]
	ldrsh r1, [r2, r1]
	bl NNS_G2dRotZ
_020B9ED4:
	ldrh r0, [r5, #0x1e]
	tst r0, #2
	beq _020B9EF0
	ldr r0, [r5, #0x10]
	ldr r1, [r5, #0x14]
	mov r2, #0x1000
	bl NNS_G2dScale
_020B9EF0:
	mov r0, r4
	bl DrawCellAnimationImpl_
	mov r0, #1
	bl NNS_G2dPopMtx
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B9F04: .word _021D94D8
_020B9F08: .word FX_SinCosTable_
	arm_func_end DrawNode_
