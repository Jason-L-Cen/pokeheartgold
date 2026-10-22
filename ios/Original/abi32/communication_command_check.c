// SPDX-License-Identifier: GPL-3.0-or-later
// Observe only external session boundaries; the private table and dispatcher below are original code.
#define Comm_SetLinkError CommandCheck_LinkError
#define Comm_GetLocalNetId CommandCheck_LocalNetId
#define Comm_IsNetIdConnected CommandCheck_IsConnected
#define Comm_SendParentCommand CommandCheck_SendParent
#define Comm_SendFixedCommand CommandCheck_SendFixed
#define Comm_RecvShutdown CommandCheck_RecvShutdown
#define CommPlayerInfo_RecvPlayerEntry CommandCheck_RecvPlayerEntry
#define CommPlayerInfo_GetEntrySize CommandCheck_EntrySize
#define CommPlayerInfo_RecvEntry CommandCheck_RecvEntry
#define CommPlayerInfo_RecvSetUnk3A4 CommandCheck_RecvPlayerFlag
#define CommState_RecvJoinRequest CommandCheck_RecvJoinRequest
#define CommState_RecvJoinReply CommandCheck_RecvJoinReply
#define CommState_GetJoinMessageSize CommandCheck_JoinSize
#define Comm_RecvTransferModeRequest CommandCheck_RecvModeRequest
#define Comm_RecvTransferModeChange CommandCheck_RecvModeChange
#define Comm_RecvTransferModeAck CommandCheck_RecvModeAck
#define LinkSync_RecvRequest CommandCheck_RecvSyncRequest
#define LinkSync_RecvDone CommandCheck_RecvSyncDone
#define LinkSync_RecvState CommandCheck_RecvSyncState
#define LinkSync_RecvValue CommandCheck_RecvSyncValue
#define LinkSync_GetValueSize CommandCheck_ValueSize
#define LinkSync_RecvPacket CommandCheck_RecvSyncPacket
#define CommState_GetSyncPacketSize CommandCheck_PacketSize
#define CommState_RecvWifiDisconnect CommandCheck_RecvWifiDisconnect
#include "../../../src/communication_command.c"

typedef struct CommandObservation {
    u32 calls;
    int command,netId,size;
    void *data,*context;
} CommandObservation;
static CommandObservation baseReceived,appReceived,sent;
static u32 commandErrors,failureLine,connectedMask,sizeCalls[4],payload;
static int localNetId;
#define CHECK(value) do { if (!(value)) { failureLine=__LINE__; return FALSE; } } while (0)
u32 HGAotCommandFailureLine(void) { return failureLine; }

void CommandCheck_LinkError(void) { commandErrors++; }
int CommandCheck_LocalNetId(void) { return localNetId; }
int CommandCheck_IsConnected(u16 netId) { return netId<8 && (connectedMask&(1u<<netId)); }
BOOL CommandCheck_SendParent(int command,void *data,int size) {
    sent.calls++; sent.command=command; sent.data=data; sent.size=size;
    return TRUE;
}
BOOL CommandCheck_SendFixed(int command,void *data) {
    sent.calls++; sent.command=command; sent.data=data; sent.size=-1;
    return TRUE;
}
static void RecordBase(int command,int netId,int size,void *data,void *context) {
    baseReceived.calls++; baseReceived.command=command; baseReceived.netId=netId;
    baseReceived.size=size; baseReceived.data=data; baseReceived.context=context;
}
#define BASE_OBSERVER(name,command) \
    void name(int netId,int size,void *data,void *context) { RecordBase(command,netId,size,data,context); }
