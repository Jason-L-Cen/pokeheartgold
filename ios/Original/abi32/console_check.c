// SPDX-License-Identifier: GPL-3.0-or-later
#include <cw/console_io.h>
static u8 buffer[32];
static u32 length;
u32 HGConsoleReference(u32 field) { return field ? (u32)&length : (u32)buffer; }
u32 HGConsoleAction(u32 action,u32 size) {
    length=size;
    if (!action) return __read_console(0,buffer,&length,NULL);
    if (action==1) return __write_console(1,(const char *)buffer,&length,NULL);
    if (action==2) { sys_writec((const char *)buffer); return 0; }
    if (action==3) return (u32)sys_readc();
    sys_exit(); return 2;
}
