// SPDX-License-Identifier: GPL-3.0-or-later
#include <nnsys/fnd_internal.h>
#include <nnsys/g3d_util_internal.h>

BOOL gFndRootHeapListInitialized;
NNSFndList gFndRootHeapList;
const NNSFndAllocatorFunc gFndExpHeapAllocatorFunc = { AllocatorAllocForExpHeap, AllocatorFreeForExpHeap };
// The full ABI32 data merge owns the original BSS; direct/probe builds need typed native storage.
G3dGeBufferState gGeBufferState;
// Original nnsys_data.s camera vectors; full ABI32 merges retain the original data object.
const VecFx32 gG2dSoftSpriteCameraUp={0,FX32_ONE,0};
const VecFx32 gG2dSoftSpriteCameraTarget={0,0,-FX32_ONE};
