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

// shortcut: IDs/cursors describe native snapshots; import ROM FNT IDs/order before supporting raw cartridge positions.
typedef struct PathEntry {
    struct PathEntry *next;
    u32 id;
    BOOL directory;
    HGNativeDirEntry *entries;
    u32 count;
    char path[PATH_MAX];
} PathEntry;
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
    if (context) {
        context->paths=calloc(1,sizeof(*context->paths));
        if (!context->paths) { free(context); return NULL; }
        context->paths->directory=TRUE; // Native directory zero is the content root.
        context->env=env; context->nextID=1; context->next=contexts; contexts=context;
    }
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
static PathEntry *rememberPath(FileContext *context,const char *text,BOOL directory) {
    char normalized[PATH_MAX];
    if (!HGNativeFSNormalizePath(text,directory,normalized,sizeof(normalized))) return NULL;
    for (PathEntry *path=context->paths;path;path=path->next)
        if (path->directory==directory && !strcmp(path->path,normalized)) return path;
    if (context->nextID>=UINT16_MAX) return NULL;
    PathEntry *path=calloc(1,sizeof(*path)); if (!path) return NULL;
    path->id=context->nextID++; path->directory=directory;
    memcpy(path->path,normalized,strlen(normalized)+1);
    path->next=context->paths; context->paths=path; return path;
}
static PathEntry *parentOf(FileContext *context,const PathEntry *path) {
    char parent[PATH_MAX]; strcpy(parent,path->path);
    char *slash=strrchr(parent,'/'); if (slash) *slash=0; else parent[0]=0;
    return rememberPath(context,parent,TRUE);
}
static u32 load32(const u8 *data,u32 offset) { u32 value; memcpy(&value,data+offset,4); return value; }
static u16 load16(const u8 *data,u32 offset) { u16 value; memcpy(&value,data+offset,2); return value; }
static void store16(u8 *data,u32 offset,u16 value) { memcpy(data+offset,&value,2); }
static void store32(u8 *file,u32 offset,u32 value) { memcpy(file+offset,&value,4); }
static void mirror(u8 *guest,const FSFile *file) {
    store32(guest,12,file->stat); store32(guest,20,file->error);
    store32(guest,32,file->prop.file.own_id); store32(guest,36,file->prop.file.top);
    store32(guest,40,file->prop.file.bottom); store32(guest,44,file->prop.file.pos);
}
u32 w2c_env_HGNativeFileRequest(struct w2c_env *env,u32 command,u32 address,u32 first,u32 second,u32 third) {
    if (!env || !env->memory || !env->memory->data || command>HG_FILE_GET_PATH) wasm_rt_trap(WASM_RT_TRAP_OOB);
    if (command==HG_FILE_AVAILABLE) {
        if (address || first || second || third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        return FS_IsAvailable();
    }
    BOOL pathOperation=command==HG_FILE_RESOLVE || command==HG_FILE_CHANGE_DIR || command==HG_FILE_RESOLVE_AT;
    const char *text=NULL;
    u8 *guest=NULL,*output=NULL;
    if (pathOperation) {
        if (address || (command==HG_FILE_RESOLVE_AT ? third>1 : second || third)) wasm_rt_trap(WASM_RT_TRAP_OOB);
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
    else if (command==HG_FILE_SEEK_DIR && (first>UINT16_MAX || second>UINT16_MAX)) wasm_rt_trap(WASM_RT_TRAP_OOB);
    else if (command==HG_FILE_READ_DIR) {
        if (first%4 || second>1 || third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        output=memorySpan(env,first,148);
    } else if (command==HG_FILE_GET_PATH) {
        if (third) wasm_rt_trap(WASM_RT_TRAP_OOB);
        if (first) output=memorySpan(env,first,second);
        else if (second) wasm_rt_trap(WASM_RT_TRAP_OOB);
    }

    pthread_mutex_lock(&fileMutex);
    FileContext *context=contextFor(env,pathOperation || command==HG_FILE_OPEN || command==HG_FILE_OPEN_RANGE || command==HG_FILE_SEEK_DIR);
    u32 result=0;
    if (pathOperation) {
        char normalized[PATH_MAX],joined[PATH_MAX];
        BOOL directory=command==HG_FILE_CHANGE_DIR || (command==HG_FILE_RESOLVE_AT && third);
        if (!context) goto finished;
        if (command==HG_FILE_RESOLVE_AT && second!=UINT32_MAX && *text!='/' && strncmp(text,"rom:",4)) {
            PathEntry *base=pathForID(context,second);
            if (!base || !base->directory) goto finished;
            int length=snprintf(joined,sizeof(joined),"rom:/%s%s%s",base->path,*base->path ? "/" : "",text);
            if (length<0 || length>=sizeof(joined)) goto finished;
            text=joined;
        }
        if (!normalize(context,text,directory,normalized)) goto finished;
        if (command==HG_FILE_CHANGE_DIR) { memcpy(context->directory,normalized,strlen(normalized)+1); result=TRUE; goto finished; }
        PathEntry *path=rememberPath(context,normalized,directory);
        if (path) result=path->id+(directory ? 1 : 0);
        goto finished;
    }
    FileEntry **link=context ? &context->files : NULL;
    while (link && *link && (*link)->address!=address) link=&(*link)->next;
    FileEntry *entry=link ? *link : NULL;
    if (command==HG_FILE_INIT) {
        if (entry) { pthread_mutex_unlock(&fileMutex); wasm_rt_trap(WASM_RT_TRAP_OOB); }
        memset(guest,0,0x48); store32(guest,16,FS_COMMAND_INVALID); result=TRUE; goto finished;
    }
    if (command==HG_FILE_SEEK_DIR) {
        PathEntry *path=context ? pathForID(context,first) : NULL;
        if (entry || !path || !path->directory) goto finished;
        if (!path->entries && !HGNativeFSReadDirectory(path->path,&path->entries,&path->count)) goto finished;
        if (second!=third || third>path->count) goto finished;
        PathEntry *parent=parentOf(context,path); if (!parent) goto finished;
        store32(guest,12,FS_FILE_STATUS_IS_DIR|FS_FILE_STATUS_SYNC); store32(guest,20,FS_RESULT_SUCCESS);
        store16(guest,36,(u16)path->id); store16(guest,38,(u16)second); store32(guest,40,third);
        store32(guest,44,parent->id); result=TRUE; goto finished;
    }
    if (command==HG_FILE_READ_DIR) {
        PathEntry *path=context ? pathForID(context,load16(guest,36)) : NULL;
        u32 cursor=load32(guest,40);
        if (entry || !(load32(guest,12)&FS_FILE_STATUS_IS_DIR) || !path || !path->entries ||
            cursor!=load16(guest,38) || cursor>=path->count) goto finished;
        const HGNativeDirEntry *child=&path->entries[cursor];
        char joined[PATH_MAX];
        int length=snprintf(joined,sizeof(joined),"%s%s%s",path->path,*path->path ? "/" : "",child->name);
        if (length<0 || length>=sizeof(joined)) goto finished;
        PathEntry *id=rememberPath(context,joined,child->directory); if (!id) goto finished;
        store32(output,0,0);
        if (child->directory) { store16(output,4,(u16)id->id); store16(output,6,0); store32(output,8,0); }
        else store32(output,4,id->id);
        store32(output,12,child->directory); store32(output,16,(u32)strlen(child->name));
        if (!second) strcpy((char *)output+20,child->name);
        store16(guest,38,(u16)(cursor+1)); store32(guest,40,cursor+1); result=TRUE; goto finished;
    }
    if (command==HG_FILE_GET_PATH) {
        PathEntry *path=NULL;
        if (entry) path=pathForID(context,entry->file.prop.file.own_id);
        else if (context && (load32(guest,12)&FS_FILE_STATUS_IS_DIR)) path=pathForID(context,load16(guest,36));
        if (!path) goto finished;
        PathEntry *parent=parentOf(context,path); if (!parent) goto finished;
        u32 length=5+(u32)strlen(path->path)+(path->directory && *path->path ? 1 : 0)+1;
        store16(guest,56,(u16)length); store16(guest,58,(u16)parent->id);
        if (!output) { result=TRUE; goto finished; }
        if (second<length) goto finished;
        snprintf((char *)output,length,"rom:/%s%s",path->path,path->directory && *path->path ? "/" : "");
        result=TRUE; goto finished;
    }
    if (command==HG_FILE_CLOSE && !entry && (load32(guest,12)&FS_FILE_STATUS_IS_DIR)) {
        store32(guest,12,0); store32(guest,20,FS_RESULT_SUCCESS); result=TRUE; goto finished;
    }
    if (command==HG_FILE_OPEN || command==HG_FILE_OPEN_RANGE) {
        PathEntry *path=context ? pathForID(context,first) : NULL;
        if (entry || !path || path->directory || (load32(guest,12)&FS_FILE_STATUS_IS_DIR)) goto finished;
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
        while (context->paths) { PathEntry *path=context->paths; context->paths=path->next; HGNativeFSFreeDirectory(path->entries); free(path); }
        free(context);
    }
    pthread_mutex_unlock(&fileMutex);
}
