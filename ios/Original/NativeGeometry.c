// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/gx/native_geometry.h>
#include <nitro/gx/g3.h>
#include <nitro/fx/fx_mtx44.h>
#include <nitro/hw/native_graphics.h>
#include <string.h>

typedef struct Geometry {
    MtxFx44 projection,position,vector,texture;
    MtxFx44 projectionSlot,textureSlot,positionSlots[32],vectorSlots[32];
    u32 mode,projectionLevel,textureLevel,positionLevel,error;
    u32 commands,slots,opcode,needed,used,parameters[16];
} Geometry;
static Geometry geometry;
static BOOL initialized;
static void identity(MtxFx44 *matrix) { memset(matrix,0,sizeof(*matrix)); for (u32 i=0;i<4;i++) matrix->a[i*5]=4096; }
void HGNativeGeometryReset(void) {
    memset(&geometry,0,sizeof(geometry));
    identity(&geometry.projection); identity(&geometry.position); identity(&geometry.vector); identity(&geometry.texture);
    initialized=TRUE;
}
static void ensure(void) { if (!initialized) HGNativeGeometryReset(); }
static s32 parameterCount(u32 opcode) {
    static const s32 counts[]={1,0,1,1,1,0,16,12,16,12,9,3,3};
    if (!opcode) return 0;
    return opcode>=0x10 && opcode<=0x1C ? counts[opcode-0x10] : -1;
}
static BOOL copyWords(const void *source,u32 count,u32 *output) {
    uaddr address=(uaddr)source;
    if (count && (!source || address%4 || count>0x3FFFFFFFu || address>UINTPTR_MAX-count*4)) return FALSE;
    while (count) {
        u32 size=count*4; const void *bytes=HGNativeMemorySpan(address,&size);
        if (!bytes || !size || size%4) return FALSE;
        memcpy(output,bytes,size); output+=size/4; address+=size; count-=size/4;
    }
    return TRUE;
}
static void loadMatrix(MtxFx44 *matrix,u32 opcode,const u32 *parameters) {
    identity(matrix);
    if (opcode==0x16 || opcode==0x18) memcpy(matrix,parameters,64);
    else if (opcode==0x17 || opcode==0x19) {
        for (u32 row=0;row<4;row++) for (u32 col=0;col<3;col++) matrix->a[row*4+col]=(s32)parameters[row*3+col];
    } else if (opcode==0x1A) {
        for (u32 row=0;row<3;row++) for (u32 col=0;col<3;col++) matrix->a[row*4+col]=(s32)parameters[row*3+col];
    } else if (opcode==0x1B) { for (u32 i=0;i<3;i++) matrix->a[i*5]=(s32)parameters[i]; }
    else if (opcode==0x1C) { for (u32 i=0;i<3;i++) matrix->a[12+i]=(s32)parameters[i]; }
}
static BOOL execute(Geometry *state,u32 opcode,const u32 *parameters,u32 count) {
    if ((s32)count!=parameterCount(opcode)) return FALSE;
    if (!opcode) return TRUE;
    if (opcode==0x10) { state->mode=parameters[0]&3; return TRUE; }
    MtxFx44 *current=state->mode==0 ? &state->projection : state->mode==3 ? &state->texture : &state->position;
    MtxFx44 *slot=state->mode==0 ? &state->projectionSlot : &state->textureSlot;
    u32 *level=state->mode==0 ? &state->projectionLevel : &state->textureLevel;
    if (opcode>=0x11 && opcode<=0x14) {
        BOOL save=opcode==0x11 || opcode==0x13;
        if (state->mode==0 || state->mode==3) {
            if (opcode==0x11) { if (*level) state->error=1; *level=(*level+1)&1; }
            else if (opcode==0x12) { if (!*level) state->error=1; *level=(*level-1)&1; }
            if (save) *slot=*current; else *current=*slot;
        } else {
            u32 index;
            if (opcode==0x11) { if (state->positionLevel>30) state->error=1; index=state->positionLevel&31; state->positionLevel=(state->positionLevel+1)&63; }
            else if (opcode==0x12) {
                s32 offset=(s32)((parameters[0]&63)^32)-32;
                state->positionLevel=(state->positionLevel-(u32)offset)&63; index=state->positionLevel&31;
            } else index=parameters[0]&31;
            if (index>30 || (opcode==0x12 && state->positionLevel>30)) state->error=1;
            if (save) { state->positionSlots[index]=state->position; state->vectorSlots[index]=state->vector; }
            else { state->position=state->positionSlots[index]; state->vector=state->vectorSlots[index]; }
        }
        return TRUE;
    }
    if (opcode==0x15) {
        identity(current); if (state->mode==2) identity(&state->vector); return TRUE;
    }
    MtxFx44 incoming;
    loadMatrix(&incoming,opcode,parameters);
    if (opcode==0x16 || opcode==0x17) {
        *current=incoming; if (state->mode==2) state->vector=incoming;
    } else {
        MTX_Concat44(&incoming,current,current);
        if (state->mode==2 && opcode!=0x1B) MTX_Concat44(&incoming,&state->vector,&state->vector);
    }
    return TRUE;
}
BOOL HGNativeGeometryCommand(u32 opcode,const void *parameters,u32 count) {
    ensure(); u32 words[16];
    if (count>16 || (s32)count!=parameterCount(opcode) || !copyWords(parameters,count,words)) return FALSE;
    if (geometry.needed || geometry.slots) return FALSE;
    return execute(&geometry,opcode,words,count);
}
static BOOL advance(Geometry *state) {
    while (!state->needed && state->slots) {
        state->opcode=state->commands&255; state->commands>>=8; state->slots--;
        s32 count=parameterCount(state->opcode); if (count<0) return FALSE;
        state->needed=(u32)count; state->used=0;
        if (!count && !execute(state,state->opcode,NULL,0)) return FALSE;
    }
    return TRUE;
}
BOOL HGNativeGeometryFifo(const void *words,u32 count) {
    ensure(); Geometry pending=geometry;
    if (count>0x3FFFFFFF || (count && (uaddr)words>UINTPTR_MAX-count*4)) return FALSE;
    for (u32 i=0;i<count;i++) {
        u32 word; if (!copyWords((const void *)((uaddr)words+i*4),1,&word)) return FALSE;
        if (pending.needed) {
            pending.parameters[pending.used++]=word;
            if (pending.used==pending.needed) {
                if (!execute(&pending,pending.opcode,pending.parameters,pending.needed)) return FALSE;
                pending.needed=pending.used=0;
            }
        } else { pending.commands=word; pending.slots=4; }
        if (!advance(&pending)) return FALSE;
    }
    geometry=pending; return TRUE;
}
BOOL HGNativeGeometryMatrix(u32 which,void *output) {
    ensure();
    if (!output || (uaddr)output%4 || which>2 || geometry.needed || geometry.slots) return FALSE;
    u32 words[16],count=which==1 ? 9 : 16;
    if (which==0) { MtxFx44 clip; MTX_Concat44(&geometry.position,&geometry.projection,&clip); memcpy(words,&clip,64); }
    else if (which==2) memcpy(words,&geometry.texture,64);
    else for (u32 row=0;row<3;row++) for (u32 col=0;col<3;col++) words[row*3+col]=(u32)geometry.vector.a[row*4+col];
    u32 size=count*4; void *span=HGNativeMemorySpan((uaddr)output,&size);
    if (!span || size!=count*4) return FALSE;
    memcpy(span,words,size); return TRUE;
}
u32 HGNativeGeometryStatus(BOOL clearError) {
    ensure(); u32 status=(geometry.positionLevel&31)<<8 | geometry.projectionLevel<<13 | geometry.error<<15;
    if (geometry.needed || geometry.slots) status|=1u<<27;
    if (clearError) { geometry.error=0; geometry.projectionLevel=geometry.textureLevel=0; }
    return status;
}
