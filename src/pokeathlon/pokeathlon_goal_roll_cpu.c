#include "global.h"

#include "pokeathlon/pokeathlon_course_internal.h"
#include "pokeathlon/pokeathlon_course_objects.h"
#include "pokeathlon/pokeathlon_goal_roll.h"

#include "assert.h"
#include "bg_window.h"
#include "heap.h"
#include "math_util.h"
#include "sprite_resource_header.h"
#include "sprite_system.h"

// Per goal (indexed by participant - 1 or by 3 - quadrant): three guard points (x, y) in front of it.
extern const u16 gPokeathlonGoalRollCpuGuardPoints[][3][2];
extern const u16 gPokeathlonGoalRollCpuGuardPointsY[][3][2];
extern const u16 gPokeathlonGoalRollCpuGuardPoints1[][3][2];
extern const u16 gPokeathlonGoalRollCpuGuardPoints2[][3][2];

// The roles of a CPU team's Pokemon, in the order they move (PokeathlonGoalRollCpuTeam.order).
enum PokeathlonGoalRollCpuRole {
    GOAL_ROLL_CPU_ROLE_GUARD,
    GOAL_ROLL_CPU_ROLE_CHASER,
    GOAL_ROLL_CPU_ROLE_FLEX,
};

// What a CPU Pokemon decides to do about a ball. The Flex decisions repeat the Chaser and Guard ones they share
// the handling with.
enum PokeathlonGoalRollCpuDecision {
    GOAL_ROLL_CPU_CHASER_NO_BALL,
    GOAL_ROLL_CPU_CHASER_AT_BALL,
    GOAL_ROLL_CPU_CHASER_FAR_BALL,
    GOAL_ROLL_CPU_CHASER_NEAR_BALL,
    GOAL_ROLL_CPU_CHASER_BEHIND_BALL,
    GOAL_ROLL_CPU_GUARD_NO_BALL,
    GOAL_ROLL_CPU_GUARD_RETURN,
    GOAL_ROLL_CPU_GUARD_CHASE,
    GOAL_ROLL_CPU_FLEX_NO_BALL,
    GOAL_ROLL_CPU_FLEX_AT_BALL,
    GOAL_ROLL_CPU_FLEX_SPREAD,
    GOAL_ROLL_CPU_FLEX_RETURN,
    GOAL_ROLL_CPU_FLEX_CHASE,
    GOAL_ROLL_CPU_FLEX_BEHIND_BALL,
};

static void PokeathlonGoalRollCpu_ChooseRoles(PokeathlonGoalRollCpuTeam *team);
static int PokeathlonGoalRollCpu_ApplyChaserDecision(PokeathlonGoalRollMon *mon, const PokeathlonGoalRollBall *ball, u8 decision, int quadrant);
static int PokeathlonGoalRollCpu_ApplyGuardDecision(PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *ball, int decision, int participant);
static int PokeathlonGoalRollCpu_ApplyFlexDecision(PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *ball, int decision, u8 participant, PokeathlonGoalRollCpuTeam *teams);
static int PokeathlonGoalRollCpu_MoveToEmptierQuadrant(PokeathlonGoalRollMon *mon, PokeathlonGoalRollCpuTeam *teams, u8 excluded);
static void PokeathlonGoalRollCpu_Think(PokeathlonCourseData *course, PokeathlonGoalRollCpu *cpu, PokeathlonGoalRollCpuSlot *slot);
static int PokeathlonGoalRollCpu_GetQuadrant(int x, int y, int pointX, int pointY);
static BOOL PokeathlonGoalRollCpu_IsMonInTargetQuadrantOfBall(int x1, int y1, int x2, int y2, int quadrantInput, int participantInput);
static BOOL PokeathlonGoalRollCpu_IsMonNotOppositeOfBall(int x1, int y1, int x2, int y2, int quadrantInput, int participantInput);
static BOOL PokeathlonGoalRollCpu_CirclesTouch(const VecFx32 *a, fx32 radiusA, const VecFx32 *b, fx32 radiusB);

