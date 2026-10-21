// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/hw/native_graphics.h>
#include "../NativeCompression.h"
#include "filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(v) do { if (!(v)) { fprintf(stderr, "Render check line %d: %s\n", __LINE__, #v); abort(); } } while (0)
static u8 frame[256 * 192 * 4 + 16];
static void pixel(unsigned x, unsigned y, u16 expected) {
    const u8 *p = frame + (y * 256 + x) * 4;
    CHECK(p[3] == 255 && (p[0] >> 3) == (expected & 31) && (p[1] >> 3) == ((expected >> 5) & 31) && (p[2] >> 3) == ((expected >> 10) & 31));
}
static void draw(unsigned engine) { CHECK(HGNativeRender2D(engine, frame, sizeof(frame))); }
static void checkBattleView(void) {
    static u8 wide[1024*192*4+16];
    _Alignas(4) u16 bgPalette[4]={0,31,0x7C00,0x7FFF},objPalette[4]={0,0x3E0,0,0};
    _Alignas(4) u16 map[2048],front[1024],oam[512]={0};
    _Alignas(4) u8 tiles[96],frontTiles[64]={0};
    memset(tiles,0x11,32); memset(tiles+32,0x22,32); memset(tiles+64,0,32); memset(frontTiles,0x33,32);
    for (u32 y=0;y<32;y++) for (u32 x=0;x<64;x++) map[(x/32)*1024+y*32+x%32]=x%3;
    for (u32 i=0;i<1024;i++) front[i]=1;
    front[3*32+2]=0;
    for (u32 i=0;i<128;i++) oam[i*4]=0x200;
    oam[0]=16; oam[1]=508; oam[3]=oam[15]=256;
    GX_SetBankForBG(GX_VRAM_BG_128_A); GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    reg_G2_BG3CNT=0x4803; reg_G2_BG1CNT=0xC08; reg_G2_BG3OFS=13; reg_G2_BG1OFS=0;
    reg_G2_BLDCNT=reg_GX_MASTER_BRIGHT=0;
    GX_LoadBGPltt(bgPalette,0,sizeof(bgPalette)); GX_LoadOBJPltt(objPalette,0,sizeof(objPalette));
    GX_LoadBG3Char(tiles,0,sizeof(tiles)); GX_LoadBG3Scr(map,0,sizeof(map));
    GX_LoadBG1Char(frontTiles,0,sizeof(frontTiles)); GX_LoadBG1Scr(front,0,sizeof(front)); GX_LoadOBJ(tiles,0,32);
    const u32 widths[]={256,384,628,1024};
    for (u32 affine=0;affine<2;affine++) for (u32 effect=0;effect<2;effect++) {
        oam[0]=16|(affine ? 0x100 : 0); GX_LoadOAM(oam,0,sizeof(oam));
        reg_GX_DISPCNT=0x11A10|(effect ? 0x2000 : 0);
        reg_G2_WIN0H=(20<<8)|80; reg_G2_WIN0V=191; reg_G2_WININ=63; reg_G2_WINOUT=8|32;
        reg_GX_MASTER_BRIGHT=effect ? 0x8008 : 0; draw(0);
        for (u32 n=0;n<sizeof(widths)/sizeof(widths[0]);n++) {
            u32 width=widths[n],pad=(width-256)/2,bytes=width*192*4;
            memset(wide,0xA5,sizeof(wide)); CHECK(HGNativeRenderBattleView(width,wide+8,bytes));
            for (u32 y=0;y<192;y++) CHECK(!memcmp(wide+8+(y*width+pad)*4,frame+y*256*4,256*4));
            for (u32 i=0;i<8;i++) CHECK(wide[i]==0xA5 && wide[8+bytes+i]==0xA5);
            if (!effect) {
                for (u32 x=0;x<width;x++) {
                    u32 tile=((x-pad+13)&511)/8;
                    u16 expected=tile%3==0 ? 31 : tile%3==1 ? 0x7C00 : 0;
                    const u8 *p=wide+8+(8*width+x)*4;
                    CHECK((p[0]>>3)==(expected&31) && (p[1]>>3)==((expected>>5)&31) && (p[2]>>3)==(expected>>10) && p[3]==255);
                }
                if (pad) { const u8 *p=wide+8+(16*width+pad-1)*4; CHECK(!p[0] && p[1]==255 && !p[2]); }
            }
        }
    }
    memset(wide,0xA5,sizeof(wide));
    CHECK(!HGNativeRenderBattleView(255,wide+8,sizeof(wide)-16) && !HGNativeRenderBattleView(1026,wide+8,sizeof(wide)-16));
    CHECK(!HGNativeRenderBattleView(628,wide+8,628*192*4-1));
    for (u32 i=0;i<sizeof(wide);i++) CHECK(wide[i]==0xA5);
    reg_GX_DISPCNT=0x80; CHECK(HGNativeRenderBattleView(1024,wide+8,1024*192*4));
    for (u32 i=0;i<1024*192*4;i++) CHECK(wide[8+i]==255);
    puts("PASS: original battle background sampling, unclipped normal/affine OBJ edges, unchanged center/window/brightness, 1024-wide output and guards.");
}

