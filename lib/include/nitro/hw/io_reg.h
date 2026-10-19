#ifndef NITRO_IO_REG_H_
#define NITRO_IO_REG_H_

#include <nitro/hw/mmap.h>
#include <nitro/hw/common/io_reg.h>

#ifdef SDK_ARM9
#include <nitro/hw/ARM9/io_reg.h>
#else
#include <nitro/hw/ARM7/io_reg.h>
#endif

#if defined(HEARTGOLD_NATIVE)
#include <nitro/hw/native_graphics.h>
#include "hg_native_registers.h"
#endif

#endif //NITRO_IO_REG_H_
