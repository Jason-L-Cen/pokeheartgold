	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220AD00
	.extern ov00_0221B1F0

.text
	arm_func_start ov00_0220AD00
ov00_0220AD00: ; 0x0220AD00
	ldr r1, _0220AD10 ; =ov00_0221B1F0
	str r0, [r1]
	ldr r0, _0220AD14 ; =ov00_0221B1F0
	bx lr
	.balign 4, 0
_0220AD10: .word ov00_0221B1F0
_0220AD14: .word ov00_0221B1F0
	arm_func_end ov00_0220AD00
