// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <nitro/types.h>
#include <nitro/mi/native_dma.h>

typedef struct HGNativeDMASpan {
    uaddr address;
    void *data;
    u32 size;
} HGNativeDMASpan;
typedef struct HGNativeDMARequest {
    u32 channel, control;
    uaddr source, destination;
    BOOL wait, fill;
    u32 value;
    const HGNativeDMASpan *spans;
    u32 spanCount;
    // Resource cleanup only: runs on the transfer queue, including cancellation; must not reenter DMA.
    void (*dispose)(void *);
    void *context;
} HGNativeDMARequest;

// Descriptors, backing memory and context stay fixed/alive until dispose or a channel wait/stop completes.
#ifdef __cplusplus
extern "C" {
#endif
BOOL HGNativeValidateDMA(const HGNativeDMARequest *request);
void HGNativeSubmitDMA(const HGNativeDMARequest *request);
// Fences all transfer queues without dispatching guest callbacks, for context teardown.
void HGNativeDrainDMA(void);
#ifdef __cplusplus
}
#endif
