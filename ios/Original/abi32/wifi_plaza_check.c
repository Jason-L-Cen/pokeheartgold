// SPDX-License-Identifier: GPL-3.0-or-later
#include "wifi_plaza_internal.h"
#include <stddef.h>
#include <string.h>

#define CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(WiFiPlazaLobbyCommandTable)==12,"Original command table layout");

// Owned record inputs only; no lobby service or matchmaking call is replaced here.
WiFiPlazaLobbyClient *gWiFiPlazaLobbyClient;
u8 *volatile gPPWLobbyWork;

u32 CheckOriginalWifiPlaza(void) {
    static WiFiPlazaLobbyClient client;
    static WiFiPlazaWork work;
    u32 ppw[20]={0};
    memset(&client,0,sizeof(client)); memset(&work,0,sizeof(work));
    gWiFiPlazaLobbyClient=&client; gPPWLobbyWork=NULL;
    CHECK(WiFiPlazaWork_GetOwnId(&work)==-1);
    const u32 ids[]={0,7,0x80000000,0xFFFFFFFF};
    for (u32 i=0;i<sizeof(ids)/sizeof(ids[0]);i++) {
        ppw[19]=ids[i]; gPPWLobbyWork=(u8 *)ppw;
        CHECK((u32)WiFiPlazaLobbyClient_GetOwnId()==ids[i] && (u32)WiFiPlazaWork_GetOwnId(&work)==ids[i]);
    }
    for (u32 i=0;i<2;i++) {
        client.unk_131=i; client.requestError=0xA5; client.addingLocation=0x5A;
        CHECK(WiFiPlazaWork_TakeLobbyFlag131(&work)==i && client.unk_131==0);
        CHECK(!WiFiPlazaWork_TakeLobbyFlag131(&work) && client.requestError==0xA5 && client.addingLocation==0x5A);
    }
    const int errors[]={1,2,4,5,7};
    CHECK(WiFiPlazaLobbyClient_CheckResult(0)==TRUE);
    for (u32 i=0;i<sizeof(errors)/sizeof(errors[0]);i++) CHECK(WiFiPlazaLobbyClient_CheckResult(errors[i])==FALSE);

    WiFiPlazaLobbyCommand lobbyCommands[2]={{0}},channelCommands[3]={{0}};
    WiFiPlazaLobbyClient_SetLobbyCommands(lobbyCommands,2,&work);
    WiFiPlazaLobbyClient_SetChannelCommands(channelCommands,3,&client);
    CHECK(client.commandTables[0].commands==lobbyCommands && client.commandTables[0].count==2 && client.commandTables[0].arg==&work);
    CHECK(client.commandTables[1].commands==channelCommands && client.commandTables[1].count==3 && client.commandTables[1].arg==&client);
    WiFiPlazaLobbyClient_ClearChannelCommands();
    const u8 zero[12]={0}; CHECK(!memcmp(&client.commandTables[1],zero,sizeof(zero)));
    CHECK(client.commandTables[0].commands==lobbyCommands && client.commandTables[0].count==2 && client.commandTables[0].arg==&work);
    s32 players[20]={0}; client.idLists[1].ids=players;
    const s16 counts[]={0,1,20};
    for (u32 i=0;i<3;i++) { client.idLists[1].count=counts[i]; CHECK(WiFiPlaza_GetChannelPlayerCount()==(u32)counts[i]); }

    WiFiPlazaTalkSession_Init(&work.talk);
    const s16 times[]={0,1,449,450,32767,-1,-32768};
    for (u32 i=0;i<sizeof(times)/sizeof(times[0]);i++) {
        work.talk.timeoutCounter=times[i]; CHECK(WiFiPlazaWork_GetTalkTimeoutCounter(&work)==(u32)(s32)times[i]);
    }
    CHECK(WiFiPlazaWork_GetGroupDuration()==1200);
    for (u32 group=0;group<9;group++) for (u32 seat=0;seat<3;seat++) work.groups.ids[group][seat]=(u32)-1;
    for (int i=0;i<27;i++) {
        CHECK(WiFiPlazaWork_JoinGroup(&work,1000+i,0)==i);
        CHECK(WiFiPlazaWork_FindGroupPlace(&work,1000+i)==i);
    }
    WiFiPlazaGroups saved=work.groups;
    CHECK(WiFiPlazaWork_JoinGroup(&work,1000,0)==-1 && WiFiPlazaWork_JoinGroup(&work,3000,0)==-1);
    CHECK(!memcmp(&saved,&work.groups,sizeof(saved)));
    WiFiPlazaWork_LeaveGroup(&work,1013);
    CHECK(WiFiPlazaWork_JoinGroup(&work,(int)0x80000000,0)==13 && WiFiPlazaWork_FindGroupPlace(&work,(int)0x80000000)==13);
    memset(&work.groups,0,sizeof(work.groups));
    for (u32 group=0;group<9;group++) for (u32 seat=0;seat<3;seat++) work.groups.ids[group][seat]=(u32)-1;
    work.groups.states[0]=2; work.groups.markerCounts[1][0]=1; work.groups.markerCounts[1][1]=1;
    CHECK(WiFiPlazaWork_JoinGroup(&work,7,0)==5 && WiFiPlazaWork_JoinGroup(&work,8,1)==3);
    CHECK(WiFiPlazaWork_JoinGroup(&work,7,1)==-1);
    gWiFiPlazaLobbyClient=NULL; gPPWLobbyWork=NULL;
    return 0;
}
