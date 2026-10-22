#ifndef POKEHEARTGOLD_APRICORN_BOX_H
#define POKEHEARTGOLD_APRICORN_BOX_H

#include "pm_string.h"
#include "save.h"

#define APRICORN_TYPE_COUNT         7
#define APRICORN_JUICE_FLAVOR_COUNT 5

typedef struct ApricornJuice {
    u16 fanCount; // Pokeathlon fans of the trainer who made it
    u8 mildness;
    u8 flavors[APRICORN_JUICE_FLAVOR_COUNT];
} ApricornJuice;

typedef struct SaveApricornBox SaveApricornBox;

SaveApricornBox *Save_ApricornBox_Get(SaveData *saveData);
void ApricornBox_AddJuiceMildnessForDays(SaveApricornBox *, int);
BOOL ApricornBox_GetJuice(SaveApricornBox *apricornBox, int index, ApricornJuice *out);
int ApricornJuice_GetFlavorTotal(ApricornJuice *data);
void ApricornBox_UpdateSteps(SaveApricornBox *saveApricornBox, u32 steps);
String *ApricornBox_GetJuiceName(SaveApricornBox *apricornBox, int type, u32 heapId);
String *ApricornBox_GetJuiceFlavorName(SaveApricornBox *apricornBox, u16 type, u32 heapId);
int ApricornBox_CountApricorn(SaveApricornBox *apricornBox, int apricorn);
int ApricornBox_GiveApricorn(SaveApricornBox *apricornBox, int apricorn, u8 amount);
int ApricornBox_GetKurtQuantity(SaveApricornBox *apricornBox);
void ApricornBox_SetKurtApricorn(SaveApricornBox *apricornBox, u32 apricorn, u32 quantity);
int ApricornBox_GetKurtBall(SaveApricornBox *apricornBox);

#endif // POKEHEARTGOLD_APRICORN_BOX_H
