// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <string.h>
static struct { _Alignas(32) u8 before[32],buffer[1984],after[32]; } slot;
static MICAutoParam param;
static u32 replies,fullReplies,lastResult,seenArgument,badMode,stopOnFull;
static void checkMode(void) {
    if (OS_GetProcMode()!=OS_PROCMODE_IRQ || OS_GetCpsrIrq()!=OS_INTRMODE_IRQ_DISABLE) badMode=1;
}
static void reply(MICResult result,void *arg) {
    checkMode(); replies++; lastResult=result; seenArgument=(u32)(uaddr)arg;
}
static void full(MICResult result,void *arg) {
    checkMode(); fullReplies++; lastResult=result; seenArgument=(u32)(uaddr)arg;
    if (stopOnFull && MIC_StopAutoSamplingAsync(reply,arg)!=MIC_RESULT_SUCCESS) badMode=1;
}
static u32 wrongCallback(void) { return 0; }
u32 HGMicAddress(u32 which) {
    return which==0 ? (u32)(uaddr)slot.buffer : which==1 ? (u32)(uaddr)&param :
        which==2 ? (u32)(uaddr)wrongCallback : (u32)(uaddr)reply;
}
u32 HGMicAction(u32 command,u32 type,u32 loop) {
    if (!command) {
        MIC_Init(); memset(&slot,0xA5,sizeof(slot)); replies=fullReplies=badMode=stopOnFull=0;
        lastResult=~0u; seenArgument=0;
        param=(MICAutoParam){MIC_SAMPLING_TYPE_SIGNED_8BIT,slot.buffer,sizeof(slot.buffer),HW_CPU_CLOCK_ARM7/2000,FALSE,full,(void *)(uaddr)0x1234ABCD};
        return 0;
    }
    if (command==1 || command==2) {
        param.type=type; param.loop_enable=loop;
        return command==1 ? MIC_StartAutoSampling(&param) : MIC_StartAutoSamplingAsync(&param,reply,(void *)(uaddr)0x9876ABCD);
    }
    if (command==3) return MIC_StopAutoSampling();
    if (command==4) return MIC_StopAutoSamplingAsync(reply,(void *)(uaddr)0xBEEF1234);
    if (command==5) return (u32)(uaddr)MIC_GetLastSamplingAddress();
    if (command==6) { stopOnFull=1; return 0; }
    if (command==7) return MIC_StartAutoSampling(NULL);
    return ~0u;
}
u32 HGMicRead(u32 which) {
    if (!which) return replies;
    if (which==1) return fullReplies;
    if (which==2) return lastResult;
    if (which==3) return seenArgument;
    if (which==4) return badMode;
    for (u32 i=0;i<32;i++) if (slot.before[i]!=0xA5 || slot.after[i]!=0xA5) return 0;
    return 1;
}
