// SPDX-License-Identifier: GPL-3.0-or-later
#include <nnsys/fnd_internal.h>
#include <nnsys/g3d_util_internal.h>

BOOL gFndRootHeapListInitialized;
NNSFndList gFndRootHeapList;
const NNSFndAllocatorFunc gFndExpHeapAllocatorFunc = { AllocatorAllocForExpHeap, AllocatorFreeForExpHeap };
// The full ABI32 data merge owns the original BSS; direct/probe builds need typed native storage.
G3dGeBufferState gGeBufferState;
