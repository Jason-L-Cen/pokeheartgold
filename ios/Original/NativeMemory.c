// SPDX-License-Identifier: GPL-3.0-or-later
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>
#ifdef HEARTGOLD_ABI32
#include <string.h>
#include <unistd.h>
static void writeMessage(const char *message) {
    size_t remaining=strlen(message);
    while (remaining) {
        ssize_t written=write(2,message,remaining);
        if (written<=0) return;
        message+=written; remaining-=(size_t)written;
    }
}
#endif

#ifndef HEARTGOLD_ABI32
void GF_AssertFail(void) {
    fputs("Original game assertion failed.\n", stderr);
    abort();
}
#endif
void HGNativeAssertFail(const char *condition, const char *file, int line) {
#ifdef HEARTGOLD_ABI32
    // MSL owns __stdio_exit as data; avoid pulling WASI's incompatible FILE cleanup function.
    char number[12]; snprintf(number,sizeof(number),"%d",line);
    writeMessage(file); writeMessage(":"); writeMessage(number);
    writeMessage(": original game assertion: "); writeMessage(condition); writeMessage("\n");
    __builtin_trap();
#else
    fprintf(stderr, "%s:%d: original game assertion: %s\n", file, line, condition);
    abort();
#endif
}
