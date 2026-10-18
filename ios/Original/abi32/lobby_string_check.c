// SPDX-License-Identifier: GPL-3.0-or-later
#include "lobby_chat_internal.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(Str)==12 && sizeof(LobbyChatManager)==0xAC,"Lobby ABI32 layouts");
_Static_assert(offsetof(LobbyChatManager,channelPrefix)==0x48,"Lobby prefix offset");

// Probe-only memory hooks, disconnect observer and constructor table inputs.
static u32 liveAllocations, failAllocations, disconnectCalls;
static void *disconnectedConnection;
VoidFunc gLobbyMemOutOfMemoryHandler;
__typeof__(ov45_022577E8) ov45_022577E8;
void chatDisconnect(void *connection) { disconnectedConnection=connection; disconnectCalls++; }
void *DWC_Alloc(int heap, u32 size) {
    if (heap!=0) __builtin_trap();
    if (failAllocations) { failAllocations--; return NULL; }
    void *result=malloc(size);
    if (result) liveAllocations++;
    return result;
}
void DWC_Free(int heap, void *data, u32 size) {
    if (heap!=0 || size!=0) __builtin_trap();
    if (data) {
        if (!liveAllocations) __builtin_trap();
        liveAllocations--;
        free(data);
    }
}
char sStr_BasicStringReserveLengthError[]="reserve length";
char sStr_BasicStringLengthError[]="string length";
char sStr_BasicStringOutOfRange[]="string range";
char sStr_StringComparePosGtLen[]="compare position";
char sStr_GSP[]="gsp";
char sStr_Bang_2[]="!";
struct NameTable { const char *names[12]; } gLobbyChatPrefixNames={{
    "p0","p1","p2","p3","p4","p5","p6","p7","p8","p9","p10","p11"
}};
struct Triple { u32 v[3]; };
LobbyChatManager *LobbyChatManager_Init(LobbyChatManager *,const Str *,const Str *,int,u32,
                                      const Str *,const Str *,const Str *,const struct Triple *);
Str *LobbyString_ConcatCStr(Str *,const Str *,const char *);
Str *LobbyString_AssignRange(Str *,char *,char *);
Vec *LobbyCharVector_Destroy(Vec *);
Vec *LobbyU32Vector_Destroy(Vec *);

static int StringMatches(Str *value, const char *text) {
    u32 size=strlen(text);
    return StrSize(value)==size && !memcmp(StrData(value),text,size+1);
}

u32 CheckOriginalLobbyStrings(void) {
    u32 before=liveAllocations;
    Str left={0},right={0},joined={0},copy={0};
    CHECK(LobbyString_InitFromCStr(&left,"")==&left);
    CHECK(LobbyString_InitCopy(&copy,&left)==&copy && StringMatches(&copy,""));
    CHECK(LobbyString_Concat(&joined,&left,&copy)==&joined && StringMatches(&joined,""));
    LobbyString_Destroy(&joined); LobbyString_Destroy(&copy); LobbyString_Destroy(&left);
    CHECK(LobbyString_InitFromCStr(&left,"abc")==&left);
    CHECK(LobbyString_InitFromCStr(&right,"def")==&right);
    CHECK(LobbyString_Concat(&joined,&left,&right)==&joined && StringMatches(&joined,"abcdef"));
    CHECK(LobbyString_InitCopy(&copy,&joined)==&copy && StringMatches(&copy,"abcdef"));
    LobbyString_Destroy(&joined); LobbyString_Destroy(&copy);
    CHECK(LobbyString_ConcatCStr(&joined,&left,"-a-long-native-string")==&joined);
    CHECK(StringMatches(&joined,"abc-a-long-native-string") && joined.l.isLong);
    CHECK(LobbyString_InitCopy(&copy,&joined)==&copy && copy.l.isLong && copy.l.ptr!=joined.l.ptr);
    LobbyString_Destroy(&joined);
    CHECK(StringMatches(&copy,"abc-a-long-native-string"));
    char range[]="range";
    CHECK(LobbyString_AssignRange(&copy,range,range+5)==&copy && StringMatches(&copy,"range"));
    LobbyString_Destroy(&copy); LobbyString_Destroy(&right); LobbyString_Destroy(&left);
    CHECK(liveAllocations==before);

    const char *names[]={"heartgold-native","secret","Trainer","native-user","A native trainer"};
    Str inputs[5]={{0}};
    for (u32 i=0;i<5;i++) CHECK(LobbyString_InitFromCStr(&inputs[i],names[i])==&inputs[i]);
    const struct Triple config={{7,11,13}};
    for (int prefix=0;prefix<12;prefix++) {
        LobbyChatManager manager;
        memset(&manager,0xA5,sizeof(manager));
        CHECK(LobbyChatManager_Init(&manager,&inputs[0],&inputs[1],prefix,0xAABBCCDD,
                                   &inputs[2],&inputs[3],&inputs[4],&config)==&manager);
        CHECK(manager.connection==NULL && manager.state==0 && (u32)manager.userId==0xAABBCCDD);
        CHECK(StringMatches(&manager.gameName,names[0]) && StringMatches(&manager.gameKey,names[1]));
        CHECK(StringMatches(&manager.nick,names[2]) && StringMatches(&manager.user,names[3]));
        CHECK(StringMatches(&manager.realName,names[4]) && !memcmp(manager.unk64,&config,sizeof(config)));
        char expected[80];
        snprintf(expected,sizeof(expected),"gsp!%s!%s!",names[0],gLobbyChatPrefixNames.names[prefix]);
        CHECK(StringMatches(&manager.channelPrefix,expected));
        Str channel={0},fullName={0};
        LobbyString_InitFromCStr(&channel,"room");
        CHECK(LobbyChat_GetFullChannelName(&fullName,&manager,&channel)==&fullName);
        snprintf(expected,sizeof(expected),"gsp!%s!%s!room",names[0],gLobbyChatPrefixNames.names[prefix]);
        CHECK(StringMatches(&fullName,expected) && StringMatches(&channel,"room"));
        LobbyString_Destroy(&fullName); LobbyString_Destroy(&channel);
        LobbyString_Destroy(&manager.nick); LobbyString_Destroy(&manager.user);
        LobbyString_Destroy(&manager.realName); LobbyString_Destroy(&manager.gameName);
        LobbyString_Destroy(&manager.gameKey); LobbyString_Destroy(&manager.channelPrefix);
    }
    for (u32 i=0;i<5;i++) LobbyString_Destroy(&inputs[i]);
    CHECK(liveAllocations==before);
    return 0;
}