// The timer on the top screen: seconds left; "Second Wind!" from the last 900 frames on.
void PokeathlonGoalRollDisplay_UpdateTimer(PokeathlonGoalRollDisplay *display, s32 timer) {
    u8 seconds;

    if (timer <= 900) {
        Sprite_SetDrawFlag(display->timerDigits[0], FALSE);
        Sprite_SetDrawFlag(display->timerDigits[1], FALSE);
        PokeathlonCourseSpritePoolSprite_Show(display->timerLabelSprite, TRUE, FALSE);
        if (display->secondWindShown == FALSE) {
            PokeathlonGoalRollDisplay_PrintSecondWind(display);
            display->secondWindShown = TRUE;
        }
    } else {
        seconds = (u8)(timer / 30);
        Sprite_SetAnimCtrlSeq(display->timerDigits[0], (u8)(seconds / 10) + 1);
        Sprite_SetAnimCtrlSeq(display->timerDigits[1], (u8)(seconds % 10) + 1);
    }
}

// The own score in big digits (cut from the digit screen: 5 digits per row, 4 x 8 tiles each).
void PokeathlonGoalRollDisplay_DrawScore(PokeathlonGoalRollDisplay *display, s32 score) {
    u8 srcX[2];
    u8 srcY[2];
    u8 digit;
    u8 i;

    digit = (u8)(score / 10);
    srcX[0] = (digit % 5) * 4;
    srcY[0] = (digit / 5) * 8;
    digit = (u8)(score % 10);
    srcX[1] = (digit % 5) * 4;
    srcY[1] = (digit / 5) * 8;

    for (i = 0; i < 2; i++) {
        CopyRectToBgTilemapRect(display->bgConfig, GF_BG_LYR_SUB_2, (u8)(i * 4 + 6), 10, 4, 8, display->digitScreen->rawData, srcX[i], srcY[i], 20, 16);
    }
    ScheduleBgTilemapBufferTransfer(display->bgConfig, GF_BG_LYR_SUB_2);
}

PokeathlonGoalRollCpu *PokeathlonGoalRollCpu_New(enum HeapID heapId, u8 cpuCount) {
    PokeathlonGoalRollCpu *cpu = Heap_Alloc(heapId, sizeof(PokeathlonGoalRollCpu));

    MI_CpuFill8(cpu, 0, sizeof(PokeathlonGoalRollCpu));
    cpu->cpuCount = cpuCount;
    cpu->humanCount = 4 - cpuCount;
    cpu->firstCpu = cpu->humanCount;
    cpu->interval = 3;
    cpu->timer = cpu->interval;
    return cpu;
}

void PokeathlonGoalRollCpu_SetMon(PokeathlonGoalRollCpuTeam *teams, s32 index, PokeathlonGoalRollMon *mon) {
    teams[(u8)(index / 3)].mons[(u8)(index % 3)] = mon;
}

void PokeathlonGoalRollCpu_ChooseAllRoles(PokeathlonGoalRollCpuTeam *teams) {
    u8 i;

    for (i = 0; i < 4; i++) {
        PokeathlonGoalRollCpu_ChooseRoles(&teams[i]);
    }
}

// The member best at kicking and with most power moves first; the others follow by kick strength.
static void PokeathlonGoalRollCpu_ChooseRoles(PokeathlonGoalRollCpuTeam *team) {
    u8 first[3];
    u8 second[3];
    u8 selected;
    u8 i;
    u8 a;
    u8 b;

    for (i = 0; i < 3; i++) {
        first[i] = team->mons[i]->kickRow + team->mons[i]->maxPower;
        second[i] = team->mons[i]->kickRow + team->mons[i]->kickLevel;
    }

    selected = 0;
    for (i = 1; i < 3; i++) {
        if (first[i - 1] < first[i]) {
            selected = i;
        }
    }

    if (selected == 0) {
        if (second[1] >= second[2]) {
            a = 1;
            b = 2;
        } else {
            a = 2;
            b = 1;
        }
    } else if (selected == 1) {
        if (second[0] >= second[2]) {
            a = 0;
            b = 2;
        } else {
            a = 2;
            b = 0;
        }
    } else {
        if (second[0] >= second[1]) {
            a = 0;
            b = 1;
        } else {
            a = 1;
            b = 0;
        }
    }

    team->order[0].mon = team->mons[selected];
    team->order[1].mon = team->mons[a];
    team->order[2].mon = team->mons[b];
}

