	.include "asm/macros.inc"
	.include "overlay_39_arm.inc"
	.include "global.inc"
	.public ov39_02229FCC
	.extern _0222AA58
	.extern _0222AB84
	.extern ov39_022298FC
	.extern ov39_02229910
	.extern ov39_02229FC0
	.extern ov39_0222AB8A

.text
	arm_func_start ov39_02229FCC
ov39_02229FCC: ; 0x02229FCC
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x17c
	ldr r0, _0222A104 ; =_0222AB84
	ldr r0, [r0, #0x138]
	cmp r0, #1
	addeq sp, sp, #0x17c
	moveq r0, #1
	ldmeqia sp!, {r4, r5, pc}
	ldr r0, _0222A108 ; =_0222AA58
	add r1, sp, #8
	bl ov00_021ECC6C
	cmp r0, #0
	bne _0222A018
	ldr r0, _0222A104 ; =_0222AB84
	mov r1, #0xe
	str r1, [r0, #0x250]
	add sp, sp, #0x17c
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0222A018:
	ldr r4, _0222A104 ; =_0222AB84
	mov r5, #1
_0222A020:
	bl ov00_021ECCE0
	cmp r0, #3
	bne _0222A074
	ldr r4, _0222A10C ; =ov39_0222AB8A
	add r3, sp, #0x4d
	mov r2, #0x96
_0222A038:
	ldrb r1, [r3]
	ldrb r0, [r3, #1]
	add r3, r3, #2
	strb r1, [r4]
	strb r0, [r4, #1]
	add r4, r4, #2
	subs r2, r2, #1
	bne _0222A038
	ldrb r2, [r3]
	ldr r1, _0222A104 ; =_0222AB84
	mov r0, #1
	strb r2, [r4]
	str r0, [r1, #0x138]
	add sp, sp, #0x17c
	ldmia sp!, {r4, r5, pc}
_0222A074:
	cmp r0, #4
	bne _0222A0A0
	add r0, sp, #0
	add r1, sp, #4
	bl ov00_021EC11C
	ldr r0, _0222A104 ; =_0222AB84
	mov r1, #0xe
	str r1, [r0, #0x250]
	add sp, sp, #0x17c
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0222A0A0:
	cmp r0, #5
	bne _0222A0C0
	ldr r0, _0222A104 ; =_0222AB84
	mov r1, #0xe
	str r1, [r0, #0x250]
	add sp, sp, #0x17c
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0222A0C0:
	bl ov39_022298FC
	ldr r0, [r4, #0x268]
	cmp r0, #1
	bne _0222A0EC
	ldr r0, _0222A104 ; =_0222AB84
	mov r1, #0xd
	str r1, [r0, #0x250]
	bl ov39_02229910
	add sp, sp, #0x17c
	mov r0, #0
	ldmia sp!, {r4, r5, pc}
_0222A0EC:
	bl ov39_02229910
	mov r0, r5
	bl ov39_02229FC0
	b _0222A020
	arm_func_end ov39_02229FCC

	arm_func_start ov39_0222A0FC
ov39_0222A0FC: ; 0x0222A0FC
	add sp, sp, #0x17c
	ldmia sp!, {r4, r5, pc}
	.balign 4, 0
_0222A104: .word _0222AB84
_0222A108: .word _0222AA58
_0222A10C: .word ov39_0222AB8A
	arm_func_end ov39_0222A0FC
