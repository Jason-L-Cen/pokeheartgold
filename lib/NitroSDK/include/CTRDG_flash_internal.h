#ifndef POKEHEARTGOLD_CTRDG_FLASH_INTERNAL_H
#define POKEHEARTGOLD_CTRDG_FLASH_INTERNAL_H

#include "CTRDG_backup.h"
#include "CTRDG_task.h"
#include "OS_thread.h"

// AGB flash backup drivers (the LE39FW512, MX29L010 / LE26FV10N1TS_10 / defaultFlash1M and MN63F805MNP
// tables of CTRDG_IdentifyAgbBackup) and the task thread that runs their asynchronous requests.
// lib/NitroSDK/src/ctrdg/ctrdg_flash_*.c, ctrdg_task.c.

#define CTRDG_BACKUP_COM_ADR1 (CTRDG_AGB_FLASH_ADR + 0x00005555)
#define CTRDG_BACKUP_COM_ADR2 (CTRDG_AGB_FLASH_ADR + 0x00002AAA)

// The task thread object (CTRDG_Init passes it to CTRDGi_InitTaskThread).
typedef struct CTRDGiTaskThread {
    OSThread thread;
    CTRDGTaskInfo *volatile task; // 0xC0: request to run, NULL when idle (set by CTRDGi_SetTask)
    CTRDGTaskInfo info;           // 0xC4: setting this one as the request ends the thread
} CTRDGiTaskThread;

typedef struct CTRDGiTaskWork {
    CTRDGiTaskThread *thread;
    CTRDGTaskInfo nextTask; // copy of the request being run
    u8 stack[0x400];
} CTRDGiTaskWork;

extern u16 ctrdgi_flash_lock_id;
extern BOOL ctrdgi_backup_irq;
extern CTRDGiTaskWork gCtrdgTaskWork;
extern CTRDGTaskInfo gCtrdgNextTask; // label of gCtrdgTaskWork.nextTask
extern u8 gCtrdgTaskStack[];         // label of gCtrdgTaskWork.stack

void CTRDGi_SetFlashBankMx(u16 bank);

u32 CTRDGi_EraseFlashChipCoreLE(CTRDGTaskInfo *arg);
u32 CTRDGi_EraseFlashSectorCoreLE(CTRDGTaskInfo *arg);
u16 CTRDGi_ProgramFlashByteLE(u8 *src, u8 *dst);
u32 CTRDGi_WriteFlashSectorCoreLE(CTRDGTaskInfo *arg);
u32 CTRDGi_VerifyFlashCoreFF(u8 *adr);
u32 CTRDGi_VerifyFlashErase(u8 *adr, u32 (*verify)(u8 *adr));
u16 CTRDGi_EraseFlashChipLE(void);
u16 CTRDGi_EraseFlashSectorLE(u16 secNo);
u16 CTRDGi_WriteFlashSectorLE(u16 secNo, u8 *src);
void CTRDGi_EraseFlashChipAsyncLE(CTRDG_TASK_FUNC callback);
void CTRDGi_EraseFlashSectorAsyncLE(u16 secNo, CTRDG_TASK_FUNC callback);
void CTRDGi_WriteFlashSectorAsyncLE(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

u32 CTRDGi_EraseFlashChipCoreMX(CTRDGTaskInfo *arg);
u32 CTRDGi_EraseFlashSectorCoreMX(CTRDGTaskInfo *arg);
u16 CTRDGi_ProgramFlashByteMX(u8 *src, u8 *dst);
u32 CTRDGi_WriteFlashSectorCoreMX(CTRDGTaskInfo *arg);
u16 CTRDGi_EraseFlashChipMX(void);
u16 CTRDGi_EraseFlashSectorMX(u16 secNo);
u16 CTRDGi_WriteFlashSectorMX(u16 secNo, u8 *src);
void CTRDGi_EraseFlashChipAsyncMX(CTRDG_TASK_FUNC callback);
void CTRDGi_EraseFlashSectorAsyncMX(u16 secNo, CTRDG_TASK_FUNC callback);
void CTRDGi_WriteFlashSectorAsyncMX(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

u32 CTRDGi_WriteFlashSectorCoreMX5(CTRDGTaskInfo *arg);
u16 CTRDGi_WriteFlashSectorMX5(u16 secNo, u8 *src);
void CTRDGi_WriteFlashSectorAsyncMX5(u16 secNo, u8 *src, CTRDG_TASK_FUNC callback);

void CTRDGi_InitTaskThread(void *thread);
BOOL CTRDGi_IsTaskAvailable(void);
void CTRDGi_InitTaskInfo(CTRDGTaskInfo *info);
void CTRDGi_TaskThread(void *arg);
void CTRDGi_SetTask(CTRDGTaskInfo *info, CTRDG_TASK_FUNC task, CTRDG_TASK_FUNC callback);

#endif // POKEHEARTGOLD_CTRDG_FLASH_INTERNAL_H
