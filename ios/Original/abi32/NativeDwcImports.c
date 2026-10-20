// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeBackup.h"
#include <dwc/native_settings.h>

u32 w2c_env_HGNativeDwcSettings(struct w2c_env *env,u32 offset,u32 buffer,u32 size,u32 write) {
    if (!env || !env->memory || !env->memory->data || env->resetRequested || write>1 ||
        (!buffer && size) || buffer>env->memory->size || size>env->memory->size-buffer)
        wasm_rt_trap(WASM_RT_TRAP_OOB);
    return HGNativeBackupSettings(offset,env->memory->data+buffer,size,write);
}
u64 w2c_env_HGNativeDwcIdentity(struct w2c_env *env) {
    if (!env || !env->memory || env->resetRequested) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return HGNativeBackupConsoleId();
}
