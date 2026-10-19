// SPDX-License-Identifier: GPL-3.0-or-later
#include <PAD_pad.h>
u32 HGLegacyPad(u32 fold) { return fold ? PAD_DetectFold() : PAD_Read(); }
