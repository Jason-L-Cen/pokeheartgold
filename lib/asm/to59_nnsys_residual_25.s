	.include "asm/macros.inc"
	.include "nnsys.inc"
	.include "global.inc"
	.extern NNSi_G2dIdentity
	.extern _021109DC
	.extern _021D94D8
	.extern _021D9568
	.extern _021D9C68

.text
	arm_func_start NNS_G2dBeginRendering
NNS_G2dBeginRendering: ; 0x020BA034
	stmdb sp!, {r4, r5, r6, r7, r8, lr}
	ldr r5, _020BA0C4 ; =_021D94D8
	mov r4, #0
	str r0, [r5, #8]
	ldr r1, _020BA0C8 ; =0x0000FFFE
	ldr r0, _020BA0CC ; =_021109DC
	str r4, [r5, #0xc]
	strh r1, [r0]
	ldrh r0, [r5]
	cmp r0, #0
	ble _020BA090
	ldr r8, _020BA0D0 ; =_021D9C68
	mvn r7, #0
	mov r6, #0x10
_020BA06C:
	mov r0, r7
	mov r2, r6
	add r1, r8, #0x10
	bl MIi_CpuClearFast
	ldrh r0, [r5]
	add r4, r4, #1
	add r8, r8, #0x20
	cmp r4, r0
	blt _020BA06C
_020BA090:
	ldr r3, _020BA0C4 ; =_021D94D8
	mov r0, #0
	strh r0, [r3]
	ldr r1, _020BA0D4 ; =_021D9568
	mov r2, #0x100
	strh r0, [r3, #2]
	bl MIi_CpuClearFast
	ldr r0, _020BA0D8 ; =0x04000444
	mov r1, #0
	str r1, [r0]
	str r1, [r0, #0x10]
	bl NNSi_G2dIdentity
	ldmia sp!, {r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020BA0C4: .word _021D94D8
_020BA0C8: .word 0x0000FFFE
_020BA0CC: .word _021109DC
_020BA0D0: .word _021D9C68
_020BA0D4: .word _021D9568
_020BA0D8: .word 0x04000444
	arm_func_end NNS_G2dBeginRendering
