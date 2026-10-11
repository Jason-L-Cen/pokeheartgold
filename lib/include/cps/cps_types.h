#ifndef POKEHEARTGOLD_CPS_TYPES_H
#define POKEHEARTGOLD_CPS_TYPES_H

#include <nitro.h>

// libCPS types that the NitroWiFi socket layer (SOCL) shares with the TCP/IP stack.

// Buffer of a CPS socket (receive, send, ...), set up by SOCLi_InitSocketBuffer.
typedef struct CpsSocketBuffer {
    s32 size;
    u8 *data;
} CpsSocketBuffer;

// Socket of the TCP/IP stack, registered with CPS_SocRegister. A SOCL socket starts with one.
typedef struct CpsSocket {
    u8 unk_00[8];
    u8 state; // 0x08, 4 = connection established
    u8 unk_09;
    u16 localPort; // 0x0A
    void *ssl;     // 0x0C
    u8 unk_10[8];
    u16 remotePort;    // 0x18
    u16 udpRemotePort; // 0x1A, sender of the datagram being received
    u32 remoteIp;      // 0x1C
    u32 udpRemoteIp;   // 0x20, sender of the datagram being received
    u8 unk_24[8];
    u16 unk_2C;
    u16 unk_2E;
    u8 unk_30[0xC];
    CpsSocketBuffer recvBuf; // 0x3C
    s32 recvLength;          // 0x44, bytes received into recvBuf
    CpsSocketBuffer sendBuf; // 0x48
    CpsSocketBuffer unk_50;
    CpsSocketBuffer unk_58;
    u8 unk_60[4];
} CpsSocket;

// Start-up parameters of the stack (CPS_Startup).
typedef struct CpsConfig {
    u32 staticIp;                  // 0x00, nonzero: fixed address, no DHCP
    void *(*alloc)(u32 size);      // 0x04
    void (*free)(void *ptr);       // 0x08
    void (*addressCallback)(void); // 0x0C, the address is known (or DHCP gave up)
    int (*linkIsOn)(void);         // 0x10
    u32 unk_14;
    u32 unk_18;
    void *work;        // 0x1C
    u32 workSize;      // 0x20
    int mss;           // 0x24, MTU - 40
    u32 dhcpRequestIp; // 0x28
    u32 unk_2C;
} CpsConfig;

#endif // POKEHEARTGOLD_CPS_TYPES_H
