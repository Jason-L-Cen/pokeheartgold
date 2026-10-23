// SPDX-License-Identifier: GPL-3.0-or-later
// Private owner-task scenario; never included in the shipping engine.
#include <stdint.h>
#include <nitro/gx/native_geometry.h>
typedef struct FieldMapsQAState {
    uint32_t serial,stage,queued,error,mapId,x,z,loaded,running,task,child,ready;
    uint32_t targetMap,warpId,targetX,targetZ,direction,matrixIndex,model;
    uint32_t originMap,originX,originZ,originDirection,completed,loadType;
    uint32_t modelDraws,drawnModel,modelFile,drawVertices,drawPolygons,returnDraws;
    uint32_t ended,endConfirmed,freed,weather,rendered,nonuniform;
    uint32_t requestedWidth,widthSet,originWidthSerial,originWidthEpochs,targetWidthSerial,targetWidthEpochs;
    uint32_t targetWidthTracked,drawWidth,renderWidth,wideRejected,guards;
    HGNativeGeometryFrameInfo geometry;
} FieldMapsQAState;
enum { FIELD_MAPS_SMALL_BYTES=256*192*4,FIELD_MAPS_WIDE_BYTES=512*192*4,
       FIELD_MAPS_WIDE_OFFSET=8,FIELD_MAPS_SMALL_OFFSET=FIELD_MAPS_WIDE_BYTES+32 };

#ifndef HG_FIELD_MAPS_QA_STATE_ONLY
#include "field_system.h"
#include "field/draw_map_name.h"
#include "field/map_load_manager.h"
#include "field_warp_tasks.h"
#include "player_avatar.h"
#include <nitro/hw/native_graphics.h>
#include <string.h>

