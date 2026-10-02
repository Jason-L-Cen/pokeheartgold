	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_02229310
	.extern _0222AB84
	.extern ov39_02229408
	.extern ov39_0222993C
	.extern ov39_02229970
	.extern ov39_0222AD84

.text
	arm_func_start ov39_02229310
ov39_02229310: ; 0x02229310
	stmdb sp!, {r3, lr}
	bl ov39_02229408
	mov r0, #0
	ldr lr, _0222936C ; =_0222AB84
	mov r1, r0
	mov r2, r0
	mov r3, r0
	mov ip, #9
_02229330:
	stmia lr!, {r0, r1, r2, r3}
	stmia lr!, {r0, r1, r2, r3}
	subs ip, ip, #1
	bne _02229330
	stmia lr!, {r0, r1, r2, r3}
	stmia lr, {r0, r1}
	ldr r1, _0222936C ; =_0222AB84
	mov r2, #0
	str r2, [r1, #0x138]
	ldr r0, _02229370 ; =ov39_0222AD84
	strb r2, [r1, #0x140]
	strh r2, [r0, #0x40]
	bl ov39_0222993C
	bl ov39_02229970
	ldmia sp!, {r3, pc}
	.balign 4, 0
_0222936C: .word _0222AB84
_02229370: .word ov39_0222AD84
	arm_func_end ov39_02229310
