	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNS_G2dSetImageLocation

.text
	arm_func_start NNS_G2dLoadImage1DMapping
NNS_G2dLoadImage1DMapping: ; 0x020B80A0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	movs r5, r2
	mov r6, r1
	mov r4, r3
	ldr r0, [r7, #8]
	beq _020B8104
	cmp r5, #1
	beq _020B80D0
	cmp r5, #2
	beq _020B80EC
	b _020B8104
_020B80D0:
	mov r3, #0x4000000
	ldr r2, [r3]
	ldr r1, _020B82C4 ; =0xFFCFFFEF
	and r1, r2, r1
	orr r0, r1, r0
	str r0, [r3]
	b _020B8104
_020B80EC:
	ldr r3, _020B82C8 ; =0x04001000
	ldr r1, _020B82C4 ; =0xFFCFFFEF
	ldr r2, [r3]
	and r1, r2, r1
	orr r0, r1, r0
	str r0, [r3]
_020B8104:
	ldr r0, [r7, #0x14]
	ldr r1, [r7, #0x10]
	bl DC_FlushRange
	cmp r5, #0
	beq _020B812C
	cmp r5, #1
	beq _020B8148
	cmp r5, #2
	beq _020B815C
	b _020B816C
_020B812C:
	bl GX_BeginLoadTex
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GX_LoadTex
	bl GX_EndLoadTex
	b _020B816C
_020B8148:
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GX_LoadOBJ
	b _020B816C
_020B815C:
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GXS_LoadOBJ
_020B816C:
	ldr r0, [r7, #8]
	cmp r0, #0
	ldrh r0, [r7, #2]
	bne _020B8284
	cmp r0, #0x10
	bgt _020B81B8
	bge _020B81E4
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020B81F4
_020B8194: ; jump table
	b _020B81F4 ; case 0
	b _020B81C4 ; case 1
	b _020B81CC ; case 2
	b _020B81F4 ; case 3
	b _020B81D4 ; case 4
	b _020B81F4 ; case 5
	b _020B81F4 ; case 6
	b _020B81F4 ; case 7
	b _020B81DC ; case 8
_020B81B8:
	cmp r0, #0x20
	beq _020B81EC
	b _020B81F4
_020B81C4:
	mov r0, #0
	b _020B81F8
_020B81CC:
	mov r0, #1
	b _020B81F8
_020B81D4:
	mov r0, #2
	b _020B81F8
_020B81DC:
	mov r0, #3
	b _020B81F8
_020B81E4:
	mov r0, #4
	b _020B81F8
_020B81EC:
	mov r0, #5
	b _020B81F8
_020B81F4:
	mov r0, #0
_020B81F8:
	str r0, [r4, #0xc]
	ldrh r0, [r7]
	cmp r0, #0x10
	bgt _020B823C
	bge _020B8268
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020B8278
_020B8218: ; jump table
	b _020B8278 ; case 0
	b _020B8248 ; case 1
	b _020B8250 ; case 2
	b _020B8278 ; case 3
	b _020B8258 ; case 4
	b _020B8278 ; case 5
	b _020B8278 ; case 6
	b _020B8278 ; case 7
	b _020B8260 ; case 8
_020B823C:
	cmp r0, #0x20
	beq _020B8270
	b _020B8278
_020B8248:
	mov r0, #0
	b _020B827C
_020B8250:
	mov r0, #1
	b _020B827C
_020B8258:
	mov r0, #2
	b _020B827C
_020B8260:
	mov r0, #3
	b _020B827C
_020B8268:
	mov r0, #4
	b _020B827C
_020B8270:
	mov r0, #5
	b _020B827C
_020B8278:
	mov r0, #0
_020B827C:
	str r0, [r4, #0x10]
	b _020B8290
_020B8284:
	str r0, [r4, #0xc]
	ldrh r0, [r7]
	str r0, [r4, #0x10]
_020B8290:
	ldr r1, [r7, #4]
	mov r0, #0
	str r1, [r4, #0x14]
	str r0, [r4, #0x18]
	mov r0, #1
	str r0, [r4, #0x1c]
	ldr r3, [r7, #8]
	mov r0, r4
	mov r1, r5
	mov r2, r6
	str r3, [r4, #0x20]
	bl NNS_G2dSetImageLocation
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B82C4: .word 0xFFCFFFEF
_020B82C8: .word 0x04001000
	arm_func_end NNS_G2dLoadImage1DMapping

	arm_func_start NNS_G2dLoadImage2DMapping
NNS_G2dLoadImage2DMapping: ; 0x020B82CC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	movs r5, r2
	mov r6, r1
	mov r4, r3
	ldr r0, [r7, #8]
	beq _020B8330
	cmp r5, #1
	beq _020B82FC
	cmp r5, #2
	beq _020B8318
	b _020B8330
_020B82FC:
	mov r3, #0x4000000
	ldr r2, [r3]
	ldr r1, _020B84F0 ; =0xFFCFFFEF
	and r1, r2, r1
	orr r0, r1, r0
	str r0, [r3]
	b _020B8330
_020B8318:
	ldr r3, _020B84F4 ; =0x04001000
	ldr r1, _020B84F0 ; =0xFFCFFFEF
	ldr r2, [r3]
	and r1, r2, r1
	orr r0, r1, r0
	str r0, [r3]
_020B8330:
	ldr r0, [r7, #0x14]
	ldr r1, [r7, #0x10]
	bl DC_FlushRange
	cmp r5, #0
	beq _020B8358
	cmp r5, #1
	beq _020B8374
	cmp r5, #2
	beq _020B8388
	b _020B8398
_020B8358:
	bl GX_BeginLoadTex
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GX_LoadTex
	bl GX_EndLoadTex
	b _020B8398
_020B8374:
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GX_LoadOBJ
	b _020B8398
_020B8388:
	ldr r0, [r7, #0x14]
	ldr r2, [r7, #0x10]
	mov r1, r6
	bl GXS_LoadOBJ
_020B8398:
	ldr r0, [r7, #8]
	cmp r0, #0
	ldrh r0, [r7, #2]
	bne _020B84B0
	cmp r0, #0x10
	bgt _020B83E4
	bge _020B8410
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020B8420
_020B83C0: ; jump table
	b _020B8420 ; case 0
	b _020B83F0 ; case 1
	b _020B83F8 ; case 2
	b _020B8420 ; case 3
	b _020B8400 ; case 4
	b _020B8420 ; case 5
	b _020B8420 ; case 6
	b _020B8420 ; case 7
	b _020B8408 ; case 8
_020B83E4:
	cmp r0, #0x20
	beq _020B8418
	b _020B8420
_020B83F0:
	mov r0, #0
	b _020B8424
_020B83F8:
	mov r0, #1
	b _020B8424
_020B8400:
	mov r0, #2
	b _020B8424
_020B8408:
	mov r0, #3
	b _020B8424
_020B8410:
	mov r0, #4
	b _020B8424
_020B8418:
	mov r0, #5
	b _020B8424
_020B8420:
	mov r0, #0
_020B8424:
	str r0, [r4, #0xc]
	ldrh r0, [r7]
	cmp r0, #0x10
	bgt _020B8468
	bge _020B8494
	cmp r0, #8
	addls pc, pc, r0, lsl #2
	b _020B84A4
_020B8444: ; jump table
	b _020B84A4 ; case 0
	b _020B8474 ; case 1
	b _020B847C ; case 2
	b _020B84A4 ; case 3
	b _020B8484 ; case 4
	b _020B84A4 ; case 5
	b _020B84A4 ; case 6
	b _020B84A4 ; case 7
	b _020B848C ; case 8
_020B8468:
	cmp r0, #0x20
	beq _020B849C
	b _020B84A4
_020B8474:
	mov r0, #0
	b _020B84A8
_020B847C:
	mov r0, #1
	b _020B84A8
_020B8484:
	mov r0, #2
	b _020B84A8
_020B848C:
	mov r0, #3
	b _020B84A8
_020B8494:
	mov r0, #4
	b _020B84A8
_020B849C:
	mov r0, #5
	b _020B84A8
_020B84A4:
	mov r0, #0
_020B84A8:
	str r0, [r4, #0x10]
	b _020B84BC
_020B84B0:
	str r0, [r4, #0xc]
	ldrh r0, [r7]
	str r0, [r4, #0x10]
_020B84BC:
	ldr r1, [r7, #4]
	mov r0, #0
	str r1, [r4, #0x14]
	str r0, [r4, #0x18]
	mov r0, #1
	str r0, [r4, #0x1c]
	ldr r3, [r7, #8]
	mov r0, r4
	mov r1, r5
	mov r2, r6
	str r3, [r4, #0x20]
	bl NNS_G2dSetImageLocation
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020B84F0: .word 0xFFCFFFEF
_020B84F4: .word 0x04001000
	arm_func_end NNS_G2dLoadImage2DMapping
