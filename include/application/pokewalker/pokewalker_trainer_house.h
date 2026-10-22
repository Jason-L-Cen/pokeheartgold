#ifndef POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_TRAINER_HOUSE_H
#define POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_TRAINER_HOUSE_H

#include "party.h"
#include "save.h"
#include "save_trainer_house.h"

// The player's Trainer House set as exchanged in the Pokewalker connection app (built at app start), with a CRC.
typedef struct PokewalkerTrainerHouseData {
    TrainerHouseSet set;
    u8 unk_180[0x72];
    u16 crc; // GF_CalcCRC16 of everything before it
} PokewalkerTrainerHouseData;

// dst: PokewalkerTrainerHouseData
void PokewalkerApp_BuildTrainerHouseData(SaveData *saveData, void *dst);
void PokewalkerApp_SaveTrainerHouseData(SaveData *saveData, PokewalkerTrainerHouseData *data);

#endif // POKEHEARTGOLD_APPLICATION_POKEWALKER_POKEWALKER_TRAINER_HOUSE_H
