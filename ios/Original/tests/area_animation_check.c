// SPDX-License-Identifier: GPL-3.0-or-later
// Keep fixture allocation/resource observations separate from the game implementations.
#define Heap_Alloc AreaCheckAlloc
#define Heap_Free AreaCheckFree
#define HeapExp_FndInitAllocator AreaCheckInitAllocator
#define AllocAndReadWholeNarcMemberByIdPair AreaCheckReadResource
#define NNS_G3dGetAnmByIdx AreaCheckGetAnimation
#define NNS_FndAllocFromAllocator AreaCheckAllocAnimation
#define NNS_G3dFreeAnmObj AreaCheckFreeAnimation
#define MIi_CpuClearFast AreaCheckClear
#define gAnmCalcFuncNsBta AreaCheckCalcFunction
#include "../../../src/field/map_prop_render.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static u32 allocations, resourceCount, requested;
static NNSFndAllocator *expectedAllocator;
static void Calculate(NNSG3dMatAnmResult *result,const NNSG3dAnmObj *obj,u32 index) { (void)result; (void)obj; (void)index; }
NNSG3dFuncAnmMat AreaCheckCalcFunction=Calculate;
void *AreaCheckAlloc(enum HeapID heapID,u32 size) { (void)heapID; void *p=calloc(1,size); if (!p) abort(); allocations++; return p; }
void AreaCheckFree(void *p) { if (p) { free(p); allocations--; } }
void AreaCheckInitAllocator(NNSFndAllocator *allocator,enum HeapID heapID,int alignment) { (void)heapID; (void)alignment; memset(allocator,0,sizeof(*allocator)); }
void AreaCheckClear(u32 value,u32 *p,u32 size) { if (value) abort(); memset(p,0,size); }
void *AreaCheckReadResource(NarcId narc,s32 member,enum HeapID heapID) {
    if (narc!=NARC_a_1_4_0 || member!=7) abort();
    ResAnim_4004 *resource=AreaCheckAlloc(heapID,sizeof(*resource)); resource->numMapData=(u8)resourceCount; return resource;
}
void *AreaCheckGetAnimation(const void *resource,u32 index) { if (index) abort(); return (void *)resource; }
void *AreaCheckAllocAnimation(NNSFndAllocator *allocator,u32 size) {
    if (allocator!=expectedAllocator) abort(); requested=size; return AreaCheckAlloc(HEAP_ID_FIELD1,size);
}
void AreaCheckFreeAnimation(NNSFndAllocator *allocator,NNSG3dAnmObj *object) { if (allocator!=expectedAllocator) abort(); AreaCheckFree(object); }
u32 CheckOriginalAreaAnimation(void) {
    const u8 counts[]={0,1,2,3,254,255};
    CHECK(!allocations);
    for (u32 n=0;n<sizeof(counts);n++) {
        resourceCount=counts[n];
        MapPropAnimationPoolList *list=MapPropAnimationPoolList_New(HEAP_ID_FIELD1);
        CHECK(list && !list->numPools && !list->head && !list->tail);
        AreaMapAnimation *area=AreaMapAnimation_New(list); expectedAllocator=&area->pool->allocator;
        CHECK(list->numPools==1 && area->pool==list->head && !AreaMapAnimation_IsLoaded(area));
        AreaMapAnimation_Load(area,7);
        CHECK(AreaMapAnimation_IsLoaded(area) && area->mapPropAnimation==area->pool->animations);
        MapPropAnimation *animation=area->mapPropAnimation; NNSG3dAnmObj *object=animation->animObj;
        CHECK(object && animation->inUse && animation->res==object->resAnm);
        CHECK(requested>=sizeof(*object) && requested>=offsetof(NNSG3dAnmObj,mapData)+resourceCount*sizeof(u16));
        CHECK(!object->frame && object->ratio==FX32_ONE && !object->next && !object->resTex && object->priority==0x7F);
        CHECK(object->funcAnm==(void *)Calculate && object->numMapData==resourceCount);
        for (u32 i=0;i<resourceCount;i++) CHECK(object->mapData[i]==(i|0x100));
        AreaMapAnimation_Free(area); MapPropAnimationPoolList_Free(list); CHECK(!allocations);
    }
    return 0;
}
#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) { u32 line=CheckOriginalAreaAnimation(); if (line) { fprintf(stderr,"Area animation check line %u\n",line); return 1; } puts("PASS: original area animation callback, pool ownership, map-data sizes 0..255 and complete cleanup."); return 0; }
#endif
