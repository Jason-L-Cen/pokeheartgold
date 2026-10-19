// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef HEARTGOLD_NATIVE_WASI_IMPORTS_H
#define HEARTGOLD_NATIVE_WASI_IMPORTS_H
#include "wasm-rt.h"
#include <stdint.h>

// Each instance owns descriptor copies. Close remaining descriptors after releasing the module.
// Bind memory after instantiation, before _initialize; keep it alive until all module calls stop.
struct w2c_wasi__snapshot__preview1 { wasm_rt_memory_t *memory; int fds[3]; };
enum { WASI_AGAIN=6,WASI_BADF=8,WASI_FAULT=21,WASI_INTR=27,WASI_INVAL=28,WASI_IO=29,WASI_OVERFLOW=61,WASI_SPIPE=70 };
#ifdef __cplusplus
extern "C" {
#endif
uint32_t w2c_wasi__snapshot__preview1_clock_time_get(struct w2c_wasi__snapshot__preview1 *,uint32_t,uint64_t,uint32_t);
uint32_t w2c_wasi__snapshot__preview1_fd_close(struct w2c_wasi__snapshot__preview1 *,uint32_t);
uint32_t w2c_wasi__snapshot__preview1_fd_fdstat_get(struct w2c_wasi__snapshot__preview1 *,uint32_t,uint32_t);
uint32_t w2c_wasi__snapshot__preview1_fd_seek(struct w2c_wasi__snapshot__preview1 *,uint32_t,uint64_t,uint32_t,uint32_t);
uint32_t w2c_wasi__snapshot__preview1_fd_write(struct w2c_wasi__snapshot__preview1 *,uint32_t,uint32_t,uint32_t,uint32_t);
#ifdef __cplusplus
}
#endif
#endif
