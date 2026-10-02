	.extern NNS_G3dGeBufferOP_N
	.extern NNS_G3dGetResDataByName
	.extern NNS_G3dGlb
	.extern _02109474
	.extern _02110A74
	.extern _02110A80
	.extern _02110A8C
	.extern _02110B9C
	.extern _021DA700
	.extern _021DA704
	.extern addLink_
	.extern updateHintVec_
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start bindMdlTex_Internal_
bindMdlTex_Internal_: ; 0x020BE5A8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	mov r8, r3
	ldr r1, [r8]
	mov sl, r0
	and r0, r1, #0x1c000000
	ldrh r3, [sb]
	cmp r0, #0x14000000
	mov r5, #0
	add r0, sl, r3
	str r0, [sp]
	ldrne r0, [r2, #8]
	ldreq r0, [r2, #0x18]
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bic fp, r0, #0xe0000000
	ldrb r0, [sb, #2]
	cmp r0, #0
	bls _020BE6C4
	add r4, sl, #4
_020BE5F8:
	ldr r0, [sp]
	cmp sl, #0
	ldrb r2, [r0, r5]
	beq _020BE648
	cmp r4, #0
	beq _020BE634
	ldrb r0, [sl, #5]
	cmp r2, r0
	bhs _020BE634
	ldrh r0, [sl, #0xa]
	add r1, r4, r0
	add r1, r1, #4
	ldrh r0, [r4, r0]
	mla r1, r0, r2, r1
	b _020BE638
_020BE634:
	mov r1, #0
_020BE638:
	cmp r1, #0
	ldrne r0, [r1]
	addne r7, sl, r0
	bne _020BE64C
_020BE648:
	mov r7, #0
_020BE64C:
	ldr r0, [r8]
	ldr r1, [r7, #0x14]
	add r0, r0, fp
	orr r0, r1, r0
	str r0, [r7, #0x14]
	ldr r3, [r8, #4]
	ldr r0, _020BE6D4 ; =0x000007FF
	ldrh r1, [r7, #0x20]
	and r2, r3, r0
	and r0, r3, r0, lsl #11
	mov r6, r0, lsr #0xb
	cmp r2, r1
	moveq r0, #0x1000
	beq _020BE690
	mov r0, r2, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020BE690:
	str r0, [r7, #0x24]
	ldrh r1, [r7, #0x22]
	cmp r6, r1
	moveq r0, #0x1000
	beq _020BE6B0
	mov r0, r6, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020BE6B0:
	str r0, [r7, #0x28]
	ldrb r0, [sb, #2]
	add r5, r5, #1
	cmp r5, r0
	blo _020BE5F8
_020BE6C4:
	ldrb r0, [sb, #3]
	orr r0, r0, #1
	strb r0, [sb, #3]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020BE6D4: .word 0x000007FF
	arm_func_end bindMdlTex_Internal_

	arm_func_start releaseMdlTex_Internal_
releaseMdlTex_Internal_: ; 0x020BE6D8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	ldrh r3, [r1]
	ldrb r2, [r1, #2]
	mov lr, #0
	add ip, r0, r3
	cmp r2, #0
	bls _020BE77C
	ldr r4, _020BE78C ; =0xC00F0000
	add r2, r0, #4
	mov r7, lr
	mov r6, lr
	mov r5, #0x1000
_020BE708:
	cmp r0, #0
	ldrb r3, [ip, lr]
	beq _020BE754
	cmp r2, #0
	beq _020BE740
	ldrb r8, [r0, #5]
	cmp r3, r8
	bhs _020BE740
	ldrh sb, [r0, #0xa]
	ldrh r8, [r2, sb]
	add sb, r2, sb
	add sb, sb, #4
	mla r3, r8, r3, sb
	b _020BE744
_020BE740:
	mov r3, r7
_020BE744:
	cmp r3, #0
	ldrne r3, [r3]
	addne r8, r0, r3
	bne _020BE758
_020BE754:
	mov r8, r6
_020BE758:
	ldr r3, [r8, #0x14]
	add lr, lr, #1
	and r3, r3, r4
	str r3, [r8, #0x14]
	str r5, [r8, #0x24]
	str r5, [r8, #0x28]
	ldrb r3, [r1, #2]
	cmp lr, r3
	blo _020BE708
_020BE77C:
	ldrb r0, [r1, #3]
	bic r0, r0, #1
	strb r0, [r1, #3]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020BE78C: .word 0xC00F0000
	arm_func_end releaseMdlTex_Internal_

	arm_func_start NNS_G3dBindMdlTex
NNS_G3dBindMdlTex: ; 0x020BE790
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r1
	cmp r0, #0
	ldrne r1, [r0, #8]
	mov r7, #1
	cmpne r1, #0
	addne r8, r0, r1
	moveq r8, #0
	ldrh r0, [r8]
	mov r6, #0
	add r5, r8, r0
	ldrb r0, [r5, #1]
	cmp r0, #0
	bls _020BE888
	mov sb, r6
	mov r4, r6
	mov fp, r6
_020BE7D4:
	cmp r5, #0
	beq _020BE800
	ldrb r0, [r5, #1]
	cmp r6, r0
	bhs _020BE800
	ldrh r0, [r5, #6]
	add r1, r5, r0
	ldrh r0, [r1, #2]
	add r0, r1, r0
	add r1, r0, sb
	b _020BE804
_020BE800:
	mov r1, r4
_020BE804:
	cmp sl, #0
	moveq r3, fp
	beq _020BE81C
	add r0, sl, #0x3c
	bl NNS_G3dGetResDataByName
	mov r3, r0
_020BE81C:
	cmp r3, #0
	beq _020BE870
	cmp r5, #0
	beq _020BE850
	ldrb r0, [r5, #1]
	cmp r6, r0
	bhs _020BE850
	ldrh r1, [r5, #6]
	ldrh r0, [r5, r1]
	add r1, r5, r1
	add r1, r1, #4
	mla r1, r0, r6, r1
	b _020BE854
_020BE850:
	mov r1, #0
_020BE854:
	ldrb r0, [r1, #3]
	tst r0, #1
	bne _020BE874
	mov r0, r8
	mov r2, sl
	bl bindMdlTex_Internal_
	b _020BE874
_020BE870:
	mov r7, #0
_020BE874:
	ldrb r0, [r5, #1]
	add r6, r6, #1
	add sb, sb, #0x10
	cmp r6, r0
	blo _020BE7D4
_020BE888:
	mov r0, r7
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G3dBindMdlTex

	arm_func_start NNS_G3dForceBindMdlTex
NNS_G3dForceBindMdlTex: ; 0x020BE890
	stmdb sp!, {r3, r4, r5, lr}
	mov ip, r1
	cmp r0, #0
	ldrne r1, [r0, #8]
	cmpne r1, #0
	addne r0, r0, r1
	moveq r0, #0
	ldrh r1, [r0]
	cmp ip, #0
	add r1, r0, r1
	beq _020BE8F0
	adds r5, ip, #0x3c
	beq _020BE8E8
	ldrb r4, [ip, #0x3d]
	cmp r3, r4
	bhs _020BE8E8
	ldrh lr, [ip, #0x42]
	ldrh r4, [r5, lr]
	add lr, r5, lr
	add lr, lr, #4
	mla r3, r4, r3, lr
	b _020BE8F4
_020BE8E8:
	mov r3, #0
	b _020BE8F4
_020BE8F0:
	mov r3, #0
_020BE8F4:
	cmp r1, #0
	beq _020BE920
	ldrb r4, [r1, #1]
	cmp r2, r4
	bhs _020BE920
	ldrh r4, [r1, #6]
	ldrh lr, [r1, r4]
	add r1, r1, r4
	add r1, r1, #4
	mla r1, lr, r2, r1
	b _020BE924
_020BE920:
	mov r1, #0
_020BE924:
	cmp r1, #0
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r2, ip
	bl bindMdlTex_Internal_
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNS_G3dForceBindMdlTex

	arm_func_start NNS_G3dReleaseMdlTex
NNS_G3dReleaseMdlTex: ; 0x020BE940
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	cmp r0, #0
	ldrne r1, [r0, #8]
	mov r5, #0
	cmpne r1, #0
	addne r6, r0, r1
	moveq r6, #0
	ldrh r0, [r6]
	add r4, r6, r0
	ldrb r0, [r4, #1]
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, pc}
	mov r7, r5
_020BE974:
	cmp r4, #0
	beq _020BE9A0
	ldrb r0, [r4, #1]
	cmp r5, r0
	bhs _020BE9A0
	ldrh r1, [r4, #6]
	ldrh r0, [r4, r1]
	add r1, r4, r1
	add r1, r1, #4
	mla r1, r0, r5, r1
	b _020BE9A4
_020BE9A0:
	mov r1, r7
_020BE9A4:
	ldrb r0, [r1, #3]
	tst r0, #1
	beq _020BE9B8
	mov r0, r6
	bl releaseMdlTex_Internal_
_020BE9B8:
	ldrb r0, [r4, #1]
	add r5, r5, #1
	cmp r5, r0
	blo _020BE974
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNS_G3dReleaseMdlTex

	arm_func_start bindMdlPltt_Internal_
bindMdlPltt_Internal_: ; 0x020BE9CC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r2, [r2, #0x2c]
	ldrh r5, [r1]
	mov r2, r2, lsl #0x10
	mov r2, r2, lsr #0x10
	ldrh r4, [r3, #2]
	bic r2, r2, #0xe0000000
	mov r2, r2, lsl #0x10
	tst r4, #1
	add ip, r0, r5
	mov r5, r2, lsr #0x10
	ldrh r4, [r3]
	bne _020BEA10
	mov r2, r4, lsl #0xf
	mov r3, r5, lsl #0xf
	mov r4, r2, lsr #0x10
	mov r5, r3, lsr #0x10
_020BEA10:
	ldrb r2, [r1, #2]
	mov r3, #0
	cmp r2, #0
	bls _020BEA9C
	add r2, r4, r5
	mov lr, r2, lsl #0x10
	add r2, r0, #4
	mov r4, lr, lsr #0x10
	mov r6, r3
	mov r5, r3
_020BEA38:
	cmp r0, #0
	ldrb r8, [ip, r3]
	beq _020BEA84
	cmp r2, #0
	beq _020BEA70
	ldrb r7, [r0, #5]
	cmp r8, r7
	bhs _020BEA70
	ldrh r7, [r0, #0xa]
	ldrh lr, [r2, r7]
	add r7, r2, r7
	add r7, r7, #4
	mla r7, lr, r8, r7
	b _020BEA74
_020BEA70:
	mov r7, r6
_020BEA74:
	cmp r7, #0
	ldrne lr, [r7]
	addne r7, r0, lr
	bne _020BEA88
_020BEA84:
	mov r7, r5
_020BEA88:
	strh r4, [r7, #0x1c]
	ldrb lr, [r1, #2]
	add r3, r3, #1
	cmp r3, lr
	blo _020BEA38
_020BEA9C:
	ldrb r0, [r1, #3]
	orr r0, r0, #1
	strb r0, [r1, #3]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end bindMdlPltt_Internal_

	arm_func_start NNS_G3dBindMdlPltt
NNS_G3dBindMdlPltt: ; 0x020BEAAC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r1
	cmp r0, #0
	ldrne r1, [r0, #8]
	mov r7, #1
	cmpne r1, #0
	addne r8, r0, r1
	moveq r8, #0
	ldrh r0, [r8, #2]
	mov r6, #0
	add r5, r8, r0
	ldrb r0, [r5, #1]
	cmp r0, #0
	bls _020BEBAC
	mov sb, r6
	mov r4, r6
	mov fp, r6
_020BEAF0:
	cmp r5, #0
	beq _020BEB1C
	ldrb r0, [r5, #1]
	cmp r6, r0
	bhs _020BEB1C
	ldrh r0, [r5, #6]
	add r1, r5, r0
	ldrh r0, [r1, #2]
	add r0, r1, r0
	add r1, r0, sb
	b _020BEB20
_020BEB1C:
	mov r1, r4
_020BEB20:
	cmp sl, #0
	ldrneh r0, [sl, #0x34]
	cmpne r0, #0
	moveq r3, fp
	beq _020BEB40
	add r0, sl, r0
	bl NNS_G3dGetResDataByName
	mov r3, r0
_020BEB40:
	cmp r3, #0
	beq _020BEB94
	cmp r5, #0
	beq _020BEB74
	ldrb r0, [r5, #1]
	cmp r6, r0
	bhs _020BEB74
	ldrh r1, [r5, #6]
	ldrh r0, [r5, r1]
	add r1, r5, r1
	add r1, r1, #4
	mla r1, r0, r6, r1
	b _020BEB78
_020BEB74:
	mov r1, #0
_020BEB78:
	ldrb r0, [r1, #3]
	tst r0, #1
	bne _020BEB98
	mov r0, r8
	mov r2, sl
	bl bindMdlPltt_Internal_
	b _020BEB98
_020BEB94:
	mov r7, #0
_020BEB98:
	ldrb r0, [r5, #1]
	add r6, r6, #1
	add sb, sb, #0x10
	cmp r6, r0
	blo _020BEAF0
_020BEBAC:
	mov r0, r7
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G3dBindMdlPltt

	arm_func_start NNS_G3dForceBindMdlPltt
NNS_G3dForceBindMdlPltt: ; 0x020BEBB4
	stmdb sp!, {r3, r4, r5, lr}
	mov ip, r1
	cmp r0, #0
	ldrne r1, [r0, #8]
	cmpne r1, #0
	addne r0, r0, r1
	moveq r0, #0
	ldrh r1, [r0, #2]
	cmp ip, #0
	ldrneh r4, [ip, #0x34]
	add r1, r0, r1
	cmpne r4, #0
	beq _020BEC1C
	adds r5, ip, r4
	beq _020BEC14
	ldrb r4, [r5, #1]
	cmp r3, r4
	bhs _020BEC14
	ldrh lr, [r5, #6]
	ldrh r4, [r5, lr]
	add lr, r5, lr
	add lr, lr, #4
	mla r3, r4, r3, lr
	b _020BEC20
_020BEC14:
	mov r3, #0
	b _020BEC20
_020BEC1C:
	mov r3, #0
_020BEC20:
	cmp r1, #0
	beq _020BEC4C
	ldrb r4, [r1, #1]
	cmp r2, r4
	bhs _020BEC4C
	ldrh r4, [r1, #6]
	ldrh lr, [r1, r4]
	add r1, r1, r4
	add r1, r1, #4
	mla r1, lr, r2, r1
	b _020BEC50
_020BEC4C:
	mov r1, #0
_020BEC50:
	cmp r1, #0
	beq _020BEC74
	ldrb r2, [r1, #3]
	tst r2, #1
	bne _020BEC74
	mov r2, ip
	bl bindMdlPltt_Internal_
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
_020BEC74:
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNS_G3dForceBindMdlPltt

	arm_func_start NNS_G3dReleaseMdlPltt
NNS_G3dReleaseMdlPltt: ; 0x020BEC7C
	cmp r0, #0
	ldrne r1, [r0, #8]
	mov ip, #0
	cmpne r1, #0
	addne r1, r0, r1
	moveq r1, #0
	ldrh r0, [r1, #2]
	add r3, r1, r0
	ldrb r0, [r3, #1]
	cmp r0, #0
	bxls lr
	mov r1, ip
_020BECAC:
	cmp r3, #0
	beq _020BECD8
	ldrb r0, [r3, #1]
	cmp ip, r0
	bhs _020BECD8
	ldrh r2, [r3, #6]
	ldrh r0, [r3, r2]
	add r2, r3, r2
	add r2, r2, #4
	mla r2, r0, ip, r2
	b _020BECDC
_020BECD8:
	mov r2, r1
_020BECDC:
	ldrb r0, [r2, #3]
	add ip, ip, #1
	tst r0, #1
	bicne r0, r0, #1
	strneb r0, [r2, #3]
	ldrb r0, [r3, #1]
	cmp ip, r0
	blo _020BECAC
	bx lr
	arm_func_end NNS_G3dReleaseMdlPltt

	arm_func_start NNS_G3dBindMdlSet
NNS_G3dBindMdlSet: ; 0x020BED00
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	ldrb r0, [sl, #9]
	mov sb, r1
	mov r7, #1
	cmp r0, #0
	mov r6, #0
	bls _020BEDA8
	add r5, sl, #8
	mov r4, r6
	mov fp, r6
_020BED2C:
	cmp sl, #0
	beq _020BED74
	cmp r5, #0
	beq _020BED60
	ldrb r0, [sl, #9]
	cmp r6, r0
	bhs _020BED60
	ldrh r1, [sl, #0xe]
	ldrh r0, [r5, r1]
	add r1, r5, r1
	add r1, r1, #4
	mla r1, r0, r6, r1
	b _020BED64
_020BED60:
	mov r1, r4
_020BED64:
	cmp r1, #0
	ldrne r0, [r1]
	addne r8, sl, r0
	bne _020BED78
_020BED74:
	mov r8, fp
_020BED78:
	mov r0, r8
	mov r1, sb
	bl NNS_G3dBindMdlTex
	and r7, r7, r0
	mov r0, r8
	mov r1, sb
	bl NNS_G3dBindMdlPltt
	ldrb r1, [sl, #9]
	add r6, r6, #1
	and r7, r7, r0
	cmp r6, r1
	blo _020BED2C
_020BEDA8:
	mov r0, r7
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNS_G3dBindMdlSet

	arm_func_start NNS_G3dReleaseMdlSet
NNS_G3dReleaseMdlSet: ; 0x020BEDB0
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r6, r0
	ldrb r0, [r6, #9]
	mov r5, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	add r4, r6, #8
	mov r8, r5
	mov r7, r5
_020BEDD4:
	cmp r6, #0
	beq _020BEE1C
	cmp r4, #0
	beq _020BEE08
	ldrb r0, [r6, #9]
	cmp r5, r0
	bhs _020BEE08
	ldrh r1, [r6, #0xe]
	ldrh r0, [r4, r1]
	add r1, r4, r1
	add r1, r1, #4
	mla r1, r0, r5, r1
	b _020BEE0C
_020BEE08:
	mov r1, r8
_020BEE0C:
	cmp r1, #0
	ldrne r0, [r1]
	addne sb, r6, r0
	bne _020BEE20
_020BEE1C:
	mov sb, r7
_020BEE20:
	mov r0, sb
	bl NNS_G3dReleaseMdlTex
	mov r0, sb
	bl NNS_G3dReleaseMdlPltt
	ldrb r0, [r6, #9]
	add r5, r5, #1
	cmp r5, r0
	blo _020BEDD4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end NNS_G3dReleaseMdlSet

	arm_func_start NNS_G3dGlbInit
NNS_G3dGlbInit: ; 0x020BEE44
	stmdb sp!, {r3, lr}
	ldr r2, _020BEF58 ; =0x17101610
	ldr r1, _020BEF5C ; =NNS_G3dGlb
	mov r0, #0
	str r2, [r1]
	str r0, [r1, #4]
	mov r2, #2
	ldr r0, _020BEF60 ; =0x32323232
	str r2, [r1, #0x48]
	ldr r2, _020BEF64 ; =0x60293130
	str r0, [r1, #0x7c]
	ldr r0, _020BEF68 ; =0x33333333
	str r2, [r1, #0x90]
	ldr r2, _020BEF6C ; =0x002A1B19
	str r0, [r1, #0xa4]
	ldr r0, _020BEF70 ; =NNS_G3dGlb + 0x4C
	str r2, [r1, #0xb8]
	bl MTX_Identity43_
	ldr r0, _020BEF74 ; =NNS_G3dGlb + 0x8
	bl MTX_Identity44_
	ldr ip, _020BEF78 ; =0x40000200
	ldr r3, _020BEF7C ; =0xC0080000
	ldr r2, _020BEF80 ; =0x2D8B62D8
	ldr r1, _020BEF5C ; =NNS_G3dGlb
	sub r0, ip, #0xc0000001
	str r2, [r1, #0x80]
	str ip, [r1, #0x84]
	str r0, [r1, #0x88]
	ldr r2, _020BEF84 ; =0x4210C210
	str r3, [r1, #0x8c]
	str r2, [r1, #0x94]
	ldr r0, _020BEF88 ; =0x001F008F
	str r2, [r1, #0x98]
	str r0, [r1, #0x9c]
	sub r2, r3, #0x90000
	ldr r0, _020BEF8C ; =0x00007FFF
	str r2, [r1, #0xa0]
	str r0, [r1, #0xa8]
	mov r2, #0x4000001f
	ldr r0, _020BEF90 ; =0x800003E0
	str r2, [r1, #0xac]
	str r0, [r1, #0xb0]
	rsb r0, ip, #0x7e00
	str r0, [r1, #0xb4]
	mov r2, #0
	str r2, [r1, #0xe0]
	str r2, [r1, #0xe4]
	ldr r0, _020BEF94 ; =NNS_G3dGlb + 0xBC
	str r2, [r1, #0xe8]
	bl MTX_Identity33_
	mov r1, #0
	ldr r0, _020BEF5C ; =NNS_G3dGlb
	mov r2, #0x1000
	str r2, [r0, #0xec]
	str r2, [r0, #0xf0]
	str r2, [r0, #0xf4]
	str r1, [r0, #0xf8]
	str r1, [r0, #0xfc]
	str r1, [r0, #0x248]
	str r1, [r0, #0x244]
	str r1, [r0, #0x240]
	str r1, [r0, #0x254]
	str r1, [r0, #0x24c]
	str r2, [r0, #0x250]
	str r1, [r0, #0x25c]
	str r1, [r0, #0x258]
	sub r1, r1, #0x1000
	str r1, [r0, #0x260]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BEF58: .word 0x17101610
_020BEF5C: .word NNS_G3dGlb
_020BEF60: .word 0x32323232
_020BEF64: .word 0x60293130
_020BEF68: .word 0x33333333
_020BEF6C: .word 0x002A1B19
_020BEF70: .word NNS_G3dGlb + 0x4C
_020BEF74: .word NNS_G3dGlb + 0x8
_020BEF78: .word 0x40000200
_020BEF7C: .word 0xC0080000
_020BEF80: .word 0x2D8B62D8
_020BEF84: .word 0x4210C210
_020BEF88: .word 0x001F008F
_020BEF8C: .word 0x00007FFF
_020BEF90: .word 0x800003E0
_020BEF94: .word NNS_G3dGlb + 0xBC
	arm_func_end NNS_G3dGlbInit

	arm_func_start NNS_G3dGlbFlushP
NNS_G3dGlbFlushP: ; 0x020BEF98
	stmdb sp!, {r3, lr}
	ldr r0, _020BEFCC ; =NNS_G3dGlb
	ldr r1, _020BEFD0 ; =NNS_G3dGlb
	ldr r0, [r0]
	add r1, r1, #4
	mov r2, #0x3e
	bl NNS_G3dGeBufferOP_N
	ldr r0, _020BEFCC ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	bic r1, r1, #1
	bic r1, r1, #2
	str r1, [r0, #0xfc]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BEFCC: .word NNS_G3dGlb
_020BEFD0: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbFlushP
