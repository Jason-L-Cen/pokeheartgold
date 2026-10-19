// SPDX-License-Identifier: GPL-3.0-or-later
#include <dsprot.h>
#ifndef HEARTGOLD_NATIVE
#error Native platform classification must not replace the DS build's hardware probes.
#endif

// A prelinked native process has no DS flashcart, DS CPU emulator or encrypted ARM wrappers.
static u32 nativeEnvironment(void *callback) {
    if (callback) ((void (*)(void))callback)();
    return 1;
}
u32 DSProt_DetectFlashcart(void *callback) { (void)callback; return 0; }
u32 DSProt_DetectNotFlashcart(void *callback) { return nativeEnvironment(callback); }
u32 DSProt_DetectEmulator(void *callback) { (void)callback; return 0; }
u32 DSProt_DetectNotEmulator(void *callback) { return nativeEnvironment(callback); }
u32 DSProt_DetectDummy(void *callback) { (void)callback; return 0; }
u32 DSProt_DetectNotDummy(void *callback) { return nativeEnvironment(callback); }
