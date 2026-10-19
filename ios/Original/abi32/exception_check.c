// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <nitro/os/native_exception.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
static void userHandler(u32 error,void *argument) { (void)error; (void)argument; }
u32 HGExceptionAddress(u32 which) {
    return which==0 ? (u32)(uaddr)&to91_OSi_ExceptionWork : which==1 ? (u32)(uaddr)&HGNativeExceptionVectorMain :
        which==2 ? HW_EXCP_VECTOR_BUF : (u32)(uaddr)OSi_ExceptionHandler;
}
u32 CheckOriginalExceptionInit(void) {
    CHECK(sizeof(to91_OSi_ExceptionWork)==44 && offsetof(HGNativeExceptionWork,hookStack)==12);
    const u32 vectors[]={0,0x025FFFFC,0x02600000,0x027FFFFC,0x02800000,0xFFFFFFFF};
    const u32 handler=(u32)(uaddr)OSi_ExceptionHandler;
    for (u32 n=0;n<sizeof(vectors)/sizeof(*vectors);n++) {
        HGNativeExceptionWork initial={.debugger=(void *)(uaddr)0xDEADBEEF,.userArg=(void *)(uaddr)0x12345678,.userHandler=userHandler};
        for (u32 i=0;i<8;i++) initial.hookStack[i]=0x87654320+i;
        to91_OSi_ExceptionWork=initial;
        HGNativeExceptionVectorMain=vectors[n]; *(u32 *)(uaddr)HW_EXCP_VECTOR_BUF=0xABCDEF00;
        OS_InitException();
        BOOL debugger=vectors[n]>=0x02600000 && vectors[n]<0x02800000;
        CHECK(to91_OSi_ExceptionWork.debugger==(debugger ? (void *)(uaddr)vectors[n] : NULL));
        CHECK(!to91_OSi_ExceptionWork.userHandler && to91_OSi_ExceptionWork.userArg==initial.userArg);
        CHECK(!memcmp(to91_OSi_ExceptionWork.hookStack,initial.hookStack,sizeof(initial.hookStack)));
        CHECK(HGNativeExceptionVectorMain==(debugger ? vectors[n] : handler));
        CHECK(*(u32 *)(uaddr)HW_EXCP_VECTOR_BUF==(debugger ? 0xABCDEF00 : handler));
    }
    HGNativeExceptionVectorMain=0; OS_InitException(); OS_InitException();
    CHECK(!to91_OSi_ExceptionWork.debugger && !to91_OSi_ExceptionWork.userHandler);
    CHECK(HGNativeExceptionVectorMain==handler && *(u32 *)(uaddr)HW_EXCP_VECTOR_BUF==handler);
    return 0;
}
