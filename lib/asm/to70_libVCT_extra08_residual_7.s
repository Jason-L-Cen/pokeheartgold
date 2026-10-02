	.include "asm/macros.inc"
	.include "libVCT.inc"
	.include "global.inc"
	.public vct_init_audio_queue
	.extern _021D6F6C
	.extern _021D6F78
	.extern _021D6F84

.text
	arm_func_start vct_init_audio_queue
vct_init_audio_queue: ; 0x020B27E0
	stmdb sp!, {r4, lr}
	mov r4, r0
	bl OS_DisableInterrupts
	ldr r3, _020B2810 ; =_021D6F84
	mov ip, #0
	ldr r1, _020B2814 ; =_021D6F78
	ldr r2, _020B2818 ; =_021D6F6C
	str ip, [r3, r4, lsl #2]
	str ip, [r2, r4, lsl #2]
	str ip, [r1, r4, lsl #2]
	bl OS_RestoreInterrupts
	ldmia sp!, {r4, pc}
	.align 2, 0
_020B2810: .word _021D6F84
_020B2814: .word _021D6F78
_020B2818: .word _021D6F6C
	arm_func_end vct_init_audio_queue
