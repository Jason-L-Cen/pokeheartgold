	.include "asm/macros.inc"
	.include "jn_spl.inc"
	.include "global.inc"
	.public spl_calc_scfield

.text
	arm_func_start spl_calc_scfield
spl_calc_scfield: ; 0x0209DAB8
	ldr r3, [r3, #0x74]
	ldr r2, [r0]
	cmp r3, #0x80000000
	movne r2, r3
	ldrh r3, [r0, #6]
	mov r3, r3, lsl #0x1e
	movs r3, r3, lsr #0x1e
	beq _0209DAE4
	cmp r3, #1
	beq _0209DB38
	bx lr
_0209DAE4:
	ldr r3, [r1, #0x3c]
	cmp r3, r2
	bge _0209DB10
	ldr r0, [r1, #0xc]
	add r0, r3, r0
	cmp r0, r2
	subgt r0, r2, r3
	strgt r0, [r1, #0xc]
	ldrgth r0, [r1, #0x24]
	strgth r0, [r1, #0x26]
	bxgt lr
_0209DB10:
	cmp r3, r2
	bxlt lr
	ldr r0, [r1, #0xc]
	add r0, r3, r0
	cmp r0, r2
	sublt r0, r2, r3
	strlt r0, [r1, #0xc]
	ldrlth r0, [r1, #0x24]
	strlth r0, [r1, #0x26]
	bx lr
_0209DB38:
	ldr ip, [r1, #0x3c]
	cmp ip, r2
	bge _0209DB88
	ldr r3, [r1, #0xc]
	add r3, ip, r3
	cmp r3, r2
	ble _0209DB88
	sub r2, r2, ip
	str r2, [r1, #0xc]
	ldrsh r2, [r0, #4]
	ldr r3, [r1, #0x18]
	mov r0, #0x800
	smull ip, r2, r3, r2
	adds r3, ip, r0
	adc r0, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	rsb r0, r2, #0
	str r0, [r1, #0x18]
	bx lr
_0209DB88:
	cmp ip, r2
	bxlt lr
	ldr r3, [r1, #0xc]
	add r3, ip, r3
	cmp r3, r2
	bxge lr
	sub r2, r2, ip
	str r2, [r1, #0xc]
	ldrsh r2, [r0, #4]
	ldr r3, [r1, #0x18]
	mov r0, #0x800
	smull ip, r2, r3, r2
	adds r3, ip, r0
	adc r0, r2, #0
	mov r2, r3, lsr #0xc
	orr r2, r2, r0, lsl #20
	rsb r0, r2, #0
	str r0, [r1, #0x18]
	bx lr
	arm_func_end spl_calc_scfield
