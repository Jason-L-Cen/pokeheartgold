	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
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