void CheckNative2D(void) {
    const u8 lz10[] = {0x10,9,0,0,0x10,'A','B','C',0x30,2}; u8 decoded[300];
    CHECK(HGNativeUncompressLZ(lz10, sizeof(lz10), decoded, sizeof(decoded)) && memcmp(decoded, "ABCABCABC", 9) == 0);
    for (unsigned i = 0; i < sizeof(lz10); i++) CHECK(!HGNativeUncompressLZ(lz10, i, decoded, sizeof(decoded)));
    CHECK(!HGNativeUncompressLZ(lz10, sizeof(lz10), decoded, 8));
    const u8 short11[] = {0x11,4,0,0,0x40,'A',0x20,0}, medium11[] = {0x11,18,0,0,0x40,'A',0,0,0};
    const u8 long11[] = {0x11,0x12,1,0,0x40,'A',0x10,0,0,0};
    const u8 bad[] = {0x10,3,0,0,0x80,0,0};
    CHECK(HGNativeUncompressLZ(short11, sizeof(short11), decoded, sizeof(decoded)));
    CHECK(HGNativeUncompressLZ(medium11, sizeof(medium11), decoded, sizeof(decoded)));
    CHECK(HGNativeUncompressLZ(long11, sizeof(long11), decoded, sizeof(decoded)));
    for (unsigned i = 0; i < 274; i++) CHECK(decoded[i] == 'A');
    for (unsigned i = 0; i < sizeof(long11); i++) CHECK(!HGNativeUncompressLZ(long11,i,decoded,sizeof(decoded)));
    CHECK(!HGNativeUncompressLZ(bad, sizeof(bad), decoded, sizeof(decoded)));
    GX_SetBankForBG(GX_VRAM_BG_128_A); GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    _Alignas(4) u16 palette[256] = {0x7C00, 31, 0x3E0, 0x7FFF};
    _Alignas(4) u16 map[1024] = {0}, oam[512] = {0};
    _Alignas(4) u8 tile[64]; memset(tile, 0x11, 32);
    for (unsigned i = 0; i < 128; i++) oam[i * 4] = 0x200;
    GX_LoadOAM(oam, 0, sizeof(oam)); GX_LoadBGPltt(palette, 0, sizeof(palette));
    reg_GX_DISPCNT = 0x10100; reg_G2_BG0CNT = 0x100; reg_GX_MASTER_BRIGHT = 0;
    reg_G2_BG0OFS = 0; reg_G2_BLDCNT = 0;
    GX_LoadBG0Char(tile, 0, 32); GX_LoadBG0Scr(map, 0, sizeof(map));
    draw(0); pixel(0,0,31); pixel(255,191,31);
    tile[0] = 0x10; tile[3] = 0x22; GX_LoadBG0Char(tile, 0, 32);
    draw(0); pixel(0,0,0x7C00); pixel(7,0,0x3E0);
    map[0] = 0x400; GX_LoadBG0Scr(map, 0, 2); draw(0); pixel(0,0,0x3E0); pixel(7,0,0x7C00);
    map[0] = 0; GX_LoadBG0Scr(map, 0, 2); G2_SetBG0Offset(1,0); draw(0); pixel(0,0,31);
    G2_SetBG0Offset(0,0); reg_G2_BG0CNT |= 0x40; reg_G2_MOSAIC = 3;
    draw(0); pixel(1,0,0x7C00); reg_G2_BG0CNT &= ~0x40;

    memset(tile, 1, sizeof(tile)); reg_G2_BG0CNT = 0x180; GX_LoadBG0Char(tile, 0, sizeof(tile));
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_0123_FG);
    palette[1] = 0x3FF; GX_BeginLoadBGExtPltt(); GX_LoadBGExtPltt(palette, 0x4000 + 15 * 512, 512); GX_EndLoadBGExtPltt();
    reg_G2_BG0CNT |= 0x2000; map[0] = 0xF000; GX_LoadBG0Scr(map, 0, 2);
    draw(0); pixel(0,0,0x3FF); G2_SetBG0Offset(1,0); draw(0); pixel(0,0,0x3FF); G2_SetBG0Offset(0,0);
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_NONE);
    palette[1] = 31; GX_LoadBGPltt(palette, 0, 512); reg_G2_BG0CNT = 0x100;
    memset(tile, 0x11, 32); GX_LoadBG0Char(tile, 0, 32); map[0] = 0; GX_LoadBG0Scr(map, 0, 2);

    palette[1] = 0x3E0; GX_LoadOBJPltt(palette, 0, 512); GX_LoadOBJ(tile, 0, 32);
    oam[0] = 12; oam[1] = 20; oam[2] = 0; GX_LoadOAM(oam, 0, sizeof(oam));
    reg_GX_DISPCNT |= 0x1010; draw(0); pixel(20,12,0x3E0); pixel(19,12,31);
    tile[0]=0x10; GX_LoadOBJ(tile,0,32); draw(0); pixel(20,12,31); pixel(21,12,0x3E0);
    tile[0]=0x11; GX_LoadOBJ(tile,0,32);
    oam[2] = 0x400; GX_LoadOAM(oam, 0, 8); draw(0); pixel(20,12,31);
    oam[2] = 0; oam[0] |= 0x100; oam[3] = 256; oam[15] = 256; GX_LoadOAM(oam, 0, sizeof(oam));
    draw(0); pixel(20,12,0x3E0);
    tile[0]=0x10; GX_LoadOBJ(tile,0,32); draw(0); pixel(20,12,31); pixel(21,12,0x3E0);
    tile[0]=0x11; GX_LoadOBJ(tile,0,32);
    oam[0] |= 0x200; GX_LoadOAM(oam, 0, 8); draw(0); pixel(20,12,31); pixel(24,16,0x3E0);
    oam[0] = 0x2000 | 12; oam[1] = 20; oam[2] = 1; memset(tile,1,32); memset(tile+32,2,32);
    GX_LoadOBJ(tile,0,64); GX_LoadOAM(oam,0,8); reg_GX_DISPCNT &= ~0x10;
    draw(0); pixel(20,12,0x3E0); // 2D 8bpp ignores the lowest tile-number bit.
    tile[0]=0; GX_LoadOBJ(tile,0,64); draw(0); pixel(20,12,31); pixel(21,12,0x3E0);

    reg_GX_DISPCNT = 0x10500; reg_G2_BG2CNT = 0x100;
    MtxFx22 identity = {FX32_ONE,0,0,FX32_ONE}; G2_SetBG2Affine(&identity,0,0,0,0);
    memset(tile,1,64); GX_LoadBG2Char(tile,0,64); memset(map,0,sizeof(map)); GX_LoadBG2Scr(map,0,256);
    reg_GX_DISPCNT = 0x10402; draw(0); pixel(0,0,31);
    reg_G2_BG2X = 0x0FFFFF00; draw(0); pixel(0,0,0x7C00); pixel(1,0,31);
    reg_G2_BG2CNT |= 0x2000; draw(0); pixel(0,0,31);
    reg_G2_BG2X = 0; reg_G2_BG2CNT = 0x84; reg_GX_DISPCNT = 0x10405;
    _Alignas(4) u16 bitmap[128*128] = {0x801F}; GX_LoadBG2Scr(bitmap,0,sizeof(bitmap));
    draw(0); pixel(0,0,31); pixel(1,0,0x7C00);

    reg_GX_DISPCNT = 0x10300; reg_G2_BG0CNT = 0x100; reg_G2_BG1CNT = 0x205;
    memset(tile,0x11,32); GX_LoadBG0Char(tile,0,32); memset(tile,0x22,32); GX_LoadBG1Char(tile,0,32);
    GX_LoadBG0Scr(map,0,sizeof(map)); GX_LoadBG1Scr(map,0,sizeof(map));
    palette[1] = 31; palette[2] = 0x7C00; GX_LoadBGPltt(palette,0,512);
    G2_SetBlendAlpha(1,2,8,8); draw(0); pixel(0,0,15 | (15<<10));
    reg_G2_BLDCNT = 0; reg_GX_DISPCNT |= 0x2000; G2_SetWnd0Position(10,0,20,10);
    reg_G2_WININ = 0x21; reg_G2_WINOUT = 0x22; draw(0); pixel(9,0,0x7C00); pixel(10,0,31);
    G2_SetWnd0Position(250,0,4,10); draw(0); pixel(0,0,31); pixel(249,0,0x7C00); pixel(255,0,31);
    reg_GX_DISPCNT |= 0x4000; G2_SetWnd1Position(0,0,100,100); reg_G2_WININ = 0x2221;
    draw(0); pixel(0,0,31); pixel(50,50,0x7C00);
    reg_GX_DISPCNT = 0x19310; reg_G2_WINOUT = 0x2122;
    oam[0] = 0x800 | 12; oam[1] = 20; oam[2] = 0; GX_LoadOAM(oam,0,sizeof(oam));
    memset(tile,0x11,32); GX_LoadOBJ(tile,0,32); draw(0); pixel(20,12,31); pixel(19,12,0x7C00);
    reg_GX_DISPCNT = 0x11310; oam[0] = 0x400 | 12;
    oam[4] = 12; oam[5] = 20; oam[6] = 1; GX_LoadOAM(oam,0,sizeof(oam));
    memset(tile+32,0x33,32); GX_LoadOBJ(tile,0,64); G2_SetBlendAlpha(0,1,8,8);
    draw(0); pixel(20,12,15 | (15<<5)); // The second OBJ is not an alpha-blend target behind the first OBJ.
    oam[0] = 0xC00 | 12; oam[2] = 7 << 12; oam[4] = 0x200; GX_LoadOAM(oam,0,sizeof(oam));
    _Alignas(4) u16 objectBitmap[64]; for (unsigned i=0;i<64;i++) objectBitmap[i]=0x83E0;
    GX_LoadOBJ(objectBitmap,0,sizeof(objectBitmap)); reg_GX_DISPCNT = 0x11340;
    draw(0); pixel(20,12,15 | (15<<5));
    objectBitmap[0]=0; GX_LoadOBJ(objectBitmap,0,sizeof(objectBitmap)); draw(0); pixel(20,12,31); pixel(21,12,15 | (15<<5));
    reg_GX_DISPCNT = 0x10300; reg_G2_BLDCNT = 0;
    reg_GX_MASTER_BRIGHT = 0x8010; draw(0); pixel(0,0,0);
    reg_GX_MASTER_BRIGHT = 0;
    memset(frame,0xA5,sizeof(frame)); reg_GX_DISPCNT = 0x10108;
    CHECK(!HGNativeRender2D(0,frame,sizeof(frame)) && frame[0] == 0xA5);
    CHECK(!HGNativeRender2D(2,frame,sizeof(frame)) && !HGNativeRender2D(0,frame,10));
    CHECK(frame[256*192*4] == 0xA5);
    reg_GX_DISPCNT = 0x80; draw(0); pixel(0,0,0x7FFF);
    CHECK(frame[256*192*4] == 0xA5);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_32_H); reg_GXS_DB_DISPCNT = 0x10100; reg_G2S_DB_BG0CNT = 0x100;
    reg_G2S_DB_BG0OFS = reg_G2S_DB_BLDCNT = reg_GXS_DB_MASTER_BRIGHT = 0;
    memset(tile,0x11,32); GXS_LoadBG0Char(tile,0,32); GXS_LoadBG0Scr(map,0,sizeof(map));
    palette[1] = 0x3E0; GXS_LoadBGPltt(palette,0,512); draw(1); pixel(0,0,0x3E0);
    checkBattleView();
    printf("PASS: native 2D tile/palette/flip/scroll/mosaic, OBJ priority/affine, bitmap, window/blend/brightness and bounded LZ10/LZ11.\n");
}

