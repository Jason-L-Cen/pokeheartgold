	.include "asm/macros.inc"
	.include "MSL_CPP_NITRO_Ai_LE.inc"
	.include "global.inc"
	.public _ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv
	.extern _ZNSt3tr16detail25shared_ptr_deleter_common12release_weakEv

.text
	arm_func_start _ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv
_ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv: ; 0x020F05A0
	stmdb sp!, {r4, lr}
	mov r4, r0
	ldr r1, [r4, #4]
	subs r1, r1, #1
	str r1, [r4, #4]
	ldmneia sp!, {r4, pc}
	ldr r1, [r0]
	ldr r1, [r1, #0xc]
	blx r1
	mov r0, r4
	bl _ZNSt3tr16detail25shared_ptr_deleter_common12release_weakEv
	ldmia sp!, {r4, pc}
	arm_func_end _ZNSt3tr16detail25shared_ptr_deleter_common7releaseEv
