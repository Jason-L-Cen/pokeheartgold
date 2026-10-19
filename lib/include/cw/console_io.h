#ifndef HEARTGOLD_MSL_CONSOLE_IO_H
#define HEARTGOLD_MSL_CONSOLE_IO_H
#include <nitro/types.h>
int sys_readc(void);
void sys_writec(const char *);
void sys_exit(void);
int __read_console(int,u8 *,u32 *,void *);
int __write_console(int,const char *,u32 *,void *);
int __close_console(u32);
#endif
