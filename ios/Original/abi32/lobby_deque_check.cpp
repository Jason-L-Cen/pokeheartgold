// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../src/ov45_0223b2dc.cpp"
#include <string.h>
extern "C" u32 HGLobbyLiveAllocations(void);
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
using namespace LobbyDequeCapacity;

extern "C" u32 CheckOriginalOwnedDeque(void) {
    u32 before=HGLobbyLiveAllocations();
    {
        Deque empty;
        CHECK(empty.map.ptr==0 && empty.map.cap==0);
    }
    CHECK(HGLobbyLiveAllocations()==before);
    const u32 requests[]={0,1,127,128,129,257};
    for (u32 blocks=0;blocks<=3;blocks++) for (u32 wrapped=0;wrapped<2;wrapped++)
    for (u32 slack=0;slack<=1 && slack<=blocks;slack++) for (u32 request=0;request<6;request++) {
        {
            Deque deque;
            u32 capacity=blocks+3, first=wrapped ? capacity-1 : 0;
            deque.map.ptr=(P *)LobbyMem_Alloc(capacity*sizeof(P));
            CHECK(deque.map.ptr);
            memset(deque.map.ptr,0,capacity*sizeof(P));
            deque.map.start=first; deque.map.size=blocks+1; deque.map.cap=capacity;
            deque.start=slack*128; deque.size=(blocks-slack)*128;
            P original[3]={0};
            for (u32 i=0;i<blocks;i++) {
                original[i]=(P)LobbyMem_Alloc(0x400); CHECK(original[i]);
                for (u32 j=0;j<256;j++) original[i][j]=0xCA000000u+i*256+j;
                deque.map.ptr[(first+i)%capacity]=original[i];
            }
            u32 added=(requests[request]+127)/128, reused=added<slack ? added : slack;
            LobbyEventDeque_AddBackCapacity(&deque,requests[request]);
            CHECK(deque.start==(slack-reused)*128 && deque.size==(blocks-slack)*128);
            CHECK(deque.map.size==blocks+added-reused+1 && deque.map.size<deque.map.cap);
            CHECK(deque.map.ptr[(deque.map.start+deque.map.size-1)%deque.map.cap]==0);
            CHECK(HGLobbyLiveAllocations()==before+1+blocks+added-reused);
            for (u32 i=0;i<blocks;i++) {
                u32 occurrences=0;
                for (u32 k=0;k+1<deque.map.size;k++)
                    occurrences+=deque.map.ptr[(deque.map.start+k)%deque.map.cap]==original[i];
                CHECK(occurrences==1);
                for (u32 j=0;j<256;j++) CHECK(original[i][j]==0xCA000000u+i*256+j);
            }
            for (u32 i=0;i<blocks-slack;i++)
                CHECK(deque.map.ptr[(deque.map.start+deque.start/128+i)%deque.map.cap]==original[slack+i]);
        }
        CHECK(HGLobbyLiveAllocations()==before);
    }
    {
        Ring source;
        P values[2]={(P)0x11110000u,(P)0x22220000u};
        source.ptr=(P *)LobbyMem_Alloc(3*sizeof(P)); CHECK(source.ptr);
        source.ptr[2]=values[0]; source.ptr[0]=values[1]; source.ptr[1]=0;
        source.start=2; source.size=2; source.cap=3;
        {
            Ring copy(source);
            CHECK(copy.ptr!=source.ptr && copy.size==2);
            for (u32 i=0;i<2;i++) CHECK(copy.ptr[(copy.start+i)%copy.cap]==values[i]);
            CHECK(HGLobbyLiveAllocations()==before+2);
        }
        CHECK(HGLobbyLiveAllocations()==before+1);
    }
    CHECK(HGLobbyLiveAllocations()==before);
    return 0;
}
