// SPDX-License-Identifier: GPL-3.0-or-later
#define BattleSystem_GetSendBufferPtr ItemReplySendBuffer
#define BattleSystem_GetRecvBufferPtr ItemReplyRecvBuffer
#define BattleSystem_GetSendBufferReadPos ItemReplySendRead
#define BattleSystem_GetSendBufferWritePos ItemReplySendWrite
#define BattleSystem_GetSendBufferEndPos ItemReplySendEnd
#define BattleSystem_GetRecvBufferReadPos ItemReplyRecvRead
#define BattleSystem_GetRecvBufferWritePos ItemReplyRecvWrite
#define BattleSystem_GetRecvBufferEndPos ItemReplyRecvEnd
#define BattleSystem_IsServer ItemReplyIsServer
#define Comm_CountConnected ItemReplyConnected
#define BattleContext_AddLinkBufferEntry ItemReplyAddLink
#define BattleContext_RemoveLinkBufferEntry ItemReplyRemoveLink
#define BattleLink_QueueSendData ItemReplyLinkSend
#include "../../../src/battle/battle_controller.c"
#include "constants/items.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

_Static_assert(sizeof(BattleItemInput)==4,"Original item reply is four bytes");
_Static_assert(offsetof(BattleItemInput,item)==0 && offsetof(BattleItemInput,pocket)==2 && offsetof(BattleItemInput,target)==3,"Original item reply fields");
static BOOL unexpectedRoute;

// Observe platform routing; the packet serializer, reply API and queue consumer above are real game code.
u8 *ItemReplySendBuffer(BattleSystem *system) { return system->sendBuffer; }
u8 *ItemReplyRecvBuffer(BattleSystem *system) { return system->recvBuffer; }
u16 *ItemReplySendRead(BattleSystem *system) { return &system->sendReadPos; }
u16 *ItemReplySendWrite(BattleSystem *system) { return &system->sendWritePos; }
u16 *ItemReplySendEnd(BattleSystem *system) { return &system->sendEndPos; }
u16 *ItemReplyRecvRead(BattleSystem *system) { return &system->recvReadPos; }
u16 *ItemReplyRecvWrite(BattleSystem *system) { return &system->recvWritePos; }
u16 *ItemReplyRecvEnd(BattleSystem *system) { return &system->recvEndPos; }
u8 ItemReplyIsServer(BattleSystem *system) { unexpectedRoute=TRUE; return FALSE; }
int ItemReplyConnected(void) { unexpectedRoute=TRUE; return 0; }
void ItemReplyAddLink(BattleContext *ctx,int id,int battler,int command) { unexpectedRoute=TRUE; }
void ItemReplyRemoveLink(BattleContext *ctx,int id,int battler,int command) { unexpectedRoute=TRUE; }
void ItemReplyLinkSend(BattleSystem *system,int direction,int battler,u8 *data,u8 size) { unexpectedRoute=TRUE; }

static u32 CheckReply(BattleItemInput input,const u8 expected[4],int battler) {
    static BattleSystem system;
    static BattleContext context;
    memset(&system,0,sizeof(system));memset(&context,0,sizeof(context));
    system.ctx=&context;unexpectedRoute=FALSE;
    memset(context.battleBuffer,0xA5,sizeof(context.battleBuffer));
    for (int i=0;i<BATTLER_MAX;i++) context.battleBuffer[i][0]=0;
    BattleController_ReplyItemSelection(&system,battler,input);
    CHECK(!unexpectedRoute && system.sendWritePos==8 && !system.sendReadPos && !system.sendEndPos);
    const u8 header[]={0,(u8)battler,4,0};
    CHECK(!memcmp(system.sendBuffer,header,4) && !memcmp(system.sendBuffer+4,expected,4));
    context.battleBuffer[battler][0]=0x99;
    BattleController_ProcessQueue(&system,0);
    CHECK(!system.sendReadPos && context.battleBuffer[battler][0]==0x99 && context.battleBuffer[battler][1]==0xA5);
    context.battleBuffer[battler][0]=0;
    BattleController_ProcessQueue(&system,0);
    CHECK(!unexpectedRoute && system.sendReadPos==system.sendWritePos);
    CHECK(!memcmp(context.battleBuffer[battler],expected,4) && context.battleBuffer[battler][4]==0xA5);
    for (int i=0;i<BATTLER_MAX;i++) if (i!=battler) {
        CHECK(context.battleBuffer[i][0]==0 && context.battleBuffer[i][1]==0xA5);
    }
    return 0;
}

u32 CheckOriginalItemReply(void) {
    const BattleItemInput inputs[]={
        {.item=ITEM_POTION,.pocket=BTLPOCKETLIST_HP_PP_RESTORE,.target=2},
        {.item=0x1234,.pocket=0x56,.target=0x78},
        // SSI_STATE_8 treats the first byte FF as cancel; empty item zero is distinct.
        {.item=0xFF,.pocket=4,.target=0x98},
        {.item=ITEM_NONE,.pocket=4,.target=0},
        {.item=0xFFFF,.pocket=0xFF,.target=0xFF},
    };
    const u8 bytes[][4]={{0x11,0,0,2},{0x34,0x12,0x56,0x78},{0xFF,0,4,0x98},{0,0,4,0},{0xFF,0xFF,0xFF,0xFF}};
    for (int battler=0;battler<BATTLER_MAX;battler++) for (u32 i=0;i<NELEMS(inputs);i++) {
        u32 line=CheckReply(inputs[i],bytes[i],battler);if (line) return line;
    }
    return 0;
}
#ifndef HEARTGOLD_ABI32
int main(void) { u32 line = CheckOriginalItemReply(); if (line) { fprintf(stderr,"Item reply check line %u\n",line); return 1; } puts("PASS: typed original item replies, four-byte packets, cancel/empty/raw fields and busy-queue preservation."); return 0; }
#endif
