	.extern MBi_CheckWmErrcode
	.extern MBi_InitSendVolatBeacon
	.extern MBi_ParentCallback
	.extern MBi_calc_cksum
	.extern _start_AutoloadDoneCallback
	.extern bsendBuff
	.extern ov13_0224598C
	.extern ov13_022474A0
	.extern ov13_0224CF40
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"

.text
	arm_func_start MBi_IsSendEnabled
MBi_IsSendEnabled: ; 0x0221E460
	ldr r1, _0221E4BC ; =ov13_0224CF40
	mov r0, #0
	ldr ip, [r1, #8]
	mov r3, r0
	add r1, ip, #0x500
	ldrh r1, [r1, #0x28]
	mov r2, r0
	cmp r1, #1
	ldreqb r1, [ip, #0x50c]
	cmpeq r1, #0
	moveq r3, #1
	cmp r3, #0
	beq _0221E4A4
	add r1, ip, #0x500
	ldrh r1, [r1, #0x26]
	cmp r1, #0
	moveq r2, #1
_0221E4A4:
	cmp r2, #0
	addne r1, ip, #0x500
	ldrneh r1, [r1, #0x2a]
	cmpne r1, #0
	movne r0, #1
	bx lr
	.balign 4, 0
_0221E4BC: .word ov13_0224CF40
	arm_func_end MBi_IsSendEnabled
