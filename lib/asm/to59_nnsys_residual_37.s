	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.public addLink_

.text
	arm_func_start addLink_
addLink_: ; 0x020BE174
	ldr ip, [r0]
	cmp ip, #0
	streq r1, [r0]
	bxeq lr
	ldr r2, [ip, #0x10]
	cmp r2, #0
	bne _020BE1D4
	ldrb r3, [ip, #0x18]
	ldrb r2, [r1, #0x18]
	cmp r3, r2
	bls _020BE1CC
	ldr r2, [r1, #0x10]
	mov r3, r1
	cmp r2, #0
	beq _020BE1C0
_020BE1B0:
	ldr r3, [r3, #0x10]
	ldr r2, [r3, #0x10]
	cmp r2, #0
	bne _020BE1B0
_020BE1C0:
	str ip, [r3, #0x10]
	str r1, [r0]
	bx lr
_020BE1CC:
	str r1, [ip, #0x10]
	bx lr
_020BE1D4:
	beq _020BE224
	ldrb r3, [r1, #0x18]
_020BE1DC:
	ldrb r0, [r2, #0x18]
	cmp r0, r3
	blo _020BE214
	ldr r0, [r1, #0x10]
	mov r3, r1
	cmp r0, #0
	beq _020BE208
_020BE1F8:
	ldr r3, [r3, #0x10]
	ldr r0, [r3, #0x10]
	cmp r0, #0
	bne _020BE1F8
_020BE208:
	str r1, [ip, #0x10]
	str r2, [r3, #0x10]
	bx lr
_020BE214:
	mov ip, r2
	ldr r2, [r2, #0x10]
	cmp r2, #0
	bne _020BE1DC
_020BE224:
	str r1, [ip, #0x10]
	bx lr
	arm_func_end addLink_
