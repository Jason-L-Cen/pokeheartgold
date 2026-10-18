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
#include "../../../lib/NitroDWC/src/tree-iterator4.h"
#include <stddef.h>

u32 CalcMonChecksum(void *data, u32 size);
static u16 data[128];
static StringSearch string;
static Iterator iterator;
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
u32 HGAotAsmPointers(void) {
    if (HgAsmBytes[0]!=1 || HgAsmBytes[3]!=4 || HgAsmTarget!=HgAsmBytes+2 || *HgAsmTarget!=3 || HgAsmAlias!=HgAsmBytes+1 || HgAsmOverlay!=7) return FALSE;
    for (u32 i=0;i<16;i++) if (HgAsmZero[i]) return FALSE;
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
