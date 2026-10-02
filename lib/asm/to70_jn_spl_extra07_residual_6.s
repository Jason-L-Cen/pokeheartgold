	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public spl_set_cross_to_axis
	.extern _021105EC

.text
	arm_func_start spl_set_cross_to_axis
spl_set_cross_to_axis: ; 0x0209D35C
	stmdb sp!, {r4, r5, r6, lr}
	sub sp, sp, #0x10
	ldr r1, _0209D5E0 ; =_021105EC
	mov r4, r0
	ldrh r3, [r1]
	ldrh r2, [r1, #2]
	ldrh r0, [r1, #4]
	strh r3, [sp]
	strh r2, [sp, #2]
	strh r0, [sp, #4]
	ldr r0, [r4, #0x20]
	ldr r0, [r0]
	ldr r0, [r0]
	mov r0, r0, lsl #0x18
	movs r0, r0, lsr #0x1e
	beq _0209D3DC
	cmp r0, #1
	beq _0209D3C4
	cmp r0, #2
	bne _0209D3F4
	mov r0, #0
	mov r1, #0x1000
	strh r1, [sp, #6]
	strh r0, [sp, #8]
	strh r0, [sp, #0xa]
	b _0209D400
_0209D3C4:
	mov r1, #0
	mov r0, #0x1000
	strh r1, [sp, #6]
	strh r0, [sp, #8]
	strh r1, [sp, #0xa]
	b _0209D400
_0209D3DC:
	mov r1, #0
	mov r0, #0x1000
	strh r1, [sp, #6]
	strh r1, [sp, #8]
	strh r0, [sp, #0xa]
	b _0209D400
_0209D3F4:
	add r1, sp, #6
	add r0, r4, #0x50
	bl VEC_Fx16Normalize
_0209D400:
	add r0, sp, #0
	add r1, sp, #6
	bl VEC_Fx16DotProduct
	cmp r0, #0x1000
	beq _0209D424
	mov r1, #0x1000
	rsb r1, r1, #0
	cmp r0, r1
	bne _0209D438
_0209D424:
	mov r0, #0
	mov r1, #0x1000
	strh r1, [sp]
	strh r0, [sp, #2]
	strh r0, [sp, #4]
_0209D438:
	ldrsh r3, [sp, #8]
	ldrsh r1, [sp, #4]
	ldrsh r2, [sp, #0xa]
	ldrsh r0, [sp, #2]
	smull r6, r5, r3, r1
	mov r1, #0x800
	adds r6, r6, r1
	smull r3, r0, r2, r0
	adc r5, r5, #0
	adds r2, r3, r1
	mov r3, r6, lsr #0xc
	adc r0, r0, #0
	mov r2, r2, lsr #0xc
	orr r3, r3, r5, lsl #20
	orr r2, r2, r0, lsl #20
	sub r0, r3, r2
	strh r0, [r4, #0x84]
	ldrsh r5, [sp, #0xa]
	ldrsh r3, [sp]
	ldrsh r2, [sp, #6]
	ldrsh r0, [sp, #4]
	smull r3, r6, r5, r3
	adds ip, r3, r1
	smull r3, r0, r2, r0
	adc r5, r6, #0
	adds r2, r3, r1
	mov r3, ip, lsr #0xc
	adc r0, r0, #0
	mov r2, r2, lsr #0xc
	orr r3, r3, r5, lsl #20
	orr r2, r2, r0, lsl #20
	sub r0, r3, r2
	strh r0, [r4, #0x86]
	ldrsh r5, [sp, #6]
	ldrsh r3, [sp, #2]
	ldrsh r2, [sp, #8]
	ldrsh r0, [sp]
	smull r3, r6, r5, r3
	adds ip, r3, r1
	smull r3, r0, r2, r0
	adc r5, r6, #0
	adds r2, r3, r1
	mov r3, ip, lsr #0xc
	adc r0, r0, #0
	mov r2, r2, lsr #0xc
	orr r3, r3, r5, lsl #20
	orr r2, r2, r0, lsl #20
	sub r0, r3, r2
	strh r0, [r4, #0x88]
	ldrsh r6, [sp, #8]
	ldrsh r5, [r4, #0x88]
	add r0, r4, #0x84
	ldrsh r3, [sp, #0xa]
	ldrsh r2, [r4, #0x86]
	smull r5, ip, r6, r5
	adds lr, r5, r1
	smull r5, r2, r3, r2
	adc r6, ip, #0
	adds r3, r5, r1
	mov r5, lr, lsr #0xc
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r5, r5, r6, lsl #20
	orr r3, r3, r2, lsl #20
	sub r2, r5, r3
	strh r2, [r4, #0x8a]
	ldrsh r6, [sp, #0xa]
	ldrsh r5, [r4, #0x84]
	ldrsh r3, [sp, #6]
	ldrsh r2, [r4, #0x88]
	smull lr, ip, r6, r5
	smull r5, r2, r3, r2
	adds r6, lr, r1
	adc r3, ip, #0
	mov r6, r6, lsr #0xc
	orr r6, r6, r3, lsl #20
	adds r3, r5, r1
	adc r2, r2, #0
	mov r3, r3, lsr #0xc
	orr r3, r3, r2, lsl #20
	sub r2, r6, r3
	strh r2, [r4, #0x8c]
	ldrsh lr, [sp, #6]
	ldrsh ip, [r4, #0x86]
	ldrsh r3, [sp, #8]
	ldrsh r2, [r4, #0x84]
	smull r6, r5, lr, ip
	smull ip, r2, r3, r2
	adds r6, r6, r1
	adc r5, r5, #0
	adds r3, ip, r1
	mov r6, r6, lsr #0xc
	orr r6, r6, r5, lsl #20
	adc r1, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r1, lsl #20
	sub r1, r6, r2
	strh r1, [r4, #0x8e]
	mov r1, r0
	bl VEC_Fx16Normalize
	add r0, r4, #0x8a
	mov r1, r0
	bl VEC_Fx16Normalize
	add sp, sp, #0x10
	ldmia sp!, {r4, r5, r6, lr}
	bx lr
	.align 2, 0
_0209D5E0: .word _021105EC
	arm_func_end spl_set_cross_to_axis
