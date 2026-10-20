// SPDX-License-Identifier: GPL-3.0-or-later
#include <SND_main.h>
#include <SND_command.h>
#include <SND_interface.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern struct SNDCommand sCommandArray[];
extern struct SNDSharedWork sSharedWork;
extern void PushCommand_impl(s32,u32,u32,u32,u32);
static const s16 pcm[]={1000,-1000,3000,-3000,5000,-5000,7000,-7000};
static struct SNDDriverInfo driver;
static struct SNDCommand raw;
_Static_assert(sizeof(struct SNDCommand)==24 && sizeof(struct SNDSharedWork)==0x280,"Original sound wire layouts");
u32 HGSoundReference(u32 field) {
    return field==0 ? (u32)(uaddr)&sSharedWork : field==1 ? (u32)(uaddr)sCommandArray :
        field==2 ? (u32)(uaddr)pcm : field==3 ? (u32)(uaddr)&driver : (u32)(uaddr)&raw;
}
u32 HGSoundRawCommand(u32 id,u32 first,u32 second) {
    raw=(struct SNDCommand){NULL,(s32)id,{first,second,0,0}}; return (u32)(uaddr)&raw;
}
static void flush(void) {
    u32 tag=SND_GetCurrentCommandTag();
    if (!SND_FlushCommand(SND_CMD_FLAG_BLOCK|SND_CMD_FLAG_IMMEDIATE)) return;
    SND_WaitForCommandProc(tag);
}
u32 HGSoundAction(u32 action,u32 value) {
    if (action==0) return SND_GetChannelStatus();
    if (action==1) {
        SND_SetMasterVolume(127); SND_ResetMasterPan();
        SND_SetupChannelPcm(0,1,pcm,1,0,sizeof(pcm)/4,127,0,512,0);
        SND_StartTimer(1,0,0,0);
    } else if (action==2) {
        SND_SetChannelVolume(1,63,1); SND_SetChannelPan(1,127);
    } else if (action==3) SND_StopTimer(1,0,0,0);
    else if (action==4) SND_InvalidateWaveData(pcm,(const u8 *)pcm+sizeof(pcm));
    else if (action==5) SND_ReadDriverInfo(&driver);
    else if (action==6) PushCommand_impl(0x0A,0,3,value,0);
    else if (action==7) PushCommand_impl(0x0B,4,value,0,0);
    else if (action==8) SND_SetMasterVolume((s32)value);
    else return ~0u;
    flush(); return SND_GetCurrentCommandTag();
}
u32 CheckNativeSoundQueue(void) {
    SND_Init(); SND_Init();
    CHECK(SND_CountFreeCommand()==255 && SND_CountReservedCommand()==0 && SND_CountWaitingCommand()==1);
    u32 tag=SND_GetCurrentCommandTag(); CHECK(tag==1 && !SND_IsFinishedCommandTag(tag));
    SND_WaitForCommandProc(tag);
    CHECK(SND_IsFinishedCommandTag(tag) && SND_CountFreeCommand()==256 && !SND_CountWaitingCommand());
    for (u32 i=0;i<8;i++) { SND_SetMasterVolume(10+i); CHECK(SND_FlushCommand(0)); }
    CHECK(SND_CountWaitingCommand()==8 && SND_CountFreeCommand()==248);
    SND_SetMasterVolume(100); CHECK(!SND_FlushCommand(0) && SND_CountReservedCommand()==1);
    tag=SND_GetCurrentCommandTag(); CHECK(SND_FlushCommand(3)); SND_WaitForCommandProc(tag);
    while (SND_RecvCommandReply(0)) {}
    CHECK(SND_CountFreeCommand()==256 && !SND_CountWaitingCommand());
    for (u32 i=0;i<256;i++) SND_SetMasterVolume(i&127);
    CHECK(!SND_AllocCommand(0) && SND_CountReservedCommand()==256);
    tag=SND_GetCurrentCommandTag(); CHECK(SND_FlushCommand(3)); SND_WaitForCommandProc(tag);
    CHECK(SND_CountFreeCommand()==256 && !SND_CountReservedCommand() && !SND_CountWaitingCommand());
    return 0;
}
