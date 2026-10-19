// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include "../../../lib/NitroSDK/src/fs/fs_private.h"
extern void HGOverlayLifecycleObserve(u32 value);
static u32 value=17;
static MWiDestructorChain nodes[3];
static void again(void *object,int mode) { if (object || mode!=-1) HGOverlayLifecycleObserve(99); HGOverlayLifecycleObserve(6); }
static void nullObject(void *object,int mode) { if (object || mode!=-1) HGOverlayLifecycleObserve(99); HGOverlayLifecycleObserve(5); }
static void ownedObject(void *object,int mode) {
    if (object!=&value || mode!=-1 || FS_LoadOverlay(MI_PROCESSOR_ARM9,0)) HGOverlayLifecycleObserve(99);
    HGOverlayLifecycleObserve(4); nodes[2]=(MWiDestructorChain){__global_destructor_chain,again,NULL}; __global_destructor_chain=&nodes[2];
}
__attribute__((constructor(101))) static void first(void) {
    HGOverlayLifecycleObserve(1);
    nodes[0]=(MWiDestructorChain){__global_destructor_chain,nullObject,NULL};
    nodes[1]=(MWiDestructorChain){&nodes[0],ownedObject,&value}; __global_destructor_chain=&nodes[1];
}
__attribute__((constructor(201))) static void last(void) { HGOverlayLifecycleObserve(3); }
__attribute__((destructor(101))) static void endLast(void) { HGOverlayLifecycleObserve(9); }
__attribute__((destructor(201))) static void endFirst(void) { HGOverlayLifecycleObserve(7); }
u32 HGOverlayLifecycleValue(u32 set) { if (set) value=set; return value; }
