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
	.public CTRDGi_WriteFlashSectorCoreMX
	.extern CTRDGi_EraseFlashSectorMX
	.extern CTRDGi_ProgramFlashByteMX
.text
	arm_func_start CTRDGi_WriteFlashSectorCoreMX
CTRDGi_WriteFlashSectorCoreMX: ; 0x020E30E8
	stmdb sp!, {r3, r4, r5, r6, r7, r8, lr}
	sub sp, sp, #0x24
	mov r5, r0
	ldmia r5!, {r0, r1, r2, r3}
	add r4, sp, #0
	stmia r4!, {r0, r1, r2, r3}
	ldmia r5!, {r0, r1, r2, r3}
	stmia r4!, {r0, r1, r2, r3}
	ldr r0, _020E3244 ; =AgbFlash
	ldr r1, [r5]
	ldr r0, [r0]
	str r1, [r4]
	ldrh r4, [sp, #0x20]
	ldrh r0, [r0, #0xa]
	ldr r7, [sp, #0xc]
	cmp r4, r0
	addhs sp, sp, #0x24
	ldrhs r0, _020E3248 ; =0x000080FF
	ldmhsia sp!, {r3, r4, r5, r6, r7, r8, pc}
	mov r0, r4
	bl CTRDGi_EraseFlashSectorMX
	movs r6, r0
	addne sp, sp, #0x24
	ldmneia sp!, {r3, r4, r5, r6, r7, r8, pc}
	ldr r0, _020E324C ; =ctrdgi_flash_lock_id
	ldrh r0, [r0]
	bl OS_LockCartridge
	ldr r5, _020E3250 ; =0x04000204
	ldr r0, _020E3244 ; =AgbFlash
	ldrh r3, [r5]
	ldr r2, [r0]
	ldrh r1, [r5]
	mov r0, r4, lsl #0xc
	ldr r2, [r2, #0x10]
	bic r1, r1, #3
	orr r1, r1, r2
	strh r1, [r5]
	mov r0, r0, lsr #0x10
	and r5, r3, #3
	bl CTRDGi_SetFlashBankMx
	ldr r0, _020E3244 ; =AgbFlash
	ldr r8, _020E3254 ; =ctrdg_flash_remainder
	ldr r2, [r0]
	ldr r1, _020E3258 ; =0x04000208
	ldr r0, [r2, #4]
	and r4, r4, #0xf
	strh r0, [r8]
	ldrh r3, [r2, #8]
	ldrh r2, [r1]
	mov r0, #0
	mov r3, r4, lsl r3
	strh r0, [r1]
	ldrh r1, [r8]
	ldr r0, _020E325C ; =ctrdgi_backup_irq
	add r4, r3, #0xa000000
	str r2, [r0]
	cmp r1, #0
	beq _020E3204
_020E31D0:
	mov r0, r7
	mov r1, r4
	bl CTRDGi_ProgramFlashByteMX
	movs r6, r0
	bne _020E3204
	ldrh r0, [r8]
	add r7, r7, #1
	add r4, r4, #1
	sub r0, r0, #1
	strh r0, [r8]
	ldrh r0, [r8]
	cmp r0, #0
	bne _020E31D0
_020E3204:
	ldr r3, _020E3258 ; =0x04000208
	ldr r0, _020E325C ; =ctrdgi_backup_irq
	ldrh r1, [r3]
	ldr r0, [r0]
	sub r2, r3, #4
	strh r0, [r3]
	ldrh r1, [r2]
	ldr r0, _020E324C ; =ctrdgi_flash_lock_id
	bic r1, r1, #3
	orr r1, r1, r5
	strh r1, [r2]
	ldrh r0, [r0]
	bl OS_UnlockCartridge
	mov r0, r6
	add sp, sp, #0x24
	ldmia sp!, {r3, r4, r5, r6, r7, r8, pc}
	.align 2, 0
_020E3244: .word AgbFlash
_020E3248: .word 0x000080FF
_020E324C: .word ctrdgi_flash_lock_id
_020E3250: .word 0x04000204
_020E3254: .word ctrdg_flash_remainder
_020E3258: .word 0x04000208
_020E325C: .word ctrdgi_backup_irq
	arm_func_end CTRDGi_WriteFlashSectorCoreMX
