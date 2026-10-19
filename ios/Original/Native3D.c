// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/gx/native_geometry.h>
#include <nitro/gx/native_texture.h>
#include <nitro/hw/native_graphics.h>
#include <nitro/math/qsort.h>
#include <math.h>
#include <string.h>

enum { WIDTH=256, HEIGHT=192, CLIP_VERTICES=16 };
typedef struct { double position[4],color[3],uv[2]; } ClipVertex;
typedef struct { double x,y,z,inverseW,color[3],uv[2]; } ScreenVertex;
static u8 scratch[WIDTH*HEIGHT*4];
static double depths[WIDTH*HEIGHT];
static u8 translucentID[WIDTH*HEIGHT],opaqueBack[WIDTH*HEIGHT];
typedef struct { u32 index,key; } PolygonOrder;
static PolygonOrder order[2048];
static int compareOrder(const void *first,const void *second) {
    const PolygonOrder *a=first,*b=second;
    if (a->key!=b->key) return (a->key>b->key)-(a->key<b->key);
    return (a->index>b->index)-(a->index<b->index);
}
static unsigned channel(double value) {
    if (value<0) return 0; if (value>31) return 31;
    return (unsigned)__builtin_floor(value+0.5);
}
static u8 expand(unsigned value) { return (u8)((value*527+23)>>6); }
static double distance(const ClipVertex *vertex,unsigned plane) {
    if (plane==6) return vertex->position[3]-1e-9;
    return vertex->position[3]+(plane&1 ? -1 : 1)*vertex->position[plane/2];
}
static int clipPolygon(ClipVertex *vertices,unsigned count) {
    ClipVertex output[CLIP_VERTICES];
    for (unsigned plane=0;plane<7 && count;plane++) {
        unsigned used=0;
        for (unsigned i=0;i<count;i++) {
            const ClipVertex *a=&vertices[i],*b=&vertices[(i+1)%count];
            double da=distance(a,plane),db=distance(b,plane);
            if (da>=0) { if (used==CLIP_VERTICES) return -1; output[used++]=*a; }
            if ((da>=0)!=(db>=0)) {
                double t=da/(da-db); ClipVertex intersection;
                for (unsigned axis=0;axis<4;axis++) intersection.position[axis]=a->position[axis]+t*(b->position[axis]-a->position[axis]);
                for (unsigned axis=0;axis<3;axis++) intersection.color[axis]=a->color[axis]+t*(b->color[axis]-a->color[axis]);
                for (unsigned axis=0;axis<2;axis++) intersection.uv[axis]=a->uv[axis]+t*(b->uv[axis]-a->uv[axis]);
                if (used==CLIP_VERTICES) return -1;
                output[used++]=intersection;
            }
        }
        count=used; memcpy(vertices,output,count*sizeof(*vertices));
    }
    return count;
}
static ScreenVertex interpolate(const ScreenVertex *a,const ScreenVertex *b,double t) {
    ScreenVertex result;
    result.x=a->x+t*(b->x-a->x); result.y=a->y+t*(b->y-a->y);
    result.z=a->z+t*(b->z-a->z); result.inverseW=a->inverseW+t*(b->inverseW-a->inverseW);
    for (unsigned i=0;i<3;i++) result.color[i]=a->color[i]+t*(b->color[i]-a->color[i]);
    for (unsigned i=0;i<2;i++) result.uv[i]=a->uv[i]+t*(b->uv[i]-a->uv[i]);
    return result;
}
static int projectPolygon(ClipVertex *vertices,unsigned count,const HGNativeGeometryPolygon *polygon,ScreenVertex *screen,BOOL *front) {
    if (!(polygon->attributes&0x1000)) {
        for (unsigned i=0;i<count;i++) if (vertices[i].position[2]>vertices[i].position[3]) return 0;
    }
    int clipped=clipPolygon(vertices,count); if (clipped<0) return -1;
    count=(unsigned)clipped; if (count<3) return 0;
    unsigned x0=polygon->viewport&255,y0=(polygon->viewport>>8)&255;
    unsigned x1=(polygon->viewport>>16)&255,y1=polygon->viewport>>24;
    double area=0;
    for (unsigned i=0;i<count;i++) {
        double w=vertices[i].position[3],inverse=1.0/w;
        screen[i].x=__builtin_floor((vertices[i].position[0]*inverse+1)*(x1-x0+1)*0.5+x0);
        screen[i].y=__builtin_floor((-vertices[i].position[1]*inverse+1)*(y1-y0+1)*0.5+191-y1);
        screen[i].z=(__builtin_trunc(vertices[i].position[2]*inverse*16384)+16383)*512;
        if (screen[i].z<0) screen[i].z=0; if (screen[i].z>16777215) screen[i].z=16777215;
        screen[i].inverseW=inverse;
        for (unsigned axis=0;axis<3;axis++) screen[i].color[axis]=vertices[i].color[axis]*inverse;
        for (unsigned axis=0;axis<2;axis++) screen[i].uv[axis]=vertices[i].uv[axis]*inverse;
    }
    for (unsigned i=0;i<count;i++) area+=screen[i].x*screen[(i+1)%count].y-screen[i].y*screen[(i+1)%count].x;
    if (!area || (area<0 ? !(polygon->attributes&0x80) : !(polygon->attributes&0x40))) return 0;
    *front=area<0; return (int)count;
}
static BOOL drawPolygon(const ScreenVertex *screen,unsigned count,const HGNativeGeometryPolygon *polygon,BOOL front,u32 control,u32 alphaReference) {
    for (unsigned y=0;y<HEIGHT;y++) {
        double py=y+0.5; ScreenVertex left={0},right={0}; unsigned intersections=0;
        for (unsigned i=0;i<count;i++) {
            const ScreenVertex *a=&screen[i],*b=&screen[(i+1)%count];
            double low=a->y<b->y ? a->y : b->y,high=a->y>b->y ? a->y : b->y;
            if (a->y==b->y || py<low || py>=high) continue;
            ScreenVertex point=interpolate(a,b,(py-a->y)/(b->y-a->y));
            if (!intersections || point.x<left.x) left=point;
            if (!intersections || point.x>right.x) right=point;
            intersections++;
        }
        if (intersections<2 || right.x<=left.x) continue;
        int first=(int)__builtin_ceil(left.x-0.5),end=(int)__builtin_ceil(right.x-0.5);
        if (first<0) first=0; if (end>WIDTH) end=WIDTH;
        for (int x=first;x<end;x++) {
            ScreenVertex point=interpolate(&left,&right,(x+0.5-left.x)/(right.x-left.x));
            unsigned index=y*WIDTH+(unsigned)x;
            if ((front && opaqueBack[index] ? point.z>depths[index] : point.z>=depths[index]) || point.inverseW<=0) continue;
            unsigned opacity=(polygon->attributes>>16)&31;
            unsigned components[3];
            for (unsigned axis=0;axis<3;axis++) components[axis]=channel(point.color[axis]/point.inverseW);
            if ((control&1) && ((polygon->texture>>26)&7)) {
                u16 texel,alpha;
                s32 s=(s32)__builtin_floor(point.uv[0]/point.inverseW),t=(s32)__builtin_floor(point.uv[1]/point.inverseW);
                if (!HGNativeSampleTexture(polygon->texture,polygon->palette,s,t,&texel,&alpha)) return FALSE;
                unsigned mode=(polygon->attributes>>4)&3;
                for (unsigned axis=0;axis<3;axis++) {
                    unsigned component=(texel>>(axis*5))&31;
                    if (!mode) components[axis]=((component+1)*(components[axis]+1)-1)>>5;
                    else if (alpha==31) components[axis]=component;
                    else if (alpha) components[axis]=(component*alpha+components[axis]*(31-alpha))>>5;
                }
                if (!mode) opacity=((alpha+1)*(opacity+1)-1)>>5;
            }
            if (!opacity || ((control&4) && opacity<=alphaReference)) continue;
            unsigned previousAlpha=(scratch[index*4+3]*31u+127)/255;
            if (opacity<31) {
                u32 id=(polygon->attributes>>24)&63;
                if (translucentID[index]==id) continue;
                translucentID[index]=(u8)id;
                if (previousAlpha && (control&8)) {
                    for (unsigned axis=0;axis<3;axis++) components[axis]=(components[axis]*(opacity+1)+(scratch[index*4+axis]>>3)*(31-opacity))>>5;
                }
                if (polygon->attributes&0x800) depths[index]=point.z;
                opaqueBack[index]=0;
            } else {
                depths[index]=point.z; translucentID[index]=64; opaqueBack[index]=!front;
            }
            for (unsigned axis=0;axis<3;axis++) scratch[index*4+axis]=expand(components[axis]);
            scratch[index*4+3]=(u8)(((opacity>previousAlpha ? opacity : previousAlpha)*255+15)/31);
        }
    }
    return TRUE;
}
BOOL HGNativeRender3D(u32 which,u8 *rgba,u32 size) {
    HGNativeGeometryFrameInfo frame;
    if (!rgba || size<sizeof(scratch) || !HGNativeGeometryFrame(which,&frame) || (frame.swapMode&2)) return FALSE;
    u32 available=sizeof(scratch); void *destination=HGNativeMemorySpan((uaddr)rgba,&available);
    if (!destination || available!=sizeof(scratch)) return FALSE;
    unsigned clearAlpha=(frame.clearColor>>16)&31;
    if (frame.clearColor&0x8000 || frame.polygons>2048) return FALSE;
    u32 orderCount=0;
    for (u32 i=0;i<frame.polygons;i++) {
        HGNativeGeometryPolygon polygon;
        if (!HGNativeGeometryReadPolygon(which,i,&polygon) || polygon.count<3 || polygon.count>4) return FALSE;
        if (!((polygon.attributes>>16)&31) ||
            ((polygon.attributes>>4)&3)>1 || (polygon.attributes&0xE000)) return FALSE;
        unsigned x0=polygon.viewport&255,y0=(polygon.viewport>>8)&255;
        unsigned x1=(polygon.viewport>>16)&255,y1=polygon.viewport>>24;
        if (x0>x1 || y0>y1 || y1>=HEIGHT) return FALSE;
        for (u32 v=0;v<polygon.count;v++) {
            HGNativeGeometryVertex vertex;
            if (!HGNativeGeometryReadVertex(which,polygon.indices[v],&vertex)) return FALSE;
        }
        ClipVertex clip[CLIP_VERTICES]; ScreenVertex screen[CLIP_VERTICES]; BOOL front;
        for (u32 v=0;v<polygon.count;v++) {
            HGNativeGeometryVertex vertex; HGNativeGeometryReadVertex(which,polygon.indices[v],&vertex);
            for (unsigned a=0;a<4;a++) clip[v].position[a]=vertex.clip[a];
            for (unsigned a=0;a<3;a++) clip[v].color[a]=(vertex.color>>(a*5))&31;
            for (unsigned a=0;a<2;a++) clip[v].uv[a]=vertex.uv[a];
        }
        int count=projectPolygon(clip,polygon.count,&polygon,screen,&front); if (count<0) return FALSE;
        if (!count) continue;
        u32 top=192,bottom=0;
        for (int v=0;v<count;v++) { u32 y=(u32)screen[v].y; if (y<top) top=y; if (y>bottom) bottom=y; }
        u32 format=(polygon.texture>>26)&7;
        BOOL translucent=((polygon.attributes>>16)&31)<31 || format==1 || format==6;
        order[orderCount++]=(PolygonOrder){i,(translucent ? 0x80000000u : 0) |
            ((translucent && (frame.swapMode&1)) ? 0 : (bottom<<8)|top)};
    }
    MATH_QSort(order,orderCount,sizeof(order[0]),compareOrder,NULL);
    for (unsigned i=0;i<WIDTH*HEIGHT;i++) {
        for (unsigned axis=0;axis<3;axis++) scratch[i*4+axis]=expand((frame.clearColor>>(axis*5))&31);
        scratch[i*4+3]=(u8)((clearAlpha*255+15)/31); depths[i]=((frame.clearDepth&0x7FFF)<<9)|511;
        translucentID[i]=64; opaqueBack[i]=0;
    }
    for (u32 i=0;i<orderCount;i++) {
        HGNativeGeometryPolygon polygon; HGNativeGeometryReadPolygon(which,order[i].index,&polygon);
        ClipVertex vertices[CLIP_VERTICES];
        for (u32 v=0;v<polygon.count;v++) {
            HGNativeGeometryVertex vertex; HGNativeGeometryReadVertex(which,polygon.indices[v],&vertex);
            for (unsigned axis=0;axis<4;axis++) vertices[v].position[axis]=vertex.clip[axis];
            for (unsigned axis=0;axis<3;axis++) vertices[v].color[axis]=(vertex.color>>(axis*5))&31;
            for (unsigned axis=0;axis<2;axis++) vertices[v].uv[axis]=vertex.uv[axis];
        }
        ScreenVertex screen[CLIP_VERTICES]; BOOL front;
        int count=projectPolygon(vertices,polygon.count,&polygon,screen,&front);
        if (count<0 || (count && !drawPolygon(screen,(unsigned)count,&polygon,front,frame.displayControl,frame.alphaReference))) return FALSE;
    }
    memcpy(destination,scratch,sizeof(scratch)); return TRUE;
}
