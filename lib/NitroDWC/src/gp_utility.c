#pragma force_active on
#include <stdarg.h>
#include <string.h>

#include "global.h"

#include "dwc/gp_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// Presence / buddy connection: processing of the pending searches, the "\rn\" and "\error\" server
// replies, and the string helpers ("\key\value" parsing, bounded copy).

extern const char ov00_022192BC[]; // "gpiProcessSearches"
extern const char ov00_02219328[]; // "Out of memory."
extern const char ov00_022193E4[]; // "gpiSearch.c"
extern const char ov00_022196A4[]; // "num < iconnection->numSearches"
extern const char ov00_022196C4[]; // ""
extern const char ov00_022196C8[]; // "\\version\\%d\\result\\%d"
extern const char ov00_022196E0[]; // "\\xfer\\"
extern const char ov00_022196E8[]; // "%d %u %u"
extern const char ov00_022196F4[]; // "\\rn\\"
extern const char ov00_022196FC[]; // "Unexpected data was received from the server."
extern const char ov00_0221972C[]; // "Out of memory."
extern const char ov00_0221973C[]; // "strzcpy"
extern const char ov00_02219744[]; // "gpiValueForKey"
extern const char ov00_02219754[]; // "gpiReadKeyAndValue"
extern const char ov00_02219768[]; // "dest != NULL"
extern const char ov00_02219778[]; // "gpiUtility.c"
extern const char ov00_02219788[]; // "src != NULL"
extern const char ov00_02219794[]; // "\\error\\"
extern const char ov00_0221979C[]; // "\\err\\"
extern const char ov00_022197A4[]; // "\\errmsg\\"
extern const char ov00_022197B0[]; // "\\fatal\\"
extern const char ov00_022197B8[]; // "command != NULL"
extern const char ov00_022197C8[]; // "key != NULL"
extern const char ov00_022197D4[]; // "value != NULL"
extern const char ov00_022197E4[]; // "len > 0"
extern const char ov00_022197EC[]; // "Error connecting\n"
extern const char ov00_02219800[]; // "There was an error checking for a completed connection."
extern const char ov00_02219838[]; // "Connection rejected\n"
extern const char ov00_02219850[]; // "Connection accepted\n"
extern const char ov00_02219868[]; // "buffer != NULL"
extern const char ov00_02219878[]; // "Parse Error."

extern int sprintf(char *, const char *, ...);
extern int strncmp(const char *, const char *, size_t);
extern int sscanf(const char *, const char *, ...);
extern long atol(const char *);
extern void __msl_assertion_failed(const char *, const char *, const char *, int);
extern void *DWCi_GsMalloc(u32 size);
extern void DWCi_GsFree(void *p);
extern int GSISocketSelect(int a, int b, int *x, int *y);
extern int gpiSendOrBufferString(GpConnection **connection, void *operation, char *buf);
extern void gpiCallErrorCallback(GpConnection **connection, int a, int b);
extern int gpiAddCallback(GpConnection **connection, GpPair pair, int *p, void *operation, int z);
extern int gpiPeerStartTransferMessage(GpConnection **connection, void *operation, int code, void *values);
extern int gpiPeerFinishTransferMessage(GpConnection **connection, void *operation, const char *s, int n);

// Sets the parse error and disconnects.
#define GP_PARSE_ERROR(connection)                   \
    {                                                \
        gpiSetError(connection, 1, ov00_02219878); \
        gpiCallErrorCallback(connection, 3, 1);             \
        return 3;                                    \
    }

int gpiProcessSearches(GpConnection **connection) {
    GpConnection *iconnection = *connection;
    int num = 0;
    GpOperation **searches;
    GpOperation *search;
    int i;
    int result;
    if (iconnection->numSearches > 0) {
        searches = DWCi_GsMalloc(iconnection->numSearches * 4);
        if (!searches) {
            gpiSetErrorString(connection, ov00_02219328);
            return 1;
        }
        for (search = iconnection->operations; search; search = search->next) {
            if (search->type == GP_OPERATION_SEARCH && search->state != 5 && ((GpSearchData *)search->data)->busy == 0) {
                if (!(num < iconnection->numSearches)) {
                    __msl_assertion_failed(ov00_022196A4, ov00_022193E4, ov00_022192BC, 0x563);
                }
                searches[num++] = search;
                ((GpSearchData *)search->data)->busy = 1;
            }
        }
        for (i = 0; i < num; i++) {
            result = gpiProcessSearch(connection, searches[i]);
            if (result) {
                searches[i]->result = result;
            }
        }
        for (i = 0; i < num; i++) {
            GpSearchData *data = searches[i]->data;
            data->busy = 0;
            if (data->done) {
                gpiRemoveOperation(connection, searches[i]);
            }
        }
        DWCi_GsFree(searches);
    }
    return 0;
}

int gpiSendTransferReply(GpConnection **connection, int *values, void *operation, int result, const char *message) {
    char buf[0x20];
    int err;
    if (!message) {
        message = ov00_022196C4;
    }
    err = gpiPeerStartTransferMessage(connection, operation, 0xc9, values);
    if (err) {
        return err;
    }
    sprintf(buf, ov00_022196C8, 1, result);
    err = gpiSendOrBufferString(connection, operation, buf);
    if (err) {
        return err;
    }
    err = gpiPeerFinishTransferMessage(connection, operation, message, -1);
    if (err) {
        return err;
    }
    return 0;
}

