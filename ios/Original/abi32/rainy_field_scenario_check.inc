// SPDX-License-Identifier: GPL-3.0-or-later
// Private scenario state and owner hook; absent from the shipping module.
#include <stdint.h>
#include <nitro/gx/native_geometry.h>
typedef struct RainyFieldQAState {
    uint32_t serial,stage,queued,error,mapId,x,z,loaded,running,task,child,ready;
    uint32_t originMap,originX,originZ,originDirection,completed;
    uint32_t weather,localWeather,rainModelDraws,returnModelDraws,modelFile;
    uint32_t rainEnded,rainEndConfirmed,rainFreed;
    uint32_t compared,renderErrors,differentPixels,firstDifferent,normalPixel,zeroPixel;
    uint32_t fogPolygons;
    HGNativeGeometryFrameInfo geometry;
    HGNativeGeometryFogInfo sourceFog,publishedFog,comparisonFog;
} RainyFieldQAState;

#ifndef HG_RAINY_FIELD_QA_STATE_ONLY
#include "field_system.h"
#include "field/draw_map_name.h"
#include "field/map_load_manager.h"
#include "field_warp_tasks.h"
#include "save_local_field_data.h"
#include "player_avatar.h"
#include "constants/maps.h"
#include "constants/global_fieldmap.h"
#include <nitro/hw/native_graphics.h>
#include <string.h>

