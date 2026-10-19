// SPDX-License-Identifier: GPL-3.0-or-later
#include <CTRDG_init.h>

// The original data file supplies an empty anchor; native callers need the complete documented state.
CTRDGiInitWork gCtrdgInitWork;
#ifdef HEARTGOLD_ABI32
_Static_assert(sizeof(CTRDGiInitWork)==28,"Original cartridge initialization state layout");
#endif
