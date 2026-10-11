#pragma force_active on
#pragma warn_unusedvar off
#include "global.h"

#include "WIFI_stubsmd5_dummy.h"
#include "dgt.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

// OVY_0 platform layer of the GameSpy-style SDK: dynamic arrays ("darray.c"), hash tables ("hashtable.c")
// and "nonport.c" (MD5 strings, memory, ticks, sockets).

typedef void (*ArrayElementFreeFn)(void *);
typedef int (*ArrayCompareFn)(const void *, const void *);

typedef struct DArray {
    int count;
    int capacity;
    int elemSize;
    int growBy;
    ArrayElementFreeFn freeFn;
    void *list;
} DArray;

extern void __msl_assertion_failed(const char *cond, const char *file, const char *func, int line);
extern void *ov00_021EC3A8(u32 size);
extern void *ov00_021EC3BC(void *p, u32 size);
extern void ov00_021EC3D8(void *p);
extern void *memcpy(void *, const void *, u32);
extern void *memmove(void *, const void *, u32);
extern void qsort(void *, u32, u32, ArrayCompareFn);

extern char ov00_02217C58[]; // "ArrayNth"
extern char ov00_02217C64[]; // "ArrayNew"
extern char ov00_02217C70[]; // "ArrayFree"
extern char ov00_02217C7C[]; // "ArrayGrow"
extern char ov00_02217C88[]; // "ArrayAppend"
extern char ov00_02217C94[]; // "ArrayRemoveAt"
extern char ov00_02217CA4[]; // "ArrayDeleteAt"
extern char ov00_02217CB4[]; // "ArrayInsertAt"
extern char ov00_02217CC4[]; // "ArrayReplaceAt"
extern char ov00_02217CE8[]; // "ArrayInsertSorted"
extern char ov00_02217D10[]; // "array->list"
extern char ov00_02217D1C[]; // "darray.c"
extern char ov00_02217D28[]; // "array"
extern char ov00_02217D30[]; // "elemSize"
extern char ov00_02217D3C[]; // "(n >= 0) && (n < array->count)"
extern char ov00_02217D5C[]; // "(n >= 0) && (n <= array->count)"
extern char ov00_02217D7C[]; // "comparator"

typedef void (*ArrayMapFn)(void *, void *);
typedef int (*ArrayMapFn2)(void *, void *);
typedef int (*TableHashFn)(const void *, int);

typedef struct HashTable {
    DArray **buckets;
    int nbuckets;
    ArrayElementFreeFn freeFn;
    TableHashFn hashFn;
    ArrayCompareFn cmpFn;
} HashTable;

typedef struct SocPollFd {
    int fd;
    s16 events;
    s16 revents;
} SocPollFd;

typedef struct DGTHash1Context Md5Ctx;

extern char ov00_02217C4C[]; // "ArrayMap"
extern char ov00_02217CD4[]; // "ArrayMapBackwards"
extern char ov00_02217CFC[]; // "ArrayMapBackwards2"
extern char ov00_02217D88[]; // "fn"
extern char ov00_02217D8C[]; // "TableMap"
extern char ov00_02217D98[]; // "TableNew2"
extern char ov00_02217DA4[]; // "TableFree"
extern char ov00_02217DB0[]; // "TableEnter"
extern char ov00_02217DBC[]; // "TableCount"
extern char ov00_02217DC8[]; // "TableRemove"
extern char ov00_02217DD4[]; // "TableLookup"
extern char ov00_02217DE0[]; // "TableMapSafe"
extern char ov00_02217DF0[]; // "TableMapSafe2"
extern char ov00_02217E00[]; // "hashFn"
extern char ov00_02217E08[]; // "hashtable.c"
extern char ov00_02217E14[]; // "compFn"
extern char ov00_02217E1C[]; // "elemSize"
extern char ov00_02217E28[]; // "nBuckets"
extern char ov00_02217E34[]; // "table"
extern char ov00_02217E3C[]; // "table->buckets"
extern char ov00_02217E4C[]; // "fn"
extern char ov00_02217E50[]; // "%02x"
extern char ov00_02217E5C[]; // "time"
extern char ov00_02217E64[]; // "current_time"
extern char ov00_02217E74[]; // "OS_IsTickAvailable() == TRUE"
extern char ov00_02217E94[]; // "nonport.c"
extern unsigned char __lower_mapC[];

