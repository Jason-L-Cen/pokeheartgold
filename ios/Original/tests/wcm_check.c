// SPDX-License-Identifier: GPL-3.0-or-later
// Observe only platform requests; execute the original WCM control/API code below.
#define OS_DisableInterrupts WcmCheck_DisableInterrupts
#define OS_RestoreInterrupts WcmCheck_RestoreInterrupts
#define OS_SetAlarm WcmCheck_SetAlarm
#define OS_CancelAlarm WcmCheck_CancelAlarm
#define WM_GetDispersionScanPeriod WcmCheck_GetScanPeriod
#define WCMi_CpsifSendNullPacket WcmCheck_NullPacket
#include "../../../lib/NitroWiFi/src/wcm_control.c"
#include "../../../lib/NitroWiFi/src/wcm.c"
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
WcmWork *gWcmWork;
const u8 WCM_Bssid_Any[6]={255,255,255,255,255,255};
const u8 WCM_Essid_Any[32]={255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255};
static OSIntrMode irqState;
static u32 disables,restores,sets,cancels,nullPackets,periodReads;
static OSAlarm *lastAlarm;
static OSTick lastDelay;
static OSAlarmHandler lastHandler;
static void *lastArg;
OSIntrMode WcmCheck_DisableInterrupts(void) { OSIntrMode old=irqState; irqState|=0x80; disables++; return old; }
OSIntrMode WcmCheck_RestoreInterrupts(OSIntrMode state) { OSIntrMode old=irqState; irqState=state; restores++; return old; }
void WcmCheck_SetAlarm(OSAlarm *alarm,OSTick delay,OSAlarmHandler handler,void *arg) {
    sets++; lastAlarm=alarm; lastDelay=delay; lastHandler=handler; lastArg=arg;
}
void WcmCheck_CancelAlarm(OSAlarm *alarm) { cancels++; lastAlarm=alarm; }
u16 WcmCheck_GetScanPeriod(void) { periodReads++; return 17; }
void WcmCheck_NullPacket(void) { nullPackets++; }
static WcmNotice notices[4];
static u32 noticeCount,reenter;
static void Notice(WcmNotice *notice) {
    if (noticeCount>=4) abort();
    notices[noticeCount++]=*notice;
    if (reenter) {
        reenter=0; gWcmWork->notify=WCM_NOTIFY_DISCONNECT;
        WcmNotify(WCM_RESULT_FAILURE,notice->param1,notice->param0,99);
    }
}
static u32 BitCount(u32 value) { u32 count=0; for (u32 bit=0;bit<32;bit++) count+=(value>>bit)&1; return count; }
static u32 LeadingZeros(u32 value) { u32 count=0; for (u32 bit=0x80000000;bit && !(value&bit);bit>>=1) count++; return count; }

