#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#include "lobby_vector.h"

char *LobbyByteVector_Data(LobbyCharVector *vector) {
    if (vector->size == 0) {
        return NULL;
    }
    return vector->data;
}
