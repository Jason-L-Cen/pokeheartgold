// SPDX-License-Identifier: GPL-3.0-or-later
#include "../NativeBackup.h"
#include <nitro/card.h>
#include <nitro/os/spinLock.h>
#include <pthread.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) { fprintf(stderr,"Backup check line %d: %s\n",__LINE__,#value); abort(); } } while (0)
typedef struct Completion { int count; CARDResult result; BOOL readBack; u8 bytes[16]; } Completion;
static void completed(void *argument) {
    Completion *state = argument;
    CHECK(OS_GetProcMode()==OS_PROCMODE_SYS);
    state->count++; state->result=CARD_GetResultCode();
    if (state->readBack) CHECK(CARD_ReadBackup(0x1000,state->bytes,sizeof(state->bytes)));
}
typedef struct CancelState {
    pthread_mutex_t mutex;
    pthread_cond_t changed;
    BOOL entered, released;
    u8 data[4];
    Completion completion;
} CancelState;
static void queueThenHold(void *argument) {
    CancelState *state=argument;
    CARD_WriteAndVerifyBackupAsync(0x2000,state->data,sizeof(state->data),completed,&state->completion);
    pthread_mutex_lock(&state->mutex); state->entered=TRUE; pthread_cond_broadcast(&state->changed);
    while (!state->released) pthread_cond_wait(&state->changed,&state->mutex);
    pthread_mutex_unlock(&state->mutex);
}