static FieldMapsQAState sFieldMapsQA;
static MapLoadManager *sTargetManager,*sOriginManager;
static BOOL sFieldMapsReturning;
void __real_MapLoadManager_End(MapLoadManager *manager);
BOOL __real_MapLoadManager_HasEnded(MapLoadManager *manager);
void __real_MapLoadManager_FreeNARCAndLoadedMapBuffers(MapLoadManager *manager);
void __wrap_MapLoadManager_End(MapLoadManager *manager) {
    __real_MapLoadManager_End(manager);
    if (manager==sTargetManager) sFieldMapsQA.ended++;
}
BOOL __wrap_MapLoadManager_HasEnded(MapLoadManager *manager) {
    BOOL ended=__real_MapLoadManager_HasEnded(manager);
    if (ended && manager==sTargetManager) sFieldMapsQA.endConfirmed++;
    return ended;
}
void __wrap_MapLoadManager_FreeNARCAndLoadedMapBuffers(MapLoadManager *manager) {
    BOOL target=manager==sTargetManager;
    __real_MapLoadManager_FreeNARCAndLoadedMapBuffers(manager);
    if (target) { sFieldMapsQA.freed++; sTargetManager=NULL; }
}
u32 FieldMapsQABuildingPolygons(void) {
    HGNativeGeometryFrameInfo frame;
    return HGNativeGeometryFrame(0,&frame) ? frame.polygons : UINT32_MAX;
}
void FieldMapsQAObserveMapModel(void *manager,int matrixIndex,const void *modelFile,u32 model,u32 before) {
    FieldMapsQAState *qa=&sFieldMapsQA;
    if (manager==sTargetManager && matrixIndex==(int)qa->matrixIndex && modelFile) {
        HGNativeGeometryFrameInfo frame;
        qa->modelDraws++; qa->drawnModel=model; qa->modelFile=(u32)(uaddr)modelFile; qa->drawWidth=HGNativeViewWidth();
        if (before!=UINT32_MAX && HGNativeGeometryFrame(0,&frame) && frame.polygons>=before) {
            qa->drawPolygons+=frame.polygons-before; qa->drawVertices=frame.vertices;
        }
    }
    if (manager==sOriginManager && matrixIndex>=0 && modelFile) qa->returnDraws++;
}
const FieldMapsQAState *HGNativeFieldMapsQAGetState(void) {
    sFieldMapsQA.requestedWidth=HGNativeViewWidth();
    HGNativeGeometryFrame(1,&sFieldMapsQA.geometry);
    return &sFieldMapsQA;
}
u32 HGNativeFieldMapsQARequest(u32 map,u32 warp,u32 x,u32 z,u32 direction,u32 matrixIndex,u32 model) {
    FieldMapsQAState *qa=&sFieldMapsQA;
    if (qa->queued || qa->stage || map>=540 || direction>=4 || x>32767 || z>32767 || matrixIndex>=MAP_MATRIX_MAX_SIZE || model>=65535) return FALSE;
    qa->targetMap=map; qa->warpId=warp; qa->targetX=x; qa->targetZ=z;
    qa->direction=direction; qa->matrixIndex=matrixIndex; qa->model=model; qa->queued=1; return TRUE;
}
u32 HGNativeFieldMapsQAReturn(void) {
    if (sFieldMapsQA.queued || sFieldMapsQA.stage!=3 || !sFieldMapsQA.rendered) return FALSE;
    sFieldMapsQA.queued=2; return TRUE;
}
static BOOL FieldMapsQATask(TaskManager *taskManager) {
    u32 *state=TaskManager_GetStatePtr(taskManager);
    FieldMapsQAState *qa=&sFieldMapsQA;
    if (!*state) {
        *state=1;
        if (!sFieldMapsReturning) CallTask_ScriptWarp(taskManager,qa->targetMap,(s32)qa->warpId,qa->targetX,qa->targetZ,qa->direction);
        else CallTask_ScriptWarp(taskManager,qa->originMap,-1,qa->originX,qa->originZ,qa->originDirection);
        return FALSE;
    }
    qa->completed++; qa->stage=sFieldMapsReturning ? 6 : 3; return TRUE;
}
static void FieldMapsQAFieldTick(FieldSystem *fieldSystem) {
    FieldMapsQAState *qa=&sFieldMapsQA; qa->serial++;
    qa->mapId=fieldSystem->location ? fieldSystem->location->mapId : UINT32_MAX;
    qa->loaded=FieldSystem_IsFieldMapLoaded(fieldSystem); qa->running=FieldSystem_IsFieldMapRunning(fieldSystem);
    qa->task=FieldSystem_TaskIsRunning(fieldSystem); qa->child=FieldSystem_IsChildAppRunning(fieldSystem);
    qa->ready=qa->loaded && qa->running && !qa->task && !qa->child && FieldSystem_IsPlayerMovementAllowed(fieldSystem);
    qa->loadType=fieldSystem->mapLoadType;
    if (qa->loaded && qa->mapId==qa->targetMap && qa->stage==3 && fieldSystem->mapLoadManager) sTargetManager=fieldSystem->mapLoadManager;
    if (qa->stage==6 && qa->mapId==qa->originMap) sOriginManager=fieldSystem->mapLoadManager;
    if (!qa->ready || !fieldSystem->playerAvatar || !fieldSystem->mapSystems) return;
    qa->x=PlayerAvatar_GetXCoord(fieldSystem->playerAvatar); qa->z=PlayerAvatar_GetZCoord(fieldSystem->playerAvatar);
    qa->weather=WeatherManager_GetWeather(fieldSystem->mapSystems->weatherManager);
    HGNativeGeometryFrame(1,&qa->geometry);
    qa->requestedWidth=HGNativeViewWidth();
    if (!qa->widthSet && qa->stage==0) {
        if (!HGNativeSetViewWidth(512)) { qa->error=__LINE__; return; }
        qa->requestedWidth=512; qa->widthSet=1; qa->originWidthSerial=qa->geometry.serial;
    }
    if (qa->widthSet && !qa->wideRejected && qa->requestedWidth!=512) { qa->error=__LINE__; return; }
    if (qa->stage==0) {
        if (qa->geometry.serial<qa->originWidthSerial) qa->originWidthEpochs=0;
        else if (qa->geometry.serial>qa->originWidthSerial && qa->geometry.vertices && qa->geometry.polygons) qa->originWidthEpochs++;
        qa->originWidthSerial=qa->geometry.serial;
        if (qa->originWidthEpochs<2) return;
    }
    if (qa->stage==3) {
        if (!qa->targetWidthTracked) { qa->targetWidthTracked=1; qa->targetWidthSerial=qa->geometry.serial; }
        else {
            if (qa->geometry.serial<qa->targetWidthSerial || qa->requestedWidth!=512) qa->targetWidthEpochs=0;
            else if (qa->geometry.serial>qa->targetWidthSerial && qa->geometry.vertices && qa->geometry.polygons) qa->targetWidthEpochs++;
            qa->targetWidthSerial=qa->geometry.serial;
        }
    }
    if (!qa->queued) return;
    if (qa->queued==1) {
        if (qa->mapId!=33) { qa->error=__LINE__; return; }
        qa->originMap=qa->mapId; qa->originX=qa->x; qa->originZ=qa->z;
        qa->originDirection=PlayerAvatar_GetFacingDirection(fieldSystem->playerAvatar); qa->stage=2;
    } else { sFieldMapsReturning=TRUE; qa->stage=5; }
    qa->queued=0;
    FieldDrawMapName_Reset(fieldSystem->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(fieldSystem,FieldMapsQATask,NULL);
}
u32 HGNativeFieldMapsQARender(void) {
    FieldMapsQAState *qa=&sFieldMapsQA;
    if (qa->stage!=3 || !qa->ready || qa->mapId!=qa->targetMap || !qa->modelDraws ||
        qa->drawnModel!=qa->model || !qa->drawPolygons ||
        (!qa->wideRejected && (qa->targetWidthEpochs<2 || HGNativeViewWidth()!=512))) return FALSE;
    HGNativeGeometryFrameInfo before,after;
    u8 *pixels=(u8 *)HGNativeFrameBuffer()+FIELD_MAPS_WIDE_OFFSET;
    u32 bytes=qa->wideRejected ? FIELD_MAPS_SMALL_BYTES : FIELD_MAPS_WIDE_BYTES;
    qa->renderWidth=qa->wideRejected ? 256 : 512;
    if (!HGNativeGeometryFrame(1,&before) || !before.vertices || !before.polygons) return FALSE;
    memset(pixels-8,0xA5,8); memset(pixels+bytes,0xA5,8);
    if (!qa->wideRejected && !HGNativeRender2DView(0,512,pixels,bytes)) {
        for (u32 i=0;i<8;i++) if (pixels[-8+(s32)i]!=0xA5 || pixels[bytes+i]!=0xA5) { qa->error=__LINE__; return FALSE; }
        qa->wideRejected++; qa->renderWidth=256; bytes=FIELD_MAPS_SMALL_BYTES;
        if (!HGNativeSetViewWidth(256)) return FALSE; // Mirror the App's explicit fallback, never call this a wide pass.
        qa->requestedWidth=256; memset(pixels+bytes,0xA5,8);
    }
    if (qa->wideRejected && !HGNativeRender2D(0,pixels,bytes)) return FALSE;
    for (u32 i=0;i<8;i++) if (pixels[-8+(s32)i]!=0xA5 || pixels[bytes+i]!=0xA5) { qa->error=__LINE__; return FALSE; }
    qa->guards++;
    if (!HGNativeGeometryFrame(1,&after) || memcmp(&before,&after,sizeof(before))) return FALSE;
    qa->nonuniform=FALSE;
    for (u32 i=1;i<qa->renderWidth*192;i++) if (memcmp(pixels,pixels+i*4,4)) { qa->nonuniform=TRUE; break; }
    if (!qa->nonuniform) return FALSE;
    qa->geometry=before; qa->rendered++; return TRUE;
}
#endif
