// SPDX-License-Identifier: GPL-3.0-or-later
#define Heap_Alloc BackgroundCheckAlloc
#define Sprite_GetImageProxy BackgroundCheckImage
#define PaletteData_GetUnfadedBuf BackgroundCheckPalette
#define BG_LoadCharTilesData BackgroundCheckLoad
#define BattleTerrain_Free BackgroundCheckFreeTerrain
#include "../../../src/battle/battle_system.c"
#include <nitro/hw/native_graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static NNSG2dImageProxy images[2];
static _Alignas(4) u16 colors[256];
static u32 loads,freed;

void *BackgroundCheckAlloc(enum HeapID heap,u32 size) { if (heap!=HEAP_ID_BATTLE) abort(); void *p=calloc(1,size); if (!p) abort(); return p; }
NNSG2dImageProxy *BackgroundCheckImage(Sprite *sprite) { return (NNSG2dImageProxy *)sprite; }
u16 *BackgroundCheckPalette(PaletteData *data,PaletteBufferId buffer) { if (buffer!=PLTTBUF_MAIN_BG) abort(); return colors; }
void BackgroundCheckLoad(BgConfig *config,u8 layer,const void *data,u32 size,u32 tile) { if (layer!=3 || size!=0x10000 || tile) abort(); loads++; }
void BackgroundCheckFreeTerrain(BattleTerrain *terrain) { freed++; }

u32 CheckOriginalBattleBackground(void) {
    vu8 *a=(vu8 *)HGNativeRegisterAddress(0x04000240,1),*e=(vu8 *)HGNativeRegisterAddress(0x04000244,1);
    u8 oldA=*a,oldE=*e; *a=0x81; *e=0x82;
    memset((void *)HGNativeGraphicsBank(0),0x44,0x20000);
    for (u32 i=0;i<256;i++) colors[i]=(u16)(0x1000+i);
    const u8 patterns[][2]={{0x21,0x43},{0x10,0x20},{0,0}};
    images[0].vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]=0;
    images[1].vramLocation.baseAddrOfVram[NNS_G2D_VRAM_TYPE_2DMAIN]=0xF000;
    for (u32 test=0;test<3;test++) {
        BattleSystem battle; ManagedSprite sprites[2];
        memset(&battle,0,sizeof(battle)); memset(sprites,0,sizeof(sprites));
        for (u32 i=0;i<2;i++) {
            sprites[i].sprite=(Sprite *)&images[i]; battle.terrains[i].sprite=&sprites[i];
            memset((u8 *)HGNativeGraphicsBank(4)+(i ? 0xF000 : 0),patterns[test][i],0x1000);
        }
        loads=freed=0; BattleSystem_SetBackground(&battle);
        CHECK(loads==1 && freed==2 && !memcmp(battle.bgPalette,colors,sizeof(colors)));
        for (u32 offset=0;offset<0x10000;offset++) {
            u32 x=(offset/64)%32,y=offset/0x800; int terrain=-1;
            if (y>=20 && y<28 && x>=16) terrain=1;
            else if ((offset>=0x9800 && offset<0xA000) || (y>=28 && x<24)) terrain=0;
            u8 color=terrain<0 ? 0 : (patterns[test][terrain]>>((offset&1)*4))&15;
            CHECK(battle.bgTiles[offset]==(color ? color+0x70 : 0x44));
        }
        free(battle.bgTiles); free(battle.bgPalette);
    }
    *a=oldA; *e=oldE;
    return 0;
}

#ifndef HEARTGOLD_ABI32
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
int main(void) {
    u32 line=CheckOriginalBattleBackground();
    if (line) { fprintf(stderr,"Battle background check line %u\n",line); return 1; }
    puts("PASS: original battle terrain composition, mapped OBJ bank end, palette and transparent pixels.");
    return 0;
}
#endif