extern int sprintf(char *, const char *, ...);
extern int SOC_Fcntl(int, int, int);
extern int SOC_Poll(SocPollFd *, int, s64);

HashTable *ov00_021FED4C(int elemSize, int nbuckets, int nchains, TableHashFn hashFn, ArrayCompareFn cmpFn, ArrayElementFreeFn freeFn);
void ov00_021FE678(DArray *a);
int ArrayLength(const DArray *a);
void ov00_021FE754(DArray *a, const void *elem);
void ov00_021FE968(DArray *a, int n);
void ov00_021FE9C0(DArray *a, const void *elem, int n);
int ov00_021FEA40(DArray *a, const void *key, ArrayCompareFn cmp, int from, int sorted);
void ov00_021FEAF8(DArray *a, ArrayMapFn fn, void *arg);
void ov00_021FEB64(DArray *a, ArrayMapFn fn, void *arg);
void *ov00_021FEBC4(DArray *a, ArrayMapFn2 fn, void *arg);
int ov00_021FF59C(int sock, int *readFlag, int *writeFlag, int *exceptFlag);
int ov00_021FF7EC(int ret, int err);
int ov00_021FF9DC(int sock, int level, int opt, void *val, int *len);
int ov00_021FFA04(int sock, int level, int opt, const void *val, int len);

static inline int ov00_tolower(int c) {
    if (c < 0 || c >= 0x80) {
        return c;
    }
    return __lower_mapC[c];
}

typedef struct SocSockAddr {
    u8 len;
    u8 family;
    u8 port[2];
    u8 addr[4];
} SocSockAddr;

typedef struct SockAddrIn {
    u8 len;
    u8 family;
    u16 port;
    u32 addr;
} SockAddrIn;

typedef struct Ov00SocketState {
    int unk0;
    int lastError;
    char *hName;
    char **hAliases;
    s16 hAddrType;
    s16 hLength;
    char **hAddrList;
    u32 addr;
    u8 pad1C[0x2C - 0x1C];
    char *addrList[2];
} Ov00SocketState;

extern char ov00_02217EA0[]; // "localhost"
extern char ov00_0221B1B4[];
extern char ov00_0221B1C4[];
extern char *ov00_0221B1D8[];
extern u32 SOCL_GetHostID(void);
extern void SOC_U32to4U8(u32, void *);

extern Ov00SocketState ov00_0221B1AC;
extern u32 ov00_02217E58;

extern int SOC_Socket(int, int, int);
extern int SOC_Close(int);
extern int SOC_Shutdown(int, int);
extern int SOC_Bind(int, SocSockAddr *);
extern int SOC_Connect(int, SocSockAddr *);
extern int SOC_Listen(int, int);
extern int SOC_Accept(int, SocSockAddr *);
extern int SOC_Recv(int, void *, int, int);
extern int SOC_RecvFrom(int, void *, int, int, SocSockAddr *);
extern int SOC_Send(int, const void *, int, int);
extern int SOC_SendTo(int, const void *, int, int, SocSockAddr *);
extern int SOC_GetSockName(int, SocSockAddr *);
extern int SOC_InetAtoN(const char *, u32 *);
u32 ov00_021FFAD4(u32 x);
u32 ov00_021FFB20(void);

extern void *memset(void *, int, u32);
extern int strcmp(const char *, const char *);

void *ov00_021FE6EC(DArray *a, int n);
void ov00_021FE7A0(DArray *a, const void *elem, int n);
void ov00_021FE8D8(DArray *a, int n);
void *ov00_021FECC0(const void *key, const void *base, int count, int size, ArrayCompareFn cmp, int *found);
void *ov00_021FEC68(const void *key, const void *base, int count, int size, ArrayCompareFn cmp);

