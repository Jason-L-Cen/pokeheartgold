#ifndef POKEHEARTGOLD_APPLICATION_PLAZA_GAME_SWALOT_PLOP_H
#define POKEHEARTGOLD_APPLICATION_PLAZA_GAME_SWALOT_PLOP_H

#include "global.h"

#include "application/plaza_game.h"

#include "bg_window.h"
#include "camera.h"
#include "filesystem.h"
#include "font_sprite.h"
#include "gf_3d_vramman.h"
#include "heap.h"
#include "message_format.h"
#include "model_animation.h"
#include "msgdata.h"
#include "obj_char_transfer.h"
#include "overlay_manager.h"
#include "pm_string.h"
#include "sprite.h"
#include "sprite_resource_header.h"
#include "sprite_resource.h"

// SWALOT Plop, a Wi-Fi Plaza game: every player flicks berries on the touch screen towards a
// spinning SWALOT; berries that land in its mouth score points, and consecutive hits build a combo.

// One entry of the level table (NARC 212 file 0).
typedef struct SwalotPlopLevel {
    int hitCount; // hits needed to reach this level
    u32 spinSpeed;
    u32 cameraHeight;
    u32 cameraPitch;
    u32 mouthOffset;
    u32 scale;
} SwalotPlopLevel;

typedef struct SwalotPlopLevelTable {
    u32 count;
    SwalotPlopLevel *levels;
} SwalotPlopLevelTable;

// Part of SwalotPlopApp shared with the game screen and the tally.
typedef struct SwalotPlopShared {
    SwalotPlopLevelTable levelTable;
    PlayerProfile *profiles[4];
    u32 nameFlags[4];
} SwalotPlopShared;

typedef struct SwalotPlopScores {
    u32 scores[4];
} SwalotPlopScores;

// Run on the parent only: game clock, the scores reported at the end, and the hit count
// that selects the level.
typedef struct SwalotPlopTally {
    int timer;
    BOOL running;
    u32 playerCount;
    u32 scores[4];
    u32 receivedMask;
    u8 unk20[0x10];
    SwalotPlopShared *shared;
    u16 hitCount;
    volatile u8 level;
    u8 levelChanged;
} SwalotPlopTally;

// What a shot looks like on the link (command 24).
typedef struct SwalotPlopShotRecord {
    u16 owner;
    u16 caught;
    s16 dir[3];
    s16 start[3];
    s16 speed;
    s8 combo;
    u8 big;
} SwalotPlopShotRecord;

typedef struct SwalotPlopShotParams {
    u16 owner;
    u16 caught;
    VecFx32 direction;
    fx32 speed;
    VecFx32 start;
    u32 combo;
    u32 big;
} SwalotPlopShotParams;

enum SwalotPlopShotState {
    SHOT_STATE_FREE,
    SHOT_STATE_FLYING,
    SHOT_STATE_FALLING,
    SHOT_STATE_CAUGHT,
    SHOT_STATE_REPORT,
    SHOT_STATE_BOUNCED,
};

// A berry in flight.
typedef struct SwalotPlopShot {
    u8 state;
    u8 remove;
    u16 frames;
    SwalotPlopShotParams params;
    VecFx32 position;
    s16 timer;
    u16 combo;
    BOOL crossedRim;
    fx32 distance;
} SwalotPlopShot;

// The touch samples of one frame.
typedef struct SwalotPlopTouch {
    u16 count;
    TPData samples[8];
} SwalotPlopTouch;

// Touch positions of the current flick.
typedef struct SwalotPlopFlick {
    u16 x;
    u16 y;
} SwalotPlopFlick;

typedef struct SwalotPlopPoint {
    s16 x;
    s16 y;
} SwalotPlopPoint;

typedef struct SwalotPlopFlickQueue {
    BOOL active;
    SwalotPlopFlick points[8];
    u16 read;
    u16 write;
} SwalotPlopFlickQueue;

// SWALOT itself: orientation, mouth and body measurements (scaled per level).
typedef struct SwalotPlopBody {
    u8 active;
    u8 mouthOpen;
    u16 spin;
    u16 tilt;
    u16 spinSpeed;
    fx32 mouthDepth;
    MtxFx33 tiltMtx;
    MtxFx33 spinMtx;
    MtxFx33 rotation;
    VecFx32 normal;
    VecFx32 mouthPos;
    fx32 mouthPlane;
    fx32 pushRadius;
    fx32 pushStrength;
    VecFx32 offset;
    fx32 scale;
    fx32 mouthDepthRange;
    fx32 mouthDepthMin;
    fx32 unkB4;
    fx32 mouthOffsetX;
    fx32 mouthOffsetY;
    fx32 heightMin;
    fx32 heightRange;
    fx32 topRadius;
    fx32 radius;
    fx32 slope;
    fx32 modelScale;
    VecFx32 position;
    fx32 height;
} SwalotPlopBody;

