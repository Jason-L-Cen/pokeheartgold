	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public NNSi_G3dAnmCalcNsBca
	.public NNSi_G3dAnmCalcNsBma
	.public NNSi_G3dAnmCalcNsBta
	.public NNSi_G3dAnmCalcNsBtp
	.public NNSi_G3dAnmCalcNsBva
	.public NNSi_G3dAnmObjInitNsBca
	.public NNSi_G3dAnmObjInitNsBma
	.public NNSi_G3dAnmObjInitNsBta
	.public NNSi_G3dAnmObjInitNsBtp
	.public NNSi_G3dAnmObjInitNsBva
	.public NNSi_G3dGetJointScaleBasic
	.public NNSi_G3dGetJointScaleMaya
	.public NNSi_G3dGetJointScaleSi3d
	.public NNSi_G3dSendJointSRTBasic
	.public NNSi_G3dSendJointSRTMaya
	.public NNSi_G3dSendJointSRTSi3d
	.public NNSi_G3dSendTexSRT3dsMax
	.public NNSi_G3dSendTexSRTMaya
	.public NNSi_G3dSendTexSRTSi3d
	.public NNSi_G3dSendTexSRTXsi
	.public texmtxCalc_flagRS_
	.public texmtxCalc_flagRS___3dsmax
	.public texmtxCalc_flagRS___xsi
	.public texmtxCalc_flagR_
	.public texmtxCalc_flagR___3dsmax
	.public texmtxCalc_flagR___xsi
	.public texmtxCalc_flagS_
	.public texmtxCalc_flagS___3dsmax
	.public texmtxCalc_flagS___xsi
	.public texmtxCalc_flagTRS_
	.public texmtxCalc_flagTRS___3dsmax
	.public texmtxCalc_flagTRS___xsi
	.public texmtxCalc_flagTR_
	.public texmtxCalc_flagTR___3dsmax
	.public texmtxCalc_flagTR___xsi
	.public texmtxCalc_flagTS_
	.public texmtxCalc_flagTS___3dsmax
	.public texmtxCalc_flagTS___xsi
	.public texmtxCalc_flagT_
	.public texmtxCalc_flagT___3dsmax
	.public texmtxCalc_flagT___xsi
	.public texmtxCalc_flag_
	.public texmtxCalc_flag___3dsmax
	.public texmtxCalc_flag___xsi
	.extern BeginSleep
	.extern EndSleep
	.extern NNS_G3dGeBufferOP_N
	.extern NNS_G3dGetResDataByName
	.extern NNS_G3dGetResDictIdxByName
	.extern NNSi_G3dGetTexPatAnmFV
	.extern NNSi_G3dGetTexPatAnmPlttNameByIdx
	.extern NNSi_G3dGetTexPatAnmTexNameByIdx
	.extern NNSi_SndCaptureInit
	.extern NNSi_SndInitResourceMgr
	.extern NNSi_SndPlayerInit
	.extern _021094B8
	.extern _02110A04
	.extern _02110A08
	.extern _02110A0C
	.extern _02110A10
	.extern _02110A14
	.extern _02110C1C
	.extern _02110C3C
	.extern _02110C5C
	.extern _021DA700
	.extern _021DB504
	.extern _021DB508
	.extern _021DB50C
	.extern _021DB510
	.extern _021DB514
	.extern _021DB518
	.extern _021DD420
	.extern _021DD430
	.extern _021DD43C

.text
	arm_func_start NNSi_G3dAnmObjInitNsBca