void ov00_021FE514(DArray *a, int n);
void ov00_021FE538(DArray *a);
void ov00_021FE590(DArray *a, const void *elem, int n);
DArray *ov00_021FE5B4(int elemSize, int growBy, ArrayElementFreeFn freeFn);
void ov00_021FE858(DArray *a, const void *elem, ArrayCompareFn cmp);
void ov00_021FEA20(DArray *a, ArrayCompareFn cmp);
void ov00_021FEC3C(DArray *a);
HashTable *ov00_021FED24(int elemSize, int nbuckets, TableHashFn hashFn, ArrayCompareFn cmpFn, ArrayElementFreeFn freeFn);
void ov00_021FEE8C(HashTable *t);
int ov00_021FEF00(HashTable *t);
void ov00_021FEF74(HashTable *t, const void *elem);
int ov00_021FF01C(HashTable *t, const void *elem);
void *ov00_021FF0C0(HashTable *t, const void *elem);
void ov00_021FF160(HashTable *t, ArrayMapFn fn, void *arg);
void ov00_021FF1F8(HashTable *t, ArrayMapFn fn, void *arg);
void *ov00_021FF264(HashTable *t, ArrayMapFn2 fn, void *arg);
void ov00_021FF2DC(u8 *digest, char *out);
void ov00_021FF31C(const void *data, u32 len, char *out);
u32 ov00_021FF368(void);
void ov00_021FF3C0(u32 ms);
void ov00_021FF3CC(void);
void ov00_021FF3D0(void);
char *ov00_021FF3D4(const char *s);
char *ov00_021FF408(char *s);
int ov00_021FF44C(int sock, int blocking);
int ov00_021FF48C(int sock, int size);
int ov00_021FF4D0(int sock, int size);
int ov00_021FF510(int sock);
int ov00_021FF558(int sock);
int ov00_021FF694(int sock);
int ov00_021FF6BC(int sock);
void *ov00_021FF6E4(void);
int ov00_021FF770(const u32 *in);
int ov00_021FF804(int domain, int type, int proto);
int ov00_021FF818(int sock);
int ov00_021FF82C(int sock, int how);
int ov00_021FF840(int sock, const SocSockAddr *addr, int len);
int ov00_021FF8A0(int sock, const SocSockAddr *addr, int len);
int ov00_021FF8EC(int sock, int backlog);
int ov00_021FF900(int sock, SocSockAddr *addr, int *len);
int ov00_021FF92C(int sock, void *buf, int len, int flags);
int ov00_021FF940(int sock, void *buf, int len, int flags, SocSockAddr *addr, int *addrlen);
int ov00_021FF970(int sock, const void *buf, int len, int flags);
int ov00_021FF984(int sock, const void *buf, int len, int flags, const SocSockAddr *addr, int addrlen);

void ov00_021FE514(DArray *a, int n) {
    if (a->freeFn != NULL) {
        a->freeFn(ov00_021FE6EC(a, n));
    }
}

void ov00_021FE538(DArray *a) {
    a->capacity += a->growBy;
    a->list = ov00_021EC3BC(a->list, a->capacity * a->elemSize);
    if (!a->list) {
        __msl_assertion_failed(ov00_02217D10, ov00_02217D1C, ov00_02217C7C, 0x41);
    }
}

void ov00_021FE590(DArray *a, const void *elem, int n) {
    memcpy(ov00_021FE6EC(a, n), elem, a->elemSize);
}

