// SPDX-License-Identifier: GPL-3.0-or-later
#include "global.h"
#include <dwc/chat_internal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
// Probe assertion inputs; assertions and an unexpected channel join remain fatal.
#define ASSERT_LABEL(name) const char name[]=#name
ASSERT_LABEL(sStr_DataNeNULL); ASSERT_LABEL(sStr_ChatCallbacksC); ASSERT_LABEL(sStr_DataTypeGe0);
ASSERT_LABEL(sStr_DataTypeLtCALLBACK_NUM); ASSERT_LABEL(sStr_DataCallbackNeNULL);
ASSERT_LABEL(sStr_DataCallbackParamsNeNULL); ASSERT_LABEL(sStr_DataID_Ge0);
ASSERT_LABEL(sStr_ciFreeCallbackData); ASSERT_LABEL(sStr_ciCallCallback); ASSERT_LABEL(sStr_ChatNeNULL); ASSERT_LABEL(sStr_0_8);
void __msl_assertion_failed(const char *expression,const char *file,const char *function,int line) {
    fprintf(stderr,"%s:%d %s: %s\n",file,line,function,expression); abort();
}
void ciJoinCallbackCalled(ChatConnection *chat,const char *name) { (void)chat; (void)name; abort(); }
static u32 live,observedCount,observed[4];
static void *observedChat,*observedUser;
static void *Allocate(u32 size) { void *value=malloc(size); if (!value) abort(); live++; return value; }
static u32 Text(const char *text) { char *value=Allocate(strlen(text)+1); strcpy(value,text); return (u32)(uaddr)value; }
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
u32 CheckOriginalChatDispatch(void) {
    const struct { int type; u32 words; void *callback; } cases[]={
        {CALLBACK_RAW,1,(void *)Observe1},{CALLBACK_DISCONNECTED,1,(void *)Observe1},
        {CALLBACK_PRIVATE_MESSAGE,3,(void *)Observe3},{CALLBACK_INVITED,2,(void *)Observe2},
        {CALLBACK_NICK_ERROR,4,(void *)Observe4}
    };
    ChatConnection chat={0};
    for (u32 i=0;i<sizeof(cases)/sizeof(cases[0]);i++) {
        u32 before=live,*args=Allocate(4*sizeof(u32)); memset(args,0,4*sizeof(u32));
        if (cases[i].type==CALLBACK_NICK_ERROR) {
            args[0]=0xAABBCCDD; args[1]=Text("nick"); args[2]=2;
            u32 *names=Allocate(2*sizeof(u32)); names[0]=Text("first"); names[1]=Text("second"); args[3]=(u32)(uaddr)names;
        } else {
            args[0]=Text("alpha");
            if (cases[i].words>1) args[1]=Text("beta");
            if (cases[i].words>2) args[2]=0x89ABCDEF;
        }
        u32 expected[4]; memcpy(expected,args,sizeof(expected));
        ChatCallback callback={cases[i].type,cases[i].callback,args,(void *)0xCAFEBABE,0,NULL};
        observedCount=0; observedChat=observedUser=NULL;
        ciCallCallback(&chat,&callback);
        CHECK(observedChat==&chat && observedUser==(void *)0xCAFEBABE && observedCount==cases[i].words);
        CHECK(!memcmp(observed,expected,cases[i].words*sizeof(u32)) && live==before);
    }
    return 0;
}
