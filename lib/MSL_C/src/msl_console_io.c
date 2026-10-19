#include <cw/console_io.h>
#ifdef HEARTGOLD_NATIVE
#include <errno.h>
#include <unistd.h>
#endif

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

// Console I/O over the debugger's character channel (sys_readc / sys_writec).

int __read_console(int handle, u8 *buf, u32 *length, void *data) {
    u32 i, size = *length;
    for (i = 0; i < size; i++) {
#ifdef HEARTGOLD_NATIVE
        ssize_t result;
        do { result=read(0,buf+i,1); } while (result<0 && errno==EINTR);
        if (result!=1) { *length=i; return result<0 ? 1 : (i ? 0 : 2); }
        if (buf[i]==13 || buf[i]==10) {
#else
        if ((buf[i] = sys_readc()) == 13 || buf[i] == 10) {
#endif
            *length = i + 1;
            break;
        }
    }
    return 0;
}

int __write_console(int handle, const char *buffer, u32 *count, void *ref) {
#ifdef HEARTGOLD_NATIVE
    u32 done=0,size=*count;
    while (done<size) {
        ssize_t result;
        do { result=write(1,buffer+done,size-done); } while (result<0 && errno==EINTR);
        if (result<=0) { *count=done; return 1; }
        done+=(u32)result;
    }
#else
    u32 i;
    u32 size = *count;
    for (i = 0; i < size; i++) {
        sys_writec(buffer + i);
    }
#endif
    return 0;
}

int __close_console(u32 handle) {
    return 0;
}
