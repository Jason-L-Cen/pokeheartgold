// SPDX-License-Identifier: GPL-3.0-or-later
#define main FieldMapsBootstrapMain
#define HGEngineTrap FieldMapsBootstrapTrap
#include "../abi32/engine_check.c"
#undef HGEngineTrap
#undef main
#include <setjmp.h>
#include <nitro/os/native_reset.h>
#define HG_FIELD_MAPS_QA_STATE_ONLY
#include "../abi32/field_maps_scenario_check.inc"
#undef HG_FIELD_MAPS_QA_STATE_ONLY

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Field maps failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
static jmp_buf resetLanding;
static struct w2c_env *mapsEnv;
static BOOL resetExpected,casePassed;
static const char *caseStatus="not_started";
static u32 target[7],enteredFrame,renderedFrame,returnedFrame,originSerial;
static u8 wideReference[FIELD_MAPS_WIDE_BYTES];
static BOOL wideStable,centerEqual,subGuards;
static u32 returnedWidth;
static FieldMapsQAState finalState,targetState;
void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && mapsEnv && mapsEnv->resetRequested) longjmp(resetLanding,1);
    FieldMapsBootstrapTrap(code);
}
static FieldMapsQAState readState(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeFieldMapsQAGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(FieldMapsQAState)<=capture->env->memory->size-address);
    FieldMapsQAState state; memcpy(&state,capture->env->memory->data+address,sizeof(state));
    QA_CHECK(!state.error); return state;
}
static void writeFrame(Capture *capture,const char *name,u32 width) {
    if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
    QA_CHECK(capture->buffer<=capture->env->memory->size && width*192*4<=capture->env->memory->size-capture->buffer);
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/%s.ppm",capture->folder,name)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n%u 192\n255\n",width)>0);
    const u8 *pixels=capture->env->memory->data+capture->buffer;
    for (u32 i=0;i<width*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
    QA_CHECK(!fclose(file));
}
static void finish(Capture *capture,FieldMapsQAState state,const char *status) {
    if (!state.rendered) targetState=state;
    finalState=state; caseStatus=status; resetExpected=TRUE;
    w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0); QA_CHECK(FALSE);
}
static void FieldMapsPresent(void *argument) {
    Capture *capture=argument; static u32 phase,lastDiagnostic,lastRender;
    u32 frame=OS_GetVBlankCount();
    if (!phase) {
        present(argument);
        if (w2c_hgengine_HGNativeFieldMap(capture->game)) {
            QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
            FieldMapsQAState initial=readState(capture); originSerial=initial.geometry.serial;
            QA_CHECK(w2c_hgengine_HGNativeFieldMapsQARequest(capture->game,target[0],target[1],target[2],target[3],target[4],target[5],target[6]));
            enteredFrame=frame; phase=1;
            fprintf(stderr,"Field maps: queued real ScriptWarp map=%u warp=%u anchor=%u,%u cell=%u model=%u\n",target[0],target[1],target[2],target[3],target[5],target[6]);
        }
        return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    FieldMapsQAState state=readState(capture);
    if (frame-lastDiagnostic>=300) {
        lastDiagnostic=frame;
        fprintf(stderr,"Field maps progress: frame=%u stage=%u map=%u pos=%u,%u ready=%u task=%u child=%u mode=%u model=%u draws=%u polys=%u end/free=%u/%u\n",
            frame,state.stage,state.mapId,state.x,state.z,state.ready,state.task,state.child,state.loadType,state.drawnModel,state.modelDraws,state.drawPolygons,state.ended,state.freed);
    }
    if (frame-enteredFrame>3600) {
        const char *status=phase==2 ? "return_or_teardown_blocked" : !state.ready || state.stage<3 ? "script_or_mode_precondition_blocked" :
            state.x!=target[2] || state.z!=target[3] || state.drawnModel!=target[6] ? "anchor_or_world_layout_changed" : "model_or_render_not_verified";
        finish(capture,state,status); return;
    }
    if (phase==1 && state.stage==3 && state.ready && state.mapId==target[0] && state.x==target[2] && state.z==target[3] &&
        state.modelDraws && state.drawnModel==target[6] && state.drawPolygons && state.geometry.serial!=originSerial &&
        (state.targetWidthEpochs>=2 || state.wideRejected) && frame-lastRender>=15) {
        lastRender=frame; HGNativeDrainDMA();
        if (!w2c_hgengine_HGNativeFieldMapsQARender(capture->game)) return;
        targetState=readState(capture); QA_CHECK(targetState.rendered==1 && targetState.nonuniform && targetState.completed==1);
        capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game)+FIELD_MAPS_WIDE_OFFSET;
        if (targetState.renderWidth==512) {
            QA_CHECK(targetState.requestedWidth==512 && targetState.originWidthEpochs>=2 && targetState.targetWidthEpochs>=2 && targetState.drawWidth==512);
            memcpy(wideReference,capture->env->memory->data+capture->buffer,FIELD_MAPS_WIDE_BYTES);
            u32 small=capture->buffer-FIELD_MAPS_WIDE_OFFSET+FIELD_MAPS_SMALL_OFFSET;
            QA_CHECK(small>=8 && small<=capture->env->memory->size && FIELD_MAPS_SMALL_BYTES+8<=capture->env->memory->size-small);
            memset(capture->env->memory->data+small-8,0xA5,8); memset(capture->env->memory->data+small+FIELD_MAPS_SMALL_BYTES,0xA5,8);
            QA_CHECK(w2c_hgengine_HGNativeRender2D(capture->game,1,small,FIELD_MAPS_SMALL_BYTES));
            subGuards=TRUE;
            for (u32 i=0;i<8;i++) subGuards&=capture->env->memory->data[small-8+i]==0xA5 && capture->env->memory->data[small+FIELD_MAPS_SMALL_BYTES+i]==0xA5;
            state=readState(capture); QA_CHECK(subGuards && state.requestedWidth==512 && !memcmp(&state.geometry,&targetState.geometry,sizeof(state.geometry)));
            QA_CHECK(w2c_hgengine_HGNativeFieldMapsQARender(capture->game)); state=readState(capture);
            wideStable=!memcmp(wideReference,capture->env->memory->data+capture->buffer,FIELD_MAPS_WIDE_BYTES);
            QA_CHECK(wideStable && state.rendered==2 && state.renderWidth==512 && state.requestedWidth==512 && state.guards>=2 && !memcmp(&state.geometry,&targetState.geometry,sizeof(state.geometry)));
            QA_CHECK(w2c_hgengine_HGNativeRender2D(capture->game,0,small,FIELD_MAPS_SMALL_BYTES));
            centerEqual=TRUE;
            for (u32 y=0;y<192;y++) centerEqual&=!memcmp(wideReference+(y*512+128)*4,capture->env->memory->data+small+y*256*4,256*4);
            for (u32 i=0;i<8;i++) QA_CHECK(capture->env->memory->data[small-8+i]==0xA5 && capture->env->memory->data[small+FIELD_MAPS_SMALL_BYTES+i]==0xA5);
            state=readState(capture);
            if (!centerEqual) {
                for (u32 y=0;y<192;y++) for (u32 x=0;x<256;x++) {
                    const u8 *wide=wideReference+(y*512+x+128)*4,*narrow=capture->env->memory->data+small+(y*256+x)*4;
                    if (memcmp(wide,narrow,4)) {
                        fprintf(stderr,"Field wide center mismatch: x=%u y=%u wide=%u,%u,%u,%u canonical=%u,%u,%u,%u requestedWidth=%u sameGeometry=%u originEpochs=%u targetEpochs=%u drawWidth=%u\n",
                            x,y,wide[0],wide[1],wide[2],wide[3],narrow[0],narrow[1],narrow[2],narrow[3],state.requestedWidth,
                            !memcmp(&state.geometry,&targetState.geometry,sizeof(state.geometry)),state.originWidthEpochs,state.targetWidthEpochs,state.drawWidth);
                        goto centerDiagnosticDone;
                    }
                }
centerDiagnosticDone:
                writeFrame(capture,"failed-wide-center",512);
                u32 saved=capture->buffer; capture->buffer=small;
                writeFrame(capture,"failed-canonical-center",256); capture->buffer=saved;
            }
            QA_CHECK(centerEqual);
            QA_CHECK(state.requestedWidth==512);
            QA_CHECK(!memcmp(&state.geometry,&targetState.geometry,sizeof(state.geometry)));
            targetState=state;
        }
        writeFrame(capture,"target",targetState.renderWidth); renderedFrame=frame;
        QA_CHECK(w2c_hgengine_HGNativeFieldMapsQAReturn(capture->game)); phase=2;
    } else if (phase==2 && state.stage==6 && state.ready && state.mapId==state.originMap && state.returnDraws) {
        QA_CHECK(state.completed==2 && state.originMap==33 && state.x==state.originX && state.z==state.originZ);
        QA_CHECK(state.ended==1 && state.endConfirmed && state.freed==1);
        capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game)+FIELD_MAPS_WIDE_OFFSET;
        HGNativeDrainDMA(); returnedWidth=targetState.renderWidth;
        if (returnedWidth==512 && !w2c_hgengine_HGNativeRender2DView(capture->game,0,512,capture->buffer,FIELD_MAPS_WIDE_BYTES)) {
            returnedWidth=256; QA_CHECK(w2c_hgengine_HGNativeSetViewWidth(capture->game,256));
        }
        if (returnedWidth==256) QA_CHECK(w2c_hgengine_HGNativeRender2D(capture->game,0,capture->buffer,FIELD_MAPS_SMALL_BYTES));
        writeFrame(capture,"return",returnedWidth); returnedFrame=frame;
        state=readState(capture);
        casePassed=targetState.renderWidth==512 && returnedWidth==512 && wideStable && centerEqual && subGuards && state.requestedWidth==512;
        finish(capture,state,casePassed ? "passed_continuous_512" : "validated_explicit_canonical_fallback");
    }
}
int main(int argc,char **argv) {
    if (argc!=12 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    for (u32 i=0;i<7;i++) { char *end; unsigned long value=strtoul(argv[5+i],&end,10); if (!*argv[5+i] || *end || value>UINT32_MAX) return 2; target[i]=(u32)value; }
    unsigned char header[0x160]; FILE *input=fopen(argv[2],"rb"); if (!input) return 2;
    int valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);
    fclose(input); if (!valid) return 2;
    wasm_rt_init(); w2c_hgengine *game=calloc(1,sizeof(*game)); struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi)); Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture);
    wasi->fds[0]=dup(0); wasi->fds[1]=dup(1); wasi->fds[2]=dup(2);
    QA_CHECK(wasi->fds[0]>=0 && wasi->fds[1]>=0 && wasi->fds[2]>=0); mapsEnv=env;
    if (!setjmp(resetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi); env->memory=wasi->memory=w2c_hgengine_memory(game);
        QA_CHECK(HGNativeSoundRealtime(env,TRUE)); // Native-host PCM sink discards audio; no sound device opens.
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer;
        env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0); env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game);
        QA_CHECK(work && work<=env->memory->size && 0x360<=env->memory->size-work);
        memcpy(env->memory->data+work+0x200,header,sizeof(header)); w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);
        QA_CHECK(status && !(status&1) && status<=env->memory->size && 2<=env->memory->size-status);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,FieldMapsPresent,capture)); w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested); HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC &&
        !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    printf("Field maps evidence: {\"status\":\"%s\",\"map\":%u,\"warp\":%u,\"x\":%u,\"z\":%u,\"cell\":%u,\"model\":%u,\"loadedModel\":%u,\"modelFile\":%u,\"draws\":%u,\"mapPolygons\":%u,\"geometrySerial\":%u,\"vertices\":%u,\"polygons\":%u,\"loadType\":%u,\"weather\":%u,\"enteredFrame\":%u,\"renderedFrame\":%u,\"returnedFrame\":%u,\"ended\":%u,\"endConfirmed\":%u,\"freed\":%u,\"completed\":%u,\"actualMap\":%u,\"stage\":%u,\"ready\":%u,\"task\":%u,\"child\":%u,\"actualX\":%u,\"actualZ\":%u,\"requestedWidth\":%u,\"targetWidth\":%u,\"returnWidth\":%u,\"originWideEpochs\":%u,\"targetWideEpochs\":%u,\"drawWidth\":%u,\"wideRejected\":%u,\"wideGuards\":%u,\"wideStable\":%u,\"center256Equal\":%u,\"subGuards\":%u,\"resetFinished\":true}\n",
        caseStatus,target[0],target[1],target[2],target[3],target[5],target[6],targetState.drawnModel,targetState.modelFile,targetState.modelDraws,
        targetState.drawPolygons,targetState.geometry.serial,targetState.geometry.vertices,targetState.geometry.polygons,targetState.loadType,targetState.weather,
        enteredFrame,renderedFrame,returnedFrame,finalState.ended,finalState.endConfirmed,finalState.freed,finalState.completed,
        finalState.mapId,finalState.stage,finalState.ready,finalState.task,finalState.child,finalState.x,finalState.z,
        finalState.requestedWidth,targetState.renderWidth,returnedWidth,targetState.originWidthEpochs,targetState.targetWidthEpochs,targetState.drawWidth,
        targetState.wideRejected,targetState.guards,wideStable,centerEqual,subGuards);
    wasm2c_hgengine_free(game); for (u32 i=0;i<3;i++) if (wasi->fds[i]>=0) close(wasi->fds[i]);
    free(game); free(env); free(wasi); free(capture); mapsEnv=NULL; wasm_rt_free();
    if (casePassed) puts("PASS: real field ScriptWarp, continuous512 submission, main512/sub256/main512, center/guards and original return/cleanup.");
    return casePassed ? 0 : 3;
}
