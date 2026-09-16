#include <nitro/code32.h>
#include <nitro/fx/fx.h>
#include <nitro/gx/g3b.h>
#include <nitro/mi/memory.h>
#include <nitro/types.h>

void G3BS_LoadMtx44(GXDLInfo *displaylist, const MtxFx44 *mtx);

void G3BS_LoadMtx44(GXDLInfo *displaylist, const MtxFx44 *mtx) {
    *(u32 *)displaylist->curr_cmd = 0x16;
    MI_Copy64B(mtx, displaylist->curr_param);
}

void G3B_PushMtx(GXDLInfo *displaylist) {
    *(u32 *)displaylist->curr_cmd = 0x11;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_PopMtx(GXDLInfo *displaylist, int mtx) {
    *(u32 *)displaylist->curr_cmd = 0x12;
    *displaylist->curr_param = (u32)mtx;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_LoadMtx44(GXDLInfo *displaylist, const MtxFx44 *mtx) {
    G3BS_LoadMtx44(displaylist, mtx);
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + sizeof(MtxFx44);
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

// color format is RGB555, stored in the lower bits
void G3B_Color(GXDLInfo *displaylist, GXRgb vtx_col) {
    *(u32 *)displaylist->curr_cmd = 0x20;
    *displaylist->curr_param = vtx_col;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

/*
Only feed normalized Vectors
only the fractional part and the sign, which is in the first nonfraction bit
since the vector is assumed to be normalized, are used
*/
void G3B_Normal(GXDLInfo *displaylist, fx16 x, fx16 y, fx16 z) {
    *(u32 *)displaylist->curr_cmd = 0x21;
    *displaylist->curr_param = (u32)(((x >> 3) & 0x3FF) | (((y >> 3) & 0x3FF) << 0xA) | (((z >> 3) & 0x3FF) << 0x14));
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_Vtx(GXDLInfo *displaylist, fx16 x, fx16 y, fx16 z) {
    *(u32 *)displaylist->curr_cmd = 0x23;
    displaylist->curr_param[0] = (u32)(u16)x | (u32)(u16)y << 0x10;
    displaylist->curr_param[1] = (u32)(u16)z;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x8;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

// TODO: name arguments
void G3B_PolygonAttr(GXDLInfo *displaylist, int r1, GXPolygonMode r2, GXCull r3, int r4, int r5, int r6) {
    *(u32 *)displaylist->curr_cmd = 0x29;
    *displaylist->curr_param = r1 | r2 << 0x4 | r3 << 0x6 | r6 | r4 << 0x18 | r5 << 0x10;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_MaterialColorDiffAmb(GXDLInfo *displaylist, GXRgb diffuse_col, GXRgb ambient_col, BOOL replace) {
    *(u32 *)displaylist->curr_cmd = 0x30;
    u32 replace_vtx_color_with_diffuse;
    if (replace) {
        replace_vtx_color_with_diffuse = TRUE;
    } else {
        replace_vtx_color_with_diffuse = FALSE;
    }
    *displaylist->curr_param = diffuse_col | ambient_col << 0x10 | replace_vtx_color_with_diffuse << 0xF;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_MaterialColorSpecEmi(GXDLInfo *displaylist, GXRgb specular_col, GXRgb emission_col, BOOL shiny_table) {
    *(u32 *)displaylist->curr_cmd = 0x31;
    u32 enable_shininess_table;
    if (shiny_table) {
        enable_shininess_table = TRUE;
    } else {
        enable_shininess_table = FALSE;
    }
    *displaylist->curr_param = specular_col | emission_col << 0x10 | enable_shininess_table << 0xF;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

/*
Only feed normalized Vectors
only the fractional part and the sign, which is in the first nonfraction bit
since the vector is assumed to be normalized, are used
*/
void G3B_LightVector(GXDLInfo *displaylist, GXLightId light_num, fx16 x, fx16 y, fx16 z) {
    *(u32 *)displaylist->curr_cmd = 0x32;
    *displaylist->curr_param = ((x >> 3) & 0x3FF) | (((y >> 3) & 0x3FF) << 0xA) | (((z >> 3) & 0x3FF) << 0x14) | light_num << 0x1E;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_LightColor(GXDLInfo *displaylist, GXLightId light_num, GXRgb col) {
    *(u32 *)displaylist->curr_cmd = 0x33;
    *displaylist->curr_param = col | light_num << 0x1E;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_Begin(GXDLInfo *displaylist, GXBegin type) {
    *(u32 *)displaylist->curr_cmd = 0x40;
    *displaylist->curr_param = type;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param + 0x4;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}

void G3B_End(GXDLInfo *displaylist) {
    *(u32 *)displaylist->curr_cmd = 0x41;
    displaylist->curr_cmd = (u8 *)displaylist->curr_param;
    displaylist->curr_param = (u32 *)displaylist->curr_cmd + 1;
}
