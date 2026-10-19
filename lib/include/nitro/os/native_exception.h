// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_EXCEPTION_H
#define HEARTGOLD_NATIVE_EXCEPTION_H
#include <nitro/types.h>
#include <nitro/os/exception.h>
typedef struct HGNativeExceptionWork {
    void *debugger;
    void *userArg;
    OSExceptionHandler userHandler;
    u32 hookStack[8];
} HGNativeExceptionWork;
extern HGNativeExceptionWork to91_OSi_ExceptionWork;
extern u32 HGNativeExceptionVectorMain;
void OSi_ExceptionHandler(void);
#endif