void PokeathlonGoalRollCpu_SetBalls(PokeathlonGoalRollCpu *cpu, PokeathlonGoalRollBall *balls) {
    cpu->balls = balls;
}

void PokeathlonGoalRollCpu_Free(PokeathlonGoalRollCpu *cpu) {
    Heap_Free(cpu);
}

// Every `interval` frames the CPU participants each move one Pokemon, in their order.
void PokeathlonGoalRollCpu_Update(PokeathlonCourseData *course, PokeathlonGoalRollCpu *cpu) {
    PokeathlonGoalRollCpuSlot *slot;

    if (cpu->humanCount == 4) {
        return;
    }
    cpu->timer++;
    if (cpu->timer >= cpu->interval) {
        cpu->run = TRUE;
        cpu->timer = 0;
    }
    if (cpu->run == FALSE) {
        return;
    }
    slot = &cpu->teams[cpu->firstCpu + cpu->current].order[cpu->orderSlot];
    if (slot->wait == 0) {
        PokeathlonGoalRollCpu_Think(course, cpu, slot);
    } else {
        slot->wait--;
    }
    cpu->current++;
    if (cpu->current >= cpu->cpuCount) {
        cpu->current = 0;
        cpu->orderSlot = (cpu->orderSlot + 1) % 3;
        cpu->run = FALSE;
    }
}

static int PokeathlonGoalRollCpu_ThinkChaser(PokeathlonCourseData *course, u8 participant, u8 member, PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *balls) {
    VecFx32 *position;
    int radius, quadrant;

    u8 decisions[2];
    u8 i;
    int result;

    if (mon->mode) {
        return 0;
    }
    if (mon->motion) {
        return 0;
    }
    quadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE);
    i = 0;
    position = &mon->position;
    for (; i < 2; i++) {
        PokeathlonGoalRollBall *ball = &balls[i];
        int ballQuadrant;
        if (!ball->state && ball->kind) {
            ballQuadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, ball->position.x / FX32_ONE, ball->position.y / FX32_ONE);
        } else {
            ballQuadrant = 4;
        }
        if (ballQuadrant != 4) {
            radius = 0;
            if (ball->kind == GOAL_ROLL_BALL_SMALL) {
                radius = 8;
            } else if (ball->kind == GOAL_ROLL_BALL_BIG) {
                radius = 12;
            } else {
                GF_ASSERT(FALSE);
            }
            if (PokeathlonGoalRollCpu_CirclesTouch(position, (u32)PokeathlonCourseObject_GetCollisionRadius(mon->object) << FX32_SHIFT, &ball->position, radius << FX32_SHIFT)) {
                decisions[i] = GOAL_ROLL_CPU_CHASER_AT_BALL;
            } else if (quadrant == ballQuadrant) {
                if (PokeathlonGoalRollCpu_IsMonInTargetQuadrantOfBall(ball->position.x / FX32_ONE, ball->position.y / FX32_ONE, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE, quadrant, participant)) {
                    decisions[i] = GOAL_ROLL_CPU_CHASER_BEHIND_BALL;
                } else {
                    decisions[i] = GOAL_ROLL_CPU_CHASER_NEAR_BALL;
                }
            } else {
                decisions[i] = GOAL_ROLL_CPU_CHASER_FAR_BALL;
            }
        } else {
            decisions[i] = GOAL_ROLL_CPU_CHASER_NO_BALL;
        }
    }
    if (decisions[0] >= decisions[1]) {
        result = PokeathlonGoalRollCpu_ApplyChaserDecision(mon, balls, decisions[0], quadrant);
    } else {
        balls++;
        result = PokeathlonGoalRollCpu_ApplyChaserDecision(mon, balls, decisions[1], quadrant);
    }
    if (mon->mode == GOAL_ROLL_MODE_DASH) {
        PokeathlonCourse_AddMonCounter(course, participant, member, POKEATHLON_COUNTER_DASHES, 1);
    }
    return result;
}

