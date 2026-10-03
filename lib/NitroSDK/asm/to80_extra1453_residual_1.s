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
	arm_func_start MBi_SendVolatBeacon
MBi_SendVolatBeacon: ; 0x0221E184
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, fp, lr}
	sub sp, sp, #0x14
	ldr r3, _0221E3DC ; =ov13_022474A0
	str r0, [sp, #8]
	ldr r0, [r3, #0x1c]
	ldrb r3, [r3, #0x26]
	ldrb r0, [r0, #0x4b4]
	str r1, [sp, #0xc]
	str r2, [sp, #0x10]
	cmp r3, r0
	beq _0221E1B4
	bl MBi_InitSendVolatBeacon
_0221E1B4:
	ldr r2, _0221E3DC ; =ov13_022474A0
	ldrb r0, [r2, #0x44]
	bic r0, r0, #3
	orr r0, r0, #2
	strb r0, [r2, #0x44]
	ldr r0, [r2, #0x1c]
	ldrb r3, [r0, #0x4b3]
	add r1, r0, #0x300
	strb r3, [r2, #0x45]
	ldrb r3, [r2, #0x26]
	strb r3, [r2, #0x46]
	ldr r3, [r0, #0x4b8]
	str r3, [r2, #0x40]
	ldrb r4, [r2, #0x44]
	ldrb r3, [r0, #0x4b5]
	bic r4, r4, #0xfc
	mov r3, r3, lsl #0x1a
	orr r3, r4, r3, lsr #24
	strb r3, [r2, #0x44]
	ldrb r4, [r2, #0x29]
	add r3, r4, #1
	strb r3, [r2, #0x29]
	strb r4, [r2, #0x47]
	ldrb r3, [r0, #0x358]
	strb r3, [r2, #0x4a]
	ldrh r3, [r1, #0x5a]
	strh r3, [r2, #0x4c]
	ldrh r1, [r1, #0x5c]
	strh r1, [r2, #0x4e]
	ldr r1, [r2, #0x10]
	cmp r1, #0
	bne _0221E248
	ldr r1, [r2, #0x14]
	cmp r1, #0
	beq _0221E248
	ldr r0, [r0, #0x4b8]
	blx r1
_0221E248:
	ldr r0, _0221E3DC ; =ov13_022474A0
	mov sb, #0
	ldr r1, _0221E3E0 ; =bsendBuff + 0x68
	ldr r2, [r0, #0x1c]
	b _0221E26C
_0221E25C:
	add r0, r2, sb
	ldrb r0, [r0, #0x4a8]
	strb r0, [r1, sb]
	add sb, sb, #1
_0221E26C:
	cmp sb, #8
	blt _0221E25C
	ldr r1, _0221E3E4 ; =bsendBuff + 0x10
	mov r0, #0
	mov r2, #0x58
	bl MIi_CpuClear16
	ldr r5, _0221E3DC ; =ov13_022474A0
	mov sl, #0
	ldr r1, [r5, #0x1c]
	mov r6, #2
	add r0, r1, #0x400
	add r1, r1, #0x300
	ldrh r2, [r0, #0xb0]
	ldrh r0, [r1, #0x5a]
	mov sb, sl
	ldr r7, _0221E3E4 ; =bsendBuff + 0x10
	eor r0, r2, r0
	mov r4, r0, lsl #0x10
	mov r8, r6
	mov fp, #0x16
	b _0221E310
_0221E2C0:
	mov r0, r8, lsl sb
	tst r0, r4, lsr #16
	beq _0221E30C
	ldr r0, [r5, #0x1c]
	mla r1, sl, fp, r7
	add r0, r0, #0x5e
	add r2, r0, #0x300
	mov r0, #0x16
	mla r0, sb, r0, r2
	mov r2, #0x16
	bl MIi_CpuCopy16
	ldr r0, [r5, #0x1c]
	add sl, sl, #1
	add r0, r0, #0x400
	ldrh r1, [r0, #0xb0]
	cmp sl, #4
	orr r1, r1, r6, lsl sb
	strh r1, [r0, #0xb0]
	beq _0221E318
_0221E30C:
	add sb, sb, #1
_0221E310:
	cmp sb, #0xf
	blt _0221E2C0
_0221E318:
	cmp sl, #4
	bhs _0221E338
	mov r0, #0x16
	mul r1, sl, r0
	ldr r2, _0221E3E4 ; =bsendBuff + 0x10
	ldrb r0, [r2, r1]
	bic r0, r0, #0xf0
	strb r0, [r2, r1]
_0221E338:
	ldr r2, _0221E3DC ; =ov13_022474A0
	mov r3, #0
	ldr r0, _0221E3E8 ; =bsendBuff + 0x8
	mov r1, #0x68
	strh r3, [r2, #0x48]
	bl MBi_calc_cksum
	ldr r2, _0221E3DC ; =ov13_022474A0
	strh r0, [r2, #0x48]
	ldr r1, [r2, #0x1c]
	add r0, r1, #0x400
	add r1, r1, #0x300
	ldrh r3, [r0, #0xb0]
	ldrh r0, [r1, #0x5a]
	ldr r1, _0221E3EC ; =bsendBuff
	cmp r3, r0
	moveq r0, #1
	streqb r0, [r2, #0x24]
	ldr r0, [sp, #0x10]
	ldr r3, [sp, #8]
	orr r2, r0, #3
	ldr r0, [sp, #0xc]
	and r4, r2, #0xff
	str r0, [sp]
	mov r0, #0
	mov r2, #0x70
	str r4, [sp, #4]
	bl WM_SetGameInfo
	ldr r0, _0221E3DC ; =ov13_022474A0
	ldr r1, [r0, #0x10]
	cmp r1, #1
	addne sp, sp, #0x14
	ldmneia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [r0, #0x14]
	cmp r1, #0
	addeq sp, sp, #0x14
	ldmeqia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r0, #0x1c]
	ldr r0, [r0, #0x4b8]
	blx r1
	add sp, sp, #0x14
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0221E3DC: .word ov13_022474A0
_0221E3E0: .word bsendBuff + 0x68
_0221E3E4: .word bsendBuff + 0x10
_0221E3E8: .word bsendBuff + 0x8
_0221E3EC: .word bsendBuff
	arm_func_end MBi_SendVolatBeacon
