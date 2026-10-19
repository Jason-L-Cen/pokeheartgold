// SPDX-License-Identifier: GPL-3.0-or-later
// Use the original private scheduler/archive callback with probe heap/IRQ-mask bindings and real CPU masks.
#include "../../../src/wifi_setup_ui_system.c"
#include "wifi_setup_internal.h"
#include "wifi_setup_local_wireless.h"
#include <stddef.h>
#include <stdlib.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(WifiSetupTask)==20 && sizeof(WifiSetupTaskQueue)==64,"Setup scheduler ABI32 layouts");

WifiSetupWork gWifiSetupWork;
WifiSetupConfig gWifiSetupConfig;
u8 gWifiSetupRecordBuffer[2048],gWifiSetupOwnMacAddress[6]={0,0x11,0x22,0x33,0x44,0x55};
WifiSetupMbParent *gWifiSetupMbParent;
WifiSetupTaskQueue *gWifiSetupTaskQueues;
NNSFndHeapHandle gWifiSetupHeap;
static OSIrqMask irqMask=~0u;
static u32 frees;
static void *expectedFree;

OSIrqMask OS_DisableIrqMask(OSIrqMask mask) { OSIrqMask old=irqMask; irqMask&=~mask; return old; }
OSIrqMask OS_EnableIrqMask(OSIrqMask mask) { OSIrqMask old=irqMask; irqMask|=mask; return old; }
s32 FX_ModS32(s32 a,s32 b) { if (!b) abort(); return a%b; }
void OS_Terminate(void) { abort(); }
void NNS_FndFreeToExpHeap(NNSFndHeapHandle heap,void *data) {
    if (heap!=gWifiSetupHeap || data!=expectedFree) abort();
    frees++; expectedFree=NULL; free(data);
}
int WifiSetupMbParent_GetChildState(int aid);
u8 *WifiSetup_GetPacketPayload(u8 *,u32 *,u32 *);

static u32 calls,order[8];
struct TaskArg { WifiSetupTask *task; u32 id; int defer; };
static void ObserveTask(void *task,void *arg) {
    struct TaskArg *state=arg;
    if (!state || task!=state->task || calls>=8) abort();
    order[calls++]=state->id;
    if (state->defer) WifiSetupTask_DestroyLater(0,task);
}
static void ObserveReplacement(void *task,void *arg) {
    struct TaskArg *state=arg;
    if (!state || task!=state->task || calls>=8) abort();
    order[calls++]=state->id+10;
}

