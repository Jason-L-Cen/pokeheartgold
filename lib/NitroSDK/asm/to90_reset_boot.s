	.include "asm/macros.inc"
	.public OSi_DoBoot
	.extern OSi_CpuClear32
	.extern SDK_AUTOLOAD_DTCM_START

.section .itcm,4,1,7
	.type OSi_DoBoot, @function
	arm_func_start OSi_DoBoot
OSi_DoBoot:
	mov	r12, #67108864
	str	r12, [r12, #520]
	ldr	r1, [pc, #156]
	add	r1, r1, #16320
	add	r1, r1, #60
	mov	r0, #0
	str	r0, [r1]
	ldr	r1, [pc, #140]
	ldrh	r0, [r1]
	and	r0, r0, #15
	cmp	r0, #1
	bne . - 0xc
	mov	r0, #256
	strh	r0, [r1]
	mov	r0, #0
	ldr	r3, [pc, #112]
	ldr	r4, [r3]
	ldr	r1, [pc, #108]
	mov	r2, #128
	bl OSi_CpuClear32
	str	r4, [r3]
	ldr	r1, [pc, #96]
	mov	r2, #24
	bl OSi_CpuClear32
	ldr	r1, [pc, #88]
	strh	r0, [r1]
	ldr	r1, [pc, #84]
	mov	r2, #100
	bl OSi_CpuClear32
	ldr	r1, [pc, #52]
	ldrh	r0, [r1]
	and	r0, r0, #15
	cmp	r0, #1
	beq . - 0xc
	mov	r0, #0
	strh	r0, [r1]
	ldr	r3, [pc, #48]
	ldr	r12, [r3, #36]
	mov	lr, r12
	ldr	r11, [pc, #24]
	ldmia	r11, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10}
	mov	r11, #0
	bx r12
	.word SDK_AUTOLOAD_DTCM_START
	.word 0x04000180
	.word 0x027ffd9c
	.word 0x027ffd80
	.word 0x027fff80
	.word 0x027fff98
	.word 0x027fff9c
	.word 0x027ffe00
	arm_func_end OSi_DoBoot
	.type $d, @function
