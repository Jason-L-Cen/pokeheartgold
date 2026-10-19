// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeFiber.h"
#include <fenv.h>
#include <setjmp.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#if __has_feature(address_sanitizer)
#include <sanitizer/common_interface_defs.h>
#endif
#if __has_feature(thread_sanitizer)
#include <sanitizer/tsan_interface.h>
#endif

struct HGNativeFiber {
    jmp_buf jump;
    fenv_t floating;
    void (*entry)(void *);
    void *argument,*mapping,*bottom,*fakeStack,*tsan;
    size_t mappingSize,size;
    HGNativeFiber *incoming;
    bool started,terminated;
};
_Noreturn void HGNativeStartOnStack(void *top,void (*entry)(void *),void *argument);
static void arrived(HGNativeFiber *fiber) {
#if __has_feature(address_sanitizer)
    const void *bottom=NULL; size_t size=0;
    __sanitizer_finish_switch_fiber(fiber->fakeStack,&bottom,&size);
    if (fiber->incoming) { fiber->incoming->bottom=(void *)bottom; fiber->incoming->size=size; }
#endif
    if (fesetenv(&fiber->floating)) abort();
}
static _Noreturn void start(void *argument) {
    HGNativeFiber *fiber=argument;
    arrived(fiber); fiber->entry(fiber->argument); abort();
}
HGNativeFiber *HGNativeFiberRoot(void) {
    HGNativeFiber *fiber=calloc(1,sizeof(*fiber)); if (!fiber) return NULL;
    if (fegetenv(&fiber->floating)) { free(fiber); return NULL; }
    fiber->started=true;
#if __has_feature(thread_sanitizer)
    fiber->tsan=__tsan_get_current_fiber();
#endif
    return fiber;
}
HGNativeFiber *HGNativeFiberCreate(void (*entry)(void *),void *argument) {
    if (!entry) return NULL;
    HGNativeFiber *fiber=calloc(1,sizeof(*fiber)); if (!fiber) return NULL;
    long page=sysconf(_SC_PAGESIZE);
    if (page<=0 || fegetenv(&fiber->floating)) { free(fiber); return NULL; }
    fiber->size=512*1024; fiber->mappingSize=fiber->size+2*(size_t)page;
    fiber->mapping=mmap(NULL,fiber->mappingSize,PROT_NONE,MAP_PRIVATE|MAP_ANON,-1,0);
    if (fiber->mapping==MAP_FAILED) { free(fiber); return NULL; }
    fiber->bottom=(char *)fiber->mapping+page;
    if (mprotect(fiber->bottom,fiber->size,PROT_READ|PROT_WRITE)) {
        munmap(fiber->mapping,fiber->mappingSize); free(fiber); return NULL;
    }
    fiber->entry=entry; fiber->argument=argument;
#if __has_feature(thread_sanitizer)
    fiber->tsan=__tsan_create_fiber(0);
#endif
    return fiber;
}
static _Noreturn void transfer(HGNativeFiber *from,HGNativeFiber *to,bool terminate) {
    to->incoming=from;
#if __has_feature(address_sanitizer)
    __sanitizer_start_switch_fiber(terminate ? NULL : &from->fakeStack,to->bottom,to->size);
#endif
#if __has_feature(thread_sanitizer)
    __tsan_switch_to_fiber(to->tsan,0);
#endif
    if (to->started) longjmp(to->jump,1);
    to->started=true;
    HGNativeStartOnStack((char *)to->bottom+to->size,start,to);
}
void HGNativeFiberSwitch(HGNativeFiber *from,HGNativeFiber *to,bool terminate) {
    if (!from || !to || from==to || from->terminated || to->terminated) abort();
    if (terminate) { from->terminated=true; transfer(from,to,true); }
    if (fegetenv(&from->floating)) abort();
    // This frame remains live on its own stack until resumed; it never returns before longjmp.
    if (!setjmp(from->jump)) transfer(from,to,false);
    arrived(from);
}
void HGNativeFiberDestroy(HGNativeFiber *fiber) {
    if (!fiber) return;
    if (fiber->mapping) {
        if (fiber->started && !fiber->terminated) abort();
#if __has_feature(thread_sanitizer)
        __tsan_destroy_fiber(fiber->tsan);
#endif
        munmap(fiber->mapping,fiber->mappingSize);
    }
    free(fiber);
}
