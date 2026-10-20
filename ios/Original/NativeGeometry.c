// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/gx/native_geometry.h>
#include <nitro/gx/g3.h>
#include <nitro/fx/fx_mtx44.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/hw/io_reg.h>
#include <string.h>

typedef struct Geometry {
    MtxFx44 projection,position,vector,texture;
    MtxFx44 projectionSlot,textureSlot,positionSlots[32],vectorSlots[32];
    u32 mode,projectionLevel,textureLevel,positionLevel,error;
    u32 commands,slots,opcode,needed,used,parameters[32];
    MtxFx44 clip;
    BOOL clipValid,drawing,published;
    s16 vertex[3],uv[2],rawUV[2],normal[3];
    s32 lightDirection[4][3],specularReciprocal[4];
    u16 lightColor[4],diffuse,ambient,specular,emission;
    u8 shininess[128]; BOOL useShininess;
    u16 color,indices[4];
    u32 primitive,assembled,stripNumber,attributes,latchedAttributes,textureParam,palette,viewport;
    u32 building,display,serial;
    u32 clearColor,clearDepth;
    HGNativeGeometryFrameInfo frames[2];
} Geometry;
static Geometry geometry;
enum { VERTEX_CAPACITY=6144, POLYGON_CAPACITY=2048 };
static HGNativeGeometryVertex vertices[2][VERTEX_CAPACITY];
static HGNativeGeometryPolygon polygons[2][POLYGON_CAPACITY];
static BOOL initialized;
static void identity(MtxFx44 *matrix) { memset(matrix,0,sizeof(*matrix)); for (u32 i=0;i<4;i++) matrix->a[i*5]=4096; }
void HGNativeGeometryReset(void) {
    memset(&geometry,0,sizeof(geometry));
    identity(&geometry.projection); identity(&geometry.position); identity(&geometry.vector); identity(&geometry.texture);
    geometry.viewport=0xBFFF0000; geometry.color=0x7FFF;
    geometry.clearDepth=0x7FFF;
    initialized=TRUE;
}
static void ensure(void) { if (!initialized) HGNativeGeometryReset(); }
static s32 parameterCount(u32 opcode) {
    static const s32 counts[]={1,0,1,1,1,0,16,12,16,12,9,3,3};
    if (!opcode) return 0;
    if (opcode>=0x10 && opcode<=0x1C) return counts[opcode-0x10];
    if ((opcode>=0x20 && opcode<=0x22) || (opcode>=0x24 && opcode<=0x2B) ||
        (opcode>=0x30 && opcode<=0x33) ||
        opcode==0x40 || opcode==0x50 || opcode==0x60) return 1;
    if (opcode==0x23) return 2;
    if (opcode==0x34) return 32;
    return opcode==0x41 ? 0 : -1;
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
static BOOL matrixCommand(Geometry *state,u32 opcode,const u32 *parameters,u32 count) {
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
static s16 signed10(u32 value) { return (s16)((s32)((value&1023)^512)-512); }
static s32 signedBits(u32 value,u32 bits) {
    u32 sign=1u<<(bits-1),mask=(sign<<1)-1;
    return (s32)(((value&mask)^sign)-sign);
}
static s32 vectorDot(const s16 vector[3],const MtxFx44 *matrix,u32 column) {
    u32 sum=0;
    for (u32 row=0;row<3;row++) sum+=(u32)((s64)vector[row]*matrix->a[row*4+column]);
    return (s32)sum;
}
static void lightVector(Geometry *state,u32 word) {
    u32 light=word>>30; s16 direction[3];
    for (u32 i=0;i<3;i++) direction[i]=signed10(word>>(i*10));
    for (u32 i=0;i<3;i++) state->lightDirection[light][i]=signedBits((u32)-(vectorDot(direction,&state->vector,i)>>12),11);
    s32 denominator=512-(signedBits((u32)vectorDot(direction,&state->vector,2),23)>>12);
    state->specularReciprocal[light]=denominator ? 262144/denominator : 0;
}
static void normalCommand(Geometry *state,u32 word) {
    s32 transformed[3];
    for (u32 i=0;i<3;i++) state->normal[i]=signed10(word>>(i*10));
    if ((state->textureParam>>30)==2) {
        for (u32 column=0;column<2;column++) {
            s64 sum=0; for (u32 row=0;row<3;row++) sum+=(s64)state->normal[row]*state->texture.a[row*4+column];
            state->uv[column]=(s16)((s32)state->rawUV[column]+(s32)(sum>>21));
        }
    }
    for (u32 i=0;i<3;i++) transformed[i]=signedBits((u32)vectorDot(state->normal,&state->vector,i),23)>>12;
    u32 channels[3]; for (u32 c=0;c<3;c++) channels[c]=((state->emission>>(c*5))&31)<<14;
    for (u32 light=0;light<4;light++) if (state->latchedAttributes&(1u<<light)) {
        s32 dot=0,shine=0;
        for (u32 i=0;i<3;i++) dot+=(state->lightDirection[light][i]*transformed[i])>>9;
        if (dot>0) {
            s32 diffuse=signedBits((u32)dot,11);
            for (u32 c=0;c<3;c++) channels[c]+=(u32)((s32)((state->diffuse>>(c*5))&31)*
                ((state->lightColor[light]>>(c*5))&31)*diffuse)&0xFFFFF;
            s32 half=signedBits((u32)(dot+transformed[2]),11);
            s32 squared=((half*half)>>10)&1023;
            shine=(s32)(((s64)squared*state->specularReciprocal[light])>>8)-512;
            if (shine<0) shine=0;
            else { shine=signedBits((u32)shine,14); if (shine<0) shine=0; else if (shine>511) shine=511; }
        }
        if (state->useShininess) shine=state->shininess[(u32)shine>>2]*2;
        for (u32 c=0;c<3;c++) channels[c]+=(((state->specular>>(c*5))&31)*(u32)shine+
            (((state->ambient>>(c*5))&31)<<9))*((state->lightColor[light]>>(c*5))&31);
    }
    state->color=0;
    for (u32 c=0;c<3;c++) { u32 value=channels[c]>>14; if (value>31) value=31; state->color|=(u16)(value<<(c*5)); }
}
static BOOL vertexCommand(Geometry *state,u32 opcode,const u32 *parameters,BOOL write) {
    if (!state->drawing) return FALSE;
    u32 bank=state->building,index=state->frames[bank].vertices;
    if (index>=VERTEX_CAPACITY) return FALSE;
    if (opcode==0x23) {
        state->vertex[0]=(s16)parameters[0]; state->vertex[1]=(s16)(parameters[0]>>16); state->vertex[2]=(s16)parameters[1];
    } else if (opcode==0x24) {
        for (u32 i=0;i<3;i++) state->vertex[i]=(s16)((s32)signed10(parameters[0]>>(i*10))*64);
    } else if (opcode==0x28) {
        for (u32 i=0;i<3;i++) state->vertex[i]=(s16)((s32)state->vertex[i]+signed10(parameters[0]>>(i*10)));
    } else {
        u32 first=opcode==0x27 ? 1 : 0,second=opcode==0x25 ? 1 : 2;
        state->vertex[first]=(s16)parameters[0]; state->vertex[second]=(s16)(parameters[0]>>16);
    }
    if (!state->clipValid) { MTX_Concat44(&state->position,&state->projection,&state->clip); state->clipValid=TRUE; }
    HGNativeGeometryVertex vertex={0};
    for (u32 col=0;col<4;col++) {
        s64 sum=(s64)4096*state->clip.a[12+col];
        for (u32 row=0;row<3;row++) sum+=(s64)state->vertex[row]*state->clip.a[row*4+col];
        vertex.clip[col]=(s32)(sum>>12);
    }
    vertex.uv[0]=state->uv[0]; vertex.uv[1]=state->uv[1]; vertex.color=state->color;
    if ((state->textureParam>>30)==3) {
        for (u32 column=0;column<2;column++) {
            s64 sum=0; for (u32 row=0;row<3;row++) sum+=(s64)state->vertex[row]*state->texture.a[row*4+column];
            vertex.uv[column]=(s16)((s32)state->rawUV[column]+(s32)(sum>>24));
        }
    }
    state->indices[state->assembled++]=(u16)index; state->frames[bank].vertices++;
    u32 required=(state->primitive==1 || state->primitive==3) ? 4 : 3;
    if (state->assembled==required) {
        u32 polygon=state->frames[bank].polygons;
        if (polygon>=POLYGON_CAPACITY) return FALSE;
        HGNativeGeometryPolygon result={.count=required,.attributes=state->latchedAttributes,
            .texture=state->textureParam,.palette=state->palette,.viewport=state->viewport};
        memcpy(result.indices,state->indices,required*2);
        if (state->primitive==2 && (state->stripNumber&1)) {
            result.indices[0]=state->indices[1]; result.indices[1]=state->indices[0];
        } else if (state->primitive==3) {
            result.indices[2]=state->indices[3]; result.indices[3]=state->indices[2];
        }
        if (write) polygons[bank][polygon]=result;
        state->frames[bank].polygons++;
        if (state->primitive==2) {
            state->indices[0]=state->indices[1]; state->indices[1]=state->indices[2]; state->assembled=2;
        } else if (state->primitive==3) {
            state->indices[0]=state->indices[2]; state->indices[1]=state->indices[3]; state->assembled=2;
        } else state->assembled=0;
        state->stripNumber++;
    }
    if (write) vertices[bank][index]=vertex;
    return TRUE;
}
static BOOL execute(Geometry *state,u32 opcode,const u32 *parameters,u32 count,BOOL write) {
    if ((s32)count!=parameterCount(opcode)) return FALSE;
    if (opcode<=0x1C) { state->clipValid=FALSE; return matrixCommand(state,opcode,parameters,count); }
    if (opcode>=0x23 && opcode<=0x28) return vertexCommand(state,opcode,parameters,write);
    switch (opcode) {
    case 0x20: state->color=(u16)(parameters[0]&0x7FFF); break;
    case 0x21: normalCommand(state,parameters[0]); break;
    case 0x22:
        state->rawUV[0]=(s16)parameters[0]; state->rawUV[1]=(s16)(parameters[0]>>16);
        for (u32 i=0;i<2;i++) {
            s64 value=state->rawUV[i];
            if ((state->textureParam>>30)==1) value=((s64)state->rawUV[0]*state->texture.a[i]+
                (s64)state->rawUV[1]*state->texture.a[4+i]+state->texture.a[8+i]+state->texture.a[12+i])>>12;
            state->uv[i]=(s16)value;
        }
        break;
    case 0x29: state->attributes=parameters[0]; break;
    case 0x2A: state->textureParam=parameters[0]; break;
    case 0x2B: state->palette=parameters[0]&0x1FFF; break;
    case 0x30:
        state->diffuse=(u16)(parameters[0]&0x7FFF); state->ambient=(u16)((parameters[0]>>16)&0x7FFF);
        if (parameters[0]&0x8000) state->color=state->diffuse; break;
    case 0x31:
        state->specular=(u16)(parameters[0]&0x7FFF); state->emission=(u16)((parameters[0]>>16)&0x7FFF);
        state->useShininess=(parameters[0]&0x8000)!=0; break;
    case 0x32: lightVector(state,parameters[0]); break;
    case 0x33: state->lightColor[parameters[0]>>30]=(u16)(parameters[0]&0x7FFF); break;
    case 0x34:
        for (u32 i=0;i<128;i++) state->shininess[i]=(u8)(parameters[i/4]>>((i&3)*8)); break;
    case 0x40:
        state->drawing=TRUE; state->primitive=parameters[0]&3; state->assembled=state->stripNumber=0;
        state->latchedAttributes=state->attributes; break;
    case 0x41: break;
    case 0x50:
        state->display=state->building; state->published=TRUE;
        state->frames[state->display].serial=++state->serial;
        state->frames[state->display].swapMode=parameters[0]&3;
        state->frames[state->display].clearColor=state->clearColor;
        state->frames[state->display].clearDepth=state->clearDepth;
        state->frames[state->display].displayControl=reg_G3X_DISP3DCNT;
        state->frames[state->display].alphaReference=reg_G3X_ALPHA_TEST_REF&31;
        state->building^=1; memset(&state->frames[state->building],0,sizeof(state->frames[0]));
        state->drawing=FALSE; state->assembled=0; break;
    case 0x60: state->viewport=parameters[0]; break;
    default: return FALSE;
    }
    return TRUE;
}
BOOL HGNativeGeometryCommand(u32 opcode,const void *parameters,u32 count) {
    ensure(); u32 words[32];
    if (count>32 || (s32)count!=parameterCount(opcode) || !copyWords(parameters,count,words)) return FALSE;
    if (geometry.needed || geometry.slots) return FALSE;
    Geometry pending=geometry;
    if (!execute(&pending,opcode,words,count,FALSE)) return FALSE;
    if (!execute(&geometry,opcode,words,count,TRUE)) return FALSE;
    return TRUE;
}
static BOOL advance(Geometry *state,BOOL write) {
    while (!state->needed && state->slots) {
        state->opcode=state->commands&255; state->commands>>=8; state->slots--;
        s32 count=parameterCount(state->opcode); if (count<0) return FALSE;
        state->needed=(u32)count; state->used=0;
        if (!count && !execute(state,state->opcode,NULL,0,write)) return FALSE;
    }
    return TRUE;
}
static BOOL feed(Geometry *pending,const void *words,u32 count,BOOL write) {
    for (u32 i=0;i<count;i++) {
        u32 word; if (!copyWords((const void *)((uaddr)words+i*4),1,&word)) return FALSE;
        if (pending->needed) {
            pending->parameters[pending->used++]=word;
            if (pending->used==pending->needed) {
                if (!execute(pending,pending->opcode,pending->parameters,pending->needed,write)) return FALSE;
                pending->needed=pending->used=0;
            }
        } else { pending->commands=word; pending->slots=4; }
        if (!advance(pending,write)) return FALSE;
    }
    return TRUE;
}
BOOL HGNativeGeometryFifo(const void *words,u32 count) {
    ensure(); Geometry pending=geometry;
    if (count>0x3FFFFFFF || (count && (uaddr)words>UINTPTR_MAX-count*4)) return FALSE;
    // Inputs stay immutable during this synchronous call; validate swaps/capacity before writing either bank.
    if (!feed(&pending,words,count,FALSE)) return FALSE;
    return feed(&geometry,words,count,TRUE);
}
BOOL HGNativeGeometryPacket(u32 command,const void *words,u32 count) {
    ensure(); Geometry pending=geometry;
    if (count>0x3FFFFFFF || (count && (uaddr)words>UINTPTR_MAX-count*4)) return FALSE;
    if (!feed(&pending,&command,1,FALSE) || !feed(&pending,words,count,FALSE)) return FALSE;
    return feed(&geometry,&command,1,TRUE) && feed(&geometry,words,count,TRUE);
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
static BOOL bankFor(u32 which,u32 *bank) {
    if (which>1 || (which && !geometry.published) || (!which && (geometry.needed || geometry.slots))) return FALSE;
    *bank=which ? geometry.display : geometry.building; return TRUE;
}
static BOOL outputValue(void *output,const void *value,u32 size) {
    if (!output || (uaddr)output%4) return FALSE;
    u32 available=size; void *destination=HGNativeMemorySpan((uaddr)output,&available);
    if (!destination || available!=size) return FALSE;
    memcpy(destination,value,size); return TRUE;
}
BOOL HGNativeGeometryFrame(u32 which,HGNativeGeometryFrameInfo *output) {
    ensure(); u32 bank; if (!bankFor(which,&bank)) return FALSE;
    HGNativeGeometryFrameInfo frame=geometry.frames[bank];
    if (!which) {
        frame.clearColor=geometry.clearColor; frame.clearDepth=geometry.clearDepth;
        frame.displayControl=reg_G3X_DISP3DCNT; frame.alphaReference=reg_G3X_ALPHA_TEST_REF&31;
    }
    return outputValue(output,&frame,sizeof(*output));
}
BOOL HGNativeGeometryReadVertex(u32 which,u32 index,HGNativeGeometryVertex *output) {
    ensure(); u32 bank;
    return bankFor(which,&bank) && index<geometry.frames[bank].vertices && outputValue(output,&vertices[bank][index],sizeof(*output));
}
BOOL HGNativeGeometryReadPolygon(u32 which,u32 index,HGNativeGeometryPolygon *output) {
    ensure(); u32 bank;
    return bankFor(which,&bank) && index<geometry.frames[bank].polygons && outputValue(output,&polygons[bank][index],sizeof(*output));
}
void HGNativeGeometryClear(u32 color,u32 depth) {
    ensure(); geometry.clearColor=color&0x3F1FFFFF; geometry.clearDepth=depth&0x7FFF;
    reg_G3X_CLEAR_COLOR=geometry.clearColor; reg_G3X_CLEAR_DEPTH=(u16)geometry.clearDepth;
}
