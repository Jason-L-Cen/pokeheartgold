// SPDX-License-Identifier: GPL-3.0-or-later
// Execute the original sender/parser/poller with socket and clock observers, without network I/O.
#define current_time HGAvailTime
#define SocketStartUp HGAvailStartUp
#define CanReceiveOnSocket HGAvailCanReceive
#define gsSocketCreate HGAvailSocket
#define gsSocketClose HGAvailClose
#define gsSocketRecvFrom HGAvailRecv
#define gsSocketSendTo HGAvailSend
#define gsSocketInetAddr HGAvailInetAddr
#define SOC_GetHostByName HGAvailDns
#include "../../../lib/NitroDWC/src/ap_connect.c"
#include "../../../lib/NitroDWC/src/available_check_poll.c"
#include <stddef.h>

_Static_assert(sizeof(GsiAvailableCheckState)==0x5C,"Original query state size");
_Static_assert(offsetof(GsiAvailableCheckState,addr)==8,"Original server address offset");
_Static_assert(offsetof(GsiAvailableCheckState,buf)==0x10,"Original packet offset");
_Static_assert(offsetof(GsiAvailableCheckState,time)==0x54,"Original send-time offset");
_Static_assert(offsetof(GsiAvailableCheckState,retries)==0x58,"Original retry offset");
char __GSIACGamename[0x40];
s8 GSIACHostname[0x40];
static u32 now,inetAddress,resolvedAddress;
static u32 *dnsAddresses[]={&resolvedAddress};
static struct { u8 pad[12]; u32 **addresses; } dnsEntry={{0},dnsAddresses};
static int observerFailure,starts,sockets,sends,closes,ready,replyLength,socketResult,dnsWorks;
static u8 reply[8],sent[0x40];
static NSockAddr replyFrom,sentTo;
static int sentLength;
static char lookedUp[0x40];
#define OBSERVE(v) do { if (!(v)) observerFailure=__LINE__; } while (0)
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

u32 HGAvailTime(void) { return now; }
void HGAvailStartUp(void) { starts++; }
int HGAvailCanReceive(int sock) { OBSERVE(sock==socketResult); return ready; }
int HGAvailSocket(int family,int type,int protocol) {
    OBSERVE(family==2 && type==2 && protocol==0); sockets++; return socketResult;
}
int HGAvailClose(int sock) { OBSERVE(sock==socketResult); closes++; return 0; }
int HGAvailSend(int sock,void *data,int len,int flags,void *address,int addressLength) {
    OBSERVE(sock==socketResult && flags==0 && addressLength==8);
    if (len<0 || len>(int)sizeof(sent)) { observerFailure=__LINE__; return -1; }
    sentLength=len; memcpy(sent,data,len); memcpy(&sentTo,address,8); sends++; return len;
}
int HGAvailRecv(int sock,void *data,int len,int flags,void *address,int *addressLength) {
    OBSERVE(sock==socketResult && flags==0 && len==0x40 && *addressLength==8);
    if (replyLength>0) {
        memcpy(data,reply,replyLength); memcpy(address,&replyFrom,8); *addressLength=8;
    }
    return replyLength;
}
u32 HGAvailInetAddr(const char *host) { strcpy(lookedUp,host); return inetAddress; }
void *HGAvailDns(const char *host) { OBSERVE(!strcmp(host,lookedUp)); return dnsWorks ? &dnsEntry : NULL; }

