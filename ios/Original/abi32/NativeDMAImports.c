// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeDMA.h"
#include <nitro/mi/dma.h>
#include <stdlib.h>
#include <string.h>

typedef struct Mapping {
    struct w2c_env *env;
    HGNativeDMASpan spans[HG_NATIVE_DMA_MAX_SPANS];
} Mapping;
static void *memorySpan(struct w2c_env *env, u32 offset, u32 size) {
    if (!offset || offset > env->memory->size || size > env->memory->size-offset)
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    return env->memory->data+offset;
}
static void dispose(void *context) {
    Mapping *mapping=context;
    __atomic_fetch_sub(&mapping->env->pendingDMA,1,__ATOMIC_RELEASE);
    free(mapping);
}
void w2c_env_HGNativeDmaRequest(struct w2c_env *env,u32 command,u32 channel,u32 source,u32 destination,
                              u32 control,u32 table,u32 count) {
    if (!env || !env->memory || !env->memory->data || channel>=4) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command>=HG_DMA_WAIT) {
        if (source || destination || table || count) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (command==HG_DMA_WAIT && !control) { MI_WaitDma(channel); return; }
        if (command==HG_DMA_STOP && !control) { MI_StopDma(channel); return; }
        if (command==HG_DMA_CHECK_AUTO && (control==MIi_DMA_TIMING_ANY || control==0)) {
            MIi_CheckAnotherAutoDMA(channel,control); return;
        }
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    }
    // Native workers retain physical pointers; fixed memory prevents a concurrent relocation by memory.grow.
    if (env->memory->is64 || env->memory->pages!=env->memory->max_pages || !count ||
        count>HG_NATIVE_DMA_MAX_SPANS || table%4) wasm_rt_trap(WASM_RT_TRAP_OOB);
    const u8 *records=memorySpan(env,table,count*sizeof(HGNativeDMARange));
    HGNativeDMASpan spans[HG_NATIVE_DMA_MAX_SPANS];
    for (u32 i=0;i<count;i++) {
        HGNativeDMARange record; memcpy(&record,records+i*sizeof(record),sizeof(record));
        if (!record.size || record.address>UINT32_MAX-record.size) wasm_rt_trap(WASM_RT_TRAP_OOB);
        spans[i]=(HGNativeDMASpan){record.address,memorySpan(env,record.offset,record.size),record.size};
    }
    HGNativeDMARequest request={.channel=channel,.control=control,.source=source,.destination=destination,
        .wait=(command&1)!=0,.fill=command>=HG_DMA_FILL,.value=source,.spans=spans,.spanCount=count};
    if (!HGNativeValidateDMA(&request)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    Mapping *mapping=malloc(sizeof(*mapping));
    if (!mapping) wasm_rt_trap(WASM_RT_TRAP_EXHAUSTION);
    mapping->env=env; memcpy(mapping->spans,spans,count*sizeof(*spans));
    request.spans=mapping->spans; request.context=mapping; request.dispose=dispose;
    __atomic_fetch_add(&env->pendingDMA,1,__ATOMIC_ACQ_REL);
    HGNativeSubmitDMA(&request);
}
