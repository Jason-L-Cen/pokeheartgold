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
    void *owner;
    // Resource/atomic notification after an actual write; must not reenter DMA or guest callbacks.
    void (*didWrite)(void *context,uaddr address,u32 size);
    // Resource cleanup only: runs on the transfer queue or stopping owner; must not reenter DMA.
    void (*dispose)(void *);
    void *context;
} HGNativeDMARequest;

// Repeat descriptors, backing memory and context stay fixed/alive until stop/reconfiguration disposes them.
#ifdef __cplusplus
extern "C" {
#endif
BOOL HGNativeValidateDMA(const HGNativeDMARequest *request);
void HGNativeSubmitDMA(const HGNativeDMARequest *request);
// Fences all transfer queues without dispatching guest callbacks, for context teardown.
void HGNativeDrainDMA(void);
// Owner scanline transaction only, after row capture and before the CPU HBlank IRQ.
void HGNativeHBlankDMA(void *owner);
#ifdef __cplusplus
}
#endif
