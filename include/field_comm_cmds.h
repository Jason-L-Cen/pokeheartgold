#ifndef POKEHEARTGOLD_FIELD_COMM_CMDS_H
#define POKEHEARTGOLD_FIELD_COMM_CMDS_H

#include "global.h"
#include "communication_command.h"

#include "overlay_manager.h"

extern const OverlayManagerTemplate gApplication_OakSpeech;

void FieldComm_RegisterCommands(void *context);
u32 CommCmd_GetTrainerCardSize(void);
u32 CommCmd_GetSize3008(void);
u32 CommCmd_GetSize1008(void);
u32 CommCmd_GetSize24(void);
u32 CommCmd_GetSize1424(void);
void CommCmd_RecvIgnored(int netId, int size, void *data, void *context);
u32 CommCmd_GetSize10(void);
u32 CommCmd_GetSize50(void);
u32 CommCmd_GetSize4(void);
u32 CommCmd_GetSize4Copy(void);

#endif // POKEHEARTGOLD_FIELD_COMM_CMDS_H
