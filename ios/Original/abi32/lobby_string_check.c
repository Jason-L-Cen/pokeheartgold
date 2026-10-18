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
// Valid-size allocator checks do not model the SDK's exception unwinder.
void _ZNSt15__vector_commonILb1EE18throw_length_errorEv(void) { abort(); }
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
char sStr_TreeInsertLengthError_3[]="tree insert length";
void *gLobbyRequestVtable;
u8 ov45_02255228[]={0,1,0};
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
typedef struct { u8 byte; } TestAllocator;
typedef struct { u32 *ptr; u32 size,cap; u32 *alloc; u32 start; } TestSplitBuffer;
void LobbyCharVector_InitRange(Vec *,char *,char *,TestAllocator);
void LobbyCharVector_AssignRange(Vec *,u8 *,u8 *,TestAllocator);
void LobbyU32VectorBuffer_Allocate(TestSplitBuffer *,u32,TestAllocator);
TestSplitBuffer *LobbyU32VectorBuffer_Destroy(TestSplitBuffer *);
void LobbyVipVector_Allocate(Vec *,u32,TestAllocator);
void LobbyVipVector_Reallocate(Vec *,TestAllocator,u32);
Vec *LobbyVipVector_Destroy(Vec *);

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

static u32 deletedRequests,deletedIds[8];
static void DeleteRequest(VObj *value) {
    if (deletedRequests>=8) abort();
    deletedIds[deletedRequests++]=((ObjKey *)value)->id;
    DWC_Free(0,value,0);
}
u32 CheckOriginalLobbyRequestMap(void) {
    static VTable table={NULL,DeleteRequest,NULL};
    u32 before=liveAllocations;
    Tree tree={0}; tree.head.right=&tree.head;
    ObjKey key={0}; ObjKey *pointer=&key; Iter found;
    LobbyRequestMap_Find(&found,&tree,&pointer);
    CHECK(found.current==&tree.head && !LobbyRequestMap_DeleteKey(&tree,0));
    const u32 ids[]={0x80000000,7,0xFFFFFFFF,0};
    Obj *values[4];
    for (u32 i=0;i<4;i++) {
        ObjKey *value=DWC_Alloc(0,sizeof(*value)); CHECK(value);
        value->vt=&table; value->id=ids[i]; value->param=i;
        values[i]=(Obj *)value; InsertResult inserted={0};
        LobbyRequestMap_Insert(&inserted,&tree,&values[i]);
        CHECK(inserted.inserted && inserted.node->key.obj==values[i] && tree.count==(int)i+1);
    }
    for (u32 i=0;i<4;i++) {
        key.id=ids[i]; LobbyRequestMap_Find(&found,&tree,&pointer);
        CHECK(found.current!=&tree.head && ((Node *)found.current)->key.obj==values[i]);
    }
    const u32 missing[]={1,6,8,0x7FFFFFFF,0x80000001};
    for (u32 i=0;i<sizeof(missing)/sizeof(missing[0]);i++) {
        key.id=missing[i]; LobbyRequestMap_Find(&found,&tree,&pointer);
        CHECK(found.current==&tree.head && !LobbyRequestMap_DeleteKey(&tree,missing[i]) && tree.count==4);
    }
    ObjKey *duplicate=DWC_Alloc(0,sizeof(*duplicate)); CHECK(duplicate);
    duplicate->vt=&table; duplicate->id=7; duplicate->param=99;
    Obj *duplicateObject=(Obj *)duplicate; InsertResult inserted={0};
    LobbyRequestMap_Insert(&inserted,&tree,&duplicateObject);
    CHECK(!inserted.inserted && inserted.node->key.obj==values[1] && tree.count==4);
    DWC_Free(0,duplicate,0);
    const u32 erase[]={0x80000000,0,0xFFFFFFFF,7}; deletedRequests=0;
    for (u32 i=0;i<4;i++) {
        CHECK(LobbyRequestMap_DeleteKey(&tree,erase[i]));
        CHECK(deletedRequests==i+1 && deletedIds[i]==erase[i] && tree.count==3-(int)i);
        key.id=erase[i]; LobbyRequestMap_Find(&found,&tree,&pointer);
        CHECK(found.current==&tree.head && !LobbyRequestMap_DeleteKey(&tree,erase[i]));
    }
    CHECK(tree.head.left==NULL && tree.head.right==&tree.head && liveAllocations==before);
    return 0;
}

