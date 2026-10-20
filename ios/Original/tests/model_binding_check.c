// SPDX-License-Identifier: GPL-3.0-or-later
#include "../../../src/render_model.c"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(v) do { if (!(v)) { fprintf(stderr,"Model binding line %d: %s\n",__LINE__,#v); abort(); } } while (0)
static RenderModel *expected;
static u32 uploads,bindings,queued;
void HGNativeAssertFail(const char *condition,const char *file,int line) { fprintf(stderr,"%s:%d: %s\n",file,line,condition); abort(); }
// Probe-only upload/binding observations; the full engine uses the real native graphics backend.
BOOL GF3dRender_AllocAndLoadTexResources(NNSG3dResTex *texture) { CHECK(texture==expected->texture); uploads++; return TRUE; }
BOOL NNS_G3dBindMdlSet(NNSG3dResMdlSet *set,const NNSG3dResTex *texture) { CHECK(set==expected->set && texture==expected->texture && uploads==bindings+1); bindings++; return TRUE; }
SysTask *SysTask_CreateOnVBlankQueue(SysTaskFunc function,void *data,u32 priority) { (void)function; (void)data; (void)priority; queued++; return NULL; }
int main(void) {
    RenderModel models[128]={0}; NNSG3dResMdlSet sets[128]={0}; NNSG3dResTex textures[128]={0};
    for (u32 i=0;i<128;i++) {
        expected=&models[i]; expected->set=&sets[i]; expected->texture=&textures[i];
        CHECK(!expected->loaded); RenderModel_BindTexturesOnVBlank(expected);
        CHECK(expected->loaded && expected->set==&sets[i] && expected->texture==&textures[i]);
    }
    CHECK(uploads==128 && bindings==128 && !queued);
    puts("PASS: 128 original native model bindings complete in upload/bind order without depending on VBlank queue capacity.");
    return 0;
}
