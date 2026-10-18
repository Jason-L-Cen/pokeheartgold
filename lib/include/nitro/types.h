#ifndef NITRO_TYPES_H_
#define NITRO_TYPES_H_

#ifndef SDK_ASM
#ifdef HEARTGOLD_NATIVE
#include <stdint.h>
#endif
typedef unsigned char u8;
typedef unsigned short int u16;
#ifdef HEARTGOLD_NATIVE
typedef uint32_t u32;
#else
typedef unsigned long u32;
#endif

typedef signed char s8;
typedef signed short int s16;
#ifdef HEARTGOLD_NATIVE
typedef int32_t s32;
#else
typedef signed long s32;
#endif

#ifdef HEARTGOLD_NATIVE
typedef uintptr_t uaddr;
#else
typedef u32 uaddr;
#endif

#ifdef HEARTGOLD_NATIVE
// Preserve serialized DS structs; the native ABI otherwise aligns these fields to 8 bytes.
typedef unsigned long long int u64 __attribute__((aligned(4)));
typedef signed long long int s64 __attribute__((aligned(4)));
#else
typedef unsigned long long int u64;
typedef signed long long int s64;
#endif

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

typedef float f32;
typedef volatile f32 vf32;

typedef u8 REGType8;
typedef u16 REGType16;
typedef u32 REGType32;
typedef u64 REGType64;

typedef vu8 REGType8v;
typedef vu16 REGType16v;
typedef vu32 REGType32v;
typedef vu64 REGType64v;

typedef int BOOL;
#endif

#define TRUE 1
#define FALSE 0

#ifndef SDK_ASM
#ifndef NULL
#ifdef  __cplusplus
#define NULL 0
#else  // __cplusplus
#define NULL ((void *)0)
#endif // __cplusplus
#endif

#define SDK_FORCE_EXPORT __declspec(force_export)
#endif //SDK_ASM
#endif //NITRO_TYPES_H_
