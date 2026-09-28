	.include "asm/macros.inc"
	.include "overlay_00_arm.inc"
	.include "global.inc"
	.public ov00_0220A69C
	.extern ov00_02202990
	.extern ov00_0220A86C
	.extern ov00_02219754
	.extern ov00_02219778
	.extern ov00_022197C8
	.extern ov00_022197D4
	.extern ov00_02219868
	.extern ov00_02219878

.text
	arm_func_start ov00_0220A69C
ov00_0220A69C: ; 0x0220A69C
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	movs r7, r1
	mov r8, r0
	mov r6, r2
	mov r5, r3
	ldr r4, [sp, #0x18]
	bne _0220A6CC
	ldr r0, _0220A844 ; =ov00_02219868
	ldr r1, _0220A848 ; =ov00_02219778
	ldr r2, _0220A84C ; =ov00_02219754
	ldr r3, _0220A850 ; =0x00000199
	bl __msl_assertion_failed
_0220A6CC:
	cmp r5, #0
	bne _0220A6E8
	ldr r0, _0220A854 ; =ov00_022197C8
	ldr r1, _0220A848 ; =ov00_02219778
	ldr r2, _0220A84C ; =ov00_02219754
	ldr r3, _0220A858 ; =0x0000019A
	bl __msl_assertion_failed
_0220A6E8:
	cmp r4, #0
	bne _0220A704
	ldr r0, _0220A85C ; =ov00_022197D4
	ldr r1, _0220A848 ; =ov00_02219778
	ldr r2, _0220A84C ; =ov00_02219754
	ldr r3, _0220A860 ; =0x0000019B
	bl __msl_assertion_failed
_0220A704:
	ldr r1, [r6]
	ldrsb r0, [r7, r1]
	add r3, r7, r1
	cmp r0, #0x5c
	beq _0220A740
	ldr r2, _0220A864 ; =ov00_02219878
	mov r0, r8
	mov r1, #1
	bl ov00_0220A86C
	mov r0, r8
	mov r1, #3
	mov r2, #1
	bl ov00_02202990
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0220A740:
	ldrsb r1, [r3, #1]
	add r7, r3, #2
	mov r2, #0
	cmp r1, #0x5c
	beq _0220A7CC
	ldr r0, _0220A868 ; =0x000001FF
_0220A758:
	cmp r1, #0
	bne _0220A788
	ldr r2, _0220A864 ; =ov00_02219878
	mov r0, r8
	mov r1, #1
	bl ov00_0220A86C
	mov r0, r8
	mov r1, #3
	mov r2, #1
	bl ov00_02202990
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0220A788:
	cmp r2, r0
	bne _0220A7B8
	ldr r2, _0220A864 ; =ov00_02219878
	mov r0, r8
	mov r1, #1
	bl ov00_0220A86C
	mov r0, r8
	mov r1, #3
	mov r2, #1
	bl ov00_02202990
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0220A7B8:
	strb r1, [r5], #1
	ldrsb r1, [r7], #1
	add r2, r2, #1
	cmp r1, #0x5c
	bne _0220A758
_0220A7CC:
	mov r1, #0
	strb r1, [r5]
	ldr r0, _0220A868 ; =0x000001FF
	b _0220A814
_0220A7DC:
	cmp r1, r0
	bne _0220A80C
	ldr r2, _0220A864 ; =ov00_02219878
	mov r0, r8
	mov r1, #1
	bl ov00_0220A86C
	mov r0, r8
	mov r1, #3
	mov r2, #1
	bl ov00_02202990
	mov r0, #3
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
_0220A80C:
	strb r2, [r4], #1
	add r1, r1, #1
_0220A814:
	ldrsb r2, [r7], #1
	cmp r2, #0x5c
	cmpne r2, #0
	bne _0220A7DC
	mov r0, #0
	strb r0, [r4]
	sub r1, r7, r3
	ldr r2, [r6]
	sub r1, r1, #1
	add r1, r2, r1
	str r1, [r6]
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.balign 4, 0
_0220A844: .word ov00_02219868
_0220A848: .word ov00_02219778
_0220A84C: .word ov00_02219754
_0220A850: .word 0x00000199
_0220A854: .word ov00_022197C8
_0220A858: .word 0x0000019A
_0220A85C: .word ov00_022197D4
_0220A860: .word 0x0000019B
_0220A864: .word ov00_02219878
_0220A868: .word 0x000001FF
	arm_func_end ov00_0220A69C
