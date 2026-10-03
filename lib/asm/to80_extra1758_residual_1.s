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
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_G2dMCMStoreCurrentMtxToMtxCache

.text
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