static int PokeathlonGoalRollCpu_ApplyChaserDecision(PokeathlonGoalRollMon *mon, const PokeathlonGoalRollBall *ball, u8 decision, int quadrant) {
    u8 random = (s32)LCRandom() % 100;

    switch (decision) {
    case GOAL_ROLL_CPU_CHASER_FAR_BALL:
        if (random < 60) {
            return 0;
        }
        if (random < 80) {
            mon->target = ball->position;
            return 10;
        }
        mon->target = ball->position;
        mon->mode = GOAL_ROLL_MODE_DASH;
        return 10;
    case GOAL_ROLL_CPU_CHASER_NEAR_BALL:
        if (random < 20) {
            return 0;
        }
        if (random < 90) {
            mon->target = ball->position;
            return 3;
        }
        mon->target = ball->position;
        mon->mode = GOAL_ROLL_MODE_DASH;
        return 3;
    case GOAL_ROLL_CPU_CHASER_BEHIND_BALL:
    case GOAL_ROLL_CPU_FLEX_BEHIND_BALL:
        mon->target = ball->position;
        mon->mode = GOAL_ROLL_MODE_DASH;
        break;
    case GOAL_ROLL_CPU_CHASER_AT_BALL:
    case GOAL_ROLL_CPU_FLEX_AT_BALL:
        if (quadrant != 4) {
            u8 goal = 3 - quadrant;
            mon->target.x = gPokeathlonGoalRollCpuGuardPoints[goal][0][0] << FX32_SHIFT;
            mon->target.y = gPokeathlonGoalRollCpuGuardPointsY[goal][0][0] << FX32_SHIFT;
        } else {
            GF_ASSERT(FALSE);
        }
        break;
    case GOAL_ROLL_CPU_CHASER_NO_BALL:
    case GOAL_ROLL_CPU_FLEX_NO_BALL:
        break;
    default:
        GF_ASSERT(FALSE);
        break;
    }
    return 0;
}

static int PokeathlonGoalRollCpu_ThinkGuard(PokeathlonCourseData *course, u8 participant, u8 member, PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *balls) {
    int quadrant, result;
    u32 ballQuadrant;
    u8 i, decisions[2];
    VecFx32 delta;

    if (mon->mode) {
        return 0;
    }
    if (mon->motion) {
        return 0;
    }
    quadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE);
    for (i = 0; i < 2; i++) {
        PokeathlonGoalRollBall *ball = &balls[i];
        if (!ball->state && ball->kind) {
            ballQuadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, ball->position.x / FX32_ONE, ball->position.y / FX32_ONE);
        } else {
            ballQuadrant = 4;
        }
        if (ballQuadrant != 4) {
            if (participant == ballQuadrant) {
                if (PokeathlonGoalRollCpu_IsMonNotOppositeOfBall(ball->position.x / FX32_ONE, ball->position.y / FX32_ONE, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE, quadrant, participant)) {
                    decisions[i] = GOAL_ROLL_CPU_GUARD_CHASE;
                } else {
                    decisions[i] = GOAL_ROLL_CPU_GUARD_RETURN;
                }
            } else {
                decisions[i] = GOAL_ROLL_CPU_GUARD_RETURN;
            }
        } else {
            decisions[i] = GOAL_ROLL_CPU_GUARD_NO_BALL;
        }
    }
    ballQuadrant = decisions[1];
    if ((decisions[0] == GOAL_ROLL_CPU_GUARD_CHASE) == (ballQuadrant == GOAL_ROLL_CPU_GUARD_CHASE)) {
        int distance;
        VEC_Subtract(&mon->position, &balls[0].position, &delta);
        distance = VEC_Mag(&delta);
        VEC_Subtract(&mon->position, &balls[1].position, &delta);
        if (distance >= VEC_Mag(&delta)) {
            result = PokeathlonGoalRollCpu_ApplyGuardDecision(mon, balls, decisions[0], participant);
        } else {
            balls++;
            result = PokeathlonGoalRollCpu_ApplyGuardDecision(mon, balls, ballQuadrant, participant);
        }
    } else {
        if (decisions[0] >= ballQuadrant) {
            result = PokeathlonGoalRollCpu_ApplyGuardDecision(mon, balls, decisions[0], participant);
        } else {
            balls++;
            result = PokeathlonGoalRollCpu_ApplyGuardDecision(mon, balls, ballQuadrant, participant);
        }
    }
    if (mon->mode == GOAL_ROLL_MODE_DASH) {
        PokeathlonCourse_AddMonCounter(course, participant, member, POKEATHLON_COUNTER_DASHES, 1);
    }
    return result;
}

