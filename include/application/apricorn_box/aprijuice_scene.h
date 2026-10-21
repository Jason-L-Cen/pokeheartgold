#ifndef POKEHEARTGOLD_APPLICATION_APRICORN_BOX_APRIJUICE_SCENE_H
#define POKEHEARTGOLD_APPLICATION_APRICORN_BOX_APRIJUICE_SCENE_H

#include "global.h"

#include "camera.h"
#include "gf_3d_vramman.h"
#include "particle_system.h"
#include "pokepic.h"

// The 3D part of the Aprijuice screen: the Pokemon picture and the particle effect.
typedef struct AprijuiceScene {
    enum HeapID heapId;
    GF3DVramMan *vramMan;
    Camera *camera;
    PokepicManager *pokepicManager;
    u8 padding_10[0x10];
    Pokepic *pokepic;
    SPLEmitter *emitter;
} AprijuiceScene;

void AprijuiceScene_InitCamera(AprijuiceScene *scene, enum HeapID heapId);

#endif // POKEHEARTGOLD_APPLICATION_APRICORN_BOX_APRIJUICE_SCENE_H
