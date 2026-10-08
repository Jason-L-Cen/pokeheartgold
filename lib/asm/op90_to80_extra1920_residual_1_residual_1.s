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
