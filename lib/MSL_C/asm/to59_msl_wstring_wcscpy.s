	.include "asm/macros.inc"
#include <nitro/fs/overlay.h>
	.include "global.inc"

	.text

	arm_func_start wcscpy
wcscpy: ; 0x020ED8A0
	mov r3, r0
_020ED8A4:
	ldrh r2, [r1], #2
	mov ip, r3
	strh r2, [r3], #2
	ldrh r2, [ip]
	cmp r2, #0
	bne _020ED8A4
	bx lr
	arm_func_end wcscpy