u32 CheckOriginalWifiSetup(void) {
    // The supplied pool/list records exercise original insertion, dispatch and deferred destruction.
    WifiSetupTaskQueue queue[2]={{0}}; WifiSetupTask storage[3]={{0}};
    struct { u16 size; u8 head,tail; void *items[4]; } available={4,0,3,{storage,storage+1,storage+2,NULL}},destroy={4,0,0,{NULL}};
    gWifiSetupTaskQueues=queue; gWifiSetupHeap=(void *)0x101;
    queue[0].taskPool=(WifiSetupPool *)&available; queue[0].destroyQueue=(WifiSetupPool *)&destroy;
    queue[0].capacity=3; queue[0].enabled=1; queue[0].tail.priority=255;
    queue[0].head.node.next=&queue[0].tail.node; queue[0].tail.node.prev=&queue[0].head.node;
    struct TaskArg a={NULL,1,0},b={NULL,2,0};
    struct TaskArg *owned=malloc(sizeof(*owned)); CHECK(owned);
    *owned=(struct TaskArg){NULL,3,1}; expectedFree=owned;
    b.task=WifiSetupTask_Create(0,ObserveTask,&b,30);
    a.task=WifiSetupTask_Create(0,ObserveTask,&a,10);
    owned->task=WifiSetupTask_CreateEx(0,ObserveTask,owned,30,1);
    WifiSetupTask *removed=owned->task;
    calls=frees=0; WifiSetupTask_SetQueueEnabled(0,0); WifiSetupTask_RunQueue(0); CHECK(!calls);
    WifiSetupTask_SetQueueEnabled(0,1); WifiSetupTask_RunQueue(0);
    CHECK(calls==3 && order[0]==1 && order[1]==2 && order[2]==3 && frees==1 && expectedFree==NULL);
    CHECK(removed->node.prev==NULL && removed->node.next==NULL && removed->arg==NULL && available.tail==1);
    calls=0; WifiSetupTask_SetFunc(b.task,ObserveReplacement); WifiSetupTask_RunQueue(0);
    CHECK(calls==2 && order[0]==1 && order[1]==12);
    WifiSetupTask_Destroy(0,a.task); WifiSetupTask_Destroy(0,b.task); CHECK(available.tail==3);
    WifiSetupTask *empty=WifiSetupTask_CreateEx(0,ObserveTask,NULL,5,1);
    WifiSetupTask_Destroy(0,empty); WifiSetupHeap_Free(NULL);
    void *none=NULL; WifiSetupHeap_FreeAndClear(&none);
    CHECK(irqMask==~0u && frees==1 && available.tail==3 && queue[0].head.node.next==&queue[0].tail.node);
    gWifiSetupTaskQueues=NULL;
    FS_ARCHIVE_WRITE_FUNC volatile writer=WifiSetupArchive_Write;
    u8 guard[4]={1,2,3,4};
    CHECK(writer(NULL,guard,0xFFFFFFFF,0xFFFFFFFF)==FS_RESULT_FAILURE && guard[0]==1 && guard[3]==4);

    static WifiSetupMbParent parent;
    gWifiSetupMbParent=&parent;
    const int aids[]={1,3,15};
    for (u32 i=0;i<3;i++) for (int state=0;state<=6;state++) {
        memset(&parent.head,0,sizeof(parent.head)); u16 mask=1u<<aids[i];
        if (state) parent.head.connectedMask=mask;
        u16 *stages[]={&parent.head.requestedMask,&parent.head.waitingMask,&parent.head.sendingMask,&parent.head.sentMask,&parent.head.bootableMask};
        if (state>=2) *stages[state-2]=mask;
        OSIntrMode prior=OS_DisableInterrupts(),wanted=i&1 ? OS_INTRMODE_DISABLE_IRQ : OS_INTRMODE_ENABLE;
        OS_RestoreInterrupts(wanted);
        CHECK(WifiSetupMbParent_GetChildState(aids[i])==state && OS_GetCpsrIrq()==wanted);
        CHECK(WifiSetupMbParent_GetChildState(aids[i]==15 ? 1 : 15)==0 && OS_GetCpsrIrq()==wanted);
        OS_RestoreInterrupts(prior);
    }
    gWifiSetupMbParent=NULL;

    static const u8 normal[]={0x00,0x02,0x00,0x28,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x07,0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x72};
    static const u8 extended[]={0x00,0x02,0x00,0x40,0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x05,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x03,0x00,0x07,0x06,0x00,0x01,0x02,0x03,0x04,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x06,0x00,0x11,0x22,0x33,0x44,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xb3};
    _Alignas(8) u8 output[2080];
    for (int mode=0;mode<2;mode++) {
        memset(output,0xA5,sizeof(output)); memset(gWifiSetupRecordBuffer,0xCC,sizeof(gWifiSetupRecordBuffer));
        gWifiSetupConfig.packetBuffer=gWifiSetupRecordBuffer+8; gWifiSetupWork.extendedRequest=mode;
        u32 length=WifiSetup_BuildVersionRequest(output+8),expected=mode ? sizeof(extended) : sizeof(normal);
        CHECK(length==expected && !memcmp(output+8,mode ? extended : normal,length));
        for (u32 i=0;i<8;i++) CHECK(output[i]==0xA5);
        for (u32 i=8+length;i<sizeof(output);i++) CHECK(output[i]==0xA5);
        CHECK(gWifiSetupRecordBuffer[length-8]==0xCC);
        u32 type,size; CHECK(WifiSetup_GetPacketPayload(output+8,&type,&size)==output+14 && type==2 && size==length-8);
        output[8+length-1]^=1; CHECK(WifiSetup_GetPacketPayload(output+8,&type,&size)==NULL);
    }

    // AES-KW golden generated independently with the installed cryptography library.
    const u8 key[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    const u8 plain[16]={0,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
    const u8 wrapped[24]={0x1F,0xA6,0x8B,0x0A,0x81,0x12,0xB4,0x47,0xAE,0xF3,0x4B,0xD8,0xFB,0x5A,0x7B,0x82,0x9D,0x3E,0x86,0x23,0x71,0xD2,0xCF,0xE5};
    _Alignas(8) u8 encrypted[40],decoded[32];
    memset(encrypted,0xA5,sizeof(encrypted)); memset(decoded,0xA5,sizeof(decoded));
    CHECK(WifiSetup_AesKeyWrap(encrypted+8,plain,16,key,16)==1 && !memcmp(encrypted+8,wrapped,24));
    CHECK(WifiSetup_AesKeyUnwrap(decoded+8,encrypted+8,24,key,16)==1 && !memcmp(decoded+8,plain,16));
    for (u32 i=0;i<8;i++) CHECK(encrypted[i]==0xA5 && encrypted[i+32]==0xA5 && decoded[i]==0xA5 && decoded[i+24]==0xA5);
    encrypted[11]^=1; CHECK(WifiSetup_AesKeyUnwrap(decoded+8,encrypted+8,24,key,16)==0);
    return 0;
}

