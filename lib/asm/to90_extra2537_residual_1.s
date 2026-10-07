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
