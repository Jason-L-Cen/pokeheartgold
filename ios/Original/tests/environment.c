// SPDX-License-Identifier: GPL-3.0-or-later
// The data-core check has no communication session or interactive reset screen.
#include "comm_state.h"
#include "error_message_reset.h"
#include <stdlib.h>

BOOL CommState_Exists(void) { return FALSE; }
void PrintErrorMessageAndReset(void) { abort(); }
