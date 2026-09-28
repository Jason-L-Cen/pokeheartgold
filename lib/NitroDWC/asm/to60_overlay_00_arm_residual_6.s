	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_021FF984
	.extern ov00_021FF7EC

.text
	arm_func_start ov00_021FF984
ov00_021FF984: ; 0x021FF984
	stmdb sp!, {r3, r4, r5, r6, lr}
	sub sp, sp, #0xc
	ldr r5, [sp, #0x20]
	add r6, sp, #4
	mov r4, #4
_021FF998:
	ldrb lr, [r5]
	ldrb ip, [r5, #1]
	add r5, r5, #2
	subs r4, r4, #1
	strb lr, [r6]
	strb ip, [r6, #1]
	add r6, r6, #2
	bne _021FF998
	ldr lr, [sp, #0x24]
	add ip, sp, #4
	strb lr, [sp, #4]
	str ip, [sp]
	bl SOC_SendTo
	mvn r1, #0
	bl ov00_021FF7EC
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, r5, r6, pc}
	arm_func_end ov00_021FF984
