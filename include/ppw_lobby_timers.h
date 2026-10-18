#ifndef POKEHEARTGOLD_PPW_LOBBY_TIMERS_H
#define POKEHEARTGOLD_PPW_LOBBY_TIMERS_H

// The timer dispatcher supplies the stored argument; zero removes the timer.
typedef int (*PPWLobbyTimerCallback)(void *arg);

#ifdef __cplusplus
extern "C" {
#endif

int PPWLobby_TimerUploadUserProfile(void *arg);
int PPWLobby_TimerUploadSystemProfile(void *arg);
int PPWLobby_TimerSendRecruitInfo(void *arg);
int PPWLobby_TimerPollChannelMode(void *arg);
int PPWLobby_TimerRequestChannelTime(void *arg);
int PPWLobby_TimerRequestLobbyData(void *arg);

#ifdef __cplusplus
}
#endif

#endif
