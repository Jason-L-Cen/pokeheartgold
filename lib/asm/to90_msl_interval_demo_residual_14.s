	.include "asm/macros.inc"
	.include "msl.inc"
	.include "global.inc"
	.public __unexpected
	.extern _021116E8
	.extern _021116F4
	.extern _0211170C
	.extern _Z17IsInSpecificationPcP16ex_specification
	.extern _Z20__DecodeSignedNumberPcPl
	.extern _Z22__DecodeUnsignedNumberPcPm
	.extern _ZNSt13bad_exceptionD1Ev
	.extern _ZSt10unexpectedv
	.extern _ZSt9terminatev
	.extern __end__catch
	.extern __rethrow
	.extern __throw
	.type __unexpected, @function

.text

	arm_func_start __unexpected
__unexpected: ; 0x020F445C
	stmdb sp!, {r4, fp, lr}
	sub sp, sp, #0x2c
	mov fp, sp
	str sp, [fp, #0x18]
	ldr r4, [r0, #0x14]
	bl _ZSt10unexpectedv
	b _020F44E8
_020F4478:
	add r1, fp, #0x1c
	add r0, r4, #1
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, fp, #0x20
	bl _Z22__DecodeUnsignedNumberPcPm
	add r1, fp, #0x24
	bl _Z20__DecodeSignedNumberPcPl
	str r0, [fp, #0x28]
	ldr r0, [fp, #8]
	add r1, fp, #0x1c
	bl _Z17IsInSpecificationPcP16ex_specification
	cmp r0, #0
	beq _020F44B0
	bl __rethrow
_020F44B0:
	ldr r0, _020F44F4 ; =_021116F4
	add r1, fp, #0x1c
	bl _Z17IsInSpecificationPcP16ex_specification
	cmp r0, #0
	beq _020F44DC
	ldr r3, _020F44F8 ; =_021116E8
	ldr r0, _020F44FC ; =_0211170C
	ldr r2, _020F4500 ; =_ZNSt13bad_exceptionD1Ev
	add r1, fp, #0
	str r3, [fp]
	bl __throw
_020F44DC:
	add r0, fp, #4
	bl __end__catch
	ldr sp, [fp, #0x18]
_020F44E8:
	bl _ZSt9terminatev
	add sp, fp, #0x2c
	ldmia sp!, {r4, fp, pc}
	.balign 4, 0
_020F44F4: .word _021116F4
_020F44F8: .word _021116E8
_020F44FC: .word _0211170C
_020F4500: .word _ZNSt13bad_exceptionD1Ev
	arm_func_end __unexpected