static int PokeathlonGoalRollCpu_ApplyGuardDecision(PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *ball, int decision, int participant) {
    volatile u16 point[2];

    switch (decision) {
    case GOAL_ROLL_CPU_GUARD_RETURN:
    case GOAL_ROLL_CPU_FLEX_RETURN: {
        const u16(*points)[2] = gPokeathlonGoalRollCpuGuardPoints[participant - 1];

        if (PokeathlonGoalRollCpu_IsNearPoint(mon->position.x / FX32_ONE, mon->position.y / FX32_ONE, points)) {
            return 0;
        }
        if ((s32)LCRandom() % 2 == 0) {
            point[0] = points[0][0];
            point[1] = points[0][1];
        } else if ((s32)LCRandom() % 2 == 0) {
            const u16 *other = gPokeathlonGoalRollCpuGuardPoints1[participant - 1][0];
            point[0] = other[0];
            point[1] = other[1];
        } else {
            const u16 *other = gPokeathlonGoalRollCpuGuardPoints2[participant - 1][0];
            point[0] = other[0];
            point[1] = other[1];
        }
        point[0] += 8 - (s32)LCRandom() % 17;
        point[1] += 8 - (s32)LCRandom() % 17;
        mon->target.x = point[0] << FX32_SHIFT;
        mon->target.y = point[1] << FX32_SHIFT;
        return 6;
    }
    case GOAL_ROLL_CPU_GUARD_CHASE:
    case GOAL_ROLL_CPU_FLEX_CHASE:
        mon->target = ball->position;
        if ((s32)LCRandom() % 100 < 80) {
            mon->mode = GOAL_ROLL_MODE_DASH;
        }
        return 10;
    default:
        GF_ASSERT(FALSE);
    case GOAL_ROLL_CPU_GUARD_NO_BALL:
    case GOAL_ROLL_CPU_FLEX_NO_BALL:
        return 0;
    }
}

