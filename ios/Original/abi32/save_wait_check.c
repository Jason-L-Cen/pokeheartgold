// SPDX-License-Identifier: GPL-3.0-or-later
// Exercise the actual private save poll without exporting test hooks in the game build.
#include "../../../src/save.c"

void HGSaveWaitReset(void) { saveWritten=FALSE; }
u32 HGSaveWaitWritten(void) { return saveWritten==TRUE; }
u32 HGSaveWaitPoll(void) { return WaitFlashWrite(0,FALSE,NULL); }
u32 HGSaveWaitCallback(void) { return (u32)(uintptr_t)FlashWriteCommandCallback; }
