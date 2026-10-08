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
	.extern NNSi_G2dMCMStoreCurrentMtxToMtxCache

.text
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
