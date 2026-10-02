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
	.extern NNS_G3dGeBufferOP_N
	.extern NNS_G3dGeFlushBuffer
	.extern NNS_G3dGeSendDL
	.extern NNS_G3dGetCurrentMtx

.text
	arm_func_start NNSi_G3dFuncSbc_NODEDESC
NNSi_G3dFuncSbc_NODEDESC: ; 0x020C06E8
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	mov sl, r0
	ldr r0, [sl]
	mov sb, r1
	ldrb r6, [r0, #1]
	mov r4, #4
	strb r6, [sl, #0xae]
	ldr r0, [sl, #8]
	orr r0, r0, #0x10
	str r0, [sl, #8]
	tst r0, #0x400
	beq _020C0770
	cmp sb, #0x40
	cmpne sb, #0x60
	addeq r4, r4, #1
	cmp sb, #0x20
	cmpne sb, #0x60
	bne _020C075C
	tst r0, #0x100
	add r4, r4, #1
	bne _020C075C
	ldr r0, [sl]
	add r1, sp, #0x10
	ldrb r3, [r0, #4]
	mov r0, #0x14
	mov r2, #1
	str r3, [sp, #0x10]
	bl NNS_G3dGeBufferOP_N
_020C075C:
	ldr r0, [sl]
	add sp, sp, #0x14
	add r0, r0, r4
	str r0, [sl]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C0770:
	cmp sb, #0x40
	cmpne sb, #0x60
	bne _020C07B4
	ldr r0, [sl]
	cmp sb, #0x40
	ldreqb r0, [r0, #4]
	add r4, r4, #1
	streq r0, [sp, #0xc]
	ldrneb r0, [r0, #5]
	strne r0, [sp, #0xc]
	ldr r0, [sl, #8]
	tst r0, #0x100
	bne _020C07B4
	add r1, sp, #0xc
	mov r0, #0x14
	mov r2, #1
	bl NNS_G3dGeBufferOP_N
_020C07B4:
	add r0, sl, #0x12c
	str r0, [sl, #0xb4]
	ldr r0, [sl, #0x24]
	cmp r0, #0
	ldrneb r8, [sl, #0x92]
	moveq r8, #0
	cmp r8, #1
	bne _020C0808
	ldr r1, [sl, #8]
	mov r0, sl
	bic r1, r1, #0x40
	str r1, [sl, #8]
	ldr r1, [sl, #0x24]
	blx r1
	ldr r0, [sl, #0x24]
	cmp r0, #0
	ldrneb r8, [sl, #0x92]
	ldr r0, [sl, #8]
	moveq r8, #0
	and r0, r0, #0x40
	b _020C080C
_020C0808:
	mov r0, #0
_020C080C:
	cmp r0, #0
	bne _020C0A54
	ldr r0, [sl, #4]
	ldr r1, [r0, #0x34]
	cmp r1, #0
	beq _020C0840
	mov r0, #0x58
	mla r5, r6, r0, r1
	ldr r0, [sl, #8]
	tst r0, #0x80
	moveq r0, #1
	movne r0, #0
	b _020C0848
_020C0840:
	add r5, sl, #0x12c
	mov r0, #0
_020C0848:
	cmp r0, #0
	bne _020C0A50
	mov r0, #0
	str r0, [r5]
	ldr r0, [sl, #4]
	ldr r1, [r0, #0x10]
	cmp r1, #0
	beq _020C0880
	ldr r3, [r0, #0x14]
	mov r0, r5
	mov r2, r6
	blx r3
	cmp r0, #0
	bne _020C0A50
_020C0880:
	ldr r2, [sl, #0xd4]
	cmp r2, #0
	beq _020C08C8
	beq _020C08B4
	ldrb r0, [r2, #1]
	cmp r6, r0
	bhs _020C08B4
	ldrh r1, [r2, #6]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r6, r1
	b _020C08B8
_020C08B4:
	mov r1, #0
_020C08B8:
	cmp r1, #0
	ldrne r0, [r1]
	addne r7, r2, r0
	bne _020C08CC
_020C08C8:
	mov r7, #0
_020C08CC:
	ldrh r0, [r7]
	add r6, r7, #4
	tst r0, #1
	beq _020C08EC
	ldr r0, [r5]
	orr r0, r0, #4
	str r0, [r5]
	b _020C0908
_020C08EC:
	ldr r0, [r6]
	str r0, [r5, #0x4c]
	ldr r0, [r6, #4]
	str r0, [r5, #0x50]
	ldr r0, [r6, #8]
	add r6, r6, #0xc
	str r0, [r5, #0x54]
_020C0908:
	ldrh r0, [r7]
	tst r0, #2
	beq _020C0924
	ldr r0, [r5]
	orr r0, r0, #2
	str r0, [r5]
	b _020C0A38
_020C0924:
	tst r0, #8
	beq _020C09EC
	and r1, r0, #0xf0
	mov fp, r1, asr #4
	ldrsh r1, [r6]
	add r0, r5, #0x28
	str r1, [sp]
	ldrsh r1, [r6, #2]
	str r1, [sp, #4]
	bl MI_Zero36B
	ldrh r0, [r7]
	ldr r2, _020C0B38 ; =_02109494
	ldr r1, _020C0B3C ; =_02109494 + 1
	tst r0, #0x100
	mov r0, #0x1000
	ldrb r2, [r2, fp, lsl #2]
	ldrb r1, [r1, fp, lsl #2]
	rsbne r0, r0, #0
	add r3, r5, fp, lsl #2
	str r0, [r3, #0x28]
	ldr r0, [sp]
	add r2, r5, r2, lsl #2
	str r0, [r2, #0x28]
	ldr r0, [sp, #4]
	add r1, r5, r1, lsl #2
	str r0, [r1, #0x28]
	ldrh r0, [r7]
	tst r0, #0x200
	beq _020C09A4
	ldr r0, [sp, #4]
	rsb r0, r0, #0
	str r0, [sp, #4]
_020C09A4:
	ldr r0, _020C0B40 ; =_02109494 + 2
	ldrb r0, [r0, fp, lsl #2]
	add r1, r5, r0, lsl #2
	ldr r0, [sp, #4]
	str r0, [r1, #0x28]
	ldrh r0, [r7]
	tst r0, #0x400
	beq _020C09D0
	ldr r0, [sp]
	rsb r0, r0, #0
	str r0, [sp]
_020C09D0:
	ldr r0, _020C0B44 ; =_02109494 + 3
	add r6, r6, #4
	ldrb r0, [r0, fp, lsl #2]
	add r1, r5, r0, lsl #2
	ldr r0, [sp]
	str r0, [r1, #0x28]
	b _020C0A38
_020C09EC:
	ldrsh r0, [r7, #2]
	str r0, [r5, #0x28]
	ldrsh r0, [r6]
	str r0, [r5, #0x2c]
	ldrsh r0, [r6, #2]
	str r0, [r5, #0x30]
	ldrsh r0, [r6, #4]
	str r0, [r5, #0x34]
	ldrsh r0, [r6, #6]
	str r0, [r5, #0x38]
	ldrsh r0, [r6, #8]
	str r0, [r5, #0x3c]
	ldrsh r0, [r6, #0xa]
	str r0, [r5, #0x40]
	ldrsh r0, [r6, #0xc]
	str r0, [r5, #0x44]
	ldrsh r0, [r6, #0xe]
	add r6, r6, #0x10
	str r0, [r5, #0x48]
_020C0A38:
	ldrh r3, [r7]
	ldr r2, [sl]
	ldr r7, [sl, #0xe8]
	mov r0, r5
	mov r1, r6
	blx r7
_020C0A50:
	str r5, [sl, #0xb4]
_020C0A54:
	cmp r8, #2
	bne _020C0A90
	ldr r1, [sl, #8]
	mov r0, sl
	bic r1, r1, #0x40
	str r1, [sl, #8]
	ldr r1, [sl, #0x24]
	blx r1
	ldr r0, [sl, #0x24]
	cmp r0, #0
	ldrneb r8, [sl, #0x92]
	ldr r0, [sl, #8]
	moveq r8, #0
	and r0, r0, #0x40
	b _020C0A94
_020C0A90:
	mov r0, #0
_020C0A94:
	cmp r0, #0
	bne _020C0AB4
	ldr r0, [sl, #8]
	tst r0, #0x100
	bne _020C0AB4
	ldr r0, [sl, #0xb4]
	ldr r1, [sl, #0xec]
	blx r1
_020C0AB4:
	mov r0, #0
	str r0, [sl, #0xb4]
	cmp r8, #3
	bne _020C0AE4
	ldr r1, [sl, #8]
	mov r0, sl
	bic r1, r1, #0x40
	str r1, [sl, #8]
	ldr r1, [sl, #0x24]
	blx r1
	ldr r0, [sl, #8]
	and r0, r0, #0x40
_020C0AE4:
	cmp sb, #0x20
	cmpne sb, #0x60
	bne _020C0B24
	cmp r0, #0
	add r4, r4, #1
	bne _020C0B24
	ldr r0, [sl, #8]
	tst r0, #0x100
	bne _020C0B24
	ldr r0, [sl]
	add r1, sp, #8
	ldrb r3, [r0, #4]
	mov r0, #0x13
	mov r2, #1
	str r3, [sp, #8]
	bl NNS_G3dGeBufferOP_N
_020C0B24:
	ldr r0, [sl]
	add r0, r0, r4
	str r0, [sl]
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C0B38: .word _02109494
_020C0B3C: .word _02109494 + 1
_020C0B40: .word _02109494 + 2
_020C0B44: .word _02109494 + 3
	arm_func_end NNSi_G3dFuncSbc_NODEDESC

	arm_func_start NNSi_G3dFuncSbc_BB
NNSi_G3dFuncSbc_BB: ; 0x020C0B48
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0xc8
	mov sb, r0
	ldr r0, [sb, #8]
	mov r8, r1
	tst r0, #0x200
	mov r4, #2
	ldr r5, _020C0E24 ; =_02110B3C
	ldr r6, _020C0E28 ; =_02110B48
	beq _020C0B9C
	cmp r8, #0x40
	cmpne r8, #0x60
	addeq r4, r4, #1
	cmp r8, #0x20
	cmpne r8, #0x60
	ldr r0, [sb]
	addeq r4, r4, #1
	add r0, r0, r4
	add sp, sp, #0xc8
	str r0, [sb]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C0B9C:
	cmp r8, #0x40
	cmpne r8, #0x60
	bne _020C0BDC
	tst r0, #0x100
	add r4, r4, #1
	bne _020C0BDC
	ldr r0, [sb]
	cmp r8, #0x40
	ldreqb r0, [r0, #2]
	add r1, sp, #4
	mov r2, #1
	streq r0, [sp, #4]
	ldrneb r0, [r0, #3]
	strne r0, [sp, #4]
	mov r0, #0x14
	bl NNS_G3dGeBufferOP_N
_020C0BDC:
	ldr r0, [sb, #0x28]
	cmp r0, #0
	ldrneb r7, [sb, #0x93]
	moveq r7, #0
	cmp r7, #1
	bne _020C0C28
	ldr r1, [sb, #8]
	mov r0, sb
	bic r1, r1, #0x40
	str r1, [sb, #8]
	ldr r1, [sb, #0x28]
	blx r1
	ldr r0, [sb, #0x28]
	cmp r0, #0
	ldrneb r7, [sb, #0x93]
	ldr r0, [sb, #8]
	moveq r7, #0
	and r1, r0, #0x40
	b _020C0C2C
_020C0C28:
	mov r1, #0
_020C0C2C:
	ldr r0, [sb, #8]
	tst r0, #0x100
	cmpeq r1, #0
	bne _020C0DA4
	bl NNS_G3dGeFlushBuffer
	ldr r2, _020C0E2C ; =0x00151110
	ldr r1, _020C0E30 ; =0x04000400
	mov r0, #0
	str r2, [r1]
	str r0, [r1]
	str r0, [r1]
	add sl, sp, #0x88
_020C0C5C:
	mov r0, sl
	bl G3X_GetClipMtx
	cmp r0, #0
	bne _020C0C5C
	ldr r0, _020C0E34 ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #1
	beq _020C0C9C
	bl NNS_G3dGlbGetWV
	add r1, sp, #0x48
	bl MTX_Copy43To44_
	add r0, sp, #0x88
	add r1, sp, #0x48
	mov r2, r0
	bl MTX_Concat44
	b _020C0CC0
_020C0C9C:
	tst r0, #2
	beq _020C0CC0
	ldr r0, _020C0E38 ; =NNS_G3dGlb + 0x4C
	add r1, sp, #8
	bl MTX_Copy43To44_
	add r0, sp, #0x88
	add r1, sp, #8
	mov r2, r0
	bl MTX_Concat44
_020C0CC0:
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	ldr r1, [sp, #0xc0]
	add r0, sp, #0x88
	str r3, [r5]
	str r2, [r5, #4]
	str r1, [r5, #8]
	bl VEC_Mag
	str r0, [r6]
	add r0, sp, #0x98
	bl VEC_Mag
	str r0, [r6, #4]
	add r0, sp, #0xa8
	bl VEC_Mag
	ldr r1, _020C0E34 ; =NNS_G3dGlb
	str r0, [r6, #8]
	ldr r0, [r1, #0xfc]
	ldr r1, _020C0E30 ; =0x04000400
	tst r0, #1
	beq _020C0D50
	ldr r3, _020C0E3C ; =0x00171012
	ldr r0, _020C0E40 ; =_02110B10
	mov r2, #8
	str r3, [r1]
	bl MIi_CpuSend32
	bl NNS_G3dGlbGetInvWV
	ldr r1, _020C0E30 ; =0x04000400
	mov r2, #0x30
	bl MIi_CpuSend32
	ldr r2, _020C0E44 ; =0x00001B19
	ldr r1, _020C0E30 ; =0x04000400
	ldr r0, _020C0E48 ; =_02110B18
	str r2, [r1]
	mov r2, #0x3c
	bl MIi_CpuSend32
	b _020C0DA4
_020C0D50:
	tst r0, #2
	beq _020C0D98
	ldr r3, _020C0E3C ; =0x00171012
	ldr r0, _020C0E40 ; =_02110B10
	mov r2, #8
	str r3, [r1]
	bl MIi_CpuSend32
	bl NNS_G3dGlbGetInvV
	ldr r1, _020C0E30 ; =0x04000400
	mov r2, #0x30
	bl MIi_CpuSend32
	ldr r2, _020C0E44 ; =0x00001B19
	ldr r1, _020C0E30 ; =0x04000400
	ldr r0, _020C0E48 ; =_02110B18
	str r2, [r1]
	mov r2, #0x3c
	bl MIi_CpuSend32
	b _020C0DA4
_020C0D98:
	ldr r0, _020C0E4C ; =_02110B0C
	mov r2, #0x48
	bl MIi_CpuSend32
_020C0DA4:
	cmp r7, #3
	movne r0, #0
	bne _020C0DD0
	ldr r1, [sb, #8]
	mov r0, sb
	bic r1, r1, #0x40
	str r1, [sb, #8]
	ldr r1, [sb, #0x28]
	blx r1
	ldr r0, [sb, #8]
	and r0, r0, #0x40
_020C0DD0:
	cmp r8, #0x20
	cmpne r8, #0x60
	bne _020C0E10
	cmp r0, #0
	add r4, r4, #1
	bne _020C0E10
	ldr r0, [sb, #8]
	tst r0, #0x100
	bne _020C0E10
	ldr r0, [sb]
	add r1, sp, #0
	ldrb r3, [r0, #2]
	mov r0, #0x13
	mov r2, #1
	str r3, [sp]
	bl NNS_G3dGeBufferOP_N
_020C0E10:
	ldr r0, [sb]
	add r0, r0, r4
	str r0, [sb]
	add sp, sp, #0xc8
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020C0E24: .word _02110B3C
_020C0E28: .word _02110B48
_020C0E2C: .word 0x00151110
_020C0E30: .word 0x04000400
_020C0E34: .word NNS_G3dGlb
_020C0E38: .word NNS_G3dGlb + 0x4C
_020C0E3C: .word 0x00171012
_020C0E40: .word _02110B10
_020C0E44: .word 0x00001B19
_020C0E48: .word _02110B18
_020C0E4C: .word _02110B0C
	arm_func_end NNSi_G3dFuncSbc_BB

	arm_func_start NNSi_G3dFuncSbc_BBY
NNSi_G3dFuncSbc_BBY: ; 0x020C0E50
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0xc8
	mov sl, r0
	ldr r0, [sl, #8]
	mov sb, r1
	tst r0, #0x200
	mov r5, #2
	ldr fp, _020C1188 ; =_02110B84
	ldr r6, _020C118C ; =_02110B90
	ldr r7, _020C1190 ; =_02110B60
	beq _020C0EA8
	cmp sb, #0x40
	cmpne sb, #0x60
	addeq r5, r5, #1
	cmp sb, #0x20
	cmpne sb, #0x60
	ldr r0, [sl]
	addeq r5, r5, #1
	add r0, r0, r5
	add sp, sp, #0xc8
	str r0, [sl]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C0EA8:
	cmp sb, #0x40
	cmpne sb, #0x60
	bne _020C0EE8
	tst r0, #0x100
	add r5, r5, #1
	bne _020C0EE8
	ldr r0, [sl]
	cmp sb, #0x40
	ldreqb r0, [r0, #2]
	add r1, sp, #4
	mov r2, #1
	streq r0, [sp, #4]
	ldrneb r0, [r0, #3]
	strne r0, [sp, #4]
	mov r0, #0x14
	bl NNS_G3dGeBufferOP_N
_020C0EE8:
	ldr r0, [sl, #0x2c]
	cmp r0, #0
	ldrneb r8, [sl, #0x94]
	moveq r8, #0
	cmp r8, #1
	bne _020C0F34
	ldr r1, [sl, #8]
	mov r0, sl
	bic r1, r1, #0x40
	str r1, [sl, #8]
	ldr r1, [sl, #0x2c]
	blx r1
	ldr r0, [sl, #0x2c]
	cmp r0, #0
	ldrneb r8, [sl, #0x94]
	ldr r0, [sl, #8]
	moveq r8, #0
	and r1, r0, #0x40
	b _020C0F38
_020C0F34:
	mov r1, #0
_020C0F38:
	ldr r0, [sl, #8]
	tst r0, #0x100
	cmpeq r1, #0
	bne _020C1108
	bl NNS_G3dGeFlushBuffer
	ldr r2, _020C1194 ; =0x00151110
	ldr r1, _020C1198 ; =0x04000400
	mov r0, #0
	str r2, [r1]
	str r0, [r1]
	str r0, [r1]
	add r4, sp, #0x88
_020C0F68:
	mov r0, r4
	bl G3X_GetClipMtx
	cmp r0, #0
	bne _020C0F68
	ldr r0, _020C119C ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #1
	beq _020C0FA8
	bl NNS_G3dGlbGetWV
	add r1, sp, #0x48
	bl MTX_Copy43To44_
	add r0, sp, #0x88
	add r1, sp, #0x48
	mov r2, r0
	bl MTX_Concat44
	b _020C0FCC
_020C0FA8:
	tst r0, #2
	beq _020C0FCC
	ldr r0, _020C11A0 ; =NNS_G3dGlb + 0x4C
	add r1, sp, #8
	bl MTX_Copy43To44_
	add r0, sp, #0x88
	add r1, sp, #8
	mov r2, r0
	bl MTX_Concat44
_020C0FCC:
	ldr r3, [sp, #0xb8]
	ldr r2, [sp, #0xbc]
	ldr r1, [sp, #0xc0]
	add r0, sp, #0x88
	str r3, [fp]
	str r2, [fp, #4]
	str r1, [fp, #8]
	bl VEC_Mag
	str r0, [r6]
	add r0, sp, #0x98
	bl VEC_Mag
	str r0, [r6, #4]
	add r0, sp, #0xa8
	bl VEC_Mag
	ldr r1, [sp, #0x9c]
	str r0, [r6, #8]
	cmp r1, #0
	ldreq r0, [sp, #0xa0]
	cmpeq r0, #0
	beq _020C1040
	add r0, sp, #0x98
	add r1, r7, #0xc
	bl VEC_Normalize
	ldr r1, [r7, #0x14]
	ldr r0, [r7, #0x10]
	rsb r1, r1, #0
	str r1, [r7, #0x1c]
	str r0, [r7, #0x20]
	b _020C1060
_020C1040:
	add r0, sp, #0xa8
	add r1, r7, #0x18
	bl VEC_Normalize
	ldr r1, [r7, #0x1c]
	ldr r0, [r7, #0x20]
	rsb r1, r1, #0
	str r1, [r7, #0x14]
	str r0, [r7, #0x10]
_020C1060:
	ldr r0, _020C119C ; =NNS_G3dGlb
	ldr r1, _020C1198 ; =0x04000400
	ldr r0, [r0, #0xfc]
	tst r0, #1
	beq _020C10B4
	ldr r3, _020C11A4 ; =0x00171012
	ldr r0, _020C11A8 ; =_02110B58
	mov r2, #8
	str r3, [r1]
	bl MIi_CpuSend32
	bl NNS_G3dGlbGetInvWV
	ldr r1, _020C1198 ; =0x04000400
	mov r2, #0x30
	bl MIi_CpuSend32
	ldr r2, _020C11AC ; =0x00001B19
	ldr r1, _020C1198 ; =0x04000400
	ldr r0, _020C1190 ; =_02110B60
	str r2, [r1]
	mov r2, #0x3c
	bl MIi_CpuSend32
	b _020C1108
_020C10B4:
	tst r0, #2
	beq _020C10FC
	ldr r3, _020C11A4 ; =0x00171012
	ldr r0, _020C11A8 ; =_02110B58
	mov r2, #8
	str r3, [r1]
	bl MIi_CpuSend32
	bl NNS_G3dGlbGetInvV
	ldr r1, _020C1198 ; =0x04000400
	mov r2, #0x30
	bl MIi_CpuSend32
	ldr r2, _020C11AC ; =0x00001B19
	ldr r1, _020C1198 ; =0x04000400
	ldr r0, _020C1190 ; =_02110B60
	str r2, [r1]
	mov r2, #0x3c
	bl MIi_CpuSend32
	b _020C1108
_020C10FC:
	ldr r0, _020C11B0 ; =_02110B54
	mov r2, #0x48
	bl MIi_CpuSend32
_020C1108:
	cmp r8, #3
	movne r0, #0
	bne _020C1134
	ldr r1, [sl, #8]
	mov r0, sl
	bic r1, r1, #0x40
	str r1, [sl, #8]
	ldr r1, [sl, #0x2c]
	blx r1
	ldr r0, [sl, #8]
	and r0, r0, #0x40
_020C1134:
	cmp sb, #0x20
	cmpne sb, #0x60
	bne _020C1174
	cmp r0, #0
	add r5, r5, #1
	bne _020C1174
	ldr r0, [sl, #8]
	tst r0, #0x100
	bne _020C1174
	ldr r0, [sl]
	add r1, sp, #0
	ldrb r3, [r0, #2]
	mov r0, #0x13
	mov r2, #1
	str r3, [sp]
	bl NNS_G3dGeBufferOP_N
_020C1174:
	ldr r0, [sl]
	add r0, r0, r5
	str r0, [sl]
	add sp, sp, #0xc8
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C1188: .word _02110B84
_020C118C: .word _02110B90
_020C1190: .word _02110B60
_020C1194: .word 0x00151110
_020C1198: .word 0x04000400
_020C119C: .word NNS_G3dGlb
_020C11A0: .word NNS_G3dGlb + 0x4C
_020C11A4: .word 0x00171012
_020C11A8: .word _02110B58
_020C11AC: .word 0x00001B19
_020C11B0: .word _02110B54
	arm_func_end NNSi_G3dFuncSbc_BBY

	arm_func_start NNSi_G3dFuncSbc_NODEMIX
NNSi_G3dFuncSbc_NODEMIX: ; 0x020C11B4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x104
	ldr r1, [r0, #4]
	ldr r6, [r0]
	ldr r7, [r1, #4]
	ldrb r2, [r6, #2]
	ldr r3, [r7, #0x10]
	mov r5, #0
	str r0, [sp]
	str r2, [sp, #0x10]
	add r3, r7, r3
	add r1, sp, #0xb0
	mov r0, r5
	mov r2, #0x54
	mov r4, r5
	str r3, [sp, #0x14]
	add r6, r6, #3
	bl MIi_CpuClearFast
	bl NNS_G3dGeFlushBuffer
	mov r0, r5
	ldr r1, _020C1900 ; =0x04000440
	str r0, [sp, #0xc]
	str r0, [r1]
	mov r0, #1
	str r0, [r1, #0xc]
	ldr r0, [sp, #0xc]
	str r0, [r1, #0x14]
	mov r0, #2
	str r0, [r1]
	ldr r0, [sp, #0x10]
	cmp r0, #0
	bls _020C16A4
_020C1234:
	ldrb fp, [r6, #1]
	mov r1, #0x64
	ldr r0, [sp]
	smulbb r1, fp, r1
	mov r2, fp, lsr #5
	str r1, [sp, #4]
	add r0, r0, r2, lsl #2
	ldr r3, _020C1904 ; =_021DBB04
	ldr r2, [sp, #4]
	ldr sb, [r0, #0xcc]
	add r7, r3, r2
	and r1, fp, #0x1f
	mov r2, #1
	ands r3, sb, r2, lsl r1
	str r3, [sp, #8]
	bne _020C12A0
	ldr sb, [r0, #0xcc]
	ldr r3, _020C1908 ; =0x04000450
	orr r1, sb, r2, lsl r1
	str r1, [r0, #0xcc]
	ldr r0, [sp, #0x14]
	mov r1, #0x54
	mla r0, fp, r1, r0
	ldrb sb, [r6]
	str sb, [r3]
	str r2, [r3, #-0x10]
	bl G3_MultMtx43
_020C12A0:
	ldr r0, [sp, #0xc]
	cmp r0, #0
	beq _020C14C8
	ldr r0, [sp, #0xe0]
	ldmia r8, {sb, lr}
	str r0, [sp, #0x18]
	mov r0, sb, asr #0x1f
	str r0, [sp, #0x20]
	ldr r0, [sp, #0xe4]
	ldr ip, [r8, #8]
	str r0, [sp, #0x24]
	mov r0, lr, asr #0x1f
	str r0, [sp, #0x28]
	ldr r0, [sp, #0xe8]
	ldr r3, [r8, #0xc]
	str r0, [sp, #0x2c]
	ldr r0, [sp, #0xec]
	ldr r2, [r8, #0x10]
	str r0, [sp, #0x34]
	mov r0, ip, asr #0x1f
	str r0, [sp, #0x30]
	ldr r0, [sp, #0xf0]
	ldr r1, [r8, #0x14]
	str r0, [sp, #0x3c]
	mov r0, r3, asr #0x1f
	str r0, [sp, #0x38]
	ldr r0, [sp, #0xf4]
	str r0, [sp, #0x44]
	mov r0, r2, asr #0x1f
	str r0, [sp, #0x40]
	ldr r0, [sp, #0xf8]
	str r0, [sp, #0x4c]
	mov r0, r1, asr #0x1f
	str r0, [sp, #0x48]
	ldr r0, [r8, #0x18]
	str r0, [sp, #0x50]
	ldr r0, [sp, #0xfc]
	str r0, [sp, #0x58]
	ldr r0, [sp, #0x50]
	mov r0, r0, asr #0x1f
	str r0, [sp, #0x54]
	ldr r0, [r8, #0x1c]
	str r0, [sp, #0xa8]
	ldr r0, [r8, #0x20]
	ldr r8, [sp, #0xa8]
	mov r8, r8, asr #0x1f
	str r8, [sp, #0x5c]
	ldr r8, [sp, #0x100]
	str r8, [sp, #0x60]
	mov r8, r0, asr #0x1f
	str r8, [sp, #0x64]
	umull sl, r8, r5, sb
	str sl, [sp, #0x1c]
	ldr sl, [sp, #0x20]
	mla r8, r5, sl, r8
	mla r8, r4, sb, r8
	ldr sb, [sp, #0x1c]
	mov sb, sb, lsr #0xc
	orr sb, sb, r8, lsl #20
	ldr r8, [sp, #0x18]
	adds r8, r8, sb
	str r8, [sp, #0xe0]
	umull sl, sb, r5, lr
	ldr r8, [sp, #0x28]
	mov sl, sl, lsr #0xc
	mla sb, r5, r8, sb
	mla sb, r4, lr, sb
	ldr r8, [sp, #0x24]
	orr sl, sl, sb, lsl #20
	adds r8, r8, sl
	str r8, [sp, #0xe4]
	umull sl, sb, r5, ip
	ldr r8, [sp, #0x30]
	mov sl, sl, lsr #0xc
	mla sb, r5, r8, sb
	mla sb, r4, ip, sb
	ldr r8, [sp, #0x2c]
	orr sl, sl, sb, lsl #20
	adds r8, r8, sl
	str r8, [sp, #0xe8]
	ldr r8, [sp, #0x38]
	umull sl, sb, r5, r3
	mla sb, r5, r8, sb
	mla sb, r4, r3, sb
	mov r8, sl, lsr #0xc
	ldr r3, [sp, #0x34]
	orr r8, r8, sb, lsl #20
	adds r3, r3, r8
	str r3, [sp, #0xec]
	ldr r3, [sp, #0x40]
	umull sb, r8, r5, r2
	mla r8, r5, r3, r8
	mla r8, r4, r2, r8
	mov r3, sb, lsr #0xc
	ldr r2, [sp, #0x3c]
	orr r3, r3, r8, lsl #20
	adds r2, r2, r3
	str r2, [sp, #0xf0]
	ldr r2, [sp, #0x48]
	umull r8, r3, r5, r1
	mla r3, r5, r2, r3
	mla r3, r4, r1, r3
	mov r2, r8, lsr #0xc
	ldr r1, [sp, #0x44]
	orr r2, r2, r3, lsl #20
	adds r1, r1, r2
	str r1, [sp, #0xf4]
	ldr r1, [sp, #0x50]
	umull r3, r2, r5, r1
	ldr r1, [sp, #0x54]
	mov r3, r3, lsr #0xc
	mla r2, r5, r1, r2
	ldr r1, [sp, #0x50]
	mla r2, r4, r1, r2
	ldr r1, [sp, #0x4c]
	orr r3, r3, r2, lsl #20
	adds r1, r1, r3
	str r1, [sp, #0xf8]
	ldr r1, [sp, #0xa8]
	umull r3, r2, r5, r1
	ldr r1, [sp, #0x5c]
	mov r3, r3, lsr #0xc
	mla r2, r5, r1, r2
	ldr r1, [sp, #0xa8]
	mla r2, r4, r1, r2
	ldr r1, [sp, #0x58]
	orr r3, r3, r2, lsl #20
	adds r1, r1, r3
	str r1, [sp, #0xfc]
	ldr r1, [sp, #0x64]
	umull r3, r2, r5, r0
	mla r2, r5, r1, r2
	mla r2, r4, r0, r2
	mov r1, r3, lsr #0xc
	ldr r0, [sp, #0x60]
	orr r1, r1, r2, lsl #20
	adds r0, r0, r1
	str r0, [sp, #0x100]
_020C14C8:
	ldr r0, [sp, #8]
	cmp r0, #0
	bne _020C1504
_020C14D4:
	mov r0, r7
	bl G3X_GetClipMtx
	cmp r0, #0
	bne _020C14D4
	ldr r0, [sp, #0x14]
	mov r1, #0x54
	mla r0, fp, r1, r0
	ldr r1, _020C1900 ; =0x04000440
	mov r2, #2
	add r0, r0, #0x30
	str r2, [r1]
	bl G3_MultMtx33
_020C1504:
	ldrb r0, [r6, #2]
	ldr r2, [r7]
	ldr r1, [r7, #4]
	mov r5, r0, lsl #4
	ldr r0, [r7, #8]
	ldr r4, [r7, #0x10]
	ldr r3, [r7, #0x14]
	smull r2, r8, r5, r2
	mov r2, r2, lsr #0xc
	orr r2, r2, r8, lsl #20
	smull r1, r8, r5, r1
	mov r1, r1, lsr #0xc
	orr r1, r1, r8, lsl #20
	smull r0, r8, r5, r0
	mov r0, r0, lsr #0xc
	orr r0, r0, r8, lsl #20
	smull r8, r4, r5, r4
	mov sb, r8, lsr #0xc
	orr sb, sb, r4, lsl #20
	smull r4, r3, r5, r3
	mov r8, r4, lsr #0xc
	orr r8, r8, r3, lsl #20
	mov r4, r5, asr #0x1f
	ldr sl, [sp, #0xb0]
	ldr r3, [sp, #0xb4]
	adds r2, sl, r2
	adds r1, r3, r1
	str r2, [sp, #0xb0]
	str r1, [sp, #0xb4]
	ldr r2, [sp, #0xb8]
	ldr r1, [sp, #0xbc]
	adds r2, r2, r0
	adds r0, r1, sb
	str r2, [sp, #0xb8]
	str r0, [sp, #0xbc]
	ldr r0, [sp, #0xc0]
	ldr r2, [sp, #0xc4]
	adds r0, r0, r8
	str r0, [sp, #0xc0]
	ldr r0, [r7, #0x18]
	ldr r3, [sp, #0xc8]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	adds r0, r2, r1
	str r0, [sp, #0xc4]
	ldr r0, [r7, #0x20]
	ldr r2, [sp, #0xcc]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	adds r0, r3, r1
	str r0, [sp, #0xc8]
	ldr r0, [r7, #0x24]
	ldr r3, [sp, #0xd0]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	adds r0, r2, r1
	str r0, [sp, #0xcc]
	ldr r0, [r7, #0x28]
	ldr r2, [sp, #0xd4]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	adds r0, r3, r1
	str r0, [sp, #0xd0]
	ldr r0, [r7, #0x30]
	ldr r3, [sp, #0xd8]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	adds r0, r2, r1
	str r0, [sp, #0xd4]
	ldr r1, [r7, #0x34]
	ldr r0, [r7, #0x38]
	smull r2, r1, r5, r1
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	adds r1, r3, r2
	str r1, [sp, #0xd8]
	smull r1, r0, r5, r0
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	ldr r0, [sp, #0xdc]
	adds r0, r0, r1
	str r0, [sp, #0xdc]
	ldr r1, _020C190C ; =_021DBB44
	ldr r0, [sp, #8]
	cmp r0, #0
	add r6, r6, #3
	ldr r0, [sp, #4]
	add r8, r1, r0
	bne _020C168C
_020C167C:
	mov r0, r8
	bl G3X_GetVectorMtx
	cmp r0, #0
	bne _020C167C
_020C168C:
	ldr r0, [sp, #0xc]
	add r1, r0, #1
	ldr r0, [sp, #0x10]
	str r1, [sp, #0xc]
	cmp r1, r0
	blo _020C1234
_020C16A4:
	ldmia r8, {r0, fp}
	ldr sl, [r8, #8]
	ldr sb, [r8, #0xc]
	ldr r7, [r8, #0x10]
	ldr r6, [r8, #0x14]
	ldr r3, [r8, #0x18]
	ldr r2, [r8, #0x1c]
	ldr r1, [r8, #0x20]
	umull ip, r8, r5, r0
	mov lr, ip, lsr #0xc
	ldr ip, [sp, #0xe0]
	str ip, [sp, #0x68]
	mov ip, r0, asr #0x1f
	mla r8, r5, ip, r8
	mla r8, r4, r0, r8
	orr lr, lr, r8, lsl #20
	umull ip, r8, r5, fp
	ldr r0, [sp, #0x68]
	str ip, [sp, #0x6c]
	adds r0, r0, lr
	str r0, [sp, #0xe0]
	mov r0, fp, asr #0x1f
	str r0, [sp, #0xac]
	ldr ip, [sp, #0xac]
	ldr r0, [sp, #0xe8]
	mla r8, r5, ip, r8
	mla r8, r4, fp, r8
	ldr fp, [sp, #0x6c]
	str r0, [sp, #0x70]
	mov fp, fp, lsr #0xc
	mov r0, sl, asr #0x1f
	str r0, [sp, #0x74]
	ldr r0, [sp, #0xec]
	ldr lr, [sp, #0xe4]
	orr fp, fp, r8, lsl #20
	adds r8, lr, fp
	str r8, [sp, #0xe4]
	str r0, [sp, #0x78]
	mov r0, sb, asr #0x1f
	str r0, [sp, #0x7c]
	ldr r0, [sp, #0xf0]
	ldr r8, [sp, #0x74]
	umull ip, fp, r5, sl
	mla fp, r5, r8, fp
	mla fp, r4, sl, fp
	mov sl, ip, lsr #0xc
	str r0, [sp, #0x80]
	mov r0, r7, asr #0x1f
	str r0, [sp, #0x84]
	ldr r0, [sp, #0xf4]
	ldr r8, [sp, #0x70]
	orr sl, sl, fp, lsl #20
	adds r8, r8, sl
	str r8, [sp, #0xe8]
	str r0, [sp, #0x88]
	mov r0, r6, asr #0x1f
	str r0, [sp, #0x8c]
	ldr r0, [sp, #0xf8]
	ldr r8, [sp, #0x7c]
	umull fp, sl, r5, sb
	mla sl, r5, r8, sl
	mla sl, r4, sb, sl
	mov sb, fp, lsr #0xc
	str r0, [sp, #0x90]
	mov r0, r3, asr #0x1f
	str r0, [sp, #0x94]
	ldr r0, [sp, #0xfc]
	ldr r8, [sp, #0x78]
	orr sb, sb, sl, lsl #20
	adds r8, r8, sb
	str r8, [sp, #0xec]
	str r0, [sp, #0x98]
	mov r0, r2, asr #0x1f
	str r0, [sp, #0x9c]
	ldr r0, [sp, #0x100]
	ldr r8, [sp, #0x84]
	umull sl, sb, r5, r7
	mla sb, r5, r8, sb
	mla sb, r4, r7, sb
	mov r8, sl, lsr #0xc
	str r0, [sp, #0xa0]
	mov r0, r1, asr #0x1f
	str r0, [sp, #0xa4]
	ldr r7, [sp, #0x80]
	orr r8, r8, sb, lsl #20
	adds r7, r7, r8
	str r7, [sp, #0xf0]
	ldr r7, [sp, #0x8c]
	umull sb, r8, r5, r6
	mla r8, r5, r7, r8
	mla r8, r4, r6, r8
	mov r7, sb, lsr #0xc
	ldr r6, [sp, #0x88]
	orr r7, r7, r8, lsl #20
	adds r6, r6, r7
	str r6, [sp, #0xf4]
	ldr r6, [sp, #0x94]
	umull r8, r7, r5, r3
	mla r7, r5, r6, r7
	mla r7, r4, r3, r7
	mov r6, r8, lsr #0xc
	ldr r3, [sp, #0x90]
	orr r6, r6, r7, lsl #20
	adds r3, r3, r6
	str r3, [sp, #0xf8]
	ldr r3, [sp, #0x9c]
	umull r7, r6, r5, r2
	mla r6, r5, r3, r6
	mla r6, r4, r2, r6
	mov r3, r7, lsr #0xc
	ldr r2, [sp, #0x98]
	orr r3, r3, r6, lsl #20
	adds r2, r2, r3
	str r2, [sp, #0xfc]
	ldr r2, [sp, #0xa4]
	umull r6, r3, r5, r1
	mla r3, r5, r2, r3
	mla r3, r4, r1, r3
	mov r2, r6, lsr #0xc
	ldr r1, [sp, #0xa0]
	orr r2, r2, r3, lsl #20
	adds r1, r1, r2
	add r0, sp, #0xe0
	str r1, [sp, #0x100]
	bl G3_LoadMtx43
	ldr r1, _020C1900 ; =0x04000440
	mov r2, #1
	add r0, sp, #0xb0
	str r2, [r1]
	bl G3_LoadMtx43
	ldr r1, _020C1900 ; =0x04000440
	mov r2, #0
	mov r0, #1
	str r2, [r1]
	str r0, [r1, #0x10]
	mov r0, #2
	str r0, [r1]
	ldr r0, [sp]
	ldr r0, [r0]
	ldrb r0, [r0, #1]
	str r0, [r1, #0xc]
	ldr r0, [sp]
	ldr r1, [r0]
	ldrb r0, [r1, #2]
	add r0, r0, #1
	add r0, r0, r0, lsl #1
	add r1, r1, r0
	ldr r0, [sp]
	str r1, [r0]
	add sp, sp, #0x104
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C1900: .word 0x04000440
_020C1904: .word _021DBB04
_020C1908: .word 0x04000450
_020C190C: .word _021DBB44
	arm_func_end NNSi_G3dFuncSbc_NODEMIX

	arm_func_start NNSi_G3dFuncSbc_CALLDL
NNSi_G3dFuncSbc_CALLDL: ; 0x020C1910
	stmdb sp!, {r4, r5, r6, lr}
	mov r5, r0
	ldr r0, [r5, #0x34]
	cmp r0, #0
	ldrneb r4, [r5, #0x96]
	moveq r4, #0
	cmp r4, #1
	bne _020C1964
	ldr r1, [r5, #8]
	mov r0, r5
	bic r1, r1, #0x40
	str r1, [r5, #8]
	ldr r1, [r5, #0x34]
	blx r1
	ldr r0, [r5, #0x34]
	cmp r0, #0
	ldrneb r4, [r5, #0x96]
	ldr r0, [r5, #8]
	moveq r4, #0
	and r1, r0, #0x40
	b _020C1968
_020C1964:
	mov r1, #0
_020C1968:
	ldr r0, [r5, #8]
	tst r0, #0x100
	cmpeq r1, #0
	bne _020C19BC
	ldr r0, [r5]
	ldrb ip, [r0, #1]
	ldrb r3, [r0, #2]
	ldrb lr, [r0, #3]
	ldrb r2, [r0, #5]
	ldrb r1, [r0, #6]
	orr r3, ip, r3, lsl #8
	ldrb r6, [r0, #4]
	orr ip, r3, lr, lsl #16
	ldrb r3, [r0, #7]
	orr r1, r2, r1, lsl #8
	orr r6, ip, r6, lsl #24
	ldrb r2, [r0, #8]
	orr r1, r1, r3, lsl #16
	add r0, r0, r6
	orr r1, r1, r2, lsl #24
	bl NNS_G3dGeSendDL
_020C19BC:
	cmp r4, #3
	bne _020C19DC
	ldr r1, [r5, #8]
	mov r0, r5
	bic r1, r1, #0x40
	str r1, [r5, #8]
	ldr r1, [r5, #0x34]
	blx r1
_020C19DC:
	ldr r0, [r5]
	add r0, r0, #9
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end NNSi_G3dFuncSbc_CALLDL

	arm_func_start NNSi_G3dFuncSbc_POSSCALE
NNSi_G3dFuncSbc_POSSCALE: ; 0x020C19EC
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	mov r4, r0
	ldr r0, [r4, #8]
	tst r0, #0x100
	bne _020C1A48
	tst r0, #0x200
	bne _020C1A48
	cmp r1, #0
	bne _020C1A28
	ldr r0, [r4, #0xe0]
	str r0, [sp, #8]
	str r0, [sp, #4]
	str r0, [sp]
	b _020C1A38
_020C1A28:
	ldr r0, [r4, #0xe4]
	str r0, [sp, #8]
	str r0, [sp, #4]
	str r0, [sp]
_020C1A38:
	add r1, sp, #0
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C1A48:
	ldr r0, [r4]
	add r0, r0, #1
	str r0, [r4]
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end NNSi_G3dFuncSbc_POSSCALE

	arm_func_start NNSi_G3dFuncSbc_ENVMAP
NNSi_G3dFuncSbc_ENVMAP: ; 0x020C1A5C
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x44
	mov r7, r0
	ldr r0, [r7, #8]
	tst r0, #0x200
	bne _020C1D78
	tst r0, #1
	beq _020C1D78
	ldr r2, [r7, #0xb0]
	ldr r0, [r2, #0x10]
	and r1, r0, #0xc0000000
	cmp r1, #0x80000000
	beq _020C1AC8
	bic r0, r0, #0xc0000000
	str r0, [r2, #0x10]
	ldr r3, [r7, #0xb0]
	ldr r0, _020C1D8C ; =_02110A9C
	ldr r2, [r3, #0x10]
	ldr r1, _020C1D90 ; =_02110AA8
	orr r2, r2, #0x80000000
	str r2, [r3, #0x10]
	ldr r3, [r7, #0xb0]
	mov r2, #1
	ldr r3, [r3, #0x10]
	str r3, [r0, #0xc]
	ldr r0, [r0, #8]
	bl NNS_G3dGeBufferOP_N
_020C1AC8:
	mov r0, #3
	str r0, [sp, #0x10]
	add r1, sp, #0x10
	mov r0, #0x10
	mov r2, #1
	bl NNS_G3dGeBufferOP_N
	ldr r0, [r7, #0x3c]
	cmp r0, #0
	ldrneb r6, [r7, #0x98]
	moveq r6, #0
	cmp r6, #1
	bne _020C1B2C
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x3c]
	blx r1
	ldr r0, [r7, #0x3c]
	cmp r0, #0
	ldrneb r6, [r7, #0x98]
	ldr r0, [r7, #8]
	moveq r6, #0
	and r0, r0, #0x40
	b _020C1B30
_020C1B2C:
	mov r0, #0
_020C1B30:
	cmp r0, #0
	bne _020C1BA8
	ldr r0, [r7, #0xb0]
	add r1, sp, #0x14
	ldrh r5, [r0, #0x2e]
	ldrh r4, [r0, #0x2c]
	mov r3, #0x10000
	rsb r0, r5, #0
	mov ip, r0, lsl #0xf
	mov lr, r4, lsl #0xf
	mov r0, #0x1b
	mov r2, #3
	str lr, [sp, #0x14]
	str ip, [sp, #0x18]
	str r3, [sp, #0x1c]
	bl NNS_G3dGeBufferOP_N
	mov r0, r5, lsl #0x13
	mov r1, r0, asr #0x10
	mov r0, r4, lsl #0x13
	mov r1, r1, lsl #0x10
	mov r0, r0, asr #0x10
	mov r1, r1, lsr #0x10
	mov r0, r0, lsl #0x10
	mov r1, r1, lsl #0x10
	orr r3, r1, r0, lsr #16
	add r1, sp, #0xc
	mov r0, #0x22
	mov r2, #1
	str r3, [sp, #0xc]
	bl NNS_G3dGeBufferOP_N
_020C1BA8:
	cmp r6, #2
	bne _020C1BE4
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x3c]
	blx r1
	ldr r0, [r7, #0x3c]
	cmp r0, #0
	ldrneb r6, [r7, #0x98]
	ldr r0, [r7, #8]
	moveq r6, #0
	and r0, r0, #0x40
	b _020C1BE8
_020C1BE4:
	mov r0, #0
_020C1BE8:
	cmp r0, #0
	bne _020C1C7C
	ldr r0, [r7]
	ldr r4, [r7, #0xd8]
	ldrb r3, [r0, #1]
	cmp r4, #0
	beq _020C1C44
	adds r2, r4, #4
	beq _020C1C30
	ldrb r0, [r4, #5]
	cmp r3, r0
	bhs _020C1C30
	ldrh r1, [r4, #0xa]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r3, r1
	b _020C1C34
_020C1C30:
	mov r1, #0
_020C1C34:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, r4, r0
	bne _020C1C48
_020C1C44:
	mov r0, #0
_020C1C48:
	ldrh r2, [r0, #0x1e]
	tst r2, #0x2000
	beq _020C1C7C
	add r1, r0, #0x2c
	tst r2, #2
	addeq r1, r1, #8
	tst r2, #4
	addeq r1, r1, #4
	tst r2, #8
	addeq r1, r1, #8
	mov r0, #0x18
	mov r2, #0x10
	bl NNS_G3dGeBufferOP_N
_020C1C7C:
	cmp r6, #3
	movne r0, #0
	bne _020C1CA8
	ldr r1, [r7, #8]
	mov r0, r7
	bic r1, r1, #0x40
	str r1, [r7, #8]
	ldr r1, [r7, #0x3c]
	blx r1
	ldr r0, [r7, #8]
	and r0, r0, #0x40
_020C1CA8:
	cmp r0, #0
	bne _020C1D60
	mov r3, #2
	add r1, sp, #8
	mov r0, #0x10
	mov r2, #1
	str r3, [sp, #8]
	bl NNS_G3dGeBufferOP_N
	add r1, sp, #0x20
	mov r0, #0
	bl NNS_G3dGetCurrentMtx
	mov r3, #3
	add r1, sp, #4
	mov r0, #0x10
	mov r2, #1
	str r3, [sp, #4]
	bl NNS_G3dGeBufferOP_N
	ldr r0, _020C1D94 ; =NNS_G3dGlb
	mov r2, #9
	ldr r0, [r0, #0xfc]
	tst r0, #1
	beq _020C1D30
	ldr r1, _020C1D98 ; =NNS_G3dGlb + 0x4C
	mov r0, #0x1a
	bl NNS_G3dGeBufferOP_N
	ldr r1, _020C1D9C ; =NNS_G3dGlb + 0xBC
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	add r1, sp, #0x20
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	b _020C1D60
_020C1D30:
	tst r0, #2
	mov r0, #0x1a
	beq _020C1D58
	ldr r1, _020C1D98 ; =NNS_G3dGlb + 0x4C
	bl NNS_G3dGeBufferOP_N
	add r1, sp, #0x20
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	b _020C1D60
_020C1D58:
	add r1, sp, #0x20
	bl NNS_G3dGeBufferOP_N
_020C1D60:
	mov r3, #2
	add r1, sp, #0
	mov r0, #0x10
	mov r2, #1
	str r3, [sp]
	bl NNS_G3dGeBufferOP_N
_020C1D78:
	ldr r0, [r7]
	add r0, r0, #3
	str r0, [r7]
	add sp, sp, #0x44
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_020C1D8C: .word _02110A9C
_020C1D90: .word _02110AA8
_020C1D94: .word NNS_G3dGlb
_020C1D98: .word NNS_G3dGlb + 0x4C
_020C1D9C: .word NNS_G3dGlb + 0xBC
	arm_func_end NNSi_G3dFuncSbc_ENVMAP

	arm_func_start NNSi_G3dFuncSbc_PRJMAP
NNSi_G3dFuncSbc_PRJMAP: ; 0x020C1DA0
	stmdb sp!, {r3, r4, r5, lr}
	sub sp, sp, #0x80
	mov r5, r0
	ldr r0, [r5, #8]
	tst r0, #0x200
	bne _020C211C
	tst r0, #1
	beq _020C211C
	add r0, sp, #0x50
	mov r1, #0
	bl NNS_G3dGetCurrentMtx
	mov r0, #0x1e
	str r0, [sp, #0xc]
	add r1, sp, #0xc
	mov r0, #0x13
	mov r2, #1
	bl NNS_G3dGeBufferOP_N
	ldr r2, [r5, #0xb0]
	ldr r0, [r2, #0x10]
	and r1, r0, #0xc0000000
	cmp r1, #0xc0000000
	beq _020C1E30
	bic r0, r0, #0xc0000000
	str r0, [r2, #0x10]
	ldr r3, [r5, #0xb0]
	ldr r0, _020C2130 ; =_02110A9C
	ldr r2, [r3, #0x10]
	ldr r1, _020C2134 ; =_02110AA0
	orr r2, r2, #0xc0000000
	str r2, [r3, #0x10]
	ldr r3, [r5, #0xb0]
	mov r2, #1
	ldr r3, [r3, #0x10]
	str r3, [r0, #4]
	ldr r0, [r0]
	bl NNS_G3dGeBufferOP_N
_020C1E30:
	ldr r0, [r5, #0x40]
	cmp r0, #0
	ldrneb r4, [r5, #0x99]
	moveq r4, #0
	cmp r4, #1
	bne _020C1E7C
	ldr r1, [r5, #8]
	mov r0, r5
	bic r1, r1, #0x40
	str r1, [r5, #8]
	ldr r1, [r5, #0x40]
	blx r1
	ldr r0, [r5, #0x40]
	cmp r0, #0
	ldrneb r4, [r5, #0x99]
	ldr r0, [r5, #8]
	moveq r4, #0
	and r0, r0, #0x40
	b _020C1E80
_020C1E7C:
	mov r0, #0
_020C1E80:
	cmp r0, #0
	bne _020C1EC8
	ldr r0, [r5, #0xb0]
	ldr r3, _020C2130 ; =_02110A9C
	ldrh ip, [r0, #0x2e]
	ldrh r2, [r0, #0x2c]
	ldr r1, _020C2138 ; =_02110ACC
	rsb r0, ip, #0
	mov r2, r2, lsl #0xf
	str r2, [r3, #0x30]
	mov r0, r0, lsl #0xf
	str r0, [r3, #0x44]
	str r2, [r3, #0x60]
	mov ip, ip, lsl #0xf
	mov r0, #0x16
	mov r2, #0x10
	str ip, [r3, #0x64]
	bl NNS_G3dGeBufferOP_N
_020C1EC8:
	cmp r4, #2
	bne _020C1F04
	ldr r1, [r5, #8]
	mov r0, r5
	bic r1, r1, #0x40
	str r1, [r5, #8]
	ldr r1, [r5, #0x40]
	blx r1
	ldr r0, [r5, #0x40]
	cmp r0, #0
	ldrneb r4, [r5, #0x99]
	ldr r0, [r5, #8]
	moveq r4, #0
	and r0, r0, #0x40
	b _020C1F08
_020C1F04:
	mov r0, #0
_020C1F08:
	cmp r0, #0
	bne _020C1F9C
	ldr r0, [r5]
	ldr ip, [r5, #0xd8]
	ldrb r3, [r0, #1]
	cmp ip, #0
	beq _020C1F64
	adds r2, ip, #4
	beq _020C1F50
	ldrb r0, [ip, #5]
	cmp r3, r0
	bhs _020C1F50
	ldrh r1, [ip, #0xa]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r3, r1
	b _020C1F54
_020C1F50:
	mov r1, #0
_020C1F54:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, ip, r0
	bne _020C1F68
_020C1F64:
	mov r0, #0
_020C1F68:
	ldrh r2, [r0, #0x1e]
	tst r2, #0x2000
	beq _020C1F9C
	add r1, r0, #0x2c
	tst r2, #2
	addeq r1, r1, #8
	tst r2, #4
	addeq r1, r1, #4
	tst r2, #8
	addeq r1, r1, #8
	mov r0, #0x18
	mov r2, #0x10
	bl NNS_G3dGeBufferOP_N
_020C1F9C:
	cmp r4, #3
	movne r0, #0
	bne _020C1FC8
	ldr r1, [r5, #8]
	mov r0, r5
	bic r1, r1, #0x40
	str r1, [r5, #8]
	ldr r1, [r5, #0x40]
	blx r1
	ldr r0, [r5, #8]
	and r0, r0, #0x40
_020C1FC8:
	cmp r0, #0
	bne _020C20EC
	ldr r0, _020C213C ; =NNS_G3dGlb
	ldr r0, [r0, #0xfc]
	tst r0, #1
	beq _020C2014
	ldr r1, _020C2140 ; =NNS_G3dGlb + 0xE0
	mov r0, #0x1c
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	ldr r1, _020C2144 ; =NNS_G3dGlb + 0xBC
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	add r1, sp, #0x50
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	b _020C2054
_020C2014:
	tst r0, #2
	beq _020C2030
	add r1, sp, #0x50
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	b _020C2054
_020C2030:
	bl NNS_G3dGlbGetInvV
	mov r1, r0
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	add r1, sp, #0x50
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
_020C2054:
	bl NNS_G3dGeFlushBuffer
	ldr r0, _020C2148 ; =0x04000440
	mov r1, #0
	str r1, [r0]
	str r1, [r0, #4]
	str r1, [r0, #0x14]
	add r4, sp, #0x10
_020C2070:
	mov r0, r4
	bl G3X_GetClipMtx
	cmp r0, #0
	bne _020C2070
	ldr r2, _020C214C ; =0x04000448
	mov r0, #1
	str r0, [r2]
	mov r0, #3
	str r0, [r2, #-8]
	add r1, sp, #0x10
	mov r0, #0x16
	mov r2, #0x10
	bl NNS_G3dGeBufferOP_N
	ldr r0, [sp, #0x44]
	ldr r1, [sp, #0x40]
	mov r0, r0, asr #4
	mov r0, r0, lsl #8
	mov r2, r1, asr #4
	mov r1, r0, asr #0x10
	mov r0, r2, lsl #8
	mov r1, r1, lsl #0x10
	mov r0, r0, asr #0x10
	mov r1, r1, lsr #0x10
	mov r0, r0, lsl #0x10
	mov r1, r1, lsl #0x10
	orr r3, r1, r0, lsr #16
	add r1, sp, #8
	mov r0, #0x22
	mov r2, #1
	str r3, [sp, #8]
	bl NNS_G3dGeBufferOP_N
_020C20EC:
	mov r3, #2
	add r1, sp, #4
	mov r0, #0x10
	mov r2, #1
	str r3, [sp, #4]
	bl NNS_G3dGeBufferOP_N
	mov r3, #0x1e
	add r1, sp, #0
	mov r0, #0x14
	mov r2, #1
	str r3, [sp]
	bl NNS_G3dGeBufferOP_N
_020C211C:
	ldr r0, [r5]
	add r0, r0, #3
	str r0, [r5]
	add sp, sp, #0x80
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C2130: .word _02110A9C
_020C2134: .word _02110AA0
_020C2138: .word _02110ACC
_020C213C: .word NNS_G3dGlb
_020C2140: .word NNS_G3dGlb + 0xE0
_020C2144: .word NNS_G3dGlb + 0xBC
_020C2148: .word 0x04000440
_020C214C: .word 0x04000448
	arm_func_end NNSi_G3dFuncSbc_PRJMAP

	arm_func_start NNS_G3dDraw1Mat1Shp
NNS_G3dDraw1Mat1Shp: ; 0x020C2150
	stmdb sp!, {r4, r5, r6, r7, lr}
	sub sp, sp, #0x6c
	mov r6, r0
	ldr ip, [r6, #0x1c]
	mov r4, r1
	mov r5, r2
	mov r7, r3
	cmp ip, #0x1000
	beq _020C2190
	add r1, sp, #0xc
	mov r0, #0x1b
	mov r2, #3
	str ip, [sp, #0xc]
	str ip, [sp, #0x10]
	str ip, [sp, #0x14]
	bl NNS_G3dGeBufferOP_N
_020C2190:
	cmp r7, #0
	beq _020C233C
	ldrb r0, [r6, #0x18]
	cmp r4, r0
	bhs _020C233C
	cmp r6, #0
	ldrne r0, [r6, #8]
	cmpne r0, #0
	addne r3, r6, r0
	moveq r3, #0
	cmp r3, #0
	beq _020C2200
	adds r2, r3, #4
	beq _020C21EC
	ldrb r0, [r3, #5]
	cmp r4, r0
	bhs _020C21EC
	ldrh r1, [r3, #0xa]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r4, r1
	b _020C21F0
_020C21EC:
	mov r1, #0
_020C21F0:
	cmp r1, #0
	ldrne r0, [r1]
	addne r4, r3, r0
	bne _020C2204
_020C2200:
	mov r4, #0
_020C2204:
	ldr r0, [r4, #0xc]
	tst r0, #0x1f0000
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	ldr r0, _020C23E8 ; =0x00293130
	str r0, [sp, #0x50]
	ldr r0, [r4, #4]
	str r0, [sp, #0x54]
	ldr r0, [r4, #8]
	str r0, [sp, #0x58]
	ldr r1, [r4, #0xc]
	str r1, [sp, #0x5c]
	ldrh r0, [r4, #0x1e]
	tst r0, #0x20
	bicne r0, r1, #0x1f0000
	ldr r1, _020C23EC ; =0x00002B2A
	strne r0, [sp, #0x5c]
	str r1, [sp, #0x60]
	ldr r2, [r4, #0x14]
	ldr r0, [sp, #0x50]
	str r2, [sp, #0x64]
	ldrh r3, [r4, #0x1c]
	add r1, sp, #0x54
	mov r2, #6
	str r3, [sp, #0x68]
	bl NNS_G3dGeBufferOP_N
	ldrh r0, [r4, #0x1e]
	tst r0, #1
	beq _020C233C
	mov r2, #8
	str r2, [sp, #0x18]
	ldrh r1, [r4, #0x20]
	add r0, r4, #0x2c
	strh r1, [sp, #0x44]
	ldrh r1, [r4, #0x22]
	strh r1, [sp, #0x46]
	ldr r1, [r4, #0x24]
	str r1, [sp, #0x48]
	ldr r1, [r4, #0x28]
	str r1, [sp, #0x4c]
	ldrh r1, [r4, #0x1e]
	tst r1, #2
	orrne r1, r2, #1
	strne r1, [sp, #0x18]
	bne _020C22CC
	ldr r1, [r0]
	str r1, [sp, #0x30]
	ldr r1, [r0, #4]
	add r0, r0, #8
	str r1, [sp, #0x34]
_020C22CC:
	ldrh r1, [r4, #0x1e]
	tst r1, #4
	ldrne r1, [sp, #0x18]
	orrne r1, r1, #2
	strne r1, [sp, #0x18]
	bne _020C22F8
	ldrsh r1, [r0]
	strh r1, [sp, #0x38]
	ldrsh r1, [r0, #2]
	add r0, r0, #4
	strh r1, [sp, #0x3a]
_020C22F8:
	ldrh r1, [r4, #0x1e]
	tst r1, #8
	ldrne r0, [sp, #0x18]
	orrne r0, r0, #4
	strne r0, [sp, #0x18]
	bne _020C2320
	ldr r1, [r0]
	str r1, [sp, #0x3c]
	ldr r0, [r0, #4]
	str r0, [sp, #0x40]
_020C2320:
	ldrb r1, [r6, #0x16]
	ldr r0, _020C23F0 ; =_02110A8C
	ldr r1, [r0, r1, lsl #2]
	cmp r1, #0
	beq _020C233C
	add r0, sp, #0x18
	blx r1
_020C233C:
	ldrb r0, [r6, #0x19]
	cmp r5, r0
	bhs _020C23B4
	cmp r6, #0
	ldrne r0, [r6, #0xc]
	cmpne r0, #0
	addne r2, r6, r0
	moveq r2, #0
	cmp r2, #0
	beq _020C23A0
	beq _020C238C
	ldrb r0, [r2, #1]
	cmp r5, r0
	bhs _020C238C
	ldrh r1, [r2, #6]
	ldrh r0, [r2, r1]
	add r1, r2, r1
	add r1, r1, #4
	mla r1, r0, r5, r1
	b _020C2390
_020C238C:
	mov r1, #0
_020C2390:
	cmp r1, #0
	ldrne r0, [r1]
	addne r2, r2, r0
	bne _020C23A4
_020C23A0:
	mov r2, #0
_020C23A4:
	ldr r0, [r2, #8]
	ldr r1, [r2, #0xc]
	add r0, r2, r0
	bl NNS_G3dGeSendDL
_020C23B4:
	ldr r3, [r6, #0x20]
	cmp r3, #0x1000
	addeq sp, sp, #0x6c
	ldmeqia sp!, {r4, r5, r6, r7, pc}
	add r1, sp, #0
	mov r0, #0x1b
	mov r2, #3
	str r3, [sp]
	str r3, [sp, #4]
	str r3, [sp, #8]
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0x6c
	ldmia sp!, {r4, r5, r6, r7, pc}
	.align 2, 0
_020C23E8: .word 0x00293130
_020C23EC: .word 0x00002B2A
_020C23F0: .word _02110A8C
	arm_func_end NNS_G3dDraw1Mat1Shp
