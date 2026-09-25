	.include "asm/macros.inc"
	.include "dwcbm.inc"
	.include "global.inc"

.text
	arm_func_start DWCi_BACKUPlConvMaskCidr
DWCi_BACKUPlConvMaskCidr: ; 0x0209E788
	stmdb sp!, {r4, lr}
	mov ip, #0
	mov r3, ip
	mov r2, ip
_0209E798:
	ldrb r4, [r0, ip]
	mov lr, r2
_0209E7A0:
	mov r1, r4, asr lr
	tst r1, #1
	add lr, lr, #1
	addne r3, r3, #1
	cmp lr, #8
	blt _0209E7A0
	add ip, ip, #1
	cmp ip, #4
	blt _0209E798
	and r0, r3, #0xff
	ldmia sp!, {r4, pc}
	arm_func_end DWCi_BACKUPlConvMaskCidr
