	.extern ov45_02236EC4
	.extern ov45_0223EA04
	.extern ov45_02242288
	.extern ov45_02242394
	.extern ov45_0225533C
	.include "asm/macros.inc"
	.include "overlay_45_arm.inc"
	.include "global.inc"

.text
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
