	.include "asm/macros.inc"
#include <nitro/fs/overlay.h>
	.include "global.inc"

	.text

	arm_func_start strcpy
strcpy: ; 0x020E959C
	stmdb sp!, {r3, r4, r5, lr}
	and r4, r1, #3
	and r3, r0, #3
	mov r2, r0
	cmp r3, r4
	bne _020E9638
	cmp r4, #0
	beq _020E95F4
	ldrb r3, [r1]
	strb r3, [r0]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	rsbs r4, r4, #3
	beq _020E95EC
_020E95D4:
	ldrb r3, [r1, #1]!
	cmp r3, #0
	strb r3, [r2, #1]!
	ldmeqia sp!, {r3, r4, r5, pc}
	subs r4, r4, #1
	bne _020E95D4
_020E95EC:
	add r2, r2, #1
	add r1, r1, #1
_020E95F4:
	ldr r5, [r1]
	ldr r3, _020E965C ; =0xFEFEFEFF
	mvn r4, r5
	add lr, r5, r3
	ldr ip, _020E9660 ; =0x80808080
	and r4, lr, r4
	tst r4, ip
	bne _020E9638
	sub r2, r2, #4
_020E9618:
	str r5, [r2, #4]!
	ldr r5, [r1, #4]!
	add r4, r5, r3
	mvn lr, r5
	and lr, r4, lr
	tst lr, ip
	beq _020E9618
	add r2, r2, #4
_020E9638:
	ldrb r3, [r1]
	strb r3, [r2]
	cmp r3, #0
	ldmeqia sp!, {r3, r4, r5, pc}
_020E9648:
	ldrb r3, [r1, #1]!
	cmp r3, #0
	strb r3, [r2, #1]!
	bne _020E9648
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020E965C: .word 0xFEFEFEFF
_020E9660: .word 0x80808080
	arm_func_end strcpy