u32 CheckOriginalWcm(void) {
    for (u32 value=0;value<=0xFFFF;value++) {
        const u32 words[]={value,value<<16,(value*0x9E3779B1u)^0xA5A5BEEF};
        for (u32 i=0;i<3;i++) CHECK(WcmCountBits(words[i])==BitCount(words[i]) && WcmCountLeadingZero(words[i])==LeadingZeros(words[i]));
    }
    CHECK(WcmCountLeadingZero(0)==32 && WcmCountBits(0xFFFFFFFF)==32);
    irqState=0x40; disables=restores=0; gWcmWork=NULL;
    CHECK(WCM_UpdateOption(0xFFFFFFFF)==0 && WCM_GetPhase()==WCM_PHASE_NULL && irqState==0x40);
    static WcmWork work; memset(&work,0,sizeof(work)); gWcmWork=&work;
    WcmConfigure(NULL,Notice); CHECK(work.dmaNo==3 && work.apList==NULL && work.apListSize==0 && work.apListMode==0 && work.callback==Notice);
    _Alignas(16) u8 buffer[128];
    for (u32 offset=0;offset<16;offset++) for (u32 size=0;size<=64;size++) {
        memset(buffer,0xA5,sizeof(buffer));
        u32 alignment=_Alignof(WcmApList),padding=(alignment-((uaddr)(buffer+16+offset)%alignment))%alignment;
        WcmConfig config={7,buffer+16+offset,size,1}; WcmConfigure(&config,Notice);
        CHECK(work.dmaNo==3 && work.apListMode==1 && work.callback==Notice);
        if (size<padding+sizeof(WcmApList)) CHECK(work.apList==NULL && work.apListSize==0);
        else CHECK(work.apList==(WcmApList *)(buffer+16+offset+padding) && work.apListSize==size-padding);
        for (u32 i=0;i<sizeof(buffer);i++) {
            int cleared=size>=padding+sizeof(WcmApList) && i>=16+offset+padding && i<16+offset+size;
            CHECK(buffer[i]==(cleared ? 0 : 0xA5));
        }
    }
    WcmConfig empty={1,NULL,0xFFFFFFFF,1}; WcmConfigure(&empty,Notice);
    CHECK(work.apList==NULL && work.apListSize==0);
    WcmInitOption(); CHECK(work.option==0xAAA082);
    for (u32 mask=0;mask<0x2000;mask++) for (int channel=0;channel<=13;channel++) {
        work.option=(mask<<1)|0xAB000001;
        u16 next=WcmGetNextScanChannel(channel); CHECK(next>=1 && next<=13);
        if (!mask) CHECK(next==channel%13+1);
        else {
            CHECK(mask&(1u<<(next-1)));
            int distance=(next-channel+13)%13; if (!distance) distance=13;
            for (int bit=1;bit<=13;bit++) if (mask&(1u<<(bit-1))) {
                int other=(bit-channel+13)%13; if (!other) other=13;
                CHECK(other>=distance);
            }
        }
    }
    work.option=0; CHECK(WCM_UpdateOption(0x8000)==0 && work.option==0xA082);
    CHECK(WCM_UpdateOption(0x8020)==0xA082 && work.option==0x8020);
    CHECK(WCM_UpdateOption(0x30000)==0x8020 && work.option==0x38020 && irqState==0x40);
    work.option=(1<<1)|(1<<13); work.scanPeriod=0; work.scanCount=42; periodReads=0;
    WcmEditScanExParam(NULL,NULL,0);
    CHECK(work.scanParam.buffer==work.recvBuffer && work.scanParam.bufferSize==0x400 && work.scanParam.channels==1 && work.scanParam.duration==17 && periodReads==1);
    CHECK(work.scanParam.scanType==1 && work.scanParam.ssidLength==0 && work.scanCount==0);
    CHECK(!memcmp(work.scanParam.bssid,WCM_Bssid_Any,6) && !memcmp(work.scanParam.ssid,WCM_Essid_Any,32));
    const u8 bssid[6]={1,2,3,4,5,6}; u8 ssid[32]; memset(ssid,'X',sizeof(ssid)); ssid[7]=0;
    work.scanPeriod=25; work.option=0x300000|(1<<13);
    WcmEditScanExParam(bssid,ssid,0);
    CHECK(work.scanParam.channels==(1<<12) && work.scanParam.duration==25 && periodReads==1 && work.scanParam.scanType==0 && work.scanParam.ssidLength==7);
    CHECK(!memcmp(work.scanParam.bssid,bssid,6) && !memcmp(work.scanParam.ssid,ssid,32));
    memset(ssid,255,sizeof(ssid)); WcmEditScanExParam(NULL,ssid,0); CHECK(work.scanParam.ssidLength==32);
    WcmEditScanExParam(NULL,WCM_Essid_Any,0); CHECK(work.scanParam.ssidLength==0);

    u32 first=1,second=2; noticeCount=0; reenter=1; work.notify=WCM_NOTIFY_CONNECT;
    WcmNotify(WCM_RESULT_FATAL_ERROR,&first,&second,42);
    CHECK(noticeCount==2 && work.notify==WCM_NOTIFY_COMMON);
    CHECK(notices[0].notify==WCM_NOTIFY_CONNECT && notices[0].result==WCM_RESULT_FATAL_ERROR && notices[0].param0==&first && notices[0].param1==&second && notices[0].line==42);
    CHECK(notices[1].notify==WCM_NOTIFY_DISCONNECT && notices[1].result==WCM_RESULT_FAILURE && notices[1].param0==&second && notices[1].param1==&first && notices[1].line==99);
    work.callback=NULL; work.notify=WCM_NOTIFY_STARTUP; WcmNotify(0,NULL,NULL,0);
    CHECK(noticeCount==2 && work.notify==WCM_NOTIFY_COMMON); work.callback=Notice;

    sets=cancels=nullPackets=0; work.phase=WCM_PHASE_IDLE;
    WcmSetPhase(WCM_PHASE_DCF); CHECK(work.phase==WCM_PHASE_DCF && sets==1 && cancels==0);
    CHECK(lastAlarm==&work.keepAliveAlarm && lastDelay==0x22F5341 && lastHandler==WcmKeepAliveAlarm && lastArg==NULL);
    WcmSetPhase(WCM_PHASE_IDLE); CHECK(work.phase==WCM_PHASE_IDLE && cancels==1);
    WcmSetPhase(WCM_PHASE_FATAL_ERROR); WcmSetPhase(WCM_PHASE_WAIT); CHECK(work.phase==WCM_PHASE_FATAL_ERROR);
    WcmSetPhase(WCM_PHASE_DCF); CHECK(work.phase==WCM_PHASE_FATAL_ERROR && sets==2);
    WCMi_ResetKeepAliveAlarm(); CHECK(cancels==2 && sets==2);
    work.phase=WCM_PHASE_DCF; WCMi_ResetKeepAliveAlarm(); CHECK(cancels==3 && sets==3);
    lastHandler(lastArg); CHECK(nullPackets==1 && cancels==4 && sets==4);
    WcmSetPhase(WCM_PHASE_IDLE); CHECK(work.phase==WCM_PHASE_IDLE && cancels==5 && sets==4);
    CHECK(irqState==0x40 && disables==restores); gWcmWork=NULL;
    return 0;
}
