	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public GetRegionOfMBlock

.text
	arm_func_start GetRegionOfMBlock
GetRegionOfMBlock: ; 0x020B4E64
	ldrh r2, [r1, #2]
	add r3, r1, #0x10
	mov r2, r2, asr #8
	and r2, r2, #0x7f
	mov r2, r2, lsl #0x10
	sub r2, r1, r2, lsr #16
	str r2, [r0]
	ldr r1, [r1, #4]
	add r1, r1, r3
	str r1, [r0, #4]
	bx lr
	arm_func_end GetRegionOfMBlock
