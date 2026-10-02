	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_init_ssp
	.extern _021D6F90
	.extern _021D6FB4
	.extern _021D6FCC
	.extern __vct

.text
	arm_func_start vct_init_ssp
vct_init_ssp: ; 0x020B3398
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r0, [r4]
	cmp r0, #0
	ldrne r1, [r4, #4]
	cmpne r1, #0
	beq _020B33BC
	cmp r1, #8
	bls _020B33C4
_020B33BC:
	mov r0, #0
	ldmia sp!, {r4, pc}
_020B33C4:
	ldr r1, _020B3490 ; =_021D6F90
	mov r2, #0x18
	str r0, [r1, #0x10]
	ldr r3, [r4, #4]
	mov r1, #0
	mul r2, r3, r2
	bl MI_CpuFill8
	ldr r0, _020B3494 ; =_021D6FCC
	mov r1, #0
	mov r2, #0x18
	bl MI_CpuFill8
	ldr r0, _020B3498 ; =_021D6FB4
	mov r1, #0
	mov r2, #0x18
	bl MI_CpuFill8
	ldr r3, [r4, #4]
	mov ip, #0
	subs r0, r3, #1
	beq _020B3444
	ldr r1, _020B3490 ; =_021D6F90
	mov lr, ip
	mov r0, #0x18
_020B341C:
	ldr r2, [r1, #0x10]
	add ip, ip, #1
	mla r3, ip, r0, r2
	add r2, r2, lr
	str r3, [r2, #0x14]
	ldr r3, [r4, #4]
	add lr, lr, #0x18
	sub r2, r3, #1
	cmp ip, r2
	blo _020B341C
_020B3444:
	ldr r1, _020B3490 ; =_021D6F90
	mov r0, #0x18
	ldr r2, [r1, #0x10]
	mov r4, #0
	mla r0, r3, r0, r2
	str r4, [r0, #-4]
	str r4, [r1, #0x14]
	str r4, [r1, #4]
	ldr r0, _020B349C ; =__vct
	str r4, [r1]
	ldr r3, _020B34A0 ; =OS_GetTick
	str r4, [r0, #0x14]
	ldr r2, _020B34A4 ; =0x5D588B65
	str r3, [r1, #0x18]
	ldr r0, _020B34A8 ; =0x00269EC3
	str r2, [r1, #0x1c]
	str r0, [r1, #0x20]
	mov r0, #1
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B3490: .word _021D6F90
_020B3494: .word _021D6FCC
_020B3498: .word _021D6FB4
_020B349C: .word __vct
_020B34A0: .word OS_GetTick
_020B34A4: .word 0x5D588B65
_020B34A8: .word 0x00269EC3
	arm_func_end vct_init_ssp
