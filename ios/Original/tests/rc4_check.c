// SPDX-License-Identifier: GPL-3.0-or-later
#include "nitrocrypto_rc4.h"
#include <cps/cps_crypto.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)

u32 CheckOriginalRC4(void) {
    u8 key[32], input[1026], expected[1026], bytes[1026], words[1026];
    CRYPTORC4Context byteState, byteSaved;
    CRYPTORC4FastContext wordState, wordSaved;
    CpsRc4Context reference;
    const u32 lengths[]={0,1,7,8,9,15,16,17,31,32,255,256,257,1024};
    for (u32 i=0;i<sizeof(key);i++) key[i]=i+1;
    for (u32 keyIndex=0;keyIndex<17;keyIndex++) {
        u32 keyLength=keyIndex<16 ? keyIndex+1 : 32;
        for (u32 test=0;test<sizeof(lengths)/sizeof(lengths[0]);test++) {
            u32 length=lengths[test];
            memset(input,0xA5,sizeof(input));
            memset(bytes,0xA5,sizeof(bytes));
            memset(words,0xA5,sizeof(words));
            memset(&byteState,0xA5,sizeof(byteState));
            for (u32 i=0;i<length;i++) input[i+1]=(u8)(i*37+keyLength);
            memcpy(expected,input,sizeof(input));
            CPSi_rc4_init(&reference,key,keyLength>16 ? 16 : keyLength);
            CPSi_rc4_crypt(&reference,expected+1,length);
            CRYPTO_RC4Init(&byteState,key,keyLength);
            CRYPTO_RC4FastInit(&wordState,key,keyLength);
            CRYPTO_RC4Encrypt(&byteState,input+1,length,bytes+1);
            CRYPTO_RC4FastEncrypt(&wordState,input+1,length,words+1);
            CHECK(!memcmp(bytes,expected,length+2) && !memcmp(words,expected,length+2));
            CHECK(byteState.i==reference.x && byteState.j==reference.y);
            CHECK(byteState.pad[0]==0xA5 && byteState.pad[1]==0xA5);
            CHECK(wordState.i==((u32)reference.x<<24) && wordState.j==((u32)reference.y<<24));
            for (u32 i=0;i<256;i++) {
                CHECK(byteState.s[i]==reference.s[i] && wordState.s[i]==((u32)reference.s[i]<<24));
            }
            byteSaved=byteState; wordSaved=wordState;
            CRYPTO_RC4Encrypt(&byteState,NULL,0,NULL);
            CRYPTO_RC4FastEncrypt(&wordState,NULL,0,NULL);
            CHECK(!memcmp(&byteState,&byteSaved,sizeof(byteState)) && !memcmp(&wordState,&wordSaved,sizeof(wordState)));

            CRYPTO_RC4Init(&byteState,key,keyLength);
            CRYPTO_RC4FastInit(&wordState,key,keyLength);
            memset(bytes,0xA5,sizeof(bytes));
            memset(words,0xA5,sizeof(words));
            for (u32 offset=0;offset<length;) {
                u32 chunk=offset%19+1;
                if (chunk>length-offset) chunk=length-offset;
                CRYPTO_RC4Encrypt(&byteState,input+offset+1,chunk,bytes+offset+1);
                CRYPTO_RC4FastEncrypt(&wordState,input+offset+1,chunk,words+offset+1);
                offset+=chunk;
            }
            CHECK(!memcmp(bytes,expected,length+2) && !memcmp(words,expected,length+2));
            CHECK(!memcmp(&byteState,&byteSaved,sizeof(byteState)) && !memcmp(&wordState,&wordSaved,sizeof(wordState)));

            CRYPTO_RC4Init(&byteState,key,keyLength);
            CRYPTO_RC4FastInit(&wordState,key,keyLength);
            CRYPTO_RC4Encrypt(&byteState,bytes+1,length,bytes+1);
            CRYPTO_RC4FastEncrypt(&wordState,words+1,length,words+1);
            CHECK(!memcmp(bytes,input,length+2) && !memcmp(words,input,length+2));
        }
    }
    // Numerical vectors from RFC 6229, section 2: https://www.rfc-editor.org/rfc/rfc6229#section-2
    static const struct { u32 offset; u8 bytes[16]; } vectors[]={
        {0,{0xb2,0x39,0x63,0x05,0xf0,0x3d,0xc0,0x27,0xcc,0xc3,0x52,0x4a,0x0a,0x11,0x18,0xa8}},
        {16,{0x69,0x82,0x94,0x4f,0x18,0xfc,0x82,0xd5,0x89,0xc4,0x03,0xa4,0x7a,0x0d,0x09,0x19}},
        {240,{0x28,0xcb,0x11,0x32,0xc9,0x6c,0xe2,0x86,0x42,0x1d,0xca,0xad,0xb8,0xb6,0x9e,0xae}},
        {256,{0x1c,0xfc,0xf6,0x2b,0x03,0xed,0xdb,0x64,0x1d,0x77,0xdf,0xcf,0x7f,0x8d,0x8c,0x93}}
    };
    memset(input,0,sizeof(input));
    CRYPTO_RC4Init(&byteState,key,5);
    CRYPTO_RC4FastInit(&wordState,key,5);
    CRYPTO_RC4Encrypt(&byteState,input,272,bytes);
    CRYPTO_RC4FastEncrypt(&wordState,input,272,words);
    for (u32 i=0;i<sizeof(vectors)/sizeof(vectors[0]);i++) {
        CHECK(!memcmp(bytes+vectors[i].offset,vectors[i].bytes,16));
        CHECK(!memcmp(words+vectors[i].offset,vectors[i].bytes,16));
    }
    return 0;
}
