#include "global.h"

#include "dwc/http_socket_internal.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

#pragma force_active on

extern OSThread ov38_02226234;
extern OSMessage ov38_02226AF4[];
extern OSMutex ov38_02226AF8;
extern OSMessageQueue ov38_02226B10;
extern OSThread ov38_02226B30;
extern s8 ov38_02224F87[]; // last character of the default boundary ov38_02224F74

int ov38_022219FC(void *(*allocFunc)(int size, int align), void (*freeFunc)(void *ptr), int priority) {
    ov38_02226210.allocFunc = allocFunc;
    ov38_02226210.freeFunc = freeFunc;
    ov38_02226210.error = 0;
    ov38_02226210.nextJobId = 0;
    ov38_02226210.jobs = NULL;
    ov38_02226210.currentJob = NULL;
    ov38_02226210.quit = 0;
    if (!ov38_0222201C()) {
        ov38_02226210.error = 9;
        return 0;
    }
    ov38_02226210.threadStack = ov38_02226210.allocFunc(8192, 8);
    if (!ov38_02226210.threadStack) {
        ov38_02226210.error = 1;
        ov38_02222034();
        return 0;
    }
    if (ov38_02222060(priority, ov38_02226210.threadStack) == 0) {
        ov38_02226210.error = 9;
        ov38_02226210.freeFunc(ov38_02226210.threadStack);
        ov38_02222034();
        return 0;
    }
    ov38_02226210.running = 1;
    return 1;
}

void ov38_02221AC4(void (*callback)(void)) {
    OS_CreateThread(&ov38_02226234, ov38_02221B0C, callback, ov38_02226AF4, 2048, 16);
    OS_WakeupThreadDirect(&ov38_02226234);
}

void ov38_02221B0C(void *callback) {
    ov38_02222D18();
    ov38_022220D8();
    ov38_02226210.freeFunc(ov38_02226210.threadStack);
    ov38_02222034();
    ov38_02226210.running = 0;
    ((void (*)(void))callback)();
}

static inline void HttpField_Append(HttpField **list, HttpField *field) {
    if (*list) {
        field->prev = (*list)->prev;
        field->next = *list;
        (*list)->prev->next = field;
        (*list)->prev = field;
    } else {
        field->next = field;
        field->prev = field;
        *list = field;
    }
}

int ov38_02221B48(HttpField **list, const char *name, const char *value, int nodeSize) {
    HttpField *first = *list;
    int found = 0;
    HttpField *field;
    if (first) {
        field = first;
        if (ov38_02224CD4(name, first->name) != 0) {
            for (field = first->next; field != *list; field = field->next) {
                if (ov38_02224CD4(name, field->name) == 0) {
                    found = 1;
                    break;
                }
            }
        } else {
            found = 1;
        }
    }
    if (found) {
        field->value = value;
    } else {
        HttpField *newField = ov38_02226218(nodeSize, 4);
        if (!newField) {
            ov38_02226214 = 1;
            return 0;
        }
        newField->name = name;
        newField->value = value;
        newField->length = 0;
        newField->isFile = 0;
        HttpField_Append(list, newField);
    }
    return 1;
}

HttpField *ov38_02221C60(HttpField **list) {
    HttpField *field = *list;
    if (field) {
        if (field == field->prev) {
            *list = NULL;
        } else {
            field->prev->next = field->next;
            field->next->prev = field->prev;
            *list = field->next;
        }
    }
    return field;
}

// Next boundary character: 0-9, then A-Z, then a-z, then 0 again.
int ov38_02221CA4(u8 ch) {
    ch++;
    if (ch == 123) {
        ch = 48;
    } else if (ch == 91) {
        ch = 97;
    } else if (ch == 58) {
        ch = 65;
    }
    return (s8)ch;
}

// Changes the request's multipart boundary (HttpRequest.boundary, at +0x44) until the POST value
// does not contain it. Takes the request as bytes: a typed pointer changes the code.
int ov38_02221CD8(char *request, const char *value, int length) {
    int ch;
    s8 *key;
    char *position;
    int index;
    if (ov38_02224DC8(value, length, request + 0x46, 18) < 0) {
        return 1;
    }
    key = ov38_02224F87;
    for (index = 19; index >= 2; index--, key--) {
        position = request + index;
        ch = (signed char)position[0x44];
        for (;;) {
            ch = ov38_02221CA4((u8)ch);
            position[0x44] = ch;
            if (ch == *key) {
                break;
            }
            if (ov38_02224DC8(value, length, request + 0x46, 18) < 0) {
                return 1;
            }
        }
    }
    return 0;
}

