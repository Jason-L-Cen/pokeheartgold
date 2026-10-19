// SPDX-License-Identifier: GPL-3.0-or-later
#include "poke_overlay.h"
#include "../../../lib/NitroSDK/src/fs/fs_private.h"
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
MWiDestructorChain *__global_destructor_chain;
static u32 events[16],count,overflow;
static u32 resident;
extern u32 HGOverlayLifecycleValue(u32 set);
void HGOverlayLifecycleObserve(u32 value) { if (count<16) events[count++]=value; else overflow=1; }
static void residentDestructor(void *object,int mode) { if (object!=&resident || mode!=-1) overflow=1; }
u32 CheckNativeOverlayLifecycle(void) {
    FSOverlayInfo info,bad; FSFile file;
    MWiDestructorChain keep={NULL,residentDestructor,&resident}; __global_destructor_chain=&keep;
    memset(&bad,0xA5,sizeof(bad)); info=bad;
    CHECK(!FS_LoadOverlayInfo(&info,MI_PROCESSOR_ARM7,0) && !memcmp(&info,&bad,sizeof(info)));
    CHECK(!FS_LoadOverlayInfo(&info,MI_PROCESSOR_ARM9,2));
    CHECK(!FS_LoadOverlay(MI_PROCESSOR_ARM9,2) && !FS_UnloadOverlay(MI_PROCESSOR_ARM9,0));
    for (u32 mode=0;mode<3;mode++) {
        count=overflow=0;
        CHECK(HandleLoadOverlay(0,mode) && !HandleLoadOverlay(0,mode));
        CHECK(count==3 && events[0]==1 && events[1]==2 && events[2]==3 && HGOverlayLifecycleValue(0)==17);
        CHECK(HGOverlayLifecycleValue(99)==99 && HandleLoadOverlay(1,OVY_LOAD_NORMAL));
        UnloadOverlayByID(1); UnloadOverlayByID(0);
        CHECK(count==9 && !overflow && __global_destructor_chain==&keep);
        for (u32 i=0;i<9;i++) CHECK(events[i]==i+1);
    }
    CHECK(FS_LoadOverlayInfo(&info,MI_PROCESSOR_ARM9,0)); count=0;
    CHECK(FS_LoadOverlayImageAsync(&info,&file) && !FS_IsBusy(&file) && FS_IsSucceeded(&file));
    CHECK(FS_WaitAsync(&file) && FS_CloseFile(&file) && !FS_CloseFile(&file) && !count);
    FS_StartOverlay(&info); CHECK(count==3); CHECK(FS_UnloadOverlayImage(&info) && count==9);
    CHECK(!FS_UnloadOverlayImage(&info) && __global_destructor_chain==&keep);
    CHECK(FS_LoadOverlayImage(&info) && !FS_LoadOverlayImage(&info));
    CHECK(FS_UnloadOverlayImage(&info) && __global_destructor_chain==&keep);
    __global_destructor_chain=NULL; residentDestructor(&resident,-1); CHECK(!overflow);
    return 0;
}
