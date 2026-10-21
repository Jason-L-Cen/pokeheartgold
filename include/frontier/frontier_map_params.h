#ifndef POKEHEARTGOLD_FRONTIER_FRONTIER_MAP_PARAMS_H
#define POKEHEARTGOLD_FRONTIER_FRONTIER_MAP_PARAMS_H

#include "global.h"

#include "frontier/frontier_map.h"

// Fields of the per-map parameter table (FrontierMap_GetParam)
enum FrontierMapParam {
    MAP_PARAM_BG_MODE,
    MAP_PARAM_SCRIPT_ID,
    MAP_PARAM_MSG_BANK,
    MAP_PARAM_BGM,
    MAP_PARAM_BG_SIZE,
    MAP_PARAM_NARC,
    MAP_PARAM_BG3_SCREEN,
    MAP_PARAM_BG3_CHAR,
    MAP_PARAM_BG3_PALETTE,
    MAP_PARAM_BG2_SCREEN,
    MAP_PARAM_BG2_CHAR,
    MAP_PARAM_BG2_PALETTE,
    MAP_PARAM_SCROLL_MODE,
    MAP_PARAM_BG2_SCROLLS,
};

u32 FrontierMap_GetParam(u32 mapId, enum FrontierMapParam param);
void FrontierMap_InitEffect(FrontierMap *map, void **effect, u32 mapId);
void FrontierMap_ExitEffect(FrontierMap *map, void **effect, u32 mapId);

#endif // POKEHEARTGOLD_FRONTIER_FRONTIER_MAP_PARAMS_H
