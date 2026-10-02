	.extern _start_AutoloadDoneCallback
	.include "asm/macros.inc"
	.include "mb.inc"
	.include "global.inc"
	.public MBi_SendMP
	.extern MBi_CheckWmErrcode
	.extern MBi_ChildCallback
	.extern ov13_0224CF40

.text
	arm_func_start MBi_SetMPData
MBi_SetMPData: ; 0x0221FF28
	stmdb sp!, {r3, r4, lr}
	sub sp, sp, #0xc
	ldrh r4, [sp, #0x18]
	mov r3, r2
	mov r2, r1
	str r4, [sp]
	mov r4, #1
	str r4, [sp, #4]
	mov r4, #3
	mov r1, #0
	str r4, [sp, #8]
	bl WM_SetMPDataToPortEx
	mov r4, r0
	mov r1, r4
	mov r0, #0xf
	bl MBi_CheckWmErrcode
	mov r0, r4
	add sp, sp, #0xc
	ldmia sp!, {r3, r4, pc}
	arm_func_end MBi_SetMPData

	arm_func_start MBi_SendMP
MBi_SendMP: ; 0x0221FF74
	stmdb sp!, {r3, r4, r5, lr}
	ldr ip, _0222004C ; =ov13_0224CF40
	mov r3, r1, lsl #0x10
	ldr r5, [ip, #8]
	mov ip, r2, lsl #0x10
	add lr, r5, #0x500
	ldrh r4, [lr, #0x28]
	mov r1, r0
	mov r2, r3, lsr #0x10
	cmp r4, #0
	ldrneh r0, [lr, #0x26]
	mov r4, ip, lsr #0x10
	cmpne r0, #1
	moveq r0, #1
	ldmeqia sp!, {r3, r4, r5, pc}
	ldrh r0, [lr, #0x24]
	cmp r0, #1
	beq _0221FFC8
	cmp r0, #2
	beq _02220010
	b _02220044
_0221FFC8:
	ldrh r0, [lr, #0x2c]
	cmp r0, #0
	moveq r0, #0x3e8
	movne r0, #0
	mov r3, r0, lsl #0x10
	str r4, [sp]
	ldr r0, [r5, #0x508]
	mov r3, r3, lsr #0x10
	bl MBi_SetMPData
	cmp r0, #2
	bne _02220004
	ldr r1, _0222004C ; =ov13_0224CF40
	mov r2, #1
	ldr r1, [r1, #8]
	strb r2, [r1, #0x50c]
_02220004:
	cmp r0, #2
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_02220010:
	ldr r0, _02220050 ; =MBi_ChildCallback
	mov r3, #0
	str r4, [sp]
	bl MBi_SetMPData
	cmp r0, #2
	bne _02220038
	ldr r1, _0222004C ; =ov13_0224CF40
	mov r2, #1
	ldr r1, [r1, #8]
	strb r2, [r1, #0x50c]
_02220038:
	cmp r0, #2
	moveq r0, #0
	ldmia sp!, {r3, r4, r5, pc}
_02220044:
	mov r0, #1
	ldmia sp!, {r3, r4, r5, pc}
	.balign 4, 0
_0222004C: .word ov13_0224CF40
_02220050: .word MBi_ChildCallback
	arm_func_end MBi_SendMP