u32 CheckOriginalLobbyComparison(void) {
    u32 before=liveAllocations;
    const struct { const char *first,*second,*lowerFirst,*lowerSecond; u32 length; int sign; } cases[]={
        {"HeLLoTail","hELLo","hellotail","hello",5,0},
        {"HeLLoTail","Hello","hellotail","hello",0xFFFFFFFF,1},
        {"A","B","a","b",1,-1},{"Z","a","z","a",1,1},
        {"A","B","a","b",0,-1},{"UNUSED","","unused","",0,0},
        {"ALongMixedCaseNativeString","alongmixedcasenativestring","alongmixedcasenativestring","alongmixedcasenativestring",0xFFFFFFFF,0},
        {"\xC0" "A","\xC0" "a","\xC0" "a","\xC0" "a",2,0}
    };
    for (u32 i=0;i<sizeof(cases)/sizeof(cases[0]);i++) {
        Str first={0},second={0};
        LobbyString_InitFromCStr(&first,cases[i].first); LobbyString_InitFromCStr(&second,cases[i].second);
        int result=LobbyString_CompareNoCase(&first,&second,cases[i].length);
        CHECK((result>0)-(result<0)==cases[i].sign);
        CHECK(StringMatches(&first,cases[i].lowerFirst) && StringMatches(&second,cases[i].lowerSecond));
        LobbyString_Destroy(&first); LobbyString_Destroy(&second);
    }
    CHECK(liveAllocations==before);
    return 0;
}

u32 CheckOriginalLobbyCleanup(void) {
    u32 before=liveAllocations;
    Vec bytes={DWC_Alloc(0,37),37,37}, words={DWC_Alloc(0,12),3,3};
    CHECK(bytes.ptr && words.ptr);
    memset(bytes.ptr,0xA5,37); memset(words.ptr,0xA5,12);
    CHECK(LobbyCharVector_Destroy(&bytes)==&bytes && LobbyU32Vector_Destroy(&words)==&words);
    Vec strings={DWC_Alloc(0,3*sizeof(Str)),3,3};
    CHECK(strings.ptr); memset(strings.ptr,0,3*sizeof(Str));
    const char *text[]={"short","one original long string","another original long string"};
    for (u32 i=0;i<3;i++) LobbyString_InitFromCStr((Str *)strings.ptr+i,text[i]);
    CHECK(LobbyStringVector_Destroy(&strings)==&strings && liveAllocations==before);
    Vec empty={0};
    CHECK(LobbyCharVector_Destroy(&empty)==&empty && LobbyU32Vector_Destroy(&empty)==&empty);
    CHECK(LobbyStringVector_Destroy(&empty)==&empty && liveAllocations==before);

    LobbyChatManager manager={0}; manager.connection=(void *)0x1234;
    failAllocations=1;
    CHECK(!LobbyMem_InstallOutOfMemoryHandler(&manager));
    CHECK(ov45_022577E8.manager==&manager && ov45_022577E8.buf==NULL && gLobbyMemOutOfMemoryHandler==NULL);
    CHECK(liveAllocations==before && !failAllocations);
    CHECK(LobbyMem_InstallOutOfMemoryHandler(&manager) && ov45_022577E8.manager==&manager);
    CHECK(ov45_022577E8.buf!=NULL && gLobbyMemOutOfMemoryHandler!=NULL && liveAllocations==before+1);
    gLobbyMemOutOfMemoryHandler();
    CHECK(ov45_022577E8.buf==NULL && manager.state==LOBBY_CHAT_STATE_ERROR && manager.lastError==2);
    CHECK(gLobbyMemOutOfMemoryHandler!=NULL && liveAllocations==before);
    disconnectCalls=0; disconnectedConnection=NULL;
    gLobbyMemOutOfMemoryHandler();
    CHECK(disconnectCalls==1 && disconnectedConnection==manager.connection && gLobbyMemOutOfMemoryHandler==NULL);
    ov45_022577E8.manager=NULL;
    CHECK(liveAllocations==before);
    return 0;
}
