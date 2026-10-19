// SPDX-License-Identifier: GPL-3.0-or-later
#include "filesystem.h"
#include <nitro/fs/native_file.h>
#include <limits.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)

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
    return 0;
}
