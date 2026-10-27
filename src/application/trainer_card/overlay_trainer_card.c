#include "overlay_trainer_card.h"

#include "global.h"
#ifdef HEARTGOLD_NATIVE
#include <stddef.h>
#include "native_trainer_card_ui.h"
_Static_assert(offsetof(Ov50_021E5A3C, saveData) == 0x670
    && offsetof(TrainerCardAppArgs, settings) + offsetof(TrainerCardAppSettings, saveData) == 0x670,
    "Trainer Card saveData ABI");
_Static_assert(offsetof(Ov50_021E5A3C, reqUpdateSignature) == 0x674
    && offsetof(TrainerCardAppArgs, settings) + offsetof(TrainerCardAppSettings, showBack) == 0x674,
    "Trainer Card signature request aliases showBack");
#endif

#include "overlay_manager.h"
#include "overlay_trainer_card_main.h"
#include "overlay_trainer_card_signature.h"
#include "save_trainer_card.h"
#include "sound_system.h"

FS_EXTERN_OVERLAY(trainer_card_main);
FS_EXTERN_OVERLAY(trainer_card_signature);

enum {
    TRAINERCARD_RUN_INIT,
    TRAINERCARD_RUN_EXEC,
    TRAINERCARD_RUN_SIGNATURE_INIT,
    TRAINERCARD_RUN_SIGNATURE_EXEC,
    TRAINERCARD_RUN_EXIT,
};

static BOOL isSubprocFinished(OverlayManager **man);
static int TCardAppRunStep_Init(TrainerCardAppState *work);
static int TCardAppRunStep_Exec(TrainerCardAppState *work);
static int TCardAppRunStep_SignatureInit(TrainerCardAppState *work);
static int TCardAppRunStep_SignatureExec(TrainerCardAppState *work);

static BOOL isSubprocFinished(OverlayManager **man) {
    if (*man && OverlayManager_Run(*man)) {
        OverlayManager_Delete(*man);
        *man = NULL;
        return TRUE;
    }
    return FALSE;
}

BOOL TrainerCard_Init(OverlayManager *man, int *state) {
    void *ptr = OverlayManager_GetArgs(man);
    Heap_Create(HEAP_ID_3, HEAP_ID_TRAINER_CARD, 0x1000);

    TrainerCardAppState *data = OverlayManager_CreateAndGetData(man, sizeof(TrainerCardAppState), HEAP_ID_TRAINER_CARD);
    MI_CpuClear8(data, sizeof(TrainerCardAppState));

    data->heapID = HEAP_ID_TRAINER_CARD;
    data->parentData = ptr;
    data->unk10 = ptr;
#ifdef HEARTGOLD_NATIVE
    NativeTrainerCardUIBegin(data);
#endif
    return TRUE;
}

BOOL TrainerCard_Main(OverlayManager *man, int *state) {
    TrainerCardAppState *data = OverlayManager_GetData(man);
    switch (*state) {
    case TRAINERCARD_RUN_INIT:
        *state = TCardAppRunStep_Init(data);
        break;
    case TRAINERCARD_RUN_EXEC:
        *state = TCardAppRunStep_Exec(data);
        break;
    case TRAINERCARD_RUN_SIGNATURE_INIT:
        *state = TCardAppRunStep_SignatureInit(data);
        break;
    case TRAINERCARD_RUN_SIGNATURE_EXEC:
        *state = TCardAppRunStep_SignatureExec(data);
        break;
    case TRAINERCARD_RUN_EXIT:
    default:
        return TRUE;
    }
    return FALSE;
}

BOOL TrainerCard_Exit(OverlayManager *man, int *state) {
    TrainerCardAppState *data = OverlayManager_GetData(man);
#ifdef HEARTGOLD_NATIVE
    NativeTrainerCardUIRetire(data);
#endif
    MI_CpuClear8(data, sizeof(TrainerCardAppState));
    OverlayManager_FreeData(man);
    Sound_ClearSecondaryScene();
    Heap_Destroy(HEAP_ID_TRAINER_CARD);
    return TRUE;
}

static int TCardAppRunStep_Init(TrainerCardAppState *work) {
    static const OverlayManagerTemplate template = {
        TrainerCardMainApp_Init,
        TrainerCardMainApp_Main,
        TrainerCardMainApp_Exit,
        FS_OVERLAY_ID(trainer_card_main)
    };

    work->ov_mgr = OverlayManager_New(&template, work->parentData, work->heapID);
    return TRAINERCARD_RUN_EXEC;
}

static int TCardAppRunStep_Exec(TrainerCardAppState *work) {
    if (!isSubprocFinished(&work->ov_mgr)) {
        return TRAINERCARD_RUN_EXEC;
    } else if (work->parentData->reqUpdateSignature != 0) {
        return TRAINERCARD_RUN_SIGNATURE_INIT;
    } else {
        return TRAINERCARD_RUN_EXIT;
    }
}

static int TCardAppRunStep_SignatureInit(TrainerCardAppState *work) {
    const OverlayManagerTemplate template = {
        TrainerCardSignature_Init,
        TrainerCardSignature_Main,
        TrainerCardSignature_Exit,
        FS_OVERLAY_ID(trainer_card_signature)
    };
    work->ov_mgr = OverlayManager_New(&template, work->parentData->saveData, work->heapID);
    return TRAINERCARD_RUN_SIGNATURE_EXEC;
}

static int TCardAppRunStep_SignatureExec(TrainerCardAppState *work) {
    struct SaveTrainerCard *ptr;
    if (!isSubprocFinished(&work->ov_mgr)) {
        return TRAINERCARD_RUN_SIGNATURE_EXEC;
    }

    ptr = Save_TrainerCard_Get(work->parentData->saveData);
    work->parentData->signatureExists = TrainerCard_SignatureExists(ptr);
    MI_CpuCopy8(TrainerCard_GetSignature(ptr), work->parentData->signature, sizeof(work->parentData->signature));
    return TRAINERCARD_RUN_INIT;
}
