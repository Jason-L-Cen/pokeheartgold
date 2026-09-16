#include <nitro/code32.h>
#include <nitro/hw/io_reg.h>
#include <nitro/os/ARM9/vramExclusive.h>
#include <nitro/types.h>

struct GX_State {
    u16 lcdc;
    u16 bg;
    u16 obj;
    u16 arm7;
    u16 tex;
    u16 texPltt;
    u16 clrImg;
    u16 bgExtPltt;
    u16 objExtPltt;
    u16 sub_bg;
    u16 sub_obj;
    u16 sub_bgExtPltt;
    u16 sub_objExtPltt;
};

void GX_VRAMCNT_SetLCDC_(u32 mask);
void GX_SetBankForBG(s32 bg);
void GX_SetBankForOBJ(s32 obj);
void GX_SetBankForBGExtPltt(s32 bgextpltt);
void GX_SetBankForOBJExtPltt(s32 objextpltt);
void GX_SetBankForTex(s32 tex);

extern u16 GXi_VRamLockId;
extern struct GX_State gGXState;

void GX_VRAMCNT_SetLCDC_(u32 mask) {
    if (mask & (0x1 << 0)) {
        reg_GX_VRAMCNT_A = 0x80;
    }
    if (mask & (0x1 << 1)) {
        reg_GX_VRAMCNT_B = 0x80;
    }
    if (mask & (0x1 << 2)) {
        reg_GX_VRAMCNT_C = 0x80;
    }
    if (mask & (0x1 << 3)) {
        reg_GX_VRAMCNT_D = 0x80;
    }
    if (mask & (0x1 << 4)) {
        reg_GX_VRAMCNT_E = 0x80;
    }
    if (mask & (0x1 << 5)) {
        reg_GX_VRAMCNT_F = 0x80;
    }
    if (mask & (0x1 << 6)) {
        reg_GX_VRAMCNT_G = 0x80;
    }
    if (mask & (0x1 << 7)) {
        reg_GX_VRAMCNT_H = 0x80;
    }
    if (mask & (0x1 << 8)) {
        reg_GX_VRAMCNT_I = 0x80;
    }
}

void GX_SetBankForBG(s32 bg) {
    gGXState.lcdc = (u16)(~bg & (gGXState.lcdc | gGXState.bg));
    gGXState.bg = (u16)bg;
    switch (bg) {
    case 8:
        reg_GX_VRAMCNT_D = 0x81;
        break;
    case 12:
        reg_GX_VRAMCNT_D = 0x89;
    case 4:
        reg_GX_VRAMCNT_C = 0x81;
        break;
    case 14:
        reg_GX_VRAMCNT_D = 0x91;
    case 6:
        reg_GX_VRAMCNT_C = 0x89;
    case 2:
        reg_GX_VRAMCNT_B = 0x81;
        break;
    case 15:
        reg_GX_VRAMCNT_D = 0x99;
    case 7:
        reg_GX_VRAMCNT_C = 0x91;
    case 3:
        reg_GX_VRAMCNT_B = 0x89;
    case 1:
        reg_GX_VRAMCNT_A = 0x81;
        break;
    case 11:
        reg_GX_VRAMCNT_A = 0x81;
        reg_GX_VRAMCNT_B = 0x89;
        reg_GX_VRAMCNT_D = 0x91;
        break;
    case 13:
        reg_GX_VRAMCNT_D = 0x91;
    case 5:
        reg_GX_VRAMCNT_A = 0x81;
        reg_GX_VRAMCNT_C = 0x89;
        break;
    case 9:
        reg_GX_VRAMCNT_A = 0x81;
        reg_GX_VRAMCNT_D = 0x89;
        break;
    case 10:
        reg_GX_VRAMCNT_B = 0x81;
        reg_GX_VRAMCNT_D = 0x89;
        break;
    case 112:
        reg_GX_VRAMCNT_G = 0x99;
    case 48:
        reg_GX_VRAMCNT_F = 0x91;
    case 16:
        reg_GX_VRAMCNT_E = 0x81;
        break;
    case 80:
        reg_GX_VRAMCNT_G = 0x91;
        reg_GX_VRAMCNT_E = 0x81;
        break;
    case 96:
        reg_GX_VRAMCNT_G = 0x89;
    case 32:
        reg_GX_VRAMCNT_F = 0x81;
        break;
    case 64:
        reg_GX_VRAMCNT_G = 0x81;
        break;
    default:
        break;
    }
    GX_VRAMCNT_SetLCDC_(gGXState.lcdc);
}

