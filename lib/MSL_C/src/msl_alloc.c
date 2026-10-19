#pragma force_active on
#include <nitro/types.h>
#pragma force_active on
#ifdef HEARTGOLD_ABI32
// SDK exception helpers and game C allocations must share one native libc heap.
void *__real_malloc(u32);
void __real_free(void *);
void *__wrap_malloc(u32 n) { return n ? __real_malloc(n) : 0; }
void __wrap_free(void *p) { if (p) __real_free(p); }
#else
void *__sys_alloc(u32);
void __sys_free(void *);
void *malloc(u32);
void free(void *);

void *malloc(u32 n) {
    if (!n) {
        return 0;
    }
    return __sys_alloc(n);
}

void free(void *p) {
    if (p) {
        __sys_free(p);
    }
}
#endif
