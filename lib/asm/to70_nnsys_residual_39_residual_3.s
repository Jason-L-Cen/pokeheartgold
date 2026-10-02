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

	arm_func_start NNS_G3dMdlSetMdlDiff
NNS_G3dMdlSetMdlDiff: ; 0x020C2CFC
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2D5C
	adds ip, lr, #4
	beq _020C2D48
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2D48
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2D4C
_020C2D48:
	mov r1, #0
_020C2D4C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r3, lr, r0
	bne _020C2D60
_020C2D5C:
	mov r3, #0
_020C2D60:
	mov r0, #0x8000
	ldr r1, [r3, #4]
	rsb r0, r0, #0
	and r0, r1, r0
	orr r0, r0, r2
	str r0, [r3, #4]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlDiff

	arm_func_start NNS_G3dMdlSetMdlAmb
NNS_G3dMdlSetMdlAmb: ; 0x020C2D7C
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2DDC
	adds ip, lr, #4
	beq _020C2DC8
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2DC8
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2DCC
_020C2DC8:
	mov r1, #0
_020C2DCC:
	cmp r1, #0
	ldrne r0, [r1]
	addne r3, lr, r0
	bne _020C2DE0
_020C2DDC:
	mov r3, #0
_020C2DE0:
	ldr r1, [r3, #4]
	ldr r0, _020C2DF8 ; =0x8000FFFF
	and r0, r1, r0
	orr r0, r0, r2, lsl #16
	str r0, [r3, #4]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020C2DF8: .word 0x8000FFFF
	arm_func_end NNS_G3dMdlSetMdlAmb

	arm_func_start NNS_G3dMdlSetMdlSpec
NNS_G3dMdlSetMdlSpec: ; 0x020C2DFC
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2E5C
	adds ip, lr, #4
	beq _020C2E48
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2E48
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2E4C
_020C2E48:
	mov r1, #0
_020C2E4C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r3, lr, r0
	bne _020C2E60
_020C2E5C:
	mov r3, #0
_020C2E60:
	mov r0, #0x8000
	ldr r1, [r3, #8]
	rsb r0, r0, #0
	and r0, r1, r0
	orr r0, r0, r2
	str r0, [r3, #8]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlSpec

	arm_func_start NNS_G3dMdlSetMdlEmi
NNS_G3dMdlSetMdlEmi: ; 0x020C2E7C
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2EDC
	adds ip, lr, #4
	beq _020C2EC8
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2EC8
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2ECC
_020C2EC8:
	mov r1, #0
_020C2ECC:
	cmp r1, #0
	ldrne r0, [r1]
	addne r3, lr, r0
	bne _020C2EE0
_020C2EDC:
	mov r3, #0
_020C2EE0:
	ldr r1, [r3, #8]
	ldr r0, _020C2EF8 ; =0x8000FFFF
	and r0, r1, r0
	orr r0, r0, r2, lsl #16
	str r0, [r3, #8]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020C2EF8: .word 0x8000FFFF
	arm_func_end NNS_G3dMdlSetMdlEmi

	arm_func_start NNS_G3dMdlSetMdlLightEnableFlag
NNS_G3dMdlSetMdlLightEnableFlag: ; 0x020C2EFC
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2F5C
	adds ip, lr, #4
	beq _020C2F48
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2F48
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2F4C
_020C2F48:
	mov r1, #0
_020C2F4C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C2F60
_020C2F5C:
	mov r1, #0
_020C2F60:
	ldr r0, [r1, #0xc]
	bic r0, r0, #0xf
	orr r0, r0, r2
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlLightEnableFlag

	arm_func_start NNS_G3dMdlSetMdlPolygonMode
NNS_G3dMdlSetMdlPolygonMode: ; 0x020C2F74
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C2FD4
	adds ip, lr, #4
	beq _020C2FC0
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C2FC0
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C2FC4
_020C2FC0:
	mov r1, #0
_020C2FC4:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C2FD8
_020C2FD4:
	mov r1, #0
_020C2FD8:
	ldr r0, [r1, #0xc]
	bic r0, r0, #0x30
	orr r0, r0, r2, lsl #4
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlPolygonMode

	arm_func_start NNS_G3dMdlSetMdlCullMode
NNS_G3dMdlSetMdlCullMode: ; 0x020C2FEC
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C304C
	adds ip, lr, #4
	beq _020C3038
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C3038
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C303C
_020C3038:
	mov r1, #0
_020C303C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C3050
_020C304C:
	mov r1, #0
_020C3050:
	ldr r0, [r1, #0xc]
	bic r0, r0, #0xc0
	orr r0, r0, r2, lsl #6
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlCullMode

	arm_func_start NNS_G3dMdlSetMdlPolygonID
NNS_G3dMdlSetMdlPolygonID: ; 0x020C3064
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C30C4
	adds ip, lr, #4
	beq _020C30B0
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C30B0
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C30B4
_020C30B0:
	mov r1, #0
_020C30B4:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C30C8
_020C30C4:
	mov r1, #0
_020C30C8:
	ldr r0, [r1, #0xc]
	bic r0, r0, #0x3f000000
	orr r0, r0, r2, lsl #24
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlPolygonID

	arm_func_start NNS_G3dMdlSetMdlAlpha
NNS_G3dMdlSetMdlAlpha: ; 0x020C30DC
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C313C
	adds ip, lr, #4
	beq _020C3128
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C3128
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C312C
_020C3128:
	mov r1, #0
_020C312C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C3140
_020C313C:
	mov r1, #0
_020C3140:
	ldr r0, [r1, #0xc]
	bic r0, r0, #0x1f0000
	orr r0, r0, r2, lsl #16
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlAlpha

	arm_func_start NNS_G3dMdlSetMdlFogEnableFlag
NNS_G3dMdlSetMdlFogEnableFlag: ; 0x020C3154
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C31B4
	adds ip, lr, #4
	beq _020C31A0
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C31A0
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C31A4
_020C31A0:
	mov r1, #0
_020C31A4:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C31B8
_020C31B4:
	mov r1, #0
_020C31B8:
	ldr r0, [r1, #0xc]
	cmp r2, #0
	orrne r0, r0, #0x8000
	biceq r0, r0, #0x8000
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlFogEnableFlag

	arm_func_start NNS_G3dMdlSetMdlDepthTestCond
NNS_G3dMdlSetMdlDepthTestCond: ; 0x020C31D0
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C3230
	adds ip, lr, #4
	beq _020C321C
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C321C
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C3220
_020C321C:
	mov r1, #0
_020C3220:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C3234
_020C3230:
	mov r1, #0
_020C3234:
	ldr r0, [r1, #0xc]
	cmp r2, #0
	orrne r0, r0, #0x4000
	biceq r0, r0, #0x4000
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlDepthTestCond

	arm_func_start NNS_G3dMdlSetMdl1Dot
NNS_G3dMdlSetMdl1Dot: ; 0x020C324C
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C32AC
	adds ip, lr, #4
	beq _020C3298
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C3298
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C329C
_020C3298:
	mov r1, #0
_020C329C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C32B0
_020C32AC:
	mov r1, #0
_020C32B0:
	ldr r0, [r1, #0xc]
	cmp r2, #0
	orrne r0, r0, #0x2000
	biceq r0, r0, #0x2000
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdl1Dot

	arm_func_start NNS_G3dMdlSetMdlFarClip
NNS_G3dMdlSetMdlFarClip: ; 0x020C32C8
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C3328
	adds ip, lr, #4
	beq _020C3314
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C3314
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C3318
_020C3314:
	mov r1, #0
_020C3318:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C332C
_020C3328:
	mov r1, #0
_020C332C:
	ldr r0, [r1, #0xc]
	cmp r2, #0
	orrne r0, r0, #0x1000
	biceq r0, r0, #0x1000
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlFarClip

	arm_func_start NNS_G3dMdlSetMdlXLDepthUpdate
NNS_G3dMdlSetMdlXLDepthUpdate: ; 0x020C3344
	stmdb sp!, {r3, lr}
	cmp r0, #0
	ldrne r3, [r0, #8]
	cmpne r3, #0
	addne lr, r0, r3
	moveq lr, #0
	cmp lr, #0
	beq _020C33A4
	adds ip, lr, #4
	beq _020C3390
	ldrb r0, [lr, #5]
	cmp r1, r0
	bhs _020C3390
	ldrh r3, [lr, #0xa]
	ldrh r0, [ip, r3]
	add r3, ip, r3
	add r3, r3, #4
	mla r1, r0, r1, r3
	b _020C3394
_020C3390:
	mov r1, #0
_020C3394:
	cmp r1, #0
	ldrne r0, [r1]
	addne r1, lr, r0
	bne _020C33A8
_020C33A4:
	mov r1, #0
_020C33A8:
	ldr r0, [r1, #0xc]
	cmp r2, #0
	orrne r0, r0, #0x800
	biceq r0, r0, #0x800
	str r0, [r1, #0xc]
	ldmia sp!, {r3, pc}
	arm_func_end NNS_G3dMdlSetMdlXLDepthUpdate

	arm_func_start NNS_G3dMdlGetMdlLightEnableFlag
NNS_G3dMdlGetMdlLightEnableFlag: ; 0x020C33C0
	cmp r0, #0
	ldrne r2, [r0, #8]
	cmpne r2, #0
	addne ip, r0, r2
	moveq ip, #0
	cmp ip, #0
	beq _020C341C
	adds r3, ip, #4
	beq _020C3408
	ldrb r0, [ip, #5]
	cmp r1, r0
	bhs _020C3408
	ldrh r2, [ip, #0xa]
	ldrh r0, [r3, r2]
	add r2, r3, r2
	add r2, r2, #4
	mla r1, r0, r1, r2
	b _020C340C
_020C3408:
	mov r1, #0
_020C340C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, ip, r0
	bne _020C3420
_020C341C:
	mov r0, #0
_020C3420:
	ldr r0, [r0, #0xc]
	and r0, r0, #0xf
	bx lr
	arm_func_end NNS_G3dMdlGetMdlLightEnableFlag

	arm_func_start NNS_G3dMdlGetMdlFogEnableFlag
NNS_G3dMdlGetMdlFogEnableFlag: ; 0x020C342C
	cmp r0, #0
	ldrne r2, [r0, #8]
	cmpne r2, #0
	addne ip, r0, r2
	moveq ip, #0
	cmp ip, #0
	beq _020C3488
	adds r3, ip, #4
	beq _020C3474
	ldrb r0, [ip, #5]
	cmp r1, r0
	bhs _020C3474
	ldrh r2, [ip, #0xa]
	ldrh r0, [r3, r2]
	add r2, r3, r2
	add r2, r2, #4
	mla r1, r0, r1, r2
	b _020C3478
_020C3474:
	mov r1, #0
_020C3478:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, ip, r0
	bne _020C348C
_020C3488:
	mov r0, #0
_020C348C:
	ldr r0, [r0, #0xc]
	and r0, r0, #0x8000
	bx lr
	arm_func_end NNS_G3dMdlGetMdlFogEnableFlag
