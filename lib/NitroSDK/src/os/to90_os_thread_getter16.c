#pragma force_active on
#include <nitro/types.h>
// clang-format off
#include <nitro/code32.h>
// clang-format on
extern BOOL OSi_IsThreadInitialized;
BOOL OS_IsThreadAvailable(void);
// @fn OS_IsThreadAvailable
BOOL OS_IsThreadAvailable(void){return OSi_IsThreadInitialized;}
