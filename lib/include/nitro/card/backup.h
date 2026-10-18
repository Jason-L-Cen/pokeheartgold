#ifndef NITRO_CARD_BACKUP_BACKUP_H_
#define NITRO_CARD_BACKUP_BACKUP_H_

#include <nitro/card/common.h>

BOOL CARD_TryWaitBackupAsync(void);
void CARD_CancelBackupAsync(void);
int CARD_SpiWaitGetStatus(void);

BOOL CARDi_RequestStreamCommand(uaddr src, uaddr dst, u32 len, MIDmaCallback callback, void *arg, BOOL is_async, CARDRequest req_type, int req_retry, CARDRequestMode req_mode);

static inline BOOL CARDi_ReadBackup(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg, BOOL is_async) {
    return CARDi_RequestStreamCommand(src, (uaddr)dst, len, callback, arg, is_async, CARD_REQ_READ_BACKUP, 1, CARD_REQUEST_MODE_RECV);
}

static inline void CARD_ReadBackupAsync(u32 src, void *dst, u32 len, MIDmaCallback callback, void *arg) {
    CARDi_ReadBackup(src, dst, len, callback, arg, TRUE);
}

BOOL CARD_WaitBackupAsync(void);
u32 CARD_GetBackupSectorSize(void);

static inline BOOL CARD_ReadBackup(u32 src, void *dst, u32 len) {
    return CARDi_ReadBackup(src, dst, len, NULL, NULL, FALSE);
}

static inline BOOL CARDi_WriteBackup(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg, BOOL is_async) {
    return CARDi_RequestStreamCommand((uaddr)src, dst, len, callback, arg, is_async, CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND);
}

static inline void CARD_WriteBackupAsync(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg) {
    CARDi_WriteBackup(dst, src, len, callback, arg, TRUE);
}

static inline BOOL CARD_WriteBackup(u32 dst, const void *src, u32 len) {
    return CARDi_WriteBackup(dst, src, len, NULL, NULL, FALSE);
}

static inline BOOL CARDi_WriteAndVerifyBackup(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg, BOOL is_async) {
    return CARDi_RequestStreamCommand((uaddr)src, dst, len, callback, arg, is_async, CARD_REQ_WRITE_BACKUP, CARD_RETRY_COUNT_MAX, CARD_REQUEST_MODE_SEND_VERIFY);
}

static inline void CARD_WriteAndVerifyBackupAsync(u32 dst, const void *src, u32 len, MIDmaCallback callback, void *arg) {
    CARDi_WriteAndVerifyBackup(dst, src, len, callback, arg, TRUE);
}

static inline BOOL CARD_WriteAndVerifyBackup(u32 dst, const void *src, u32 len) {
    return CARDi_WriteAndVerifyBackup(dst, src, len, NULL, NULL, FALSE);
}

#endif //NITRO_CARD_BACKUP_BACKUP_H_
