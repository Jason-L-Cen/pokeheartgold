// SPDX-License-Identifier: GPL-3.0-or-later
// Include the original private dispatcher; its unrelated entry points are discarded by the probe link.
#include "../../../src/lobby/ppw_lobby_process.cpp"
#include <stddef.h>

#define TIMER_CHECK(value) do { if (!(value)) return __LINE__; } while (0)
_Static_assert(sizeof(LobbyTimerMapNode)==40 && offsetof(LobbyTimerMapNode,timer)+offsetof(LobbyTimer,param)==32 && offsetof(LobbyTimerMapNode,timer)+offsetof(LobbyTimer,callback)==36,"Original timer ABI32 layout");

struct TimerObservation { int calls,keep; void *seen; };
static int ObserveTimer(void *arg) {
    TimerObservation *observation=(TimerObservation *)arg;
    observation->calls++;
    observation->seen=arg;
    return observation->keep;
}
static LobbyTimerCallback volatile timerCallback=ObserveTimer;

struct TimerInsertResult { LobbyTimerMapNode *node; u8 inserted; u8 padding[3]; };
extern "C" void LobbyTimerMap_Insert(TimerInsertResult *,LobbyTimerMap *,u32 *);
extern "C" u32 HGLobbyLiveAllocations(void);

extern "C" u32 CheckOriginalPPWTimers(void) {
    TimerObservation observation={0,1,0};
    LobbyTimerMapNode node={0};
    node.timer.lastTime=1000; node.timer.interval=3; node.timer.param=&observation; node.timer.callback=timerCallback;
    TIMER_CHECK(Timer_Update(&node,1003)==1 && observation.calls==0 && node.timer.lastTime==1000);
    TIMER_CHECK(Timer_Update(&node,1004)==1 && observation.calls==1 && node.timer.lastTime==1004 && observation.seen==&observation);
    TIMER_CHECK(Timer_Update(&node,1006)==1 && observation.calls==1 && node.timer.lastTime==1004);
    observation.keep=0;
    TIMER_CHECK(Timer_Update(&node,1008)==0 && observation.calls==2 && node.timer.lastTime==1008);
    TIMER_CHECK(Timer_Update(&node,1006)==1 && observation.calls==2 && node.timer.lastTime==1008);
    observation.keep=1; node.timer.interval=0;
    TIMER_CHECK(Timer_Update(&node,1008)==1 && observation.calls==2);
    TIMER_CHECK(Timer_Update(&node,1009)==1 && observation.calls==3 && node.timer.lastTime==1009);
    node.timer.lastTime=(s64)1<<40; node.timer.interval=3;
    TIMER_CHECK(Timer_Update(&node,((s64)1<<40)+3)==1 && observation.calls==3);
    TIMER_CHECK(Timer_Update(&node,((s64)1<<40)+4)==1 && observation.calls==4 && node.timer.lastTime==((s64)1<<40)+4);
    u32 allocations=HGLobbyLiveAllocations();
    LobbyTimerMap map={0}; map.leftmost=(LobbyTimerMapNode *)&map.root;
    const u32 keys[]={20,10,30,15,5,25},minimum[]={20,10,10,10,5,5},ordered[]={5,10,15,20,25,30};
    LobbyTimerMapNode *inserted[6];
    for (u32 i=0;i<6;i++) {
        LobbyTimer timer={((s64)1<<40)+i,3+i,&observation,timerCallback};
        LobbyTimerMapEntry entry; LobbyTimerMap_MakeEntry(&entry,keys[i],&timer);
        TimerInsertResult result; LobbyTimerMap_Insert(&result,&map,&entry.id);
        TIMER_CHECK(result.inserted==1 && result.node && result.node->id==keys[i] && map.count==i+1 && map.leftmost->id==minimum[i]);
        TIMER_CHECK(result.node->timer.lastTime==timer.lastTime && result.node->timer.interval==timer.interval &&
                    result.node->timer.param==&observation && result.node->timer.callback==timerCallback);
        inserted[i]=result.node;
    }
    LobbyTimer changed={0,99,0,timerCallback}; LobbyTimerMapEntry duplicate;
    LobbyTimerMap_MakeEntry(&duplicate,15,&changed);
    TimerInsertResult result; LobbyTimerMap_Insert(&result,&map,&duplicate.id);
    TIMER_CHECK(!result.inserted && result.node==inserted[3] && map.count==6 && map.leftmost==inserted[4]);
    TIMER_CHECK(result.node->timer.lastTime==((s64)1<<40)+3 && result.node->timer.interval==6 && result.node->timer.param==&observation);
    TIMER_CHECK(Timer_Update(result.node,((s64)1<<40)+10)==1 && observation.calls==5 && observation.seen==&observation);
    Iterator it={map.leftmost};
    for (u32 i=0;i<6;i++) {
        TIMER_CHECK(it.current!=(void *)&map.root && ((LobbyTimerMapNode *)it.current)->id==ordered[i]);
        LobbyTreeIterator_Advance(&it);
    }
    TIMER_CHECK(it.current==(void *)&map.root);
    LobbyTimerMap_Clear(&map);
    TIMER_CHECK(!map.count && !map.root && map.leftmost==(LobbyTimerMapNode *)&map.root && HGLobbyLiveAllocations()==allocations);
    return 0;
}
