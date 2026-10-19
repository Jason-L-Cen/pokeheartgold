// SPDX-License-Identifier: GPL-3.0-or-later
// Native Apple arm64 stack bootstrap; libc preserves subsequent continuation registers.
#if !defined(__aarch64__)
#error Native fibers currently require Apple arm64
#endif
.text
.p2align 2
.globl _HGNativeStartOnStack
_HGNativeStartOnStack:
    mov sp, x0
    mov x0, x2
    br x1