static RainyFieldQAState sRainyFieldQA;
static MapLoadManager *sRainManager,*sReturnManager;
static BOOL sReturning,sZeroDensity;
BOOL __real_HGNativeGeometryFog(u32 which,HGNativeGeometryFogInfo *output);
BOOL __wrap_HGNativeGeometryFog(u32 which,HGNativeGeometryFogInfo *output) {
    BOOL result=__real_HGNativeGeometryFog(which,output);
    if (result && which==1 && sZeroDensity) {
        memset(output->density,0,sizeof(output->density));
        sRainyFieldQA.comparisonFog=*output;
    }
    return result;
}
void __real_MapLoadManager_End(MapLoadManager *manager);
BOOL __real_MapLoadManager_HasEnded(MapLoadManager *manager);
void __real_MapLoadManager_FreeNARCAndLoadedMapBuffers(MapLoadManager *manager);
void __wrap_MapLoadManager_End(MapLoadManager *manager) {
    __real_MapLoadManager_End(manager);
    if (manager==sRainManager) sRainyFieldQA.rainEnded++;
}
BOOL __wrap_MapLoadManager_HasEnded(MapLoadManager *manager) {
    BOOL result=__real_MapLoadManager_HasEnded(manager);
    if (result && manager==sRainManager) sRainyFieldQA.rainEndConfirmed++;
    return result;
}
void __wrap_MapLoadManager_FreeNARCAndLoadedMapBuffers(MapLoadManager *manager) {
    BOOL rain=manager==sRainManager;
    __real_MapLoadManager_FreeNARCAndLoadedMapBuffers(manager);
    if (rain) { sRainyFieldQA.rainFreed++; sRainManager=NULL; }
}
void RainyFieldQAObserveMapModel(void *manager,int matrixIndex,const void *modelFile) {
    if (manager==sRainManager && matrixIndex==672 && modelFile) {
        sRainyFieldQA.rainModelDraws++; sRainyFieldQA.modelFile=(u32)(uaddr)modelFile;
    }
    if (manager==sReturnManager && matrixIndex>=0 && modelFile) sRainyFieldQA.returnModelDraws++;
}
const RainyFieldQAState *HGNativeRainyFieldQAGetState(void) { return &sRainyFieldQA; }
u32 HGNativeRainyFieldQARequest(u32 action) {
    if (sRainyFieldQA.queued || (action!=1 && action!=2)) return FALSE;
    if ((action==1 && sRainyFieldQA.stage) || (action==2 && sRainyFieldQA.stage!=3)) return FALSE;
    sRainyFieldQA.queued=action; return TRUE;
}
static BOOL RainyFieldQATask(TaskManager *taskManager) {
    u32 *state=TaskManager_GetStatePtr(taskManager);
    if (!*state) {
        *state=1;
        if (!sReturning) CallTask_ScriptWarp(taskManager,MAP_ROUTE_33,0,0,0,DIR_SOUTH);
        else CallTask_ScriptWarp(taskManager,sRainyFieldQA.originMap,-1,sRainyFieldQA.originX,
                                sRainyFieldQA.originZ,sRainyFieldQA.originDirection);
        return FALSE;
    }
    sRainyFieldQA.completed++; sRainyFieldQA.stage=sReturning ? 6 : 3;
    return TRUE;
}
static void RainyFieldQAFieldTick(FieldSystem *fieldSystem) {
    RainyFieldQAState *qa=&sRainyFieldQA; qa->serial++;
    qa->mapId=fieldSystem->location ? fieldSystem->location->mapId : 0;
    qa->loaded=FieldSystem_IsFieldMapLoaded(fieldSystem);
    qa->running=FieldSystem_IsFieldMapRunning(fieldSystem);
    qa->task=FieldSystem_TaskIsRunning(fieldSystem); qa->child=FieldSystem_IsChildAppRunning(fieldSystem);
    qa->ready=qa->loaded && qa->running && !qa->task && !qa->child && FieldSystem_IsPlayerMovementAllowed(fieldSystem);
    if (!qa->ready || !fieldSystem->playerAvatar || !fieldSystem->mapSystems || !fieldSystem->fog ||
        !fieldSystem->mapSystems->weatherManager) return;
    qa->x=PlayerAvatar_GetXCoord(fieldSystem->playerAvatar); qa->z=PlayerAvatar_GetZCoord(fieldSystem->playerAvatar);
    qa->weather=WeatherManager_GetWeather(fieldSystem->mapSystems->weatherManager);
    qa->localWeather=LocalFieldData_GetWeatherType(Save_LocalFieldData_Get(fieldSystem->saveData));
    FogData *fog=fieldSystem->fog;
    qa->sourceFog=(HGNativeGeometryFogInfo){.color=fog->color|(fog->alpha<<16),.offset=(u16)fog->fogOffset,
        .control=(u16)((fog->enable<<7)|(fog->fogMode<<6)|(fog->fogSlope<<8))};
    memcpy(qa->sourceFog.density,fog->fogTable,32);
    HGNativeGeometryFog(1,&qa->publishedFog); HGNativeGeometryFrame(1,&qa->geometry);
    if (qa->mapId==MAP_ROUTE_33 && !sRainManager && qa->stage!=6) sRainManager=fieldSystem->mapLoadManager;
    if (qa->stage==6 && qa->mapId==qa->originMap) sReturnManager=fieldSystem->mapLoadManager;
    if (!qa->queued) return;
    if (qa->queued==1) {
        if (qa->mapId!=MAP_ROUTE_29) { qa->error=__LINE__; return; }
        qa->originMap=qa->mapId; qa->originX=qa->x; qa->originZ=qa->z;
        qa->originDirection=PlayerAvatar_GetFacingDirection(fieldSystem->playerAvatar);
        qa->stage=2;
    } else { sReturning=TRUE; qa->stage=5; }
    qa->queued=0;
    FieldDrawMapName_Reset(fieldSystem->mapSystems->drawMapNameInfo);
    FieldSystem_CreateTask(fieldSystem,RainyFieldQATask,NULL);
}
u32 HGNativeRainyFieldQACompare(void) {
    RainyFieldQAState *qa=&sRainyFieldQA;
    if (qa->stage!=3 || !qa->ready || qa->mapId!=MAP_ROUTE_33 || !qa->rainModelDraws) return FALSE;
    HGNativeGeometryFrameInfo before,after;
    HGNativeGeometryFogInfo fogBefore,fogAfter;
    if (!HGNativeGeometryFrame(1,&before) || !HGNativeGeometryFog(1,&fogBefore) || !(fogBefore.control&0x80)) return FALSE;
    qa->fogPolygons=0;
    for (u32 i=0;i<before.polygons;i++) {
        HGNativeGeometryPolygon polygon;
        if (HGNativeGeometryReadPolygon(1,i,&polygon) && (polygon.attributes&0x8000)) qa->fogPolygons++;
    }
    const u32 bytes=256*192*4;
    u8 *pixels=(u8 *)HGNativeFrameBuffer(),*zero=pixels+bytes,*repeat=zero+bytes;
    if (!qa->fogPolygons || !HGNativeRender2D(0,pixels,bytes)) { qa->renderErrors++; return FALSE; }
    sZeroDensity=TRUE;
    BOOL rendered=HGNativeRender2D(0,zero,bytes);
    sZeroDensity=FALSE;
    if (!rendered || !HGNativeRender2D(0,repeat,bytes)) { qa->renderErrors++; return FALSE; }
    if (!HGNativeGeometryFrame(1,&after) || !HGNativeGeometryFog(1,&fogAfter) ||
        memcmp(&before,&after,32) || memcmp(&fogBefore,&fogAfter,40) || memcmp(pixels,repeat,bytes)) {
        qa->error=__LINE__; return FALSE;
    }
    qa->differentPixels=0;
    for (u32 i=0;i<256*192;i++) if (memcmp(pixels+i*4,zero+i*4,4)) {
        if (!qa->differentPixels) {
            qa->firstDifferent=i; memcpy(&qa->normalPixel,pixels+i*4,4); memcpy(&qa->zeroPixel,zero+i*4,4);
        }
        qa->differentPixels++;
    }
    qa->geometry=before; qa->publishedFog=fogBefore; qa->compared++;
    return qa->differentPixels!=0;
}
#endif
