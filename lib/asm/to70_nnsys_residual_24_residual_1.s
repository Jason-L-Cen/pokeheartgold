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
