	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_02229BE4
	.extern _0222AB84
	.extern ov39_022298FC
	.extern ov39_02229910
	.extern ov39_02229FC0
	.extern ov39_0222AD84

.text
	arm_func_start ov39_02229BE4
ov39_02229BE4: ; 0x02229BE4
	stmdb sp!, {r4, r5, r6, r7, r8, sb, sl, lr}
	sub sp, sp, #8
	mov r2, #8
	mov r1, #2
	ldr r0, _02229D20 ; =ov39_0222AD84
	strb r2, [sp]
	strb r1, [sp, #1]
	ldrh r1, [r0, #0x40]
	ldr r0, _02229D24 ; =_0222AB84
	mov r2, r1, asr #8
	mov r1, r1, lsl #8
	and r2, r2, #0xff
	and r1, r1, #0xff00
	orr r1, r2, r1
	strh r1, [sp, #2]
	ldr r0, [r0, #0x264]
	str r0, [sp, #4]
	bl OS_GetTick
	ldr r2, _02229D28 ; =0x0007FD88
	mov r3, #0
	bl _ll_udiv
	ldr r5, _02229D28 ; =0x0007FD88
	ldr sb, _02229D24 ; =_0222AB84
	mov r7, r0
	mov r4, #0
	mov sl, #1
	add r6, sp, #0
	mvn r8, #0x19
_02229C54:
	ldr r0, [sb, #0x26c]
	mov r1, r6
	bl SOC_Connect
	cmp r0, #0
	bge _02229D14
	cmp r0, r8
	bne _02229CB4
	ldr r0, [sb, #0x244]
	cmp r0, #0
	beq _02229CD8
	bl OS_GetTick
	mov r2, r5
	mov r3, r4
	bl _ll_udiv
	ldr r1, [sb, #0x244]
	subs r0, r0, r7
	cmp r0, r1
	blt _02229CD8
	ldr r0, _02229D24 ; =_0222AB84
	mov r1, #0xa
	str r1, [r0, #0x250]
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02229CB4:
	mvn r1, #0x1d
	cmp r0, r1
	beq _02229D14
	ldr r0, _02229D24 ; =_0222AB84
	mov r1, #7
	str r1, [r0, #0x250]
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02229CD8:
	bl ov39_022298FC
	ldr r0, [sb, #0x268]
	cmp r0, #1
	bne _02229D04
	ldr r0, _02229D24 ; =_0222AB84
	mov r1, #0xd
	str r1, [r0, #0x250]
	bl ov39_02229910
	add sp, sp, #8
	mov r0, #0
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
_02229D04:
	bl ov39_02229910
	mov r0, sl
	bl ov39_02229FC0
	b _02229C54
_02229D14:
	mov r0, #1
	add sp, sp, #8
	ldmia sp!, {r4, r5, r6, r7, r8, sb, sl, pc}
	.balign 4, 0
_02229D20: .word ov39_0222AD84
_02229D24: .word _0222AB84
_02229D28: .word 0x0007FD88
	arm_func_end ov39_02229BE4
