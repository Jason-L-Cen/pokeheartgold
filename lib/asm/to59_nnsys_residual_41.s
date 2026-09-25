	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"

.text
	arm_func_start NNS_G3dGetAnmByIdx
NNS_G3dGetAnmByIdx: ; 0x020C3B90
	cmp r0, #0
	beq _020C3BE4
	ldrh r2, [r0, #0xc]
	ldr r2, [r0, r2]
	add ip, r0, r2
	adds r3, ip, #8
	beq _020C3BD0
	ldrb r0, [ip, #9]
	cmp r1, r0
	bhs _020C3BD0
	ldrh r2, [ip, #0xe]
	ldrh r0, [r3, r2]
	add r2, r3, r2
	add r2, r2, #4
	mla r1, r0, r1, r2
	b _020C3BD4
_020C3BD0:
	mov r1, #0
_020C3BD4:
	cmp r1, #0
	ldrne r0, [r1]
	addne r0, ip, r0
	bxne lr
_020C3BE4:
	mov r0, #0
	bx lr
	arm_func_end NNS_G3dGetAnmByIdx
