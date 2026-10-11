#include <nitro/types.h>

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

// The default terminate / unexpected handlers and std::terminate / std::unexpected, which call the handlers
// installed in the handler table. (Written as C with the mangled names.)

extern void (*_02111848[2])(void); // [0] terminate handler, [1] unexpected handler

void abort(void);

void _ZSt9dthandlerv(void);
void _ZSt9duhandlerv(void);
void _ZSt9terminatev(void);
void _ZSt10unexpectedv(void);

void _ZSt9dthandlerv(void);
#pragma force_active on

void _ZSt9dthandlerv(void) {
    abort();
}

void _ZSt9duhandlerv(void);

void _ZSt9duhandlerv(void) {
    _ZSt9terminatev();
}

void _ZSt9terminatev(void);

void _ZSt9terminatev(void) {
    _02111848[0]();
}

void _ZSt10unexpectedv(void);

void _ZSt10unexpectedv(void) {
    _02111848[1]();
}
