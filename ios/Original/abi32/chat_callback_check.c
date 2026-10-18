// SPDX-License-Identifier: GPL-3.0-or-later
#include "global.h"
#include <dwc/chat_internal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
// Probe assertion labels and join observer; this does not create a joined channel.
#define ASSERT_LABEL(name) const char name[]=#name
ASSERT_LABEL(sStr_DataNeNULL); ASSERT_LABEL(sStr_ChatCallbacksC); ASSERT_LABEL(sStr_DataTypeGe0);
ASSERT_LABEL(sStr_DataTypeLtCALLBACK_NUM); ASSERT_LABEL(sStr_DataCallbackNeNULL);
ASSERT_LABEL(sStr_DataCallbackParamsNeNULL); ASSERT_LABEL(sStr_DataID_Ge0);
ASSERT_LABEL(sStr_ciFreeCallbackData); ASSERT_LABEL(sStr_ciCallCallback); ASSERT_LABEL(sStr_ChatNeNULL); ASSERT_LABEL(sStr_0_8);
void __msl_assertion_failed(const char *expression,const char *file,const char *function,int line) {
    fprintf(stderr,"%s:%d %s: %s\n",file,line,function,expression); abort();
}
static u32 joinCalls;
static ChatConnection *joinedChat;
static char joinedName[32];
void ciJoinCallbackCalled(ChatConnection *chat,const char *name) {
    joinCalls++; joinedChat=chat; snprintf(joinedName,sizeof(joinedName),"%s",name);
}
static u32 live,observedCount,observed[6];
static void *observedChat,*observedUser;
static void *Allocate(u32 size) { void *value=calloc(1,size); if (!value) abort(); live++; return value; }
static u32 Text(const char *text) { char *value=Allocate(strlen(text)+1); strcpy(value,text); return (u32)(uaddr)value; }
static u32 TextList(u32 count) {
    if (!count) return 0;
    u32 *values=Allocate(count*sizeof(u32));
    for (u32 i=0;i<count;i++) values[i]=Text(i ? "second" : "first");
    return (u32)(uaddr)values;
}
void DWCi_GsFree(void *value) { if (value) { if (!live) abort(); live--; free(value); } }
static void Observe1(void *chat,u32 a,void *user) {
    observedChat=chat; observedUser=user; observedCount=1; observed[0]=a;
}
static void Observe2(void *chat,u32 a,u32 b,void *user) {
    observedChat=chat; observedUser=user; observedCount=2; observed[0]=a; observed[1]=b;
}
static void Observe3(void *chat,u32 a,u32 b,u32 c,void *user) {
    observedChat=chat; observedUser=user; observedCount=3; observed[0]=a; observed[1]=b; observed[2]=c;
}
static void Observe4(void *chat,u32 a,u32 b,u32 c,u32 d,void *user) {
    observedChat=chat; observedUser=user; observedCount=4; observed[0]=a; observed[1]=b; observed[2]=c; observed[3]=d;
}
static void Observe5(void *chat,u32 a,u32 b,u32 c,u32 d,u32 e,void *user) {
    observedChat=chat; observedUser=user; observedCount=5; observed[0]=a; observed[1]=b; observed[2]=c; observed[3]=d; observed[4]=e;
}
static void Observe6(void *chat,u32 a,u32 b,u32 c,u32 d,u32 e,u32 f,void *user) {
    observedChat=chat; observedUser=user; observedCount=6; observed[0]=a; observed[1]=b; observed[2]=c; observed[3]=d; observed[4]=e; observed[5]=f;
}
u32 CheckOriginalChatDispatch(void) {
    const struct { int type; u32 words; void *callback; u32 count; int withValues; } cases[]={
        {CALLBACK_RAW,1,(void *)Observe1},{CALLBACK_DISCONNECTED,1,(void *)Observe1},
        {CALLBACK_PRIVATE_MESSAGE,3,(void *)Observe3},{CALLBACK_INVITED,2,(void *)Observe2},
        {CALLBACK_NICK_ERROR,4,(void *)Observe4},
        {CALLBACK_CHANNEL_MESSAGE,4,(void *)Observe4},{CALLBACK_KICKED,3,(void *)Observe3},
        {CALLBACK_USER_JOINED,3,(void *)Observe3},{CALLBACK_USER_PARTED,5,(void *)Observe5},
        {CALLBACK_USER_CHANGED_NICK,3,(void *)Observe3},{CALLBACK_TOPIC_CHANGED,2,(void *)Observe2},
        {CALLBACK_CHANNEL_MODE_CHANGED,2,(void *)Observe2},{CALLBACK_USER_MODE_CHANGED,3,(void *)Observe3},
        {CALLBACK_USER_LIST_UPDATED,1,(void *)Observe1},{CALLBACK_BROADCAST_KEY_CHANGED,4,(void *)Observe4},
        {CALLBACK_ENTER_CHANNEL,3,(void *)Observe3},
        {CALLBACK_ENUM_CHANNELS_EACH,5,(void *)Observe5},
        {CALLBACK_ENUM_CHANNELS_ALL,5,(void *)Observe5,0},{CALLBACK_ENUM_CHANNELS_ALL,5,(void *)Observe5,2},
        {CALLBACK_ENUM_USERS,5,(void *)Observe5,0},{CALLBACK_ENUM_USERS,5,(void *)Observe5,2},
        {CALLBACK_GET_CHANNEL_MODE,3,(void *)Observe3},
        {CALLBACK_GET_CHANNEL_KEYS,6,(void *)Observe6,0,0},
        {CALLBACK_GET_CHANNEL_KEYS,6,(void *)Observe6,2,0},{CALLBACK_GET_CHANNEL_KEYS,6,(void *)Observe6,2,1}
    };
    ChatConnection chat={0};
    for (u32 i=0;i<sizeof(cases)/sizeof(cases[0]);i++) {
        u32 before=live,*args=Allocate(6*sizeof(u32));
        if (cases[i].type==CALLBACK_NICK_ERROR) {
            args[0]=0xAABBCCDD; args[1]=Text("nick"); args[2]=2;
            u32 *names=Allocate(2*sizeof(u32)); names[0]=Text("first"); names[1]=Text("second"); args[3]=(u32)(uaddr)names;
        } else if (cases[i].type==CALLBACK_ENTER_CHANNEL) {
            args[0]=1; args[1]=0x89ABCDEF; args[2]=Text("room");
        } else if (cases[i].type==CALLBACK_ENUM_CHANNELS_EACH) {
            args[0]=1; args[1]=0x89ABCDEF; args[2]=Text("room"); args[3]=Text("topic"); args[4]=0xAABBCCDD;
        } else if (cases[i].type==CALLBACK_ENUM_CHANNELS_ALL) {
            args[0]=1; args[1]=cases[i].count;
            args[2]=TextList(cases[i].count); args[3]=TextList(cases[i].count);
            args[4]=cases[i].count ? (u32)(uaddr)Allocate(cases[i].count*sizeof(u32)) : 0;
        } else if (cases[i].type==CALLBACK_ENUM_USERS) {
            args[0]=1; args[1]=Text("room"); args[2]=cases[i].count;
            args[3]=TextList(cases[i].count);
            args[4]=cases[i].count ? (u32)(uaddr)Allocate(cases[i].count*sizeof(u32)) : 0;
        } else if (cases[i].type==CALLBACK_GET_CHANNEL_MODE) {
            args[0]=1; args[1]=Text("room"); args[2]=(u32)(uaddr)Allocate(32);
        } else if (cases[i].type==CALLBACK_GET_CHANNEL_KEYS) {
            args[0]=1; args[1]=Text("room"); args[2]=Text("nick"); args[3]=cases[i].count;
            args[4]=TextList(cases[i].count); args[5]=cases[i].withValues ? TextList(cases[i].count) : 0;
        } else {
            args[0]=Text("alpha");
            if (cases[i].words>1) args[1]=Text("beta");
            if (cases[i].words>2) args[2]=0x89ABCDEF;
            if (cases[i].type==CALLBACK_CHANNEL_MESSAGE || cases[i].type==CALLBACK_KICKED ||
                cases[i].type==CALLBACK_USER_CHANGED_NICK || cases[i].type==CALLBACK_BROADCAST_KEY_CHANGED) args[2]=Text("third");
            if (cases[i].type==CALLBACK_CHANNEL_MESSAGE) args[3]=0xAABBCCDD;
            if (cases[i].type==CALLBACK_BROADCAST_KEY_CHANGED || cases[i].type==CALLBACK_USER_PARTED) args[3]=Text("fourth");
            if (cases[i].type==CALLBACK_USER_PARTED) args[4]=Text("fifth");
        }
        u32 expected[6]; memcpy(expected,args,sizeof(expected));
        ChatCallback callback={cases[i].type,cases[i].callback,args,(void *)0xCAFEBABE,0,NULL};
        observedCount=0; observedChat=observedUser=NULL;
        joinCalls=0; joinedChat=NULL; joinedName[0]=0;
        ciCallCallback(&chat,&callback);
        CHECK(observedChat==&chat && observedUser==(void *)0xCAFEBABE && observedCount==cases[i].words);
        CHECK(!memcmp(observed,expected,cases[i].words*sizeof(u32)) && live==before);
        if (cases[i].type==CALLBACK_ENTER_CHANNEL) CHECK(joinCalls==1 && joinedChat==&chat && !strcmp(joinedName,"room"));
        else CHECK(joinCalls==0);
    }
    return 0;
}