NNSi_G3dAnmObjInitNsBca: ; 0x020C3CEC
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	mov r4, r1
	ldr r0, _020C3D64 ; =_02110A08
	str r4, [r5, #8]
	ldr r0, [r0]
	add r1, r5, #0x1a
	str r0, [r5, #0xc]
	ldrb r2, [r2, #0x17]
	mov r0, #0
	strb r2, [r5, #0x19]
	mov r2, r2, lsl #1
	bl MIi_CpuClear16
	ldrh r0, [r4, #6]
	add r3, r4, #0x14
	mov r2, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, pc}
_020C3D34:
	mov r0, r2, lsl #1
	ldrh r1, [r3, r0]
	add r0, r5, r2, lsl #1
	add r2, r2, #1
	ldr r1, [r4, r1]
	mov r1, r1, lsr #0x18
	orr r1, r1, #0x100
	strh r1, [r0, #0x1a]
	ldrh r0, [r4, #6]
	cmp r2, r0
	blo _020C3D34
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C3D64: .word _02110A08
	arm_func_end NNSi_G3dAnmObjInitNsBca

	arm_func_start NNSi_G3dAnmCalcNsBca
NNSi_G3dAnmCalcNsBca: ; 0x020C3D68
	stmdb sp!, {r3, lr}
	mov r3, r0
	ldr r0, [r1, #8]
	ldr ip, [r1]
	ldrh r1, [r0, #4]
	cmp ip, r1, lsl #12
	mov r1, r1, lsl #0xc
	subge ip, r1, #1
	bge _020C3D94
	cmp ip, #0
	movlt ip, #0
_020C3D94:
	mov r1, r2
	mov r2, ip
	bl getJntSRTAnmResult_
	ldmia sp!, {r3, pc}
	arm_func_end NNSi_G3dAnmCalcNsBca

	arm_func_start getMdlTrans_
getMdlTrans_: ; 0x020C3DA4
	ldr r1, _020C3E38 ; =_021DA700
	ldr r2, [r1]
	ldr r1, [r2]
	ldr ip, [r2, #0xd4]
	ldrb r3, [r1, #1]
	cmp ip, #0
	beq _020C3DFC
	beq _020C3DE8
	ldrb r1, [ip, #1]
	cmp r3, r1
	bhs _020C3DE8
	ldrh r2, [ip, #6]
	ldrh r1, [ip, r2]
	add r2, ip, r2
	add r2, r2, #4
	mla r2, r1, r3, r2
	b _020C3DEC
_020C3DE8:
	mov r2, #0
_020C3DEC:
	cmp r2, #0
	ldrne r1, [r2]
	addne r2, ip, r1
	bne _020C3E00
_020C3DFC:
	mov r2, #0
_020C3E00:
	ldrh r1, [r2]
	tst r1, #1
	beq _020C3E1C
	ldr r1, [r0]
	orr r1, r1, #4
	str r1, [r0]
	bx lr
_020C3E1C:
	ldr r1, [r2, #4]
	str r1, [r0, #0x4c]
	ldr r1, [r2, #8]
	str r1, [r0, #0x50]
	ldr r1, [r2, #0xc]
	str r1, [r0, #0x54]
	bx lr
	.align 2, 0
_020C3E38: .word _021DA700
	arm_func_end getMdlTrans_

	arm_func_start getMdlScale_
getMdlScale_: ; 0x020C3E3C
	stmdb sp!, {r4, lr}
	ldr r1, _020C3EC8 ; =_021DA700
	ldr ip, [r1]
	ldr r2, [ip]
	ldr r4, [ip, #0xd4]
	ldrb lr, [r2, #1]
	cmp r4, #0
	beq _020C3E98
	beq _020C3E84
	ldrb r1, [r4, #1]
	cmp lr, r1
	bhs _020C3E84
	ldrh r3, [r4, #6]
	ldrh r1, [r4, r3]
	add r3, r4, r3
	add r3, r3, #4
	mla r3, r1, lr, r3
	b _020C3E88
_020C3E84:
	mov r3, #0
_020C3E88:
	cmp r3, #0
	ldrne r1, [r3]
	addne r1, r4, r1
	bne _020C3E9C
_020C3E98:
	mov r1, #0
_020C3E9C:
	ldrh r3, [r1], #4
	tst r3, #1
	addeq r1, r1, #0xc
	tst r3, #2
	bne _020C3EBC
	tst r3, #8
	addne r1, r1, #4
	addeq r1, r1, #0x10
_020C3EBC:
	ldr ip, [ip, #0xe8]
	blx ip
	ldmia sp!, {r4, pc}
	.align 2, 0
_020C3EC8: .word _021DA700
	arm_func_end getMdlScale_

	arm_func_start getMdlRot_
getMdlRot_: ; 0x020C3ECC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r1, _020C4038 ; =_021DA700
	mov r8, r0
	ldr r1, [r1]
	ldr r0, [r1]
	ldr r3, [r1, #0xd4]
	ldrb r2, [r0, #1]
	cmp r3, #0
	beq _020C3F2C
	beq _020C3F18
	ldrb r0, [r3, #1]
	cmp r2, r0
	bhs _020C3F18
	ldrh r1, [r3, #6]
	ldrh r0, [r3, r1]
	add r1, r3, r1
	add r1, r1, #4
	mla r1, r0, r2, r1
	b _020C3F1C
_020C3F18:
	mov r1, #0
_020C3F1C:
	cmp r1, #0
	ldrne r0, [r1]
	addne r5, r3, r0
	bne _020C3F30
_020C3F2C:
	mov r5, #0
_020C3F30:
	ldrh r0, [r5]
	add r1, r5, #4
	tst r0, #1
	addeq r1, r1, #0xc
	tst r0, #2
	bne _020C4028
	tst r0, #8
	beq _020C3FDC
	and r2, r0, #0xf0
	add r0, r8, #0x28
	mov r4, r2, asr #4
	ldrsh r7, [r1]
	ldrsh r6, [r1, #2]
	bl MI_Zero36B
	ldrh r0, [r5]
	ldr r1, _020C403C ; =_021094B8
	mov r3, #0x1000
	tst r0, #0x100
	ldr r0, _020C4040 ; =_021094B8 + 1
	ldrb r1, [r1, r4, lsl #2]
	ldrb r0, [r0, r4, lsl #2]
	rsbne r3, r3, #0
	add r2, r8, r4, lsl #2
	str r3, [r2, #0x28]
	add r1, r8, r1, lsl #2
	str r7, [r1, #0x28]
	add r0, r8, r0, lsl #2
	str r6, [r0, #0x28]
	ldrh r0, [r5]
	tst r0, #0x200
	ldr r0, _020C4044 ; =_021094B8 + 2
	rsbne r6, r6, #0
	ldrb r0, [r0, r4, lsl #2]
	add r0, r8, r0, lsl #2
	str r6, [r0, #0x28]
	ldrh r0, [r5]
	tst r0, #0x400
	ldr r0, _020C4048 ; =_021094B8 + 3
	rsbne r7, r7, #0
	ldrb r0, [r0, r4, lsl #2]
	add r0, r8, r0, lsl #2
	str r7, [r0, #0x28]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C3FDC:
	ldrsh r0, [r5, #2]
	str r0, [r8, #0x28]
	ldrsh r0, [r1]
	str r0, [r8, #0x2c]
	ldrsh r0, [r1, #2]
	str r0, [r8, #0x30]
	ldrsh r0, [r1, #4]
	str r0, [r8, #0x34]
	ldrsh r0, [r1, #6]
	str r0, [r8, #0x38]
	ldrsh r0, [r1, #8]
	str r0, [r8, #0x3c]
	ldrsh r0, [r1, #0xa]
	str r0, [r8, #0x40]
	ldrsh r0, [r1, #0xc]
	str r0, [r8, #0x44]
	ldrsh r0, [r1, #0xe]
	str r0, [r8, #0x48]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C4028:
	ldr r0, [r8]
	orr r0, r0, #2
	str r0, [r8]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C4038: .word _021DA700
_020C403C: .word _021094B8
_020C4040: .word _021094B8 + 1
_020C4044: .word _021094B8 + 2
_020C4048: .word _021094B8 + 3
	arm_func_end getMdlRot_

	arm_func_start getJntSRTAnmResult_
getJntSRTAnmResult_: ; 0x020C404C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x30
	mov sb, r0
	add r0, sb, r1, lsl #1
	ldrh r0, [r0, #0x14]
	mov r7, r3
	mov r8, r2
	ldr r4, [sb, r0]
	add r1, sb, r0
	tst r4, #1
	movne r0, #7
	strne r0, [r7]
	bne _020C43C4
	ldr r0, _020C43F8 ; =0x00000FFF
	add r5, r1, #4
	tst r8, r0
	beq _020C40A0
	ldr r0, [sb, #8]
	tst r0, #1
	movne r6, #1
	bne _020C40A4
_020C40A0:
	mov r6, #0
_020C40A4:
	mov r0, #0
	str r0, [r7]
	tst r4, #6
	bne _020C4184
	tst r4, #8
	bne _020C40F0
	cmp r6, #0
	mov r1, r8
	mov r2, r5
	beq _020C40DC
	mov r3, sb
	add r0, r7, #0x4c
	bl getTransDataEx_
	b _020C40E8
_020C40DC:
	mov r3, sb
	add r0, r7, #0x4c
	bl getTransData_
_020C40E8:
	add r5, r5, #8
	b _020C40F8
_020C40F0:
	ldr r0, [r5], #4
	str r0, [r7, #0x4c]
_020C40F8:
	tst r4, #0x10
	bne _020C4134
	cmp r6, #0
	mov r1, r8
	mov r2, r5
	beq _020C4120
	mov r3, sb
	add r0, r7, #0x50
	bl getTransDataEx_
	b _020C412C
_020C4120:
	mov r3, sb
	add r0, r7, #0x50
	bl getTransData_
_020C412C:
	add r5, r5, #8
	b _020C413C
_020C4134:
	ldr r0, [r5], #4
	str r0, [r7, #0x50]
_020C413C:
	tst r4, #0x20
	bne _020C4178
	cmp r6, #0
	mov r1, r8
	mov r2, r5
	beq _020C4164
	mov r3, sb
	add r0, r7, #0x54
	bl getTransDataEx_
	b _020C4170
_020C4164:
	mov r3, sb
	add r0, r7, #0x54
	bl getTransData_
_020C4170:
	add r5, r5, #8
	b _020C41A4
_020C4178:
	ldr r0, [r5], #4
	str r0, [r7, #0x54]
	b _020C41A4
_020C4184:
	tst r4, #2
	beq _020C419C
	ldr r0, [r7]
	orr r0, r0, #4
	str r0, [r7]
	b _020C41A4
_020C419C:
	mov r0, r7
	bl getMdlTrans_
_020C41A4:
	tst r4, #0xc0
	bne _020C4268
	tst r4, #0x100
	bne _020C41E8
	cmp r6, #0
	mov r1, r8
	mov r2, r5
	beq _020C41D4
	mov r3, sb
	add r0, r7, #0x28
	bl getRotDataEx_
	b _020C41E0
_020C41D4:
	mov r3, sb
	add r0, r7, #0x28
	bl getRotData_
_020C41E0:
	add r5, r5, #8
	b _020C4288
_020C41E8:
	ldr r1, [sb, #0xc]
	ldr r2, [sb, #0x10]
	ldr r3, [r5]
	add r0, r7, #0x28
	add r1, sb, r1
	add r2, sb, r2
	bl getRotDataByIdx_
	cmp r0, #0
	beq _020C4260
	ldr ip, [r7, #0x3c]
	ldr r0, [r7, #0x2c]
	ldr r3, [r7, #0x28]
	ldr fp, [r7, #0x38]
	ldr r1, [r7, #0x30]
	ldr r2, [r7, #0x34]
	mul sl, r0, ip
	mul lr, r1, fp
	sub sl, sl, lr
	mov sl, sl, asr #0xc
	mul lr, r1, r2
	mul r1, r3, ip
	sub r1, lr, r1
	mul fp, r3, fp
	mul r2, r0, r2
	sub r0, fp, r2
	str sl, [r7, #0x40]
	mov r1, r1, asr #0xc
	str r1, [r7, #0x44]
	mov r0, r0, asr #0xc
	str r0, [r7, #0x48]
_020C4260:
	add r5, r5, #4
	b _020C4288
_020C4268:
	tst r4, #0x40
	beq _020C4280
	ldr r0, [r7]
	orr r0, r0, #2
	str r0, [r7]
	b _020C4288
_020C4280:
	mov r0, r7
	bl getMdlRot_
_020C4288:
	tst r4, #0x600
	bne _020C439C
	tst r4, #0x800
	bne _020C42D8
	cmp r6, #0
	add r0, sp, #0x10
	mov r1, r8
	beq _020C42B8
	mov r2, r5
	mov r3, sb
	bl getScaleDataEx_
	b _020C42C4
_020C42B8:
	mov r2, r5
	mov r3, sb
	bl getScaleData_
_020C42C4:
	ldr r1, [sp, #0x10]
	ldr r0, [sp, #0x14]
	str r1, [sp, #0x18]
	str r0, [sp, #0x24]
	b _020C42E8
_020C42D8:
	ldr r0, [r5]
	str r0, [sp, #0x18]
	ldr r0, [r5, #4]
	str r0, [sp, #0x24]
_020C42E8:
	tst r4, #0x1000
	bne _020C4330
	cmp r6, #0
	add r0, sp, #8
	mov r1, r8
	beq _020C4310
	mov r3, sb
	add r2, r5, #8
	bl getScaleDataEx_
	b _020C431C
_020C4310:
	mov r3, sb
	add r2, r5, #8
	bl getScaleData_
_020C431C:
	ldr r1, [sp, #8]
	ldr r0, [sp, #0xc]
	str r1, [sp, #0x1c]
	str r0, [sp, #0x28]
	b _020C4340
_020C4330:
	ldr r0, [r5, #8]
	str r0, [sp, #0x1c]
	ldr r0, [r5, #0xc]
	str r0, [sp, #0x28]
_020C4340:
	tst r4, #0x2000
	bne _020C4388
	cmp r6, #0
	add r0, sp, #0
	mov r1, r8
	beq _020C4368
	mov r3, sb
	add r2, r5, #0x10
	bl getScaleDataEx_
	b _020C4374
_020C4368:
	mov r3, sb
	add r2, r5, #0x10
	bl getScaleData_
_020C4374:
	ldr r1, [sp]
	ldr r0, [sp, #4]
	str r1, [sp, #0x20]
	str r0, [sp, #0x2c]
	b _020C43C4
_020C4388:
	ldr r0, [r5, #0x10]
	str r0, [sp, #0x20]
	ldr r0, [r5, #0x14]
	str r0, [sp, #0x2c]
	b _020C43C4
_020C439C:
	tst r4, #0x200
	beq _020C43B4
	ldr r0, [r7]
	orr r0, r0, #1
	str r0, [r7]
	b _020C43C4
_020C43B4:
	mov r0, r7
	bl getMdlScale_
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C43C4:
	ldr r0, [r7]
	add r1, sp, #0x18
	tst r0, #1
	ldr r0, _020C43FC ; =_021DA700
	movne r3, #4
	ldr r4, [r0]
	moveq r3, #0
	ldr r2, [r4]
	ldr r4, [r4, #0xe8]
	mov r0, r7
	blx r4
	add sp, sp, #0x30
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C43F8: .word 0x00000FFF
_020C43FC: .word _021DA700
	arm_func_end getJntSRTAnmResult_

	arm_func_start getTransData_
getTransData_: ; 0x020C4400
	stmdb sp!, {r3, r4, r5, lr}
	ldr r4, [r2, #4]
	ldr ip, [r2]
	mov r5, r1, asr #0xc
	tst ip, #0xc0000000
	add r1, r3, r4
	beq _020C453C
	ldr r2, _020C455C ; =0x1FFF0000
	tst ip, #0x40000000
	and r2, ip, r2
	mov r3, r2, lsr #0x10
	beq _020C4458
	tst r5, #1
	beq _020C4450
	cmp r5, r3
	movhi r2, r3, lsr #1
	addhi r5, r2, #1
	bhi _020C453C
	mov r4, r5, lsr #1
	b _020C44FC
_020C4450:
	mov r5, r5, lsr #1
	b _020C453C
_020C4458:
	ands r2, r5, #3
	beq _020C44F4
	cmp r5, r3
	addhi r5, r2, r3, lsr #2
	bhi _020C453C
	tst r5, #1
	beq _020C44EC
	tst r5, #2
	movne r5, r5, lsr #2
	addne r2, r5, #1
	moveq r2, r5, lsr #2
	addeq r5, r2, #1
	tst ip, #0x20000000
	beq _020C44B4
	mov r3, r2, lsl #1
	mov r2, r5, lsl #1
	ldrsh r3, [r1, r3]
	ldrsh r1, [r1, r2]
	add r2, r3, r3, lsl #1
	add r1, r2, r1
	mov r1, r1, asr #2
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
_020C44B4:
	ldr ip, [r1, r2, lsl #2]
	mov r2, #3
	mov r3, #0
	umull r4, lr, ip, r2
	mla lr, ip, r3, lr
	mov r3, ip, asr #0x1f
	mla lr, r3, r2, lr
	ldr r1, [r1, r5, lsl #2]
	adds r2, r4, r1
	adc r1, lr, r1, asr #31
	mov r2, r2, lsr #2
	orr r2, r2, r1, lsl #30
	str r2, [r0]
	ldmia sp!, {r3, r4, r5, pc}
_020C44EC:
	mov r4, r5, lsr #2
	b _020C44FC
_020C44F4:
	mov r5, r5, lsr #2
	b _020C453C
_020C44FC:
	tst ip, #0x20000000
	beq _020C4520
	mov r3, r4, lsl #1
	add r2, r1, r4, lsl #1
	ldrsh r3, [r1, r3]
	ldrsh r1, [r2, #2]
	add r1, r3, r1
	mov r1, r1, asr #1
	b _020C4534
_020C4520:
	add r2, r1, r4, lsl #2
	ldr r2, [r2, #4]
	ldr r3, [r1, r4, lsl #2]
	mov r1, r2, asr #1
	add r1, r1, r3, asr #1
_020C4534:
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
_020C453C:
	tst ip, #0x20000000
	ldreq r1, [r1, r5, lsl #2]
	streq r1, [r0]
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r2, r5, lsl #1
	ldrsh r1, [r1, r2]
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C455C: .word 0x1FFF0000
	arm_func_end getTransData_

	arm_func_start getTransDataEx_
getTransDataEx_: ; 0x020C4560
	stmdb sp!, {r3, r4, r5, lr}
	ldrh r4, [r3, #4]
	ldr ip, [r2, #4]
	mov lr, r1, asr #0xc
	sub r4, r4, #1
	cmp r4, r1, asr #12
	add ip, r3, ip
	ldr r2, [r2]
	bne _020C45FC
	tst r2, #0xc0000000
	beq _020C45A0
	tst r2, #0x40000000
	andne r4, lr, #1
	addne lr, r4, lr, lsr #1
	andeq r4, lr, #3
	addeq lr, r4, lr, lsr #2
_020C45A0:
	ldr r3, [r3, #8]
	tst r3, #2
	beq _020C45E4
	ldr r3, _020C46B0 ; =0x00000FFF
	tst r2, #0x20000000
	and r2, r1, r3
	ldreq r3, [ip, lr, lsl #2]
	ldreq r1, [ip]
	beq _020C45D0
	mov r1, lr, lsl #1
	ldrsh r3, [ip, r1]
	ldrsh r1, [ip]
_020C45D0:
	sub r1, r1, r3
	mul r1, r2, r1
	add r1, r3, r1, asr #12
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
_020C45E4:
	tst r2, #0x20000000
	movne r1, lr, lsl #1
	ldrnesh r1, [ip, r1]
	ldreq r1, [ip, lr, lsl #2]
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
_020C45FC:
	tst r2, #0xc0000000
	beq _020C4660
	ldr r3, _020C46B4 ; =0x1FFF0000
	tst r2, #0x40000000
	and r4, r2, r3
	mov r4, r4, lsr #0x10
	beq _020C4638
	cmp lr, r4
	movhs lr, r4, lsr #1
	bhs _020C4660
	mov lr, lr, lsr #1
	and r1, r1, r3, lsr #16
	mov r4, #2
	mov r5, #1
	b _020C4670
_020C4638:
	cmp lr, r4
	andhs r3, lr, #3
	addhs lr, r3, lr, lsr #2
	bhs _020C4660
	ldr r3, _020C46B8 ; =0x00003FFF
	mov lr, lr, lsr #2
	and r1, r1, r3
	mov r4, #4
	mov r5, #2
	b _020C4670
_020C4660:
	ldr r3, _020C46B0 ; =0x00000FFF
	mov r4, #1
	and r1, r1, r3
	mov r5, #0
_020C4670:
	tst r2, #0x20000000
	addeq r2, ip, lr, lsl #2
	ldreq ip, [ip, lr, lsl #2]
	ldreq r2, [r2, #4]
	beq _020C4694
	add r2, ip, lr, lsl #1
	mov r3, lr, lsl #1
	ldrsh ip, [ip, r3]
	ldrsh r2, [r2, #2]
_020C4694:
	sub r2, r2, ip
	mul r3, ip, r4
	mul r2, r1, r2
	add r1, r3, r2, asr #12
	mov r1, r1, asr r5
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C46B0: .word 0x00000FFF
_020C46B4: .word 0x1FFF0000
_020C46B8: .word 0x00003FFF
	arm_func_end getTransDataEx_

	arm_func_start getScaleData_
getScaleData_: ; 0x020C46BC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldr r4, [r2, #4]
	ldr r5, [r2]
	mov r6, r1, asr #0xc
	tst r5, #0xc0000000
	add r1, r3, r4
	beq _020C4808
	ldr r2, _020C48AC ; =0x1FFF0000
	tst r5, #0x40000000
	and r2, r5, r2
	mov r3, r2, lsr #0x10
	beq _020C4714
	tst r6, #1
	beq _020C470C
	cmp r6, r3
	movhi r2, r3, lsr #1
	addhi r6, r2, #1
	bhi _020C4808
	mov r4, r6, lsr #1
	b _020C4840
_020C470C:
	mov r6, r6, lsr #1
	b _020C4808
_020C4714:
	ands r2, r6, #3
	beq _020C4804
	cmp r6, r3
	addhi r6, r2, r3, lsr #2
	bhi _020C4808
	tst r6, #1
	beq _020C47FC
	tst r6, #2
	movne r3, r6, lsr #2
	addne r2, r3, #1
	moveq r2, r6, lsr #2
	addeq r3, r2, #1
	tst r5, #0x20000000
	beq _020C4790
	mov r5, r2, lsl #2
	mov r4, r3, lsl #2
	ldrsh r6, [r1, r5]
	ldrsh r5, [r1, r4]
	add r2, r1, r2, lsl #2
	add r4, r6, r6, lsl #1
	add r4, r5, r4
	mov r4, r4, asr #2
	str r4, [r0]
	add r1, r1, r3, lsl #2
	ldrsh r3, [r2, #2]
	ldrsh r2, [r1, #2]
	add r1, r3, r3, lsl #1
	add r1, r2, r1
	mov r1, r1, asr #2
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020C4790:
	ldr r4, [r1, r2, lsl #3]
	mov ip, #3
	mov lr, #0
	umull r6, r5, r4, ip
	mla r5, r4, lr, r5
	mov r4, r4, asr #0x1f
	ldr r7, [r1, r3, lsl #3]
	add r2, r1, r2, lsl #3
	mla r5, r4, ip, r5
	adds r6, r6, r7
	adc r4, r5, r7, asr #31
	mov r5, r6, lsr #2
	orr r5, r5, r4, lsl #30
	str r5, [r0]
	add r1, r1, r3, lsl #3
	ldr r2, [r2, #4]
	ldr r1, [r1, #4]
	umull r4, r3, r2, ip
	mla r3, r2, lr, r3
	mov r2, r2, asr #0x1f
	mla r3, r2, ip, r3
	adds r4, r4, r1
	adc r1, r3, r1, asr #31
	mov r2, r4, lsr #2
	orr r2, r2, r1, lsl #30
	str r2, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020C47FC:
	mov r4, r6, lsr #2
	b _020C4840
_020C4804:
	mov r6, r6, lsr #2
_020C4808:
	tst r5, #0x20000000
	beq _020C4828
	mov r2, r6, lsl #2
	ldrsh r2, [r1, r2]
	add r1, r1, r6, lsl #2
	str r2, [r0]
	ldrsh r1, [r1, #2]
	b _020C4838
_020C4828:
	ldr r2, [r1, r6, lsl #3]
	add r1, r1, r6, lsl #3
	str r2, [r0]
	ldr r1, [r1, #4]
_020C4838:
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020C4840:
	tst r5, #0x20000000
	beq _020C487C
	mov r2, r4, lsl #2
	add r3, r1, r4, lsl #2
	ldrsh r2, [r1, r2]
	ldrsh r1, [r3, #4]
	add r1, r2, r1
	mov r1, r1, asr #1
	str r1, [r0]
	ldrsh r2, [r3, #2]
	ldrsh r1, [r3, #6]
	add r1, r2, r1
	mov r1, r1, asr #1
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020C487C:
	add r3, r1, r4, lsl #3
	ldr r2, [r1, r4, lsl #3]
	ldr r1, [r3, #8]
	add r1, r2, r1
	mov r1, r1, asr #1
	str r1, [r0]
	ldr r2, [r3, #4]
	ldr r1, [r3, #0xc]
	add r1, r2, r1
	mov r1, r1, asr #1
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020C48AC: .word 0x1FFF0000
	arm_func_end getScaleData_

	arm_func_start getScaleDataEx_
getScaleDataEx_: ; 0x020C48B0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	ldrh r4, [r3, #4]
	ldr r5, [r2, #4]
	mov lr, r1, asr #0xc
	sub r4, r4, #1
	cmp r4, r1, asr #12
	add ip, r3, r5
	ldr r2, [r2]
	bne _020C4938
	tst r2, #0xc0000000
	beq _020C48F0
	tst r2, #0x40000000
	andne r4, lr, #1
	addne lr, r4, lr, lsr #1
	andeq r4, lr, #3
	addeq lr, r4, lr, lsr #2
_020C48F0:
	ldr r3, [r3, #8]
	tst r3, #2
	movne r3, #0
	bne _020C49B4
	tst r2, #0x20000000
	beq _020C4920
	mov r1, lr, lsl #2
	ldrsh r2, [ip, r1]
	add r1, ip, lr, lsl #2
	str r2, [r0]
	ldrsh r1, [r1, #2]
	b _020C4930
_020C4920:
	ldr r2, [ip, lr, lsl #3]
	add r1, ip, lr, lsl #3
	str r2, [r0]
	ldr r1, [r1, #4]
_020C4930:
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
_020C4938:
	tst r2, #0xc0000000
	beq _020C49B0
	ldr r4, _020C4A38 ; =0x1FFF0000
	tst r2, #0x40000000
	and r3, r2, r4
	mov r3, r3, lsr #0x10
	beq _020C497C
	cmp lr, r3
	movhs lr, r3, lsr #1
	addhs r3, lr, #1
	bhs _020C49B4
	mov lr, lr, lsr #1
	add r3, lr, #1
	and r1, r1, r4, lsr #16
	mov r4, #2
	mov r5, #1
	b _020C49C4
_020C497C:
	cmp lr, r3
	blo _020C4994
	and r3, lr, #3
	add lr, r3, lr, lsr #2
	add r3, lr, #1
	b _020C49B4
_020C4994:
	ldr r4, _020C4A3C ; =0x00003FFF
	mov lr, lr, lsr #2
	add r3, lr, #1
	and r1, r1, r4
	mov r4, #4
	mov r5, #2
	b _020C49C4
_020C49B0:
	add r3, lr, #1
_020C49B4:
	ldr r5, _020C4A40 ; =0x00000FFF
	mov r4, #1
	and r1, r1, r5
	mov r5, #0
_020C49C4:
	tst r2, #0x20000000
	beq _020C49F0
	mov r2, lr, lsl #2
	add r7, ip, lr, lsl #2
	mov r6, r3, lsl #2
	add r3, ip, r3, lsl #2
	ldrsh r2, [ip, r2]
	ldrsh lr, [r7, #2]
	ldrsh r7, [ip, r6]
	ldrsh r3, [r3, #2]
	b _020C4A04
_020C49F0:
	add r7, ip, lr, lsl #3
	ldmia r7, {r2, lr}
	add r6, ip, r3, lsl #3
	ldr r7, [ip, r3, lsl #3]
	ldr r3, [r6, #4]
_020C4A04:
	sub ip, r7, r2
	sub r3, r3, lr
	mul r6, r2, r4
	mul r2, r1, ip
	add r2, r6, r2, asr #12
	mov r6, r2, asr r5
	mul r4, lr, r4
	mul r2, r1, r3
	add r1, r4, r2, asr #12
	mov r1, r1, asr r5
	str r6, [r0]
	str r1, [r0, #4]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.align 2, 0
_020C4A38: .word 0x1FFF0000
_020C4A3C: .word 0x00003FFF
_020C4A40: .word 0x00000FFF
	arm_func_end getScaleDataEx_

	arm_func_start getRotData_
getRotData_: ; 0x020C4A44
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #0x48
	ldr sb, [r2]
	mov r4, r3
	ldr r2, [r2, #4]
	mov r5, r0
	tst sb, #0xc0000000
	mov r3, r1, asr #0xc
	add r8, r4, r2
	ldr r7, [r4, #0xc]
	ldr r6, [r4, #0x10]
	beq _020C4DC8
	ldr r0, _020C4E58 ; =0x1FFF0000
	tst sb, #0x40000000
	and r0, sb, r0
	mov r1, r0, lsr #0x10
	beq _020C4AB0
	tst r3, #1
	beq _020C4AA8
	cmp r3, r1
	movhi r0, r1, lsr #1
	addhi r3, r0, #1
	bhi _020C4DC8
	mov sb, r3, lsr #1
	b _020C4C70
_020C4AA8:
	mov r3, r3, lsr #1
	b _020C4DC8
_020C4AB0:
	ands r0, r3, #3
	beq _020C4C68
	cmp r3, r1
	addhi r3, r0, r1, lsr #2
	bhi _020C4DC8
	tst r3, #1
	beq _020C4C60
	tst r3, #2
	movne sb, r3, lsr #2
	addne r0, sb, #1
	moveq r0, r3, lsr #2
	addeq sb, r0, #1
	mov r0, r0, lsl #1
	ldrh r3, [r8, r0]
	mov r0, r5
	add r1, r4, r7
	add r2, r4, r6
	mov sl, #0
	bl getRotDataByIdx_
	mov r1, sb, lsl #1
	ldrh r3, [r8, r1]
	orr sl, sl, r0
	add r0, sp, #0x24
	add r1, r4, r7
	add r2, r4, r6
	bl getRotDataByIdx_
	ldr r1, [r5]
	ldr r2, [sp, #0x24]
	add r1, r1, r1, lsl #1
	add r1, r2, r1
	str r1, [r5]
	ldr r1, [r5, #4]
	ldr r2, [sp, #0x28]
	add r1, r1, r1, lsl #1
	add r1, r2, r1
	str r1, [r5, #4]
	ldr r1, [r5, #8]
	ldr r2, [sp, #0x2c]
	add r1, r1, r1, lsl #1
	add r1, r2, r1
	str r1, [r5, #8]
	ldr r1, [r5, #0xc]
	ldr r2, [sp, #0x30]
	add r1, r1, r1, lsl #1
	add r1, r2, r1
	str r1, [r5, #0xc]
	ldr r1, [r5, #0x10]
	orr sl, sl, r0
	ldr r2, [sp, #0x34]
	add r0, r1, r1, lsl #1
	add r0, r2, r0
	str r0, [r5, #0x10]
	ldr r0, [r5, #0x14]
	ldr r1, [sp, #0x38]
	add r0, r0, r0, lsl #1
	add r2, r1, r0
	mov r0, r5
	mov r1, r5
	str r2, [r5, #0x14]
	bl VEC_Normalize
	add r0, r5, #0xc
	mov r1, r0
	bl VEC_Normalize
	cmp sl, #0
	bne _020C4C04
	ldr r0, [r5, #0x18]
	ldr r1, [sp, #0x3c]
	add r0, r0, r0, lsl #1
	add r0, r1, r0
	str r0, [r5, #0x18]
	ldr r0, [r5, #0x1c]
	ldr r1, [sp, #0x40]
	add r0, r0, r0, lsl #1
	add r0, r1, r0
	str r0, [r5, #0x1c]
	ldr r0, [r5, #0x20]
	ldr r1, [sp, #0x44]
	add r0, r0, r0, lsl #1
	add r2, r1, r0
	add r0, r5, #0x18
	mov r1, r0
	str r2, [r5, #0x20]
	bl VEC_Normalize
	add sp, sp, #0x48
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C4C04:
	ldr sb, [r5, #0x14]
	ldr r6, [r5, #4]
	ldr r4, [r5, #0x10]
	ldr r2, [r5, #8]
	mul r1, r6, sb
	mul r0, r2, r4
	sub r0, r1, r0
	ldr r7, [r5, #0xc]
	ldr r8, [r5]
	mov r3, r0, asr #0xc
	mul r1, r2, r7
	mul r0, r8, sb
	sub r2, r1, r0
	mul r1, r8, r4
	mul r0, r6, r7
	sub r0, r1, r0
	str r3, [r5, #0x18]
	mov r1, r2, asr #0xc
	str r1, [r5, #0x1c]
	mov r0, r0, asr #0xc
	add sp, sp, #0x48
	str r0, [r5, #0x20]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C4C60:
	mov sb, r3, lsr #2
	b _020C4C70
_020C4C68:
	mov r3, r3, lsr #2
	b _020C4DC8
_020C4C70:
	mov r0, sb, lsl #1
	ldrh r3, [r8, r0]
	mov r0, r5
	add r1, r4, r7
	add r2, r4, r6
	bl getRotDataByIdx_
	add r1, r8, sb, lsl #1
	ldrh r3, [r1, #2]
	orr r8, r0, #0
	add r0, sp, #0
	add r1, r4, r7
	add r2, r4, r6
	bl getRotDataByIdx_
	ldr r2, [r5]
	ldr r1, [sp]
	orr r8, r8, r0
	add r0, r2, r1
	str r0, [r5]
	ldr r2, [r5, #4]
	ldr r1, [sp, #4]
	mov r0, r5
	add r1, r2, r1
	str r1, [r5, #4]
	ldr r3, [r5, #8]
	ldr r2, [sp, #8]
	mov r1, r5
	add r2, r3, r2
	str r2, [r5, #8]
	ldr r3, [r5, #0xc]
	ldr r2, [sp, #0xc]
	add r2, r3, r2
	str r2, [r5, #0xc]
	ldr r3, [r5, #0x10]
	ldr r2, [sp, #0x10]
	add r2, r3, r2
	str r2, [r5, #0x10]
	ldr r3, [r5, #0x14]
	ldr r2, [sp, #0x14]
	add r2, r3, r2
	str r2, [r5, #0x14]
	bl VEC_Normalize
	add r0, r5, #0xc
	mov r1, r0
	bl VEC_Normalize
	cmp r8, #0
	bne _020C4D6C
	ldr r2, [r5, #0x18]
	ldr r1, [sp, #0x18]
	add r0, r5, #0x18
	add r1, r2, r1
	str r1, [r5, #0x18]
	ldr r3, [r5, #0x1c]
	ldr r2, [sp, #0x1c]
	mov r1, r0
	add r2, r3, r2
	str r2, [r5, #0x1c]
	ldr r3, [r5, #0x20]
	ldr r2, [sp, #0x20]
	add r2, r3, r2
	str r2, [r5, #0x20]
	bl VEC_Normalize
	add sp, sp, #0x48
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C4D6C:
	ldr sb, [r5, #0x14]
	ldr r6, [r5, #4]
	ldr r4, [r5, #0x10]
	ldr r2, [r5, #8]
	mul r1, r6, sb
	mul r0, r2, r4
	sub r0, r1, r0
	ldr r7, [r5, #0xc]
	ldr r8, [r5]
	mov r3, r0, asr #0xc
	mul r1, r2, r7
	mul r0, r8, sb
	sub r2, r1, r0
	mul r1, r8, r4
	mul r0, r6, r7
	sub r0, r1, r0
	str r3, [r5, #0x18]
	mov r1, r2, asr #0xc
	str r1, [r5, #0x1c]
	mov r0, r0, asr #0xc
	add sp, sp, #0x48
	str r0, [r5, #0x20]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C4DC8:
	mov r0, r3, lsl #1
	ldrh r3, [r8, r0]
	mov r0, r5
	add r1, r4, r7
	add r2, r4, r6
	bl getRotDataByIdx_
	cmp r0, #0
	beq _020C4E44
	ldr sb, [r5, #0x14]
	ldr r6, [r5, #4]
	ldr r4, [r5, #0x10]
	ldr r2, [r5, #8]
	mul r1, r6, sb
	mul r0, r2, r4
	sub r0, r1, r0
	ldr r7, [r5, #0xc]
	ldr r8, [r5]
	mov r3, r0, asr #0xc
	mul r1, r2, r7
	mul r0, r8, sb
	sub r2, r1, r0
	mul r1, r8, r4
	mul r0, r6, r7
	sub r0, r1, r0
	str r3, [r5, #0x18]
	mov r1, r2, asr #0xc
	str r1, [r5, #0x1c]
	mov r0, r0, asr #0xc
	add sp, sp, #0x48
	str r0, [r5, #0x20]
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_020C4E44:
	add r0, r5, #0x18
	mov r1, r0
	bl VEC_Normalize
	add sp, sp, #0x48
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.align 2, 0
_020C4E58: .word 0x1FFF0000
	arm_func_end getRotData_

	arm_func_start getRotDataEx_
getRotDataEx_: ; 0x020C4E5C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x48
	mov sb, r3
	ldrh r3, [sb, #4]
	ldr r4, [r2, #4]
	mov sl, r0
	sub r0, r3, #1
	cmp r0, r1, asr #12
	add r8, sb, r4
	mov r0, r1, asr #0xc
	ldr r4, [sb, #0xc]
	ldr fp, [sb, #0x10]
	ldr r3, [r2]
	bne _020C4F50
	tst r3, #0xc0000000
	beq _020C4EB0
	tst r3, #0x40000000
	andne r2, r0, #1
	addne r0, r2, r0, lsr #1
	andeq r2, r0, #3
	addeq r0, r2, r0, lsr #2
_020C4EB0:
	ldr r2, [sb, #8]
	tst r2, #2
	movne r5, #0
	bne _020C4FC4
	mov r0, r0, lsl #1
	ldrh r3, [r8, r0]
	mov r0, sl
	add r1, sb, r4
	add r2, sb, fp
	bl getRotDataByIdx_
	cmp r0, #0
	beq _020C4F3C
	ldr r8, [sl, #0x14]
	ldr r5, [sl, #4]
	ldr r4, [sl, #0x10]
	ldr r2, [sl, #8]
	mul r1, r5, r8
	mul r0, r2, r4
	sub r0, r1, r0
	ldr r6, [sl, #0xc]
	ldr r7, [sl]
	mov r3, r0, asr #0xc
	mul r1, r2, r6
	mul r0, r7, r8
	sub r2, r1, r0
	mul r1, r7, r4
	mul r0, r5, r6
	sub r0, r1, r0
	str r3, [sl, #0x18]
	mov r1, r2, asr #0xc
	str r1, [sl, #0x1c]
	mov r0, r0, asr #0xc
	add sp, sp, #0x48
	str r0, [sl, #0x20]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C4F3C:
	add r0, sl, #0x18
	mov r1, r0
	bl VEC_Normalize
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C4F50:
	tst r3, #0xc0000000
	beq _020C4FC0
	ldr r2, _020C5194 ; =0x1FFF0000
	tst r3, #0x40000000
	and r3, r3, r2
	mov r3, r3, lsr #0x10
	beq _020C4F90
	cmp r0, r3
	movhs r0, r3, lsr #1
	addhs r5, r0, #1
	bhs _020C4FC4
	mov r0, r0, lsr #1
	add r5, r0, #1
	and r6, r1, r2, lsr #16
	mov r7, #2
	b _020C4FD0
_020C4F90:
	cmp r0, r3
	blo _020C4FA8
	and r2, r0, #3
	add r0, r2, r0, lsr #2
	add r5, r0, #1
	b _020C4FC4
_020C4FA8:
	ldr r2, _020C5198 ; =0x00003FFF
	mov r0, r0, lsr #2
	add r5, r0, #1
	and r6, r1, r2
	mov r7, #4
	b _020C4FD0
_020C4FC0:
	add r5, r0, #1
_020C4FC4:
	ldr r2, _020C519C ; =0x00000FFF
	mov r7, #1
	and r6, r1, r2
_020C4FD0:
	mov r0, r0, lsl #1
	ldrh r3, [r8, r0]
	add r0, sp, #0x24
	add r1, sb, r4
	add r2, sb, fp
	bl getRotDataByIdx_
	mov r1, r5, lsl #1
	ldrh r3, [r8, r1]
	orr r5, r0, #0
	add r0, sp, #0
	add r1, sb, r4
	add r2, sb, fp
	bl getRotDataByIdx_
	ldr r3, [sp, #0x24]
	ldr r1, [sp]
	mul r2, r3, r7
	sub r1, r1, r3
	mul r1, r6, r1
	add r1, r2, r1, asr #12
	str r1, [sl]
	ldr r3, [sp, #0x28]
	ldr r1, [sp, #4]
	mul r2, r3, r7
	sub r1, r1, r3
	mul r1, r6, r1
	add r1, r2, r1, asr #12
	str r1, [sl, #4]
	ldr r3, [sp, #0x2c]
	ldr r1, [sp, #8]
	mul r2, r3, r7
	sub r1, r1, r3
	mul r1, r6, r1
	add r1, r2, r1, asr #12
	str r1, [sl, #8]
	ldr r3, [sp, #0x30]
	ldr r1, [sp, #0xc]
	mul r2, r3, r7
	sub r1, r1, r3
	mul r1, r6, r1
	add r1, r2, r1, asr #12
	str r1, [sl, #0xc]
	ldr r2, [sp, #0x34]
	ldr r1, [sp, #0x10]
	orr r5, r5, r0
	sub r0, r1, r2
	mul r1, r2, r7
	mul r0, r6, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x10]
	ldr r2, [sp, #0x38]
	ldr r0, [sp, #0x14]
	mul r1, r2, r7
	sub r0, r0, r2
	mul r0, r6, r0
	add r2, r1, r0, asr #12
	mov r0, sl
	mov r1, sl
	str r2, [sl, #0x14]
	bl VEC_Normalize
	add r0, sl, #0xc
	mov r1, r0
	bl VEC_Normalize
	cmp r5, #0
	bne _020C5138
	ldr r2, [sp, #0x3c]
	ldr r0, [sp, #0x18]
	mul r1, r2, r7
	sub r0, r0, r2
	mul r0, r6, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x18]
	ldr r2, [sp, #0x40]
	ldr r0, [sp, #0x1c]
	mul r1, r2, r7
	sub r0, r0, r2
	mul r0, r6, r0
	add r0, r1, r0, asr #12
	str r0, [sl, #0x1c]
	ldr r2, [sp, #0x44]
	ldr r0, [sp, #0x20]
	mul r1, r2, r7
	sub r0, r0, r2
	mul r0, r6, r0
	add r2, r1, r0, asr #12
	add r0, sl, #0x18
	mov r1, r0
	str r2, [sl, #0x20]
	bl VEC_Normalize
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_020C5138:
	ldr r8, [sl, #0x14]
	ldr r5, [sl, #4]
	ldr r7, [sl]
	ldr r4, [sl, #0x10]
	ldr r2, [sl, #8]
	ldr r6, [sl, #0xc]
	mul r1, r5, r8
	mul r0, r2, r4
	sub r0, r1, r0
	mov r3, r0, asr #0xc
	mul r1, r2, r6
	mul r0, r7, r8
	sub r2, r1, r0
	mul r1, r7, r4
	mul r0, r5, r6
	sub r0, r1, r0
	str r3, [sl, #0x18]
	mov r1, r2, asr #0xc
	str r1, [sl, #0x1c]
	mov r0, r0, asr #0xc
	str r0, [sl, #0x20]
	add sp, sp, #0x48
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C5194: .word 0x1FFF0000
_020C5198: .word 0x00003FFF
_020C519C: .word 0x00000FFF
	arm_func_end getRotDataEx_

	arm_func_start getRotDataByIdx_
getRotDataByIdx_: ; 0x020C51A0
	stmdb sp!, {r3, r4, r5, lr}
	tst r3, #0x8000
	beq _020C5258
	mov r4, #0
	str r4, [r0, #0x20]
	str r4, [r0, #0x1c]
	str r4, [r0, #0x18]
	str r4, [r0, #0x14]
	str r4, [r0, #0x10]
	ldr r2, _020C52F8 ; =0x00007FFF
	str r4, [r0, #0xc]
	str r4, [r0, #8]
	and r2, r3, r2
	add r3, r2, r2, lsl #1
	str r4, [r0, #4]
	add r2, r1, r3, lsl #1
	str r4, [r0]
	mov r3, r3, lsl #1
	ldrsh r1, [r1, r3]
	ldrsh ip, [r2, #2]
	ldrsh r3, [r2, #4]
	tst r1, #0x10
	subne r5, r4, #0x1000
	ldr r4, _020C52FC ; =_021094B8
	and r1, r1, #0xf
	ldr lr, _020C5300 ; =_021094B8 + 1
	moveq r5, #0x1000
	ldrb r4, [r4, r1, lsl #2]
	str r5, [r0, r1, lsl #2]
	ldrb lr, [lr, r1, lsl #2]
	str ip, [r0, r4, lsl #2]
	str r3, [r0, lr, lsl #2]
	ldrsh lr, [r2]
	tst lr, #0x20
	ldr lr, _020C5304 ; =_021094B8 + 2
	rsbne r3, r3, #0
	ldrb lr, [lr, r1, lsl #2]
	str r3, [r0, lr, lsl #2]
	ldrsh r2, [r2]
	tst r2, #0x40
	ldr r2, _020C5308 ; =_021094B8 + 3
	rsbne ip, ip, #0
	ldrb r1, [r2, r1, lsl #2]
	str ip, [r0, r1, lsl #2]
	mov r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_020C5258:
	ldr r1, _020C52F8 ; =0x00007FFF
	and r1, r3, r1
	add r3, r1, r1, lsl #2
	add r1, r2, r3, lsl #1
	ldrsh lr, [r1, #8]
	mov ip, r3, lsl #1
	mov r3, lr, asr #3
	str r3, [r0, #0x10]
	ldrsh ip, [r2, ip]
	and r2, lr, #7
	mov r2, r2, lsl #0x10
	mov r3, ip, asr #3
	str r3, [r0]
	ldrsh r3, [r1, #2]
	and ip, ip, #7
	orr lr, ip, r2, asr #13
	mov r2, r3, asr #3
	str r2, [r0, #4]
	ldrsh ip, [r1, #4]
	mov r2, lr, lsl #0x10
	and lr, r3, #7
	mov r3, ip, asr #3
	str r3, [r0, #8]
	ldrsh r3, [r1, #6]
	orr r1, lr, r2, asr #13
	mov r1, r1, lsl #0x10
	and r2, ip, #7
	orr r1, r2, r1, asr #13
	mov r1, r1, lsl #0x10
	and r2, r3, #7
	orr r1, r2, r1, asr #13
	mov r1, r1, lsl #0x10
	mov r1, r1, asr #0x10
	mov r2, r3, asr #3
	mov r1, r1, lsl #0x13
	str r2, [r0, #0xc]
	mov r1, r1, asr #0x13
	str r1, [r0, #0x14]
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C52F8: .word 0x00007FFF
_020C52FC: .word _021094B8
_020C5300: .word _021094B8 + 1
_020C5304: .word _021094B8 + 2
_020C5308: .word _021094B8 + 3
	arm_func_end getRotDataByIdx_

	arm_func_start GetMatColAnmValue_
GetMatColAnmValue_: ; 0x020C530C
	tst r1, #0x20000000
	movne r0, r1, lsl #0x10
	movne r0, r0, lsr #0x10
	bxne lr
	mov r3, r1, lsl #0x10
	tst r1, #0xc0000000
	add r0, r0, r3, lsr #16
	moveq r1, r2, lsl #1
	ldreqh r0, [r0, r1]
	bxeq lr
	ldr r3, _020C546C ; =0x1FFF0000
	tst r1, #0x40000000
	and r1, r1, r3
	mov r3, r1, lsr #0x10
	beq _020C537C
	tst r2, #1
	beq _020C5370
	cmp r2, r3
	bls _020C5368
	bic r1, r3, #1
	add r0, r0, r1
	ldrh r0, [r0, #2]
	bx lr
_020C5368:
	mov r1, r2, lsr #1
	b _020C5424
_020C5370:
	bic r1, r2, #1
	ldrh r0, [r0, r1]
	bx lr
_020C537C:
	ands r1, r2, #3
	beq _020C5414
	cmp r2, r3
	bls _020C53A0
	mov r2, r3, lsr #2
	mov r1, r1, lsl #1
	add r0, r0, r2, lsl #1
	ldrh r0, [r1, r0]
	bx lr
_020C53A0:
	tst r2, #1
	beq _020C540C
	tst r2, #2
	movne r3, r2, lsr #2
	addne r1, r3, #1
	moveq r1, r2, lsr #2
	mov r2, r1, lsl #1
	addeq r3, r1, #1
	mov r1, r3, lsl #1
	ldrh r2, [r0, r2]
	ldrh ip, [r0, r1]
	ldr r0, _020C5470 ; =0x00007C1F
	and r1, r2, #0x3e0
	and r3, r2, r0
	add r2, r1, r1, lsl #1
	and r1, ip, #0x3e0
	add r1, r2, r1
	add r3, r3, r3, lsl #1
	and r2, ip, r0
	add r2, r3, r2
	mov r1, r1, lsr #2
	and r2, r0, r2, lsr #2
	and r0, r1, #0x3e0
	orr r0, r2, r0
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bx lr
_020C540C:
	mov r1, r2, lsr #2
	b _020C5424
_020C5414:
	mov r1, r2, lsr #2
	mov r1, r1, lsl #1
	ldrh r0, [r0, r1]
	bx lr
_020C5424:
	mov r2, r1, lsl #1
	add r1, r0, r1, lsl #1
	ldrh r3, [r0, r2]
	ldrh ip, [r1, #2]
	ldr r0, _020C5470 ; =0x00007C1F
	and r2, r3, #0x3e0
	and r1, ip, #0x3e0
	add r1, r2, r1
	mov r1, r1, lsr #1
	and r3, r3, r0
	and r2, ip, r0
	add r2, r3, r2
	and r2, r0, r2, lsr #1
	and r0, r1, #0x3e0
	orr r0, r2, r0
	mov r0, r0, lsl #0x10
	mov r0, r0, lsr #0x10
	bx lr
	.align 2, 0
_020C546C: .word 0x1FFF0000
_020C5470: .word 0x00007C1F
	arm_func_end GetMatColAnmValue_

	arm_func_start GetMatColAnmuAlphaValue_
GetMatColAnmuAlphaValue_: ; 0x020C5474
	tst r1, #0x20000000
	movne r0, r1, lsl #0x10
	movne r0, r0, lsr #0x10
	bxne lr
	mov r3, r1, lsl #0x10
	add r3, r0, r3, lsr #16
	tst r1, #0xc0000000
	ldreqb r0, [r3, r2]
	bxeq lr
	ldr r0, _020C555C ; =0x1FFF0000
	tst r1, #0x40000000
	and r0, r1, r0
	mov r0, r0, lsr #0x10
	beq _020C54E8
	tst r2, #1
	beq _020C54E0
	cmp r2, r0
	addhi r0, r3, r0, lsr #1
	ldrhib r0, [r0, #1]
	bxhi lr
	add r0, r3, r2, lsr #1
	ldrb r1, [r3, r2, lsr #1]
	ldrb r0, [r0, #1]
	add r0, r1, r0
	mov r0, r0, lsl #0xf
	mov r0, r0, lsr #0x10
	bx lr
_020C54E0:
	ldrb r0, [r3, r2, lsr #1]
	bx lr
_020C54E8:
	ands r1, r2, #3
	beq _020C5554
	cmp r2, r0
	addhi r0, r3, r0, lsr #2
	ldrhib r0, [r1, r0]
	bxhi lr
	tst r2, #1
	beq _020C5538
	tst r2, #2
	movne r2, r2, lsr #2
	addne r0, r2, #1
	moveq r0, r2, lsr #2
	ldrb r1, [r3, r0]
	addeq r2, r0, #1
	ldrb r0, [r3, r2]
	add r1, r1, r1, lsl #1
	add r0, r1, r0
	mov r0, r0, lsl #0xe
	mov r0, r0, lsr #0x10
	bx lr
_020C5538:
	add r0, r3, r2, lsr #2
	ldrb r1, [r3, r2, lsr #2]
	ldrb r0, [r0, #1]
	add r0, r1, r0
	mov r0, r0, lsl #0xf
	mov r0, r0, lsr #0x10
	bx lr
_020C5554:
	ldrb r0, [r3, r2, lsr #2]
	bx lr
	.align 2, 0
_020C555C: .word 0x1FFF0000
	arm_func_end GetMatColAnmuAlphaValue_

	arm_func_start NNSi_G3dAnmObjInitNsBma
NNSi_G3dAnmObjInitNsBma: ; 0x020C5560
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	cmp r2, #0
	ldrne r0, [r2, #8]
	mov sb, r1
	cmpne r0, #0
	addne r7, r2, r0
	ldr r0, _020C5630 ; =_02110A14
	add r1, sl, #0x1a
	ldr r3, [r0]
	mov r0, #0
	str r3, [sl, #0xc]
	ldrb r2, [r2, #0x18]
	moveq r7, #0
	strb r2, [sl, #0x19]
	mov r2, r2, lsl #1
	bl MIi_CpuClear16
	ldrb r0, [sb, #9]
	mov r6, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r8, r6
	add r5, sb, #8
	mov fp, r6
	mvn r4, #0
_020C55C4:
	cmp r5, #0
	beq _020C55F0
	ldrb r0, [sb, #9]
	cmp r6, r0
	bhs _020C55F0
	ldrh r0, [sb, #0xe]
	add r1, r5, r0
	ldrh r0, [r1, #2]
	add r0, r1, r0
	add r1, r0, r8
	b _020C55F4
_020C55F0:
	mov r1, fp
_020C55F4:
	cmp r7, #0
	moveq r0, r4
	beq _020C5608
	add r0, r7, #4
	bl NNS_G3dGetResDictIdxByName
_020C5608:
	cmp r0, #0
	orrge r1, r6, #0x100
	addge r0, sl, r0, lsl #1
	strgeh r1, [r0, #0x1a]
	ldrb r0, [sb, #9]
	add r6, r6, #1
	add r8, r8, #0x10
	cmp r6, r0
	blo _020C55C4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C5630: .word _02110A14
	arm_func_end NNSi_G3dAnmObjInitNsBma

	arm_func_start NNSi_G3dAnmCalcNsBma
NNSi_G3dAnmCalcNsBma: ; 0x020C5634
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r4, [r1, #8]
	ldr r1, [r1]
	mov r7, r0
	mov r5, r1, asr #0xc
	adds r3, r4, #8
	beq _020C567C
	ldrb r1, [r4, #9]
	mov r0, r2, lsl #0x10
	mov r2, r0, lsr #0x10
	cmp r1, r0, lsr #16
	bls _020C567C
	ldrh r1, [r4, #0xe]
	ldrh r0, [r3, r1]
	add r1, r3, r1
	add r1, r1, #4
	mla r6, r0, r2, r1
	b _020C5680
_020C567C:
	mov r6, #0
_020C5680:
	ldr r1, [r6]
	mov r0, r4
	mov r2, r5
	bl GetMatColAnmValue_
	mov r8, r0
	ldr r1, [r6, #4]
	mov r0, r4
	mov r2, r5
	bl GetMatColAnmValue_
	ldr r1, [r7, #4]
	orr r0, r8, r0, lsl #16
	tst r1, #0x8000
	movne r1, #1
	moveq r1, #0
	orr r0, r0, r1, lsl #15
	str r0, [r7, #4]
	ldr r1, [r6, #0xc]
	mov r0, r4
	mov r2, r5
	bl GetMatColAnmValue_
	mov r8, r0
	ldr r1, [r6, #8]
	mov r0, r4
	mov r2, r5
	bl GetMatColAnmValue_
	ldr r1, [r7, #8]
	orr r0, r0, r8, lsl #16
	tst r1, #0x8000
	movne r1, #1
	moveq r1, #0
	orr r0, r0, r1, lsl #15
	str r0, [r7, #8]
	ldr r1, [r6, #0x10]
	mov r0, r4
	mov r2, r5
	bl GetMatColAnmuAlphaValue_
	ldr r1, [r7, #0xc]
	bic r1, r1, #0x1f0000
	orr r0, r1, r0, lsl #16
	str r0, [r7, #0xc]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	arm_func_end NNSi_G3dAnmCalcNsBma

	arm_func_start GetTexSRTAnmVectorVal_
GetTexSRTAnmVectorVal_: ; 0x020C5724
	tst r1, #0x20000000
	movne r0, r2
	bxne lr
	tst r1, #0xc0000000
	add r0, r0, r2
	beq _020C57E0
	mov r2, r1, lsl #0x10
	tst r1, #0x40000000
	mov ip, r2, lsr #0x10
	beq _020C5774
	tst r3, #1
	beq _020C576C
	cmp r3, ip
	movhi r2, ip, lsr #1
	addhi r3, r2, #1
	bhi _020C57E0
	mov r3, r3, lsr #1
	b _020C57F4
_020C576C:
	mov r3, r3, lsr #1
	b _020C57E0
_020C5774:
	ands r2, r3, #3
	beq _020C57DC
	cmp r3, ip
	addhi r3, r2, ip, lsr #2
	bhi _020C57E0
	tst r3, #1
	beq _020C57D4
	tst r3, #2
	movne r3, r3, lsr #2
	addne r2, r3, #1
	moveq r2, r3, lsr #2
	addeq r3, r2, #1
	tst r1, #0x10000000
	ldreq r2, [r0, r2, lsl #2]
	ldreq r1, [r0, r3, lsl #2]
	beq _020C57C4
	mov r2, r2, lsl #1
	mov r1, r3, lsl #1
	ldrsh r2, [r0, r2]
	ldrsh r1, [r0, r1]
_020C57C4:
	add r0, r2, r2, lsl #1
	add r0, r0, r1
	mov r0, r0, asr #2
	bx lr
_020C57D4:
	mov r3, r3, lsr #2
	b _020C57F4
_020C57DC:
	mov r3, r3, lsr #2
_020C57E0:
	tst r1, #0x10000000
	movne r1, r3, lsl #1
	ldrnesh r0, [r0, r1]
	ldreq r0, [r0, r3, lsl #2]
	bx lr
_020C57F4:
	tst r1, #0x10000000
	addeq r1, r0, r3, lsl #2
	ldreq r0, [r0, r3, lsl #2]
	ldreq r1, [r1, #4]
	beq _020C5818
	add r1, r0, r3, lsl #1
	mov r2, r3, lsl #1
	ldrsh r0, [r0, r2]
	ldrsh r1, [r1, #2]
_020C5818:
	add r0, r0, r1
	mov r0, r0, asr #1
	bx lr
	arm_func_end GetTexSRTAnmVectorVal_

	arm_func_start GetTexSRTAnmSinCosVal_
GetTexSRTAnmSinCosVal_: ; 0x020C5824
	tst r1, #0x20000000
	movne r0, r2
	bxne lr
	tst r1, #0xc0000000
	add r0, r0, r2
	beq _020C58F4
	mov r2, r1, lsl #0x10
	tst r1, #0x40000000
	mov r2, r2, lsr #0x10
	beq _020C5874
	tst r3, #1
	beq _020C586C
	cmp r3, r2
	movhi r1, r2, lsr #1
	addhi r3, r1, #1
	bhi _020C58F4
	mov r1, r3, lsr #1
	b _020C58FC
_020C586C:
	mov r3, r3, lsr #1
	b _020C58F4
_020C5874:
	ands r1, r3, #3
	beq _020C58F0
	cmp r3, r2
	addhi r3, r1, r2, lsr #2
	bhi _020C58F4
	tst r3, #1
	beq _020C58E8
	tst r3, #2
	movne r3, r3, lsr #2
	addne r1, r3, #1
	moveq r1, r3, lsr #2
	add r2, r0, r1, lsl #2
	mov ip, r1, lsl #2
	addeq r3, r1, #1
	add r1, r0, r3, lsl #2
	ldrsh r2, [r2, #2]
	mov r3, r3, lsl #2
	ldrsh ip, [r0, ip]
	ldrsh r1, [r1, #2]
	ldrsh r3, [r0, r3]
	add r2, r2, r2, lsl #1
	add r0, r2, r1
	add ip, ip, ip, lsl #1
	mov r1, r0, asr #2
	add r2, ip, r3
	mov r0, r2, lsl #0xe
	mov r1, r1, lsl #0x10
	orr r0, r1, r0, lsr #16
	bx lr
_020C58E8:
	mov r1, r3, lsr #2
	b _020C58FC
_020C58F0:
	mov r3, r3, lsr #2
_020C58F4:
	ldr r0, [r0, r3, lsl #2]
	bx lr
_020C58FC:
	add r3, r0, r1, lsl #2
	mov ip, r1, lsl #2
	ldrsh r2, [r3, #2]
	ldrsh r1, [r3, #6]
	ldrsh ip, [r0, ip]
	ldrsh r3, [r3, #4]
	add r0, r2, r1
	mov r1, r0, asr #1
	add r0, ip, r3
	mov r0, r0, lsl #0xf
	mov r1, r1, lsl #0x10
	orr r0, r1, r0, lsr #16
	bx lr
	arm_func_end GetTexSRTAnmSinCosVal_

	arm_func_start GetTexSRTAnm_
GetTexSRTAnm_: ; 0x020C5930
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, lr}
	mov r7, r0
	mov r6, r2
	mov r5, r3
	adds r3, r7, #8
	beq _020C596C
	ldrb r0, [r7, #9]
	cmp r1, r0
	bhs _020C596C
	ldrh r2, [r7, #0xe]
	ldrh r0, [r3, r2]
	add r2, r3, r2
	add r2, r2, #4
	mla r4, r0, r1, r2
	b _020C5970
_020C596C:
	mov r4, #0
_020C5970:
	ldr r1, [r4, #0x18]
	ldr r2, [r4, #0x1c]
	mov r0, r7
	mov r3, r6
	ldr r8, [r5]
	bl GetTexSRTAnmVectorVal_
	mov sb, r0
	ldr r1, [r4, #0x20]
	ldr r2, [r4, #0x24]
	mov r0, r7
	mov r3, r6
	bl GetTexSRTAnmVectorVal_
	cmp sb, #0
	cmpeq r0, #0
	strne sb, [r5, #0x24]
	strne r0, [r5, #0x28]
	orreq r8, r8, #4
	ldr r1, [r4, #0x10]
	ldr r2, [r4, #0x14]
	bicne r8, r8, #4
	mov r0, r7
	mov r3, r6
	bl GetTexSRTAnmSinCosVal_
	cmp r0, #0x10000000
	orreq r8, r8, #2
	beq _020C59E8
	strh r0, [r5, #0x20]
	mov r0, r0, lsr #0x10
	strh r0, [r5, #0x22]
	bic r8, r8, #2
_020C59E8:
	mov r0, r7
	mov r3, r6
	ldmia r4, {r1, r2}
	bl GetTexSRTAnmVectorVal_
	mov sb, r0
	ldr r1, [r4, #8]
	ldr r2, [r4, #0xc]
	mov r0, r7
	mov r3, r6
	bl GetTexSRTAnmVectorVal_
	cmp sb, #0x1000
	cmpeq r0, #0x1000
	orreq r8, r8, #1
	strne sb, [r5, #0x18]
	strne r0, [r5, #0x1c]
	bicne r8, r8, #1
	str r8, [r5]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, pc}
	arm_func_end GetTexSRTAnm_

	arm_func_start NNSi_G3dAnmObjInitNsBta
NNSi_G3dAnmObjInitNsBta: ; 0x020C5A30
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	cmp r2, #0
	ldrne r0, [r2, #8]
	mov sb, r1
	cmpne r0, #0
	addne r7, r2, r0
	ldr r0, _020C5B00 ; =_02110A0C
	add r1, sl, #0x1a
	ldr r3, [r0]
	mov r0, #0
	str r3, [sl, #0xc]
	ldrb r2, [r2, #0x18]
	moveq r7, #0
	strb r2, [sl, #0x19]
	mov r2, r2, lsl #1
	bl MIi_CpuClear16
	ldrb r0, [sb, #9]
	mov r6, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r8, r6
	add r5, sb, #8
	mov fp, r6
	mvn r4, #0
_020C5A94:
	cmp r5, #0
	beq _020C5AC0
	ldrb r0, [sb, #9]
	cmp r6, r0
	bhs _020C5AC0
	ldrh r0, [sb, #0xe]
	add r1, r5, r0
	ldrh r0, [r1, #2]
	add r0, r1, r0
	add r1, r0, r8
	b _020C5AC4
_020C5AC0:
	mov r1, fp
_020C5AC4:
	cmp r7, #0
	moveq r0, r4
	beq _020C5AD8
	add r0, r7, #4
	bl NNS_G3dGetResDictIdxByName
_020C5AD8:
	cmp r0, #0
	orrge r1, r6, #0x100
	addge r0, sl, r0, lsl #1
	strgeh r1, [r0, #0x1a]
	ldrb r0, [sb, #9]
	add r6, r6, #1
	add r8, r8, #0x10
	cmp r6, r0
	blo _020C5A94
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C5B00: .word _02110A0C
	arm_func_end NNSi_G3dAnmObjInitNsBta

	arm_func_start NNSi_G3dAnmCalcNsBta
NNSi_G3dAnmCalcNsBta: ; 0x020C5B04
	stmdb sp!, {r4, lr}
	ldr ip, [r1]
	mov r4, r0
	mov r2, r2, lsl #0x10
	ldr r0, [r1, #8]
	mov r1, r2, lsr #0x10
	mov r3, r4
	mov r2, ip, asr #0xc
	bl GetTexSRTAnm_
	ldr r0, [r4, #0x10]
	bic r0, r0, #0xc0000000
	orr r0, r0, #0x40000000
	str r0, [r4, #0x10]
	ldr r0, [r4]
	orr r0, r0, #8
	str r0, [r4]
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_G3dAnmCalcNsBta

	arm_func_start NNSi_G3dAnmObjInitNsBtp
NNSi_G3dAnmObjInitNsBtp: ; 0x020C5B48
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sl, r0
	cmp r2, #0
	ldrne r0, [r2, #8]
	mov sb, r1
	cmpne r0, #0
	addne r7, r2, r0
	ldr r0, _020C5C20 ; =_02110A10
	add r1, sl, #0x1a
	ldr r3, [r0]
	mov r0, #0
	str r3, [sl, #0xc]
	ldrb r2, [r2, #0x18]
	moveq r7, #0
	strb r2, [sl, #0x19]
	str sb, [sl, #8]
	ldrb r2, [sl, #0x19]
	mov r2, r2, lsl #1
	bl MIi_CpuClear16
	ldrb r0, [sb, #0xd]
	mov r6, #0
	cmp r0, #0
	ldmlsia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	mov r8, r6
	add r5, sb, #0xc
	mov fp, r6
	mvn r4, #0
_020C5BB4:
	cmp r5, #0
	beq _020C5BE0
	ldrb r0, [sb, #0xd]
	cmp r6, r0
	bhs _020C5BE0
	ldrh r0, [sb, #0x12]
	add r1, r5, r0
	ldrh r0, [r1, #2]
	add r0, r1, r0
	add r1, r0, r8
	b _020C5BE4
_020C5BE0:
	mov r1, fp
_020C5BE4:
	cmp r7, #0
	moveq r0, r4
	beq _020C5BF8
	add r0, r7, #4
	bl NNS_G3dGetResDictIdxByName
_020C5BF8:
	cmp r0, #0
	orrge r1, r6, #0x100
	addge r0, sl, r0, lsl #1
	strgeh r1, [r0, #0x1a]
	ldrb r0, [sb, #0xd]
	add r6, r6, #1
	add r8, r8, #0x10
	cmp r6, r0
	blo _020C5BB4
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.align 2, 0
_020C5C20: .word _02110A10
	arm_func_end NNSi_G3dAnmObjInitNsBtp

	arm_func_start SetTexParamaters_
SetTexParamaters_: ; 0x020C5C24
	stmdb sp!, {r3, r4, r5, lr}
	movs r5, r0
	mov r4, r2
	moveq r0, #0
	beq _020C5C40
	add r0, r5, #0x3c
	bl NNS_G3dGetResDataByName
_020C5C40:
	ldr r1, [r0]
	ldr r3, [r4, #0x10]
	and r1, r1, #0x1c000000
	cmp r1, #0x14000000
	ldrne r1, [r5, #8]
	ldr r2, _020C5CF4 ; =0x000007FF
	ldreq r1, [r5, #0x18]
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	bic r5, r1, #0xe0000000
	ldr r1, _020C5CF8 ; =0xC00F0000
	and r3, r3, r1
	str r3, [r4, #0x10]
	ldr r1, [r0]
	add r1, r1, r5
	orr r1, r3, r1
	str r1, [r4, #0x10]
	ldr r1, [r0, #4]
	and r1, r1, r2
	strh r1, [r4, #0x2c]
	ldr r1, [r0, #4]
	and r1, r1, r2, lsl #11
	mov r1, r1, lsr #0xb
	strh r1, [r4, #0x2e]
	ldr r0, [r0, #4]
	ldrh r1, [r4, #0x2c]
	and r3, r0, r2
	and r0, r0, r2, lsl #11
	cmp r3, r1
	mov r5, r0, lsr #0xb
	moveq r0, #0x1000
	beq _020C5CCC
	mov r0, r3, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020C5CCC:
	str r0, [r4, #0x30]
	ldrh r1, [r4, #0x2e]
	cmp r5, r1
	moveq r0, #0x1000
	beq _020C5CEC
	mov r0, r5, lsl #0xc
	mov r1, r1, lsl #0xc
	bl FX_Div
_020C5CEC:
	str r0, [r4, #0x34]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020C5CF4: .word 0x000007FF
_020C5CF8: .word 0xC00F0000
	arm_func_end SetTexParamaters_

	arm_func_start SetPlttParamaters_
SetPlttParamaters_: ; 0x020C5CFC
	stmdb sp!, {r3, r4, r5, lr}
	movs r5, r0
	ldrneh r0, [r5, #0x34]
	mov r4, r2
	cmpne r0, #0
	moveq r0, #0
	beq _020C5D20
	add r0, r5, r0
	bl NNS_G3dGetResDataByName
_020C5D20:
	ldr r1, [r5, #0x2c]
	ldrh r2, [r0, #2]
	mov r1, r1, lsl #0x10
	mov r1, r1, lsr #0x10
	bic r1, r1, #0xe0000000
	mov r1, r1, lsl #0x10
	tst r2, #1
	mov r1, r1, lsr #0x10
	ldrh r0, [r0]
	bne _020C5D58
	mov r0, r0, lsl #0xf
	mov r1, r1, lsl #0xf
	mov r0, r0, lsr #0x10
	mov r1, r1, lsr #0x10
_020C5D58:
	add r0, r0, r1
	str r0, [r4, #0x14]
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end SetPlttParamaters_

	arm_func_start NNSi_G3dAnmCalcNsBtp
NNSi_G3dAnmCalcNsBtp: ; 0x020C5D64
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r6, r1
	ldr r3, [r6]
	ldr r5, [r6, #8]
	mov r1, r2, lsl #0x10
	mov r2, r3, lsl #4
	mov r7, r0
	mov r0, r5
	mov r1, r1, lsr #0x10
	mov r2, r2, lsr #0x10
	bl NNSi_G3dGetTexPatAnmFV
	mov r4, r0
	ldrb r1, [r4, #2]
	mov r0, r5
	bl NNSi_G3dGetTexPatAnmTexNameByIdx
	mov r1, r0
	ldr r0, [r6, #0x14]
	mov r2, r7
	bl SetTexParamaters_
	ldrb r1, [r4, #3]
	cmp r1, #0xff
	ldmeqia sp!, {r3, r4, r5, r6, r7, pc}
	mov r0, r5
	bl NNSi_G3dGetTexPatAnmPlttNameByIdx
	mov r1, r0
	ldr r0, [r6, #0x14]
	mov r2, r7
	bl SetPlttParamaters_
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end NNSi_G3dAnmCalcNsBtp

	arm_func_start NNSi_G3dAnmObjInitNsBva
NNSi_G3dAnmObjInitNsBva: ; 0x020C5DD8
	ldr r3, _020C5E20 ; =_02110A04
	mov ip, #0
	ldr r3, [r3]
	str r3, [r0, #0xc]
	ldrb r2, [r2, #0x17]
	strb r2, [r0, #0x19]
	str r1, [r0, #8]
	ldrb r1, [r0, #0x19]
	cmp r1, #0
	bxls lr
_020C5E00:
	orr r2, ip, #0x100
	add r1, r0, ip, lsl #1
	strh r2, [r1, #0x1a]
	ldrb r1, [r0, #0x19]
	add ip, ip, #1
	cmp ip, r1
	blo _020C5E00
	bx lr
	.align 2, 0
_020C5E20: .word _02110A04
	arm_func_end NNSi_G3dAnmObjInitNsBva

	arm_func_start NNSi_G3dAnmCalcNsBva
NNSi_G3dAnmCalcNsBva: ; 0x020C5E24
	stmdb sp!, {r3, lr}
	ldr lr, [r1, #8]
	ldr r3, [r1]
	ldrh r1, [lr, #6]
	mov ip, r3, asr #0xc
	mov r3, #1
	mla r2, ip, r1, r2
	mov r1, r2, lsr #5
	add r1, lr, r1, lsl #2
	ldr r1, [r1, #0xc]
	and r2, r2, #0x1f
	and r1, r1, r3, lsl r2
	str r1, [r0]
	ldmia sp!, {r3, pc}
	arm_func_end NNSi_G3dAnmCalcNsBva

	arm_func_start NNSi_G3dSendJointSRTBasic
NNSi_G3dSendJointSRTBasic: ; 0x020C5E5C
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4]
	tst r0, #4
	bne _020C5EA0
	tst r0, #2
	bne _020C5E8C
	add r1, r4, #0x28
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	b _020C5EB8
_020C5E8C:
	add r1, r4, #0x4c
	mov r0, #0x1c
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	b _020C5EB8
_020C5EA0:
	tst r0, #2
	bne _020C5EB8
	add r1, r4, #0x28
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
_020C5EB8:
	ldr r0, [r4]
	tst r0, #1
	ldmneia sp!, {r4, pc}
	add r1, r4, #4
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	ldmia sp!, {r4, pc}
	arm_func_end NNSi_G3dSendJointSRTBasic

	arm_func_start NNSi_G3dGetJointScaleBasic
NNSi_G3dGetJointScaleBasic: ; 0x020C5ED8
	tst r3, #4
	beq _020C5EF0
	ldr r1, [r0]
	orr r1, r1, #1
	str r1, [r0]
	b _020C5F08
_020C5EF0:
	ldr r2, [r1]
	str r2, [r0, #4]
	ldr r2, [r1, #4]
	str r2, [r0, #8]
	ldr r1, [r1, #8]
	str r1, [r0, #0xc]
_020C5F08:
	ldr r1, [r0]
	orr r1, r1, #0x18
	str r1, [r0]
	bx lr
	arm_func_end NNSi_G3dGetJointScaleBasic

	arm_func_start NNSi_G3dSendJointSRTMaya
NNSi_G3dSendJointSRTMaya: ; 0x020C5F18
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r0, [r5]
	mov r4, #0
	tst r0, #4
	moveq r4, #1
	tst r0, #0x20
	beq _020C5F6C
	tst r0, #8
	bne _020C5F6C
	cmp r4, #0
	beq _020C5F5C
	add r1, r5, #0x4c
	mov r0, #0x1c
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	mov r4, #0
_020C5F5C:
	add r1, r5, #0x10
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C5F6C:
	ldr r0, [r5]
	tst r0, #2
	bne _020C5FA4
	cmp r4, #0
	add r1, r5, #0x28
	beq _020C5F94
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	b _020C5FBC
_020C5F94:
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	b _020C5FBC
_020C5FA4:
	cmp r4, #0
	beq _020C5FBC
	add r1, r5, #0x4c
	mov r0, #0x1c
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C5FBC:
	ldr r0, [r5]
	tst r0, #1
	ldmneia sp!, {r3, r4, r5, pc}
	add r1, r5, #4
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end NNSi_G3dSendJointSRTMaya

	arm_func_start NNSi_G3dGetJointScaleMaya
NNSi_G3dGetJointScaleMaya: ; 0x020C5FDC
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov ip, r0
	tst r3, #4
	ldrb r0, [r2, #3]
	beq _020C6030
	ldr r1, [ip]
	tst r0, #2
	orr r1, r1, #1
	str r1, [ip]
	beq _020C60A8
	ldr r1, _020C611C ; =_021DA700
	ldrb r7, [r2, #1]
	ldr r1, [r1]
	mov r3, #1
	add r6, r1, #0xc4
	mov r5, r7, lsr #5
	ldr r4, [r6, r5, lsl #2]
	and r1, r7, #0x1f
	orr r1, r4, r3, lsl r1
	str r1, [r6, r5, lsl #2]
	b _020C60A8
_020C6030:
	ldr r3, [r1]
	tst r0, #2
	str r3, [ip, #4]
	ldr r3, [r1, #4]
	str r3, [ip, #8]
	ldr r3, [r1, #8]
	str r3, [ip, #0xc]
	beq _020C60A8
	ldr r3, _020C611C ; =_021DA700
	ldrb r8, [r2, #1]
	ldr r4, [r3]
	mov r3, #0x18
	add r7, r4, #0xc4
	mov r6, r8, lsr #5
	and r4, r8, #0x1f
	mov r5, #1
	mvn r4, r5, lsl r4
	ldr lr, [r7, r6, lsl #2]
	smulbb r5, r8, r3
	and r3, lr, r4
	str r3, [r7, r6, lsl #2]
	ldr r4, [r1, #0xc]
	ldr r3, _020C6120 ; =_021DB510
	ldr lr, _020C6124 ; =_021DB514
	str r4, [r3, r5]
	ldr r4, [r1, #0x10]
	ldr r3, _020C6128 ; =_021DB518
	str r4, [lr, r5]
	ldr r1, [r1, #0x14]
	str r1, [r3, r5]
_020C60A8:
	tst r0, #1
	beq _020C610C
	ldrb r3, [r2, #2]
	ldr r1, [ip]
	ldr r0, _020C611C ; =_021DA700
	orr r1, r1, #0x20
	str r1, [ip]
	ldr r1, [r0]
	mov r0, r3, lsr #5
	add r0, r1, r0, lsl #2
	ldr r0, [r0, #0xc4]
	and r1, r3, #0x1f
	mov r2, #1
	tst r0, r2, lsl r1
	beq _020C60F4
	ldr r0, [ip]
	orr r0, r0, #8
	str r0, [ip]
	b _020C610C
_020C60F4:
	ldr r1, _020C6120 ; =_021DB510
	mov r0, #0x18
	mla r0, r3, r0, r1
	add r3, ip, #0x10
	ldmia r0, {r0, r1, r2}
	stmia r3, {r0, r1, r2}
_020C610C:
	ldr r0, [ip]
	orr r0, r0, #0x10
	str r0, [ip]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C611C: .word _021DA700
_020C6120: .word _021DB510
_020C6124: .word _021DB514
_020C6128: .word _021DB518
	arm_func_end NNSi_G3dGetJointScaleMaya

	arm_func_start texmtxCalc_flag_
texmtxCalc_flag_: ; 0x020C612C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	ldrh r2, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	mov sl, r0
	mov r8, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, r8
	bl FX_DivAsync
	ldrsh r5, [sb, #0x22]
	ldr r3, [sb, #0x18]
	ldrsh r0, [sb, #0x20]
	ldr r4, [sb, #0x1c]
	smull r2, r1, r3, r5
	mov r6, r2, lsr #0xc
	orr r6, r6, r1, lsl #20
	smull r2, r1, r3, r0
	mov r7, r2, lsr #0xc
	orr r7, r7, r1, lsl #20
	smull r3, r2, r4, r0
	smull r1, r0, r4, r5
	mov r5, r3, lsr #0xc
	orr r5, r5, r2, lsl #20
	mov r4, r1, lsr #0xc
	orr r4, r4, r0, lsl #20
	str r6, [sl]
	str r4, [sl, #0x14]
	bl FX_GetDivResult
	mov r1, fp
	rsb r2, r5, #0
	mul r0, r2, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #4]
	mov r0, r8
	bl FX_DivAsync
	sub r0, r5, r4
	add r1, r7, r6
	ldr r5, [sb, #0x18]
	ldrh r4, [sb, #0x2c]
	sub r2, r5, r1
	ldr r1, [sb, #0x24]
	mul r3, r4, r2
	smull r2, r1, r5, r1
	mov r2, r2, lsr #8
	orr r2, r2, r1, lsl #24
	mul r1, r4, r2
	rsb r1, r1, r3, lsl #3
	str r1, [sl, #0x30]
	ldr r4, [sb, #0x1c]
	ldrh r3, [sb, #0x2e]
	sub r0, r0, r4
	add r0, r0, #0x2000
	mul r2, r3, r0
	ldr r0, [sb, #0x28]
	smull r1, r0, r4, r0
	mov r1, r1, lsr #8
	orr r1, r1, r0, lsl #24
	mul r0, r3, r1
	add r0, r0, r2, lsl #3
	str r0, [sl, #0x34]
	bl FX_GetDivResult
	mul r0, r7, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flag_

	arm_func_start texmtxCalc_flagS_
texmtxCalc_flagS_: ; 0x020C6234
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	rsb r2, r2, #0
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrsh r2, [r5, #0x20]
	ldrsh r1, [r5, #0x22]
	ldrh r3, [r5, #0x2c]
	ldr r0, [r5, #0x24]
	add r1, r2, r1
	rsb r1, r1, #0
	add r2, r1, #0x1000
	mul r1, r0, r3
	mul r2, r3, r2
	mov r0, r1, lsl #4
	rsb r0, r0, r2, lsl #3
	str r0, [r6, #0x30]
	ldrsh r2, [r5, #0x20]
	ldrsh r1, [r5, #0x22]
	ldrh r3, [r5, #0x2e]
	ldr r0, [r5, #0x28]
	sub r1, r2, r1
	add r2, r1, #0x1000
	mul r1, r0, r3
	mul r2, r3, r2
	mov r0, r1, lsl #4
	add r0, r0, r2, lsl #3
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagS_

	arm_func_start texmtxCalc_flagR_
texmtxCalc_flagR_: ; 0x020C6304
	stmdb sp!, {r4, lr}
	ldr r3, [r1, #0x18]
	mov r2, #0
	str r3, [r0]
	ldr r3, [r1, #0x1c]
	str r3, [r0, #0x14]
	str r2, [r0, #4]
	ldr r4, [r1, #0x18]
	ldr r3, [r1, #0x24]
	ldrh lr, [r1, #0x2c]
	smull ip, r3, r4, r3
	mov r4, ip, lsr #8
	orr r4, r4, r3, lsl #24
	rsb r3, r4, #0
	mul r3, lr, r3
	str r3, [r0, #0x30]
	ldr r4, [r1, #0x1c]
	ldr ip, [r1, #0x28]
	mov r3, r4, lsl #1
	smull lr, ip, r4, ip
	rsb r4, r3, #0
	mov r3, lr, lsr #8
	ldrh lr, [r1, #0x2e]
	add r1, r4, #0x2000
	orr r3, r3, ip, lsl #24
	mul r4, lr, r1
	mul r1, lr, r3
	add r1, r1, r4, lsl #3
	str r1, [r0, #0x34]
	str r2, [r0, #0x10]
	ldmia sp!, {r4, pc}
	arm_func_end texmtxCalc_flagR_

	arm_func_start texmtxCalc_flagRS_
texmtxCalc_flagRS_: ; 0x020C6380
	mov r2, #0x1000
	str r2, [r0]
	str r2, [r0, #0x14]
	mov ip, #0
	str ip, [r0, #4]
	ldrh r2, [r1, #0x2c]
	ldr r3, [r1, #0x24]
	mul r2, r3, r2
	rsb r2, r2, #0
	mov r2, r2, lsl #4
	str r2, [r0, #0x30]
	ldrh r2, [r1, #0x2e]
	ldr r1, [r1, #0x28]
	mul r2, r1, r2
	mov r1, r2, lsl #4
	str r1, [r0, #0x34]
	str ip, [r0, #0x10]
	bx lr
	arm_func_end texmtxCalc_flagRS_

	arm_func_start texmtxCalc_flagT_
texmtxCalc_flagT_: ; 0x020C63C8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	ldrh r2, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	mov sl, r0
	mov r8, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, r8
	bl FX_DivAsync
	ldrsh r5, [sb, #0x22]
	ldr r3, [sb, #0x18]
	ldrsh r0, [sb, #0x20]
	ldr r4, [sb, #0x1c]
	smull r2, r1, r3, r5
	mov r6, r2, lsr #0xc
	orr r6, r6, r1, lsl #20
	smull r2, r1, r3, r0
	mov r7, r2, lsr #0xc
	orr r7, r7, r1, lsl #20
	smull r3, r2, r4, r0
	smull r1, r0, r4, r5
	mov r5, r3, lsr #0xc
	orr r5, r5, r2, lsl #20
	mov r4, r1, lsr #0xc
	orr r4, r4, r0, lsl #20
	str r6, [sl]
	str r4, [sl, #0x14]
	bl FX_GetDivResult
	mov r1, fp
	rsb r2, r5, #0
	mul r0, r2, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #4]
	mov r0, r8
	bl FX_DivAsync
	sub r1, r5, r4
	add r0, r7, r6
	ldrh r3, [sb, #0x2c]
	ldr r2, [sb, #0x18]
	sub r0, r2, r0
	mul r0, r3, r0
	mov r0, r0, lsl #3
	str r0, [sl, #0x30]
	ldrh r2, [sb, #0x2e]
	ldr r0, [sb, #0x1c]
	sub r0, r1, r0
	add r0, r0, #0x2000
	mul r0, r2, r0
	mov r0, r0, lsl #3
	str r0, [sl, #0x34]
	bl FX_GetDivResult
	mul r0, r7, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flagT_

	arm_func_start texmtxCalc_flagTS_
texmtxCalc_flagTS_: ; 0x020C64A8
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	rsb r2, r2, #0
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrsh r1, [r5, #0x20]
	ldrsh r0, [r5, #0x22]
	ldrh r2, [r5, #0x2c]
	add r0, r1, r0
	rsb r0, r0, #0
	add r0, r0, #0x1000
	mul r0, r2, r0
	mov r0, r0, lsl #3
	str r0, [r6, #0x30]
	ldrsh r1, [r5, #0x20]
	ldrsh r0, [r5, #0x22]
	ldrh r2, [r5, #0x2e]
	sub r0, r1, r0
	add r0, r0, #0x1000
	mul r0, r2, r0
	mov r0, r0, lsl #3
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagTS_

	arm_func_start texmtxCalc_flagTR_
texmtxCalc_flagTR_: ; 0x020C6560
	ldr r2, [r1, #0x18]
	mov ip, #0
	str r2, [r0]
	ldr r2, [r1, #0x1c]
	str r2, [r0, #0x14]
	str ip, [r0, #4]
	str ip, [r0, #0x30]
	ldr r2, [r1, #0x1c]
	ldrh r3, [r1, #0x2e]
	mov r1, r2, lsl #1
	rsb r1, r1, #0
	add r1, r1, #0x2000
	mul r1, r3, r1
	mov r1, r1, lsl #3
	str r1, [r0, #0x34]
	str ip, [r0, #0x10]
	bx lr
	arm_func_end texmtxCalc_flagTR_

	arm_func_start texmtxCalc_flagTRS_
texmtxCalc_flagTRS_: ; 0x020C65A4
	mov r2, #0x1000
	str r2, [r0]
	mov r1, #0
	str r1, [r0, #4]
	str r1, [r0, #0x10]
	str r2, [r0, #0x14]
	str r1, [r0, #0x30]
	str r1, [r0, #0x34]
	bx lr
	arm_func_end texmtxCalc_flagTRS_

	arm_func_start NNSi_G3dSendTexSRTMaya
NNSi_G3dSendTexSRTMaya: ; 0x020C65C8
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x4c
	mov r4, r0
	ldr r0, [r4]
	mov r1, #0
	tst r0, #8
	ldrne r0, _020C6704 ; =0x00101610
	mov r3, #3
	strne r0, [sp]
	ldreq r0, _020C6708 ; =0x00101810
	mov r2, #2
	streq r0, [sp]
	mov r0, #0x1000
	str r0, [sp, #0x44]
	str r3, [sp, #4]
	str r2, [sp, #0x48]
	str r1, [sp, #0x40]
	str r1, [sp, #0x34]
	str r1, [sp, #0x30]
	str r1, [sp, #0x2c]
	str r1, [sp, #0x28]
	str r1, [sp, #0x24]
	str r1, [sp, #0x20]
	str r1, [sp, #0x14]
	str r1, [sp, #0x10]
	ldr r1, [r4]
	ldr r0, _020C670C ; =_02110C1C
	and r1, r1, #7
	ldr r2, [r0, r1, lsl #2]
	add r0, sp, #8
	mov r1, r4
	blx r2
	ldr r3, [r4, #0x30]
	cmp r3, #0x1000
	beq _020C6698
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #8]
	ldr r2, [r4, #0x30]
	ldr r0, [sp, #0x38]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0xc]
	ldr r1, [r4, #0x30]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x38]
_020C6698:
	ldr r3, [r4, #0x34]
	cmp r3, #0x1000
	beq _020C66E8
	ldr r0, [sp, #0x18]
	ldr r1, [sp, #0x1c]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #0x18]
	ldr r2, [r4, #0x34]
	ldr r0, [sp, #0x3c]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x1c]
	ldr r1, [r4, #0x34]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x3c]
_020C66E8:
	add r1, sp, #0
	ldr r0, [sp]
	add r1, r1, #4
	mov r2, #0x12
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0x4c
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020C6704: .word 0x00101610
_020C6708: .word 0x00101810
_020C670C: .word _02110C1C
	arm_func_end NNSi_G3dSendTexSRTMaya

	arm_func_start NNSi_G3dSendJointSRTSi3d
NNSi_G3dSendJointSRTSi3d: ; 0x020C6710
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	mov r6, r0
	ldr r0, [r6]
	mov r4, #0
	ands r5, r0, #0x18
	bne _020C673C
	add r1, r6, #0x1c
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C673C:
	ldr r0, [r6]
	tst r0, #4
	bne _020C67AC
	cmp r5, #0
	movne r4, #1
	bne _020C67AC
	ldr r2, [r6, #0x4c]
	ldr r0, [r6, #0x10]
	add r1, sp, #0
	smull r3, r0, r2, r0
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp]
	ldr r3, [r6, #0x50]
	ldr r2, [r6, #0x14]
	mov r0, #0x1c
	smull ip, r2, r3, r2
	mov r3, ip, lsr #0xc
	orr r3, r3, r2, lsl #20
	str r3, [sp, #4]
	ldr ip, [r6, #0x54]
	ldr r3, [r6, #0x18]
	mov r2, #3
	smull lr, r3, ip, r3
	mov ip, lr, lsr #0xc
	orr ip, ip, r3, lsl #20
	str ip, [sp, #8]
	bl NNS_G3dGeBufferOP_N
_020C67AC:
	ldr r0, [r6]
	tst r0, #2
	bne _020C67E4
	cmp r4, #0
	add r1, r6, #0x28
	beq _020C67D4
	mov r0, #0x19
	mov r2, #0xc
	bl NNS_G3dGeBufferOP_N
	b _020C67FC
_020C67D4:
	mov r0, #0x1a
	mov r2, #9
	bl NNS_G3dGeBufferOP_N
	b _020C67FC
_020C67E4:
	cmp r4, #0
	beq _020C67FC
	add r1, r6, #0x4c
	mov r0, #0x1c
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C67FC:
	cmp r5, #0
	bne _020C6814
	add r1, r6, #0x10
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
_020C6814:
	ldr r0, [r6]
	tst r0, #1
	addne sp, sp, #0xc
	ldmneia sp!, {r3, r4, r5, r6, pc}
	add r1, r6, #4
	mov r0, #0x1b
	mov r2, #3
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end NNSi_G3dSendJointSRTSi3d

	arm_func_start NNSi_G3dGetJointScaleSi3d
NNSi_G3dGetJointScaleSi3d: ; 0x020C683C
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	mov r5, r0
	tst r3, #4
	ldrb r4, [r2, #1]
	ldrb r0, [r2, #2]
	beq _020C68D8
	ldr r2, [r5]
	ldr r1, _020C6A3C ; =_021DA700
	orr r2, r2, #1
	str r2, [r5]
	ldr r3, [r1]
	mov r1, r0, lsr #5
	add r1, r3, r1, lsl #2
	ldr r1, [r1, #0xc4]
	and r2, r0, #0x1f
	mov r6, #1
	tst r1, r6, lsl r2
	beq _020C68AC
	add r3, r3, #0xc4
	mov r2, r4, lsr #5
	ldr r1, [r3, r2, lsl #2]
	and r0, r4, #0x1f
	orr r0, r1, r6, lsl r0
	str r0, [r3, r2, lsl #2]
	ldr r0, [r5]
	orr r0, r0, #0x18
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C68AC:
	mov r2, #0x18
	ldr r7, _020C6A40 ; =_021DB504
	mul r6, r0, r2
	mla r1, r4, r2, r7
	add r0, r7, r6
	bl MIi_CpuCopy32
	add r0, r7, r6
	add r1, r5, #0x10
	mov r2, #0x18
	bl MIi_CpuCopy32
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C68D8:
	ldr r3, [r1]
	ldr r2, _020C6A3C ; =_021DA700
	str r3, [r5, #4]
	ldr r3, [r1, #4]
	mov r7, r0, lsr #5
	str r3, [r5, #8]
	ldr r3, [r1, #8]
	and r8, r0, #0x1f
	str r3, [r5, #0xc]
	ldr r6, [r2]
	mov r3, #1
	add r2, r6, r7, lsl #2
	ldr r2, [r2, #0xc4]
	tst r2, r3, lsl r8
	mov r2, #0x18
	beq _020C6960
	ldr r3, _020C6A40 ; =_021DB504
	mov r0, r1
	mla r1, r4, r2, r3
	bl MIi_CpuCopy32
	ldr r0, _020C6A3C ; =_021DA700
	mov r2, r4, lsr #5
	ldr r1, [r0]
	and r0, r4, #0x1f
	add r3, r1, #0xc4
	mov r1, #1
	mvn r0, r1, lsl r0
	ldr r1, [r3, r2, lsl #2]
	and r0, r1, r0
	str r0, [r3, r2, lsl #2]
	ldr r0, [r5]
	orr r0, r0, #0x18
	str r0, [r5]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_020C6960:
	mul ip, r0, r2
	and r7, r4, #0x1f
	mvn r7, r3, lsl r7
	add r6, r6, #0xc4
	mov lr, r4, lsr #5
	ldr r8, [r6, lr, lsl #2]
	mul r3, r4, r2
	and r4, r8, r7
	str r4, [r6, lr, lsl #2]
	ldr r0, _020C6A40 ; =_021DB504
	ldr r7, [r1]
	ldr r4, [r0, ip]
	ldr r6, _020C6A44 ; =_021DB508
	smull r8, r4, r7, r4
	mov r7, r8, lsr #0xc
	orr r7, r7, r4, lsl #20
	str r7, [r0, r3]
	ldr r8, [r1, #4]
	ldr r4, [r6, ip]
	ldr r7, _020C6A48 ; =_021DB50C
	smull lr, r4, r8, r4
	mov r8, lr, lsr #0xc
	orr r8, r8, r4, lsl #20
	str r8, [r6, r3]
	ldr r8, [r1, #8]
	ldr r4, [r7, ip]
	ldr r6, _020C6A4C ; =_021DB510
	smull lr, r4, r8, r4
	mov r8, lr, lsr #0xc
	orr r8, r8, r4, lsl #20
	str r8, [r7, r3]
	ldr r7, [r1, #0xc]
	ldr r4, [r6, ip]
	ldr lr, _020C6A50 ; =_021DB514
	smull r8, r4, r7, r4
	mov r7, r8, lsr #0xc
	orr r7, r7, r4, lsl #20
	str r7, [r6, r3]
	ldr r7, [r1, #0x10]
	ldr r6, [lr, ip]
	ldr r4, _020C6A54 ; =_021DB518
	smull r8, r6, r7, r6
	mov r7, r8, lsr #0xc
	orr r7, r7, r6, lsl #20
	str r7, [lr, r3]
	ldr r6, [r1, #0x14]
	ldr r1, [r4, ip]
	add r0, r0, ip
	smull ip, r1, r6, r1
	mov r6, ip, lsr #0xc
	orr r6, r6, r1, lsl #20
	add r1, r5, #0x10
	str r6, [r4, r3]
	bl MIi_CpuCopy32
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020C6A3C: .word _021DA700
_020C6A40: .word _021DB504
_020C6A44: .word _021DB508
_020C6A48: .word _021DB50C
_020C6A4C: .word _021DB510
_020C6A50: .word _021DB514
_020C6A54: .word _021DB518
	arm_func_end NNSi_G3dGetJointScaleSi3d

	arm_func_start NNSi_G3dSendTexSRTSi3d
NNSi_G3dSendTexSRTSi3d: ; 0x020C6A58
	stmdb sp!, {lr}
	sub sp, sp, #0x3c
	ldr r1, [r0]
	mov r2, #0
	tst r1, #8
	ldrne r1, _020C6C1C ; =0x00101710
	mov r3, #3
	strne r1, [sp]
	ldreq r1, _020C6C20 ; =0x00101910
	str r3, [sp, #4]
	streq r1, [sp]
	mov r1, #2
	str r2, [sp, #0x34]
	str r2, [sp, #0x28]
	str r2, [sp, #0x24]
	str r2, [sp, #0x20]
	str r2, [sp, #0x1c]
	str r2, [sp, #0x14]
	str r2, [sp, #0x10]
	str r2, [sp, #0xc]
	str r1, [sp, #0x38]
	ldr r1, [r0]
	tst r1, #4
	beq _020C6AF0
	str r2, [sp, #0x2c]
	str r2, [sp, #0x30]
	ldr r1, [r0]
	tst r1, #1
	beq _020C6ADC
	mov r1, #0x1000
	str r1, [sp, #8]
	str r1, [sp, #0x18]
	b _020C6B90
_020C6ADC:
	ldr r1, [r0, #0x18]
	str r1, [sp, #8]
	ldr r1, [r0, #0x1c]
	str r1, [sp, #0x18]
	b _020C6B90
_020C6AF0:
	tst r1, #1
	beq _020C6B38
	ldr r2, [r0, #0x24]
	ldrh r1, [r0, #0x2c]
	mov r2, r2, lsl #4
	rsb r2, r2, #0
	mul r1, r2, r1
	str r1, [sp, #0x2c]
	ldr r2, [r0, #0x28]
	ldrh r1, [r0, #0x2e]
	mov r2, r2, lsl #4
	rsb r2, r2, #0
	mul r3, r2, r1
	mov r1, #0x1000
	str r3, [sp, #0x30]
	str r1, [sp, #8]
	str r1, [sp, #0x18]
	b _020C6B90
_020C6B38:
	ldr r2, [r0, #0x18]
	ldr r1, [r0, #0x24]
	ldrh ip, [r0, #0x2c]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #8
	orr r2, r2, r1, lsl #24
	rsb r1, r2, #0
	mul r1, ip, r1
	str r1, [sp, #0x2c]
	ldr r2, [r0, #0x1c]
	ldr r1, [r0, #0x28]
	ldrh ip, [r0, #0x2e]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #8
	orr r2, r2, r1, lsl #24
	rsb r1, r2, #0
	mul r1, ip, r1
	str r1, [sp, #0x30]
	ldr r1, [r0, #0x18]
	str r1, [sp, #8]
	ldr r1, [r0, #0x1c]
	str r1, [sp, #0x18]
_020C6B90:
	ldr ip, [r0, #0x30]
	cmp ip, #0x1000
	beq _020C6BC8
	ldr r2, [sp, #8]
	ldr r1, [sp, #0x2c]
	smull r3, r2, ip, r2
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	str r3, [sp, #8]
	ldr r2, [r0, #0x30]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x2c]
_020C6BC8:
	ldr ip, [r0, #0x34]
	cmp ip, #0x1000
	beq _020C6C00
	ldr r2, [sp, #0x18]
	ldr r1, [sp, #0x30]
	smull r3, r2, ip, r2
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	str r3, [sp, #0x18]
	ldr r0, [r0, #0x34]
	smull r2, r1, r0, r1
	mov r0, r2, lsr #0xc
	orr r0, r0, r1, lsl #20
	str r0, [sp, #0x30]
_020C6C00:
	add r1, sp, #0
	ldr r0, [sp]
	add r1, r1, #4
	mov r2, #0xe
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0x3c
	ldmia sp!, {pc}
	.align 2, 0
_020C6C1C: .word 0x00101710
_020C6C20: .word 0x00101910
	arm_func_end NNSi_G3dSendTexSRTSi3d

	arm_func_start texmtxCalc_flag___3dsmax
texmtxCalc_flag___3dsmax: ; 0x020C6C24
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	ldrh r2, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	mov sl, r0
	mov r8, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, r8
	bl FX_DivAsync
	ldrsh r0, [sb, #0x22]
	ldr r3, [sb, #0x18]
	ldrsh r5, [sb, #0x20]
	ldr r4, [sb, #0x1c]
	smull r2, r1, r3, r0
	mov r7, r2, lsr #0xc
	orr r7, r7, r1, lsl #20
	smull r2, r1, r3, r5
	mov r6, r2, lsr #0xc
	orr r6, r6, r1, lsl #20
	smull r3, r2, r4, r0
	smull r1, r0, r4, r5
	mov r5, r3, lsr #0xc
	orr r5, r5, r2, lsl #20
	mov r4, r1, lsr #0xc
	orr r4, r4, r0, lsl #20
	str r7, [sl]
	str r5, [sl, #0x14]
	bl FX_GetDivResult
	mov r1, fp
	mul r0, r4, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #4]
	mov r0, r8
	bl FX_DivAsync
	ldrh r1, [sb, #0x2c]
	ldrh r8, [sb, #0x2e]
	ldr r2, [sb, #0x28]
	rsb r0, r1, #0
	mul r3, r2, r8
	rsb r2, r8, #0
	add r8, r3, r2, lsl #11
	smull r3, r2, r5, r8
	smull r8, r5, r6, r8
	ldr fp, [sb, #0x24]
	mul ip, fp, r1
	rsb r0, ip, r0, lsl #11
	smlal r3, r2, r4, r0
	smull r4, r0, r7, r0
	subs r4, r4, r8
	sbc r0, r0, r5
	mov r4, r4, lsr #8
	orr r4, r4, r0, lsl #24
	add r0, r4, r1, lsl #15
	str r0, [sl, #0x30]
	ldrh r1, [sb, #0x2e]
	mov r0, r3, lsr #8
	orr r0, r0, r2, lsl #24
	add r0, r0, r1, lsl #15
	str r0, [sl, #0x34]
	bl FX_GetDivResult
	rsb r1, r6, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flag___3dsmax

	arm_func_start texmtxCalc_flagS___3dsmax
texmtxCalc_flagS___3dsmax: ; 0x020C6D2C
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrh r2, [r5, #0x2c]
	ldrh r4, [r5, #0x2e]
	ldr r1, [r5, #0x28]
	ldr r0, [r5, #0x24]
	mul r3, r1, r4
	rsb r1, r4, #0
	mul r4, r0, r2
	rsb r0, r2, #0
	add r1, r3, r1, lsl #11
	ldrsh r3, [r5, #0x20]
	rsb r0, r4, r0, lsl #11
	ldrsh lr, [r5, #0x22]
	smull ip, r4, r3, r1
	smull r7, r3, lr, r0
	subs r7, r7, ip
	sbc r3, r3, r4
	mov r4, r7, lsr #8
	orr r4, r4, r3, lsl #24
	add r2, r4, r2, lsl #15
	str r2, [r6, #0x30]
	ldrsh r2, [r5, #0x22]
	ldrsh r3, [r5, #0x20]
	ldrh ip, [r5, #0x2e]
	smull r4, r1, r2, r1
	smlal r4, r1, r3, r0
	mov r0, r4, lsr #8
	orr r0, r0, r1, lsl #24
	add r0, r0, ip, lsl #15
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	rsb r1, r1, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagS___3dsmax

	arm_func_start texmtxCalc_flagR___3dsmax
texmtxCalc_flagR___3dsmax: ; 0x020C6E14
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, [r1, #0x18]
	mov r3, #0
	str r2, [r0]
	ldr r2, [r1, #0x1c]
	str r2, [r0, #0x14]
	str r3, [r0, #4]
	ldrh r5, [r1, #0x2c]
	ldr ip, [r1, #0x24]
	ldrh r2, [r1, #0x2e]
	mul lr, ip, r5
	rsb ip, r5, #0
	ldr r4, [r1, #0x18]
	rsb ip, lr, ip, lsl #11
	smull lr, ip, r4, ip
	ldr r6, [r1, #0x28]
	mov r4, lr, lsr #8
	orr r4, r4, ip, lsl #24
	add r4, r4, r5, lsl #15
	str r4, [r0, #0x30]
	mul r4, r6, r2
	rsb r5, r2, #0
	ldr r2, [r1, #0x1c]
	add r4, r4, r5, lsl #11
	smull lr, ip, r2, r4
	mov r2, lr, lsr #8
	ldrh r1, [r1, #0x2e]
	orr r2, r2, ip, lsl #24
	add r1, r2, r1, lsl #15
	str r1, [r0, #0x34]
	str r3, [r0, #0x10]
	ldmia sp!, {r4, r5, r6, pc}
	arm_func_end texmtxCalc_flagR___3dsmax

	arm_func_start texmtxCalc_flagRS___3dsmax
texmtxCalc_flagRS___3dsmax: ; 0x020C6E94
	mov r2, #0x1000
	str r2, [r0]
	str r2, [r0, #0x14]
	mov ip, #0
	str ip, [r0, #4]
	ldr r3, [r1, #0x24]
	ldrh r2, [r1, #0x2c]
	rsb r3, r3, #0
	mul r2, r3, r2
	mov r2, r2, lsl #4
	str r2, [r0, #0x30]
	ldrh r2, [r1, #0x2e]
	ldr r1, [r1, #0x28]
	mul r2, r1, r2
	mov r1, r2, lsl #4
	str r1, [r0, #0x34]
	str ip, [r0, #0x10]
	bx lr
	arm_func_end texmtxCalc_flagRS___3dsmax

	arm_func_start texmtxCalc_flagT___3dsmax
texmtxCalc_flagT___3dsmax: ; 0x020C6EDC
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	ldrh r2, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	mov sl, r0
	mov r8, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, r8
	bl FX_DivAsync
	ldrsh r0, [sb, #0x22]
	ldr r3, [sb, #0x18]
	ldrsh r5, [sb, #0x20]
	ldr r4, [sb, #0x1c]
	smull r2, r1, r3, r0
	mov r7, r2, lsr #0xc
	orr r7, r7, r1, lsl #20
	smull r2, r1, r3, r5
	mov r6, r2, lsr #0xc
	orr r6, r6, r1, lsl #20
	smull r3, r2, r4, r0
	smull r1, r0, r4, r5
	mov r5, r3, lsr #0xc
	orr r5, r5, r2, lsl #20
	mov r4, r1, lsr #0xc
	orr r4, r4, r0, lsl #20
	str r7, [sl]
	str r5, [sl, #0x14]
	bl FX_GetDivResult
	mov r1, fp
	mul r0, r4, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #4]
	mov r0, r8
	bl FX_DivAsync
	ldrh r3, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	rsb r0, r3, #0
	rsb r1, r1, #0
	mov r2, r1, lsl #0xb
	mov fp, r0, lsl #0xb
	smull r1, r0, r7, fp
	smull r8, r7, r5, r2
	smlal r8, r7, r4, fp
	mov r4, r8, lsr #8
	orr r4, r4, r7, lsl #24
	smull r5, r2, r6, r2
	subs r1, r1, r5
	sbc r0, r0, r2
	mov r1, r1, lsr #8
	orr r1, r1, r0, lsl #24
	add r0, r1, r3, lsl #15
	str r0, [sl, #0x30]
	ldrh r0, [sb, #0x2e]
	add r0, r4, r0, lsl #15
	str r0, [sl, #0x34]
	bl FX_GetDivResult
	rsb r1, r6, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flagT___3dsmax

	arm_func_start texmtxCalc_flagTS___3dsmax
texmtxCalc_flagTS___3dsmax: ; 0x020C6FD4
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	ldrsh r4, [r5, #0x22]
	rsb r0, r2, #0
	rsb r1, r1, #0
	mov r0, r0, lsl #0xb
	ldrsh r3, [r5, #0x20]
	mov r1, r1, lsl #0xb
	smull r7, lr, r4, r0
	smull ip, r4, r3, r1
	subs r7, r7, ip
	sbc r3, lr, r4
	mov r4, r7, lsr #8
	orr r4, r4, r3, lsl #24
	add r2, r4, r2, lsl #15
	str r2, [r6, #0x30]
	ldrsh r2, [r5, #0x22]
	ldrsh r3, [r5, #0x20]
	ldrh ip, [r5, #0x2e]
	smull r4, r1, r2, r1
	smlal r4, r1, r3, r0
	mov r0, r4, lsr #8
	orr r0, r0, r1, lsl #24
	add r0, r0, ip, lsl #15
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	rsb r1, r1, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagTS___3dsmax

	arm_func_start texmtxCalc_flagTR___3dsmax
texmtxCalc_flagTR___3dsmax: ; 0x020C70AC
	ldr r2, [r1, #0x18]
	mov ip, #0
	str r2, [r0]
	ldr r2, [r1, #0x1c]
	str r2, [r0, #0x14]
	str ip, [r0, #4]
	ldr r3, [r1, #0x18]
	ldrh r2, [r1, #0x2c]
	rsb r3, r3, #0x1000
	mul r2, r3, r2
	mov r2, r2, lsl #3
	str r2, [r0, #0x30]
	ldr r2, [r1, #0x1c]
	ldrh r1, [r1, #0x2e]
	rsb r2, r2, #0x1000
	mul r1, r2, r1
	mov r1, r1, lsl #3
	str r1, [r0, #0x34]
	str ip, [r0, #0x10]
	bx lr
	arm_func_end texmtxCalc_flagTR___3dsmax

	arm_func_start texmtxCalc_flagTRS___3dsmax
texmtxCalc_flagTRS___3dsmax: ; 0x020C70FC
	mov r2, #0x1000
	str r2, [r0]
	mov r1, #0
	str r1, [r0, #4]
	str r1, [r0, #0x10]
	str r2, [r0, #0x14]
	str r1, [r0, #0x30]
	str r1, [r0, #0x34]
	bx lr
	arm_func_end texmtxCalc_flagTRS___3dsmax

	arm_func_start NNSi_G3dSendTexSRT3dsMax
NNSi_G3dSendTexSRT3dsMax: ; 0x020C7120
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x4c
	mov r4, r0
	ldr r0, [r4]
	mov r1, #0
	tst r0, #8
	ldrne r0, _020C725C ; =0x00101610
	mov r3, #3
	strne r0, [sp]
	ldreq r0, _020C7260 ; =0x00101810
	mov r2, #2
	streq r0, [sp]
	mov r0, #0x1000
	str r0, [sp, #0x44]
	str r3, [sp, #4]
	str r2, [sp, #0x48]
	str r1, [sp, #0x40]
	str r1, [sp, #0x34]
	str r1, [sp, #0x30]
	str r1, [sp, #0x2c]
	str r1, [sp, #0x28]
	str r1, [sp, #0x24]
	str r1, [sp, #0x20]
	str r1, [sp, #0x14]
	str r1, [sp, #0x10]
	ldr r1, [r4]
	ldr r0, _020C7264 ; =_02110C3C
	and r1, r1, #7
	ldr r2, [r0, r1, lsl #2]
	add r0, sp, #8
	mov r1, r4
	blx r2
	ldr r3, [r4, #0x30]
	cmp r3, #0x1000
	beq _020C71F0
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #8]
	ldr r2, [r4, #0x30]
	ldr r0, [sp, #0x38]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0xc]
	ldr r1, [r4, #0x30]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x38]
_020C71F0:
	ldr r3, [r4, #0x34]
	cmp r3, #0x1000
	beq _020C7240
	ldr r0, [sp, #0x18]
	ldr r1, [sp, #0x1c]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #0x18]
	ldr r2, [r4, #0x34]
	ldr r0, [sp, #0x3c]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x1c]
	ldr r1, [r4, #0x34]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x3c]
_020C7240:
	add r1, sp, #0
	ldr r0, [sp]
	add r1, r1, #4
	mov r2, #0x12
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0x4c
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020C725C: .word 0x00101610
_020C7260: .word 0x00101810
_020C7264: .word _02110C3C
	arm_func_end NNSi_G3dSendTexSRT3dsMax

	arm_func_start texmtxCalc_flag___xsi
texmtxCalc_flag___xsi: ; 0x020C7268
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r6, r1
	ldrh r2, [r6, #0x2c]
	ldrh r1, [r6, #0x2e]
	mov r7, r0
	mov sl, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, sl
	bl FX_DivAsync
	ldrsh r3, [r6, #0x22]
	ldr r0, [r6, #0x18]
	ldrsh sb, [r6, #0x20]
	smull r2, r1, r0, r3
	mov r2, r2, lsr #0xc
	ldr r8, [r6, #0x1c]
	orr r2, r2, r1, lsl #20
	str r2, [r7]
	smull r2, r1, r8, r3
	mov r4, r2, lsr #0xc
	orr r4, r4, r1, lsl #20
	smull r2, r1, r0, sb
	mov r5, r2, lsr #0xc
	orr r5, r5, r1, lsl #20
	str r4, [r7, #0x14]
	bl FX_GetDivResult
	smull r2, r1, r8, sb
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	mul r0, r2, r0
	mov r0, r0, asr #0xc
	str r0, [r7, #4]
	mov r0, sl
	mov r1, fp
	bl FX_DivAsync
	ldr lr, [r6, #0x1c]
	ldrsh r2, [r6, #0x20]
	ldr r8, [r6, #0x24]
	ldr r0, [r6, #0x28]
	smull sl, sb, r8, r2
	ldrh r1, [r6, #0x2c]
	smull r3, r2, r0, r2
	ldrsh ip, [r6, #0x22]
	str r1, [sp]
	mov fp, lr, asr #0x1f
	smlal r3, r2, r8, ip
	smull r8, ip, r0, ip
	subs r8, sl, r8
	sbc r0, sb, ip
	mov sb, r8, lsr #0xc
	mov ip, r3, lsr #0xc
	orr sb, sb, r0, lsl #20
	mov r3, r2, asr #0xc
	orr ip, ip, r2, lsl #20
	umull sl, r2, sb, lr
	mla r2, sb, fp, r2
	mov r8, r0, asr #0xc
	mla r2, r8, lr, r2
	ldr r1, [r6, #0x18]
	mov r8, sl, lsr #0xc
	orr r8, r8, r2, lsl #20
	add r2, r4, r8
	mov r0, r1, asr #0x1f
	umull r8, r4, ip, r1
	mla r4, ip, r0, r4
	mla r4, r3, r1, r4
	mov r0, r8, lsr #0xc
	orr r0, r0, r4, lsl #20
	sub r1, r5, r0
	ldr r0, [sp]
	sub r2, r2, #0x1000
	mul r1, r0, r1
	mov r0, r1, lsl #4
	str r0, [r7, #0x30]
	ldrh r0, [r6, #0x2e]
	rsb r0, r0, #0
	mul r1, r0, r2
	mov r0, r1, lsl #4
	str r0, [r7, #0x34]
	bl FX_GetDivResult
	rsb r1, r5, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r7, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flag___xsi

	arm_func_start texmtxCalc_flagS___xsi
texmtxCalc_flagS___xsi: ; 0x020C73BC
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrsh lr, [r5, #0x20]
	ldr r4, [r5, #0x28]
	ldrsh r0, [r5, #0x22]
	ldr ip, [r5, #0x24]
	smull r3, r2, r4, lr
	smlal r3, r2, ip, r0
	smull r1, r0, r4, r0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	sub r4, lr, r3
	smull r3, r2, ip, lr
	subs r1, r3, r1
	sbc r0, r2, r0
	ldrh r2, [r5, #0x2c]
	mov r1, r1, lsr #0xc
	orr r1, r1, r0, lsl #20
	mul r0, r2, r4
	mov r0, r0, lsl #4
	str r0, [r6, #0x30]
	ldrsh r0, [r5, #0x22]
	ldrh r2, [r5, #0x2e]
	add r0, r0, r1
	rsb r1, r2, #0
	sub r0, r0, #0x1000
	mul r0, r1, r0
	mov r0, r0, lsl #4
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	rsb r1, r1, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagS___xsi

	arm_func_start texmtxCalc_flagR___xsi
texmtxCalc_flagR___xsi: ; 0x020C749C
	stmdb sp!, {r4, lr}
	ldr r3, [r1, #0x18]
	mov r2, #0
	str r3, [r0]
	ldr r3, [r1, #0x1c]
	str r3, [r0, #0x14]
	str r2, [r0, #4]
	ldr ip, [r1, #0x24]
	ldr r3, [r1, #0x18]
	ldr r4, [r1, #0x28]
	smull lr, r3, ip, r3
	mov ip, lr, lsr #0xc
	orr ip, ip, r3, lsl #20
	ldrh lr, [r1, #0x2c]
	rsb r3, ip, #0
	ldr ip, [r1, #0x1c]
	rsb r4, r4, #0
	mul r3, lr, r3
	smull lr, ip, r4, ip
	mov r3, r3, lsl #4
	str r3, [r0, #0x30]
	mov r4, lr, lsr #0xc
	ldrh r3, [r1, #0x2e]
	ldr r1, [r1, #0x1c]
	orr r4, r4, ip, lsl #20
	add r1, r1, r4
	rsb r3, r3, #0
	sub r1, r1, #0x1000
	mul r1, r3, r1
	mov r1, r1, lsl #4
	str r1, [r0, #0x34]
	str r2, [r0, #0x10]
	ldmia sp!, {r4, pc}
	arm_func_end texmtxCalc_flagR___xsi

	arm_func_start texmtxCalc_flagRS___xsi
texmtxCalc_flagRS___xsi: ; 0x020C7520
	stmdb sp!, {r3, lr}
	mov r2, #0x1000
	str r2, [r0]
	str r2, [r0, #0x14]
	mov lr, #0
	str lr, [r0, #4]
	ldr r2, [r1, #0x24]
	ldrh r3, [r1, #0x2c]
	rsb r2, r2, #0
	ldr ip, [r1, #0x28]
	mul r2, r3, r2
	mov r2, r2, lsl #4
	str r2, [r0, #0x30]
	ldrh r1, [r1, #0x2e]
	rsb r2, ip, #0
	rsb r1, r1, #0
	mul r2, r1, r2
	mov r1, r2, lsl #4
	str r1, [r0, #0x34]
	str lr, [r0, #0x10]
	ldmia sp!, {r3, pc}
	arm_func_end texmtxCalc_flagRS___xsi

	arm_func_start texmtxCalc_flagT___xsi
texmtxCalc_flagT___xsi: ; 0x020C7574
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov sb, r1
	ldrh r2, [sb, #0x2c]
	ldrh r1, [sb, #0x2e]
	mov sl, r0
	mov r8, r2, lsl #0xc
	mov fp, r1, lsl #0xc
	mov r0, fp
	mov r1, r8
	bl FX_DivAsync
	ldrsh r3, [sb, #0x22]
	ldr r0, [sb, #0x18]
	ldrsh r7, [sb, #0x20]
	ldr r5, [sb, #0x1c]
	smull r2, r1, r0, r3
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sl]
	smull r2, r1, r5, r3
	mov r4, r2, lsr #0xc
	orr r4, r4, r1, lsl #20
	smull r2, r1, r0, r7
	mov r6, r2, lsr #0xc
	orr r6, r6, r1, lsl #20
	str r4, [sl, #0x14]
	bl FX_GetDivResult
	smull r2, r1, r5, r7
	mov r2, r2, lsr #0xc
	orr r2, r2, r1, lsl #20
	mul r0, r2, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #4]
	mov r0, r8
	mov r1, fp
	bl FX_DivAsync
	sub r0, r4, #0x1000
	ldrh r1, [sb, #0x2c]
	mul r2, r1, r6
	mov r1, r2, lsl #4
	str r1, [sl, #0x30]
	ldrh r1, [sb, #0x2e]
	rsb r1, r1, #0
	mul r0, r1, r0
	mov r0, r0, lsl #4
	str r0, [sl, #0x34]
	bl FX_GetDivResult
	rsb r1, r6, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [sl, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	arm_func_end texmtxCalc_flagT___xsi

	arm_func_start texmtxCalc_flagTS___xsi
texmtxCalc_flagTS___xsi: ; 0x020C7640
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r5, r1
	ldrh r2, [r5, #0x2c]
	ldrh r1, [r5, #0x2e]
	mov r6, r0
	mov r4, r2, lsl #0xc
	mov r7, r1, lsl #0xc
	mov r0, r7
	mov r1, r4
	bl FX_DivAsync
	ldrsh r0, [r5, #0x22]
	str r0, [r6]
	ldrsh r0, [r5, #0x22]
	str r0, [r6, #0x14]
	bl FX_GetDivResult
	ldrsh r2, [r5, #0x20]
	mov r1, r7
	mul r0, r2, r0
	mov r2, r0, asr #0xc
	mov r0, r4
	str r2, [r6, #4]
	bl FX_DivAsync
	ldrh r1, [r5, #0x2c]
	ldrsh r0, [r5, #0x20]
	mul r0, r1, r0
	mov r0, r0, lsl #4
	str r0, [r6, #0x30]
	ldrh r1, [r5, #0x2e]
	ldrsh r0, [r5, #0x22]
	rsb r1, r1, #0
	sub r0, r0, #0x1000
	mul r0, r1, r0
	mov r0, r0, lsl #4
	str r0, [r6, #0x34]
	bl FX_GetDivResult
	ldrsh r1, [r5, #0x20]
	rsb r1, r1, #0
	mul r0, r1, r0
	mov r0, r0, asr #0xc
	str r0, [r6, #0x10]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end texmtxCalc_flagTS___xsi

	arm_func_start texmtxCalc_flagTR___xsi
texmtxCalc_flagTR___xsi: ; 0x020C76E4
	ldr r2, [r1, #0x18]
	mov r3, #0
	str r2, [r0]
	ldr r2, [r1, #0x1c]
	str r2, [r0, #0x14]
	str r3, [r0, #4]
	str r3, [r0, #0x30]
	ldrh r2, [r1, #0x2e]
	ldr r1, [r1, #0x1c]
	rsb r2, r2, #0
	sub r1, r1, #0x1000
	mul r1, r2, r1
	mov r1, r1, lsl #4
	str r1, [r0, #0x34]
	str r3, [r0, #0x10]
	bx lr
	arm_func_end texmtxCalc_flagTR___xsi

	arm_func_start texmtxCalc_flagTRS___xsi
texmtxCalc_flagTRS___xsi: ; 0x020C7724
	mov r2, #0x1000
	str r2, [r0]
	mov r1, #0
	str r1, [r0, #4]
	str r1, [r0, #0x10]
	str r2, [r0, #0x14]
	str r1, [r0, #0x30]
	str r1, [r0, #0x34]
	bx lr
	arm_func_end texmtxCalc_flagTRS___xsi

	arm_func_start NNSi_G3dSendTexSRTXsi
NNSi_G3dSendTexSRTXsi: ; 0x020C7748
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0x4c
	mov r4, r0
	ldr r0, [r4]
	mov r3, #3
	tst r0, #8
	ldrne r0, _020C78C4 ; =0x00101610
	mov r2, #2
	strne r0, [sp]
	ldreq r0, _020C78C8 ; =0x00101810
	mov r1, #0x1000
	streq r0, [sp]
	mov r0, #0
	str r3, [sp, #4]
	str r2, [sp, #0x48]
	str r1, [sp, #0x44]
	str r0, [sp, #0x40]
	str r0, [sp, #0x34]
	str r0, [sp, #0x30]
	str r0, [sp, #0x2c]
	str r0, [sp, #0x28]
	str r0, [sp, #0x24]
	str r0, [sp, #0x20]
	str r0, [sp, #0x14]
	str r0, [sp, #0x10]
	ldr r0, [r4]
	tst r0, #1
	strne r1, [r4, #0x1c]
	strne r1, [r4, #0x18]
	ldr r0, [r4]
	tst r0, #2
	beq _020C77D8
	mov r0, #0x1000
	strh r0, [r4, #0x22]
	mov r0, #0
	strh r0, [r4, #0x20]
_020C77D8:
	ldr r0, [r4]
	tst r0, #4
	movne r0, #0
	strne r0, [r4, #0x28]
	strne r0, [r4, #0x24]
	ldr r1, [r4]
	ldr r0, _020C78CC ; =_02110C5C
	and r1, r1, #7
	ldr r2, [r0, r1, lsl #2]
	add r0, sp, #8
	mov r1, r4
	blx r2
	ldr r3, [r4, #0x30]
	cmp r3, #0x1000
	beq _020C7858
	ldr r0, [sp, #8]
	ldr r1, [sp, #0xc]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #8]
	ldr r2, [r4, #0x30]
	ldr r0, [sp, #0x38]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0xc]
	ldr r1, [r4, #0x30]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x38]
_020C7858:
	ldr r3, [r4, #0x34]
	cmp r3, #0x1000
	beq _020C78A8
	ldr r0, [sp, #0x18]
	ldr r1, [sp, #0x1c]
	smull r2, r0, r3, r0
	mov r2, r2, lsr #0xc
	orr r2, r2, r0, lsl #20
	str r2, [sp, #0x18]
	ldr r2, [r4, #0x34]
	ldr r0, [sp, #0x3c]
	smull r3, r1, r2, r1
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	str r2, [sp, #0x1c]
	ldr r1, [r4, #0x34]
	smull r2, r0, r1, r0
	mov r1, r2, lsr #0xc
	orr r1, r1, r0, lsl #20
	str r1, [sp, #0x3c]
_020C78A8:
	add r1, sp, #0
	ldr r0, [sp]
	add r1, r1, #4
	mov r2, #0x12
	bl NNS_G3dGeBufferOP_N
	add sp, sp, #0x4c
	ldmia sp!, {r3, r4, pc}
	.align 2, 0
_020C78C4: .word 0x00101610
_020C78C8: .word 0x00101810
_020C78CC: .word _02110C5C
	arm_func_end NNSi_G3dSendTexSRTXsi

	arm_func_start NNS_SndInit
NNS_SndInit: ; 0x020C78D0
	stmdb sp!, {r3, lr}
	ldr r0, _020C7944 ; =_021DD420
	ldr r1, [r0, #0xc]
	cmp r1, #0
	ldmneia sp!, {r3, pc}
	mov r1, #1
	str r1, [r0, #0xc]
	bl SND_Init
	ldr r0, _020C7948 ; =BeginSleep
	ldr r1, _020C7944 ; =_021DD420
	mov r2, #0
	str r0, [r1, #0x10]
	ldr r0, _020C794C ; =EndSleep
	str r2, [r1, #0x14]
	str r0, [r1, #0x1c]
	ldr r0, _020C7950 ; =_021DD430
	str r2, [r1, #0x20]
	bl PM_PrependPreSleepCallback
	ldr r0, _020C7954 ; =_021DD43C
	bl PM_AppendPostSleepCallback
	bl NNSi_SndInitResourceMgr
	bl NNSi_SndCaptureInit
	bl NNSi_SndPlayerInit
	ldr r0, _020C7944 ; =_021DD420
	mvn r1, #0
	strb r1, [r0]
	mov r1, #1
	str r1, [r0, #4]
	ldmia sp!, {r3, pc}
	.align 2, 0
_020C7944: .word _021DD420
_020C7948: .word BeginSleep
_020C794C: .word EndSleep
_020C7950: .word _021DD430
_020C7954: .word _021DD43C
	arm_func_end NNS_SndInit