DArray *ov00_021FE5B4(int elemSize, int growBy, ArrayElementFreeFn freeFn) {
    DArray *a = ov00_021EC3A8(sizeof(DArray));
    if (!a) {
        __msl_assertion_failed(ov00_02217D28, ov00_02217D1C, ov00_02217C64, 0x52);
    }
    if (!elemSize) {
        __msl_assertion_failed(ov00_02217D30, ov00_02217D1C, ov00_02217C64, 0x53);
    }
    if (growBy == 0) {
        growBy = 8;
    }
    a->count = 0;
    a->capacity = growBy;
    a->elemSize = elemSize;
    a->growBy = growBy;
    a->freeFn = freeFn;
    if (a->capacity != 0) {
        a->list = ov00_021EC3A8(a->capacity * a->elemSize);
        if (!a->list) {
            __msl_assertion_failed(ov00_02217D10, ov00_02217D1C, ov00_02217C64, 0x5e);
        }
    } else {
        a->list = NULL;
    }
    return a;
}

void ov00_021FE678(DArray *a) {
    int i;
    if (!a) {
        __msl_assertion_failed(ov00_02217D28, ov00_02217D1C, ov00_02217C70, 0x69);
    }
    for (i = 0; i < a->count; i++) {
        ov00_021FE514(a, i);
    }
    ov00_021EC3D8(a->list);
    ov00_021EC3D8(a);
}

int ArrayLength(const DArray *a) {
    return a->count;
}

void *ov00_021FE6EC(DArray *a, int n) {
    if (!(n >= 0 && n < a->count)) {
        __msl_assertion_failed(ov00_02217D3C, ov00_02217D1C, ov00_02217C58, 0x94);
    }
    if (n < 0 || n >= a->count) {
        return NULL;
    }
    return (char *)a->list + a->elemSize * n;
}

void ov00_021FE754(DArray *a, const void *elem) {
    if (!a) {
        __msl_assertion_failed(ov00_02217D28, ov00_02217D1C, ov00_02217C88, 0xa0);
    }
    if (a) {
        ov00_021FE7A0(a, elem, a->count);
    }
}

void ov00_021FE7A0(DArray *a, const void *elem, int n) {
    int last;
    if (!(n >= 0 && n <= a->count)) {
        __msl_assertion_failed(ov00_02217D5C, ov00_02217D1C, ov00_02217CB4, 0xa7);
    }
    if (a->count == a->capacity) {
        ov00_021FE538(a);
    }
    a->count++;
    if (n < a->count - 1) {
        memmove(ov00_021FE6EC(a, n + 1), ov00_021FE6EC(a, n), (a->count - 1 - n) * a->elemSize);
    }
    ov00_021FE590(a, elem, n);
}

void ov00_021FE858(DArray *a, const void *elem, ArrayCompareFn cmp) {
    int found;
    void *p;
    if (!cmp) {
        __msl_assertion_failed(ov00_02217D7C, ov00_02217D1C, ov00_02217CE8, 0xb7);
    }
    p = ov00_021FECC0(elem, a->list, a->count, a->elemSize, cmp, &found);
    ov00_021FE7A0(a, elem, ((char *)p - (char *)a->list) / a->elemSize);
}

void ov00_021FE8D8(DArray *a, int n) {
    if (!(n >= 0 && n < a->count)) {
        __msl_assertion_failed(ov00_02217D3C, ov00_02217D1C, ov00_02217C94, 0xc0);
    }
    if (n < a->count - 1) {
        memmove(ov00_021FE6EC(a, n), ov00_021FE6EC(a, n + 1), (a->count - 1 - n) * a->elemSize);
    }
    a->count--;
}

void ov00_021FE968(DArray *a, int n) {
    if (!(n >= 0 && n < a->count)) {
        __msl_assertion_failed(ov00_02217D3C, ov00_02217D1C, ov00_02217CA4, 0xca);
    }
    ov00_021FE514(a, n);
    ov00_021FE8D8(a, n);
}

void ov00_021FE9C0(DArray *a, const void *elem, int n) {
    if (!(n >= 0 && n < a->count)) {
        __msl_assertion_failed(ov00_02217D3C, ov00_02217D1C, ov00_02217CC4, 0xd3);
    }
    ov00_021FE514(a, n);
    ov00_021FE590(a, elem, n);
}

void ov00_021FEA20(DArray *a, ArrayCompareFn cmp) {
    qsort(a->list, a->count, a->elemSize, cmp);
}

