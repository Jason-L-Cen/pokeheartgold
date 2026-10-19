#pragma force_active on
#include <cw/console_io.h>
#ifdef HEARTGOLD_NATIVE
void sys_writec(const char *c) {
    u32 count=1; (void)__write_console(1,c,&count,NULL);
}
int sys_readc(void) {
    u8 value; u32 count=1;
    return !__read_console(0,&value,&count,NULL) && count ? value : -1;
}
void sys_exit(void) {
    static const char message[]="Original runtime requested termination.\n";
    u32 count=sizeof(message)-1; (void)__write_console(1,message,&count,NULL);
    __builtin_trap();
}
#else

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// clang-format off
asm void sys_writec(const char *c) {
    str lr, [sp, #-4]!
    mov r1, r0
    mov r0, #3
    swi 0x123456
    ldr pc, [sp], #4
}
// clang-format on

// clang-format off
asm int sys_readc(void) {
    str lr, [sp, #-4]!
    mov r1, #0
    mov r0, #7
    swi 0x123456
    ldr pc, [sp], #4
}
// clang-format on

// clang-format off
asm void sys_exit(void) {
    mov r1, #0
    mov r0, #0x18
    swi 0x123456
    mov pc, lr
}
// clang-format on
#endif
