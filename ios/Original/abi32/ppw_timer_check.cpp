// SPDX-License-Identifier: GPL-3.0-or-later
// Include the original private dispatcher; its unrelated entry points are discarded by the probe link.
#include "../../../src/ppw_lobby_process.cpp"
#include <stddef.h>

#define TIMER_CHECK(value) do { if (!(value)) return __LINE__; } while (0)
static_assert(sizeof(Timer)==40 && offsetof(Timer,arg)==32 && offsetof(Timer,fn)==36,"Original timer ABI32 layout");

struct TimerObservation { int calls,keep; void *seen; };
static int ObserveTimer(void *arg) {
    TimerObservation *observation=(TimerObservation *)arg;
    observation->calls++;
    observation->seen=arg;
    return observation->keep;
}
static PPWLobbyTimerCallback volatile timerCallback=ObserveTimer;

extern "C" u32 CheckOriginalPPWTimers(void) {
    TimerObservation observation={0,1,0};
    Timer timer={0};
    timer.last=1000; timer.interval=3; timer.arg=&observation; timer.fn=timerCallback;
    TIMER_CHECK(Timer_Update(&timer,1003)==1 && observation.calls==0 && timer.last==1000);
    TIMER_CHECK(Timer_Update(&timer,1004)==1 && observation.calls==1 && timer.last==1004 && observation.seen==&observation);
    TIMER_CHECK(Timer_Update(&timer,1006)==1 && observation.calls==1 && timer.last==1004);
    observation.keep=0;
    TIMER_CHECK(Timer_Update(&timer,1008)==0 && observation.calls==2 && timer.last==1008);
    TIMER_CHECK(Timer_Update(&timer,1006)==1 && observation.calls==2 && timer.last==1008);
    observation.keep=1; timer.interval=0;
    TIMER_CHECK(Timer_Update(&timer,1008)==1 && observation.calls==2);
    TIMER_CHECK(Timer_Update(&timer,1009)==1 && observation.calls==3 && timer.last==1009);
    timer.last=(s64)1<<40; timer.interval=3;
    TIMER_CHECK(Timer_Update(&timer,((s64)1<<40)+3)==1 && observation.calls==3);
    TIMER_CHECK(Timer_Update(&timer,((s64)1<<40)+4)==1 && observation.calls==4 && timer.last==((s64)1<<40)+4);
    return 0;
}
