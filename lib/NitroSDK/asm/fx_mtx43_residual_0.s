	.include "asm/macros.inc"
	.include "fx_mtx43.inc"
	.include "global.inc"
	.text

	arm_func_start MTX_Identity43_
MTX_Identity43_: ; 0x020CB6AC
	mov r2, #0x1000
	mov r3, #0
	stmia r0!, {r2, r3}
	mov r1, #0
	stmia r0!, {r1, r3}
	stmia r0!, {r2, r3}
	stmia r0!, {r1, r3}
	stmia r0!, {r2, r3}
	stmia r0!, {r1, r3}
	bx lr
	arm_func_end MTX_Identity43_

	arm_func_start MTX_Copy43To44_
MTX_Copy43To44_: ; 0x020CB6D4
	stmdb sp!, {r4}
	mov ip, #0
	ldmia r0!, {r2, r3, r4}
	stmia r1!, {r2, r3, r4, ip}
	ldmia r0!, {r2, r3, r4}
	stmia r1!, {r2, r3, r4, ip}
	ldmia r0!, {r2, r3, r4}
	stmia r1!, {r2, r3, r4, ip}
	mov ip, #0x1000
	ldmia r0!, {r2, r3, r4}
	stmia r1!, {r2, r3, r4, ip}
	ldmia sp!, {r4}
	bx lr
	arm_func_end MTX_Copy43To44_

	arm_func_start MTX_TransApply43
MTX_TransApply43: ; 0x020CB708
	stmdb sp!, {r3, r4, r5, r6, r7, lr}
	mov r7, r0
	mov r6, r1
	mov r5, r2
	mov r4, r3
	cmp r7, r6
	beq _020CB728
	bl MI_Copy36B
_020CB728:
	ldr r0, [r7, #0xc]
	ldr r1, [r7]
	smull r3, r2, r4, r0
	smlal r3, r2, r5, r1
	ldr r0, [sp, #0x18]
	ldr r1, [r7, #0x18]
	ldr ip, [r7, #0x24]
	smlal r3, r2, r0, r1
	mov r1, r3, lsr #0xc
	orr r1, r1, r2, lsl #20
	add r1, ip, r1
	str r1, [r6, #0x24]
	ldr r1, [r7, #0x10]
	ldr r2, [r7, #4]
	smull ip, r3, r4, r1
	smlal ip, r3, r5, r2
	ldr r1, [r7, #0x1c]
	ldr r2, [r7, #0x28]
	smlal ip, r3, r0, r1
	mov r1, ip, lsr #0xc
	orr r1, r1, r3, lsl #20
	add r1, r2, r1
	str r1, [r6, #0x28]
	ldr r1, [r7, #0x14]
	ldr r2, [r7, #8]
	smull ip, r3, r4, r1
	smlal ip, r3, r5, r2
	ldr r1, [r7, #0x20]
	ldr r2, [r7, #0x2c]
	smlal ip, r3, r0, r1
	mov r0, ip, lsr #0xc
	orr r0, r0, r3, lsl #20
	add r0, r2, r0
	str r0, [r6, #0x2c]
	ldmia sp!, {r3, r4, r5, r6, r7, pc}
	arm_func_end MTX_TransApply43

	thumb_func_start MTX_Scale43_
MTX_Scale43_: ; 0x020CB7B4
	stmia r0!, {r1}
	mov r1, #0
	str r3, [r0, #0x1c]
	mov r3, #0
	stmia r0!, {r1, r3}
	stmia r0!, {r1, r2, r3}
	mov r2, #0
	stmia r0!, {r1, r3}
	add r0, #4
	stmia r0!, {r1, r2, r3}
	bx lr
	.align 2, 0
	thumb_func_end MTX_Scale43_
