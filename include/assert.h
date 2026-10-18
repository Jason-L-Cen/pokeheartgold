#ifndef POKEHEARTGOLD_ASSERT_H
#define POKEHEARTGOLD_ASSERT_H

#include "error_handling.h"

/*
 * Assert statements. If the expression evaluates false,
 * the software will terminate.
 *
 * Pass NO_GF_ASSERT=1 to make to remove these.
 */
#ifdef PM_KEEP_ASSERTS
#ifdef HEARTGOLD_NATIVE
#ifdef __cplusplus
extern "C" {
#endif
void HGNativeAssertFail(const char *condition, const char *file, int line);
#ifdef __cplusplus
}
#endif
#define GF_ASSERT(expr) ((expr) ? (void)0 : HGNativeAssertFail(#expr, __FILE__, __LINE__))
#else
#define GF_ASSERT(expr) ((expr) ? (void)0 : GF_AssertFail())
#endif
#else
#define GF_ASSERT(...) ((void)0)
#endif // PM_KEEP_ASSERTS

#endif // POKEHEARTGOLD_ASSERT_H
