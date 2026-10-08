	.extern DC_InvalidateRange
	.extern SNDi_SharedWork
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"

.text
	arm_func_start SND_ReadChannelInfo
SND_ReadChannelInfo: ; 0x020D5DA4
	stmdb sp!, {r3, lr}
	cmp r1, #0
	blt _020D5DB8
	cmp r1, #0xf
	ble _020D5DC0
_020D5DB8:
	mov r0, #0
	ldmia sp!, {r3, pc}
_020D5DC0:
	add r3, r0, #0x1000
	ldr r3, [r3, #0x1c4]
	mov lr, #1
	ldr ip, [r2]
	tst r3, lr, lsl r1
	moveq lr, #0
	mov r3, lr, lsl #0x1f
	bic ip, ip, #2
	orr ip, ip, r3, lsr #30
	mov r3, ip, lsl #0x1e
	str ip, [r2]
	movs r3, r3, asr #0x1f
	beq _020D5E94
	add r0, r0, r1, lsl #2
	add r0, r0, #0x1000
	ldr r0, [r0, #0x180]
	ldr r3, [r2]
	tst r0, #0x80000000
	movne lr, #1
	moveq lr, #0
	bic ip, r3, #1
	and r3, lr, #1
	orr r3, ip, r3
	str r3, [r2]
	and r1, r0, #0x7f
	strh r1, [r2, #4]
	ldrh r1, [r2, #4]
	and r3, r0, #0x300
	mov r3, r3, lsr #8
	mov r1, r1, lsl #4
	strh r1, [r2, #4]
	cmp r3, #3
	addls pc, pc, r3, lsl #2
	b _020D5E84
_020D5E48: ; jump table
	b _020D5E84 ; case 0
	b _020D5E58 ; case 1
	b _020D5E68 ; case 2
	b _020D5E78 ; case 3
_020D5E58:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #1
	strh r1, [r2, #4]
	b _020D5E84
_020D5E68:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #2
	strh r1, [r2, #4]
	b _020D5E84
_020D5E78:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #4
	strh r1, [r2, #4]
_020D5E84:
	and r0, r0, #0x7f0000
	mov r0, r0, lsr #0x10
	strb r0, [r2, #6]
	b _020D5F2C
_020D5E94:
	mov r3, #0x54
	mla r0, r1, r3, r0
	ldrb r1, [r0, #3]
	bic r3, ip, #1
	mov r1, r1, lsl #0x1f
	mov r1, r1, lsr #0x1f
	and r1, r1, #1
	orr r1, r3, r1
	str r1, [r2]
	ldrb r1, [r0, #2]
	str r1, [r2, #8]
	ldrh r3, [r0, #0x24]
	and r1, r3, #0xff
	strh r1, [r2, #4]
	ldrh r1, [r2, #4]
	mov r3, r3, asr #8
	cmp r3, #3
	mov r1, r1, lsl #4
	strh r1, [r2, #4]
	addls pc, pc, r3, lsl #2
	b _020D5F24
_020D5EE8: ; jump table
	b _020D5F24 ; case 0
	b _020D5EF8 ; case 1
	b _020D5F08 ; case 2
	b _020D5F18 ; case 3
_020D5EF8:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #1
	strh r1, [r2, #4]
	b _020D5F24
_020D5F08:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #2
	strh r1, [r2, #4]
	b _020D5F24
_020D5F18:
	ldrh r1, [r2, #4]
	mov r1, r1, asr #4
	strh r1, [r2, #4]
_020D5F24:
	ldrb r0, [r0, #0x23]
	strb r0, [r2, #6]
_020D5F2C:
	mov r0, #1
	ldmia sp!, {r3, pc}
	arm_func_end SND_ReadChannelInfo