BASE_OBSERVER(CommandCheck_RecvShutdown,2)
BASE_OBSERVER(CommandCheck_RecvPlayerEntry,3)
BASE_OBSERVER(CommandCheck_RecvEntry,4)
BASE_OBSERVER(CommandCheck_RecvPlayerFlag,5)
BASE_OBSERVER(CommandCheck_RecvJoinRequest,6)
BASE_OBSERVER(CommandCheck_RecvJoinReply,7)
BASE_OBSERVER(CommandCheck_RecvModeRequest,10)
BASE_OBSERVER(CommandCheck_RecvModeChange,11)
BASE_OBSERVER(CommandCheck_RecvModeAck,12)
BASE_OBSERVER(CommandCheck_RecvSyncRequest,16)
BASE_OBSERVER(CommandCheck_RecvSyncDone,17)
BASE_OBSERVER(CommandCheck_RecvSyncState,18)
BASE_OBSERVER(CommandCheck_RecvSyncValue,19)
BASE_OBSERVER(CommandCheck_RecvSyncPacket,20)
BASE_OBSERVER(CommandCheck_RecvWifiDisconnect,21)
u32 CommandCheck_EntrySize(void) { sizeCalls[0]++; return 37; }
u32 CommandCheck_JoinSize(void) { sizeCalls[1]++; return 13; }
u32 CommandCheck_ValueSize(void) { sizeCalls[2]++; return 9; }
u32 CommandCheck_PacketSize(void) { sizeCalls[3]++; return 31; }
static void ObserveCommand(int netId,int size,void *data,void *context) {
    appReceived.calls++; appReceived.netId=netId; appReceived.size=size;
    appReceived.data=data; appReceived.context=context;
}
static u32 CommandSize(void) { return 7; }
static void *CommandBuffer(int netId,void *context,int size) {
    appReceived.netId=netId; appReceived.size=size; appReceived.context=context;
    return &payload;
}
static BOOL RejectCommand(int command) {
    u32 errors=commandErrors,appCalls=appReceived.calls,baseCalls=baseReceived.calls;
    Comm_CallCommandRecv(-1,command,-9,NULL);
    CHECK(Comm_GetCommandSize(command)==0 && !Comm_CommandHasBuffer(command));
    CHECK(Comm_GetCommandBuffer(command,0x7FFFFFFF,-9)==NULL);
    CHECK(commandErrors==errors+4 && appReceived.calls==appCalls && baseReceived.calls==baseCalls);
    return TRUE;
}