static int PokeathlonGoalRollCpu_ThinkFlex(PokeathlonCourseData *course, u8 participant, u8 member, PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *balls, PokeathlonGoalRollCpuTeam *teams) {
    u8 i;
    int ballQuadrant;
    int quadrant;
    int radius;
    int result;
    fx32 distance;
    PokeathlonGoalRollBall *ball;
    u8 decisions[2];
    u8 quadrants[2];
    VecFx32 delta;
    u8 decision0;
    u8 decision1;
    BOOL isChase0;
    BOOL isChase1;

    if (mon->mode) {
        return 0;
    }
    if (mon->motion) {
        return 0;
    }
    quadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE);
    for (i = 0; i < 2; i++) {
        ball = &balls[i];
        if (ball->state == GOAL_ROLL_BALL_ROLLING && ball->kind != GOAL_ROLL_BALL_NONE) {
            quadrants[i] = PokeathlonGoalRollCpu_GetQuadrant(128, 96, ball->position.x / FX32_ONE, ball->position.y / FX32_ONE);
        } else {
            quadrants[i] = 4;
        }
    }
    for (i = 0; i < 2; i++) {
        ball = &balls[i];
        decisions[i] = GOAL_ROLL_CPU_FLEX_NO_BALL;
        ballQuadrant = quadrants[i];
        if (ballQuadrant == 4) {
            continue;
        }
        radius = 0;
        if (ball->kind == GOAL_ROLL_BALL_SMALL) {
            radius = 8;
        } else if (ball->kind == GOAL_ROLL_BALL_BIG) {
            radius = 12;
        } else {
            GF_ASSERT(FALSE);
        }
        if (PokeathlonGoalRollCpu_CirclesTouch(&mon->position, PokeathlonCourseObject_GetCollisionRadius(mon->object) << FX32_SHIFT, &ball->position, radius << FX32_SHIFT)) {
            decisions[i] = GOAL_ROLL_CPU_FLEX_NO_BALL;
        } else if (quadrant == ballQuadrant) {
            if (PokeathlonGoalRollCpu_IsMonInTargetQuadrantOfBall(ball->position.x / FX32_ONE, ball->position.y / FX32_ONE, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE, quadrant, participant)) {
                decisions[i] = GOAL_ROLL_CPU_FLEX_BEHIND_BALL;
            }
        }
    }
    if (decisions[0] == GOAL_ROLL_CPU_FLEX_BEHIND_BALL) {
        return PokeathlonGoalRollCpu_ApplyChaserDecision(mon, balls, decisions[0], quadrant);
    }
    if (decisions[1] == GOAL_ROLL_CPU_FLEX_BEHIND_BALL) {
        balls++;
        return PokeathlonGoalRollCpu_ApplyChaserDecision(mon, balls, decisions[1], quadrant);
    }
    if (teams[participant].order[0].mon->power >= 50) {
        return PokeathlonGoalRollCpu_MoveToEmptierQuadrant(mon, teams, participant);
    }
    for (i = 0; i < 2; i++) {
        ball = &balls[i];
        if (quadrants[i] != 4) {
            if (participant == quadrants[i]) {
                if (PokeathlonGoalRollCpu_IsMonInTargetQuadrantOfBall(ball->position.x / FX32_ONE, ball->position.y / FX32_ONE, mon->position.x / FX32_ONE, mon->position.y / FX32_ONE, quadrant, participant)) {
                    decisions[i] = GOAL_ROLL_CPU_FLEX_CHASE;
                } else {
                    decisions[i] = GOAL_ROLL_CPU_FLEX_RETURN;
                }
            } else {
                decisions[i] = GOAL_ROLL_CPU_FLEX_RETURN;
            }
        } else {
            decisions[i] = GOAL_ROLL_CPU_FLEX_SPREAD;
        }
    }
    decision0 = decisions[0];
    decision1 = decisions[1];
    // BUG: the Flex decisions are 10..12, so neither is ever the Guard's chase and the nearer ball is always taken.
    isChase0 = decision0 == GOAL_ROLL_CPU_GUARD_CHASE ? TRUE : FALSE;
    isChase1 = decision1 == GOAL_ROLL_CPU_GUARD_CHASE ? TRUE : FALSE;
    if (isChase0 == isChase1) {
        VEC_Subtract(&mon->position, &balls[0].position, &delta);
        distance = VEC_Mag(&delta);
        VEC_Subtract(&mon->position, &balls[1].position, &delta);
        if (distance >= VEC_Mag(&delta)) {
            result = PokeathlonGoalRollCpu_ApplyFlexDecision(mon, balls, decisions[0], participant, teams);
        } else {
            balls++;
            result = PokeathlonGoalRollCpu_ApplyFlexDecision(mon, balls, decision1, participant, teams);
        }
    } else if (decision0 >= decision1) {
        result = PokeathlonGoalRollCpu_ApplyFlexDecision(mon, balls, decision0, participant, teams);
    } else {
        balls++;
        result = PokeathlonGoalRollCpu_ApplyFlexDecision(mon, balls, decision1, participant, teams);
    }
    if (mon->mode == GOAL_ROLL_MODE_DASH) {
        PokeathlonCourse_AddMonCounter(course, participant, member, POKEATHLON_COUNTER_DASHES, 1);
    }
    return result;
}

