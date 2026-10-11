#pragma force_active on
#include "global.h"

// The code below is compiled as ARM (32-bit) code.
#include <nitro/code32.h>

typedef void *(*Ov38AllocFunc)(u32 size, int align);
typedef void (*Ov38FreeFunc)(void *ptr);

typedef struct Ov38Work {
    void (*shutdownCallback)(void); // 0x00
    int unk_04;                     // 0x04
    char *unk_08;                   // 0x08
    Ov38AllocFunc allocFunc;        // 0x0C
    char *unk_10;                   // 0x10
    int active;                     // 0x14
    int unk_18;                     // 0x18
    int error;                      // 0x1C
    Ov38FreeFunc freeFunc;          // 0x20
    int unk_24;                     // 0x24
    char *strings[3];               // 0x28, base64-encoded copies, see ov38_0221FFC0
} Ov38Work;

extern Ov38Work ov38_02226050;
// fields of ov38_02226050 that the code addresses through their own labels
extern Ov38AllocFunc ov38_0222605C; // .allocFunc
extern int ov38_0222606C;           // .error
extern Ov38FreeFunc ov38_02226070;  // .freeFunc
extern int ov38_02226074;           // .unk_24
extern char *ov38_02226078[3];      // .strings

// "00:0000000-00"
typedef struct Ov38Text13 {
    char chars[13];
} Ov38Text13;

typedef struct Ov38Info {
    int unk_00;
    int unk_04;
} Ov38Info;

Ov38Info ov38_022260FC;

extern const char ov38_02224E78[]; // base64 alphabet ending in ".-", padding '*'
extern Ov38Text13 ov38_02224EBC;
extern char ov38_02226104[];
extern char ov38_02226084[];
extern char ov38_02226090[];
extern char ov38_022260A4[];
extern char ov38_022260BC[];
extern char ov38_022260D8[];

void ov38_02220810(void *);
int ov38_02221718(void);
void ov38_02221730(void);
void ov38_02221734(void);
void ov38_02221748(void);
int ov38_0222175C(void);
void ov38_02221784(void);
int ov38_02221788(void);
void ov38_022217D0(void);
void ov38_02221810(void *);
int ov38_02221870(const char *str);
void ov38_0222189C(char *dst, const char *src, int n);
int ov38_022219FC(Ov38AllocFunc allocFunc, Ov38FreeFunc freeFunc, int count);
void ov38_02221AC4(void (*func)(void));
int ov38_02222C9C(int);
int ov00_021EC9E0(Ov38Info *info);
void ov38_022218C0(char *buf, int value);

void ov38_0221FF7C(void);
int ov38_0221FFC0(int i, const char *str);
int ov38_02220090(const char *a, const char *b, const char *c);
int ov38_02220100(char *out, const char *in, int size);
int ov38_02220148(unsigned ch);
int ov38_022201A0(int encodedLen);
int ov38_022201AC(int len);
int ov38_022201C8(char *out, const char *in, int len);
int ov38_022202C0(char *out, const char *in, int len);
char *ov38_022203E8(const char *str);
int ov38_02220434(Ov38AllocFunc allocFunc, Ov38FreeFunc freeFunc, const char *a, const char *b, const char *small, const char *big, int value);
void ov38_022205DC(void);
void ov38_02220620(void (*callback)(void));
int ov38_02220648(void);
int ov38_02220684(char *out);

void ov38_0221FF7C(void) {
    int i;
    for (i = 0; i < 3; i++) {
        if (ov38_02226078[i]) {
            ov38_02226070(ov38_02226078[i]);
            ov38_02226078[i] = NULL;
        }
    }
}

int ov38_0221FFC0(int i, const char *str) {
    int len;
    int encodedLen;
    if (*str) {
        if (ov38_02221870(str) > 10) {
            ov38_0222606C = 7;
            return 0;
        }
        len = ov38_02221870(str);
        if (len > 10) {
            ov38_0222606C = 7;
            return 0;
        }
        ov38_02226078[i] = ov38_0222605C(ov38_022201AC(len) + 1, 4);
        if (!ov38_02226078[i]) {
            ov38_0222606C = 1;
            return 0;
        }
        encodedLen = ov38_022201C8(ov38_02226078[i], str, len);
        ov38_02226078[i][encodedLen] = '\0';
    }
    return 1;
}

