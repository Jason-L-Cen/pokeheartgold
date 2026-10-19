// SPDX-License-Identifier: GPL-3.0-or-later
#include "pokepic.h"
#include "pokemon.h"
#include <nitro/gx/native_geometry.h>
#include <nnsys/fnd_internal.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
void GX_InitGXState(void);
extern u32 GXi_DmaId;
static u8 pixels[256*192*4];
static BOOL frame(PokepicManager *manager) {
    HGNativeGeometryReset(); reg_G3X_DISP3DCNT=1;
    G3X_SetClearColor(0,0,32767,0,FALSE); NNS_G2dSetupSoftwareSpriteCamera();
    PokepicManager_DrawAll(manager); G3_SwapBuffers(GX_SORTMODE_AUTO,GX_BUFFERMODE_Z);
    return HGNativeRender3D(1,pixels,sizeof(pixels));
}
static BOOL matches(PokepicManager *manager,u32 golden) {
    Pokepic *pic=&manager->pics[0]; u32 opaque=0,hash=2166136261u;
    // Slot zero holds the original 80x80 frames side by side in the 256x256 atlas.
    for (u32 y=0;y<80;y++) for (u32 x=0;x<80;x++) {
        u32 u=x+80*pic->whichAnimStep;
        u8 packed=manager->charRawData[y*128+u/2],index=(packed>>((u&1)*4))&15;
        const u8 *p=pixels+((y+56)*256+x+88+pic->drawParam.xOffset)*4;
        if (!index) { if (p[3]) return FALSE; }
        else {
            opaque++; u16 color=manager->plttRawData[index];
            if (p[3]!=255 || p[0]>>3!=(color&31) || p[1]>>3!=((color>>5)&31) || p[2]>>3!=((color>>10)&31)) return FALSE;
        }
        u16 color=p[3] ? (p[0]>>3)|((p[1]>>3)<<5)|((p[2]>>3)<<10)|0x8000 : 0;
        hash=(hash^(color&255))*16777619u; hash=(hash^(color>>8))*16777619u;
    }
    u32 actual=0; for (u32 i=0;i<256*192;i++) actual+=pixels[i*4+3]!=0;
    return opaque>500 && actual==opaque && hash==golden;
}
u32 CheckOriginalPokepic(void) {
    u32 savedDma=GXi_DmaId,freeBefore=HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT);
    GX_InitGXState(); GXi_DmaId=(u32)-1;
    GX_SetBankForTex(GX_VRAM_TEX_0_A); GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_0123_E);
    const u16 species[]={152,163};
    // Original male/{back,front}.png indices with front.png's shared normal palette; FNV-1a over RGB555 plus opacity.
    const u32 goldens[2][2][2]={
        {{0x7C393E2A,0xF0EBDC37},{0x4E46858F,0xE4A90A1E}},
        {{0x5EE159C4,0xF1C1BBC5},{0x697B2BA1,0xC679C174}}
    };
    for (u32 i=0;i<2;i++) for (u32 facing=0;facing<2;facing++) {
        HGNativeGeometryReset();
        PokepicManager *manager=PokepicManager_Create(HEAP_ID_DEFAULT); CHECK(manager);
        PokepicTemplate template;
        GetMonSpriteCharAndPlttNarcIdsEx(&template,species[i],MON_MALE,facing ? MON_PIC_FACING_FRONT : MON_PIC_FACING_BACK,0,0,0);
        Pokepic *pic=PokepicManager_CreatePokepic(manager,&template,128,96,0,1,NULL,NULL);
        CHECK(pic && pic->needReloadChar && pic->needReloadPltt);
        PokepicManager_DrawAll(manager);
        CHECK(manager->needLoadImage && manager->needLoadPltt && !pic->needReloadChar && !pic->needReloadPltt);
        PokepicManager_HandleLoadImgAndOrPltt(manager);
        CHECK(!manager->needLoadImage && !manager->needLoadPltt && manager->imageProxy.attr.fmt==GX_TEXFMT_PLTT16);
        CHECK(frame(manager) && matches(manager,goldens[i][facing][0]));
        PokepicAnimScript script[10]={{1,1,3,0},{0,0,-2,0},{-1,0,0,0}};
        Pokepic_SetAnimScript(pic,script); Pokepic_StartAnim(pic);
        CHECK(frame(manager) && pic->animActive && pic->whichAnimStep==1 && pic->drawParam.xOffset==3 && matches(manager,goldens[i][facing][1]));
        CHECK(frame(manager) && pic->animActive && !pic->whichAnimStep && pic->drawParam.xOffset==-2 && matches(manager,goldens[i][facing][0]));
        CHECK(frame(manager) && !pic->animActive && !pic->whichAnimStep && !pic->drawParam.xOffset && matches(manager,goldens[i][facing][0]));
        Pokepic_Delete(pic); CHECK(frame(manager));
        for (u32 pixel=0;pixel<256*192;pixel++) CHECK(!pixels[pixel*4+3]);
        PokepicManager_Delete(manager);
        CHECK(HeapExp_FndGetTotalFreeSize(HEAP_ID_DEFAULT)==freeBefore);
    }
    GXi_DmaId=savedDma; HGNativeGeometryReset(); return 0;
}