// Handles a "\xfer\" message of a peer.
void gpiHandleTransferMessage(GpConnection **connection, void *peer, int type, const char *headers, char *buffer, int len) {
    char buf[0x40];
    int values[3];
    if (!gpiValueForKey(headers, ov00_022196E0, buf, 0x40)) {
        return;
    }
    if (sscanf(buf, ov00_022196E8, &values[0], &values[1], &values[2]) != 3) {
        return;
    }
    gpiSendTransferReply(connection, values, peer, 2, 0);
}

int gpiProcessRegisterUniqueNick(GpConnection **connection, GpOperation *operation, char *message) {
    GpPair pair;
    int *p;
    int err;
    if (gpiCheckForError(connection, message, 1)) {
        return 4;
    }
    if (strncmp(message, ov00_022196F4, 4) != 0) {
        gpiSetError(connection, 1, ov00_022196FC);
        gpiCallErrorCallback(connection, 3, 1);
        return 3;
    }
    pair = *(GpPair *)&operation->callback;
    if (pair.a) {
        p = DWCi_GsMalloc(4);
        if (!p) {
            gpiSetErrorString(connection, ov00_0221972C);
            return 1;
        }
        *p = 0;
        err = gpiAddCallback(connection, pair, p, operation, 0);
        if (err) {
            return err;
        }
    }
    gpiRemoveOperation(connection, operation);
    return 0;
}

void strzcpy(char *dest, const char *src, int len) {
    if (!dest) {
        __msl_assertion_failed(ov00_02219768, ov00_02219778, ov00_0221973C, 0x2f);
    }
    if (!src) {
        __msl_assertion_failed(ov00_02219788, ov00_02219778, ov00_0221973C, 0x30);
    }
    strncpy(dest, src, len);
    *(dest + len - 1) = 0;
}

void gpiDebug(GpConnection **connection, const char *format, ...) {
}

int gpiCheckForError(GpConnection **connection, const char *message, int callback) {
    char buf[0x10];
    GpConnection *iconnection = *connection;
    if (strncmp(message, ov00_02219794, 7) == 0) {
        if (gpiValueForKey(message, ov00_0221979C, buf, 0x10)) {
            iconnection->errorCode = atol(buf);
        }
        if (!gpiValueForKey(message, ov00_022197A4, iconnection->errorString, 0x100)) {
            iconnection->errorString[0] = 0;
        }
        if (callback) {
            gpiCallErrorCallback(connection, 4, (strstr(message, ov00_022197B0) != NULL ? 1 : 0) ? 1 : 0);
        }
        return 1;
    }
    return 0;
}

int gpiValueForKey(const char *command, const char *key, char *value, int len) {
    char term;
    const char *p;
    int i;
    char ch;
    if (!command) {
        __msl_assertion_failed(ov00_022197B8, ov00_02219778, ov00_02219744, 0x10d);
    }
    if (!key) {
        __msl_assertion_failed(ov00_022197C8, ov00_02219778, ov00_02219744, 0x10e);
    }
    if (!value) {
        __msl_assertion_failed(ov00_022197D4, ov00_02219778, ov00_02219744, 0x10f);
    }
    if (!(len > 0)) {
        __msl_assertion_failed(ov00_022197E4, ov00_02219778, ov00_02219744, 0x110);
    }
    term = key[0];
    p = strstr(command, key);
    if (!p) {
        return 0;
    }
    p += strlen(key);
    for (i = 0; i < len - 1 && (ch = p[i]) != 0 && ch != term; i++) {
        value[i] = ch;
    }
    value[i] = 0;
    return 1;
}

int gpiCheckSocketConnect(GpConnection **connection, int socket, int *result) {
    int accepted = 0;
    int rejected = 0;
    int r = GSISocketSelect(socket, 0, &accepted, &rejected);
    if (r == -1) {
        gpiDebug(connection, ov00_022197EC);
        gpiSetError(connection, 5, ov00_02219800);
        gpiCallErrorCallback(connection, 3, 1);
        return 3;
    }
    if (r > 0) {
        if (rejected) {
            gpiDebug(connection, ov00_02219838);
            *result = 4;
            return 0;
        }
        if (accepted) {
            gpiDebug(connection, ov00_02219850);
            *result = 3;
            return 0;
        }
    }
    *result = 0;
    return 0;
}

int gpiReadKeyAndValue(GpConnection **connection, const char *buffer, int *index, char *key, char *value) {
    const char *start;
    char c;
    int i;
    int j;
    if (buffer == NULL) {
        __msl_assertion_failed(ov00_02219868, ov00_02219778, ov00_02219754, 0x199);
    }
    if (key == NULL) {
        __msl_assertion_failed(ov00_022197C8, ov00_02219778, ov00_02219754, 0x19a);
    }
    if (value == NULL) {
        __msl_assertion_failed(ov00_022197D4, ov00_02219778, ov00_02219754, 0x19b);
    }
    if (buffer[*index] != '\\') {
        GP_PARSE_ERROR(connection);
    }
    start = buffer + *index;
    buffer = start + 1;
    c = *buffer++;
    i = 0;
    while (c != '\\') {
        if (c == 0) {
            GP_PARSE_ERROR(connection);
        }
        if (i == 0x1ff) {
            GP_PARSE_ERROR(connection);
        }
        *key++ = c;
        c = *buffer++;
        i++;
    }
    *key = 0;
    j = 0;
    while ((c = *buffer++) != '\\' && c != 0) {
        if (j == 0x1ff) {
            GP_PARSE_ERROR(connection);
        }
        *value++ = c;
        j++;
    }
    *value = 0;
    *index += (buffer - start) - 1;
    return 0;
}