int ov38_02220090(const char *a, const char *b, const char *c) {
    ov38_02221734();
    ov38_0221FF7C();
    if (ov38_0221FFC0(0, a) && ov38_0221FFC0(1, b) && ov38_0221FFC0(2, c)) {
        ov38_02221748();
        return 1;
    }
    ov38_0221FF7C();
    ov38_02221748();
    return 0;
}

// Copies `size` bytes, padding with zeros after the end of `in`; returns the characters copied.
int ov38_02220100(char *out, const char *in, int size) {
    u32 i;
    int count = 0;
    i = count;
    for (; (s32)i < size; i++) {
        if (*in != 0) {
            char c = *in;
            count++;
            in++;
            *out++ = c;
        } else {
            *out++ = 0;
        }
    }
    return count;
}

// Value of one character of the ov38_02224E78 alphabet, -1 if it is not part of it.
int ov38_02220148(unsigned ch) {
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    }
    if (ch >= 'a' && ch <= 'z') {
        return ch - 71;
    }
    if (ch >= '0' && ch <= '9') {
        return ch + 4;
    }
    if (ch == '.') {
        return 62;
    }
    if (ch == '-') {
        return 63;
    }
    return -1;
}

int ov38_022201A0(int encodedLen) {
    return (encodedLen >> 2) * 3;
}

int ov38_022201AC(int len) {
    return ((len + 2) / 3) << 2;
}

// Base64 with the ov38_02224E78 alphabet.
int ov38_022201C8(char *out, const char *in, int len) {
    u8 buf[3];
    int n;
    u8 i0;
    u8 i1;
    u8 i2;
    u8 i3;
    int total = 0;
    n = ov38_02220100((char *)buf, in, len > 3 ? 3 : len);
    while (n > 0) {
        if (len < 3) {
            buf[2] = 0;
        }
        if (len < 2) {
            buf[1] = 0;
        }
        i0 = buf[0] >> 2;
        i1 = ((buf[0] << 4) | (buf[1] >> 4)) & 0x3F;
        i2 = ((buf[1] << 2) | (buf[2] >> 6)) & 0x3F;
        i3 = buf[2] & 0x3F;
        out[0] = ov38_02224E78[i0];
        out[1] = ov38_02224E78[i1];
        out[2] = n > 1 ? ov38_02224E78[i2] : '*';
        out[3] = n > 2 ? ov38_02224E78[i3] : '*';
        in += n;
        out += 4;
        total += 4;
        len -= n;
        if (len == 0) {
            break;
        }
        n = ov38_02220100((char *)buf, in, len > 3 ? 3 : len);
    }
    *out = '\0';
    return total;
}

int ov38_022202C0(char *out, const char *in, int len) {
    struct {
        s8 decoded[3];
        char encoded[4];
    } buf;

    int value;
    int n;
    int count;
    count = 0;
    n = ov38_02220100(buf.encoded, in, 4);
    value = count;
    while (n > 0 && value >= 0) {
        buf.decoded[0] = 0;
        buf.decoded[1] = 0;
        buf.decoded[2] = 0;
        value = ov38_02220148((u8)buf.encoded[0]);
        if (value >= 0) {
            buf.decoded[0] = value << 2;
            value = ov38_02220148((u8)buf.encoded[1]);
            if (value >= 0) {
                buf.decoded[0] |= value >> 4;
                buf.decoded[1] = value << 4;
                value = ov38_02220148((u8)buf.encoded[2]);
                if (value >= 0) {
                    buf.decoded[1] |= value >> 2;
                    buf.decoded[2] = value << 6;
                    value = ov38_02220148((u8)buf.encoded[3]);
                    if (value >= 0) {
                        buf.decoded[2] |= value;
                    }
                }
            }
        }
        {
            int limit = n - 1;
            int i = 0;
            if (limit > 0) {
                s8 *p = buf.decoded;
                do {
                    *out++ = *p++;
                    i++;
                } while (i < limit);
            }
        }
        in += n;
        count += n - 1;
        len -= 4;
        if (len <= 0) {
            break;
        }
        n = ov38_02220100(buf.encoded, in, 4);
    }
    return count;
}

