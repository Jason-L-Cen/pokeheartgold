#ifndef HEARTGOLD_NATIVE_IRQ_ABI_H
#define HEARTGOLD_NATIVE_IRQ_ABI_H
#include <nitro/types.h>
enum HGNativeIrqCommand {
    HG_IRQ_ENABLE,HG_IRQ_DISABLE,HG_IRQ_RESTORE,HG_IRQ_GET_MASK,HG_IRQ_SET_MASK,
    HG_IRQ_ENABLE_MASK,HG_IRQ_DISABLE_MASK,HG_IRQ_GET_REQUEST,HG_IRQ_RESET_REQUEST,
    HG_IRQ_GET_CHECK,HG_IRQ_SET_CHECK,HG_IRQ_CLEAR_CHECK,HG_IRQ_GET_VBLANK,
    HG_IRQ_WAIT,HG_IRQ_WAIT_CLEAR
};
enum HGNativeIrqKind { HG_IRQ_PLAIN,HG_IRQ_DMA,HG_IRQ_TIMER };
#ifdef HEARTGOLD_ABI32
extern void HGNativeIrqInit(u32 dummy) __attribute__((import_module("env"),import_name("HGNativeIrqInit")));
extern u32 HGNativeIrqControl(u32 command,u32 value) __attribute__((import_module("env"),import_name("HGNativeIrqControl")));
extern void HGNativeIrqSet(u32 kind,u32 target,u32 function,u32 argument) __attribute__((import_module("env"),import_name("HGNativeIrqSet")));
extern u32 HGNativeIrqGet(u32 mask) __attribute__((import_module("env"),import_name("HGNativeIrqGet")));
#endif
#endif
