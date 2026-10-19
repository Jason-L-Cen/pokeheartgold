// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeBackup.h"
#include "../NativeEvents.h"
#include <nitro/card.h>
#include <nitro/card/native_card.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { KERNEL_BACKUP_OWNER=0x7F };
typedef struct CardReply {
    struct w2c_env *env;
    u32 request,function,argument,destination,length;
    CARDResult result;
    u8 *buffer,*expected;
} CardReply;
struct HGNativeCardContext { BOOL bound,identified; CardReply *active; };
static pthread_mutex_t ownerMutex=PTHREAD_MUTEX_INITIALIZER;
static struct w2c_env *owner;

static u8 *span(struct w2c_env *env,u32 offset,u32 length) {
    if ((!offset && length) || offset>env->memory->size || length>env->memory->size-offset) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return env->memory->data+offset;
}
static u32 word(const u8 *bytes,u32 offset) { u32 value; memcpy(&value,bytes+offset,4); return value; }
static void validateCallback(struct w2c_env *env,u32 function) {
    if (!function || !env->irqTable || !env->rtcType || function>=env->irqTable->size) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
    wasm_rt_funcref_t ref=env->irqTable->data[function];
    if (!ref.func || !ref.func_type || memcmp(ref.func_type,env->rtcType,32)) wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT);
}
static void completed(void *argument) {
    CardReply *reply=argument; struct w2c_env *env=reply->env;
    env->card->active=NULL;
    if (!env->resetRequested) {
        if (reply->request==CARD_REQ_IDENTIFY) env->card->identified=reply->result==CARD_RESULT_SUCCESS;
        if (reply->request==CARD_REQ_READ_BACKUP && reply->result==CARD_RESULT_SUCCESS && reply->length)
            memcpy(span(env,reply->destination,reply->length),reply->buffer,reply->length);
        HGNativeInvokeIrqCallback(env,reply->function,reply->result,reply->argument,2);
    }
    free(reply->buffer); free(reply->expected); free(reply);
    __atomic_fetch_sub(&env->pendingCARD,1,__ATOMIC_RELEASE);
}
static void post(CardReply *reply) {
    if (!HGNativeQueueIrqCallback(OS_IE_SPFIFO_RECV,completed,reply)) {
        fputs("Native card completion could not be queued.\n",stderr); abort();
    }
}
static void backendCompleted(void *argument) {
    CardReply *reply=argument; reply->result=CARD_GetResultCode();
    if (reply->request==CARD_REQ_VERIFY_BACKUP && reply->result==CARD_RESULT_SUCCESS && reply->length &&
        memcmp(reply->buffer,reply->expected,reply->length)) reply->result=CARD_RESULT_FAILURE;
    post(reply);
}
static BOOL bind(struct w2c_env *env) {
    if (!env->card) { env->card=calloc(1,sizeof(*env->card)); if (!env->card) return FALSE; }
    if (env->card->bound) return TRUE;
    pthread_mutex_lock(&ownerMutex);
    if (!owner && HGNativeBackupTryLock(KERNEL_BACKUP_OWNER)) { owner=env; env->card->bound=TRUE; }
    pthread_mutex_unlock(&ownerMutex); return TRUE;
}
u32 w2c_env_HGNativeCardRequest(struct w2c_env *env,u32 request,u32 command,u32 mode,u32 retry,u32 function,u32 argument) {
    if (!env || !env->memory || !env->memory->data || env->memory->is64 || env->memory->pages!=env->memory->max_pages ||
        env->resetRequested) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (request==HG_CARD_CANCEL) {
        if (command || mode || retry || function || argument) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (env->card && env->card->bound && env->card->active) CARD_CancelBackupAsync();
        return TRUE;
    }
    if (command%4 || mode>3 || !retry || retry>0x7FFFFFFF) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const u8 *cmd=span(env,command,96); validateCallback(env,function);
    if (!env->irqGuard || !bind(env) || env->card->active) return FALSE;
    u32 source=word(cmd,12),destination=word(cmd,16),length=word(cmd,20),type=word(cmd,4);
    BOOL read=request==CARD_REQ_READ_BACKUP;
    BOOL write=request==CARD_REQ_WRITE_BACKUP;
    BOOL verify=request==CARD_REQ_VERIFY_BACKUP;
    u32 offset=read ? source : destination;
    CARDResult prepared=CARD_RESULT_SUCCESS; u8 *memory=NULL;
    if (read || write || verify) {
        if (offset>0x80000 || length>0x80000-offset || (read && mode!=CARD_REQUEST_MODE_RECV) ||
            (write && mode!=CARD_REQUEST_MODE_SEND && mode!=CARD_REQUEST_MODE_SEND_VERIFY)) prepared=CARD_RESULT_INVALID_PARAM;
        else memory=span(env,read ? destination : source,length);
    }
    CardReply *reply=calloc(1,sizeof(*reply)); if (!reply) return FALSE;
    *reply=(CardReply){env,request,function,argument,destination,length,prepared,NULL,NULL};
    if ((read || write || verify) && prepared==CARD_RESULT_SUCCESS) {
        // Bounds are checked before allocating/starting I/O; background work never reads guest memory.
        if (length) {
            reply->buffer=malloc(length);
            if (verify) reply->expected=malloc(length);
            if (!reply->buffer || (verify && !reply->expected)) {
                free(reply->buffer); free(reply->expected); free(reply); return FALSE;
            }
            if (write) memcpy(reply->buffer,memory,length);
            if (verify) memcpy(reply->expected,memory,length);
        }
    }
    env->card->active=reply; __atomic_fetch_add(&env->pendingCARD,1,__ATOMIC_ACQ_REL);
    if (!env->card->bound) reply->result=CARD_RESULT_NO_RESPONSE;
    else if (request==CARD_REQ_INIT) reply->result=CARD_RESULT_SUCCESS;
    else if (request==CARD_REQ_IDENTIFY) {
        (void)CARD_IdentifyBackup((CARDBackupType)type); reply->result=CARD_GetResultCode();
    } else if ((read || write || verify) && reply->result==CARD_RESULT_SUCCESS) {
        if (!env->card->identified) reply->result=CARD_RESULT_UNSUPPORTED;
        else {
            CARDRequest operation=write ? CARD_REQ_WRITE_BACKUP : CARD_REQ_READ_BACKUP;
            uaddr input=write ? (uaddr)reply->buffer : offset;
            uaddr output=write ? offset : (uaddr)reply->buffer;
            // The existing backend owns async execution, cancellation, verification and durable atomic publication.
            (void)CARDi_RequestStreamCommand(input,output,length,backendCompleted,reply,TRUE,operation,(int)retry,
                                             write ? (CARDRequestMode)mode : CARD_REQUEST_MODE_RECV);
            return TRUE;
        }
    } else if (!read && !write && !verify) reply->result=CARD_RESULT_UNSUPPORTED;
    post(reply); return TRUE;
}
void HGNativeDrainCard(struct w2c_env *env) {
    if (env->card && env->card->bound) { CARD_CancelBackupAsync(); (void)CARD_WaitBackupAsync(); }
}
void HGNativeCloseCardContext(struct w2c_env *env) {
    if (__atomic_load_n(&env->pendingCARD,__ATOMIC_ACQUIRE) || (env->card && env->card->active)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (env->card && env->card->bound) {
        (void)CARD_WaitBackupAsync();
        pthread_mutex_lock(&ownerMutex);
        if (owner!=env) abort();
        CARD_UnlockBackup(KERNEL_BACKUP_OWNER); owner=NULL;
        pthread_mutex_unlock(&ownerMutex);
    }
    free(env->card); env->card=NULL;
}
