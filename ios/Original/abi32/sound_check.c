// SPDX-License-Identifier: GPL-3.0-or-later
#include <SND_main.h>
#include <SND_command.h>
#include <SND_interface.h>
#include <SND_bank.h>
#include <SND_work.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern struct SNDCommand sCommandArray[];
extern struct SNDSharedWork sSharedWork;
extern void PushCommand_impl(s32,u32,u32,u32,u32);
static const s16 pcm[]={1000,-1000,3000,-3000,5000,-5000,7000,-7000};
static struct SNDDriverInfo driver;
static struct SNDCommand raw;
_Alignas(4) static u8 music[36],bankImage[80],arcImage[92];
static void put16(u8 *p,u16 value) { memcpy(p,&value,2); }
static void put32(u8 *p,u32 value) { memcpy(p,&value,4); }
static void header(u8 *p,const char *magic,u32 size) {
    memcpy(p,magic,4); put16(p+4,0xFEFF); put16(p+6,0x100); put32(p+8,size);
    put16(p+12,16); put16(p+14,1); memcpy(p+16,"DATA",4); put32(p+20,size-16);
}
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
u32 HGSoundMusicAction(u32 action) {
    if (action==9) return SND_GetPlayerTickCounter(0);
    if (action==10) return SND_GetPlayerStatus();
    if (action==0) {
        memset(music,0,sizeof(music)); memset(bankImage,0,sizeof(bankImage)); memset(arcImage,0,sizeof(arcImage));
        header(music,"SSEQ",sizeof(music)); put32(music+24,28);
        const u8 commands[]={60,127,4,0x80,1,0xFF}; memcpy(music+28,commands,sizeof(commands));
        header(bankImage,"SBNK",sizeof(bankImage)); put32(bankImage+56,1); put32(bankImage+60,64*256+1);
        bankImage[68]=60; bankImage[69]=bankImage[70]=bankImage[71]=bankImage[72]=127; bankImage[73]=64;
        header(arcImage,"SWAR",sizeof(arcImage)); put32(arcImage+56,1); put32(arcImage+60,64);
        arcImage[64]=1; arcImage[65]=1; put16(arcImage+66,32000); put16(arcImage+68,512); put32(arcImage+72,4);
        memcpy(arcImage+76,pcm,sizeof(pcm));
        SND_AssignWaveArc((struct SNDBankData *)bankImage,0,(struct SNDWaveArc *)arcImage);
        SND_SetMasterVolume(127); SND_ResetMasterPan();
        SND_PrepareSeq(0,music+28,0,(struct SNDBankData *)bankImage);
    } else if (action==1) SND_StartPreparedSeq(0);
    else if (action==2) SND_StopSeq(0);
    else if (action==3) SND_SetTrackPan(0,0xFFFF,63);
    else if (action==4) SND_PauseSeq(0,TRUE);
    else if (action==5) SND_PauseSeq(0,FALSE);
    else if (action==6) SND_InvalidateBankData(bankImage,bankImage+sizeof(bankImage));
    else if (action==7) SND_InvalidateWaveData(arcImage,arcImage+sizeof(arcImage));
    else if (action==8) SND_SetWaveDataAddress((struct SNDWaveArc *)arcImage,0,(const struct SNDWaveData *)(arcImage+64));
    else return ~0u;
    flush(); return SND_GetCurrentCommandTag();
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
