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
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