static u32 word(const u8 *data) { return data[0] | (data[1]<<8) | (data[2]<<16) | ((u32)data[3]<<24); }
static u8 *member(NARC *narc, u32 id, u32 *size) {
    *size = GetNarcMemberSizeByIdPair(NARC_a_0_0_7,id); CHECK(*size >= 4);
    u8 *raw = malloc(*size); CHECK(raw); NARC_ReadWholeMember(narc,id,raw);
    if (raw[0] != 0x10 && raw[0] != 0x11) return raw;
    u32 decodedSize = word(raw) >> 8; CHECK(decodedSize && decodedSize < 1048576);
    u8 *decoded = malloc(decodedSize); CHECK(decoded);
    CHECK(HGNativeUncompressLZ(raw,*size,decoded,decodedSize)); free(raw); *size = decodedSize; return decoded;
}
void CheckNative2DOriginalAssets(void) {
    NARC *narc = NARC_New(NARC_a_0_0_7,HEAP_ID_DEFAULT); CHECK(narc);
    u32 charSize, mapSize; u8 *chars = member(narc,3,&charSize), *mapData = member(narc,2,&mapSize);
    CHECK(charSize >= 48 && mapSize >= 36 && !memcmp(chars,"RGCN",4) && !memcmp(mapData,"RCSN",4));
    u32 charBytes = word(chars+40), mapBytes = word(mapData+32); unsigned bpp = chars[28] == 3 ? 4 : 8;
    CHECK(charBytes <= charSize-48 && charBytes <= 65536 && mapBytes <= mapSize-36 && mapBytes == 4096);
    GX_SetBankForBG(GX_VRAM_BG_128_A); reg_GX_DISPCNT = 0x10100; reg_G2_BG0CNT = 0x4010 | (bpp == 8 ? 0x80 : 0);
    reg_G2_BLDCNT = reg_GX_MASTER_BRIGHT = 0;
    GX_LoadBG0Char(chars+48,0,charBytes); GX_LoadBG0Scr(mapData+36,0,mapBytes);
    for (unsigned time = 176; time <= 178; time++) {
        u32 paletteSize; u8 *pal = member(narc,time,&paletteSize);
        CHECK(paletteSize >= 40 && !memcmp(pal,"RLCN",4) && word(pal+32) <= paletteSize-40 && word(pal+32) <= 512);
        GX_LoadBGPltt(pal+40,0,word(pal+32));
        for (unsigned scrollX = 0; scrollX <= 256; scrollX += 256) for (unsigned scrollY = 0; scrollY <= 64; scrollY += 64) {
            G2_SetBG0Offset(scrollX,scrollY); draw(0);
            const char *output = getenv("HG_NATIVE_RENDER_FRAME");
            if (output && time == 176 && !scrollX && !scrollY) {
                FILE *file = fopen(output,"wb"); CHECK(file); CHECK(fprintf(file,"P6\n256 192\n255\n") > 0);
                for (unsigned i=0;i<256*192;i++) CHECK(fwrite(frame+i*4,1,3,file) == 3);
                CHECK(fclose(file) == 0);
            }
            for (unsigned y = 0; y < 192; y++) for (unsigned x = 0; x < 256; x++) {
                unsigned px=x+scrollX, py=y+scrollY;
                unsigned at=(px/256*1024 + py/8*32 + px%256/8)*2; CHECK(at+2 <= mapBytes);
                u16 entry=mapData[36+at] | (mapData[37+at]<<8);
                unsigned tx=px%8, ty=py%8; if (entry&1024) tx=7-tx; if (entry&2048) ty=7-ty;
                unsigned offset=(entry&1023)*(bpp*8)+ty*bpp+tx*bpp/8; CHECK(offset < charBytes);
                unsigned index=chars[48+offset]; if (bpp==4) index=(index>>(4*(tx%2)))&15;
                unsigned colorIndex=bpp==4 ? (entry>>12)*16+index : index;
                if (!index) colorIndex=0;
                CHECK(colorIndex*2+2 <= word(pal+32));
                pixel(x,y,pal[40+colorIndex*2] | (pal[41+colorIndex*2]<<8));
            }
        }
        free(pal);
    }
    free(chars); free(mapData); NARC_Delete(narc);
    printf("PASS: original ROM day/evening/night NCGR/NCLR/NSCR backgrounds match all decoded 512x256 pixels.\n");
}
