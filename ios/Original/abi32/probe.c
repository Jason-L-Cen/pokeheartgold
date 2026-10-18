// SPDX-License-Identifier: GPL-3.0-or-later
#include "pokemon.h"
#include "math_util.h"
#include "overlay45_string_search.h"
#include "orbit_cursor.h"
#include "wifi_setup_ui.h"
#include "wifi_connection.h"
#include "billboard_internal.h"
#include "field_interaction.h"
#include "field_system.h"
#include "overlay_35.h"
#include "spawn_warp_lookup_internal.h"
#include "frontier_save_access.h"
#include "particle_system_internal.h"
#include "../NativeCompression.h"
#include <nitro/fx/fx_mtx22.h>
#include <nitro/fx/fx_mtx33.h>
#include <nitro/fx/fx_mtx43.h>
#include <nitro/fx/fx_mtx44.h>
#include <nnsys/g2d_internal.h>
#include "../../../lib/NitroDWC/src/tree-iterator4.h"
#include <stddef.h>

u32 CalcMonChecksum(void *data, u32 size);
static u16 data[128];
static StringSearch string;
static Iterator iterator;
u32 HGAotCompressedResource(u32 variant, u32 inputSize, u32 capacity) {
    static const u8 lz10[]={0x10,9,0,0,0x10,'A','B','C',0x30,2};
    static const u8 lz11[]={0x11,4,0,0,0x40,'A',0x20,0};
    static const u8 invalid[]={0x10,3,0,0,0x80,0,0};
    static u8 output[16];
    const u8 *input=variant==0 ? lz10 : variant==1 ? lz11 : invalid;
    u32 size=variant==0 ? sizeof(lz10) : variant==1 ? sizeof(lz11) : sizeof(invalid);
    if (variant>2 || inputSize>size || capacity>sizeof(output)) return FALSE;
    if (!HGNativeUncompressLZ(input,inputSize,output,capacity)) return FALSE;
    const char *expected=variant==0 ? "ABCABCABC" : "AAAA";
    u32 length=variant==0 ? 9 : 4;
    for (u32 i=0;i<length;i++) if (output[i]!=expected[i]) return FALSE;
    return TRUE;
}
static u32 particleCallbacks;
static void ParticleInitialized(SPLEmitter *emitter) {
    particleCallbacks++;
    emitter->userwork.uw_u32=0x12345678;
}
u32 HGAotParticleCreation(void) {
    SPLResBase base={0};
    base.pos=(VecFx32){4096,-8192,12288};
    base.axis=(VecFx16){1,-2,3};
    base.init_rtt=123; base.gen_num=2048;
    SPLResource resource={0}; resource.p_base=&base;
    SPLEmitter emitter={0};
    SPLManager manager={0};
    manager.res_ary=&resource;
    manager.ina_emtr_list=(SPLEmitterList){&emitter,1,&emitter};
    ParticleSystem system={0}; system.manager=&manager;
    const VecFx32 position={-1024,2048,4096};
    ParticleSystem_CreateEmitter((SPLEmitter *)&system,0,&position);
    if (system.emitter!=&emitter || emitter.p_res!=&resource || emitter.emtr_pos.x!=3072 ||
        emitter.emtr_pos.y!=-6144 || emitter.emtr_pos.z!=16384 || emitter.axis.y!=-2 ||
        emitter.init_rtt!=123 || emitter.gen_num!=2048 || manager.act_emtr_list.node_num!=1 ||
        manager.act_emtr_list.p_begin!=&emitter || manager.ina_emtr_list.node_num!=0) return FALSE;
    ParticleSystem_CreateEmitter((SPLEmitter *)&system,0,&position);
    if (system.emitter || manager.act_emtr_list.node_num!=1) return FALSE;
    SPL_DeleteAll(&manager);
    if (manager.act_emtr_list.node_num || manager.ina_emtr_list.node_num!=1) return FALSE;
    particleCallbacks=0;
    SPLEmitter *created=SPL_CreateWithInitialize(&manager,0,ParticleInitialized);
    if (created!=&emitter || particleCallbacks!=1 || emitter.userwork.uw_u32!=0x12345678 ||
        emitter.emtr_pos.x!=4096 || emitter.emtr_pos.y!=-8192 || emitter.emtr_pos.z!=12288) return FALSE;
    SPL_DeleteAll(&manager);
    base.flag.self_dest=1;
    created=SPL_CreateWithInitialize(&manager,0,ParticleInitialized);
    return !created && particleCallbacks==2 && manager.act_emtr_list.node_num==1 && !manager.ina_emtr_list.node_num;
}
u32 HGAotSDKMath(void) {
    MtxFx22 matrix;
    MTX_Identity22_(&matrix);
    if (matrix.a[0]!=4096 || matrix.a[1]!=0 || matrix.a[2]!=0 || matrix.a[3]!=4096) return FALSE;
    MTX_Rot22_(&matrix,2048,4096);
    if (matrix.a[0]!=4096 || matrix.a[1]!=2048 || matrix.a[2]!=-2048 || matrix.a[3]!=4096) return FALSE;
    matrix.a[0]=4096; matrix.a[1]=-4096; matrix.a[2]=matrix.a[3]=2048;
    MTX_ScaleApply22(&matrix,&matrix,6144,-8192);
    return matrix.a[0]==6144 && matrix.a[1]==-6144 && matrix.a[2]==-4096 && matrix.a[3]==-4096;
}
u32 HGAotSDKMatrix(u32 operation, s32 sine, s32 cosine) {
    static union { MtxFx33 m33; MtxFx43 m43; MtxFx44 m44; } result;
    for (u32 i=0;i<16;i++) result.m44.a[i]=0x5A5A5A5A;
    switch (operation) {
    case 0: MTX_Identity33_(&result.m33); break;
    case 1: MTX_RotY33_(&result.m33,sine,cosine); break;
    case 2: MTX_RotZ33_(&result.m33,sine,cosine); break;
    case 3: MTX_Identity43_(&result.m43); break;
    case 4: MTX_Scale43_(&result.m43,sine,cosine,1234); break;
    case 5: MTX_RotX43_(&result.m43,sine,cosine); break;
    case 6: MTX_RotY43_(&result.m43,sine,cosine); break;
    case 7: MTX_Identity44_(&result.m44); break;
    case 8: MTX_RotX44_(&result.m44,sine,cosine); break;
    case 9: MTX_RotY44_(&result.m44,sine,cosine); break;
    case 10: MTX_RotZ44_(&result.m44,sine,cosine); break;
    case 11: {
        MtxFx43 source;
        for (u32 i=0;i<12;i++) source.a[i]=i+1;
        MTX_Copy43To44_(&source,&result.m44);
        break;
    }
    case 12: case 13: {
        MtxFx44 source;
        for (u32 i=0;i<16;i++) source.a[i]=i+1;
        if (operation==13) {
            result.m44=source;
            MTX_Copy44To43_(&result.m44,&result.m43);
        } else MTX_Copy44To43_(&source,&result.m43);
        break;
    }
    default: return 0;
    }
    return (u32)&result;
}
u32 HGAotSDKCharIndex(u32 width, u32 height, u32 x, u32 y) {
    if (!width || !height || width>16 || height>16 || x>=width || y>=height) return UINT32_MAX;
    u32 sw=0,sh=0;
    while (sw<3 && (2u<<sw)<=width) sw++;
    while (sh<3 && (2u<<sh)<=height) sh++;
    const u8 *entry=gCharShiftTable[sh][sw];
    CharShift shift={entry[0],entry[1]};
    return GetCharIndex1D(x,y,width,height,shift);
}
u32 HGAotFactoryFlags(void) {
    u8 flags=0xA5;
    const u8 expected[]={1,0,1,0,0,1,0,1};
    for (u32 i=0;i<8;i++) if (FactoryFlags_GetField(&flags,10,i)!=expected[i]) return FALSE;
    if (FactoryFlags_GetField(&flags,9,0)!=0) return FALSE;
    u8 value=1; FactoryFlags_SetField(&flags,10,1,&value);
    if (flags!=0xA7) return FALSE;
    value=0; FactoryFlags_SetField(&flags,10,2,&value);
    if (flags!=0xA3) return FALSE;
    value=1; FactoryFlags_SetField(&flags,9,0,&value);
    return flags==0xA3;
}
extern const u8 HgAsmBytes[], HgAsmAlias[];
extern const u8 *HgAsmTarget;
extern u16 (*HgAsmRng)(void);
extern u8 HgAsmZero[16];
extern const u32 HgAsmOverlay;
extern const u8 HgSdkData[8];
extern u8 HgSdkZero[8];
u32 HGAotAsmPointers(void) {
    if (HgAsmBytes[0]!=1 || HgAsmBytes[3]!=4 || HgAsmTarget!=HgAsmBytes+2 || *HgAsmTarget!=3 || HgAsmAlias!=HgAsmBytes+1 || HgAsmOverlay!=7) return FALSE;
    for (u32 i=0;i<16;i++) if (HgAsmZero[i]) return FALSE;
    const u8 sdkBytes[]={4,3,2,1,0x34,0x12,0x55,0x66};
    for (u32 i=0;i<8;i++) if (HgSdkData[i]!=sdkBytes[i] || HgSdkZero[i]) return FALSE;
    if ((u32)HgSdkData%16 || (u32)HgSdkZero%8) return FALSE;
    return TRUE;
}
u32 HGAotAsmRandom(void) { return HgAsmRng(); }
u32 HGAotSpawnWarp(u32 id, u32 kind, u32 field) {
    if (id<1 || id>30 || kind>2 || field>=5) return UINT32_MAX;
    WarpDataPrivate warp;
    if (kind==0) GetDeathWarpData(id,&warp);
    else if (kind==1) GetFlyWarpData(id,&warp);
    else GetSpecialSpawnWarpData(id,&warp);
    u32 values[]={warp.map,(u32)warp.warpId,warp.x,warp.y,warp.direction};
    return values[field];
}
u32 HGAotMovementAllowed(u32 value) {
    u32 record[0x4C/4]={0};
    if (value>UINT8_MAX) return UINT32_MAX;
    ((u8 *)record)[3]=(u8)value;
    return FieldHookUnused_IsMovementAllowed((struct Ov35State *)record);
}
u32 HGAotSequenceIndex(u32 frame) {
    if (frame>UINT16_MAX) return UINT32_MAX;
    const struct {u32 count;u16 start[3];u8 texture[3],palette[3];} raw={3,{0,5,15},{0x10,0x20,0x30},{0xA0,0xB0,0xC0}};
    BillboardGfxSequence sequence;
    BillboardGfxSequence_Init(&raw,&sequence);
    BillboardTexPlttIndex index=BillboardGfxSequence_GetTexPlttIndex(&sequence,(u16)frame);
    return index.textureIdx | ((u32)index.plttIdx<<8);
}
u32 HGAotBottomScreenCallbacks(u32 index) {
    void (*callbacks[])(FieldSystem *)={BottomScreen_None_Init,BottomScreen_None_End};
    if (index>=2) return FALSE;
    callbacks[index](NULL);
    return BottomScreen_None_IsDone(NULL);
}
// Preserve assertion failures in the isolated probe without importing host I/O.
void HGNativeAssertFail(const char *condition, const char *file, int line) { __builtin_trap(); }
void GF_AssertFail(void) { __builtin_trap(); }
BOOL BillboardObject_AdvanceFrame(BillboardObject *,fx32);
u32 HGAotBillboardFrames(void) {
    s32 sequences[2][3]={{1,3,1},{2,4,0}};
    BillboardObject object={0}; object.resourceId=(uaddr)sequences; object.frame=FX32_ONE;
    if (BillboardObject_AdvanceFrame(&object,FX32_ONE) || object.frame!=2*FX32_ONE) return FALSE;
    if (!BillboardObject_AdvanceFrame(&object,2*FX32_ONE) || object.frame!=3*FX32_ONE) return FALSE;
    if (!BillboardObject_AdvanceFrame(&object,FX32_ONE) || object.frame!=3*FX32_ONE) return FALSE;
    object.sequence=1; object.frame=4*FX32_ONE;
    if (BillboardObject_AdvanceFrame(&object,FX32_ONE) || object.frame!=2*FX32_ONE) return FALSE;
    object.frame=0;
    return !BillboardObject_AdvanceFrame(&object,FX32_ONE) && object.frame==2*FX32_ONE;
}
// Test-only SDK stand-ins check original status and host-count return propagation.
static BOOL statusResult, statusForwarded;
BOOL DWC_SetOwnStatusData(const u8 *payload, int size) {
    statusForwarded = payload == (u8 *)data && size == 17;
    return statusResult;
}
int DWC_GetNumConnectionHost(void) { return statusResult ? 3 : 0; }
u32 HGAotWifiStatus(u32 success) {
    statusResult = success != 0; statusForwarded = FALSE;
    BOOL result = WifiConnection_SetOwnStatusData(data,17);
    return statusForwarded && result == statusResult && WifiConnection_GetNumConnectionHost() == (statusResult ? 3 : 0);
}