static int PokeathlonGoalRollCpu_ApplyFlexDecision(PokeathlonGoalRollMon *mon, PokeathlonGoalRollBall *ball, int decision, u8 participant, PokeathlonGoalRollCpuTeam *teams) {
    if (decision == GOAL_ROLL_CPU_FLEX_SPREAD) {
        return PokeathlonGoalRollCpu_MoveToEmptierQuadrant(mon, teams, participant);
    }
    return PokeathlonGoalRollCpu_ApplyGuardDecision(mon, ball, decision, participant);
}

// Sends the Pokemon to a random spot in a quadrant with fewer Pokemon than the one before it.
static int PokeathlonGoalRollCpu_MoveToEmptierQuadrant(PokeathlonGoalRollMon *mon, PokeathlonGoalRollCpuTeam *teams, u8 excluded) {
    u8 counts[4] = { 0, 0, 0, 0 };
    u8 i, j;
    int chosen, x, y;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 3; j++) {
            PokeathlonGoalRollMon *other = teams[i].order[j].mon;
            int quadrant = PokeathlonGoalRollCpu_GetQuadrant(128, 96, other->position.x / FX32_ONE, other->position.y / FX32_ONE);
            if (quadrant != 4) {
                counts[quadrant]++;
            }
        }
    }
    chosen = 0;
    for (i = 1; i < 4; i++) {
        if (counts[i - 1] > counts[i] && i != excluded) {
            chosen = i;
        }
    }
    x = (int)LCRandom() % 64;
    y = (int)LCRandom() % 32;
    switch (chosen) {
    case 0:
        mon->target.x = (128 - x) << FX32_SHIFT;
        mon->target.y = (96 - y) << FX32_SHIFT;
        break;
    case 1:
        mon->target.x = (128 + x) << FX32_SHIFT;
        mon->target.y = (96 - y) << FX32_SHIFT;
        break;
    case 2:
        mon->target.x = (128 - x) << FX32_SHIFT;
        mon->target.y = (96 + y) << FX32_SHIFT;
        break;
    case 3:
        mon->target.x = (128 + x) << FX32_SHIFT;
        mon->target.y = (96 + y) << FX32_SHIFT;
        break;
    default:
        GF_ASSERT(FALSE);
        break;
    }
    return 10;
}

// Lets the Pokemon in `slot` act according to its role; the result is how long it waits.
static void PokeathlonGoalRollCpu_Think(PokeathlonCourseData *course, PokeathlonGoalRollCpu *cpu, PokeathlonGoalRollCpuSlot *slot) {
    u8 participant = cpu->firstCpu + cpu->current;
    u8 role = cpu->orderSlot;
    // BUG: the role is passed where the Pokemon's team slot is expected, so dashes are counted for the wrong Pokemon.
    switch (role) {
    case GOAL_ROLL_CPU_ROLE_GUARD:
        slot->wait = (u32)PokeathlonGoalRollCpu_ThinkGuard(course, participant, role, slot->mon, cpu->balls);
        break;
    case GOAL_ROLL_CPU_ROLE_CHASER:
        slot->wait = (u32)PokeathlonGoalRollCpu_ThinkChaser(course, participant, role, slot->mon, cpu->balls);
        break;
    case GOAL_ROLL_CPU_ROLE_FLEX:
        slot->wait = PokeathlonGoalRollCpu_ThinkFlex(course, participant, role, slot->mon, cpu->balls, cpu->teams);
        break;
    default:
        GF_ASSERT(FALSE);
        break;
    }
}

// 0 top left, 1 top right, 2 bottom left, 3 bottom right of (x, y)
static int PokeathlonGoalRollCpu_GetQuadrant(int x, int y, int pointX, int pointY) {
    if (pointX < x && pointY < y) {
        return 0;
    }
    if (pointX >= x && pointY < y) {
        return 1;
    }
    if (pointX < x && pointY >= y) {
        return 2;
    }
    if (pointX >= x && pointY >= y) {
        return 3;
    }
    GF_ASSERT(FALSE);
    return 4;
}

