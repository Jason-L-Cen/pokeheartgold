	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0223CDBC

.text
	arm_func_start ov45_0223CDBC
ov45_0223CDBC: ; 0x0223CDBC
	stmdb sp!, {r3, lr}
	ldr ip, [r1]
	ldr r3, [r0]
	ldr lr, [r0, #8]
	sub r2, r3, ip
	mov r1, r2, asr #1
	add r1, r2, r1, lsr #30
	cmp r3, lr
	mov r2, r1, asr #2
	blo _0223CE08
	cmp ip, lr
	bhs _0223CE28
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	sub r2, r2, r0, asr #2
	b _0223CE28
_0223CE08:
	cmp ip, lr
	blo _0223CE28
	ldr r1, [r0, #0xc]
	ldr r0, [r0, #4]
	sub r1, r1, r0
	mov r0, r1, asr #1
	add r0, r1, r0, lsr #30
	add r2, r2, r0, asr #2
_0223CE28:
	mov r0, r2
	ldmia sp!, {r3, pc}
	arm_func_end ov45_0223CDBC