typedef struct TestMap { int count; void *root; u32 unused; void *leftmost; } TestMap;
typedef struct TestNode { void *left,*right; u32 parent; } TestNode;
_Static_assert(sizeof(TestMap)==16 && sizeof(TestNode)==12,"Map test views");
static void *Leaf(TestMap *map,u32 size) {
    TestNode *node=DWC_Alloc(0,size); if (!node) abort();
    memset(node,0,size); node->parent=(u32)(uaddr)&map->root;
    map->count=1; map->root=node; map->leftmost=node;
    return node;
}
u32 CheckOriginalLobbyMapErase(void) {
    u32 before=liveAllocations;
    TestMap timer={0}; timer.leftmost=&timer.root;
    int timerKey=17, missing=18;
    CHECK(!LobbyTimerMap_EraseKey(&timer,&timerKey));
    u8 *node=Leaf(&timer,32); memcpy(node+12,&timerKey,4);
    CHECK(!LobbyTimerMap_EraseKey(&timer,&missing) && timer.count==1);
    CHECK(LobbyTimerMap_EraseKey(&timer,&timerKey) && timer.count==0 && timer.root==NULL && timer.leftmost==&timer.root);
    CHECK(!LobbyTimerMap_EraseKey(&timer,&timerKey) && liveAllocations==before);

    TestMap users={0}; users.leftmost=&users.root;
    int userKey=-17;
    CHECK(!LobbyUserMap_EraseKey((Tree *)&users,&userKey));
    node=Leaf(&users,28); memcpy(node+12,&userKey,4);
    LobbyString_InitFromCStr((Str *)(node+16),"a long user nickname");
    CHECK(!LobbyUserMap_EraseKey((Tree *)&users,&missing) && users.count==1);
    CHECK(LobbyUserMap_EraseKey((Tree *)&users,&userKey) && users.count==0 && users.root==NULL && users.leftmost==&users.root);
    CHECK(!LobbyUserMap_EraseKey((Tree *)&users,&userKey) && liveAllocations==before);

    TestMap channels={0}; channels.leftmost=&channels.root;
    Str channel={0},unknown={0};
    LobbyString_InitFromCStr(&channel,"an original long channel");
    LobbyString_InitFromCStr(&unknown,"another missing channel");
    CHECK(!LobbyChannelMap_EraseKey((Tree *)&channels,&channel));
    node=Leaf(&channels,72);
    LobbyString_InitFromCStr((Str *)(node+12),"an original long channel");
    void *member=DWC_Alloc(0,16); CHECK(member); memset(member,0,16);
    ((u32 *)member)[3]=42;
    u32 memberCount=1; memcpy(node+28,&memberCount,4);
    memcpy(node+32,&member,4); memcpy(node+36,&member,4);
    CHECK(!LobbyChannelMap_EraseKey((Tree *)&channels,&unknown) && channels.count==1);
    CHECK(LobbyChannelMap_EraseKey((Tree *)&channels,&channel) && channels.count==0 && channels.root==NULL && channels.leftmost==&channels.root);
    CHECK(!LobbyChannelMap_EraseKey((Tree *)&channels,&channel));
    LobbyString_Destroy(&channel); LobbyString_Destroy(&unknown);
    CHECK(liveAllocations==before);
    return 0;
}

u32 CheckOriginalLobbyAllocators(void) {
    u32 before=liveAllocations;
    TestAllocator allocator={0};
    u8 source[]={0,1,0x80,0xFF,7,8,9,10,11};
    Vec bytes={0};
    LobbyCharVector_InitRange(&bytes,(char *)source,(char *)source,allocator);
    CHECK(bytes.ptr==NULL && bytes.size==0 && bytes.cap==0);
    LobbyCharVector_InitRange(&bytes,(char *)source,(char *)source+sizeof(source),allocator);
    CHECK(bytes.size==sizeof(source) && bytes.cap>=bytes.size && !memcmp(bytes.ptr,source,sizeof(source)));
    LobbyCharVector_AssignRange(&bytes,(u8 *)bytes.ptr+1,(u8 *)bytes.ptr+5,allocator);
    CHECK(bytes.size==4 && !memcmp(bytes.ptr,source+1,4));
    u8 larger[37]; for (u32 i=0;i<sizeof(larger);i++) larger[i]=(u8)(i*31+5);
    LobbyCharVector_AssignRange(&bytes,larger,larger+sizeof(larger),allocator);
    CHECK(bytes.size==sizeof(larger) && bytes.cap>=bytes.size && !memcmp(bytes.ptr,larger,sizeof(larger)));
    LobbyCharVector_AssignRange(&bytes,larger,larger,allocator);
    CHECK(bytes.size==0 && LobbyCharVector_Destroy(&bytes)==&bytes && liveAllocations==before);

    for (u32 count=0;count<8;count++) {
        TestSplitBuffer buffer={0};
        LobbyU32VectorBuffer_Allocate(&buffer,count,allocator);
        CHECK(buffer.ptr && buffer.cap==count && buffer.size==0);
        for (u32 i=0;i<count;i++) buffer.ptr[i]=0x80000000+i;
        buffer.size=count;
        CHECK(LobbyU32VectorBuffer_Destroy(&buffer)==&buffer && buffer.size==0 && liveAllocations==before);
        Vec vip={0};
        LobbyVipVector_Allocate(&vip,count,allocator);
        CHECK(vip.ptr && vip.cap==count && vip.size==0);
        memset(vip.ptr,0xA5,count*8); vip.size=count;
        LobbyVipVector_Reallocate(&vip,allocator,count+3);
        CHECK(vip.ptr && vip.cap==count+3 && vip.size==0 && liveAllocations==before+1);
        vip.size=count+3;
        CHECK(LobbyVipVector_Destroy(&vip)==&vip && liveAllocations==before);
    }
    return 0;
}