void CheckNativeBackup(void) {
    s32 ids[48];
    for (u32 i=0;i<48;i++) { ids[i]=OS_GetLockID(); CHECK(ids[i]==OS_MAINP_LOCK_ID_START+i); }
    CHECK(OS_GetLockID()==OS_LOCK_ID_ERROR);
    OS_ReleaseLockID(ids[7]); CHECK(OS_GetLockID()==ids[7]);
    for (u32 i=0;i<48;i++) OS_ReleaseLockID(ids[i]);
    s32 id=OS_GetLockID(); CHECK(id==OS_MAINP_LOCK_ID_START);
    char folder[]="/tmp/heartgold-backup-XXXXXX"; CHECK(mkdtemp(folder));
    char path[PATH_MAX], bad[PATH_MAX], linkPath[PATH_MAX], foreign[PATH_MAX], lockPath[PATH_MAX];
    snprintf(path,sizeof(path),"%s/game.sav",folder); snprintf(lockPath,sizeof(lockPath),"%s.lock",path);
    snprintf(bad,sizeof(bad),"%s/bad.sav",folder); snprintf(linkPath,sizeof(linkPath),"%s/link.sav",folder);
    snprintf(foreign,sizeof(foreign),"%s/foreign.sav",folder);
    CHECK(HGNativeBackupSetPath(path));
    struct stat info; CHECK(!stat(path,&info) && info.st_size==0x80000);
    int other=open(lockPath,O_RDWR); CHECK(other>=0);
    CHECK(flock(other,LOCK_EX|LOCK_NB)==-1 && (errno==EWOULDBLOCK || errno==EAGAIN)); close(other);
    CARD_LockBackup(id);
    CHECK(CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_4MBITS_EX));
    CHECK(!CARD_IdentifyBackup(CARD_BACKUP_TYPE_EEPROM_4KBITS) && CARD_GetResultCode()==CARD_RESULT_UNSUPPORTED);
    CHECK(CARD_IdentifyBackup(CARD_BACKUP_TYPE_FLASH_4MBITS) && CARD_GetBackupSectorSize()==0x10000);
    CHECK(!HGNativeBackupSetPath(NULL));
    u8 input[128], output[130];
    for (u32 i=0;i<sizeof(input);i++) input[i]=(u8)(i^0xA5);
    CHECK(CARD_ReadBackup(0,output,sizeof(output)));
    for (u32 i=0;i<sizeof(output);i++) CHECK(output[i]==0xFF);
    CHECK(CARD_WriteAndVerifyBackup(0x1000,input,sizeof(input)));
    CHECK(CARD_ReadBackup(0xFFF,output,sizeof(output)) && output[0]==0xFF && output[129]==0xFF);
    CHECK(!memcmp(output+1,input,sizeof(input)));
    Completion state={.readBack=TRUE};
    CARD_ReadBackupAsync(0x1000,output,sizeof(input),completed,&state);
    CHECK(CARD_WaitBackupAsync() && state.count==1 && state.result==CARD_RESULT_SUCCESS);
    CHECK(!memcmp(output,input,sizeof(input)) && !memcmp(state.bytes,input,sizeof(state.bytes)));
    CHECK(CARD_TryWaitBackupAsync());
    CHECK(CARD_WriteBackup(0x7FFF0,input,16));
    memset(output,0xA5,sizeof(output));
    CHECK(!CARD_ReadBackup(0x7FFFF,output,2) && CARD_GetResultCode()==CARD_RESULT_INVALID_PARAM && output[0]==0xA5);
    state=(Completion){0};
    CARD_WriteBackupAsync(0x7FFFF,input,2,completed,&state);
    CHECK(!CARD_WaitBackupAsync() && state.count==1 && state.result==CARD_RESULT_INVALID_PARAM);
    CHECK(!CARDi_RequestStreamCommand(0,0,0,NULL,NULL,FALSE,CARD_REQ_PROGRAM_BACKUP,1,CARD_REQUEST_MODE_SEND));
    CHECK(CARD_GetResultCode()==CARD_RESULT_UNSUPPORTED);
    CancelState cancellation={.mutex=PTHREAD_MUTEX_INITIALIZER,.changed=PTHREAD_COND_INITIALIZER,.data={1,2,3,4}};
    CARD_ReadBackupAsync(0,output,4,queueThenHold,&cancellation);
    pthread_mutex_lock(&cancellation.mutex);
    while (!cancellation.entered) pthread_cond_wait(&cancellation.changed,&cancellation.mutex);
    CARD_CancelBackupAsync(); cancellation.released=TRUE; pthread_cond_broadcast(&cancellation.changed);
    pthread_mutex_unlock(&cancellation.mutex);
    CHECK(!CARD_WaitBackupAsync() && cancellation.completion.count==1 && cancellation.completion.result==CARD_RESULT_CANCELED);
    CHECK(CARD_ReadBackup(0x2000,output,4));
    for (u32 i=0;i<4;i++) CHECK(output[i]==0xFF);
    CARD_CancelBackupAsync(); CHECK(CARD_WriteBackup(0x2000,input,4));
    pthread_cond_destroy(&cancellation.changed); pthread_mutex_destroy(&cancellation.mutex);
    CARD_UnlockBackup(id); CHECK(HGNativeBackupSetPath(NULL));
    CHECK(HGNativeBackupSetPath(path)); CARD_LockBackup(id);
    CHECK(CARD_ReadBackup(0x1000,output,sizeof(input)) && !memcmp(output,input,sizeof(input)));
    CHECK(CARD_ReadBackup(0x7FFF0,output,16) && !memcmp(output,input,16)); CARD_UnlockBackup(id);
    int fd=open(bad,O_WRONLY|O_CREAT|O_EXCL,0600); CHECK(fd>=0 && write(fd,"KEEP",4)==4); close(fd);
    CHECK(!HGNativeBackupSetPath(bad));
    CHECK(!symlink(path,linkPath) && !HGNativeBackupSetPath(linkPath));
    fd=open(bad,O_RDONLY); CHECK(fd>=0 && read(fd,output,4)==4 && !memcmp(output,"KEEP",4)); close(fd);
    u8 *replacement=malloc(0x80000); CHECK(replacement); memset(replacement,0x7E,0x80000);
    fd=open(foreign,O_WRONLY|O_CREAT|O_EXCL,0600); CHECK(fd>=0);
    CHECK(write(fd,replacement,0x80000)==0x80000); close(fd); free(replacement);
    CHECK(!rename(foreign,path)); CARD_LockBackup(id);
    CHECK(!CARD_WriteBackup(0x1000,input,sizeof(input)) && CARD_GetResultCode()==CARD_RESULT_ERROR);
    CARD_UnlockBackup(id); CHECK(HGNativeBackupSetPath(path)); CARD_LockBackup(id);
    CHECK(CARD_ReadBackup(0x1000,output,sizeof(output)));
    for (u32 i=0;i<sizeof(output);i++) CHECK(output[i]==0x7E);
    CARD_UnlockBackup(id); CHECK(HGNativeBackupSetPath(NULL)); CARD_LockBackup(id);
    state=(Completion){0}; CARD_ReadBackupAsync(0,output,4,completed,&state);
    CHECK(!CARD_WaitBackupAsync() && state.count==1 && state.result==CARD_RESULT_NO_RESPONSE);
    CARD_UnlockBackup(id); OS_ReleaseLockID(id);
    CHECK(!unlink(path) && !unlink(lockPath) && !unlink(bad) && !unlink(linkPath));
    snprintf(lockPath,sizeof(lockPath),"%s.lock",bad); CHECK(!unlink(lockPath));
    snprintf(lockPath,sizeof(lockPath),"%s.lock",linkPath); CHECK(!unlink(lockPath));
    CHECK(!rmdir(folder));
    puts("PASS: native backup persistence, 48 SDK lock IDs, async callbacks/reentry/cancel, exact bounds and existing-file protection.");
}