// SWALOT's model, its three animations and the mouth animation.
typedef struct SwalotPlopStage {
    ModelInstance models[3];
    ModelResource resources[3];
    ModelAnimation animations[3];
    fx32 animSpeed;
    u8 mouthActive;
    u8 mouthState;
    s16 mouthTimer;
    fx32 mouthFrame;
    fx32 mouthStep;
} SwalotPlopStage;

// The points that pop up over a player's seat when a berry is caught.
typedef struct SwalotPlopPopups {
    Sprite *sprites[3];
    s32 timers[3];
    VecFx32 pos[3];
    int mode;
} SwalotPlopPopups;

// The window with the other players' names shown during the intro.
typedef struct SwalotPlopIntro {
    u32 state;
    s32 delay;
    u32 counter;
    Window window;
} SwalotPlopIntro;

typedef struct SwalotPlopFinish {
    u32 state;
    u32 counter;
} SwalotPlopFinish;

// The SWALOT model of the top screen with its six animations.
typedef struct SwalotPlopScene {
    ModelInstance models[3];
    ModelResource resources[3];
    ModelAnimation animations[6];
    volatile u16 current;
    u16 state;
    ModelAnimation special;
    volatile s32 frame;
    u16 leader;
    u16 bonus;
} SwalotPlopScene;

typedef struct SwalotPlopBgFade {
    u16 state;
    s16 timer;
    s16 layer;
    u8 palette;
    u8 counter;
} SwalotPlopBgFade;

// Sprite and shadow of a berry.
typedef struct SwalotPlopShotView {
    Sprite *sprite;
    ModelInstance model;
    ModelInstance shadow;
    int state;
    SwalotPlopShot *shot;
    u16 rotX;
    u16 rotZ;
    u16 rotSpeedX;
    u16 rotSpeedZ;
    s16 timer;
} SwalotPlopShotView;

typedef struct SwalotPlopCursor {
    BOOL active;
    Sprite *sprite;
} SwalotPlopCursor;

typedef struct SwalotPlopParticle {
    BOOL active;
    Sprite *sprite;
    VecFx32 position;
    s32 age;
    BOOL direction;
} SwalotPlopParticle;

// Accelerated motion: value = start + slope * step + curvature * step^2 / 2.
typedef struct SwalotPlopCurve {
    s32 value;
    s32 start;
    s32 slope;
    s32 curvature;
    s32 step;
    s32 duration;
} SwalotPlopCurve;

// The combo counter that slides in on the bottom screen.
typedef struct SwalotPlopCombo {
    SpriteResource *res[4];
    SpriteResourcesHeader header;
    Sprite *sprite;
    Window window;
    TextOBJ *textObj;
    TextOBJLayout *textLayout;
    ObjCharSpaceReservation charTransfer;
    String *string;
    SpriteResource *textPltt;
    SwalotPlopCurve slideX;
    SwalotPlopCurve slideY;
    s16 slideFrame;
    s16 bounceFrame;
    u16 state;
    s16 hideDelay;
    enum HeapID heapId;
} SwalotPlopCombo;

// Graphics of the game screens.
typedef struct SwalotPlopGfx {
    BgConfig *bgConfig;
    MessageFormat *msgFormat;
    MsgData *msgData;
    String *string1;
    String *string2;
    u32 unk14;
    GF3DVramMan *vramMan;
    SpriteList *spriteList;
    G2dRenderer renderer;
    GF_2DGfxResMan *resMan[4];
    SpriteResourcesHeader header;
    SpriteResource *res[4];
    FontSystem *fontSystem;
    Camera *camera;
    VecFx32 cameraTarget;
    SwalotPlopStage stage;
    SwalotPlopPopups popups[4];
    SwalotPlopIntro intro;
    SwalotPlopFinish finish;
    u8 unk4A8[0x18];
    SwalotPlopScene scene;
    PlazaGameSign *sign;
    SwalotPlopBgFade bgFade;
    ModelResource shotModels[5];
    ModelResource shadowModel;
    SpriteResource *shotRes[4];
    SpriteResourcesHeader shotHeader;
    SwalotPlopShotView shotViews[0x60];
    SwalotPlopCursor cursor;
    SwalotPlopParticle particles[8];
    SwalotPlopCombo combo;
} SwalotPlopGfx;

