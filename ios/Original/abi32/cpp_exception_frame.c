// SPDX-License-Identifier: GPL-3.0-or-later
void CppExceptionCFrame(void (*function)(void)) {
    volatile unsigned char local[1024];
    for (unsigned i=0;i<sizeof(local);i++) local[i]=(unsigned char)i;
    function();
    if (local[1]!=1) __builtin_trap();
}