u32 HGAotCommandDispatch(void) {
    static const CommCommand commands[]={{ObserveCommand,CommandSize,CommandBuffer},{NULL,NULL,NULL}};
    const int invalid[]={-1,(-2147483647-1),24,0x7FFFFFFF};
    const int nullReceivers[]={0,1,8,9};
    const int externalReceivers[]={2,3,4,5,6,7,10,11,12,16,17,18,19,20,21};
    const u32 sizes[22]={0,0,0,37,37,0,13,13,0,0,1,1,1,0,0,0,1,1,2,9,31,0};
    u32 freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_15),context=0x12345678;
    failureLine=commandErrors=0; localNetId=0; connectedMask=0;
    baseReceived=(CommandObservation){0}; appReceived=baseReceived; sent=baseReceived;
    for (u32 i=0;i<4;i++) sizeCalls[i]=0;
    CHECK(sCommCommandTable==NULL && COMM_BASE_COMMAND_COUNT==22);
    // This must not silently become the legacy fixture's synthetic base0/base21 table.
    CHECK(sCommBaseCommands[0].recv==NULL && sCommBaseCommands[21].recv==CommandCheck_RecvWifiDisconnect);
    for (u32 i=0;i<sizeof(nullReceivers)/sizeof(nullReceivers[0]);i++) Comm_CallCommandRecv(3,nullReceivers[i],4,&payload);
    CHECK(!baseReceived.calls && !appReceived.calls && !commandErrors);
    for (u32 i=0;i<22;i++) {
        CHECK(Comm_GetCommandSize(i)==sizes[i] && !Comm_CommandHasBuffer(i));
        CHECK(Comm_GetCommandBuffer(i,5,11)==NULL);
    }
    CHECK(sizeCalls[0]==2 && sizeCalls[1]==2 && sizeCalls[2]==1 && sizeCalls[3]==1);
    for (u32 i=0;i<sizeof(externalReceivers)/sizeof(externalReceivers[0]);i++) {
        u32 calls=baseReceived.calls;
        Comm_CallCommandRecv(7,externalReceivers[i],8,&payload);
        CHECK(baseReceived.calls==calls+1 && baseReceived.command==externalReceivers[i]);
        CHECK(baseReceived.netId==7 && baseReceived.size==8 && baseReceived.data==&payload && !baseReceived.context);
    }
    CHECK(RejectCommand(22));
    Comm_RegisterCommandTable(commands,2,&context);
    CommCommandTable *allocation=sCommCommandTable;
    CHECK(allocation && allocation->commands==commands && allocation->count==2 && allocation->context==&context && !allocation->shutdown);
    for (u32 i=0;i<8;i++) CHECK(allocation->ready[i]==0);
    Comm_CallCommandRecv(6,22,12,&payload);
    CHECK(appReceived.calls==1 && appReceived.netId==6 && appReceived.size==12 && appReceived.data==&payload && appReceived.context==&context);
    CHECK(Comm_GetCommandSize(22)==7 && Comm_CommandHasBuffer(22));
    CHECK(Comm_GetCommandBuffer(22,4,13)==&payload && appReceived.netId==4 && appReceived.size==13 && appReceived.context==&context);
    // Sender/size are transport-owned integers; dispatch forwards them without narrowing.
    Comm_CallCommandRecv(-1,22,-1,NULL);
    CHECK(appReceived.netId==-1 && appReceived.size==-1 && appReceived.data==NULL);
    Comm_CallCommandRecv(0x7FFFFFFF,22,0x7FFFFFFF,&payload);
    CHECK(appReceived.calls==3 && appReceived.netId==0x7FFFFFFF && appReceived.size==0x7FFFFFFF);
    Comm_CallCommandRecv(0,23,0,NULL);
    CHECK(appReceived.calls==3 && Comm_GetCommandSize(23)==0 && !Comm_CommandHasBuffer(23) && !Comm_GetCommandBuffer(23,0,0));
    Comm_CallCommandRecv(4,21,3,&payload);
    CHECK(baseReceived.command==21 && baseReceived.netId==4 && baseReceived.size==3 && baseReceived.context==&context);
    for (u32 i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++) CHECK(RejectCommand(invalid[i]));
    CHECK(commandErrors==20 && appReceived.calls==3);

    // The private ready/shutdown handlers retain their real table/session state transitions.
    connectedMask=(1u<<3)|(1u<<4);
    Comm_CallCommandRecv(3,13,0,NULL);
    CHECK(allocation->ready[3] && !allocation->ready[4] && !sent.calls);
    Comm_CallCommandRecv(4,13,0,NULL);
    CHECK(allocation->ready[4] && sent.calls==1 && sent.command==14 && !sent.data && !sent.size);
    Comm_CallCommandRecv(4,15,0,NULL);
    CHECK(!allocation->ready[4]);
    localNetId=1;
    Comm_CallCommandRecv(4,13,0,NULL);
    CHECK(!allocation->ready[4] && sent.calls==1);
    allocation->ready[4]=1; Comm_CallCommandRecv(4,15,0,NULL);
    CHECK(allocation->ready[4]); localNetId=0;
    Comm_CallCommandRecv(0,14,0,&payload);
    CHECK(allocation->commands==NULL && !allocation->count && !allocation->context && allocation->shutdown);
    CHECK(sent.calls==2 && sent.command==15 && sent.data==&payload && sent.size==-1);
    CHECK(RejectCommand(22));

    Comm_RegisterCommandTable(NULL,1,&context); CHECK(RejectCommand(22));
    const int invalidCounts[]={0,-1,(-2147483647-1)};
    for (u32 i=0;i<sizeof(invalidCounts)/sizeof(invalidCounts[0]);i++) {
        Comm_RegisterCommandTable(commands,invalidCounts[i],&context); CHECK(RejectCommand(22));
    }
    Comm_RegisterCommandTable(commands,1,NULL);
    CHECK(sCommCommandTable==allocation && !allocation->shutdown);
    for (u32 i=0;i<8;i++) CHECK(allocation->ready[i]==0);
    CHECK(RejectCommand(23));
    Comm_CallCommandRecv(2,22,6,&payload);
    CHECK(appReceived.calls==4 && appReceived.context==NULL);
    CHECK(Comm_GetCommandBuffer(22,2,6)==&payload && appReceived.context==NULL);
    Comm_FreeCommandTable();
    CHECK(sCommCommandTable==NULL && HeapExp_FndGetTotalFreeSize(HEAP_ID_15)==freeBefore);
    Comm_FreeCommandTable();
    CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_15)==freeBefore && RejectCommand(22));
    CHECK(commandErrors==48 && appReceived.calls==4);
    return TRUE;
}
