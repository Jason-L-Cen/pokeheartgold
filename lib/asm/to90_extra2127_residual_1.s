	.extern CARD_GetBackupSectorSize
	.extern CARDi_Request
	.extern DC_FlushRange
	.extern DC_InvalidateRange
	.extern cardi_common
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"

.text
	arm_func_start CARDi_IdentifyBackupCore
CARDi_IdentifyBackupCore: ; 0x020DC9F0
	stmdb sp!, {r3, r4, r5, lr}
	ldr r1, _020DCD54 ; =cardi_common
	mov r5, r0
	ldr r4, [r1]
	mov r1, #0
	add r0, r4, #0x18
	mov r2, #0x48
	bl MI_CpuFill8
	ldr r0, _020DCD58 ; =0x0000203F
	str r5, [r4, #4]
	str r0, [r4, #0x58]
	cmp r5, #0
	ldmeqia sp!, {r3, r4, r5, pc}
	mov r0, r5, asr #8
	and r0, r0, #0xff
	mov r3, #1
	mov ip, r3, lsl r0
	and r2, r5, #0xff
	mov r1, r5, asr #0x10
	str ip, [r4, #0x18]
	mov r0, #0xff
	strb r0, [r4, #0x54]
	cmp r2, #1
	and r0, r1, #0xff
	bne _020DCB2C
	cmp ip, #0x2000
	bhi _020DCA6C
	bhs _020DCAA8
	cmp ip, #0x200
	beq _020DCA88
	b _020DCD34
_020DCA6C:
	cmp ip, #0x10000
	bhi _020DCA7C
	beq _020DCACC
	b _020DCD34
_020DCA7C:
	cmp ip, #0x20000
	beq _020DCAF0
	b _020DCD34
_020DCA88:
	mov r0, #0x10
	str r0, [r4, #0x24]
	str r3, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0xf0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCAA8:
	mov r0, #0x20
	str r0, [r4, #0x24]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCACC:
	mov r0, #0x80
	str r0, [r4, #0x24]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #0xa
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
	b _020DCB10
_020DCAF0:
	mov r0, #0x100
	str r0, [r4, #0x24]
	mov r0, #3
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	mov r0, #0
	strb r0, [r4, #0x54]
_020DCB10:
	ldr r0, [r4, #0x24]
	str r0, [r4, #0x1c]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x40
	orr r0, r0, #0x4300
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCB2C:
	cmp r2, #2
	bne _020DCCF4
	cmp ip, #0x100000
	bhi _020DCB5C
	bhs _020DCB80
	cmp ip, #0x40000
	bhi _020DCB50
	beq _020DCB80
	b _020DCD34
_020DCB50:
	cmp ip, #0x80000
	beq _020DCB80
	b _020DCD34
_020DCB5C:
	cmp ip, #0x400000
	bhi _020DCB74
	bhs _020DCBF8
	cmp ip, #0x200000
	beq _020DCBAC
	b _020DCD34
_020DCB74:
	cmp ip, #0x800000
	beq _020DCC48
	b _020DCD34
_020DCB80:
	mov r0, #0x19
	str r0, [r4, #0x30]
	mov r1, #0x12c
	str r1, [r4, #0x34]
	ldr r0, _020DCD5C ; =0x00001388
	str r1, [r4, #0x50]
	str r0, [r4, #0x40]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x480
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCBAC:
	mov r0, #0x17
	str r0, [r4, #0x30]
	mov r1, #0x12c
	str r1, [r4, #0x34]
	mov r1, #0x1f4
	ldr r0, _020DCD5C ; =0x00001388
	str r1, [r4, #0x40]
	str r0, [r4, #0x44]
	mov r1, r0, lsl #1
	ldr r0, _020DCD60 ; =0x0000EA60
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x80
	orr r0, r0, #0x5400
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCBF8:
	mov r0, #0x258
	str r0, [r4, #0x40]
	add r0, r0, #0x960
	str r0, [r4, #0x44]
	mov r0, #0x46
	str r0, [r4, #0x48]
	mov r0, #0x96
	str r0, [r4, #0x4c]
	ldr r1, _020DCD64 ; =0x000059D8
	ldr r0, _020DCD68 ; =0x000C3500
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	mov r0, #0x1000
	str r0, [r4, #0x20]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0xd000
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCC48:
	cmp r0, #0
	bne _020DCC88
	mov r0, #0x3e8
	str r0, [r4, #0x40]
	add r0, r0, #0x7d0
	str r0, [r4, #0x44]
	ldr r1, _020DCD6C ; =0x000109A0
	ldr r0, _020DCD70 ; =0x00027100
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x5000
	str r0, [r4, #0x58]
	b _020DCCC4
_020DCC88:
	cmp r0, #1
	bne _020DCCC4
	mov r0, #0x3e8
	str r0, [r4, #0x40]
	add r0, r0, #0x7d0
	str r0, [r4, #0x44]
	ldr r1, _020DCD6C ; =0x000109A0
	ldr r0, _020DCD70 ; =0x00027100
	str r1, [r4, #0x38]
	str r0, [r4, #0x3c]
	mov r0, #0x84
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x5000
	str r0, [r4, #0x58]
_020DCCC4:
	mov r0, #0x10000
	str r0, [r4, #0x1c]
	mov r0, #0x100
	str r0, [r4, #0x24]
	mov r0, #3
	str r0, [r4, #0x28]
	mov r0, #5
	str r0, [r4, #0x2c]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0xb40
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCCF4:
	cmp r2, #3
	bne _020DCD34
	cmp ip, #0x2000
	cmpne ip, #0x8000
	bne _020DCD34
	str ip, [r4, #0x24]
	str ip, [r4, #0x1c]
	mov r0, #2
	str r0, [r4, #0x28]
	mov r0, #0
	strb r0, [r4, #0x54]
	ldr r0, [r4, #0x58]
	orr r0, r0, #0x40
	orr r0, r0, #0x4300
	str r0, [r4, #0x58]
	ldmia sp!, {r3, r4, r5, pc}
_020DCD34:
	mov r1, #0
	str r1, [r4, #4]
	str r1, [r4, #0x18]
	ldr r0, _020DCD54 ; =cardi_common
	mov r1, #3
	ldr r0, [r0]
	str r1, [r0]
	ldmia sp!, {r3, r4, r5, pc}
	.align 2, 0
_020DCD54: .word cardi_common
_020DCD58: .word 0x0000203F
_020DCD5C: .word 0x00001388
_020DCD60: .word 0x0000EA60
_020DCD64: .word 0x000059D8
_020DCD68: .word 0x000C3500
_020DCD6C: .word 0x000109A0
_020DCD70: .word 0x00027100
	arm_func_end CARDi_IdentifyBackupCore