int ov00_021FEA40(DArray *a, const void *key, ArrayCompareFn cmp, int from, int sorted) {
    int count;
    int found = 1;
    void *p;
    if (!a || !(count = a->count)) {
        return -1;
    }
    if (sorted) {
        p = ov00_021FECC0(key, ov00_021FE6EC(a, from), count - from, a->elemSize, cmp, &found);
    } else {
        p = ov00_021FEC68(key, ov00_021FE6EC(a, from), count - from, a->elemSize, cmp);
    }
    if (!p || !found) {
        return -1;
    }
    return ((char *)p - (char *)a->list) / a->elemSize;
}

void ov00_021FEAF8(DArray *a, ArrayMapFn fn, void *arg) {
    int i;
    if (!fn) {
        __msl_assertion_failed(ov00_02217D88, ov00_02217D1C, ov00_02217C4C, 0xf9);
    }
    for (i = 0; i < a->count; i++) {
        fn(ov00_021FE6EC(a, i), arg);
    }
}

void ov00_021FEB64(DArray *a, ArrayMapFn fn, void *arg) {
    int i;
    if (!fn) {
        __msl_assertion_failed(ov00_02217D88, ov00_02217D1C, ov00_02217CD4, 0x104);
    }
    for (i = a->count - 1; i >= 0; i--) {
        fn(ov00_021FE6EC(a, i), arg);
    }
}

void *ov00_021FEBC4(DArray *a, ArrayMapFn2 fn, void *arg) {
    int i;
    void *e;
    if (!fn) {
        __msl_assertion_failed(ov00_02217D88, ov00_02217D1C, ov00_02217CFC, 0x121);
    }
    for (i = a->count - 1; i >= 0; i--) {
        e = ov00_021FE6EC(a, i);
        if (!fn(e, arg)) {
            return e;
        }
    }
    return NULL;
}

void ov00_021FEC3C(DArray *a) {
    int i;
    for (i = ArrayLength(a) - 1; i >= 0; i--) {
        ov00_021FE968(a, i);
    }
}

void *ov00_021FEC68(const void *key, const void *base, int count, int size, ArrayCompareFn cmp) {
    int i;
    for (i = 0; i < count; i++) {
        if (cmp(key, (char *)base + i * size) == 0) {
            return (char *)base + size * i;
        }
    }
    return NULL;
}

void *ov00_021FECC0(const void *key, const void *base, int count, int size, ArrayCompareFn cmp, int *found) {
    int lo = 0;
    int hi;
    int mid;
    int c;
    *found = 0;
    hi = count - 1;
    while (lo <= hi) {
        mid = (lo + hi) >> 1;
        c = cmp((char *)base + mid * size, key);
        if (c == 0) {
            *found = 1;
        }
        if (c < 0) {
            lo = mid + 1;
        } else {
            hi = mid - 1;
        }
    }
    return (char *)base + lo * size;
}

HashTable *ov00_021FED24(int elemSize, int nbuckets, TableHashFn hashFn, ArrayCompareFn cmpFn, ArrayElementFreeFn freeFn) {
    return ov00_021FED4C(elemSize, nbuckets, 4, hashFn, cmpFn, freeFn);
}

HashTable *ov00_021FED4C(int elemSize, int nbuckets, int nchains, TableHashFn hashFn, ArrayCompareFn cmpFn, ArrayElementFreeFn freeFn) {
    HashTable *t;
    int i;
    if (!hashFn) {
        __msl_assertion_failed(ov00_02217E00, ov00_02217E08, ov00_02217D98, 0x38);
    }
    if (!cmpFn) {
        __msl_assertion_failed(ov00_02217E14, ov00_02217E08, ov00_02217D98, 0x39);
    }
    if (!elemSize) {
        __msl_assertion_failed(ov00_02217E1C, ov00_02217E08, ov00_02217D98, 0x3a);
    }
    if (!nbuckets) {
        __msl_assertion_failed(ov00_02217E28, ov00_02217E08, ov00_02217D98, 0x3b);
    }
    t = ov00_021EC3A8(sizeof(HashTable));
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217D98, 0x3e);
    }
    t->buckets = ov00_021EC3A8(nbuckets * sizeof(DArray *));
    if (!t->buckets) {
        __msl_assertion_failed(ov00_02217E3C, ov00_02217E08, ov00_02217D98, 0x41);
    }
    for (i = 0; i < nbuckets; i++) {
        t->buckets[i] = ov00_021FE5B4(elemSize, nchains, freeFn);
    }
    t->nbuckets = nbuckets;
    t->freeFn = freeFn;
    t->cmpFn = cmpFn;
    t->hashFn = hashFn;
    return t;
}

