// SPDX-License-Identifier: GPL-3.0-or-later
#include <nnsys/fnd_internal.h>

BOOL gFndRootHeapListInitialized;
NNSFndList gFndRootHeapList;
const NNSFndAllocatorFunc gFndExpHeapAllocatorFunc = { AllocatorAllocForExpHeap, AllocatorFreeForExpHeap };
