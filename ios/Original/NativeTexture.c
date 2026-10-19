// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/gx/native_texture.h>
#include <nitro/hw/native_graphics.h>

static u8 byteAt(BOOL palette,u32 address) {
    u8 value=0; HGNativeReadTextureMemory(palette,address&(palette ? 0x1FFFF : 0x7FFFF),&value,1); return value;
}
static u16 halfAt(BOOL palette,u32 address) {
    return (u16)(byteAt(palette,address)|((u32)byteAt(palette,address+1)<<8));
}
static u32 coordinate(s32 value,u32 size,BOOL repeat,BOOL flip) {
    s64 pixel=value/16; if (value<0 && value%16) pixel--;
    if (!repeat) return pixel<0 ? 0 : pixel>=size ? size-1 : (u32)pixel;
    s64 period=flip ? size*2 : size;
    pixel%=period; if (pixel<0) pixel+=period;
    return (u32)(flip && pixel>=size ? period-1-pixel : pixel);
}
static u16 mix(u16 a,u16 b,u32 weightA,u32 weightB,u32 shift) {
    u16 value=0;
    for (u32 component=0;component<3;component++) {
        u32 x=(a>>(component*5))&31,y=(b>>(component*5))&31;
        value|=(u16)(((x*weightA+y*weightB)>>shift)<<(component*5));
    }
    return value;
}
BOOL HGNativeSampleTexture(u32 texture,u32 palette,s32 s,s32 t,u16 *color,u16 *alpha) {
    if (!color || !alpha || (uaddr)color%2 || (uaddr)alpha%2) return FALSE;
    u32 format=(texture>>26)&7,width=8u<<((texture>>20)&7),height=8u<<((texture>>23)&7);
    u32 x=coordinate(s,width,texture&(1u<<16),texture&(1u<<18));
    u32 y=coordinate(t,height,texture&(1u<<17),texture&(1u<<19));
    u32 texel=y*width+x,address=(texture&0xFFFF)*8,paletteBase=(palette&0x1FFF)<<(format==2 ? 3 : 4);
    u32 index=0; u16 result=0x7FFF,opacity=31;
    switch (format) {
    case 0: break;
    case 1: {
        u32 value=byteAt(FALSE,address+texel); index=value&31;
        opacity=(u16)(((value>>5)<<2)+(value>>6)); result=halfAt(TRUE,paletteBase+index*2); break;
    }
    case 2: case 3: case 4: {
        if (format==2) index=(byteAt(FALSE,address+texel/4)>>((texel&3)*2))&3;
        else if (format==3) index=(byteAt(FALSE,address+texel/2)>>((texel&1)*4))&15;
        else index=byteAt(FALSE,address+texel);
        result=halfAt(TRUE,paletteBase+index*2);
        if (!index && (texture&(1u<<29))) opacity=0;
        break;
    }
    case 5: {
        u32 block=((y/4)*(width/4)+x/4)*4;
        u32 data=(address+block)&0x7FFFF;
        u32 descriptor=0x20000+((data&0x1FFFC)>>1)+(data>=0x40000 ? 0x10000 : 0);
        u16 info=halfAt(FALSE,descriptor); u32 mode=info>>14,base=paletteBase+(info&0x3FFF)*4;
        index=(data>=0x20000 && data<0x40000) ? 0 : (byteAt(FALSE,data+(y&3))>>((x&3)*2))&3;
        u16 first=halfAt(TRUE,base),second=halfAt(TRUE,base+2);
        if (index<2) result=index ? second : first;
        else if (mode==2 || (!mode && index==2)) result=halfAt(TRUE,base+index*2);
        else if (mode==1 && index==2) result=mix(first,second,1,1,1);
        else if (mode==3) result=index==2 ? mix(first,second,5,3,3) : mix(first,second,3,5,3);
        else { result=0; opacity=0; }
        break;
    }
    case 6: {
        u32 value=byteAt(FALSE,address+texel); result=halfAt(TRUE,paletteBase+(value&7)*2); opacity=(u16)(value>>3); break;
    }
    case 7: result=halfAt(FALSE,address+texel*2); opacity=(result&0x8000) ? 31 : 0; break;
    }
    *color=result&0x7FFF; *alpha=opacity; return TRUE;
}