void ov00_021FEE8C(HashTable *t) {
    int i;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217DA4, 0x51);
    }
    if (!t) {
        return;
    }
    for (i = 0; i < t->nbuckets; i++) {
        ov00_021FE678(t->buckets[i]);
    }
    ov00_021EC3D8(t->buckets);
    ov00_021EC3D8(t);
}

int ov00_021FEF00(HashTable *t) {
    int i;
    int count = 0;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217DBC, 0x61);
    }
    if (!t) {
        return 0;
    }
    for (i = 0; i < t->nbuckets; i++) {
        count += ArrayLength(t->buckets[i]);
    }
    return count;
}

void ov00_021FEF74(HashTable *t, const void *elem) {
    int hash;
    int idx;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217DB0, 0x71);
    }
    if (!t) {
        return;
    }
    hash = t->hashFn(elem, t->nbuckets);
    idx = ov00_021FEA40(t->buckets[hash], elem, t->cmpFn, 0, 0);
    if (idx == -1) {
        ov00_021FE754(t->buckets[hash], elem);
    } else {
        ov00_021FE9C0(t->buckets[hash], elem, idx);
    }
}

int ov00_021FF01C(HashTable *t, const void *elem) {
    int hash;
    int idx;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217DC8, 0x82);
    }
    if (!t) {
        return 0;
    }
    hash = t->hashFn(elem, t->nbuckets);
    idx = ov00_021FEA40(t->buckets[hash], elem, t->cmpFn, 0, 0);
    if (idx == -1) {
        return 0;
    }
    ov00_021FE968(t->buckets[hash], idx);
    return 1;
}

void *ov00_021FF0C0(HashTable *t, const void *elem) {
    int hash;
    int idx;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217DD4, 0x94);
    }
    if (!t) {
        return NULL;
    }
    hash = t->hashFn(elem, t->nbuckets);
    idx = ov00_021FEA40(t->buckets[hash], elem, t->cmpFn, 0, 0);
    if (idx == -1) {
        return NULL;
    }
    return ov00_021FE6EC(t->buckets[hash], idx);
}

void ov00_021FF160(HashTable *t, ArrayMapFn fn, void *arg) {
    int i;
    if (!t) {
        __msl_assertion_failed(ov00_02217E34, ov00_02217E08, ov00_02217D8C, 0xa7);
    }
    if (!fn) {
        __msl_assertion_failed(ov00_02217E4C, ov00_02217E08, ov00_02217D8C, 0xa8);
    }
    if (!t || !fn) {
        return;
    }
    for (i = 0; i < t->nbuckets; i++) {
        ov00_021FEAF8(t->buckets[i], fn, arg);
    }
}

void ov00_021FF1F8(HashTable *t, ArrayMapFn fn, void *arg) {
    int i;
    if (!fn) {
        __msl_assertion_failed(ov00_02217E4C, ov00_02217E08, ov00_02217DE0, 0xb6);
    }
    for (i = 0; i < t->nbuckets; i++) {
        ov00_021FEB64(t->buckets[i], fn, arg);
    }
}

void *ov00_021FF264(HashTable *t, ArrayMapFn2 fn, void *arg) {
    int i;
    void *e;
    if (!fn) {
        __msl_assertion_failed(ov00_02217E4C, ov00_02217E08, ov00_02217DF0, 0xd3);
    }
    for (i = 0; i < t->nbuckets; i++) {
        e = ov00_021FEBC4(t->buckets[i], fn, arg);
        if (e) {
            return e;
        }
    }
    return NULL;
}