static BOOL PokeathlonGoalRollCpu_IsMonInTargetQuadrantOfBall(int x1, int y1, int x2, int y2, int quadrantInput, int participantInput) {
    int result = PokeathlonGoalRollCpu_GetQuadrant(x1, y1, x2, y2);
    u8 participant = *(const u8 *)&participantInput;
    u8 quadrant = *(const u8 *)&quadrantInput;
    u8 target;

    if (participant != quadrant) {
        target = 3 - quadrant;
    } else {
        target = participant;
    }
    return result == target;
}

static BOOL PokeathlonGoalRollCpu_IsMonNotOppositeOfBall(int x1, int y1, int x2, int y2, int quadrantInput, int participantInput) {
    int result = PokeathlonGoalRollCpu_GetQuadrant(x1, y1, x2, y2);
    u8 quadrant = *(const u8 *)&quadrantInput;
    u8 participant = *(const u8 *)&participantInput;

    if (participant == quadrant) {
        return (3 - participant) != result;
    }
    return FALSE;
}

BOOL PokeathlonGoalRollCpu_IsNearPoint(int x, int y, const u16 (*points)[2]) {
    u8 i;

    for (i = 0; i < 3; i++) {
        int dx = x - points[i][0];
        int dy = y - points[i][1];

        if (dx >= -8 && dx <= 8 && dy >= -8 && dy <= 8) {
            return TRUE;
        }
    }
    return FALSE;
}

static BOOL PokeathlonGoalRollCpu_CirclesTouch(const VecFx32 *a, fx32 radiusA, const VecFx32 *b, fx32 radiusB) {
    VecFx32 delta;

    VEC_Subtract(a, b, &delta);
    return VEC_Mag(&delta) < radiusA + radiusB;
}

// Screen shake task (started by PokeathlonScreenShake_Start).
void PokeathlonScreenShake_Task(SysTask *task, PokeathlonScreenShake *shake) {
    int sign;
    int x;
    int y;

    if (shake->direction == 0) {
        sign = 1;
    } else {
        sign = -1;
    }
    y = (shake->offset >> 12) * sign;
    x = y / 2;
    switch (shake->frame % 3) {
    case 0:
        x *= -1;
        break;
    case 1:
        x = 0;
        break;
    case 2:
        break;
    default:
        GF_ASSERT(FALSE);
        break;
    }

    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_0, BG_POS_OP_SET_X, x);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_1, BG_POS_OP_SET_X, x);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_2, BG_POS_OP_SET_X, x);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_3, BG_POS_OP_SET_X, x);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_0, BG_POS_OP_SET_Y, y);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_1, BG_POS_OP_SET_Y, y);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_2, BG_POS_OP_SET_Y, y);
    ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_3, BG_POS_OP_SET_Y, y);
    G2dRenderer_SetMainSurfaceCoords(SpriteSystem_GetRenderer(shake->spriteSystem), x << FX32_SHIFT, y << FX32_SHIFT);

    shake->offset -= shake->step;
    shake->direction ^= 1;
    shake->frame++;
    if (shake->frame >= shake->duration) {
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_0, BG_POS_OP_SET_X, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_1, BG_POS_OP_SET_X, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_2, BG_POS_OP_SET_X, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_3, BG_POS_OP_SET_X, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_0, BG_POS_OP_SET_Y, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_1, BG_POS_OP_SET_Y, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_2, BG_POS_OP_SET_Y, 0);
        ScheduleSetBgPosText(shake->bgConfig, GF_BG_LYR_MAIN_3, BG_POS_OP_SET_Y, 0);
        G2dRenderer_SetMainSurfaceCoords(SpriteSystem_GetRenderer(shake->spriteSystem), 0, 0);
        shake->task = NULL;
        SysTask_Destroy(task);
    }
}