static void ResetQuery(void) {
    memset(&gsiAvailableCheckState,0,sizeof(gsiAvailableCheckState));
    memset(__GSIACGamename,0,sizeof(__GSIACGamename)); memset(GSIACHostname,0,sizeof(GSIACHostname));
    memset(sent,0,sizeof(sent)); memset(reply,0,sizeof(reply)); memset(lookedUp,0,sizeof(lookedUp));
    observerFailure=starts=sockets=sends=closes=ready=0; replyLength=7; socketResult=17; dnsWorks=1;
    now=100; inetAddress=0x44332211; resolvedAddress=0x88776655;
}
static void SetReply(u32 result) {
    memcpy(reply,gsiAvailableCheckResponseMagic,3);
    reply[3]=(u8)(result>>24); reply[4]=(u8)(result>>16); reply[5]=(u8)(result>>8); reply[6]=(u8)result;
    replyFrom=gsiAvailableCheckState.addr; ready=1;
}
u32 CheckOriginalAvailability(void) {
    ResetQuery();
    GSIStartAvailableCheck("heartgold");
    CHECK(starts==1 && sockets==1 && sends==1 && !closes && gsiAvailableCheckState.sock==17);
    CHECK(!strcmp(lookedUp,"heartgold.available.gs.nintendowifi.net") && !strcmp(__GSIACGamename,"heartgold"));
    CHECK(sentLength==15 && sent[0]==9 && !memcmp(sent+1,"\0\0\0\0",4) && !strcmp((char *)sent+5,"heartgold"));
    CHECK(sentTo.family==2 && sentTo.port==0xFC6C && sentTo.addr==inetAddress);
    CHECK(gsiAvailableCheckState.len==15 && gsiAvailableCheckState.time==now && !gsiAvailableCheckState.retries);
    now=2100; CHECK(!GSIAvailableCheckThink() && sends==1 && !closes);
    now=2101; CHECK(!GSIAvailableCheckThink() && sends==2 && !closes && gsiAvailableCheckState.retries==1);
    CHECK(gsiAvailableCheckState.time==2101);
    now=4101; CHECK(!GSIAvailableCheckThink() && !closes);
    now=4102; CHECK(GSIAvailableCheckThink()==1 && closes==1 && GSI_AVAILABLE_RESULT==1 && !observerFailure);

    const u32 results[]={0,1,2,3,0x80000002};
    const int codes[]={1,2,3,2,3};
    for (u32 i=0;i<sizeof(results)/sizeof(results[0]);i++) {
        ResetQuery(); GSIStartAvailableCheck("hg"); SetReply(results[i]);
        CHECK(GSIAvailableCheckThink()==codes[i] && GSI_AVAILABLE_RESULT==codes[i] && closes==1 && !observerFailure);
    }
    ResetQuery(); GSIStartAvailableCheck("hg"); SetReply(0xABCD1234);
    struct { u32 before,value,after; } output={0x11223344,0xDEADBEEF,0x55667788};
    CHECK(!gsiAvailableCheckParseResponse(reply,7,&replyFrom,&output.value) && output.value==0xABCD1234);
    CHECK(output.before==0x11223344 && output.after==0x55667788);
    for (int invalid=0;invalid<5;invalid++) {
        SetReply(0); output.value=0xDEADBEEF;
        int len=invalid==0 ? 6 : invalid==1 ? -1 : 7;
        if (invalid==2) replyFrom.addr^=1;
        if (invalid==3) replyFrom.port^=1;
        if (invalid==4) reply[0]^=1;
        CHECK(gsiAvailableCheckParseResponse(reply,len,&replyFrom,&output.value)==1 && output.value==0xDEADBEEF);
    }
    SetReply(0); reply[0]^=1;
    CHECK(!GSIAvailableCheckThink() && !closes && GSI_AVAILABLE_RESULT==0);
    replyLength=-1; CHECK(!GSIAvailableCheckThink() && !closes && !observerFailure);

    ResetQuery(); inetAddress=0xFFFFFFFF; GSIStartAvailableCheck("hg");
    CHECK(sends==1 && sentTo.addr==resolvedAddress && !observerFailure);
    ResetQuery(); inetAddress=0xFFFFFFFF; dnsWorks=0; GSIStartAvailableCheck("hg");
    CHECK(!sockets && !sends && gsiAvailableCheckState.sock==-1 && GSIAvailableCheckThink()==1);
    ResetQuery(); socketResult=-1; GSIStartAvailableCheck("hg");
    CHECK(sockets==1 && !sends && GSIAvailableCheckThink()==1);
    ResetQuery(); strcpy((char *)GSIACHostname,"127.0.0.1"); GSIStartAvailableCheck("hg");
    CHECK(!strcmp(lookedUp,"127.0.0.1") && sends==1 && !observerFailure);
    gsiAvailableCheckState.time=0xFFFFFC17; now=0;
    CHECK(!GSIAvailableCheckThink() && sends==1); // Preserve the original unsigned deadline wrap rule.
    return 0;
}