void ov00_021FF2DC(u8 *digest, char *out) {
    unsigned int i;
    for (i = 0; i < 16; i++) {
        sprintf(out + 2 * i, ov00_02217E50, digest[i]);
    }
}

void ov00_021FF31C(const void *data, u32 len, char *out) {
    u8 digest[16];
    Md5Ctx ctx;
    MD5Init(&ctx);
    MD5Update(&ctx, (u8 *)data, len);
    MD5Final(digest, &ctx);
    ov00_021FF2DC(digest, out);
}

u32 ov00_021FF368(void) {
    if (OS_IsTickAvailable() != TRUE) {
        __msl_assertion_failed(ov00_02217E74, ov00_02217E94, ov00_02217E64, 0x109);
    }
    return (u32)((OS_GetTick() * 64) / 33514);
}

void ov00_021FF3C0(u32 ms) {
    OS_Sleep(ms);
}

void ov00_021FF3CC(void) {
}

void ov00_021FF3D0(void) {
}

char *ov00_021FF3D4(const char *s) {
    char *p;
    if (!s) {
        return NULL;
    }
    p = ov00_021EC3A8(strlen(s) + 1);
    if (p) {
        strcpy(p, s);
    }
    return p;
}

char *ov00_021FF408(char *s) {
    char *p = s;
    while (*s) {
        *s = ov00_tolower(*s);
        s++;
    }
    return p;
}

int ov00_021FF44C(int sock, int blocking) {
    int flags = SOC_Fcntl(sock, 3, 0);
    if (blocking) {
        flags &= ~4;
    } else {
        flags |= 4;
    }
    return SOC_Fcntl(sock, 4, flags) == 0;
}

int ov00_021FF48C(int sock, int size) {
    return ov00_021FFA04(sock, 0xFFFF, 0x1002, &size, sizeof(size)) != -1;
}

int ov00_021FF4D0(int sock, int size) {
    return ov00_021FFA04(sock, 0xFFFF, 0x1001, &size, sizeof(size)) != -1;
}

int ov00_021FF510(int sock) {
    int size;
    int len;
    len = sizeof(size);
    if (ov00_021FF9DC(sock, 0xFFFF, 0x1002, &size, &len) == -1) {
        return -1;
    }
    return size;
}

int ov00_021FF558(int sock) {
    int size;
    int len;
    len = sizeof(size);
    if (ov00_021FF9DC(sock, 0xFFFF, 0x1001, &size, &len) == -1) {
        return -1;
    }
    return size;
}

int ov00_021FF59C(int sock, int *readFlag, int *writeFlag, int *exceptFlag) {
    SocPollFd pfd;
    int rval;
    pfd.fd = sock;
    pfd.events = 0;
    if (readFlag) {
        pfd.events |= 1;
    }
    if (writeFlag) {
        pfd.events |= 8;
    }
    pfd.revents = 0;
    rval = SOC_Poll(&pfd, 1, 0);
    if (rval < 0) {
        return -1;
    }
    if (readFlag) {
        if (rval > 0 && (pfd.revents & 0x41)) {
            *readFlag = 1;
        } else {
            *readFlag = 0;
        }
    }
    if (writeFlag) {
        if (rval > 0 && (pfd.revents & 8)) {
            *writeFlag = 1;
        } else {
            *writeFlag = 0;
        }
    }
    if (exceptFlag) {
        if (rval > 0 && (pfd.revents & 0x20)) {
            *exceptFlag = 1;
        } else {
            *exceptFlag = 0;
        }
    }
    return rval;
}

int ov00_021FF694(int sock) {
    int ready = 0;
    if (ov00_021FF59C(sock, &ready, NULL, NULL) == 1) {
        return ready;
    }
    return 0;
}

