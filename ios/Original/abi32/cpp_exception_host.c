// SPDX-License-Identifier: GPL-3.0-or-later
#include "CppExceptionRuntime.h"
#include "NativeWASIImports.h"
#include "wasm-rt-impl.h"
#include "wasm-rt-exceptions.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef struct CheckInstance { w2c_cxxexceptions module; struct w2c_wasi__snapshot__preview1 wasi; } CheckInstance;
static CheckInstance *create(void) {
    CheckInstance *check=calloc(1,sizeof(*check)); assert(check);
    for (unsigned i=0;i<3;i++) { check->wasi.fds[i]=dup((int)i); assert(check->wasi.fds[i]>=0); }
    wasm2c_cxxexceptions_instantiate(&check->module,&check->wasi);
    check->wasi.memory=w2c_cxxexceptions_memory(&check->module);
    w2c_cxxexceptions_0x5Finitialize(&check->module); return check;
}
static void release(CheckInstance *check) {
    wasm2c_cxxexceptions_free(&check->module);
    for (unsigned i=0;i<3;i++) if (check->wasi.fds[i]>=0) close(check->wasi.fds[i]);
    free(check);
}
int main(void) {
    wasm_rt_init();
    for (unsigned instance=0;instance<2;instance++) {
        CheckInstance *check=create(); u32 stack=check->module.w2c_0x5F_stack_pointer;
#if WASM_RT_STACK_DEPTH_COUNT
        u32 depth=wasm_rt_call_stack_depth;
#endif
        for (unsigned pass=0;pass<100;pass++) {
            u32 line=w2c_cxxexceptions_CheckCppExceptionRuntime(&check->module);
            if (line) fprintf(stderr,"C++ exception check line %u\n",line);
            assert(!line && check->module.w2c_0x5F_stack_pointer==stack && !check->wasi.exited);
#if WASM_RT_STACK_DEPTH_COUNT
            assert(wasm_rt_call_stack_depth==depth);
#endif
        }
        release(check);
    }
    {
        CheckInstance *check=create(); WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_cxxexceptions_RecurseCppExceptionRuntime(&check->module,2000); assert(0); }
        assert(trap==WASM_RT_TRAP_EXHAUSTION && !check->wasi.exited);
        wasm_rt_set_unwind_target(saved); release(check);
    }
    for (unsigned status=0;status<=73;status+=73) {
        CheckInstance *check=create(); WASM_RT_UNWIND_TARGET *saved=wasm_rt_get_unwind_target();
        wasm_rt_trap_t trap=wasm_rt_impl_try();
        if (!trap) { w2c_cxxexceptions_ExitCppExceptionRuntime(&check->module,status); assert(0); }
        assert(trap==WASM_RT_TRAP_UNREACHABLE && check->wasi.exited && check->wasi.exitCode==status);
        wasm_rt_set_unwind_target(saved); release(check);
        if (status==73) break;
    }
    wasm_rt_free();
    puts("PASS: original scalar/nothrow new, 8/12-byte MSL message ownership/copies/rollback, handlers, typed catches, C-frame unwinding, RAII, multiple inheritance, exception_ptr, rethrow, stack recovery and terminal exit status.");
    return 0;
}
