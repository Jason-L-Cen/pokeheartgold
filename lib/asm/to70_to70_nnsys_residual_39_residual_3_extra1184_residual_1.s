	.extern NNS_FndAllocFromAllocator
	.extern NNS_FndFreeToAllocator
	.extern NNS_G3dAnmObjCalcSizeRequired
	.extern NNS_G3dBindMdlSet
	.extern NNS_G3dGetMdlSet
	.extern NNS_G3dGetTex
	.extern NNS_G3dGlb
	.extern NNS_G3dGlbGetInvV
	.extern NNS_G3dGlbGetInvVP
	.extern NNS_G3dGlbGetInvWV
	.extern NNS_G3dGlbGetViewPort
	.extern NNS_G3dGlbGetWV
	.extern NNS_G3dGlbInit
	.extern NNS_G3dPlttGetRequiredSize
	.extern NNS_G3dPlttLoad
	.extern NNS_G3dPlttSetPlttKey
	.extern NNS_G3dTex4x4GetRequiredSize
	.extern NNS_G3dTexGetRequiredSize
	.extern NNS_G3dTexLoad
	.extern NNS_G3dTexSetTexKey
	.extern NNS_GfdDefaultFuncAllocPlttVram
	.extern NNS_GfdDefaultFuncAllocTexVram
	.extern NNS_GfdDefaultFuncFreePlttVram
	.extern NNS_GfdDefaultFuncFreeTexVram
	.extern _02109494
	.extern _02110A8C
	.extern _02110A9C
	.extern _02110AA0
	.extern _02110AA8
	.extern _02110ACC
	.extern _02110B0C
	.extern _02110B10
	.extern _02110B18
	.extern _02110B3C
	.extern _02110B48
	.extern _02110B54
	.extern _02110B58
	.extern _02110B60
	.extern _02110B84
	.extern _02110B90
	.extern _021DBB04
	.extern _021DBB44
	.extern _021DD404
	.extern _021DD408
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNSi_G3dModifyMatFlag
NNSi_G3dModifyMatFlag: ; 0x020C2BAC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	cmp r0, #0
	ldrne r4, [r0, #8]
	ldrb r3, [r0, #0x18]
	cmpne r4, #0
	addne lr, r0, r4
	moveq lr, #0
	cmp r3, #0
	mov ip, #0
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
	mvn r4, r2
	add r0, lr, #4
	mov r7, ip
	mov r6, ip
_020C2BE4:
	cmp lr, #0
	beq _020C2C2C
	cmp r0, #0
	beq _020C2C18
	ldrb r5, [lr, #5]
	cmp ip, r5
	bhs _020C2C18
	ldrh r8, [lr, #0xa]
	ldrh r5, [r0, r8]
	add r8, r0, r8
	add r8, r8, #4
	mla r8, r5, ip, r8
	b _020C2C1C
_020C2C18:
	mov r8, r7
_020C2C1C:
	cmp r8, #0
	ldrne r5, [r8]
	addne r8, lr, r5
	bne _020C2C30
_020C2C2C:
	mov r8, r6
_020C2C30:
	ldrh r5, [r8, #0x1e]
	cmp r1, #0
	add ip, ip, #1
	orrne r5, r5, r2
	andeq r5, r5, r4
	strh r5, [r8, #0x1e]
	cmp ip, r3
	blo _020C2BE4
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G3dModifyMatFlag

	arm_func_start NNSi_G3dModifyPolygonAttrMask
NNSi_G3dModifyPolygonAttrMask: ; 0x020C2C54
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	cmp r0, #0
	ldrne r4, [r0, #8]
	ldrb r3, [r0, #0x18]
	cmpne r4, #0
	addne lr, r0, r4
	moveq lr, #0
	cmp r3, #0
	mov ip, #0
	ldmlsia sp!, {r4, r5, r6, r7, r8, pc}
	mvn r4, r2
	add r0, lr, #4
	mov r7, ip
	mov r6, ip
_020C2C8C:
	cmp lr, #0
	beq _020C2CD4
	cmp r0, #0
	beq _020C2CC0
	ldrb r5, [lr, #5]
	cmp ip, r5
	bhs _020C2CC0
	ldrh r8, [lr, #0xa]
	ldrh r5, [r0, r8]
	add r8, r0, r8
	add r8, r8, #4
	mla r8, r5, ip, r8
	b _020C2CC4
_020C2CC0:
	mov r8, r7
_020C2CC4:
	cmp r8, #0
	ldrne r5, [r8]
	addne r8, lr, r5
	bne _020C2CD8
_020C2CD4:
	mov r8, r6
_020C2CD8:
	ldr r5, [r8, #0x10]
	cmp r1, #0
	orrne r5, r5, r2
	andeq r5, r5, r4
	add ip, ip, #1
	str r5, [r8, #0x10]
	cmp ip, r3
	blo _020C2C8C
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G3dModifyPolygonAttrMask