char *ov38_022203E8(const char *str) {
    int len = ov38_02221870(str);
    char *copy = ov38_02226050.allocFunc(len + 1, 4);
    if (copy) {
        ov38_0222189C(copy, str, len);
        copy[len] = '\0';
    }
    return copy;
}

int ov38_02220434(Ov38AllocFunc allocFunc, Ov38FreeFunc freeFunc, const char *a, const char *b, const char *small, const char *big, int value) {
    ov38_02226050.allocFunc = allocFunc;
    ov38_02226050.freeFunc = freeFunc;
    ov38_02226050.unk_18 = value;
    ov38_02226050.error = 0;
    ov38_02226050.strings[0] = NULL;
    ov38_02226050.strings[1] = NULL;
    ov38_02226050.strings[2] = NULL;
    ov38_02226050.unk_04 = 0;
    ov38_02226050.unk_24 = -1;
    ov38_02226050.unk_08 = NULL;
    ov38_02226050.unk_10 = NULL;
    ov38_02221810(ov38_02226090);
    ov38_02220810(ov38_022260D8);
    if (!(ov38_02226050.unk_10 = ov38_022203E8(a))) {
        goto failed;
    }
    if (!(ov38_02226050.unk_08 = ov38_022203E8(b))) {
        goto failed;
    }
    ov38_02226084[ov38_022201C8(ov38_02226084, small, 4)] = '\0';
    ov38_022260BC[ov38_022201C8(ov38_022260BC, big, 16)] = '\0';
    if (ov38_0222175C() && ov38_02221718()) {
        if (ov38_022219FC(allocFunc, freeFunc, 10)) {
            ov38_02226050.unk_04 = ov38_02220684(ov38_022260A4);
            ov38_02226050.active = 1;
            return 1;
        }
        ov38_02221730();
    }
    ov38_02226050.freeFunc(ov38_02226050.unk_08);
    ov38_02226050.freeFunc(ov38_02226050.unk_10);
    ov38_02226050.error = 8;
    return 0;
failed:
    if (ov38_02226050.unk_08) {
        ov38_02226050.freeFunc(ov38_02226050.unk_08);
    }
    if (ov38_02226050.unk_10) {
        ov38_02226050.freeFunc(ov38_02226050.unk_10);
    }
    ov38_02226050.error = 1;
    return 0;
}

void ov38_022205DC(void) {
    ov38_0221FF7C();
    ov38_02221730();
    ov38_02221784();
    ov38_02226050.freeFunc(ov38_02226050.unk_08);
    ov38_02226050.freeFunc(ov38_02226050.unk_10);
    ov38_02226050.shutdownCallback();
}

void ov38_02220620(void (*callback)(void)) {
    ov38_02226050.active = 0;
    ov38_02226050.shutdownCallback = callback;
    ov38_02221AC4(ov38_022205DC);
}

int ov38_02220648(void) {
    int x;
    if (!ov38_02221788()) {
        ov38_02221734();
        x = ov38_02226074;
        ov38_02221748();
        return ov38_02222C9C(x);
    }
    ov38_022217D0();
    return 0;
}

int ov38_02220684(char *out) {
    Ov38Text13 text = ov38_02224EBC;
    if (ov00_021EC9E0(&ov38_022260FC)) {
        if (ov38_022260FC.unk_00 >= 0 && ov38_022260FC.unk_00 < 10) {
            text.chars[1] = ov38_022260FC.unk_00 + '0';
        } else if (ov38_022260FC.unk_00 >= 10 && ov38_022260FC.unk_00 < 100) {
            ov38_022218C0(text.chars, ov38_022260FC.unk_00);
        }
        if (ov38_022260FC.unk_00 == 4 || ov38_022260FC.unk_00 == 8) {
            if (ov38_022260FC.unk_04 >= 0 && ov38_022260FC.unk_04 < 10) {
                text.chars[3] = ov38_022260FC.unk_04 + '0';
            }
            ov38_0222189C(text.chars + 4, ov38_02226104, 9);
        }
        ov38_022201C8(out, text.chars, 13);
        return 1;
    }
    return 0;
}
