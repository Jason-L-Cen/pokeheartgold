// SPDX-License-Identifier: GPL-3.0-or-later
#include "assert.h"
#include <stdio.h>
#include <stdlib.h>

void GF_AssertFail(void) {
    fputs("Original game assertion failed.\n", stderr);
    abort();
}
void HGNativeAssertFail(const char *condition, const char *file, int line) {
    fprintf(stderr, "%s:%d: original game assertion: %s\n", file, line, condition);
    abort();
}
