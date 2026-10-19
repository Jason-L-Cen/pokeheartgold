// SPDX-License-Identifier: GPL-3.0-or-later
#include "NativeImports.h"
#include "../NativeFile.h"
#include <nitro/fs/file.h>
#include <nitro/fs/native_file.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct PathEntry { struct PathEntry *next; u32 id; char path[PATH_MAX]; } PathEntry;
typedef struct FileEntry { struct FileEntry *next; u32 address; FSFile file; } FileEntry;
typedef struct FileContext {
    struct FileContext *next;
    struct w2c_env *env;
    PathEntry *paths;
    FileEntry *files;
    u32 nextID;
    char directory[PATH_MAX];
} FileContext;
static FileContext *contexts;
static pthread_mutex_t fileMutex=PTHREAD_MUTEX_INITIALIZER;

static void *memorySpan(struct w2c_env *env,u32 offset,u32 size) {
    if (!offset || offset>env->memory->size || size>env->memory->size-offset) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return env->memory->data+offset;
}
static const char *pathString(struct w2c_env *env,u32 offset) {
    const char *text=memorySpan(env,offset,1);
    u64 remaining=env->memory->size-offset;
    if (remaining>PATH_MAX) remaining=PATH_MAX;
    if (!memchr(text,0,(size_t)remaining)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    return text;
}
static FileContext *contextFor(struct w2c_env *env,BOOL create) {
    for (FileContext *context=contexts;context;context=context->next) if (context->env==env) return context;
    if (!create) return NULL;
    FileContext *context=calloc(1,sizeof(*context));
    if (context) { context->env=env; context->nextID=1; context->next=contexts; contexts=context; }
    return context;
}
static BOOL normalize(FileContext *context,const char *path,BOOL directory,char result[PATH_MAX]) {
    char joined[PATH_MAX];
    if (!strncmp(path,"rom:",4)) { path+=4; if (*path=='/') path++; }
    else if (*path=='/') path++;
    else if (context->directory[0]) {
        int size=snprintf(joined,sizeof(joined),"%s/%s",context->directory,path);
        if (size<0 || size>=sizeof(joined)) return FALSE;
        path=joined;
    }
    return HGNativeFSNormalizePath(path,directory,result,PATH_MAX);
}
static PathEntry *pathForID(FileContext *context,u32 id) {
    for (PathEntry *path=context->paths;path;path=path->next) if (path->id==id) return path;
    return NULL;
}
static void store32(u8 *file,u32 offset,u32 value) { memcpy(file+offset,&value,4); }
static void mirror(u8 *guest,const FSFile *file) {
    store32(guest,12,file->stat); store32(guest,20,file->error);
    store32(guest,32,file->prop.file.own_id); store32(guest,36,file->prop.file.top);
    store32(guest,40,file->prop.file.bottom); store32(guest,44,file->prop.file.pos);
}
u32 w2c_env_HGNativeFileRequest(struct w2c_env *env,u32 command,u32 address,u32 first,u32 second,u32 third) {
    if (!env || !env->memory || !env->memory->data || command>HG_FILE_CHANGE_DIR) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_FILE_AVAILABLE) {
        if (address || first || second || third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        return FS_IsAvailable();
    }
    BOOL pathOperation=command==HG_FILE_RESOLVE || command==HG_FILE_CHANGE_DIR;
    const char *text=NULL;
    u8 *guest=NULL,*output=NULL;
    if (pathOperation) {
        if (address || second || third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        text=pathString(env,first);
    } else {
        if (address%4) wasm_rt_trap(WASM_RT_TRAP_OOB);
        guest=memorySpan(env,address,0x48);
    }
    if (command==HG_FILE_READ || command==HG_FILE_READ_ASYNC) {
        if (second>INT32_MAX || third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (second) output=memorySpan(env,first,second);
    } else if ((command==HG_FILE_INIT || command==HG_FILE_CLOSE || command==HG_FILE_WAIT) &&
               (first || second || third)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    else if (command==HG_FILE_OPEN && (second || third)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    else if (command==HG_FILE_SEEK && (second>FS_SEEK_END || third)) wasm_rt_trap(WASM_RT_TRAP_OOB);

    pthread_mutex_lock(&fileMutex);
    FileContext *context=contextFor(env,pathOperation || command==HG_FILE_OPEN || command==HG_FILE_OPEN_RANGE);
    u32 result=0;
    if (pathOperation) {
        char normalized[PATH_MAX];
        if (!context || !normalize(context,text,command==HG_FILE_CHANGE_DIR,normalized)) goto finished;
        if (command==HG_FILE_CHANGE_DIR) { memcpy(context->directory,normalized,strlen(normalized)+1); result=TRUE; goto finished; }
        for (PathEntry *path=context->paths;path;path=path->next)
            if (!strcmp(path->path,normalized)) { result=path->id; goto finished; }
        if (context->nextID==UINT32_MAX) goto finished;
        PathEntry *path=malloc(sizeof(*path)); if (!path) goto finished;
        path->id=context->nextID++; memcpy(path->path,normalized,strlen(normalized)+1);
        path->next=context->paths; context->paths=path; result=path->id; goto finished;
    }
    FileEntry **link=context ? &context->files : NULL;
    while (link && *link && (*link)->address!=address) link=&(*link)->next;
    FileEntry *entry=link ? *link : NULL;
    if (command==HG_FILE_INIT) {
        if (entry) { pthread_mutex_unlock(&fileMutex); wasm_rt_trap(WASM_RT_TRAP_OOB); }
        memset(guest,0,0x48); store32(guest,16,FS_COMMAND_INVALID); result=TRUE; goto finished;
    }
    if (command==HG_FILE_OPEN || command==HG_FILE_OPEN_RANGE) {
        PathEntry *path=context ? pathForID(context,first) : NULL;
        if (entry || !path) goto finished;
        entry=calloc(1,sizeof(*entry)); if (!entry) goto finished;
        FS_InitFile(&entry->file);
        if (!FS_OpenFile(&entry->file,path->path)) { free(entry); goto finished; }
        if (command==HG_FILE_OPEN_RANGE) {
            if (second>third || third>FS_GetLength(&entry->file)) { FS_CloseFile(&entry->file); free(entry); goto finished; }
            entry->file.prop.file.top=second; entry->file.prop.file.bottom=third;
            if (!FS_SeekFile(&entry->file,0,FS_SEEK_SET)) { FS_CloseFile(&entry->file); free(entry); goto finished; }
        }
        entry->address=address; entry->file.prop.file.own_id=path->id;
        entry->next=context->files; context->files=entry; mirror(guest,&entry->file); result=TRUE; goto finished;
    }
    if (!entry) { result=(command==HG_FILE_READ || command==HG_FILE_READ_ASYNC) ? UINT32_MAX : FALSE; goto finished; }
    if (command==HG_FILE_CLOSE) {
        result=FS_CloseFile(&entry->file); mirror(guest,&entry->file);
        *link=entry->next; free(entry);
    } else if (command==HG_FILE_READ || command==HG_FILE_READ_ASYNC) {
        result=(u32)FSi_ReadFileCore(&entry->file,output,(s32)second,command==HG_FILE_READ_ASYNC);
        mirror(guest,&entry->file);
    } else if (command==HG_FILE_SEEK) {
        result=FS_SeekFile(&entry->file,(s32)first,(FSSeekFileMode)second); mirror(guest,&entry->file);
    } else if (command==HG_FILE_WAIT) result=FS_WaitAsync(&entry->file);
finished:
    pthread_mutex_unlock(&fileMutex);
    return result;
}
void HGNativeCloseFileContext(struct w2c_env *env) {
    pthread_mutex_lock(&fileMutex);
    FileContext **link=&contexts;
    while (*link && (*link)->env!=env) link=&(*link)->next;
    if (*link) {
        FileContext *context=*link; *link=context->next;
        while (context->files) { FileEntry *file=context->files; context->files=file->next; FS_CloseFile(&file->file); free(file); }
        while (context->paths) { PathEntry *path=context->paths; context->paths=path->next; free(path); }
        free(context);
    }
    pthread_mutex_unlock(&fileMutex);
}