u32 HGAotLayout(u32 field) {
    switch (field) {
    case 0: return sizeof(void *);
    case 1: return sizeof(BoxPokemon);
    case 2: return sizeof(Pokemon);
    case 3: return sizeof(StringSearch);
    case 4: return sizeof(Iterator);
    case 5: return offsetof(StringSearch, raw.longForm.data);
    case 6: return sizeof(FSFile);
    case 7: return offsetof(WifiSetupArchiveState, lockId);
    case 8: return sizeof(OrbitCursorSprite);
    case 9: return sizeof(OrbitCursor);
    case 10: return sizeof(BillboardResourceData);
    case 11: return sizeof(BillboardTexPlttIndex);
    case 12: return offsetof(EngagedTrainer,approachTask);
    case 13: return sizeof(EngagedTrainer);
    case 14: return offsetof(FieldMapSystems,unk14);
    case 15: return sizeof(FieldMapSystems);
    case 16: return sizeof(FieldEffectBasicTemplate);
    default: return 0;
    }
}
u32 HGAotPointerCheck(u32 offset, u32 size) {
    u32 start = (uaddr)data, end = start + sizeof(data);
    if (offset < start || offset > end || size > 32 || size > end - offset) return FALSE;
    string.raw.longForm.flag = 1; string.raw.longForm.capacity = 32;
    string.raw.longForm.size = size; string.raw.longForm.data = (s8 *)(uaddr)offset;
    iterator.current = &string;
    // Exercise the real 32-bit byte offsets, rather than weakening their layout assertions.
    return *(s8 **)((u8 *)iterator.current + 8) == (s8 *)(uaddr)offset && string.raw.longForm.size == size;
}
u32 HGAotString(void) { return (uaddr)&string; }
u32 HGAotIterator(void) { return (uaddr)&iterator; }
u32 HGAotData(void) { return (uaddr)data; }
static BOOL valid_size(u32 size) { return size <= sizeof(data) && !(size & 1); }
u32 HGAotEncrypt(u32 size, u32 seed) {
    if (!valid_size(size)) return FALSE;
    _MonEncryptSegment(data, size, seed); return TRUE;
}
u32 HGAotDecrypt(u32 size, u32 seed) {
    if (!valid_size(size)) return FALSE;
    _MonDecryptSegment(data, size, seed); return TRUE;
}
u32 HGAotChecksum(u32 size) { return valid_size(size) ? CalcMonChecksum(data, size) : UINT32_MAX; }
u32 HGAotLobbyFind(u32 length, u32 needleLength, u32 start, u32 shortForm) {
    if (length > 128 || needleLength > 128 || (shortForm && (length > 11 || needleLength > 11))) return UINT32_MAX;
    StringSearch haystack = {0}, needle = {0};
    if (shortForm) {
        haystack.raw.shortForm.size = length; needle.raw.shortForm.size = needleLength;
        __builtin_memcpy(haystack.raw.shortForm.data,data,length);
        __builtin_memcpy(needle.raw.shortForm.data,(u8 *)data+128,needleLength);
    } else {
        haystack.raw.longForm.flag = needle.raw.longForm.flag = 1;
        haystack.raw.longForm.size = length; needle.raw.longForm.size = needleLength;
        haystack.raw.longForm.data = (s8 *)data; needle.raw.longForm.data = (s8 *)data+128;
    }
    return (u32)LobbyString_Find(&haystack,&needle,start);
}
typedef struct {u32 a,b;} ProbeEntry;
typedef struct {ProbeEntry *p;} ProbeIterator;
typedef struct {ProbeEntry **map,**begin;u32 unused;ProbeEntry **end;ProbeEntry *current;} ProbePosition;
ProbePosition LobbyEventDeque_UninitializedCopy(ProbeIterator,ProbeIterator,ProbePosition *,u32 *,u32 *);
u32 HGAotDequeCopy(void) {
    ProbeEntry input[4]={{1,2},{3,4},{5,6},{7,8}}, output[128]={{0,0}};
    ProbeEntry *segments[]={output};
    ProbePosition position={segments,segments,0,segments+1,output};
    ProbeIterator first={input},last={input+4}; u32 count=0;
    ProbePosition result=LobbyEventDeque_UninitializedCopy(first,last,&position,NULL,&count);
    if (count!=4 || result.current!=output+4) return FALSE;
    for (u32 i=0;i<4;i++) if (output[i].a!=input[i].a || output[i].b!=input[i].b) return FALSE;
    return TRUE;
}
void HGAotSeed(u32 seed) { SetLCRNGSeed(seed); }
u32 HGAotRandom(void) { return LCRandom(); }
void HGAotMTSeed(u32 seed) { SetMTRNGSeed(seed); }
u32 HGAotMTRandom(void) { return MTRandom(); }
