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
	.extern getMdlTrans_
	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
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
