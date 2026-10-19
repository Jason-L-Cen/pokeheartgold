// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/os/reset.h>
#include <stddef.h>
#include <string.h>
extern void CARDi_InitCommon(void);
_Static_assert(sizeof(CARDiCommandArg)==96,"Original card command layout");
_Static_assert(offsetof(CARDiCommon,flag)==0x114 && offsetof(CARDiCommon,backup_cache_page_buf)==0x120,"Original card task layout");
static u8 input[1024],output[1024];
static struct { u32 count,mode,worker,result; } completion;
static s32 lockId=-1;
static void completed(void *argument) {
    (void)argument;
    completion.count++; completion.mode=OS_GetProcMode();
    completion.worker=OS_GetCurrentThread()==cardi_common.thread;
    completion.result=CARD_GetResultCode();
    if (completion.result==CARD_RESULT_SUCCESS) (void)CARD_ReadBackup(0x1000,output,sizeof(output));
}
static void rawCompletion(u32 result,void *argument) { (void)result; (void)argument; }
static void acquire(void) {
    if (lockId<0) { lockId=OS_GetLockID(); CARD_LockBackup((u16)lockId); }
}
static void release(void) { CARD_UnlockBackup((u16)lockId); OS_ReleaseLockID((u16)lockId); lockId=-1; }
u32 HGCardSetup(void) {
    OS_InitArena(); OS_InitLock(); OS_InitIrqTable(); OS_InitThread(); OS_InitReset();
    cardi_common.flag=CARD_STAT_INIT; CARDi_InitCommon();
    OS_EnableIrqMask(OS_IE_SPFIFO_RECV); OS_EnableIrq();
    for (u32 i=0;i<sizeof(input);i++) input[i]=(u8)(i*29+7);
    memset(output,0xA5,sizeof(output));
    return CARD_IsEnabled() && CARD_GetResultCode()==CARD_RESULT_SUCCESS;
}
u32 HGCardBuffer(u32 field) { return (u32)(uaddr)(field ? output : input); }
u32 HGCardReference(u32 field) {
    return field==0 ? (u32)(uaddr)cardi_common.cmd : field==1 ? (u32)(uaddr)&completion : (u32)(uaddr)rawCompletion;
}
u32 HGCardMode(u32 field) { return field==0 ? completion.count : field==1 ? completion.mode : field==2 ? completion.worker : completion.result; }
u32 HGCardAction(u32 action,u32 value) {
    BOOL result;
    switch (action) {
    case 0:
        acquire(); result=CARD_IdentifyBackup((CARDBackupType)value); release(); return result;
    case 1:
        acquire(); result=CARD_WriteAndVerifyBackup(value,input,sizeof(input)); release(); return result;
    case 2:
        acquire(); result=CARD_ReadBackup(value,output,sizeof(output)); release(); return result;
    case 3:
        completion=(typeof(completion)){0}; acquire();
        return CARDi_WriteAndVerifyBackup(value,input,sizeof(input),completed,NULL,TRUE);
    case 4: OS_SpinWait(1000); return CARD_TryWaitBackupAsync();
    case 5: result=CARD_GetResultCode(); release(); return result;
    case 6:
        acquire(); result=CARD_ReadBackup(0x7FFFF,output,2);
        { u32 status=result ? ~0u : CARD_GetResultCode(); release(); return status; }
    case 7: CARD_CancelBackupAsync(); return 0;
    case 8:
        OS_DisableIrq(); completion=(typeof(completion)){0}; acquire();
        CARD_WriteAndVerifyBackupAsync(0x1000,input,sizeof(input),completed,NULL);
        OS_ResetSystem(value); return ~0u;
    case 9: return CARD_GetBackupSectorSize();
    case 10:
        acquire(); cardi_common.cmd->src=(u32)(uaddr)input; cardi_common.cmd->dest=value;
        cardi_common.cmd->len=sizeof(input);
        extern BOOL CARDi_Request(CARDiCommon *,s32,s32);
        result=CARDi_Request(&cardi_common,CARD_REQ_VERIFY_BACKUP,1);
        { u32 status=result ? 0 : CARD_GetResultCode(); release(); return status; }
    }
    return ~0u;
}
