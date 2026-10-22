#!/usr/bin/env python3
"""Check the real native raster register/memory provider with ASan and UBSan."""
from pathlib import Path
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'ios/tools'))
from prepare_original_headers import register_header
FIXTURE = r'''
#include <nitro/hw/native_graphics.h>
#include <nitro/gx/g2.h>
#include <nitro/gx/native_display.h>
#include <nitro/mi/memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"Raster memory line %d: %s\n",__LINE__,#x); exit(2); } } while (0)
u32 HGNativeDisplayLine(void) { return 17; }
void HGNativeAssertFail(const char *condition,const char *file,int line) {
    fprintf(stderr,"%s:%d: %s\n",file,line,condition); exit(73);
}
static u32 *word(u32 offset) { return (u32 *)HGNativeRegisterAddress(0x04000000u+offset,4); }
static u16 *half(u32 offset) { return (u16 *)HGNativeRegisterAddress(0x04000000u+offset,2); }
static u8 *byte(u32 offset) { return (u8 *)HGNativeRegisterAddress(0x04000000u+offset,1); }
static void step(u32 line,u32 phase) { HGNativeRasterStep(line,(void *)(uaddr)phase); }
static const MtxFx22 identity={4096,0,0,4096};
static void frame(int invalidate) {
    for (u32 tick=0;tick<HG_DISPLAY_LINES;tick++) {
        u32 line=(tick+192)%HG_DISPLAY_LINES;
        step(line,HG_NATIVE_RASTER_LINE);
        CHECK(*half(6)==line);
        CHECK((*half(4)&3)==((line>=192 && line<262) ? 1u : 0u));
        CHECK(((*half(4)&4)!=0)==(line==257));
        CHECK((*half(4)&0xFFB8)==0x01B8);
        if (line<192) *word(0x10)=line;
        u32 writes=line==6 ? 0x41u<<HG_NATIVE_RASTER_AFFINE_SHIFT : 0;
        step(line,HG_NATIVE_RASTER_HBLANK|writes);
        CHECK((*half(4)&2)!=0 && *half(6)==line);
        if (line==0) G2_SetBG2Affine(&identity,0,0,4,8);
        if (line==1) HGNativeMarkAffineWrite((uaddr)word(0x38),2);
        if (line==2) G2S_SetBG3Affine(&identity,0,0,2,3);
        if (line==4) *word(0x2C)+=256;
        if (line==7) {
            u32 affine[4]; MI_CpuCopy32(word(0x20),affine,16);
            MI_CpuCopy32(affine,(void *)0x04000020,16);
        }
        if (line==8) MI_CpuFill32((void *)0x04001038,*word(0x1038),4);
        if (line==9) { u32 value=*word(0x1028); MIi_CpuSend32(&value,(void *)0x04001028,4); }
        if (invalidate && line==7) HGNativeInvalidateRasterFrame();
    }
    step(HG_DISPLAY_LINES,HG_NATIVE_RASTER_FINISH);
}
int main(int argc,char **argv) {
    if (argc>1 && !strcmp(argv[1],"bad-phase")) { step(0,3); return 1; }
    if (argc>1 && !strcmp(argv[1],"bad-order")) { step(192,0); step(1,1); return 1; }
    if (argc>1 && !strcmp(argv[1],"bad-mask")) { step(192,0x10000); return 1; }
    u32 *scroll=word(0x10); u8 *bank=byte(0x240);
    u8 *physical=(u8 *)HGNativeGraphicsBank(0);
    u32 *palette=(u32 *)HGNativePaletteAddress();
    u32 size=4; u32 *objects=HGNativeMemorySpan(0x07000000,&size); CHECK(objects && size==4);
    CHECK(HGNativeRegisterDSAddress((uaddr)scroll)==0x04000010 && HGNativeRegisterDSAddress(0x04000010)==0x04000010);
    CHECK(HGNativeIsGraphicsRegisterSpan(0x04000020,32) && HGNativeIsGraphicsRegisterSpan(0x04001020,32));
    CHECK(!HGNativeIsGraphicsRegisterSpan(0x04000004,2) && !HGNativeIsGraphicsRegisterSpan(0x04000006,2));
    CHECK(!HGNativeIsGraphicsRegisterSpan(0x04000060,2) && !HGNativeIsGraphicsRegisterSpan(0x04000280,4));
    CHECK(!HGNativeIsGraphicsRegisterSpan(0x05000000,4) && !HGNativeIsGraphicsRegisterSpan(0x04000020,0));
    CHECK(!HGNativeIsGraphicsRegisterSpan(UINTPTR_MAX-1,4));
    if (sizeof(uaddr)>4) CHECK(!HGNativeIsGraphicsRegisterSpan((uaddr)0x104000020ULL,4));
    size=8; CHECK(HGNativeMemorySpan(0x0400005C,&size)==word(0x5C) && size==4);
    size=4; CHECK(!HGNativeMemorySpan(0x04000060,&size));
    *scroll=23; *bank=0x81; physical[0]=0x45; *palette=0x12345678; *objects=0x87654321;
    CHECK(!HGNativeSetRasterRow(1) && !HGNativeSetRasterRow(192));
    CHECK(HGNativeSetRasterRow(0));
    *scroll=91; *bank=0x89; physical[0]=0x67; *palette=0x11223344; *objects=0x44332211;
    CHECK(*word(0x10)==23 && *byte(0x240)==0x81);
    size=4; CHECK(*(u32 *)HGNativeMemorySpan(0x04000010,&size)==23 && size==4);
    u32 value=0; HGNativeSnapshotGraphics(0x06000000,&value,4); CHECK((value&255)==0x45);
    size=4; CHECK((*(u32 *)HGNativeMemorySpan(0x06000000,&size)&255)==0x45);
    HGNativeSnapshotGraphics(0x05000000,&value,4); CHECK(value==0x12345678);
    HGNativeSnapshotGraphics(0x07000000,&value,4); CHECK(value==0x87654321);
    CHECK(*(const u8 *)HGNativeGraphicsBank(0)==0x45);
    CHECK(HGNativeSetRasterRow(191) && HGNativeRasterAffineWrites(0,191)==0);
    HGNativeEndRasterReplay(); HGNativeEndRasterReplay();
    CHECK(*word(0x10)==91 && *byte(0x240)==0x89 && physical[0]==0x67);
    CHECK(HGNativeSetRasterRow(0) && *word(0x10)==91); HGNativeEndRasterReplay();
    *bank=0x83; physical[0]=0x12;
    CHECK(HGNativeSetRasterRow(0)); physical[0]=0x34; *bank=0x80;
    u8 texture=0; CHECK(HGNativeReadTextureMemory(0,0,&texture,1) && texture==0x12);
    HGNativeEndRasterReplay();
    *half(4)=0x01B8;
    G2_SetBG2Affine(&identity,0,0,4,8);
    step(192,0); CHECK(!HGNativeSetRasterRow(0)); HGNativeEndRasterReplay();
    CHECK(*half(6)==192);
    frame(0);
    CHECK(*half(6)==17 && !(*half(4)&3));
    CHECK(HGNativeSetRasterRow(0) && *word(0x10)==0 && HGNativeRasterAffineWrites(0,0)==15);
    CHECK(HGNativeSetRasterRow(1) && *word(0x10)==1 && HGNativeRasterAffineWrites(0,1)==3);
    CHECK(HGNativeSetRasterRow(2) && HGNativeRasterAffineWrites(0,2)==4);
    CHECK(HGNativeSetRasterRow(3) && HGNativeRasterAffineWrites(1,3)==12);
    CHECK(HGNativeSetRasterRow(4) && HGNativeRasterAffineWrites(0,4)==0);
    CHECK(HGNativeSetRasterRow(5) && HGNativeRasterAffineWrites(0,5)==2);
    CHECK(HGNativeSetRasterRow(6) && HGNativeRasterAffineWrites(0,6)==1 && HGNativeRasterAffineWrites(1,6)==4);
    CHECK(HGNativeSetRasterRow(8) && HGNativeRasterAffineWrites(0,8)==3);
    CHECK(HGNativeSetRasterRow(9) && HGNativeRasterAffineWrites(1,9)==4);
    CHECK(HGNativeSetRasterRow(10) && HGNativeRasterAffineWrites(1,10)==1);
    HGNativeEndRasterReplay();
    CHECK(*word(0x10)==191);
    CHECK(HGNativeSetRasterRow(1) && *word(0x10)==1); HGNativeEndRasterReplay();
    HGNativeInvalidateRasterFrame(); CHECK(!HGNativeSetRasterRow(0));
    frame(1); CHECK(!HGNativeSetRasterRow(0));
    frame(0); CHECK(HGNativeSetRasterRow(0)); HGNativeEndRasterReplay();
    puts("PASS: static/frame snapshots, live-state isolation, VCOUNT/flags, partial/stale rejection, repeated replay and same-value affine reload masks");
    return 0;
}
'''


