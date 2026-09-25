	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNS_G3dBindMdlSet
	.public NNS_G3dGlbGetInvV
	.public NNS_G3dGlbGetInvVP
	.public NNS_G3dGlbGetInvWV
	.public NNS_G3dGlbGetViewPort
	.public NNS_G3dGlbGetWV
	.public NNS_G3dGlbInit
	.public NNS_G3dPlttGetRequiredSize
	.public NNS_G3dPlttLoad
	.public NNS_G3dPlttSetPlttKey
	.public NNS_G3dTex4x4GetRequiredSize
	.public NNS_G3dTexGetRequiredSize
	.public NNS_G3dTexLoad
	.public NNS_G3dTexSetTexKey
	.public NNSi_G3dAnmBlendJnt
	.public NNSi_G3dAnmBlendMat
	.public NNSi_G3dAnmBlendVis
	.public NNSi_G3dFuncSbc_MAT_InternalDefault
	.public NNSi_G3dFuncSbc_MTX
	.public NNSi_G3dFuncSbc_NODE
	.public NNSi_G3dFuncSbc_NOP
	.public NNSi_G3dFuncSbc_RET
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

.text
	arm_func_start NNS_G3dRenderObjAddAnmObj
NNS_G3dRenderObjAddAnmObj: ; 0x020BE294
	stmdb sp!, {r3, r4, r5, lr}
	movs r4, r1
	mov r5, r0
	ldrne r0, [r4, #8]
	cmpne r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrb r0, [r0]
	cmp r0, #0x4a
	beq _020BE2E4
	cmp r0, #0x4d
	beq _020BE2CC
	cmp r0, #0x56
	beq _020BE2FC
	ldmia sp!, {r3, r4, r5, pc}
_020BE2CC:
	add r0, r5, #0x3c
	bl updateHintVec_
	mov r1, r4
	add r0, r5, #8
	bl addLink_
	ldmia sp!, {r3, r4, r5, pc}
_020BE2E4:
	add r0, r5, #0x44
	bl updateHintVec_
	mov r1, r4
	add r0, r5, #0x10
	bl addLink_
	ldmia sp!, {r3, r4, r5, pc}
_020BE2FC:
	add r0, r5, #0x4c
	bl updateHintVec_
	mov r1, r4
	add r0, r5, #0x18
	bl addLink_
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNS_G3dRenderObjAddAnmObj

	arm_func_start removeLink_
removeLink_: ; 0x020BE314
	ldr r3, [r0]
	cmp r3, #0
	moveq r0, #0
	bxeq lr
	cmp r3, r1
	bne _020BE344
	ldr r3, [r3, #0x10]
	mov r2, #0
	str r3, [r0]
	str r2, [r1, #0x10]
	mov r0, #1
	bx lr
_020BE344:
	ldr r2, [r3, #0x10]
	cmp r2, #0
	beq _020BE380
_020BE350:
	cmp r2, r1
	bne _020BE370
	ldr r1, [r2, #0x10]
	mov r0, #0
	str r1, [r3, #0x10]
	str r0, [r2, #0x10]
	mov r0, #1
	bx lr
_020BE370:
	mov r3, r2
	ldr r2, [r2, #0x10]
	cmp r2, #0
	bne _020BE350
_020BE380:
	mov r0, #0
	bx lr
	arm_func_end removeLink_

	arm_func_start NNS_G3dRenderObjRemoveAnmObj
NNS_G3dRenderObjRemoveAnmObj: ; 0x020BE388
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	add r0, r5, #8
	mov r4, r1
	bl removeLink_
	cmp r0, #0
	bne _020BE3CC
	mov r1, r4
	add r0, r5, #0x10
	bl removeLink_
	cmp r0, #0
	bne _020BE3CC
	mov r1, r4
	add r0, r5, #0x18
	bl removeLink_
	cmp r0, #0
	ldmeqia sp!, {r3, r4, r5, pc}
_020BE3CC:
	ldr r0, [r5]
	orr r0, r0, #0x10
	str r0, [r5]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNS_G3dRenderObjRemoveAnmObj

	arm_func_start NNS_G3dTexGetRequiredSize
NNS_G3dTexGetRequiredSize: ; 0x020BE3DC
	cmp r0, #0
	ldrneh r0, [r0, #0xc]
	movne r0, r0, lsl #3
	moveq r0, #0
	bx lr
	arm_func_end NNS_G3dTexGetRequiredSize

	arm_func_start NNS_G3dTex4x4GetRequiredSize
NNS_G3dTex4x4GetRequiredSize: ; 0x020BE3F0
	cmp r0, #0
	ldrneh r0, [r0, #0x1c]
	movne r0, r0, lsl #3
	moveq r0, #0
	bx lr
	arm_func_end NNS_G3dTex4x4GetRequiredSize

	arm_func_start NNS_G3dTexSetTexKey
NNS_G3dTexSetTexKey: ; 0x020BE404
	cmp r1, #0
	strne r1, [r0, #8]
	cmp r2, #0
	strne r2, [r0, #0x18]
	bx lr
	arm_func_end NNS_G3dTexSetTexKey

	arm_func_start NNS_G3dTexLoad
NNS_G3dTexLoad: ; 0x020BE418
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	movs r8, r1
	mov sb, r0
	beq _020BE42C
	bl GX_BeginLoadTex
_020BE42C:
	ldrh r0, [sb, #0xc]
	movs r2, r0, lsl #3
	beq _020BE45C
	ldr r0, [sb, #8]
	ldr r3, [sb, #0x14]
	mov r1, r0, lsl #0x10
	add r0, sb, r3
	mov r1, r1, lsr #0xd
	bl GX_LoadTex
	ldrh r0, [sb, #0x10]
	orr r0, r0, #1
	strh r0, [sb, #0x10]
_020BE45C:
	ldrh r0, [sb, #0x1c]
	movs r6, r0, lsl #3
	beq _020BE4BC
	ldr r1, [sb, #0x18]
	ldr r0, [sb, #0x24]
	mov r4, r1, lsl #0x10
	mov r7, r4, lsr #0xd
	mov r1, r7
	mov r2, r6
	add r0, sb, r0
	ldr r5, [sb, #0x28]
	bl GX_LoadTex
	ldr r0, _020BE4CC ; =0x0001FFFF
	and r1, r7, #0x40000
	and r0, r0, r4, lsr #13
	mov r0, r0, lsr #1
	add r2, r0, #0x20000
	add r0, sb, r5
	add r1, r2, r1, lsr #2
	mov r2, r6, lsr #1
	bl GX_LoadTex
	ldrh r0, [sb, #0x20]
	orr r0, r0, #1
	strh r0, [sb, #0x20]
_020BE4BC:
	cmp r8, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	bl GX_EndLoadTex
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	.align 2, 0
_020BE4CC: .word 0x0001FFFF
	arm_func_end NNS_G3dTexLoad

	arm_func_start NNS_G3dTexReleaseTexKey
NNS_G3dTexReleaseTexKey: ; 0x020BE4D0
	cmp r1, #0
	beq _020BE4F4
	ldrh ip, [r0, #0x10]
	mov r3, #0
	bic ip, ip, #1
	strh ip, [r0, #0x10]
	ldr ip, [r0, #8]
	str ip, [r1]
	str r3, [r0, #8]
_020BE4F4:
	cmp r2, #0
	bxeq lr
	ldrh r3, [r0, #0x20]
	mov r1, #0
	bic r3, r3, #1
	strh r3, [r0, #0x20]
	ldr r3, [r0, #0x18]
	str r3, [r2]
	str r1, [r0, #0x18]
	bx lr
	arm_func_end NNS_G3dTexReleaseTexKey

	arm_func_start NNS_G3dPlttGetRequiredSize
NNS_G3dPlttGetRequiredSize: ; 0x020BE51C
	cmp r0, #0
	ldrneh r0, [r0, #0x30]
	movne r0, r0, lsl #3
	moveq r0, #0
	bx lr
	arm_func_end NNS_G3dPlttGetRequiredSize

	arm_func_start NNS_G3dPlttSetPlttKey
NNS_G3dPlttSetPlttKey: ; 0x020BE530
	str r1, [r0, #0x2c]
	bx lr
	arm_func_end NNS_G3dPlttSetPlttKey

	arm_func_start NNS_G3dPlttLoad
NNS_G3dPlttLoad: ; 0x020BE538
	stmdb sp!, {r3, r4, r5, lr}
	movs r5, r1
	mov r4, r0
	beq _020BE54C
	bl GX_BeginLoadTexPltt
_020BE54C:
	ldr r0, [r4, #0x2c]
	ldrh r2, [r4, #0x30]
	mov r1, r0, lsl #0x10
	ldr r3, [r4, #0x38]
	mov r1, r1, lsr #0xd
	add r0, r4, r3
	mov r2, r2, lsl #3
	bl GX_LoadTexPltt
	ldrh r0, [r4, #0x32]
	cmp r5, #0
	orr r0, r0, #1
	strh r0, [r4, #0x32]
	ldmeqia sp!, {r3, r4, r5, pc}
	bl GX_EndLoadTexPltt
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNS_G3dPlttLoad

	arm_func_start NNS_G3dPlttReleasePlttKey
NNS_G3dPlttReleasePlttKey: ; 0x020BE588
	ldrh r2, [r0, #0x32]
	mov r1, #0
	bic r2, r2, #1
	strh r2, [r0, #0x32]
	ldr r2, [r0, #0x2c]
	str r1, [r0, #0x2c]
	mov r0, r2
	bx lr
	arm_func_end NNS_G3dPlttReleasePlttKey

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

	arm_func_start NNS_G3dGlbSetBaseTrans
NNS_G3dGlbSetBaseTrans: ; 0x020BEFD4
	cmp r0, #0
	bxeq lr
	ldr r3, _020BEFFC ; =NNS_G3dGlb + 0xE0
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, _020BF000 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	bic r1, r1, #0xa4
	str r1, [r0, #0xfc]
	bx lr
	.align 2, 0
_020BEFFC: .word NNS_G3dGlb + 0xE0
_020BF000: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbSetBaseTrans

	arm_func_start NNS_G3dGlbSetBaseScale
NNS_G3dGlbSetBaseScale: ; 0x020BF004
	cmp r0, #0
	bxeq lr
	ldr r3, _020BF02C ; =NNS_G3dGlb + 0xEC
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
	ldr r0, _020BF030 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	bic r1, r1, #0xa4
	str r1, [r0, #0xfc]
	bx lr
	.align 2, 0
_020BF02C: .word NNS_G3dGlb + 0xEC
_020BF030: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbSetBaseScale

	arm_func_start NNS_G3dGlbLightVector
NNS_G3dGlbLightVector: ; 0x020BF034
	stmdb sp!, {r3, lr}
	ldr ip, _020BF068 ; =0x000003FF
	mov r2, r2, asr #3
	mov lr, r3, asr #3
	and r3, ip, r1, asr #3
	mov r1, r2, lsl #0x16
	mov r2, lr, lsl #0x16
	orr r1, r3, r1, lsr #12
	orr r2, r1, r2, lsr #2
	ldr r1, _020BF06C ; =NNS_G3dGlb + 0x80
	orr r2, r2, r0, lsl #30
	str r2, [r1, r0, lsl #2]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF068: .word 0x000003FF
_020BF06C: .word NNS_G3dGlb + 0x80
	arm_func_end NNS_G3dGlbLightVector

	arm_func_start NNS_G3dGlbLightColor
NNS_G3dGlbLightColor: ; 0x020BF070
	ldr r2, _020BF080 ; =NNS_G3dGlb + 0xA8
	orr r1, r1, r0, lsl #30
	str r1, [r2, r0, lsl #2]
	bx lr
	.align 2, 0
_020BF080: .word NNS_G3dGlb + 0xA8
	arm_func_end NNS_G3dGlbLightColor

	arm_func_start NNS_G3dGlbMaterialColorDiffAmb
NNS_G3dGlbMaterialColorDiffAmb: ; 0x020BF084
	cmp r2, #0
	movne r2, #1
	orr r1, r0, r1, lsl #16
	moveq r2, #0
	ldr r0, _020BF0A4 ; =NNS_G3dGlb
	orr r1, r1, r2, lsl #15
	str r1, [r0, #0x94]
	bx lr
	.align 2, 0
_020BF0A4: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbMaterialColorDiffAmb

	arm_func_start NNS_G3dGlbMaterialColorSpecEmi
NNS_G3dGlbMaterialColorSpecEmi: ; 0x020BF0A8
	cmp r2, #0
	movne r2, #1
	orr r1, r0, r1, lsl #16
	moveq r2, #0
	ldr r0, _020BF0C8 ; =NNS_G3dGlb
	orr r1, r1, r2, lsl #15
	str r1, [r0, #0x98]
	bx lr
	.align 2, 0
_020BF0C8: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbMaterialColorSpecEmi

	arm_func_start NNS_G3dGlbPolygonAttr
NNS_G3dGlbPolygonAttr: ; 0x020BF0CC
	orr r0, r0, r1, lsl #4
	ldr r1, [sp, #4]
	orr r0, r0, r2, lsl #6
	orr r0, r1, r0
	ldr r2, [sp]
	orr r1, r0, r3, lsl #24
	ldr r0, _020BF0F4 ; =NNS_G3dGlb
	orr r1, r1, r2, lsl #16
	str r1, [r0, #0x9c]
	bx lr
	.align 2, 0
_020BF0F4: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbPolygonAttr

	arm_func_start NNS_G3dGlbGetInvV
NNS_G3dGlbGetInvV: ; 0x020BF0F8
	stmdb sp!, {r3, lr}
	ldr r0, _020BF130 ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #8
	bne _020BF128
	ldr r0, _020BF134 ; =NNS_G3dGlb + 0x4C
	ldr r1, _020BF138 ; =NNS_G3dGlb + 0x100
	bl MTX_Inverse43
	ldr r0, _020BF130 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	orr r1, r1, #8
	str r1, [r0, #0xfc]
_020BF128:
	ldr r0, _020BF138 ; =NNS_G3dGlb + 0x100
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF130: .word NNS_G3dGlb
_020BF134: .word NNS_G3dGlb + 0x4C
_020BF138: .word NNS_G3dGlb + 0x100
	arm_func_end NNS_G3dGlbGetInvV

	arm_func_start mtx_inverse44
mtx_inverse44: ; 0x020BF13C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x48
	str r1, [sp]
	add r1, sp, #8
	bl MI_Copy64B
	ldr r0, [sp]
	bl MTX_Identity44_
	mov fp, #0
_020BF15C:
	mov r3, fp
	mov r1, fp
	cmp fp, #4
	mov r2, #0
	bge _020BF19C
	add r0, sp, #8
	add r0, r0, fp, lsl #2
_020BF178:
	ldr r4, [r0, r1, lsl #4]
	cmp r4, #0
	rsblt r4, r4, #0
	cmp r4, r2
	movgt r3, r1
	add r1, r1, #1
	movgt r2, r4
	cmp r1, #4
	blt _020BF178
_020BF19C:
	cmp r2, #0
	addeq sp, sp, #0x48
	mvneq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r3, fp
	beq _020BF1FC
	ldr r1, [sp]
	add r2, sp, #8
	mov r0, #0
	add r4, r1, fp, lsl #4
	add r6, r2, fp, lsl #4
	add r5, r2, r3, lsl #4
	add r2, r1, r3, lsl #4
_020BF1D0:
	ldr r3, [r6, r0, lsl #2]
	ldr r1, [r5, r0, lsl #2]
	str r1, [r6, r0, lsl #2]
	str r3, [r5, r0, lsl #2]
	ldr r3, [r4, r0, lsl #2]
	ldr r1, [r2, r0, lsl #2]
	str r1, [r4, r0, lsl #2]
	str r3, [r2, r0, lsl #2]
	add r0, r0, #1
	cmp r0, #4
	blt _020BF1D0
_020BF1FC:
	add r0, sp, #8
	add r0, r0, fp, lsl #4
	ldr r0, [r0, fp, lsl #2]
	bl FX_InvFx64c
	ldr r2, [sp]
	mov r3, #0
	mov lr, #0x80000000
	add r4, sp, #8
	add sb, r2, fp, lsl #4
	add r2, r4, fp, lsl #4
	mov r4, r3
	mov r6, r3
	mov r5, lr
_020BF230:
	ldr r8, [r2, r3, lsl #2]
	mov r7, r8, asr #0x1f
	umull ip, sl, r0, r8
	adds ip, ip, lr
	mla sl, r0, r7, sl
	mla sl, r1, r8, sl
	adc r7, sl, r4
	str r7, [r2, r3, lsl #2]
	ldr r8, [sb, r3, lsl #2]
	mov r7, r8, asr #0x1f
	umull ip, sl, r0, r8
	adds ip, ip, r5
	mla sl, r0, r7, sl
	mla sl, r1, r8, sl
	adc r7, sl, r6
	str r7, [sb, r3, lsl #2]
	add r3, r3, #1
	cmp r3, #4
	blt _020BF230
	ldr r0, [sp]
	add r1, sp, #8
	add r6, r0, fp, lsl #4
	add r0, sp, #8
	add r1, r1, fp, lsl #2
	mov ip, #0
	add r0, r0, fp, lsl #4
	str r1, [sp, #4]
_020BF29C:
	cmp ip, fp
	beq _020BF320
	ldr r1, [sp, #4]
	ldr r4, [sp]
	ldr r3, [r1, ip, lsl #4]
	add r1, sp, #8
	mov lr, #0
	mov r2, r3, asr #0x1f
	add r1, r1, ip, lsl #4
	add sl, r4, ip, lsl #4
_020BF2C4:
	ldr r5, [r0, lr, lsl #2]
	ldr sb, [r1, lr, lsl #2]
	mov r4, r5, asr #0x1f
	umull r8, r7, r3, r5
	mla r7, r3, r4, r7
	mla r7, r2, r5, r7
	mov r4, r8, lsr #0xc
	orr r4, r4, r7, lsl #20
	sub r4, sb, r4
	str r4, [r1, lr, lsl #2]
	ldr sb, [sl, lr, lsl #2]
	ldr r5, [r6, lr, lsl #2]
	mov r4, r5, asr #0x1f
	umull r8, r7, r3, r5
	mla r7, r3, r4, r7
	mla r7, r2, r5, r7
	mov r4, r8, lsr #0xc
	orr r4, r4, r7, lsl #20
	sub r4, sb, r4
	str r4, [sl, lr, lsl #2]
	add lr, lr, #1
	cmp lr, #4
	blt _020BF2C4
_020BF320:
	add ip, ip, #1
	cmp ip, #4
	blt _020BF29C
	add fp, fp, #1
	cmp fp, #4
	blt _020BF15C
	mov r0, #0
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end mtx_inverse44

	arm_func_start NNS_G3dGlbGetInvP
NNS_G3dGlbGetInvP: ; 0x020BF344
	stmdb sp!, {r3, lr}
	ldr r0, _020BF37C ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #0x10
	bne _020BF374
	ldr r0, _020BF380 ; =NNS_G3dGlb + 0x8
	ldr r1, _020BF384 ; =NNS_G3dGlb + 0x1C0
	bl mtx_inverse44
	ldr r0, _020BF37C ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	orr r1, r1, #0x10
	str r1, [r0, #0xfc]
_020BF374:
	ldr r0, _020BF384 ; =NNS_G3dGlb + 0x1C0
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF37C: .word NNS_G3dGlb
_020BF380: .word NNS_G3dGlb + 0x8
_020BF384: .word NNS_G3dGlb + 0x1C0
	arm_func_end NNS_G3dGlbGetInvP

	arm_func_start calcSrtCameraMtx_
calcSrtCameraMtx_: ; 0x020BF388
	stmdb sp!, {r3, lr}
	ldr r0, _020BF3CC ; =NNS_G3dGlb + 0xBC
	ldr r1, _020BF3D0 ; =NNS_G3dGlb + 0x4C
	ldr r2, _020BF3D4 ; =NNS_G3dGlb + 0x130
	bl MTX_Concat43
	ldr r3, _020BF3D8 ; =NNS_G3dGlb
	ldr r0, _020BF3D4 ; =NNS_G3dGlb + 0x130
	ldr r2, [r3, #0xf4]
	mov r1, r0
	str r2, [sp]
	ldr r2, [r3, #0xec]
	ldr r3, [r3, #0xf0]
	bl MTX_ScaleApply43
	ldr r0, _020BF3D4 ; =NNS_G3dGlb + 0x130
	ldr r1, _020BF3DC ; =NNS_G3dGlb + 0x160
	bl MTX_Inverse43
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF3CC: .word NNS_G3dGlb + 0xBC
_020BF3D0: .word NNS_G3dGlb + 0x4C
_020BF3D4: .word NNS_G3dGlb + 0x130
_020BF3D8: .word NNS_G3dGlb
_020BF3DC: .word NNS_G3dGlb + 0x160
	arm_func_end calcSrtCameraMtx_

	arm_func_start NNS_G3dGlbGetWV
NNS_G3dGlbGetWV: ; 0x020BF3E0
	stmdb sp!, {r3, lr}
	ldr r0, _020BF410 ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #0x80
	bne _020BF408
	bl calcSrtCameraMtx_
	ldr r0, _020BF410 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	orr r1, r1, #0x80
	str r1, [r0, #0xfc]
_020BF408:
	ldr r0, _020BF414 ; =NNS_G3dGlb + 0x130
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF410: .word NNS_G3dGlb
_020BF414: .word NNS_G3dGlb + 0x130
	arm_func_end NNS_G3dGlbGetWV

	arm_func_start NNS_G3dGlbGetInvWV
NNS_G3dGlbGetInvWV: ; 0x020BF418
	stmdb sp!, {r3, lr}
	ldr r0, _020BF448 ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #0x80
	bne _020BF440
	bl calcSrtCameraMtx_
	ldr r0, _020BF448 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	orr r1, r1, #0x80
	str r1, [r0, #0xfc]
_020BF440:
	ldr r0, _020BF44C ; =NNS_G3dGlb + 0x160
	ldmia sp!, {r3, pc}
	.align 2, 0
_020BF448: .word NNS_G3dGlb
_020BF44C: .word NNS_G3dGlb + 0x160
	arm_func_end NNS_G3dGlbGetInvWV

	arm_func_start NNS_G3dGlbGetInvVP
NNS_G3dGlbGetInvVP: ; 0x020BF450
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x40
	ldr r0, _020BF4B0 ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #0x40
	bne _020BF4A4
	bl NNS_G3dGlbGetInvV
	mov r5, r0
	bl NNS_G3dGlbGetInvP
	mov r4, r0
	add r1, sp, #0
	mov r0, r5
	bl MTX_Copy43To44_
	ldr r2, _020BF4B4 ; =NNS_G3dGlb + 0x200
	add r1, sp, #0
	mov r0, r4
	bl MTX_Concat44
	ldr r0, _020BF4B0 ; =NNS_G3dGlb
	ldr r1, [r0, #0xfc]
	orr r1, r1, #0x40
	str r1, [r0, #0xfc]
_020BF4A4:
	ldr r0, _020BF4B4 ; =NNS_G3dGlb + 0x200
	add sp, sp, #0x40
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BF4B0: .word NNS_G3dGlb
_020BF4B4: .word NNS_G3dGlb + 0x200
	arm_func_end NNS_G3dGlbGetInvVP

	arm_func_start NNS_G3dGlbGetViewPort
NNS_G3dGlbGetViewPort: ; 0x020BF4B8
	cmp r0, #0
	beq _020BF4D0
	ldr ip, _020BF528 ; =NNS_G3dGlb
	ldr ip, [ip, #0xa0]
	and ip, ip, #0xff
	str ip, [r0]
_020BF4D0:
	cmp r1, #0
	beq _020BF4EC
	ldr r0, _020BF528 ; =NNS_G3dGlb
	ldr r0, [r0, #0xa0]
	mov r0, r0, lsr #8
	and r0, r0, #0xff
	str r0, [r1]
_020BF4EC:
	cmp r2, #0
	beq _020BF508
	ldr r0, _020BF528 ; =NNS_G3dGlb
	ldr r0, [r0, #0xa0]
	mov r0, r0, lsr #0x10
	and r0, r0, #0xff
	str r0, [r2]
_020BF508:
	cmp r3, #0
	bxeq lr
	ldr r0, _020BF528 ; =NNS_G3dGlb
	ldr r0, [r0, #0xa0]
	mov r0, r0, lsr #0x18
	and r0, r0, #0xff
	str r0, [r3]
	bx lr
	.align 2, 0
_020BF528: .word NNS_G3dGlb
	arm_func_end NNS_G3dGlbGetViewPort

	arm_func_start NNSi_G3dAnmBlendMat
NNSi_G3dAnmBlendMat: ; 0x020BF52C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r0
	movs r5, r1
	mov r4, r2
	mov r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r7, #1
_020BF548:
	ldrb r1, [r5, #0x19]
	cmp r4, r1
	bhs _020BF588
	add r1, r5, r4, lsl #1
	ldrh r2, [r1, #0x1a]
	and r1, r2, #0x300
	cmp r1, #0x100
	bne _020BF588
	ldr r3, [r5, #0xc]
	cmp r3, #0
	beq _020BF588
	mov r0, r6
	mov r1, r5
	and r2, r2, #0xff
	blx r3
	mov r0, r7
_020BF588:
	ldr r5, [r5, #0x10]
	cmp r5, #0
	bne _020BF548
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNSi_G3dAnmBlendMat

	arm_func_start blendScaleVec_
blendScaleVec_: ; 0x020BF598
	cmp r3, #0
	beq _020BF5C8
	ldr r1, [r0]
	add r1, r1, r2
	str r1, [r0]
	ldr r1, [r0, #4]
	add r1, r1, r2
	str r1, [r0, #4]
	ldr r1, [r0, #8]
	add r1, r1, r2
	str r1, [r0, #8]
	bx lr
_020BF5C8:
	ldr r3, [r1]
	ldr ip, [r0]
	mul r3, r2, r3
	add r3, ip, r3, asr #12
	str r3, [r0]
	ldr r3, [r1, #4]
	ldr ip, [r0, #4]
	mul r3, r2, r3
	add r3, ip, r3, asr #12
	str r3, [r0, #4]
	ldr r1, [r1, #8]
	ldr r3, [r0, #8]
	mul r1, r2, r1
	add r1, r3, r1, asr #12
	str r1, [r0, #8]
	bx lr
	arm_func_end blendScaleVec_

	arm_func_start NNSi_G3dAnmBlendJnt
NNSi_G3dAnmBlendJnt: ; 0x020BF608
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x70
	movs sb, r1
	mov sl, r0
	mov r8, r2
	addeq sp, sp, #0x70
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r2, [sb, #0x10]
	cmp r2, #0
	bne _020BF694
	ldrb r2, [sb, #0x19]
	cmp r8, r2
	bhs _020BF688
	add r2, sb, r8, lsl #1
	ldrh r4, [r2, #0x1a]
	and r2, r4, #0x300
	cmp r2, #0x100
	bne _020BF67C
	ldr r3, [sb, #0xc]
	cmp r3, #0
	addeq sp, sp, #0x70
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	and r2, r4, #0xff
	blx r3
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BF67C:
	add sp, sp, #0x70
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BF688:
	add sp, sp, #0x70
	mov r0, #0
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BF694:
	mov r5, #0
	mov r3, r5
	mov r6, r5
	mov r2, sb
_020BF6A4:
	ldrb r0, [r2, #0x19]
	cmp r8, r0
	bhs _020BF6E4
	add r0, r2, r8, lsl #1
	ldrh r0, [r0, #0x1a]
	and r0, r0, #0x300
	cmp r0, #0x100
	bne _020BF6E4
	ldr r0, [r2, #4]
	cmp r0, #0x1000
	addgt r5, r5, #0x1000
	bgt _020BF6DC
	cmp r0, #0
	addgt r5, r5, r0
_020BF6DC:
	mov r1, r2
	add r3, r3, #1
_020BF6E4:
	ldr r2, [r2, #0x10]
	cmp r2, #0
	bne _020BF6A4
	cmp r5, #0
	addeq sp, sp, #0x70
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	cmp r3, #1
	bne _020BF73C
	ldr r3, [r1, #0xc]
	add r0, r1, r8, lsl #1
	cmp r3, #0
	ldrh r2, [r0, #0x1a]
	addeq sp, sp, #0x70
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r0, sl
	and r2, r2, #0xff
	blx r3
	add sp, sp, #0x70
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020BF73C:
	mov r1, sl
	mov r0, #0
	mov r2, #0x58
	bl MIi_CpuClearFast
	mvn r0, #0
	str r0, [sl]
	add fp, sp, #0x40
	add r4, sp, #0x18
_020BF75C:
	ldrb r0, [sb, #0x19]
	cmp r8, r0
	bhs _020BF940
	add r0, sb, r8, lsl #1
	ldrh r1, [r0, #0x1a]
	and r0, r1, #0x300
	cmp r0, #0x100
	bne _020BF940
	ldr r0, [sb, #4]
	cmp r0, #0
	ble _020BF940
	ldr r3, [sb, #0xc]
	cmp r3, #0
	beq _020BF940
	and r2, r1, #0xff
	mov r0, r4
	mov r1, sb
	blx r3
	cmp r6, #0
	bne _020BF7CC
	mov r0, fp
	add r1, sp, #0xc
	mov r2, #0xc
	bl MIi_CpuCopy32
	add r0, sp, #0x58
	add r1, sp, #0
	mov r2, #0xc
	bl MIi_CpuCopy32
_020BF7CC:
	cmp r5, #0x1000
	ldreq r7, [sb, #4]
	beq _020BF7E8
	ldr r0, [sb, #4]
	mov r1, r5
	bl FX_Div
	mov r7, r0
_020BF7E8:
	ldr r3, [sp, #0x18]
	add r0, sl, #4
	add r1, sp, #0x1c
	mov r2, r7
	and r3, r3, #1
	bl blendScaleVec_
	ldr r3, [sp, #0x18]
	add r0, sl, #0x10
	add r1, sp, #0x28
	mov r2, r7
	and r3, r3, #8
	bl blendScaleVec_
	ldr r3, [sp, #0x18]
	add r0, sl, #0x1c
	add r1, sp, #0x34
	mov r2, r7
	and r3, r3, #0x10
	bl blendScaleVec_
	ldr r0, [sp, #0x18]
	tst r0, #4
	bne _020BF890
	ldr r2, [sl, #0x4c]
	ldr r0, [sp, #0x64]
	smull r1, r0, r7, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	add r0, r2, r1
	str r0, [sl, #0x4c]
	ldr r2, [sl, #0x50]
	ldr r0, [sp, #0x68]
	smull r1, r0, r7, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	add r0, r2, r1
	str r0, [sl, #0x50]
	ldr r2, [sl, #0x54]
	ldr r0, [sp, #0x6c]
	smull r1, r0, r7, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	add r0, r2, r1
	str r0, [sl, #0x54]
_020BF890:
	ldr r0, [sp, #0x18]
	tst r0, #2
	bne _020BF918
	ldr r1, [sl, #0x28]
	ldr r0, [sp, #0x40]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x28]
	ldr r1, [sl, #0x2c]
	ldr r0, [sp, #0x44]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x2c]
	ldr r1, [sl, #0x30]
	ldr r0, [sp, #0x48]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x30]
	ldr r1, [sl, #0x34]
	ldr r0, [sp, #0x4c]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x34]
	ldr r1, [sl, #0x38]
	ldr r0, [sp, #0x50]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x38]
	ldr r1, [sl, #0x3c]
	ldr r0, [sp, #0x54]
	mul r0, r7, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x3c]
	b _020BF930
_020BF918:
	ldr r0, [sl, #0x28]
	add r0, r0, r7
	str r0, [sl, #0x28]
	ldr r0, [sl, #0x38]
	add r0, r0, r7
	str r0, [sl, #0x38]
_020BF930:
	ldr r1, [sl]
	ldr r0, [sp, #0x18]
	and r0, r1, r0
	str r0, [sl]
_020BF940:
	ldr sb, [sb, #0x10]
	add r6, r6, #1
	cmp sb, #0
	bne _020BF75C
	add r0, sl, #0x28
	add r1, sl, #0x34
	add r2, sl, #0x40
	bl VEC_CrossProduct
	ldr r0, [sl, #0x28]
	cmp r0, #0
	ldreq r0, [sl, #0x2c]
	cmpeq r0, #0
	ldreq r0, [sl, #0x30]
	cmpeq r0, #0
	bne _020BF990
	add r0, sp, #0xc
	add r1, sl, #0x28
	mov r2, #0xc
	bl MIi_CpuCopy32
	b _020BF99C
_020BF990:
	add r0, sl, #0x28
	mov r1, r0
	bl VEC_Normalize
_020BF99C:
	ldr r0, [sl, #0x40]
	cmp r0, #0
	ldreq r0, [sl, #0x44]
	cmpeq r0, #0
	ldreq r0, [sl, #0x48]
	cmpeq r0, #0
	bne _020BF9CC
	add r0, sp, #0
	add r1, sl, #0x40
	mov r2, #0xc
	bl MIi_CpuCopy32
	b _020BF9D8
_020BF9CC:
	add r0, sl, #0x40
	mov r1, r0
	bl VEC_Normalize
_020BF9D8:
	add r0, sl, #0x40
	add r1, sl, #0x28
	add r2, sl, #0x34
	bl VEC_CrossProduct
	mov r0, #1
	add sp, sp, #0x70
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end NNSi_G3dAnmBlendJnt

	arm_func_start NNSi_G3dAnmBlendVis
NNSi_G3dAnmBlendVis: ; 0x020BF9F4
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #4
	mov r7, r0
	mov r0, #0
	mov r6, r1
	mov r5, r2
	str r0, [r7]
	add r4, sp, #0
	mov r8, #1
_020BFA18:
	ldrb r1, [r6, #0x19]
	cmp r5, r1
	bhs _020BFA68
	add r1, r6, r5, lsl #1
	ldrh r2, [r1, #0x1a]
	and r1, r2, #0x300
	cmp r1, #0x100
	bne _020BFA68
	ldr r3, [r6, #0xc]
	cmp r3, #0
	beq _020BFA68
	mov r0, r4
	mov r1, r6
	and r2, r2, #0xff
	blx r3
	ldr r2, [r7]
	ldr r1, [sp]
	mov r0, r8
	orr r1, r2, r1
	str r1, [r7]
_020BFA68:
	ldr r6, [r6, #0x10]
	cmp r6, #0
	bne _020BFA18
	add sp, sp, #4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G3dAnmBlendVis

	arm_func_start G3dDrawInternal_Loop_
G3dDrawInternal_Loop_: ; 0x020BFA7C
	stmdb sp!, {r3, r4, r5, lr}
	ldr r5, _020BFAC0 ; =_02110B9C
	mov r4, r0
_020BFA88:
	ldr r1, [r4, #8]
	mov r0, r4
	bic r1, r1, #0x40
	str r1, [r4, #8]
	ldr r1, [r4]
	ldrb r3, [r1]
	and r1, r3, #0x1f
	ldr r2, [r5, r1, lsl #2]
	and r1, r3, #0xe0
	blx r2
	ldr r0, [r4, #8]
	tst r0, #0x20
	beq _020BFA88
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BFAC0: .word _02110B9C
	arm_func_end G3dDrawInternal_Loop_

	arm_func_start G3dDrawInternal_
G3dDrawInternal_: ; 0x020BFAC4
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	mov r1, r5
	mov r0, #0
	mov r2, #0x188
	bl MIi_CpuClearFast
	mov r0, #1
	str r0, [r5, #0xc4]
	str r0, [r5, #8]
	ldr r0, [r4, #0x30]
	ldr r2, _020BFC54 ; =_02110A80
	cmp r0, #0
	ldreq r1, [r4, #4]
	ldreq r0, [r1, #4]
	addeq r0, r1, r0
	str r0, [r5]
	str r4, [r5, #4]
	ldr r0, [r4, #4]
	cmp r0, #0
	addne r0, r0, #0x40
	moveq r0, #0
	str r0, [r5, #0xd4]
	ldr r1, [r4, #4]
	cmp r1, #0
	ldrne r0, [r1, #8]
	cmpne r0, #0
	addne r0, r1, r0
	moveq r0, #0
	str r0, [r5, #0xd8]
	ldr r1, [r4, #4]
	cmp r1, #0
	ldrne r0, [r1, #0xc]
	cmpne r0, #0
	addne r0, r1, r0
	moveq r0, #0
	str r0, [r5, #0xdc]
	ldr r0, [r4, #4]
	ldr r1, _020BFC58 ; =_02110A74
	ldrb r3, [r0, #0x15]
	ldr r0, _020BFC5C ; =_02110A8C
	ldr r2, [r2, r3, lsl #2]
	str r2, [r5, #0xe8]
	ldr r2, [r4, #4]
	ldrb r2, [r2, #0x15]
	ldr r1, [r1, r2, lsl #2]
	str r1, [r5, #0xec]
	ldr r1, [r4, #4]
	ldrb r1, [r1, #0x16]
	ldr r0, [r0, r1, lsl #2]
	str r0, [r5, #0xf0]
	ldr r0, [r4, #4]
	ldr r0, [r0, #0x1c]
	str r0, [r5, #0xe0]
	ldr r0, [r4, #4]
	ldr r0, [r0, #0x20]
	str r0, [r5, #0xe4]
	ldr r1, [r4, #0x20]
	cmp r1, #0
	beq _020BFBD8
	ldrb r0, [r4, #0x24]
	cmp r0, #0x20
	bhs _020BFBD8
	add r0, r5, r0, lsl #2
	str r1, [r0, #0xc]
	ldrb r0, [r4, #0x24]
	ldrb r1, [r4, #0x25]
	add r0, r5, r0
	strb r1, [r0, #0x8c]
_020BFBD8:
	ldr r0, [r4]
	tst r0, #1
	ldrne r0, [r5, #8]
	orrne r0, r0, #0x80
	strne r0, [r5, #8]
	ldr r0, [r4]
	tst r0, #2
	ldrne r0, [r5, #8]
	orrne r0, r0, #0x100
	strne r0, [r5, #8]
	ldr r0, [r4]
	tst r0, #4
	ldrne r0, [r5, #8]
	orrne r0, r0, #0x200
	strne r0, [r5, #8]
	ldr r0, [r4]
	tst r0, #8
	ldrne r0, [r5, #8]
	orrne r0, r0, #0x400
	strne r0, [r5, #8]
	ldr r1, [r4, #0x28]
	cmp r1, #0
	beq _020BFC3C
	mov r0, r5
	blx r1
_020BFC3C:
	mov r0, r5
	bl G3dDrawInternal_Loop_
	ldr r0, [r4]
	bic r0, r0, #1
	str r0, [r4]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020BFC54: .word _02110A80
_020BFC58: .word _02110A74
_020BFC5C: .word _02110A8C
	arm_func_end G3dDrawInternal_

	arm_func_start updateHintVec___g3d
updateHintVec___g3d: ; 0x020BFC60
	stmdb sp!, {r3, r4, r5, lr}
	cmp r1, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r3, #1
	mov r4, #0
_020BFC74:
	ldrb r2, [r1, #0x19]
	mov r5, r4
	cmp r2, #0
	ble _020BFCB8
_020BFC84:
	add r2, r1, r5, lsl #1
	ldrh r2, [r2, #0x1a]
	tst r2, #0x100
	beq _020BFCA8
	mov lr, r5, asr #5
	ldr ip, [r0, lr, lsl #2]
	and r2, r5, #0x1f
	orr r2, ip, r3, lsl r2
	str r2, [r0, lr, lsl #2]
_020BFCA8:
	ldrb r2, [r1, #0x19]
	add r5, r5, #1
	cmp r5, r2
	blt _020BFC84
_020BFCB8:
	ldr r1, [r1, #0x10]
	cmp r1, #0
	bne _020BFC74
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end updateHintVec___g3d

	arm_func_start NNS_G3dDraw
NNS_G3dDraw: ; 0x020BFCC8
	stmdb sp!, {r4, lr}
	sub sp, sp, #0x188
	mov r4, r0
	ldr r0, [r4]
	and r0, r0, #0x10
	cmp r0, #0x10
	bne _020BFD5C
	add r1, r4, #0x3c
	mov r0, #0
	mov r2, #8
	bl MIi_CpuClear32
	add r1, r4, #0x44
	mov r0, #0
	mov r2, #8
	bl MIi_CpuClear32
	add r1, r4, #0x4c
	mov r0, #0
	mov r2, #8
	bl MIi_CpuClear32
	ldr r1, [r4, #8]
	cmp r1, #0
	beq _020BFD28
	add r0, r4, #0x3c
	bl updateHintVec___g3d
_020BFD28:
	ldr r1, [r4, #0x10]
	cmp r1, #0
	beq _020BFD3C
	add r0, r4, #0x44
	bl updateHintVec___g3d
_020BFD3C:
	ldr r1, [r4, #0x18]
	cmp r1, #0
	beq _020BFD50
	add r0, r4, #0x4c
	bl updateHintVec___g3d
_020BFD50:
	ldr r0, [r4]
	bic r0, r0, #0x10
	str r0, [r4]
_020BFD5C:
	ldr r2, _020BFDA0 ; =_021DA700
	ldr r0, [r2]
	cmp r0, #0
	beq _020BFD7C
	mov r1, r4
	bl G3dDrawInternal_
	add sp, sp, #0x188
	ldmia sp!, {r4, pc}
_020BFD7C:
	add r0, sp, #0
	mov r1, r4
	str r0, [r2]
	bl G3dDrawInternal_
	ldr r0, _020BFDA0 ; =_021DA700
	mov r1, #0
	str r1, [r0]
	add sp, sp, #0x188
	ldmia sp!, {r4, pc}
	.align 2, 0
_020BFDA0: .word _021DA700
	arm_func_end NNS_G3dDraw

	arm_func_start NNSi_G3dFuncSbc_NOP
NNSi_G3dFuncSbc_NOP: ; 0x020BFDA4
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #0xc]
	cmp r1, #0
	beq _020BFDBC
	blx r1
_020BFDBC:
	ldr r0, [r4]
	add r0, r0, #1
	str r0, [r4]
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_G3dFuncSbc_NOP

	arm_func_start NNSi_G3dFuncSbc_RET
NNSi_G3dFuncSbc_RET: ; 0x020BFDCC
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #0x10]
	cmp r1, #0
	beq _020BFDE4
	blx r1
_020BFDE4:
	ldr r0, [r4, #8]
	orr r0, r0, #0x20
	str r0, [r4, #8]
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_G3dFuncSbc_RET

	arm_func_start NNSi_G3dFuncSbc_NODE
NNSi_G3dFuncSbc_NODE: ; 0x020BFDF4
	stmdb sp!, {r4, r5, r6, lr}
	mov r6, r0
	ldr r0, [r6, #8]
	tst r0, #0x200
	bne _020BFF60
	ldr r1, [r6]
	add r0, r6, #0x184
	ldrb r4, [r1, #1]
	strb r4, [r6, #0xac]
	ldr r1, [r6, #8]
	orr r1, r1, #4
	str r1, [r6, #8]
	str r0, [r6, #0xb8]
	ldr r0, [r6, #0x14]
	cmp r0, #0
	ldrneb r5, [r6, #0x8e]
	moveq r5, #0
	cmp r5, #1
	bne _020BFE74
	ldr r1, [r6, #8]
	mov r0, r6
	bic r1, r1, #0x40
	str r1, [r6, #8]
	ldr r1, [r6, #0x14]
	blx r1
	ldr r0, [r6, #0x14]
	cmp r0, #0
	ldrneb r5, [r6, #0x8e]
	ldr r0, [r6, #8]
	moveq r5, #0
	and r0, r0, #0x40
	b _020BFE78
_020BFE74:
	mov r0, #0
_020BFE78:
	cmp r0, #0
	bne _020BFED8
	ldr ip, [r6, #4]
	ldr r1, [ip, #0x18]
	cmp r1, #0
	beq _020BFEC4
	mov r0, r4, lsr #5
	add r0, ip, r0, lsl #2
	ldr r0, [r0, #0x4c]
	and r2, r4, #0x1f
	mov r3, #1
	tst r0, r3, lsl r2
	beq _020BFEC4
	ldr r0, [r6, #0xb8]
	ldr r3, [ip, #0x1c]
	mov r2, r4
	blx r3
	cmp r0, #0
	bne _020BFED8
_020BFEC4:
	ldr r1, [r6]
	ldr r0, [r6, #0xb8]
	ldrb r1, [r1, #2]
	and r1, r1, #1
	str r1, [r0]
_020BFED8:
	cmp r5, #2
	bne _020BFF14
	ldr r1, [r6, #8]
	mov r0, r6
	bic r1, r1, #0x40
	str r1, [r6, #8]
	ldr r1, [r6, #0x14]
	blx r1
	ldr r0, [r6, #0x14]
	cmp r0, #0
	ldrneb r5, [r6, #0x8e]
	ldr r0, [r6, #8]
	moveq r5, #0
	and r0, r0, #0x40
	b _020BFF18
_020BFF14:
	mov r0, #0
_020BFF18:
	cmp r0, #0
	bne _020BFF40
	ldr r0, [r6, #0xb8]
	ldr r0, [r0]
	cmp r0, #0
	ldr r0, [r6, #8]
	orrne r0, r0, #1
	strne r0, [r6, #8]
	biceq r0, r0, #1
	streq r0, [r6, #8]
_020BFF40:
	cmp r5, #3
	bne _020BFF60
	ldr r1, [r6, #8]
	mov r0, r6
	bic r1, r1, #0x40
	str r1, [r6, #8]
	ldr r1, [r6, #0x14]
	blx r1
_020BFF60:
	ldr r0, [r6]
	add r0, r0, #3
	str r0, [r6]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end NNSi_G3dFuncSbc_NODE

	arm_func_start NNSi_G3dFuncSbc_MTX
NNSi_G3dFuncSbc_MTX: ; 0x020BFF70
	stmdb sp!, {r3, r4, r5, lr}
	mov r4, r0
	ldr r0, [r4, #8]
	tst r0, #0x200
	bne _020C002C
	tst r0, #1
	beq _020C002C
	ldr r0, [r4, #0x18]
	cmp r0, #0
	ldrneb r5, [r4, #0x8f]
	moveq r5, #0
	cmp r5, #1
	bne _020BFFD8
	ldr r1, [r4, #8]
	mov r0, r4
	bic r1, r1, #0x40
	str r1, [r4, #8]
	ldr r1, [r4, #0x18]
	blx r1
	ldr r0, [r4, #0x18]
	cmp r0, #0
	ldrneb r5, [r4, #0x8f]
	ldr r0, [r4, #8]
	moveq r5, #0
	and r0, r0, #0x40
	b _020BFFDC
_020BFFD8:
	mov r0, #0
_020BFFDC:
	cmp r0, #0
	bne _020C000C
	ldr r0, [r4]
	ldrb r0, [r0, #1]
	str r0, [sp]
	ldr r0, [r4, #8]
	tst r0, #0x100
	bne _020C000C
	add r1, sp, #0
	mov r0, #0x14
	mov r2, #1
	bl NNS_G3dGeBufferOP_N
_020C000C:
	cmp r5, #3
	bne _020C002C
	ldr r1, [r4, #8]
	mov r0, r4
	bic r1, r1, #0x40
	str r1, [r4, #8]
	ldr r1, [r4, #0x18]
	blx r1
_020C002C:
	ldr r0, [r4]
	add r0, r0, #2
	str r0, [r4]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNSi_G3dFuncSbc_MTX

	arm_func_start NNSi_G3dFuncSbc_MAT_InternalDefault
NNSi_G3dFuncSbc_MAT_InternalDefault: ; 0x020C003C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x1c
	mov r7, r0
	mov r5, r3
	strb r5, [r7, #0xad]
	ldr r3, [r7, #8]
	add r0, r7, #0xf4
	orr r3, r3, #8
	str r3, [r7, #8]
	str r0, [r7, #0xb0]
	ldr r0, [r7, #0x1c]
	mov r8, r1
	cmp r0, #0
	ldrneb r4, [r7, #0x90]
	mov r6, r2
	moveq r4, #0
	cmp r4, #1
	bne _020C00B8
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x1c]
	blx r1
	ldr r0, [r7, #0x1c]
	cmp r0, #0
	ldrneb r4, [r7, #0x90]
	ldr r0, [r7, #8]
	moveq r4, #0
	and r0, r0, #0x40
	b _020C00BC
_020C00B8:
	mov r0, #0
_020C00BC:
	cmp r0, #0
	bne _020C039C
	ldr r0, [r7, #4]
	ldr r0, [r0, #0x38]
	cmp r0, #0
	beq _020C00E8
	ldr r1, [r7, #8]
	tst r1, #0x80
	moveq r1, #0x38
	mlaeq r8, r5, r1, r0
	beq _020C0398
_020C00E8:
	cmp r8, #0x20
	cmpne r8, #0x40
	bne _020C0130
	mov r1, r5, lsr #5
	add r1, r7, r1, lsl #2
	ldr r1, [r1, #0xbc]
	and r2, r5, #0x1f
	mov r3, #1
	tst r1, r3, lsl r2
	beq _020C0130
	cmp r0, #0
	movne r1, #0x38
	mlane r8, r5, r1, r0
	bne _020C0398
	ldr r1, _020C04B0 ; =_021DA704
	mov r0, #0x38
	mla r8, r5, r0, r1
	b _020C0398
_020C0130:
	cmp r0, #0
	beq _020C0168
	add r8, r7, #0xbc
	mov r3, r5, lsr #5
	ldr r2, [r8, r3, lsl #2]
	and r0, r5, #0x1f
	mov r1, #1
	orr r0, r2, r1, lsl r0
	str r0, [r8, r3, lsl #2]
	ldr r1, [r7, #4]
	mov r0, #0x38
	ldr r1, [r1, #0x38]
	mla r8, r5, r0, r1
	b _020C019C
_020C0168:
	cmp r8, #0x40
	addne r8, r7, #0xf4
	bne _020C019C
	add lr, r7, #0xbc
	mov ip, r5, lsr #5
	ldr r1, _020C04B0 ; =_021DA704
	ldr r8, [lr, ip, lsl #2]
	mov r0, #0x38
	and r2, r5, #0x1f
	mov r3, #1
	orr r2, r8, r3, lsl r2
	mla r8, r5, r0, r1
	str r2, [lr, ip, lsl #2]
_020C019C:
	mov r0, #0
	str r0, [r8]
	ldr r3, [r7, #0xd8]
	cmp r3, #0
	beq _020C01F0
	adds r2, r3, #4
	beq _020C01DC
	ldrb r0, [r3, #5]
	cmp r5, r0
	bhs _020C01DC
	ldrh r1, [r3, #0xa]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r5, r1
	b _020C01E0
_020C01DC:
	mov r1, #0
_020C01E0:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, r3, r0
	bne _020C01F4
_020C01F0:
	mov r0, #0
_020C01F4:
	ldrh r0, [r0, #0x1e]
	ldr r1, _020C04B4 ; =NNS_G3dGlb
	tst r0, #0x20
	ldrne r0, [r8]
	orrne r0, r0, #0x20
	strne r0, [r8]
	ldrh r2, [r6, #0x1e]
	ldr r0, _020C04B8 ; =_02109474
	ldr ip, [r1, #0x94]
	mov r2, r2, asr #6
	and r2, r2, #7
	ldr lr, [r0, r2, lsl #2]
	ldr r2, [r6, #4]
	mvn r3, lr
	and r3, ip, r3
	and r2, r2, lr
	orr r2, r3, r2
	str r2, [r8, #4]
	ldrh ip, [r6, #0x1e]
	ldr r3, [r1, #0x98]
	ldr r2, [r6, #8]
	mov ip, ip, asr #9
	and ip, ip, #7
	ldr ip, [r0, ip, lsl #2]
	mvn r0, ip
	and r3, r3, r0
	and r0, r2, ip
	orr r0, r3, r0
	str r0, [r8, #8]
	ldr r3, [r6, #0x10]
	ldr r0, [r6, #0xc]
	ldr r2, [r1, #0x9c]
	mvn r1, r3
	and r1, r2, r1
	and r0, r0, r3
	orr r0, r1, r0
	str r0, [r8, #0xc]
	ldr r0, [r6, #0x14]
	str r0, [r8, #0x10]
	ldrh r0, [r6, #0x1c]
	str r0, [r8, #0x14]
	ldrh r0, [r6, #0x1e]
	tst r0, #1
	beq _020C0330
	tst r0, #2
	ldrne r1, [r8]
	add r0, r6, #0x2c
	orrne r1, r1, #1
	strne r1, [r8]
	bne _020C02D0
	ldr r1, [r0]
	str r1, [r8, #0x18]
	ldr r1, [r0, #4]
	add r0, r0, #8
	str r1, [r8, #0x1c]
_020C02D0:
	ldrh r1, [r6, #0x1e]
	tst r1, #4
	ldrne r1, [r8]
	orrne r1, r1, #2
	strne r1, [r8]
	bne _020C02FC
	ldrsh r1, [r0]
	strh r1, [r8, #0x20]
	ldrsh r1, [r0, #2]
	add r0, r0, #4
	strh r1, [r8, #0x22]
_020C02FC:
	ldrh r1, [r6, #0x1e]
	tst r1, #8
	ldrne r0, [r8]
	orrne r0, r0, #4
	strne r0, [r8]
	bne _020C0324
	ldr r1, [r0]
	str r1, [r8, #0x24]
	ldr r0, [r0, #4]
	str r0, [r8, #0x28]
_020C0324:
	ldr r0, [r8]
	orr r0, r0, #8
	str r0, [r8]
_020C0330:
	ldr ip, [r7, #4]
	ldr r1, [ip, #8]
	cmp r1, #0
	beq _020C036C
	mov r0, r5, lsr #5
	add r0, ip, r0, lsl #2
	ldr r0, [r0, #0x3c]
	and r2, r5, #0x1f
	mov r3, #1
	tst r0, r3, lsl r2
	beq _020C036C
	ldr r3, [ip, #0xc]
	mov r0, r8
	mov r2, r5
	blx r3
_020C036C:
	ldr r0, [r8]
	tst r0, #0x18
	beq _020C0398
	ldrh r0, [r6, #0x20]
	strh r0, [r8, #0x2c]
	ldrh r0, [r6, #0x22]
	strh r0, [r8, #0x2e]
	ldr r0, [r6, #0x24]
	str r0, [r8, #0x30]
	ldr r0, [r6, #0x28]
	str r0, [r8, #0x34]
_020C0398:
	str r8, [r7, #0xb0]
_020C039C:
	cmp r4, #2
	bne _020C03D8
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x1c]
	blx r1
	ldr r0, [r7, #0x1c]
	cmp r0, #0
	ldrneb r4, [r7, #0x90]
	ldr r0, [r7, #8]
	moveq r4, #0
	and r0, r0, #0x40
	b _020C03DC
_020C03D8:
	mov r0, #0
_020C03DC:
	cmp r0, #0
	bne _020C0484
	ldr r5, [r7, #0xb0]
	ldr r1, [r5, #0xc]
	tst r1, #0x1f0000
	beq _020C0478
	ldr r0, [r5]
	tst r0, #0x20
	bicne r0, r1, #0x1f0000
	strne r0, [r5, #0xc]
	ldr r0, [r7, #8]
	bic r0, r0, #2
	str r0, [r7, #8]
	tst r0, #0x100
	bne _020C0484
	ldr r0, _020C04BC ; =0x00293130
	ldr r3, _020C04C0 ; =0x00002B2A
	str r0, [sp]
	ldr r2, [r5, #4]
	add r1, sp, #4
	str r2, [sp, #4]
	ldr r6, [r5, #8]
	mov r2, #6
	str r6, [sp, #8]
	ldr r6, [r5, #0xc]
	str r6, [sp, #0xc]
	str r3, [sp, #0x10]
	ldr r3, [r5, #0x10]
	str r3, [sp, #0x14]
	ldr r3, [r5, #0x14]
	str r3, [sp, #0x18]
	bl NNS_G3dGeBufferOP_N
	ldr r0, [r5]
	tst r0, #0x18
	beq _020C0484
	ldr r1, [r7, #0xf0]
	mov r0, r5
	blx r1
	b _020C0484
_020C0478:
	ldr r0, [r7, #8]
	orr r0, r0, #2
	str r0, [r7, #8]
_020C0484:
	cmp r4, #3
	addne sp, sp, #0x1c
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x1c]
	blx r1
	add sp, sp, #0x1c
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C04B0: .word _021DA704
_020C04B4: .word NNS_G3dGlb
_020C04B8: .word _02109474
_020C04BC: .word 0x00293130
_020C04C0: .word 0x00002B2A
	arm_func_end NNSi_G3dFuncSbc_MAT_InternalDefault
