	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_SendFixedBeacon
	.extern MBi_calc_cksum
	.extern bsendBuff
	.extern ov13_022474A0

.text
	arm_func_start MBi_SendFixedBeacon
MBi_SendFixedBeacon: ; 0x0221DFF0
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	sub sp, sp, #8
	ldr r3, _0221E144 ; =ov13_022474A0
	mov r6, r0
	ldr r4, [r3, #0x1c]
	ldr r0, [r3, #0x20]
	add ip, r4, #0x358
	add r7, r0, #0x62
	cmp r7, ip
	movls r0, #0x62
	mov r5, r1
	mov r4, r2
	strlsb r0, [r3, #0x4c]
	bls _0221E048
	sub r7, ip, r0
	and r1, r7, #0xff
	ldr r0, _0221E148 ; =bsendBuff + 0xE
	rsb r2, r1, #0x62
	add r1, r0, r1
	mov r0, #0
	strb r7, [r3, #0x4c]
	bl MIi_CpuClear16
_0221E048:
	ldr r0, _0221E144 ; =ov13_022474A0
	ldr r1, _0221E148 ; =bsendBuff + 0xE
	ldrb r2, [r0, #0x4c]
	ldr r0, [r0, #0x20]
	bl MIi_CpuCopy16
	ldr r2, _0221E144 ; =ov13_022474A0
	ldr r0, _0221E14C ; =bsendBuff + 0x8
	ldrb r7, [r2, #0x27]
	mov r3, #0
	mov r1, #0x68
	strb r7, [r2, #0x4a]
	ldrb r7, [r2, #0x28]
	strb r7, [r2, #0x4b]
	ldr r7, [r2, #0x1c]
	ldrb lr, [r2, #0x44]
	ldrb ip, [r7, #0x4b2]
	bic lr, lr, #3
	and ip, ip, #3
	orr ip, lr, ip
	strb ip, [r2, #0x44]
	ldrb ip, [r7, #0x4b3]
	strb ip, [r2, #0x45]
	ldrb ip, [r2, #0x26]
	strb ip, [r2, #0x46]
	ldr ip, [r7, #0x4b8]
	str ip, [r2, #0x40]
	ldrb lr, [r2, #0x44]
	ldrb ip, [r7, #0x4b5]
	bic lr, lr, #0xfc
	mov ip, ip, lsl #0x1a
	orr ip, lr, ip, lsr #24
	strb ip, [r2, #0x44]
	ldrb lr, [r2, #0x29]
	add ip, lr, #1
	strb ip, [r2, #0x29]
	strb lr, [r2, #0x47]
	strh r3, [r2, #0x48]
	bl MBi_calc_cksum
	ldr r1, _0221E144 ; =ov13_022474A0
	strh r0, [r1, #0x48]
	ldrb r0, [r1, #0x27]
	add r2, r0, #1
	strb r2, [r1, #0x27]
	ldrb r0, [r1, #0x28]
	and r2, r2, #0xff
	cmp r2, r0
	movhs r0, #4
	strhsb r0, [r1, #0x24]
	bhs _0221E118
	ldr r0, [r1, #0x20]
	add r0, r0, #0x62
	str r0, [r1, #0x20]
_0221E118:
	orr r0, r4, #3
	ldr r1, _0221E150 ; =bsendBuff
	mov r3, r6
	str r5, [sp]
	and r4, r0, #0xff
	mov r0, #0
	mov r2, #0x70
	str r4, [sp, #4]
	bl WM_SetGameInfo
	add sp, sp, #8
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	.balign 4, 0
_0221E144: .word ov13_022474A0
_0221E148: .word bsendBuff + 0xE
_0221E14C: .word bsendBuff + 0x8
_0221E150: .word bsendBuff
	arm_func_end MBi_SendFixedBeacon
