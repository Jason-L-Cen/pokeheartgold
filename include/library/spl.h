#ifndef POKEHEARTGOLD_SPL_H
#define POKEHEARTGOLD_SPL_H

// Software particle library

#include "spl_emitter.h"
#include "spl_manager.h"

SPLManager *SPL_Init(SPLAlloc alloc, int max_emtr_num, int max_ptcl_num, u16 fix_polygon_id, u16 min_polygon_id, u16 max_polygon_id);
void SPL_Load(SPLManager *mgr, const void *data);
BOOL SPL_LoadTexByVRAMManager(SPLManager *mgr);
BOOL SPL_LoadTexPlttByVRAMManager(SPLManager *mgr);
BOOL SPL_LoadTexByCallbackFunction(SPLManager *mgr, u32 (*alloc)(u32 size, BOOL is4x4comp));
BOOL SPL_LoadTexPlttByCallbackFunction(SPLManager *mgr, u32 (*alloc)(u32 size, BOOL is4pltt));
SPLEmitter *SPL_Create(SPLManager *mgr, int res_no, const VecFx32 *pos);
void SPL_Delete(SPLManager *mgr, SPLEmitter *emtr);
void SPL_DeleteAll(SPLManager *mgr);
void SPL_Calc(SPLManager *mgr);
void SPL_Draw(SPLManager *mgr, const MtxFx43 *camera);
extern SPLEmitter *SPL_CreateWithInitialize(SPLManager *p_mgr, int res_no, void (*fp_callback)(struct SPLEmitter *));

#endif // POKEHEARTGOLD_SPL_H
