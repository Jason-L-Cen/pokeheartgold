#include "fs_private.h"
#include <nitro/code32.h>

#ifdef HEARTGOLD_ABI32
#include <nitro/fs/native_file.h>
#include <nitro/fs/native_overlay.h>
#include <stddef.h>
#include "assert.h"
_Static_assert(sizeof(FSFile)==0x48 && offsetof(FSFile,stat)==12 && offsetof(FSFile,error)==20 &&
               offsetof(FSFile,prop.file.own_id)==32 && offsetof(FSFile,prop.file.top)==36 &&
               offsetof(FSFile,prop.file.bottom)==40 && offsetof(FSFile,prop.file.pos)==44,"Native file mirror layout");
BOOL is_init=FALSE;
void FS_Init(u32 dma) {
    if (!is_init) {
        FS_InitArchive(&fsi_arc_rom);
        BOOL registered=FS_RegisterArchiveName(&fsi_arc_rom,"rom",3);
        GF_ASSERT(registered);
        if (!registered) return;
        fsi_arc_rom.flag|=FS_ARCHIVE_FLAG_LOADED;
        FS_SetDefaultDMA(dma); is_init=TRUE;
    }
}
BOOL FS_IsAvailable(void) { return is_init && HGNativeFileRequest(HG_FILE_AVAILABLE,0,0,0,0); }
void FS_InitFile(FSFile *file) { HGNativeFileRequest(HG_FILE_INIT,(u32)file,0,0,0); }
BOOL FS_ConvertPathToFileID(FSFileID *id,const char *path) {
    if (!FS_IsAvailable() || !id) return FALSE;
    u32 index=HGNativeFileRequest(HG_FILE_RESOLVE,0,(u32)path,0,0);
    if (!index) return FALSE;
    id->arc=&fsi_arc_rom; id->file_id=index; return TRUE;
}
BOOL FS_OpenFileFast(FSFile *file,FSFileID id) {
    if (!FS_IsAvailable() || id.arc!=&fsi_arc_rom) return FALSE;
    if (!HGNativeFileRequest(HG_FILE_OPEN,(u32)file,id.file_id,0,0)) return FALSE;
    file->arc=&fsi_arc_rom; file->command=FS_COMMAND_OPENFILEFAST;
    file->arg.openfilefast.id=id; return TRUE;
}
BOOL FS_OpenFileDirect(FSFile *file,FSArchive *archive,u32 top,u32 bottom,u32 index) {
    if (!FS_IsAvailable() || archive!=&fsi_arc_rom) return FALSE;
    if (!HGNativeFileRequest(HG_FILE_OPEN_RANGE,(u32)file,index,top,bottom)) return FALSE;
    file->arc=archive; file->command=FS_COMMAND_OPENFILEDIRECT;
    file->arg.openfiledirect=(FSOpenFileDirectInfo){top,bottom,index}; return TRUE;
}
BOOL FS_OpenFile(FSFile *file,const char *path) {
    FSFileID id; return FS_ConvertPathToFileID(&id,path) && FS_OpenFileFast(file,id);
}
BOOL FS_CloseFile(FSFile *file) {
    if (HGNativeIsOverlayFile(file)) return HGNativeCloseOverlayFile(file);
    BOOL result=HGNativeFileRequest(HG_FILE_CLOSE,(u32)file,0,0,0);
    if (result) { file->arc=NULL; file->command=FS_COMMAND_INVALID; }
    return result;
}
int FSi_ReadFileCore(FSFile *file,void *destination,s32 length,BOOL async) {
    u32 original=(u32)length;
    if (length<0) length=0;
    u32 remaining=file->prop.file.bottom-file->prop.file.pos;
    if ((u32)length>remaining) length=(s32)remaining;
    file->command=FS_COMMAND_READFILE;
    file->arg.readfile=(FSReadFileInfo){destination,original,(u32)length};
    return (s32)HGNativeFileRequest(async ? HG_FILE_READ_ASYNC : HG_FILE_READ,(u32)file,
                                  (u32)destination,(u32)length,0);
}
int FS_ReadFile(FSFile *file,void *destination,s32 length) { return FSi_ReadFileCore(file,destination,length,FALSE); }
int FS_ReadFileAsync(FSFile *file,void *destination,s32 length) { return FSi_ReadFileCore(file,destination,length,TRUE); }
BOOL FS_WaitAsync(FSFile *file) { return HGNativeIsOverlayFile(file) ? file->error==FS_RESULT_SUCCESS : HGNativeFileRequest(HG_FILE_WAIT,(u32)file,0,0,0); }
BOOL FS_SeekFile(FSFile *file,s32 offset,FSSeekFileMode mode) {
    return HGNativeFileRequest(HG_FILE_SEEK,(u32)file,(u32)offset,mode,0);
}
BOOL FS_ChangeDir(const char *path) { return FS_IsAvailable() && HGNativeFileRequest(HG_FILE_CHANGE_DIR,0,(u32)path,0,0); }
BOOL FSi_FindPath(FSFile *directory,const char *path,FSFileID *id,FSDirPos *position) {
    if (!FS_IsAvailable() || (!position && !id)) return FALSE;
    if (!directory) return !position && FS_ConvertPathToFileID(id,path);
    directory->arc=&fsi_arc_rom;
    directory->arg.findpath.pos=(FSDirPos){0}; // A null base selects the instance's working directory.
    directory->arg.findpath.path=path;
    directory->arg.findpath.find_directory=position!=NULL;
    if (position) directory->arg.findpath.result.dir=position;
    else directory->arg.findpath.result.file=id;
    return FSi_FindPathCommand(directory)==FS_RESULT_SUCCESS;
}
#else
extern FSDirPos current_dir_pos;
BOOL is_init = FALSE;