// Interpolates the level parameters (spin, camera, scale) over 16 frames.
typedef struct SwalotPlopLevelBlend {
    s32 current[6];
    s32 start[6];
    s32 delta[6];
    s32 step;
} SwalotPlopLevelBlend;

// The hand that throws two demo berries before the game starts.
typedef struct SwalotPlopDemo {
    SpriteResource *res[4];
    SpriteResourcesHeader header;
    Sprite *sprite;
    u8 active;
    u8 count;
    u8 state;
    u8 thrown;
    int timer;
} SwalotPlopDemo;

// The scores the players report every 30 frames; once all reported, the leader is shown.
typedef struct SwalotPlopScoreReports {
    u32 scores[4];
    u32 mask;
    u32 leader;
} SwalotPlopScoreReports;

typedef struct SwalotPlopSpeedUp {
    u16 state;
    u8 active;
    s8 frame;
} SwalotPlopSpeedUp;

enum SwalotPlopPhase {
    SWALOT_PLOP_PHASE_NORMAL,
    SWALOT_PLOP_PHASE_FAST,
    SWALOT_PLOP_PHASE_FASTEST,
};

typedef struct SwalotPlopGame {
    u32 phase;
    BOOL running;
    u32 frame;
    u32 duration;
    u32 playerCount;
    u32 localIndex;
    s32 score;
    s32 combo;
    SwalotPlopFlickQueue flick;
    u32 releasedFrames;
    SwalotPlopShot shots[0x60];
    SwalotPlopBody body;
    SwalotPlopGfx gfx;
    NNSFndAllocator allocator;
    u32 finalScores[4];
    SwalotPlopTouch touch;
    SwalotPlopLevelBlend levelBlend;
    SwalotPlopShared *shared;
    u16 level;
    u16 prevLevel;
    u8 options[4];
    u32 optionsSet;
    SwalotPlopDemo demo;
    SwalotPlopScoreReports scoreReports;
    SwalotPlopSpeedUp speedUp;
} SwalotPlopGame;

typedef struct SwalotPlopApp {
    PlazaGameEntry *entry;
    PlazaGameResults *results;
    PlazaGameRoster roster;
    PlazaGameResultData resultData;
    SwalotPlopTally *tally;
    SwalotPlopGame *game;
    BOOL startReceived;
    BOOL finishReceived;
    BOOL scoresReceived;
    u16 netId;
    u16 localIndex;
    SwalotPlopShared shared;
    u32 selection;
    u32 playerCount;
    u32 connectionLostState;
    u32 syncRound;
    BOOL canSend;
    u8 syncCounts[4];
} SwalotPlopApp;

// Link command table entry (Comm_RegisterCommandTable).
typedef struct SwalotPlopCommand {
    void (*handler)(int netId, int size, void *data, void *context);
    u32 (*getSize)(void);
    void *getBuffer;
} SwalotPlopCommand;

BOOL SwalotPlop_Init(OverlayManager *man, int *state);
BOOL SwalotPlop_Main(OverlayManager *man, int *state);
BOOL SwalotPlop_Exit(OverlayManager *man, int *state);

int SwalotPlopBody_GetTiltNormal(SwalotPlopBody *body, u32 frame, u32 duration);
int SwalotPlopBody_GetTiltFast(SwalotPlopBody *body, u32 frame, u32 duration);
u16 SwalotPlopBody_GetTiltFastest(SwalotPlopBody *body, u32 frame, u32 duration);
u32 SwalotPlop_GetNoDataSize(void);
void SwalotPlop_HandleStart(int netId, int size, void *data, void *context);
void SwalotPlop_HandleFinish(int netId, int size, void *data, void *context);
void SwalotPlop_HandleShot(int netId, int size, void *data, void *context);
void SwalotPlop_HandleFinalScore(int netId, int size, void *data, void *context);
void SwalotPlop_HandleScores(int netId, int size, void *data, void *context);
void SwalotPlop_HandleScoreReport(int netId, int size, void *data, void *context);
void SwalotPlop_HandleSync(int netId, int size, void *data, void *context);
void SwalotPlop_HandleLevel(int netId, int size, void *data, void *context);
u32 SwalotPlop_GetShotSize(void);
u32 SwalotPlop_GetScoresSize(void);
u32 SwalotPlop_GetScoreSize(void);
u32 SwalotPlop_GetLevelSize(void);

#endif // POKEHEARTGOLD_APPLICATION_PLAZA_GAME_SWALOT_PLOP_H
