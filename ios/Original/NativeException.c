// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/os/native_exception.h>
#include <stddef.h>
#include "assert.h"

HGNativeExceptionWork to91_OSi_ExceptionWork;
u32 HGNativeExceptionVectorMain;
_Static_assert(sizeof(HGNativeExceptionWork)==44,"Original exception work size");
_Static_assert(offsetof(HGNativeExceptionWork,userArg)==4 && offsetof(HGNativeExceptionWork,userHandler)==8 &&
    offsetof(HGNativeExceptionWork,hookStack)==12,"Original exception work offsets");

void OSi_ExceptionHandler(void) {
    // Native faults use host diagnostics; ARM registers/debugger entry points cannot execute here.
    HGNativeAssertFail("Original DS exception vector reached native code",__FILE__,__LINE__);
    __builtin_trap();
}
