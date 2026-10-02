	.include "asm/macros.inc"
	.include "MSL_CPP_NITRO_Ai_LE.inc"
	.include "global.inc"
	.public _ZNSt3tr110shared_ptrIcED1Ev
	.extern _ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv

.text
	arm_func_start _ZNSt3tr110shared_ptrIcED1Ev
_ZNSt3tr110shared_ptrIcED1Ev: ; 0x020F04B0
	stmdb sp!, {r3, r4, fp, lr}
	sub sp, sp, #0x18
	mov fp, sp
	mov r4, r0
	ldr r0, [r4, #4]
	cmp r0, #0
	beq _020F04D0
	bl _ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv
_020F04D0:
	add sp, fp, #0x18
	mov r0, r4
	ldmia sp!, {r3, r4, fp, pc}
_020F04DC: ; 0x020F04DC
	add r0, fp, #0
	bl __unexpected
_020F04E4:
	b _020F04E4
_020F04E8: ; 0x020F04E8
	add sp, fp, #0x18
	ldmia sp!, {r3, r4, fp, pc}
	arm_func_end _ZNSt3tr110shared_ptrIcED1Ev
