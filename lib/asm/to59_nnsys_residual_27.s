	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G2dPopMtx
	.public NNS_G2dPushMtx
	.public NNS_G2dRotZ
	.public NNS_G2dScale
	.public NNS_G2dTranslate
	.extern NNSi_G2dMtxConcat32
	.extern _021109DC
	.extern _021D94D8
	.extern _021D94E8
	.extern _021D9568
	.extern _021D956C
	.extern _021D9668
	.extern _021D9678
	.extern _021D967C
	.extern _021D9968

.text
	arm_func_start NNS_G2dPushMtx
NNS_G2dPushMtx: ; 0x020BA560
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	ldr r0, _020BA764 ; =_021D94D8
	ldr r1, [r0, #8]
	ldr r1, [r1, #0x7c]
	tst r1, #1
	bne _020BA6A0
	ldr r1, [r0, #0xc]
	mov r0, r1, lsl #0x10
	add r6, r1, #1
	cmp r6, #0x20
	mov r5, r0, lsr #0x10
	movlt r0, #1
	movge r0, #0
	cmp r0, #0
	beq _020BA640
	ldr r0, _020BA764 ; =_021D94D8
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _020BA5E4
	ldr r3, [r0, #0xc]
	mov r0, #0x18
	mul r8, r3, r0
	ldr r4, _020BA768 ; =_021D9678
	ldr r1, _020BA76C ; =_021D94E8
	mul r7, r6, r0
	ldr r2, [r4, r8]
	ldr r0, [r1, r3, lsl #2]
	ldr r3, _020BA770 ; =_021D967C
	str r2, [r4, r7]
	ldr r2, [r3, r8]
	str r0, [r1, r6, lsl #2]
	str r2, [r3, r7]
	b _020BA638
_020BA5E4:
	ldr r4, [r0, #0xc]
	mov r0, #0x18
	mul lr, r4, r0
	ldr r1, _020BA774 ; =_021D9668
	mul ip, r6, r0
	ldr r8, _020BA778 ; =_021D9968
	add sl, r1, lr
	ldr r7, _020BA76C ; =_021D94E8
	add sb, r1, ip
	ldmia sl!, {r0, r1, r2, r3}
	stmia sb!, {r0, r1, r2, r3}
	ldmia sl, {r0, r1}
	stmia sb, {r0, r1}
	add sb, r8, lr
	ldr r4, [r7, r4, lsl #2]
	add r8, r8, ip
	ldmia sb!, {r0, r1, r2, r3}
	stmia r8!, {r0, r1, r2, r3}
	str r4, [r7, r6, lsl #2]
	ldmia sb, {r0, r1}
	stmia r8, {r0, r1}
_020BA638:
	ldr r0, _020BA764 ; =_021D94D8
	str r6, [r0, #0xc]
_020BA640:
	ldr r0, _020BA764 ; =_021D94D8
	ldr r2, _020BA77C ; =_021D9568
	ldr r0, [r0, #0xc]
	mov r6, r5, lsl #3
	mov r0, r0, lsl #0x10
	add r4, r2, r5, lsl #3
	mov r3, r0, lsr #0x10
	mov r5, r3, lsl #3
	ldrh r1, [r2, r6]
	ldrh r0, [r4, #2]
	add r3, r2, r3, lsl #3
	strh r1, [r2, r5]
	strh r0, [r3, #2]
	ldrh r2, [r4, #4]
	ldrh r0, [r4, #6]
	ldr r1, _020BA780 ; =_021D956C
	strh r2, [r3, #4]
	strh r0, [r3, #6]
	ldrh r0, [r1, r6]
	cmp r0, #1
	moveq r0, #2
	streqh r0, [r1, r5]
	strneh r0, [r1, r5]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020BA6A0:
	ldr r0, [r0, #0xc]
	add r4, r0, #1
	cmp r4, #0x20
	movlt r0, #1
	movge r0, #0
	cmp r0, #0
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	ldr r0, _020BA764 ; =_021D94D8
	ldr r1, [r0, #4]
	cmp r1, #0
	beq _020BA704
	ldr r3, [r0, #0xc]
	mov r0, #0x18
	mul r7, r3, r0
	ldr r5, _020BA768 ; =_021D9678
	ldr r1, _020BA76C ; =_021D94E8
	mul r6, r4, r0
	ldr r2, [r5, r7]
	ldr r0, [r1, r3, lsl #2]
	ldr r3, _020BA770 ; =_021D967C
	str r2, [r5, r6]
	ldr r2, [r3, r7]
	str r0, [r1, r4, lsl #2]
	str r2, [r3, r6]
	b _020BA758
_020BA704:
	ldr sb, [r0, #0xc]
	mov r0, #0x18
	mul lr, sb, r0
	ldr r1, _020BA774 ; =_021D9668
	mul r8, r4, r0
	ldr ip, _020BA778 ; =_021D9968
	add r5, r1, lr
	ldr r6, _020BA76C ; =_021D94E8
	add r7, r1, r8
	ldmia r5!, {r0, r1, r2, r3}
	stmia r7!, {r0, r1, r2, r3}
	ldmia r5, {r0, r1}
	ldr r5, [r6, sb, lsl #2]
	add lr, ip, lr
	stmia r7, {r0, r1}
	add ip, ip, r8
	ldmia lr!, {r0, r1, r2, r3}
	stmia ip!, {r0, r1, r2, r3}
	ldmia lr, {r0, r1}
	stmia ip, {r0, r1}
	str r5, [r6, r4, lsl #2]
_020BA758:
	ldr r0, _020BA764 ; =_021D94D8
	str r4, [r0, #0xc]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020BA764: .word _021D94D8
_020BA768: .word _021D9678
_020BA76C: .word _021D94E8
_020BA770: .word _021D967C
_020BA774: .word _021D9668
_020BA778: .word _021D9968
_020BA77C: .word _021D9568
_020BA780: .word _021D956C
	arm_func_end NNS_G2dPushMtx

	arm_func_start NNS_G2dPopMtx
NNS_G2dPopMtx: ; 0x020BA784
	ldr r0, _020BA7C8 ; =_021D94D8
	ldr r0, [r0, #0xc]
	cmp r0, #0
	movle r0, #1
	movgt r0, #0
	cmp r0, #0
	bxne lr
	ldr r1, _020BA7C8 ; =_021D94D8
	ldr r0, _020BA7CC ; =_021109DC
	ldr r3, [r1, #0xc]
	ldrh r2, [r0]
	sub r3, r3, #1
	str r3, [r1, #0xc]
	cmp r2, r3
	ldrgt r1, _020BA7D0 ; =0x0000FFFE
	strgth r1, [r0]
	bx lr
	.align 2, 0
_020BA7C8: .word _021D94D8
_020BA7CC: .word _021109DC
_020BA7D0: .word 0x0000FFFE
	arm_func_end NNS_G2dPopMtx

	arm_func_start NNS_G2dTranslate
NNS_G2dTranslate: ; 0x020BA7D4
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x18
	ldr r3, _020BA8B8 ; =_021D94D8
	mov r4, r2
	ldr r2, [r3, #4]
	cmp r2, #0
	beq _020BA834
	ldr r2, [r3, #0xc]
	mov r3, #0x18
	mul r6, r2, r3
	ldr r5, _020BA8BC ; =_021D9678
	ldr r3, _020BA8C0 ; =_021D94E8
	ldr ip, [r5, r6]
	ldr lr, _020BA8C4 ; =_021D967C
	add r0, ip, r0
	str r0, [r5, r6]
	ldr r0, [r3, r2, lsl #2]
	ldr ip, [lr, r6]
	add r0, r0, r4
	add r1, ip, r1
	str r0, [r3, r2, lsl #2]
	add sp, sp, #0x18
	str r1, [lr, r6]
	ldmia sp!, {r4, r5, r6, pc}
_020BA834:
	mov ip, #0x1000
	mov r2, #0
	str r0, [sp, #0x10]
	str ip, [sp]
	str r2, [sp, #4]
	str r2, [sp, #8]
	str ip, [sp, #0xc]
	str r1, [sp, #0x14]
	ldr r1, [r3, #0xc]
	mov r0, #0x18
	mul r3, r1, r0
	ldr r2, _020BA8C8 ; =_021D9668
	add r0, sp, #0
	add r1, r2, r3
	add r2, r2, r3
	bl NNSi_G2dMtxConcat32
	ldr r1, _020BA8B8 ; =_021D94D8
	mov r0, #0x18
	ldr r1, [r1, #0xc]
	ldr r2, _020BA8CC ; =_021D9968
	mul r3, r1, r0
	add r1, r2, r3
	add r0, sp, #0
	add r2, r2, r3
	bl NNSi_G2dMtxConcat32
	ldr r0, _020BA8B8 ; =_021D94D8
	ldr r2, _020BA8C0 ; =_021D94E8
	ldr r1, [r0, #0xc]
	ldr r0, [r2, r1, lsl #2]
	add r0, r0, r4
	str r0, [r2, r1, lsl #2]
	add sp, sp, #0x18
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020BA8B8: .word _021D94D8
_020BA8BC: .word _021D9678
_020BA8C0: .word _021D94E8
_020BA8C4: .word _021D967C
_020BA8C8: .word _021D9668
_020BA8CC: .word _021D9968
	arm_func_end NNS_G2dTranslate

	arm_func_start NNS_G2dScale
NNS_G2dScale: ; 0x020BA8D0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x18
	ldr r2, _020BAA0C ; =_021D94D8
	mov lr, #0
	ldr r3, [r2, #0xc]
	mov r2, #0x18
	mul ip, r3, r2
	mov r5, r0
	mov r4, r1
	ldr r2, _020BAA10 ; =_021D9668
	add r0, sp, #0
	add r1, r2, ip
	add r2, r2, ip
	str r5, [sp]
	str lr, [sp, #4]
	str lr, [sp, #8]
	str r4, [sp, #0xc]
	str lr, [sp, #0x10]
	str lr, [sp, #0x14]
	bl NNSi_G2dMtxConcat32
	mov r0, r5
	bl FX_Inv
	mov r1, #0
	str r0, [sp]
	mov r0, r4
	str r1, [sp, #4]
	str r1, [sp, #8]
	bl FX_Inv
	mov ip, #0
	ldr r1, _020BAA0C ; =_021D94D8
	str r0, [sp, #0xc]
	ldr r1, [r1, #0xc]
	mov r0, #0x18
	mul r3, r1, r0
	ldr r2, _020BAA14 ; =_021D9968
	add r0, sp, #0
	add r1, r2, r3
	add r2, r2, r3
	str ip, [sp, #0x10]
	str ip, [sp, #0x14]
	bl NNSi_G2dMtxConcat32
	ldr r0, _020BAA0C ; =_021D94D8
	ldr r0, [r0, #4]
	cmp r0, #0
	bne _020BA99C
	ldr r1, _020BAA18 ; =_021109DC
	ldr r0, _020BAA1C ; =0x0000FFFE
	ldrh r1, [r1]
	cmp r1, r0
	movne r0, #1
	bne _020BA9A0
_020BA99C:
	mov r0, #0
_020BA9A0:
	cmp r0, #0
	bne _020BA9B8
	ldr r1, _020BAA0C ; =_021D94D8
	ldr r0, _020BAA18 ; =_021109DC
	ldr r1, [r1, #0xc]
	strh r1, [r0]
_020BA9B8:
	ldr r1, _020BAA0C ; =_021D94D8
	ldr r2, _020BAA20 ; =_021D9568
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x10
	add ip, r2, r0, lsr #13
	ldrh r0, [ip, #4]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _020BAA04
_020BA9DC: ; jump table
	b _020BA9EC ; case 0
	b _020BAA04 ; case 1
	b _020BA9EC ; case 2
	b _020BA9EC ; case 3
_020BA9EC:
	ldrh r3, [r1, #2]
	mov r0, #1
	add r2, r3, #1
	strh r2, [r1, #2]
	strh r3, [ip, #2]
	strh r0, [ip, #4]
_020BAA04:
	add sp, sp, #0x18
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BAA0C: .word _021D94D8
_020BAA10: .word _021D9668
_020BAA14: .word _021D9968
_020BAA18: .word _021109DC
_020BAA1C: .word 0x0000FFFE
_020BAA20: .word _021D9568
	arm_func_end NNS_G2dScale

	arm_func_start NNS_G2dRotZ
NNS_G2dRotZ: ; 0x020BAA24
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x18
	ldr r2, _020BAB2C ; =_021D94D8
	mov lr, #0
	ldr r3, [r2, #0xc]
	mov r2, #0x18
	mul ip, r3, r2
	rsb r3, r0, #0
	str r0, [sp, #4]
	ldr r2, _020BAB30 ; =_021D9668
	add r0, sp, #0
	str r1, [sp]
	str r1, [sp, #0xc]
	add r1, r2, ip
	add r2, r2, ip
	str r3, [sp, #8]
	str lr, [sp, #0x10]
	str lr, [sp, #0x14]
	bl NNSi_G2dMtxConcat32
	ldr r1, _020BAB2C ; =_021D94D8
	mov r0, #0x18
	ldr r1, [r1, #0xc]
	ldr r2, _020BAB34 ; =_021D9968
	mul r3, r1, r0
	add r1, r2, r3
	add r0, sp, #0
	add r2, r2, r3
	bl NNSi_G2dMtxConcat32
	ldr r0, _020BAB2C ; =_021D94D8
	ldr r0, [r0, #4]
	cmp r0, #0
	bne _020BAABC
	ldr r1, _020BAB38 ; =_021109DC
	ldr r0, _020BAB3C ; =0x0000FFFE
	ldrh r1, [r1]
	cmp r1, r0
	movne r0, #1
	bne _020BAAC0
_020BAABC:
	mov r0, #0
_020BAAC0:
	cmp r0, #0
	bne _020BAAD8
	ldr r1, _020BAB2C ; =_021D94D8
	ldr r0, _020BAB38 ; =_021109DC
	ldr r1, [r1, #0xc]
	strh r1, [r0]
_020BAAD8:
	ldr r1, _020BAB2C ; =_021D94D8
	ldr r2, _020BAB40 ; =_021D9568
	ldr r0, [r1, #0xc]
	mov r0, r0, lsl #0x10
	add ip, r2, r0, lsr #13
	ldrh r0, [ip, #4]
	cmp r0, #3
	addls pc, pc, r0, lsl #2
	b _020BAB24
_020BAAFC: ; jump table
	b _020BAB0C ; case 0
	b _020BAB24 ; case 1
	b _020BAB0C ; case 2
	b _020BAB0C ; case 3
_020BAB0C:
	ldrh r3, [r1, #2]
	mov r0, #1
	add r2, r3, #1
	strh r2, [r1, #2]
	strh r3, [ip, #2]
	strh r0, [ip, #4]
_020BAB24:
	add sp, sp, #0x18
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BAB2C: .word _021D94D8
_020BAB30: .word _021D9668
_020BAB34: .word _021D9968
_020BAB38: .word _021109DC
_020BAB3C: .word 0x0000FFFE
_020BAB40: .word _021D9568
	arm_func_end NNS_G2dRotZ
