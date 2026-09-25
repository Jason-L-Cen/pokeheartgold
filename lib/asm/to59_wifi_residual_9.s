	.include "asm/macros.inc"
	.include "wifi.inc"
	.include "global.inc"
	.public SOCL_Resolve
	.extern CPS_Resolve
	.extern CPS_SocRegister
	.extern CPS_SocUnRegister
	.extern _02110628

.text
	arm_func_start SOCL_Resolve
SOCL_Resolve: ; 0x020A2AE0
	stmdb sp!, {r4, r5, lr}
	sub sp, sp, #0x64
	movs r5, r0
	addeq sp, sp, #0x64
	moveq r0, #0
	ldmeqia sp!, {r4, r5, pc}
	ldr r1, _020A2B84 ; =_02110628
	mov r0, #0xfd0
	ldr r1, [r1]
	ldr r1, [r1, #0x18]
	blx r1
	movs r4, r0
	addeq sp, sp, #0x64
	moveq r0, #0
	ldmeqia sp!, {r4, r5, pc}
	add r0, sp, #0
	mov r1, #0
	mov r2, #0x64
	bl MI_CpuFill8
	add r0, r4, #0x368
	add r2, r0, #0x800
	ldr r3, _020A2B88 ; =0x00000B68
	ldr r1, _020A2B8C ; =0x00000466
	add r0, sp, #0
	str r4, [sp, #0x40]
	str r3, [sp, #0x3c]
	str r2, [sp, #0x4c]
	str r1, [sp, #0x48]
	bl CPS_SocRegister
	mov r0, r5
	bl CPS_Resolve
	mov r5, r0
	bl CPS_SocUnRegister
	ldr r1, _020A2B84 ; =_02110628
	mov r0, r4
	ldr r1, [r1]
	ldr r1, [r1, #0x1c]
	blx r1
	mov r0, r5
	add sp, sp, #0x64
	ldmia sp!, {r4, r5, pc}
	.align 2, 0
_020A2B84: .word _02110628
_020A2B88: .word 0x00000B68
_020A2B8C: .word 0x00000466
	arm_func_end SOCL_Resolve
