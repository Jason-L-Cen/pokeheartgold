#ifndef POKEHEARTGOLD_WIRELESS_MANAGER_H
#define POKEHEARTGOLD_WIRELESS_MANAGER_H

#include "global.h"

typedef void (*WirelessReceiveFunc)(int aid, const u8 *data, int length);

u16 Wireless_GetConnectedBitmap(void);
BOOL Wireless_ScanAndConnect(int mode, const u16 *bssid, u16 channel);
BOOL Wireless_StartChild(int mode, const void *parentInfo);
BOOL Wireless_SetEntry(BOOL enabled);
void Wireless_SetReceiveCallback(WirelessReceiveFunc callback, int port);

#endif // POKEHEARTGOLD_WIRELESS_MANAGER_H
