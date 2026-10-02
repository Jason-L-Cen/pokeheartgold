	.include "asm/macros.inc"
	.include "MSL_CPP_NITRO_Ai_LE.inc"
	.include "global.inc"
	.public _ZNSt15__vector_commonILb1EE18throw_length_errorEv
	.extern _02111644
	.extern _02111658
	.extern _02111664
	.extern _02111678
	.extern _ZNSt12length_errorD1Ev
	.extern _ZNSt3tr110shared_ptrIcEC1IcNSt6detail14default_deleteIA0_cEEEEPT_T0_

.text
	arm_func_start _ZNSt15__vector_commonILb1EE18throw_length_errorEv
_ZNSt15__vector_commonILb1EE18throw_length_errorEv: ; 0x020F05FC
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	ldr r4, _020F0678 ; =_02111664
	ldr r1, _020F067C ; =_02111658
	add r6, sp, #4
	mov r0, r4
	str r1, [sp, #4]
	add r5, sp, #8
	bl strlen
	add r0, r0, #1
	rsb r0, r0, r0, lsl #1
	bl _Znam
	mov r1, #0
	strb r1, [sp]
	sub r2, sp, #4
	strb r1, [r2]
	mov r1, r0
	ldr r2, [r2]
	mov r0, r5
	bl _ZNSt3tr110shared_ptrIcEC1IcNSt6detail14default_deleteIA0_cEEEEPT_T0_
	ldr r0, [r5]
	mov r1, r4
	bl strcpy
	ldr r3, _020F0680 ; =_02111644
	ldr r0, _020F0684 ; =_02111678
	ldr r2, _020F0688 ; =_ZNSt12length_errorD1Ev
	mov r1, r6
	str r3, [sp, #4]
	bl __throw
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020F0678: .word _02111664
_020F067C: .word _02111658
_020F0680: .word _02111644
_020F0684: .word _02111678
_020F0688: .word _ZNSt12length_errorD1Ev
	arm_func_end _ZNSt15__vector_commonILb1EE18throw_length_errorEv