void GX_SetBankForOBJ(s32 obj) {
    gGXState.lcdc = (u16)(~obj & (gGXState.lcdc | gGXState.obj));
    gGXState.obj = (u16)obj;
    switch (obj) {
    case 3:
        reg_GX_VRAMCNT_B = 0x8A;
    case 1:
        reg_GX_VRAMCNT_A = 0x82;
    case 0: // needed to match
        break;
    case 2:
        reg_GX_VRAMCNT_B = 0x82;
        break;
    case 112:
        reg_GX_VRAMCNT_G = 0x9A;
    case 48:
        reg_GX_VRAMCNT_F = 0x92;
    case 16:
        reg_GX_VRAMCNT_E = 0x82;
        break;
    case 80:
        reg_GX_VRAMCNT_G = 0x92;
        reg_GX_VRAMCNT_E = 0x82;
        break;
    case 96:
        reg_GX_VRAMCNT_G = 0x8A;
    case 32:
        reg_GX_VRAMCNT_F = 0x82;
        break;
    case 64:
        reg_GX_VRAMCNT_G = 0x82;
        break;
    default:
        break;
    }
    GX_VRAMCNT_SetLCDC_(gGXState.lcdc);
}

void GX_SetBankForBGExtPltt(s32 bgextpltt) {
    gGXState.lcdc = (u16)(~bgextpltt & (gGXState.lcdc | gGXState.bgExtPltt));
    gGXState.bgExtPltt = (u16)bgextpltt;
    switch (bgextpltt) {
    case 0x10:
        reg_GX_DISPCNT |= 0x40000000;
        reg_GX_VRAMCNT_E = 0x84;
        break;
    case 0x40:
        reg_GX_DISPCNT |= 0x40000000;
        reg_GX_VRAMCNT_G = 0x8C;
        break;
    case 0x60:
        reg_GX_VRAMCNT_G = 0x8C;
    case 0x20:
        reg_GX_VRAMCNT_F = 0x84;
        reg_GX_DISPCNT |= 0x40000000;
        break;
    case 0:
        reg_GX_DISPCNT &= ~0x40000000;
        break;
    }
    GX_VRAMCNT_SetLCDC_(gGXState.lcdc);
}

void GX_SetBankForOBJExtPltt(s32 objextpltt) {
    gGXState.lcdc = (u16)(~objextpltt & (gGXState.lcdc | gGXState.objExtPltt));
    gGXState.objExtPltt = (u16)objextpltt;
    switch (objextpltt) {
    case 32:
        reg_GX_DISPCNT |= 0x80000000;
        reg_GX_VRAMCNT_F = 0x85;
        break;
    case 64:
        reg_GX_DISPCNT |= 0x80000000;
        reg_GX_VRAMCNT_G = 0x85;
        break;
    case 0:
        reg_GX_DISPCNT &= ~0x80000000;
        break;
    }
    GX_VRAMCNT_SetLCDC_(gGXState.lcdc);
}

void GX_SetBankForTex(s32 tex) {
    gGXState.lcdc = (u16)(~tex & (gGXState.lcdc | gGXState.tex));
    gGXState.tex = (u16)tex;
    if (tex == 0) {
        reg_G3X_DISP3DCNT &= 0x0000CFFE;
    } else {
        reg_G3X_DISP3DCNT = (u16)((reg_G3X_DISP3DCNT & ~0x3000) | 0x1);
        switch (tex) {
        case 5:
            reg_GX_VRAMCNT_A = 0x83;
            reg_GX_VRAMCNT_C = 0x8B;
            break;
        case 9:
            reg_GX_VRAMCNT_A = 0x83;
            reg_GX_VRAMCNT_D = 0x8B;
            break;
        case 10:
            reg_GX_VRAMCNT_B = 0x83;
            reg_GX_VRAMCNT_D = 0x8B;
            break;
        case 11:
            reg_GX_VRAMCNT_A = 0x83;
            reg_GX_VRAMCNT_B = 0x8B;
            reg_GX_VRAMCNT_D = 0x93;
            break;
        case 13:
            reg_GX_VRAMCNT_A = 0x83;
            reg_GX_VRAMCNT_C = 0x8B;
            reg_GX_VRAMCNT_D = 0x93;
            break;
        case 8:
            reg_GX_VRAMCNT_D = 0x83;
            break;
        case 12:
            reg_GX_VRAMCNT_D = 0x8B;
        case 4:
            reg_GX_VRAMCNT_C = 0x83;
            break;
        case 14:
            reg_GX_VRAMCNT_D = 0x93;
        case 6:
            reg_GX_VRAMCNT_C = 0x8B;
        case 2:
            reg_GX_VRAMCNT_B = 0x83;
            break;
        case 15:
            reg_GX_VRAMCNT_D = 0x9B;
        case 7:
            reg_GX_VRAMCNT_C = 0x93;
        case 3:
            reg_GX_VRAMCNT_B = 0x8B;
        case 1:
            reg_GX_VRAMCNT_A = 0x83;
            break;
        }
    }
    GX_VRAMCNT_SetLCDC_(gGXState.lcdc);
}
