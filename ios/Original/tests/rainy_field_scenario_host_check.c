// SPDX-License-Identifier: GPL-3.0-or-later
#define main RainyBootstrapMain
#define HGEngineTrap RainyBootstrapTrap
#include "../abi32/engine_check.c"
#undef HGEngineTrap
#undef main
#include <setjmp.h>
#include <nitro/os/native_reset.h>
#define HG_RAINY_FIELD_QA_STATE_ONLY
#include "../abi32/rainy_field_scenario_check.inc"
#undef HG_RAINY_FIELD_QA_STATE_ONLY

#define QA_CHECK(value) do { if (!(value)) { fprintf(stderr,"Rainy field failure line %d: %s\n",__LINE__,#value); _Exit(1); } } while (0)
static jmp_buf resetLanding;
static struct w2c_env *rainyEnv;
static BOOL resetExpected;
static RainyFieldQAState finalState,rainState;
static u32 enteredFrame,comparedFrame,returnedFrame;

void HGEngineTrap(wasm_rt_trap_t code) {
    if (code==WASM_RT_TRAP_UNREACHABLE && resetExpected && rainyEnv && rainyEnv->resetRequested) longjmp(resetLanding,1);
    RainyBootstrapTrap(code);
}
static RainyFieldQAState readState(Capture *capture) {
    u32 address=w2c_hgengine_HGNativeRainyFieldQAGetState(capture->game);
    QA_CHECK(address && address<=capture->env->memory->size && sizeof(RainyFieldQAState)<=capture->env->memory->size-address);
    RainyFieldQAState state; memcpy(&state,capture->env->memory->data+address,sizeof(state));
    QA_CHECK(!state.error); return state;
}
static BOOL rainFogReady(const RainyFieldQAState *state) {
    const HGNativeGeometryFogInfo *fog=&state->publishedFog,*source=&state->sourceFog;
    if (state->stage!=3 || !state->ready || state->mapId!=37 || state->weather!=1 || state->localWeather!=1 ||
        !state->rainModelDraws || !state->geometry.vertices || !state->geometry.polygons ||
        fog->color!=0x001F6B5A || fog->offset!=0x726F || (fog->control&0xFC0)!=0x380 ||
        source->color!=fog->color || source->offset!=fog->offset || (source->control&0xFC0)!=(fog->control&0xFC0)) return FALSE;
    for (u32 i=0;i<32;i++) if (fog->density[i]!=i*4 || source->density[i]!=i*4) return FALSE;
    return TRUE;
}
static void writeFrame(Capture *capture,const char *name,u32 offset) {
    QA_CHECK(capture->folder && offset<=capture->env->memory->size && 256*192*4<=capture->env->memory->size-offset);
    char path[4096]; QA_CHECK(snprintf(path,sizeof(path),"%s/%s.ppm",capture->folder,name)<sizeof(path));
    FILE *file=fopen(path,"wb"); QA_CHECK(file && fprintf(file,"P6\n256 192\n255\n")>0);
    const u8 *pixels=capture->env->memory->data+offset;
    for (u32 i=0;i<256*192;i++) QA_CHECK(fwrite(pixels+i*4,1,3,file)==3);
    QA_CHECK(!fclose(file));
}
static void RainyPresent(void *argument) {
    Capture *capture=argument;
    static u32 phase,lastDiagnostic;
    u32 frame=OS_GetVBlankCount();
    if (!phase) {
        present(argument);
        if (w2c_hgengine_HGNativeFieldMap(capture->game)) {
            QA_CHECK(HGNativeSetInput(capture->env,0,FALSE,0,0,FALSE));
            QA_CHECK(w2c_hgengine_HGNativeRainyFieldQARequest(capture->game,1));
            enteredFrame=frame; phase=1;
            fputs("Rainy field: queued original ScriptWarp to validated Route33 warp0\n",stderr);
        }
        return;
    }
    QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
    RainyFieldQAState state=readState(capture);
    if (frame-lastDiagnostic>=300) {
        lastDiagnostic=frame;
        fprintf(stderr,"Rainy field progress: frame=%u stage=%u map=%u pos=%u,%u ready=%u task=%u child=%u weather=%u fog=%x/%x/%x modelDraws=%u end/free=%u/%u\n",
            frame,state.stage,state.mapId,state.x,state.z,state.ready,state.task,state.child,state.weather,
            state.publishedFog.color,state.publishedFog.control,state.publishedFog.offset,state.rainModelDraws,state.rainEnded,state.rainFreed);
    }
    QA_CHECK(frame-enteredFrame<7200);
    if (phase==1 && rainFogReady(&state)) {
        QA_CHECK(state.x==467 && state.z==461 && state.completed==1);
        HGNativeDrainDMA();
        QA_CHECK(w2c_hgengine_HGNativeRainyFieldQACompare(capture->game));
        state=readState(capture);
        rainState=state;
        QA_CHECK(state.compared==1 && state.differentPixels && state.fogPolygons && !state.renderErrors);
        QA_CHECK(state.comparisonFog.color==state.publishedFog.color && state.comparisonFog.offset==state.publishedFog.offset &&
            state.comparisonFog.control==state.publishedFog.control);
        for (u32 i=0;i<32;i++) QA_CHECK(!state.comparisonFog.density[i]);
        if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
        writeFrame(capture,"route33-rain",capture->buffer);
        writeFrame(capture,"route33-zero-density",capture->buffer+256*192*4);
        comparedFrame=frame;
        QA_CHECK(HGNativeSoundRealtime(capture->env,TRUE));
        QA_CHECK(w2c_hgengine_HGNativeRainyFieldQARequest(capture->game,2)); phase=2;
        fputs("Rainy field: actual rain/fog/model/geometry and same-epoch zero-density contrast verified; returning\n",stderr);
    } else if (phase==2 && state.stage==6 && state.ready && state.mapId==state.originMap && state.returnModelDraws) {
        QA_CHECK(state.completed==2 && state.originMap==33 && state.x==state.originX && state.z==state.originZ);
        QA_CHECK(state.weather==0 && state.localWeather==0 && !(state.publishedFog.control&0x80));
        QA_CHECK(state.rainEnded==1 && state.rainEndConfirmed && state.rainFreed==1);
        QA_CHECK(!state.renderErrors);
        if (!capture->buffer) capture->buffer=w2c_hgengine_HGNativeFrameBuffer(capture->game);
        HGNativeDrainDMA(); QA_CHECK(w2c_hgengine_HGNativeRender2D(capture->game,0,capture->buffer,256*192*4));
        writeFrame(capture,"route29-return",capture->buffer);
        finalState=state; returnedFrame=frame; resetExpected=TRUE;
        w2c_env_HGNativeResetControl(capture->env,HG_RESET_REQUEST,0);
        QA_CHECK(FALSE);
    }
}
int main(int argc,char **argv) {
    if (argc!=5 || !HGNativeFSSetRoot(argv[1]) || !HGNativeBackupSetPath(argv[3])) return 2;
    unsigned char header[0x160]; FILE *input=fopen(argv[2],"rb");
    if (!input) return 2;
    int valid=fread(header,1,sizeof(header),input)==sizeof(header) && fgetc(input)==EOF && !ferror(input);
    fclose(input); if (!valid) return 2;
    wasm_rt_init();
    w2c_hgengine *game=calloc(1,sizeof(*game));
    struct w2c_env *env=calloc(1,sizeof(*env));
    struct w2c_wasi__snapshot__preview1 *wasi=calloc(1,sizeof(*wasi));
    Capture *capture=calloc(1,sizeof(*capture));
    QA_CHECK(game && env && wasi && capture);
    wasi->fds[0]=dup(0); wasi->fds[1]=dup(1); wasi->fds[2]=dup(2);
    QA_CHECK(wasi->fds[0]>=0 && wasi->fds[1]>=0 && wasi->fds[2]>=0);
    rainyEnv=env;
    if (!setjmp(resetLanding)) {
        wasm_rt_trap_t trap=wasm_rt_impl_try(); QA_CHECK(!trap);
        wasm2c_hgengine_instantiate(game,env,wasi);
        env->memory=wasi->memory=w2c_hgengine_memory(game);
        QA_CHECK(HGNativeSoundRealtime(env,TRUE)); // The native-host sink discards PCM; no audio device is opened.
        env->irqTable=&game->w2c_T0; env->stackPointer=&game->w2c_0x5F_stack_pointer;
        env->rootStackHigh=*env->stackPointer; *env->stackPointer-=16;
        env->irqType0=wasm2c_hgengine_get_func_type(0,0);
        env->irqType1=wasm2c_hgengine_get_func_type(1,0,WASM_RT_I32);
        env->rtcType=wasm2c_hgengine_get_func_type(2,0,WASM_RT_I32,WASM_RT_I32);
        env->irqGuardType=wasm2c_hgengine_get_func_type(4,0,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32,WASM_RT_I32);
        u32 work=w2c_hgengine_HGNativeSystemWorkAddress(game);
        QA_CHECK(work && work<=env->memory->size && 0x360<=env->memory->size-work);
        memcpy(env->memory->data+work+0x200,header,sizeof(header));
        w2c_hgengine_0x5Finitialize(game);
        u32 status=w2c_hgengine_HGNativeRegisterAddress(game,0x04000004,2);
        QA_CHECK(status && !(status&1) && status<=env->memory->size && 2<=env->memory->size-status);
        QA_CHECK(HGNativeStartDisplayClock(env,(const vu16 *)(env->memory->data+status)));
        *capture=(Capture){.game=game,.env=env,.folder=argv[4],.resume=TRUE};
        QA_CHECK(HGNativeSetDisplayObserver(env,RainyPresent,capture));
        w2c_hgengine_NitroMain(game); QA_CHECK(FALSE);
    }
    QA_CHECK(resetExpected && env->resetRequested);
    HGNativeFinishReset(env);
    QA_CHECK(env->resetFinished && !env->pendingDMA && !env->dmaAffineWrites && !env->pendingRTC && !env->pendingMIC &&
        !env->pendingCARD && !env->pendingPOWER && !env->pendingINPUT && !env->irqTable && !env->threads && !env->rasterFunction);
    printf("Rainy field evidence: {\"map\":37,\"anchor\":0,\"x\":467,\"z\":461,\"enteredFrame\":%u,\"comparedFrame\":%u,\"returnedFrame\":%u,\"differentPixels\":%u,\"fogPolygons\":%u,\"rainModelDraws\":%u,\"returnModelDraws\":%u,\"rainEnded\":%u,\"rainFreed\":%u,\"normalPixel\":%u,\"zeroDensityPixel\":%u,\"resetFinished\":true,\"pendingDMA\":0,\"fogColor\":%u,\"fogOffset\":%u,\"fogControl\":%u,\"geometrySerial\":%u,\"density\":[",
        enteredFrame,comparedFrame,returnedFrame,finalState.differentPixels,finalState.fogPolygons,finalState.rainModelDraws,
        finalState.returnModelDraws,finalState.rainEnded,finalState.rainFreed,finalState.normalPixel,finalState.zeroPixel,
        rainState.publishedFog.color,rainState.publishedFog.offset,rainState.publishedFog.control,rainState.geometry.serial);
    for (u32 i=0;i<32;i++) printf("%s%u",i ? "," : "",rainState.publishedFog.density[i]);
    puts("]}");
    wasm2c_hgengine_free(game);
    for (u32 i=0;i<3;i++) if (wasi->fds[i]>=0) close(wasi->fds[i]);
    free(game); free(env); free(wasi); free(capture); rainyEnv=NULL; wasm_rt_free();
    puts("PASS: real rainy Route33 ScriptWarp, original rain/fog, loaded map draw, same-epoch zero-density pixels, return and reset cleanup; no App/full-game claim.");
    return 0;
}
