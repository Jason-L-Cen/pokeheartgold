	.include "asm/macros.inc"
	.include "gx_vramcnt.inc"
	.include "global.inc"

	.public gGXState
	.public GXi_VRamLockId

	.extern GX_VRAMCNT_SetLCDC_

	.text
	arm_func_start GX_SetBankForTexPltt
GX_SetBankForTexPltt: ; 0x020CE368
	stmdb sp!, {r3, lr}
	ldr r1, _020CE440 ; =gGXState
	mvn ip, r0
	ldrh r3, [r1]
	ldrh r2, [r1, #0xa]
	cmp r0, #0x30
	orr r2, r3, r2
	and r2, ip, r2
	strh r2, [r1]
	strh r0, [r1, #0xa]
	bgt _020CE3BC
	cmp r0, #0x30
	bge _020CE418
	cmp r0, #0x10
	bgt _020CE3B0
	bge _020CE424
	cmp r0, #0
	b _020CE430
_020CE3B0:
	cmp r0, #0x20
	beq _020CE3EC
	b _020CE430
_020CE3BC:
	cmp r0, #0x60
	bgt _020CE3D4
	bge _020CE3E0
	cmp r0, #0x40
	beq _020CE3FC
	b _020CE430
_020CE3D4:
	cmp r0, #0x70
	beq _020CE40C
	b _020CE430
_020CE3E0:
	ldr r0, _020CE444 ; =0x04000246
	mov r1, #0x8b
	strb r1, [r0]
_020CE3EC:
	ldr r0, _020CE448 ; =0x04000245
	mov r1, #0x83
	strb r1, [r0]
	b _020CE430
_020CE3FC:
	ldr r0, _020CE444 ; =0x04000246
	mov r1, #0x83
	strb r1, [r0]
	b _020CE430
_020CE40C:
	ldr r0, _020CE444 ; =0x04000246
	mov r1, #0x9b
	strb r1, [r0]
_020CE418:
	ldr r0, _020CE448 ; =0x04000245
	mov r1, #0x93
	strb r1, [r0]
_020CE424:
	ldr r0, _020CE44C ; =0x04000244
	mov r1, #0x83
	strb r1, [r0]
_020CE430:
	ldr r0, _020CE440 ; =gGXState
	ldrh r0, [r0]
	bl GX_VRAMCNT_SetLCDC_
	ldmia sp!, {r3, pc}
	.align 2, 0
_020CE440: .word gGXState
_020CE444: .word 0x04000246
_020CE448: .word 0x04000245
_020CE44C: .word 0x04000244
	arm_func_end GX_SetBankForTexPltt
