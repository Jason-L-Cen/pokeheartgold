	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"
	.public ov45_0224210C
	.public ov45_022421E4
	.public ov45_02242254
	.extern ov45_02236EC4
	.extern ov45_0223EA04
	.extern ov45_02242288
	.extern ov45_02242394
	.extern ov45_0225533C

.text
	arm_func_start ov45_0224210C
ov45_0224210C: ; 0x0224210C
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x14
	mov r8, r0
	ldr r4, [r8]
	mvn r0, #0
	mov r7, r1
	mov r6, r2
	mov r5, r3
	cmp r4, r0
	bne _02242140
	ldr r0, _022421E0 ; =ov45_0225533C
	bl printf
	bl abort
_02242140:
	mov r1, #0
	mov r0, #0x10
	strb r1, [sp]
	bl ov45_02242394
	mov r4, r0
	add r0, r8, #4
	str r0, [sp, #0xc]
	adds r1, r4, #0xc
	ldrne r0, [sp, #0x30]
	str r4, [sp, #0x10]
	ldrne r0, [r0]
	str r4, [sp, #4]
	strne r0, [r1]
	mov r0, #0
	str r0, [r4, #4]
	str r0, [sp, #0x10]
	str r0, [r4]
	adds r0, r4, #8
	strne r7, [r0]
	cmp r6, #0
	strne r4, [r7]
	streq r4, [r7, #4]
	ldr r1, [r8]
	mov r0, r4
	add r1, r1, #1
	str r1, [r8]
	ldr r1, [r8, #4]
	str r4, [sp, #8]
	bl ov45_02236EC4
	cmp r5, #0
	ldr r1, [sp, #0x10]
	strne r4, [r8, #8]
	cmp r1, #0
	beq _022421D4
	mov r0, #0
	mov r2, r0
	bl ov00_021EC2EC
_022421D4:
	mov r0, r4
	add sp, sp, #0x14
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_022421E0: .word ov45_0225533C
	arm_func_end ov45_0224210C

	arm_func_start ov45_022421E4
ov45_022421E4: ; 0x022421E4
	stmdb sp!, {r3, r4, r5, lr}
	mov r5, r0
	ldr r3, [r5, #4]
	mov r0, #0
	strb r0, [sp, #2]
	ldr r1, [r5, #4]
	mov r4, r2
	sub r1, r1, r3
	str r1, [r5, #4]
	ldr r1, [r5]
	cmp r1, #0
	beq _0224222C
	beq _02242220
	mov r2, r0
	bl ov00_021EC2EC
_02242220:
	mov r0, #0
	str r0, [r5]
	str r0, [r5, #8]
_0224222C:
	mov r3, #0
	strb r3, [sp]
	sub r0, sp, #4
	strb r3, [r0]
	ldr r2, [r0]
	mov r0, r5
	mov r1, r4
	strb r3, [sp, #1]
	bl ov45_0223EA04
	ldmia sp!, {r3, r4, r5, pc}
	arm_func_end ov45_022421E4

	arm_func_start ov45_02242254
ov45_02242254: ; 0x02242254
	stmdb sp!, {r3, lr}
	ldr r3, [r1]
	mov r3, r3, lsl #0x1f
	movs r3, r3, lsr #0x1f
	ldmneib r1, {r3, ip}
	bne _0224227C
	ldrb r3, [r1]
	add ip, r1, #1
	mov r1, r3, lsl #0x18
	mov r3, r1, lsr #0x19
_0224227C:
	mov r1, ip
	bl ov45_02242288
	ldmia sp!, {r3, pc}
	arm_func_end ov45_02242254
