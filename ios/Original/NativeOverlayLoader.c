// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/fs/native_overlay.h>
#include "../../lib/NitroSDK/src/fs/fs_private.h"
#include "assert.h"
#include <string.h>

enum { UNLOADED,RESTORED,ACTIVE,TRANSITION };
static u8 residency[HG_OVERLAY_COUNT];
static FSArchive completedOverlay;
static const HGNativeOverlayEntry *entry(const FSOverlayInfo *info) {
    if (!info || info->target!=MI_PROCESSOR_ARM9 || info->header.id>=HGNativeOverlayCount ||
        info->header.id>=HG_OVERLAY_COUNT || info->header.ram_address!=&residency[info->header.id]) return NULL;
    const HGNativeOverlayEntry *result=&HGNativeOverlays[info->header.id];
    return result->ready ? result : NULL;
}
BOOL FS_LoadOverlayInfo(FSOverlayInfo *info,MIProcessor target,FSOverlayID id) {
    if (!info || target!=MI_PROCESSOR_ARM9 || id>=HGNativeOverlayCount || id>=HG_OVERLAY_COUNT || !HGNativeOverlays[id].ready) return FALSE;
    memset(info,0,sizeof(*info)); info->target=target; info->header.id=id;
    // Native code is prelinked; the address/size are a residency token for original overlap checks.
    info->header.ram_address=&residency[id]; info->header.ram_size=1; info->header.file_id=~0u;
    return TRUE;
}
BOOL FSi_LoadOverlayInfoCore(FSOverlayInfo *info,MIProcessor target,FSOverlayID id,FSArchive *archive,u32 a,u32 b,u32 c,u32 d) {
    (void)archive; (void)a; (void)b; (void)c; (void)d; return FS_LoadOverlayInfo(info,target,id);
}
u32 FSi_GetOverlayBinarySize(FSOverlayInfo *info) { return entry(info) ? 1 : 0; }
FSFileID FS_GetOverlayFileID(FSOverlayInfo *info) { (void)info; return (FSFileID){&fsi_arc_rom,~0u}; }
BOOL FS_LoadOverlayImage(FSOverlayInfo *info) {
    const HGNativeOverlayEntry *group=entry(info);
    if (!group || residency[info->header.id]!=UNLOADED) return FALSE;
    for (u32 i=0;i<group->unitCount;i++) if (!group->units[i] || !HGNativeValidateOverlayUnit(group->units[i]())) return FALSE;
    OSIntrMode previous=OS_DisableInterrupts(); residency[info->header.id]=TRANSITION;
    for (u32 i=0;i<group->unitCount;i++) GF_ASSERT(HGNativeRestoreOverlayUnit(group->units[i]()));
    residency[info->header.id]=RESTORED; OS_RestoreInterrupts(previous); return TRUE;
}
void FS_ClearOverlayImage(FSOverlayInfo *info) { GF_ASSERT(FS_LoadOverlayImage(info)); }
BOOL HGNativeIsOverlayFile(const FSFile *file) { return file && file->arc==&completedOverlay; }
BOOL HGNativeCloseOverlayFile(FSFile *file) {
    if (!HGNativeIsOverlayFile(file)) return FALSE;
    file->arc=NULL; file->stat=0; file->command=FS_COMMAND_INVALID; return TRUE;
}
BOOL FS_LoadOverlayImageAsync(FSOverlayInfo *info,FSFile *file) {
    if (!file) return FALSE;
    FS_InitFile(file);
    if (!FS_LoadOverlayImage(info)) return FALSE;
    file->arc=&completedOverlay; file->stat=FS_FILE_STATUS_IS_FILE|FS_FILE_STATUS_ASYNC;
    file->command=FS_COMMAND_INVALID; file->error=FS_RESULT_SUCCESS; return TRUE;
}
static void initializers(const HGNativeOverlayEntry *group,BOOL destruct) {
    BOOL first=TRUE; u32 lastPriority=0; u64 lastOrder=0;
    // shortcut: priority selection is quadratic in constructor count; generate a merged order if lists grow large.
    for (;;) {
        void (*next)(void)=NULL; u32 best=0; u64 bestOrder=0,order=0;
        for (u32 u=0;u<group->unitCount;u++) {
            const HGNativeOverlayUnit *unit=group->units[destruct ? group->unitCount-1-u : u]();
            u32 count=destruct ? unit->destructorCount : unit->constructorCount;
            const HGNativeOverlayFunction *list=destruct ? unit->destructors : unit->constructors;
            for (u32 i=0;i<count;i++,order++) {
                u32 priority=destruct ? ~list[i].priority : list[i].priority;
                if (!first && (priority<lastPriority || (priority==lastPriority && order<=lastOrder))) continue;
                if (!next || priority<best || (priority==best && order<bestOrder)) { next=list[i].function; best=priority; bestOrder=order; }
            }
        }
        if (!next) return;
        first=FALSE; lastPriority=best; lastOrder=bestOrder; next();
    }
}
void FS_StartOverlay(FSOverlayInfo *info) {
    const HGNativeOverlayEntry *group=entry(info);
    GF_ASSERT(group && residency[info->header.id]==RESTORED);
    residency[info->header.id]=TRANSITION; initializers(group,FALSE); residency[info->header.id]=ACTIVE;
}
static BOOL owns(const HGNativeOverlayEntry *group,const void *object,MWI_DESTRUCTOR_FUNC function) {
    for (u32 u=0;u<group->unitCount;u++) {
        const HGNativeOverlayUnit *unit=group->units[u]();
        if (object) {
            for (u32 i=0;i<unit->spanCount;i++) {
                uaddr start=(uaddr)unit->spans[i].address,address=(uaddr)object;
                if (address>=start && address-start<unit->spans[i].size) return TRUE;
            }
        } else for (u32 i=0;i<unit->functionCount;i++) if (unit->functions[i]==(u32)(uaddr)function) return TRUE;
    }
    return FALSE;
}
void FS_EndOverlay(FSOverlayInfo *info) {
    const HGNativeOverlayEntry *group=entry(info);
    GF_ASSERT(group && (residency[info->header.id]==ACTIVE || residency[info->header.id]==RESTORED));
    BOOL active=residency[info->header.id]==ACTIVE; residency[info->header.id]=TRANSITION;
    if (active) {
        for (;;) {
            OSIntrMode previous=OS_DisableInterrupts();
            MWiDestructorChain **link=&__global_destructor_chain,*node,*head=NULL,*tail=NULL;
            while ((node=*link)!=NULL) {
                if (owns(group,node->obj,node->dtor)) {
                    *link=node->next; node->next=NULL;
                    if (tail) tail->next=node; else head=node;
                    tail=node;
                } else link=&node->next;
            }
            OS_RestoreInterrupts(previous);
            if (!head) break;
            while (head) { node=head; head=node->next; if (node->dtor) node->dtor(node->obj,-1); }
        }
        initializers(group,TRUE);
    }
    residency[info->header.id]=UNLOADED;
}
BOOL FS_UnloadOverlayImage(FSOverlayInfo *info) {
    if (!entry(info) || (residency[info->header.id]!=ACTIVE && residency[info->header.id]!=RESTORED)) return FALSE;
    FS_EndOverlay(info); return TRUE;
}
BOOL FS_LoadOverlay(MIProcessor target,FSOverlayID id) {
    FSOverlayInfo info;
    if (!FS_LoadOverlayInfo(&info,target,id) || !FS_LoadOverlayImage(&info)) return FALSE;
    FS_StartOverlay(&info); return TRUE;
}
BOOL FS_UnloadOverlay(MIProcessor target,FSOverlayID id) {
    FSOverlayInfo info; return FS_LoadOverlayInfo(&info,target,id) && FS_UnloadOverlayImage(&info);
}
