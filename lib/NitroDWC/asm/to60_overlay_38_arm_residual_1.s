	.include "asm/macros.inc"
	.include "overlay_38_arm.inc"
	.include "global.inc"
	.public ghiPostStringStateDoPosting
	.extern _02224E60
	.extern ghiAppendCharToBuffer
	.extern ghiAppendDataToBuffer
	.extern ov38_0221CD90
	.extern ov38_022255A4
	.extern ov38_02225644
	.extern ov38_02225650
	.extern ov38_022257B8
	.extern ov38_022257C8
	.extern ov38_022257F4
	.extern ov38_02225804

.text
	arm_func_start ghiPostStringStateDoPosting
ghiPostStringStateDoPosting: ; 0x0221E154
	stmdb sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, lr}
	mov r4, r0
	ldr r0, [r4, #4]
	mov sl, r1
	cmp r0, #0
	bge _0221E180
	ldr r0, _0221E30C ; =ov38_022257B8
	ldr r1, _0221E310 ; =ov38_02225644
	ldr r2, _0221E314 ; =ov38_022255A4
	ldr r3, _0221E318 ; =0x000002E7
	bl __msl_assertion_failed
_0221E180:
	ldr r0, [r4]
	ldr r1, [r0, #0xc]
	cmp r1, #0
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r0, [r4, #4]
	cmp r0, r1
	blt _0221E1B4
	ldr r0, _0221E31C ; =ov38_022257C8
	ldr r1, _0221E310 ; =ov38_02225644
	ldr r2, _0221E314 ; =ov38_022255A4
	ldr r3, _0221E320 ; =0x000002EE
	bl __msl_assertion_failed
_0221E1B4:
	ldr r0, [sl, #0x13c]
	ldr r0, [r0, #0xc]
	cmp r0, #0
	bne _0221E2C0
	ldr r1, [r4]
	ldr r0, [r1, #0x10]
	cmp r0, #0
	beq _0221E2C0
	ldr r0, _0221E324 ; =_02224E60
	ldr r8, [r1, #8]
	ldrb r2, [r0]
	ldrb r1, [r0, #1]
	add r5, sp, #0
	strb r2, [sp]
	strb r1, [sp, #1]
	ldrb r1, [r0, #2]
	ldrb r0, [r0, #3]
	mov r6, #0
	strb r1, [sp, #2]
	strb r0, [sp, #3]
	ldrsb r7, [r8]
	cmp r7, #0
	beq _0221E2B8
	ldr r4, _0221E328 ; =ov38_02225804
	ldr fp, _0221E32C ; =ov38_02225650
_0221E218:
	mov r0, fp
	mov r1, r7
	bl strchr
	cmp r0, #0
	beq _0221E23C
	mov r1, r7
	add r0, sl, #0x50
	bl ghiAppendCharToBuffer
	b _0221E2A8
_0221E23C:
	cmp r7, #0x20
	bne _0221E254
	mov r1, #0x2b
	add r0, sl, #0x50
	bl ghiAppendCharToBuffer
	b _0221E2A8
_0221E254:
	mov r0, r7, asr #3
	add r0, r7, r0, lsr #28
	mov sb, r0, asr #4
	cmp sb, #0x10
	blt _0221E27C
	ldr r0, _0221E330 ; =ov38_022257F4
	ldr r1, _0221E310 ; =ov38_02225644
	ldr r2, _0221E314 ; =ov38_022255A4
	ldr r3, _0221E334 ; =0x0000030D
	bl __msl_assertion_failed
_0221E27C:
	ldrsb r2, [r4, sb]
	mov r1, r7, lsr #0x1f
	rsb r0, r1, r7, lsl #28
	add r0, r1, r0, ror #28
	strb r2, [sp, #1]
	ldrsb r2, [r4, r0]
	mov r1, r5
	add r0, sl, #0x50
	strb r2, [sp, #2]
	mov r2, #3
	bl ghiAppendDataToBuffer
_0221E2A8:
	add r6, r6, #1
	ldrsb r7, [r8, r6]
	cmp r7, #0
	bne _0221E218
_0221E2B8:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
_0221E2C0:
	ldr r1, [r4]
	ldr r0, [r4, #4]
	ldr r2, [r1, #0xc]
	ldr r1, [r1, #8]
	sub r5, r2, r0
	mov r0, sl
	mov r2, r5
	bl ov38_0221CD90
	mvn r1, #0
	cmp r0, r1
	moveq r0, #0
	ldmeqia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	ldr r1, [r4, #4]
	cmp r0, r5
	add r0, r1, r0
	str r0, [r4, #4]
	moveq r0, #1
	movne r0, #2
	ldmia sp!, {r3, r4, r5, r6, r7, r8, sb, sl, fp, pc}
	.balign 4, 0
_0221E30C: .word ov38_022257B8
_0221E310: .word ov38_02225644
_0221E314: .word ov38_022255A4
_0221E318: .word 0x000002E7
_0221E31C: .word ov38_022257C8
_0221E320: .word 0x000002EE
_0221E324: .word _02224E60
_0221E328: .word ov38_02225804
_0221E32C: .word ov38_02225650
_0221E330: .word ov38_022257F4
_0221E334: .word 0x0000030D
	arm_func_end ghiPostStringStateDoPosting
