	.include "asm/macros.inc"
	.include "overlay_13_arm_2.inc"
	.include "global.inc"
	.public ov13_0222BCC0
	.extern ov13_0223EAD4
	.extern ov13_0223EE58
	.extern ov13_0223EE9C
	.extern ov13_022412B0
	.extern ov13_022425D0
	.extern ov13_0224F4F0

.text
	arm_func_start ov13_0222BCC0
ov13_0222BCC0: ; 0x0222BCC0
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #4
	mov r4, r0
	bl ov13_0223EAD4
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldrb r1, [r0, #0x2a0]
	cmp r1, #0
	beq _0222BD14
	ldrb r1, [r0, #0x2a1]
	cmp r1, #0
	bne _0222BD14
	ldr r1, [r0, #0x298]
	cmp r1, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #0
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BD14:
	add r0, sp, #1
	add r1, sp, #0
	bl ov13_0223EE58
	ldrb r0, [sp, #1]
	cmp r0, #0x1a
	bgt _0222BDA8
	bge _0222BEB4
	cmp r0, #0x14
	bgt _0222BD98
	cmp r0, #0
	addge pc, pc, r0, lsl #2
	b _0222BF50
_0222BD44: ; jump table
	b _0222BF50 ; case 0
	b _0222BF50 ; case 1
	b _0222BF50 ; case 2
	b _0222BF50 ; case 3
	b _0222BF50 ; case 4
	b _0222BDCC ; case 5
	b _0222BF50 ; case 6
	b _0222BF50 ; case 7
	b _0222BF50 ; case 8
	b _0222BF50 ; case 9
	b _0222BF50 ; case 10
	b _0222BF50 ; case 11
	b _0222BEF8 ; case 12
	b _0222BE2C ; case 13
	b _0222BF50 ; case 14
	b _0222BF50 ; case 15
	b _0222BF50 ; case 16
	b _0222BF50 ; case 17
	b _0222BF50 ; case 18
	b _0222BF50 ; case 19
	b _0222BE70 ; case 20
_0222BD98:
	cmp r0, #0x17
	beq _0222BE70
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BDA8:
	cmp r0, #0x1d
	bgt _0222BDBC
	beq _0222BEB4
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BDBC:
	cmp r0, #0x22
	beq _0222BF3C
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BDCC:
	ldrb r0, [sp]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r4, r0, #0x2280
	bl ov13_0223EE9C
	add r1, r4, #0x1c000
	mov r2, #0x16
	bl MIi_CpuCopy16
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldr r1, [r0, #0x298]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0x2a0]
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #0
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BE2C:
	ldrb r0, [sp]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldr r1, [r0, #0x298]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0x2a0]
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #1
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BE70:
	ldrb r0, [sp]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldr r1, [r0, #0x298]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0x2a0]
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #3
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BEB4:
	ldrb r0, [sp]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldr r1, [r0, #0x298]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0x2a0]
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #4
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BEF8:
	ldrb r0, [sp]
	cmp r0, #0
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	ldr r0, _0222BF58 ; =ov13_0224F4F0
	ldr r0, [r0]
	add r0, r0, #0x1e000
	ldr r1, [r0, #0x298]
	cmp r1, #0
	moveq r1, #1
	streqb r1, [r0, #0x2a0]
	addeq sp, sp, #4
	ldmeqia sp!, {r3, r4, pc}
	mov r0, #2
	blx r1
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
_0222BF3C:
	mov r1, r4
	mov r0, #0
	bl ov13_022425D0
	ldr r0, _0222BF5C ; =ov13_0224F4F0
	bl ov13_022412B0
_0222BF50:
	add sp, sp, #4
	ldmia sp!, {r3, r4, pc}
	.balign 4, 0
_0222BF58: .word ov13_0224F4F0
_0222BF5C: .word ov13_0224F4F0
	arm_func_end ov13_0222BCC0