int ov00_021FF6BC(int sock) {
    int ready = 0;
    if (ov00_021FF59C(sock, NULL, &ready, NULL) == 1) {
        return ready;
    }
    return 0;
}

void *ov00_021FF6E4(void) {
    ov00_0221B1AC.hName = ov00_02217EA0;
    ov00_0221B1AC.hAliases = (char **)(ov00_0221B1B4 - 8);
    ov00_0221B1AC.hAddrType = 2;
    ov00_0221B1AC.hLength = 0;
    ov00_0221B1AC.hAddrList = ov00_0221B1D8;
    ov00_0221B1AC.addr = 0;
    SOC_U32to4U8(SOCL_GetHostID(), ov00_0221B1C4);
    if (ov00_0221B1AC.addr == 0) {
        return NULL;
    }
    ov00_0221B1AC.addrList[0] = ov00_0221B1C4;
    ov00_0221B1AC.hLength = 4;
    ov00_0221B1AC.addrList[1] = NULL;
    return ov00_0221B1B4;
}

int ov00_021FF770(const u32 *in) {
    u32 x = *in;
    u32 ip = ((x >> 24) & 0xff) | ((x >> 8) & 0xff00) | ((x << 8) & 0xff0000) | ((x << 24) & 0xff000000);
    int b1 = (ip >> 24) & 0xff;
    int b2 = (ip >> 16) & 0xff;
    if (b1 == 10) {
        return 1;
    }
    if (b1 == 172 && b2 >= 16 && b2 <= 31) {
        return 1;
    }
    return b1 == 192 && b2 == 168;
}

int ov00_021FF7EC(int ret, int err) {
    if (ret < 0) {
        ov00_0221B1AC.lastError = ret;
        ret = err;
    }
    return ret;
}

int ov00_021FF804(int domain, int type, int proto) {
    return ov00_021FF7EC(SOC_Socket(domain, type, proto), -1);
}

int ov00_021FF818(int sock) {
    return ov00_021FF7EC(SOC_Close(sock), -1);
}

int ov00_021FF82C(int sock, int how) {
    return ov00_021FF7EC(SOC_Shutdown(sock, how), -1);
}

int ov00_021FF840(int sock, const SocSockAddr *addr, int len) {
    SocSockAddr local;
    if (((const SockAddrIn *)addr)->port == 0) {
        return 0;
    }
    local = *addr;
    local.len = len;
    return ov00_021FF7EC(SOC_Bind(sock, &local), -1);
}

int ov00_021FF8A0(int sock, const SocSockAddr *addr, int len) {
    SocSockAddr local;
    local = *addr;
    local.len = len;
    return ov00_021FF7EC(SOC_Connect(sock, &local), -1);
}

int ov00_021FF8EC(int sock, int backlog) {
    return ov00_021FF7EC(SOC_Listen(sock, backlog), -1);
}

int ov00_021FF900(int sock, SocSockAddr *addr, int *len) {
    int ret;
    addr->len = *len;
    ret = SOC_Accept(sock, addr);
    *len = addr->len;
    return ov00_021FF7EC(ret, -1);
}

int ov00_021FF92C(int sock, void *buf, int len, int flags) {
    return ov00_021FF7EC(SOC_Recv(sock, buf, len, flags), -1);
}

int ov00_021FF940(int sock, void *buf, int len, int flags, SocSockAddr *addr, int *addrlen) {
    int ret;
    addr->len = *addrlen;
    ret = SOC_RecvFrom(sock, buf, len, flags, addr);
    *addrlen = addr->len;
    return ov00_021FF7EC(ret, -1);
}

int ov00_021FF970(int sock, const void *buf, int len, int flags) {
    return ov00_021FF7EC(SOC_Send(sock, buf, len, flags), -1);
}

int ov00_021FF984(int sock, const void *buf, int len, int flags, const SocSockAddr *addr, int addrlen) {
    SocSockAddr local;
    memcpy(&local, addr, 8);
    local.len = addrlen;
    return ov00_021FF7EC(SOC_SendTo(sock, buf, len, flags, &local), -1);
}
