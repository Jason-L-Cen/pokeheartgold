// SPDX-License-Identifier: GPL-3.0-or-later
#include "DGT_dgt.h"
#include "dgt.h"
#include <stddef.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(struct DGTHash1Context)==88 && offsetof(struct DGTHash1Context,buffer8)==24,"MD5 context ABI");
_Static_assert(sizeof(struct DGTHash2Context)==104 && offsetof(struct DGTHash2Context,Message_Block)==32,"SHA-1 context ABI");

static int DigestMatches(const u8 *digest, const char *hex) {
    const char digits[]="0123456789abcdef";
    for (u32 i=0;hex[i];i++) {
        if (hex[i]!=digits[(digest[i/2]>>(i%2 ? 0 : 4))&15]) return 0;
    }
    return 1;
}

static u32 CheckDigests(u8 *data, u32 length, u32 chunk, const char *md5, const char *sha1) {
    struct DGTHash1Context first;
    struct DGTHash2Context second;
    u8 output[22];
    memset(&first,0xA5,sizeof(first)); memset(&second,0xA5,sizeof(second));
    DGT_Hash1Reset(&first); DGT_Hash2Reset(&second);
    DGT_Hash1SetSource(&first,NULL,0); DGT_Hash2SetSource(&second,NULL,0);
    for (u32 offset=0;offset<length;) {
        u32 size=length-offset;
        if (size>chunk) size=chunk;
        DGT_Hash1SetSource(&first,data+offset,size);
        DGT_Hash2SetSource(&second,data+offset,size);
        offset+=size;
    }
    CHECK(first.length==length && second.Length_Low==length*8 && second.Length_High==0);
    CHECK(second.Message_Block_Index==(int)(length%64));
    memset(output,0xA5,sizeof(output));
    DGT_Hash1GetDigest_R(output+1,&first);
    CHECK(DigestMatches(output+1,md5) && output[0]==0xA5 && output[17]==0xA5);
    for (u32 i=0;i<sizeof(first);i++) CHECK(((u8 *)&first)[i]==0);
    memset(output,0xA5,sizeof(output));
    DGT_Hash2GetDigest(&second,output+1);
    CHECK(DigestMatches(output+1,sha1) && output[0]==0xA5 && output[21]==0xA5);
    CHECK(second.Message_Block_Index==0 && second.Length_Low==length*8 && second.Length_High==0);
    CHECK((u32)second.Computed==0xA5A5A5A5 && (u32)second.Corrupted==0xA5A5A5A5);
    return 0;
}

