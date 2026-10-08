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