def main():
    with tempfile.TemporaryDirectory(prefix='heartgold-raster-memory-') as folder:
        folder = Path(folder)
        source = folder / 'check.c'
        source.write_text(FIXTURE)
        includes = folder / 'include'
        includes.mkdir()
        (includes / 'hg_native_registers.h').write_text(register_header())
        (includes / 'hg_native_overlay_ids.h').write_text('')
        binary = folder / 'check'
        subprocess.run([
            'clang', '-std=c11', '-O1', '-g', '-fsanitize=address,undefined',
            '-fno-sanitize-recover=all', '-DHEARTGOLD_NATIVE', '-DSDK_ARM9', '-DPM_KEEP_ASSERTS',
            '-iquote', str(ROOT / 'include'), '-idirafter', str(ROOT / 'lib/include'),
            '-I', str(includes), str(source), str(ROOT / 'ios/Original/NativeGraphicsMemory.c'),
            str(ROOT / 'lib/NitroSDK/src/gx/g2.c'), str(ROOT / 'lib/NitroSDK/src/mi/mi_memory.c'), '-o', str(binary),
        ], check=True)
        subprocess.run([str(binary)], check=True)
        for invalid in ('bad-phase', 'bad-order', 'bad-mask'):
            result = subprocess.run([str(binary), invalid], capture_output=True, text=True)
            if result.returncode != 73:
                raise AssertionError(f'{invalid} was not rejected: {result.returncode}\n{result.stderr}')
        print('PASS: invalid raster phase, reserved mask bits and out-of-order capture rejected')


if __name__ == '__main__':
    main()
