	.extern AgbFlash
	.extern CTRDGi_PollingSR
	.extern CTRDGi_SetFlashBankMx
	.extern OS_LockCartridge
	.extern OS_UnlockCartridge
	.extern _021E4F68
	.extern _021E4F6C
	.extern _021E4F90
	.extern ctrdg_flash_remainder
	.extern ctrdgi_backup_irq
	.extern ctrdgi_flash_lock_id
	.include "asm/macros.inc"
	.include "nitro.inc"
	.include "global.inc"
	.public CTRDGi_VerifyFlashErase
	.section .text.unified_44,4,1,6
	arm_func_start CTRDGi_VerifyFlashErase
CTRDGi_VerifyFlashErase: ; 0x020E2CB8
	stmdb sp!, {r4, r5, r6, lr}
	ldr r2, _020E2D34 ; =ctrdgi_flash_lock_id
	mov r5, r0
	ldrh r0, [r2]
	mov r4, r1
	bl OS_LockCartridge
	ldr ip, _020E2D38 ; =0x04000204
	ldr r0, _020E2D3C ; =AgbFlash
	ldrh r3, [ip]
	ldr r0, [r0]
	ldrh r1, [ip]
	ldr r2, [r0, #0x10]
	mov r0, r5
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [ip]
	and r6, r3, #3
	mov r5, #0
	blx r4
	cmp r0, #0
	ldr r2, _020E2D38 ; =0x04000204
	ldr r0, _020E2D34 ; =ctrdgi_flash_lock_id
	ldrh r1, [r2]
	ldrne r5, _020E2D40 ; =0x00008004
	bic r1, r1, #3
	orr r1, r1, r6
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r5
	ldmia sp!, {r4, r5, r6, pc}
	.align 2, 0
_020E2D34: .word ctrdgi_flash_lock_id
_020E2D38: .word 0x04000204
_020E2D3C: .word AgbFlash
_020E2D40: .word 0x00008004
	arm_func_end CTRDGi_VerifyFlashErase