int ov38_02221D78(HttpRequest *request, const char *name, const char *value) {
    if (request->queued) {
        return 0;
    }
    return ov38_02221B48(&request->headers, name, value, sizeof(HttpField));
}

int ov38_02221D9C(HttpRequest *request, const char *name, const char *value) {
    int ok = 0;
    int length;
    if (request->queued) {
        return 0;
    }
    length = ov38_0222334C(value);
    if (ov38_02221CD8((char *)request, value, length)) {
        ok = ov38_02221B48(&request->posts, name, value, sizeof(HttpField));
        if (ok) {
            request->posts->prev->length = length;
        }
    }
    return ok;
}

int ov38_02221E10(HttpRequest *request, int unk_1C, int unk_18) {
    if (request->queued) {
        return 0;
    }
    request->unk_1C = unk_1C;
    request->unk_18 = unk_18;
    return 1;
}

int ov38_02221E2C(HttpRequest *request) {
    int result = -1;
    HttpJob *job = ov38_02226218(sizeof(HttpJob), 4);
    if (job) {
        if (ov38_02226230) {
            job->prev = ov38_02226230->prev;
            job->next = ov38_02226230;
            ov38_02226230->prev->next = job;
            *(HttpJob *volatile *)&ov38_02226230->prev = job;
        } else {
            job->prev = job;
            job->next = job;
            ov38_02226230 = job;
        }
        {
            int id = ov38_0222622C;
            ov38_0222622C = (int)((u32)ov38_0222622C + 1);
            job->id = id;
        }
        job->request = request;
        job->socket = -1;
        result = job->id;
        if (ov38_0222622C < 0) {
            ov38_0222622C = 0;
        }
    }
    return result;
}

HttpJob *ov38_02221EE0(int id) {
    HttpJob *result = NULL;
    HttpJob *const head = ov38_02226230;
    if (head) {
        if (head->id == id) {
            result = head;
        } else {
            HttpJob *job = head->next;
            while (job != head) {
                if (job->id == id) {
                    result = job;
                    break;
                }
                job = job->next;
            }
        }
    }
    return result;
}

int ov38_02221F38(int id) {
    int result = 0;
    HttpJob *job = ov38_02221EE0(id);
    if (job) {
        if (ov38_02226230 != ov38_02226230->prev) {
            job->prev->next = job->next;
            job->next->prev = job->prev;
            if (ov38_02226230 == job) {
                ov38_02226230 = job->next;
            }
        } else {
            ov38_02226230 = NULL;
        }
        {
            HttpRequest *request = job->request;
            HttpSocketCallback callback = request->callback;
            HttpResponse *response = request->response;
            void *param = request->param;
            ov38_02222B98(request);
            ov38_0222621C(job);
            callback(8, response, param);
        }
        result = 1;
    }
    return result;
}

void ov38_02221FD8(void) {
    while (ov38_02226230) {
        ov38_02221F38(ov38_02226230->id);
    }
}

HttpJob *ov38_02222008(void) {
    return (HttpJob *)ov38_02221C60((HttpField **)&ov38_02226230);
}

int ov38_0222201C(void) {
    OS_InitMutex(&ov38_02226AF8);
    return 1;
}

void ov38_02222034(void) {
}

void ov38_02222038(void) {
    OS_LockMutex(&ov38_02226AF8);
}

void ov38_0222204C(void) {
    OS_UnlockMutex(&ov38_02226AF8);
}

int ov38_02222060(int priority, u8 *stack) {
    if (!OS_IsThreadAvailable()) {
        return 0;
    }
    OS_InitMessageQueue(&ov38_02226B10, ov38_02226AF4, 1);
    OS_CreateThread(&ov38_02226B30, ov38_022236A0, NULL, stack + 8192, 8192, priority);
    OS_WakeupThreadDirect(&ov38_02226B30);
    return 1;
}

void ov38_022220D8(void) {
    ov38_02226224 = 1;
    ov38_0222211C();
    OS_JoinThread(&ov38_02226B30);
}

void ov38_02222100(void) {
    OSMessage message;
    OS_ReceiveMessage(&ov38_02226B10, &message, 1);
}

int ov38_0222211C(void) {
    return OS_SendMessage(&ov38_02226B10, 0, 0);
}

// Header byte pos lives in bytes[] for the first 1024 bytes, then in 512-byte chunks.
void ov38_02222138(HttpResponse *response, int pos, HttpResponseChunk **chunk, int *offset) {
    int count;
    HttpResponseChunk *cur;
    if (pos < 1024) {
        *chunk = NULL;
        *offset = pos;
        return;
    }
    count = (pos - 1024) >> 9;
    cur = response->chunks;
    while (count--) {
        cur = cur->next;
    }
    *chunk = cur;
    *offset = (pos - 1024) & 511;
}

