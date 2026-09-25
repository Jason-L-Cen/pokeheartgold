	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dRndCoreBeginRendering
	.extern NNS_G2dGetImageLocation
	.extern NNS_G2dGetImagePaletteLocation
	.extern NNS_G2dIsImagePaletteReadyToUse
	.extern NNS_G2dIsImageReadyToUse
	.extern _021DA498

.text
	arm_func_start NNS_G2dRndCoreBeginRendering
NNS_G2dRndCoreBeginRendering: ; 0x020BBFB0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r2, _020BC0B4 ; =_021DA498
	mov r4, r0
	str r4, [r2]
	ldr r0, [r4]
	ldr r5, [r0, #0x14]
	cmp r5, #0
	bne _020BC068
	ldr lr, _020BC0B8 ; =0x04000440
	mov r1, #0
	str r1, [lr]
	str r1, [lr, #4]
	ldr ip, [r4]
	mov r0, #1
	ldmia ip, {r3, ip}
	rsb r3, r3, #0
	str r3, [lr, #0x30]
	rsb r3, ip, #0
	str r3, [lr, #0x30]
	str r1, [lr, #0x30]
	str r0, [lr]
	ldr r0, [r2]
	ldr r5, [r0, #8]
	mov r0, r5
	bl NNS_G2dIsImageReadyToUse
	cmp r0, #0
	moveq r0, #0
	beq _020BC02C
	mov r0, r5
	mov r1, #0
	bl NNS_G2dGetImageLocation
_020BC02C:
	ldr r1, _020BC0B4 ; =_021DA498
	str r0, [r4, #0x14]
	ldr r0, [r1]
	mov r1, #0
	ldr r5, [r0, #0xc]
	mov r0, r5
	bl NNS_G2dIsImagePaletteReadyToUse
	cmp r0, #0
	moveq r0, #0
	beq _020BC060
	mov r0, r5
	mov r1, #0
	bl NNS_G2dGetImagePaletteLocation
_020BC060:
	str r0, [r4, #0x18]
	ldmia sp!, {r3, r4, r5, pc}
_020BC068:
	ldr r4, [r4, #8]
	mov r1, r5
	mov r0, r4
	bl NNS_G2dIsImageReadyToUse
	cmp r0, #0
	moveq r1, #0
	beq _020BC0A4
	mov r0, r4
	mov r1, r5
	bl NNS_G2dGetImageLocation
	ldr r1, [r4, #0x20]
	and r1, r1, #0x700000
	mov r1, r1, asr #0x14
	add r1, r1, #5
	mov r1, r0, lsr r1
_020BC0A4:
	ldr r0, _020BC0B4 ; =_021DA498
	ldr r0, [r0]
	str r1, [r0, #0x10]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BC0B4: .word _021DA498
_020BC0B8: .word 0x04000440
	arm_func_end NNS_G2dRndCoreBeginRendering