u32 CheckOriginalDigests(void) {
    // RFC 1321 / RFC 3174 vectors; additional boundary values use Python hashlib as the independent oracle.
    static const struct { const char *text,*md5,*sha1; } text[]={
        {"","d41d8cd98f00b204e9800998ecf8427e","da39a3ee5e6b4b0d3255bfef95601890afd80709"},
        {"a","0cc175b9c0f1b6a831c399e269772661","86f7e437faa5a7fce15d1ddcb9eaeaea377667b8"},
        {"abc","900150983cd24fb0d6963f7d28e17f72","a9993e364706816aba3e25717850c26c9cd0d89d"},
        {"message digest","f96b697d7cb7938d525a2f31aaf161d0","c12252ceda8be8994d5fa0290a47231c1d16aae3"},
        {"abcdefghijklmnopqrstuvwxyz","c3fcd3d76192e4007dfb496cca67e13b","32d10c7b8cf96570ca04ce37f2a19d84240d3a89"},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789","d174ab98d277d9f5a5611c2c9f419d9f","761c457bf73b14d27e9e9265c46f4b4dda11f940"},
        {"12345678901234567890123456789012345678901234567890123456789012345678901234567890","57edf4a22be3c955ac49da2e2107b67a","50abf5706a150990a08b2c5ea40fa0e585554732"},
        {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq","8215ef0796a20bcaaae116d3876c664a","84983e441c3bd26ebaae4aa1f95129e5e54670f1"}
    };
    static const struct { u32 size; const char *md5,*sha1; } binary[]={
        {55,"d872aa0473a24da995ce4ac518ade767","c4622048cfef59b72875839ee7ae1cbcf55e7658"},
        {56,"e23567645846677c205de80f9779081b","ddc12942656468475970fa4fa49161f52ed138e4"},
        {63,"4775b66278a8fc132ff80923378216cd","7f8c3fa49f1297bd8b9feb964b6b419987f9f0d1"},
        {64,"71e123b70c7aa64826fcfe472694cd1c","a334b47180c61fd522f99905ec02c36f9e848211"},
        {65,"5949948f26e35203661075214faa3966","dd27d9eb923d39687e10872c3e8133ba2f0a68a1"},
        {257,"de795cc04890f2dd3e653c88a67f3e68","fd9b3aed5c02eaa88a6798a27a3b70a675539f91"}
    };
    const u32 chunks[]={1,7,31,64,512};
    for (u32 i=0;i<sizeof(text)/sizeof(text[0]);i++) for (u32 c=0;c<sizeof(chunks)/sizeof(chunks[0]);c++) {
        u32 line=CheckDigests((u8 *)text[i].text,strlen(text[i].text),chunks[c],text[i].md5,text[i].sha1);
        if (line) return line;
    }
    u32 storage[66];
    for (u32 a=0;a<4;a++) for (u32 i=0;i<sizeof(binary)/sizeof(binary[0]);i++) {
        u8 *data=(u8 *)storage+a;
        for (u32 j=0;j<binary[i].size;j++) data[j]=(u8)(j*37+11);
        for (u32 c=0;c<sizeof(chunks)/sizeof(chunks[0]);c++) {
            u32 line=CheckDigests(data,binary[i].size,chunks[c],binary[i].md5,binary[i].sha1);
            if (line) return line;
        }
        for (u32 j=0;j<binary[i].size;j++) CHECK(data[j]==(u8)(j*37+11));
    }
    // RFC 2202 HMAC-SHA1 vectors, including keys and messages larger than one block.
    static const struct { u32 keyLength; u8 keyByte; const char *message,*digest; } hmac[]={
        {20,0x0B,"Hi There","b617318655057264e28bc0b6fb378c8ef146be00"},
        {4,0,"what do ya want for nothing?","effcdf6ae5eb2fa2d27416d5f184df9c259a7c79"},
        {80,0xAA,"Test Using Larger Than Block-Size Key - Hash Key First","aa4ae5e15272d00e95705637ce8a3b55ed402112"},
        {80,0xAA,"Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data","e8e99d0f45237d786d6bbaa7965c7808bbff1a91"}
    };
    u8 key[80],output[22];
    for (u32 i=0;i<sizeof(hmac)/sizeof(hmac[0]);i++) {
        memset(key,hmac[i].keyByte,sizeof(key));
        if (!hmac[i].keyByte) memcpy(key,"Jefe",4);
        memset(output,0xA5,sizeof(output));
        DGT_Hash2CalcHmac(output+1,hmac[i].message,strlen(hmac[i].message),key,hmac[i].keyLength);
        CHECK(DigestMatches(output+1,hmac[i].digest) && output[0]==0xA5 && output[21]==0xA5);
    }
    memset(output,0xA5,sizeof(output));
    DGT_Hash2CalcHmac(output+1,key,0,key,1);
    DGT_Hash2CalcHmac(output+1,key,1,key,0);
    DGT_Hash2CalcHmac(output+1,NULL,1,key,1);
    for (u32 i=0;i<sizeof(output);i++) CHECK(output[i]==0xA5);
    struct DGTHash1Context first;
    struct DGTHash2Context second;
    memset(&first,0,sizeof(first)); DGT_Hash1Reset(&first); first.length=0xFFFFFFFFULL;
    DGT_Hash1SetSource(&first,key,1);
    CHECK(first.length==0x100000000ULL);
    DGT_Hash2Reset(&second); second.Length_Low=0xFFFFFFF8; second.Length_High=7;
    DGT_Hash2SetSource(&second,key,1);
    CHECK(second.Length_Low==0 && second.Length_High==8 && second.Message_Block_Index==1);
    return 0;
}