int ov38_02222194(HttpResponse *response, HttpResponseChunk **chunk, int *offset) {
    int i;
    if (!*chunk) {
        if (*offset < 1024) {
            i = (*offset)++;
            return response->bytes[i];
        }
        *offset = 0;
        *chunk = response->chunks;
    } else if (*offset == 512) {
        *offset = 0;
        *chunk = (*chunk)->next;
    }
    i = (*offset)++;
    return (*chunk)->bytes[i];
}

// Returns the start of the header line after begin (0 at the end of the header, -1 if none) and
// the position of the line's first ':' in *colon.
int ov38_02222210(HttpResponse *response, int begin, int end, int *colon) {
    HttpResponseChunk *chunk;
    int offset, cr = 0;
    if (colon) {
        *colon = -1;
    }
    if (begin < end) {
        ov38_02222138(response, begin, &chunk, &offset);
        for (; begin < end; begin++) {
            int c = ov38_02222194(response, &chunk, &offset);
            if (c == ':' && colon && *colon < 0) {
                *colon = begin;
            }
            if (!cr) {
                if (c == '\r') {
                    cr = 1;
                }
            } else {
                if (c == '\n') {
                    if (begin == end - 1) {
                        return 0;
                    }
                    return begin + 1;
                }
                cr = 0;
            }
        }
    }
    return -1;
}

int ov38_022222E0(HttpResponse *response, int begin, int end) {
    HttpResponseChunk *chunk;
    int offset;
    if (begin < end) {
        ov38_02222138(response, begin, &chunk, &offset);
        for (; begin < end; begin++) {
            if (ov38_02222194(response, &chunk, &offset) != ' ') {
                return begin;
            }
        }
    }
    return -1;
}

static inline BOOL HttpChar_EqualsFolded(int c, int ch) {
    int upper = 0;
    int rhs;
    if (ch >= 'A' && ch <= 'Z') {
        upper = 1;
    }
    rhs = upper ? ch + 32 : ch;
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    return c == rhs;
}

// Case-insensitive match of str (up to its end, a space or stop) at the header position start.
int ov38_02222350(HttpResponse *response, int begin, int end, const char *str, int stop) {
    HttpResponseChunk *chunk;
    int offset, c;
    s8 ch;
    if (begin < end) {
        ov38_02222138(response, begin, &chunk, &offset);
        c = ov38_02222194(response, &chunk, &offset);
        end--;
        while (HttpChar_EqualsFolded(c, ch = *str)) {
            if (ch == 0 || ch == ' ' || ch == stop || begin == end) {
                return 0;
            }
            c = ov38_02222194(response, &chunk, &offset);
            begin++;
            str++;
        }
    }
    return -1;
}

// Copies size bytes of the header from offset.
int ov38_02222420(HttpResponse *response, void *out, int offset, int size) {
    int n, index;
    HttpResponseChunk *chunk;
    u8 *dst = out;
    if (offset + size <= response->headerLength) {
        if (size) {
            if (offset < 1024) {
                n = 1024 - offset;
                if (size <= n) {
                    n = size;
                }
                ov38_0222336C(dst, response->bytes + offset, n);
                offset += n;
                size -= n;
                dst += n;
            }
            if (size) {
                offset -= 1024;
                index = offset >> 9;
                offset &= 511;
                int blocks = index;
                index--;
                chunk = response->chunks;
                if (blocks) {
                    do {
                        chunk = chunk->next;
                    } while (index-- != 0);
                }
                while (size) {
                    n = 512 - offset;
                    if (size <= n) {
                        n = size;
                    }
                    ov38_0222336C(dst, chunk->bytes + offset, n);
                    offset = (offset + n) & 511;
                    size -= n;
                    dst += n;
                    chunk = chunk->next;
                }
            }
        }
        return 1;
    }
    return 0;
}

int ov38_0222250C(HttpResponse *response, u32 length) {
    return (u32)response->bodyCapacity <= length;
}

int ov38_02222520(HttpRequest *request, int socket, int offset, int flags) {
    HttpResponse *response = request->response;
    return ov38_02223224(request, socket, response->body + offset, response->bodyCapacity - offset, flags);
}

int ov38_02222548(HttpRequest *request, int socket, int offset, int size, int flags) {
    HttpResponse *response = request->response;
    int left;
    if (ov38_0222250C(response, offset)) {
        return -1003;
    }
    left = response->bodyCapacity - offset;
    if (size <= left) {
        left = size;
    }
    return ov38_02223224(request, socket, response->body + offset, left, flags);
}
