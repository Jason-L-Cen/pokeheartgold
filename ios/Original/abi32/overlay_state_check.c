// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/fs/native_overlay.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
extern const HGNativeOverlayUnit *HGTestOverlayUnit(void);
extern const HGNativeOverlayUnit *HGTestRtcUnit(void);
extern const HGNativeOverlayUnit *HGTestAsmOverlayUnit(void);
extern const HGNativeOverlayUnit *HGTestPokewalkerOverlayUnit(void);
extern u32 HgAsmMutable[3];
extern u32 HGAotAsmPointers(void);
extern const u8 HgAsmBytes[];
extern u8 HgAsmZero[16],HgSdkZero[8];
extern u32 HgSdkData;
extern u16 LCRandom(void);
static u32 events[8],eventCount;
static struct { u32 first; u8 bytes[3]; u32 last; } initialized={0x12345678,{1,2,3},0x87654321};
static u8 zero[7];
static u32 *pointer=&initialized.first;
static const u32 untouched=0xABCDEF01;
static u32 localCounter(void) { static u32 counter=37; return counter++; }
__attribute__((constructor(201))) static void constructLater(void) { events[eventCount++]=2; }
__attribute__((constructor(101))) static void constructFirst(void) { events[eventCount++]=1; }
__attribute__((destructor(101))) static void destructLast(void) { events[eventCount++]=4; }
__attribute__((destructor(201))) static void destructFirst(void) { events[eventCount++]=3; }
void HGAsmOverlayConstructor(void) { events[eventCount++]=5; }
u32 CheckNativeOverlayState(void) {
    const HGNativeOverlayUnit *unit=HGTestOverlayUnit(),*rtc=HGTestRtcUnit();
    CHECK(!eventCount && unit->constructorCount==2 && unit->destructorCount==2);
    CHECK(unit->spanCount>=5 && rtc->spanCount==1 && rtc->spans[0].size);
    CHECK(localCounter()==37 && localCounter()==38);
    initialized.first=9; initialized.bytes[2]=8; initialized.last=7; pointer=NULL; memset(zero,0xA5,sizeof(zero));
    CHECK(HGNativeRestoreOverlayUnit(unit));
    CHECK(initialized.first==0x12345678 && initialized.last==0x87654321 && initialized.bytes[2]==3);
    CHECK(pointer==&initialized.first && untouched==0xABCDEF01 && localCounter()==37);
    for (u32 i=0;i<sizeof(zero);i++) CHECK(!zero[i]);
    CHECK(unit->constructors[0].priority==101 && unit->constructors[1].priority==201);
    for (u32 i=0;i<unit->constructorCount;i++) unit->constructors[i].function();
    CHECK(eventCount==2 && events[0]==1 && events[1]==2);
    CHECK(unit->destructors[0].priority==201 && unit->destructors[1].priority==101);
    for (u32 i=0;i<unit->destructorCount;i++) unit->destructors[i].function();
    CHECK(eventCount==4 && events[2]==3 && events[3]==4);
    CHECK(HGNativeRestoreOverlayUnit(unit) && !eventCount && localCounter()==37);
    memset(rtc->spans[0].address,0x5A,rtc->spans[0].size);
    CHECK(HGNativeRestoreOverlayUnit(rtc));
    for (u32 i=0;i<rtc->spans[0].size;i++) CHECK(!((u8 *)rtc->spans[0].address)[i]);
    const HGNativeOverlayUnit *assembly=HGTestAsmOverlayUnit(),*pokewalker=HGTestPokewalkerOverlayUnit();
    CHECK(assembly->constructorCount==1 && !assembly->destructorCount && assembly->spanCount==6 && pokewalker->spanCount==1);
    CHECK(pokewalker->spans[0].size==196);
    for (u32 i=0;i<assembly->spanCount;i++) memset(assembly->spans[i].address,0xA5,assembly->spans[i].size);
    memset(pokewalker->spans[0].address,0x5A,196);
    CHECK(HGNativeRestoreOverlayUnit(assembly) && HGNativeRestoreOverlayUnit(pokewalker));
    CHECK(HGAotAsmPointers() && HgSdkData==0x01020304);
    for (u32 i=0;i<16;i++) CHECK(!HgAsmZero[i]);
    for (u32 i=0;i<8;i++) CHECK(!HgSdkZero[i]);
    for (u32 i=0;i<196;i++) CHECK(!((u8 *)pokewalker->spans[0].address)[i]);
    CHECK(HgAsmMutable[0]==(u32)(uaddr)(HgAsmBytes+2) && HgAsmMutable[1]==(u32)(uaddr)LCRandom && HgAsmMutable[2]==7);
    assembly->constructors[0].function(); CHECK(eventCount==1 && events[0]==5);
    u32 original=initialized.first;
    HGNativeOverlaySpan bad[]={{&initialized.first,NULL,sizeof(u32)},{(void *)(uaddr)0xFFFFFFFF,NULL,4}};
    HGNativeOverlayUnit rejected={2,bad,0,NULL,0,NULL};
    CHECK(!HGNativeRestoreOverlayUnit(&rejected) && initialized.first==original);
    CHECK(!HGNativeRestoreOverlayUnit(NULL));
    HGNativeOverlayUnit empty={0}; CHECK(HGNativeRestoreOverlayUnit(&empty));
    return 0;
}