BOOL FSi_FindPath(FSFile * p_dir, const char * path, FSFileID * p_file_id, FSDirPos * p_dir_pos);

void FS_Init(u32 default_dma_no)
{
    if (!is_init)
    {
        is_init = TRUE;
        FSi_InitRom(default_dma_no);
    }
}

BOOL FS_IsAvailable(void)
{
    return is_init;
}

void FS_InitFile(FSFile * p_file)
{
    p_file->link.next = p_file->link.prev = NULL;
    OS_InitThreadQueue(p_file->queue);
    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat = 0;
}

BOOL FSi_FindPath(FSFile * p_dir, const char * path, FSFileID * p_file_id, FSDirPos * p_dir_pos)
{
    FSDirPos pos;
    if (FSi_IsSlash(MI_ReadByte(path)))
    {
        pos.arc = current_dir_pos.arc;
        pos.own_id = 0;
        pos.pos = 0;
        pos.index = 0;
        path++;
    }
    else
    {
        int i;
        pos = current_dir_pos;
        for (i = 0; i <= FS_ARCHIVE_NAME_LEN_MAX; ++i)
        {
            u32 c = MI_ReadByte(path + i);
            if (!c || FSi_IsSlash(c))
                break;
            else if (c == ':')
            {
                FSArchive * const p_arc = FS_FindArchive(path, i);
                if (!p_arc)
                {
                    return FALSE;
                }
                else if (!FS_IsArchiveLoaded(p_arc))
                {
                    return FALSE;
                }
                pos.arc = p_arc;
                pos.pos = 0;
                pos.index = 0;
                pos.own_id = 0;
                path += i + 1;
                if (FSi_IsSlash(MI_ReadByte(path)))
                    ++path;
                break;
            }
        }
    }
    p_dir->arc = pos.arc;
    p_dir->arg.findpath.path = path;
    p_dir->arg.findpath.pos = pos;
    if (p_dir_pos)
    {
        p_dir->arg.findpath.find_directory = TRUE;
        p_dir->arg.findpath.result.dir = p_dir_pos;
    }
    else
    {
        p_dir->arg.findpath.find_directory = FALSE;
        p_dir->arg.findpath.result.file = p_file_id;
    }
    return FSi_SendCommand(p_dir, FS_COMMAND_FINDPATH);
}

int FSi_ReadFileCore(FSFile * p_file, void * dst, s32 len, BOOL async)
{
    const s32 pos = (s32)p_file->prop.file.pos;
    const s32 rest = (s32)(p_file->prop.file.bottom - pos);
    const u32 org = (u32)len;
    if (len > rest)
        len = rest;
    if (len < 0)
        len = 0;
    p_file->arg.readfile.dst = dst;
    p_file->arg.readfile.len_org = org;
    p_file->arg.readfile.len = (u32)len;
    if (!async)
        p_file->stat |= FS_FILE_STATUS_SYNC;
    (void)FSi_SendCommand(p_file, FS_COMMAND_READFILE);
    if (!async)
    {
        if (FS_WaitAsync(p_file))
            len = (s32)(p_file->prop.file.pos - pos);
        else
            len = -1;
    }
    return len;
}

