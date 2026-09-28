	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_021FE3D8
	.extern ov00_021FE1D8
	.extern ov00_021FE320
	.extern ov00_021FF368
	.extern ov00_021FF694
	.extern ov00_021FF818
	.extern ov00_021FF940
	.extern ov00_0221B0D0

.text
	arm_func_start ov00_021FE3D8
ov00_021FE3D8: ; 0x021FE3D8
	stmdb sp!, {r3, lr}
	sub sp, sp, #0x58
	ldr r2, _021FE510 ; =ov00_0221B0D0
	mov r3, #8
	ldr r0, [r2, #4]
	sub r1, r3, #9
	cmp r0, r1
	str r3, [sp, #0xc]
	moveq r0, #1
	streq r0, [r2]
	addeq sp, sp, #0x58
	ldmeqia sp!, {r3, pc}
	bl ov00_021FF694
	cmp r0, #0
	beq _021FE4B0
	add r0, sp, #0x10
	str r0, [sp]
	add r1, sp, #0xc
	str r1, [sp, #4]
	ldr r0, _021FE510 ; =ov00_0221B0D0
	add r1, sp, #0x18
	ldr r0, [r0, #4]
	mov r2, #0x40
	mov r3, #0
	bl ov00_021FF940
	mov r1, r0
	add r0, sp, #0x18
	add r2, sp, #0x10
	add r3, sp, #8
	bl ov00_021FE320
	cmp r0, #0
	bne _021FE4B0
	ldr r0, _021FE510 ; =ov00_0221B0D0
	ldr r0, [r0, #4]
	bl ov00_021FF818
	ldr r0, [sp, #8]
	tst r0, #1
	beq _021FE480
	ldr r0, _021FE510 ; =ov00_0221B0D0
	mov r1, #2
	str r1, [r0]
	b _021FE4A0
_021FE480:
	tst r0, #2
	ldreq r0, _021FE510 ; =ov00_0221B0D0
	moveq r1, #1
	streq r1, [r0]
	beq _021FE4A0
	ldr r0, _021FE510 ; =ov00_0221B0D0
	mov r1, #3
	str r1, [r0]
_021FE4A0:
	ldr r0, _021FE510 ; =ov00_0221B0D0
	add sp, sp, #0x58
	ldr r0, [r0]
	ldmia sp!, {r3, pc}
_021FE4B0:
	bl ov00_021FF368
	ldr r1, _021FE510 ; =ov00_0221B0D0
	ldr r2, [r1, #0x54]
	add r2, r2, #0x7d0
	cmp r0, r2
	bls _021FE504
	ldr r0, [r1, #0x58]
	cmp r0, #1
	bne _021FE4F0
	ldr r0, [r1, #4]
	bl ov00_021FF818
	ldr r1, _021FE510 ; =ov00_0221B0D0
	mov r0, #1
	str r0, [r1]
	add sp, sp, #0x58
	ldmia sp!, {r3, pc}
_021FE4F0:
	bl ov00_021FE1D8
	ldr r0, _021FE510 ; =ov00_0221B0D0
	ldr r1, [r0, #0x58]
	add r1, r1, #1
	str r1, [r0, #0x58]
_021FE504:
	mov r0, #0
	add sp, sp, #0x58
	ldmia sp!, {r3, pc}
	.balign 4, 0
_021FE510: .word ov00_0221B0D0
	arm_func_end ov00_021FE3D8
