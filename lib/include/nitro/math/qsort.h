#ifndef NITRO_MATH_QSORT_H_
#define NITRO_MATH_QSORT_H_

#include <nitro/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*MATHQSortComparator)(const void *a, const void *b);

// Optional work storage holds two 32-bit words per bit in count.
void MATH_QSort(void *base, u32 count, u32 width, MATHQSortComparator compare, void *work);

#ifdef __cplusplus
}
#endif

#endif