BOOL FS_ConvertPathToFileID(FSFileID * p_file_id, const char * path)
{
    FSFile dir;
    FS_InitFile(&dir);
    if (!FSi_FindPath(&dir, path, p_file_id, NULL))
        return FALSE;
    return TRUE;
}

BOOL FS_OpenFileDirect(FSFile * p_file, FSArchive * p_arc, u32 image_top, u32 image_bottom, u32 file_index)
{
    p_file->arc = p_arc;
    p_file->arg.openfiledirect.index = file_index;
    p_file->arg.openfiledirect.top = image_top;
    p_file->arg.openfiledirect.bottom = image_bottom;
    if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEDIRECT))
        return FALSE;
    p_file->stat |= FS_FILE_STATUS_IS_FILE;
    p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
    return TRUE;
}

BOOL FS_OpenFileFast(FSFile * p_file, FSFileID file_id)
{
    if (!file_id.arc)
        return FALSE;
    p_file->arc = file_id.arc;
    p_file->arg.openfilefast.id = file_id;
    if (!FSi_SendCommand(p_file, FS_COMMAND_OPENFILEFAST))
        return FALSE;
    p_file->stat |= FS_FILE_STATUS_IS_FILE;
    p_file->stat &= ~FS_FILE_STATUS_IS_DIR;
    return TRUE;
}

BOOL FS_OpenFile(FSFile * p_file, const char * path)
{
    FSFileID file_id;
    return FS_ConvertPathToFileID(&file_id, path) && FS_OpenFileFast(p_file, file_id);
}

BOOL FS_CloseFile(FSFile * p_file)
{
    if (!FSi_SendCommand(p_file, FS_COMMAND_CLOSEFILE))
        return FALSE;
    p_file->arc = NULL;
    p_file->command = FS_COMMAND_INVALID;
    p_file->stat &= ~(FS_FILE_STATUS_IS_FILE | FS_FILE_STATUS_IS_DIR);
    return TRUE;
}

BOOL FS_WaitAsync(FSFile * p_file)
{
    BOOL is_owner = FALSE;
    OSIntrMode bak_par = OS_DisableInterrupts();
    if (FS_IsBusy(p_file))
    {
        is_owner = !(p_file->stat & (FS_FILE_STATUS_SYNC | FS_FILE_STATUS_OPERATING));
        if (is_owner)
        {
            p_file->stat |= FS_FILE_STATUS_SYNC;
            do
            {
                OS_SleepThread(p_file->queue);
            } while (!(p_file->stat & FS_FILE_STATUS_OPERATING));
        }
        else
        {
            do
            {
                OS_SleepThread(p_file->queue);
            } while (FS_IsBusy(p_file));
        }
    }
    (void)OS_RestoreInterrupts(bak_par);
    if (is_owner)
    {
        return FSi_ExecuteSyncCommand(p_file);
    }

    return FS_IsSucceeded(p_file);
}

int FS_ReadFileAsync(FSFile * p_file, void * dst, s32 len)
{
    return FSi_ReadFileCore(p_file, dst, len, TRUE);
}

int FS_ReadFile(FSFile * p_file, void * dst, s32 len)
{
    return FSi_ReadFileCore(p_file, dst, len, FALSE);
}

BOOL FS_SeekFile(FSFile * p_file, s32 offset, FSSeekFileMode origin)
{
    switch (origin)
    {
    case FS_SEEK_SET:
        offset += p_file->prop.file.top;
        break;
    case FS_SEEK_CUR:
        offset += p_file->prop.file.pos;
        break;
    case FS_SEEK_END:
        offset += p_file->prop.file.bottom;
        break;
    default:
        return FALSE;
    }
    if (offset < (s32)p_file->prop.file.top)
        offset = (s32)p_file->prop.file.top;
    if (offset > (s32)p_file->prop.file.bottom)
        offset = (s32)p_file->prop.file.bottom;
    p_file->prop.file.pos = (u32)offset;
    return TRUE;
}

BOOL FS_ChangeDir(const char * path)
{
    FSDirPos pos;
    FSFile dir;
    FS_InitFile(&dir);
    if (!FSi_FindPath(&dir, path, NULL, &pos))
        return FALSE;
    current_dir_pos = pos;
    return TRUE;
}
#endif
