// SPDX-License-Identifier: GPL-3.0-or-later
#include "filesystem.h"
#include <nitro/fs/native_file.h>
#include <limits.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
BOOL FSi_FindPath(FSFile *,const char *,FSFileID *,FSDirPos *);
FSResult FSi_SeekDirCommand(FSFile *);
FSResult FSi_SeekDirDirect(FSFile *,u32);
FSResult FSi_ReadDirCommand(FSFile *);
FSResult FSi_FindPathCommand(FSFile *);
FSResult FSi_GetPathCommand(FSFile *);

static struct { u32 before; FSFile file; u32 after; } held;
static u8 output[8192];
static const char binaryPath[]="rom:/sub/binary.dat";
static const char directoryPath[]="rom:/sub",relativePath[]="binary.dat";
u32 HGFileReference(u32 field) {
    return field==0 ? (u32)&held.file : field==1 ? (u32)output : field==2 ? (u32)binaryPath :
           field==3 ? (u32)directoryPath : (u32)relativePath;
}
u32 HGFileOpenHeld(void) { FS_InitFile(&held.file); return FS_OpenFile(&held.file,binaryPath); }
u32 HGFileReadSample(u32 member,u32 mode) {
    NARC *narc=NARC_New(NARC_a_0_0_0,HEAP_ID_DEFAULT);
    if (!narc) return UINT32_MAX;
    if (!mode) { u32 count=NARC_GetFileCount(narc); NARC_Delete(narc); return count; }
    u32 size=NARC_GetMemberSize(narc,member);
    if (size>sizeof(output)) { NARC_Delete(narc); return UINT32_MAX; }
    void *data=NARC_AllocAndReadWholeMember(narc,member,HEAP_ID_DEFAULT);
    if (!data) { NARC_Delete(narc); return UINT32_MAX; }
    memcpy(output,data,size); Heap_Free(data); NARC_Delete(narc); return size;
}
static u32 CheckDirectories(void) {
    FSFile directory,file;
    FSDirPos root,sub;
    struct { u32 before; FSDirEntry value; u32 after; } entry;
    FS_InitFile(&directory); FS_InitFile(&file);
    CHECK(FSi_FindPath(&directory,"rom:/",NULL,&root));
    CHECK(root.arc==FS_FindArchive("rom",3) && root.own_id==0 && !root.index && !root.pos);
    CHECK(FS_IsDir(&directory) && !FS_OpenFile(&directory,binaryPath));
    entry.before=0x12345678; entry.after=0x87654321;
    memset(&entry.value,0xA5,sizeof(entry.value));
    directory.arg.readdir=(FSReadDirInfo){&entry.value,TRUE};
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_SUCCESS);
    CHECK(entry.value.is_directory && entry.value.name_len==1 && (u8)entry.value.name[0]==0xA5);
    FSDirPos saved=directory.prop.dir.pos;
    directory.arg.readdir.skip_string=FALSE;
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_SUCCESS && !strcmp(entry.value.name,"sub"));
    sub=entry.value.dir_id;
    CHECK(sub.arc==root.arc && sub.own_id && !sub.index && !sub.pos);
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_FAILURE);
    directory.arg.seekdir.pos=saved;
    CHECK(FSi_SeekDirCommand(&directory)==FS_RESULT_SUCCESS);
    directory.arg.readdir=(FSReadDirInfo){&entry.value,FALSE};
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_SUCCESS && !strcmp(entry.value.name,"sub"));
    directory.arg.seekdir.pos=(FSDirPos){root.arc,0,1,0};
    CHECK(FSi_SeekDirCommand(&directory)==FS_RESULT_FAILURE);
    directory.arg.seekdir.pos=sub;
    CHECK(FSi_SeekDirCommand(&directory)==FS_RESULT_SUCCESS && directory.prop.dir.parent==0);
    directory.arg.getpath=(FSGetPathInfo){NULL,0,0,0};
    CHECK(FSi_GetPathCommand(&directory)==FS_RESULT_SUCCESS && directory.arg.getpath.total_len==10);
    memset(output,0xA5,sizeof(output));
    directory.arg.getpath.buf=output+1; directory.arg.getpath.buf_len=9;
    CHECK(FSi_GetPathCommand(&directory)==FS_RESULT_FAILURE && output[1]==0xA5);
    directory.arg.getpath.buf_len=10;
    CHECK(FSi_GetPathCommand(&directory)==FS_RESULT_SUCCESS && !strcmp((char *)output+1,"rom:/sub/"));
    CHECK(output[0]==0xA5 && output[11]==0xA5);
    directory.arg.readdir=(FSReadDirInfo){&entry.value,FALSE};
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_SUCCESS && !strcmp(entry.value.name,"binary.dat"));
    CHECK(!entry.value.is_directory && FS_OpenFileFast(&file,entry.value.file_id));
    file.arg.getpath=(FSGetPathInfo){output,sizeof(output),0,0};
    CHECK(FSi_GetPathCommand(&file)==FS_RESULT_SUCCESS && !strcmp((char *)output,binaryPath));
    CHECK(file.arg.getpath.total_len==sizeof(binaryPath) && file.arg.getpath.dir_id==sub.own_id);
    CHECK(FSi_SeekDirDirect(&file,0)==FS_RESULT_FAILURE && FS_CloseFile(&file));
    directory.arg.findpath=(FSFindPathInfo){sub,"./binary.dat",FALSE,{.file=&entry.value.file_id}};
    CHECK(FSi_FindPathCommand(&directory)==FS_RESULT_SUCCESS && FS_OpenFileFast(&file,entry.value.file_id));
    CHECK(FS_ReadFile(&file,output,1)==1 && output[0]==7 && FS_CloseFile(&file));
    directory.arg.findpath=(FSFindPathInfo){sub,"..",TRUE,{.dir=&saved}};
    CHECK(FSi_FindPathCommand(&directory)==FS_RESULT_SUCCESS && saved.own_id==0);
    CHECK(FSi_FindPath(&directory,"rom:/a/empty",NULL,&saved));
    directory.arg.readdir=(FSReadDirInfo){&entry.value,FALSE};
    CHECK(FSi_ReadDirCommand(&directory)==FS_RESULT_FAILURE);
    CHECK(FS_CloseFile(&directory) && !FS_CloseFile(&directory));
    FS_InitFile(&directory);
    CHECK(!FSi_FindPath(&directory,binaryPath,NULL,&saved));
    CHECK(!FSi_FindPath(&directory,"rom:/sub",&entry.value.file_id,NULL));
    CHECK(!FSi_FindPath(&directory,"rom:/../../etc",NULL,&saved));
    CHECK(!FSi_FindPath(&directory,"dwc:/sub",NULL,&saved));
    CHECK(FS_ChangeDir("rom:/sub") && FSi_FindPath(&directory,".",NULL,&saved) && saved.own_id==sub.own_id);
    CHECK(FS_CloseFile(&directory) && FS_ChangeDir("rom:/"));
    CHECK(entry.before==0x12345678 && entry.after==0x87654321);
    return 0;
}
u32 CheckOriginalBridgedFiles(void) {
    held.before=0x12345678; held.after=0x87654321;
    FS_Init(1); FS_Init(2);
    CHECK(FS_IsAvailable() && FS_FindArchive("rom",3));
    CHECK(FS_SetDefaultDMA(2)==1 && FS_SetDefaultDMA(1)==2);
    CHECK(FS_TryLoadTable(NULL,0)==0);
    FS_InitFile(&held.file);
    CHECK(held.file.command==FS_COMMAND_INVALID && !FS_IsBusy(&held.file));
    CHECK(FS_OpenFile(&held.file,binaryPath) && FS_GetLength(&held.file)==64);
    CHECK(FS_IsFileSyncMode(&held.file)==FALSE && FS_GetFileImageTop(&held.file)==0);
    CHECK(!FS_OpenFile(&held.file,binaryPath));
    memset(output,0xA5,sizeof(output));
    CHECK(FS_ReadFile(&held.file,output+2,7)==7 && held.file.prop.file.pos==7);
    for (u32 i=0;i<7;i++) CHECK(output[2+i]==(u8)(i*3+7));
    CHECK(output[1]==0xA5 && output[9]==0xA5);
    CHECK(FS_SeekFile(&held.file,5,FS_SEEK_CUR) && held.file.prop.file.pos==12);
    CHECK(FS_ReadFileAsync(&held.file,output,8)==8 && FS_WaitAsync(&held.file) && !FS_IsBusy(&held.file));
    for (u32 i=0;i<8;i++) CHECK(output[i]==(u8)((i+12)*3+7));
    CHECK(FS_SeekFile(&held.file,-2,FS_SEEK_END));
    CHECK(FS_ReadFile(&held.file,output,10)==2 && held.file.prop.file.pos==64);
    CHECK(output[0]==(u8)(62*3+7) && output[1]==(u8)(63*3+7));
    CHECK(FS_ReadFile(&held.file,NULL,-1)==0);
    CHECK(FS_SeekFile(&held.file,INT32_MIN,FS_SEEK_SET) && held.file.prop.file.pos==0);
    CHECK(FS_SeekFile(&held.file,INT32_MAX,FS_SEEK_END) && held.file.prop.file.pos==64);
    FSFileID id,again;
    CHECK(FS_ConvertPathToFileID(&id,binaryPath) && FS_ConvertPathToFileID(&again,binaryPath));
    CHECK(id.arc==again.arc && id.file_id==again.file_id);
    CHECK(FS_CloseFile(&held.file) && !FS_CloseFile(&held.file) && !FS_WaitAsync(&held.file));
    CHECK(FS_OpenFileFast(&held.file,id) && FS_CloseFile(&held.file));
    CHECK(FS_OpenFileDirect(&held.file,id.arc,10,20,id.file_id));
    CHECK(FS_GetLength(&held.file)==10 && FS_GetFileImageTop(&held.file)==10);
    CHECK(FS_ReadFile(&held.file,output,12)==10);
    for (u32 i=0;i<10;i++) CHECK(output[i]==(u8)((i+10)*3+7));
    CHECK(FS_SeekFile(&held.file,-1,FS_SEEK_SET) && held.file.prop.file.pos==10);
    CHECK(FS_SeekFile(&held.file,INT32_MIN,FS_SEEK_CUR) && held.file.prop.file.pos==10);
    CHECK(FS_CloseFile(&held.file));
    CHECK(!FS_OpenFileDirect(&held.file,id.arc,20,10,id.file_id));
    CHECK(!FS_OpenFileDirect(&held.file,id.arc,0,65,id.file_id));
    CHECK(FS_ChangeDir("rom:/sub"));
    CHECK(FS_ConvertPathToFileID(&again,"binary.dat") && again.file_id==id.file_id);
    CHECK(FS_OpenFile(&held.file,"binary.dat") && FS_CloseFile(&held.file));
    CHECK(FS_OpenFile(&held.file,"/sub/binary.dat") && FS_CloseFile(&held.file));
    CHECK(!FS_ChangeDir("rom:/missing") && FS_ChangeDir("rom:/"));
    CHECK(!FS_OpenFile(&held.file,"missing.dat") && !FS_OpenFile(&held.file,"../escape.dat"));
    CHECK(!FS_OpenFile(&held.file,"dwc:/missing.dat"));
    CHECK(!FS_OpenFile(&held.file,"escape.dat"));
    CHECK(held.before==0x12345678 && held.after==0x87654321);
    return CheckDirectories();
}
