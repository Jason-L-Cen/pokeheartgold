// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro/hw/io_reg.h>
#include <nitro/gx/native_display.h>
#include "assert.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static u32 registers[0x1070 / 4];
static u32 palettes[0x800 / 4], objects[0x800 / 4], vram[0xA4000 / 4];
uaddr HGNativePaletteAddress(void) { return (uaddr)palettes; }
static const u32 bankOffset[9] = {0, 0x20000, 0x40000, 0x60000, 0x80000, 0x90000, 0x94000, 0x98000, 0xA0000};
static const u32 bankSize[9] = {0x20000, 0x20000, 0x20000, 0x20000, 0x10000, 0x4000, 0x4000, 0x8000, 0x4000};
static const u32 bankControl[9] = {0x240, 0x241, 0x242, 0x243, 0x244, 0x245, 0x246, 0x248, 0x249};

uaddr HGNativeRegisterDSAddress(uaddr address) {
    uaddr base=(uaddr)registers;
    return address>=base && address-base<sizeof(registers) ? 0x04000000u+(address-base) : address;
}
static u32 graphicsRegisterBytes(uaddr address) {
    // No DISPSTAT/VCOUNT, 3D control, capture, FIFO, or unrelated hardware registers.
    const u32 start[]={0x04000000,0x04000008,0x0400006C,0x04001000,0x04001008,0x0400106C};
    const u32 end[]={0x04000004,0x04000060,0x04000070,0x04001004,0x04001060,0x04001070};
    for (u32 i=0;i<6;i++) if (address>=start[i] && address<end[i]) return (u32)(end[i]-address);
    return 0;
}
BOOL HGNativeIsGraphicsRegisterSpan(uaddr address,u32 size) {
    return size && size<=graphicsRegisterBytes(address);
}

typedef struct RasterRow {
    u32 main[0x70/4],sub[0x70/4];
    u8 banks[12],affineWrites[2];
} RasterRow;
static RasterRow rasterRows[192],staticRow;
static struct { u32 palettes[0x800/4],objects[0x800/4],vram[0xA4000/4]; } rasterMemory;
static int rasterProducerLine=-1,rasterReplayLine=-1;
static BOOL rasterDriver,rasterCapturing,rasterCommitted,rasterInvalid,rasterStatic;
static u32 rasterNextLine;
static u8 affineWrites[2];
static u32 captureEpoch;
static BOOL captureCompleted;

static const RasterRow *replayRow(void) {
    return rasterReplayLine<0 ? NULL : rasterStatic ? &staticRow : &rasterRows[rasterReplayLine];
}
static u8 graphicsBankControl(unsigned bank) {
    const RasterRow *row=replayRow();
    return row ? row->banks[bankControl[bank]-0x240] : ((const u8 *)registers)[bankControl[bank]];
}
static void freezeGraphicsMemory(void) {
    memcpy(rasterMemory.palettes,palettes,sizeof(palettes));
    memcpy(rasterMemory.objects,objects,sizeof(objects));
    // shortcut: HBlank currently changes registers; journal graphics-memory writes before adding such effects.
    memcpy(rasterMemory.vram,vram,sizeof(vram));
}
static void displayState(u32 line,BOOL hblank) {
    u16 status; memcpy(&status,(u8 *)registers+4,2);
    u32 compare=(status>>8)|((status&0x80)<<1);
    status=(u16)((status&~7u)|((line>=192 && line<262) ? 1 : 0)|(hblank ? 2 : 0)|(line==compare ? 4 : 0));
    u16 count=(u16)line;
    memcpy((u8 *)registers+4,&status,2); memcpy((u8 *)registers+6,&count,2);
}
static void captureRow(RasterRow *row,u32 line) {
    memcpy(row->main,registers,sizeof(row->main));
    memcpy(row->sub,(u8 *)registers+0x1000,sizeof(row->sub));
    memcpy(row->banks,(u8 *)registers+0x240,sizeof(row->banks));
    const u32 references[]={0x28/4,0x2C/4,0x38/4,0x3C/4};
    for (u32 engine=0;engine<2;engine++) {
        row->affineWrites[engine]=line ? affineWrites[engine] : 15;
        if (line) {
            const u32 *current=engine ? row->sub : row->main;
            const u32 *previous=engine ? rasterRows[line-1].sub : rasterRows[line-1].main;
            for (u32 i=0;i<4;i++) if (current[references[i]]!=previous[references[i]]) row->affineWrites[engine]|=1u<<i;
        }
        affineWrites[engine]=0;
    }
}
void HGNativeRasterStep(u32 line,void *phasePointer) {
    uaddr packed=(uaddr)phasePointer;
    u32 phase=(u32)(packed&255);
    GF_ASSERT(phase<=HG_NATIVE_RASTER_FINISH && (packed>>HG_NATIVE_RASTER_AFFINE_SHIFT)<=255);
    if (phase==HG_NATIVE_RASTER_FINISH) {
        GF_ASSERT(line==HG_DISPLAY_LINES);
        rasterCommitted=rasterCapturing && !rasterInvalid && rasterNextLine==192;
        rasterCapturing=FALSE; rasterProducerLine=-1; rasterReplayLine=-1;
        displayState(HGNativeDisplayLine(),FALSE); return;
    }
    GF_ASSERT(line<HG_DISPLAY_LINES);
    if (phase==HG_NATIVE_RASTER_LINE && line==192) {
        rasterDriver=TRUE; rasterCapturing=TRUE; rasterCommitted=rasterInvalid=rasterStatic=FALSE;
        rasterNextLine=0; rasterReplayLine=-1; affineWrites[0]=affineWrites[1]=0;
        captureEpoch++; captureCompleted=FALSE;
    }
    u32 writes=(u32)(packed>>HG_NATIVE_RASTER_AFFINE_SHIFT);
    affineWrites[0]|=writes&15; affineWrites[1]|=writes>>4;
    rasterProducerLine=(int)line;
    if (phase==HG_NATIVE_RASTER_HBLANK && line<192) {
        GF_ASSERT(rasterCapturing && line==rasterNextLine);
        if (!line) { freezeGraphicsMemory(); rasterInvalid=FALSE; }
        captureRow(&rasterRows[line],line); rasterNextLine++;
    }
    displayState(line,phase==HG_NATIVE_RASTER_HBLANK);
}
BOOL HGNativeSetRasterRow(u32 line) {
    if (line>=192) return FALSE;
    if (!rasterDriver) {
        if (!rasterStatic) {
            if (line) return FALSE;
            freezeGraphicsMemory(); captureRow(&staticRow,0); rasterStatic=TRUE;
        }
    } else if (!rasterCommitted) return FALSE;
    rasterReplayLine=(int)line; return TRUE;
}
void HGNativeEndRasterReplay(void) { rasterReplayLine=-1; rasterStatic=FALSE; }
u32 HGNativeRasterAffineWrites(u32 engine,u32 line) {
    GF_ASSERT(engine<2 && line<192);
    return !line ? 15 : rasterStatic ? 0 : rasterRows[line].affineWrites[engine];
}
void HGNativeInvalidateRasterFrame(void) {
    rasterCommitted=FALSE; captureEpoch++;
    if (rasterCapturing) rasterInvalid=TRUE;
}
void HGNativeMarkAffineWrite(uaddr address,u32 size) {
    if (!size || address>UINTPTR_MAX-size) return;
    address=HGNativeRegisterDSAddress(address);
    if (address>UINTPTR_MAX-size || address+size<=0x04000028u || address>=0x04001040u) return;
    const u32 references[]={0x28,0x2C,0x38,0x3C};
    for (u32 engine=0;engine<2;engine++) for (u32 i=0;i<4;i++) {
        uaddr reference=0x04000000u+engine*0x1000+references[i];
        if (address<reference+4 && reference<address+size) affineWrites[engine]|=1u<<i;
    }
}

static void updateMath(u32 offset) {
    u8 *bytes=(u8 *)registers;
    if (offset<0x282 || (offset>=0x2A0 && offset<0x2B0)) {
        u16 control; u64 numerator,denominator;
        memcpy(&control,bytes+0x280,2); memcpy(&numerator,bytes+0x290,8); memcpy(&denominator,bytes+0x298,8);
        u32 mode=control&3;
        s64 n=mode ? (s64)numerator : (s32)numerator;
        s64 d=mode==2 ? (s64)denominator : (s32)denominator;
        s64 quotient,remainder;
        if (!d) {
            quotient=n<0 ? 1 : -1; remainder=n;
            if (!mode) quotient=(s64)((u64)quotient^0xFFFFFFFF00000000ULL);
        } else if (n==(-9223372036854775807LL-1) && d==-1) {
            quotient=n; remainder=0;
        } else { quotient=n/d; remainder=n%d; }
        control=(u16)(mode | (denominator ? 0 : 0x4000));
        memcpy(bytes+0x280,&control,2); memcpy(bytes+0x2A0,&quotient,8); memcpy(bytes+0x2A8,&remainder,8);
    } else if (offset==0x2B0 || offset==0x2B1 || (offset>=0x2B4 && offset<0x2B8)) {
        u16 control; u64 value;
        memcpy(&control,bytes+0x2B0,2); memcpy(&value,bytes+0x2B8,8); control&=1;
        if (!control) value=(u32)value;
        u64 root=(u64)__builtin_sqrt((double)value);
        while (root && root>value/root) --root;
        while (root<0xFFFFFFFFULL && root+1<=value/(root+1)) ++root;
        u32 result=(u32)root;
        memcpy(bytes+0x2B0,&control,2); memcpy(bytes+0x2B4,&result,4);
    }
}
uaddr HGNativeRegisterAddress(u32 address, u32 width) {
    GF_ASSERT(width == 1 || width == 2 || width == 4 || width == 8);
    GF_ASSERT(address % width == 0);
    u32 offset = address - 0x04000000u;
    // Graphics state and native math registers; the display FIFO still needs an active backend.
    BOOL supported = (offset < 0x64 && width <= 0x64 - offset) ||
        (offset >= 0x64 && offset < 0x68 && width <= 0x68 - offset) ||
        (offset >= 0x6C && offset < 0x70 && width <= 0x70 - offset) ||
        (offset >= 0x1000 && offset < 0x1070 && width <= 0x1070 - offset) ||
        (offset >= 0x240 && offset < 0x247 && width <= 0x247 - offset) ||
        (offset >= 0x248 && offset < 0x24A && width <= 0x24A - offset) ||
        (offset >= 0x280 && offset < 0x282 && width <= 0x282 - offset) ||
        (offset >= 0x290 && offset < 0x2B0 && width <= 0x2B0 - offset) ||
        (offset >= 0x2B0 && offset < 0x2B2 && width <= 0x2B2 - offset) ||
        (offset >= 0x2B4 && offset < 0x2C0 && width <= 0x2C0 - offset) ||
        (offset >= 0x330 && offset < 0x3C0 && width <= 0x3C0 - offset) ||
        (offset==0x340 && width<=2) ||
        (offset == 0x304 && width <= 2);
    if (!supported) fprintf(stderr,"Unsupported native graphics register: address=0x%08X width=%u\n",address,width);
    GF_ASSERT(supported);
    const RasterRow *row=replayRow();
    if (row) {
        if (offset<0x70 && width<=0x70-offset) return (uaddr)((const u8 *)row->main+offset);
        if (offset>=0x1000 && offset<0x1070 && width<=0x1070-offset) return (uaddr)((const u8 *)row->sub+(offset-0x1000));
        if (offset>=0x240 && offset<0x24C && width<=0x24C-offset) return (uaddr)(row->banks+(offset-0x240));
    }
    if (offset<8 && offset+width>4 && rasterProducerLine<0) displayState(HGNativeDisplayLine(),FALSE);
    // shortcut: math completes on status/result access; calibrate hardware busy-cycle timing before timing-sensitive comparisons.
    if (offset>=0x280 && offset<0x2C0) updateMath(offset);
    return (uaddr)((u8 *)registers + offset);
}

static BOOL bankAddress(unsigned bank, u8 control, u32 *address) {
    if (!(control & 0x80)) return FALSE;
    u32 mode = control & 7, slot = (control >> 3) & 3;
    if (!mode) { *address = 0x06800000u + bankOffset[bank]; return TRUE; }
    if (bank < 4) {
        if (mode == 1) *address = 0x06000000u + slot * 0x20000;
        else if (mode == 2 && bank < 2 && slot < 2) *address = 0x06400000u + slot * 0x20000;
        else if (mode == 4 && bank == 2) *address = 0x06200000u;
        else if (mode == 4 && bank == 3) *address = 0x06600000u;
        else return FALSE; // Texture/ARM7 mappings are not CPU-visible here.
    } else if (bank < 7 && (mode == 1 || mode == 2)) {
        u32 offset = bank == 4 ? 0 : ((slot & 1) * 0x4000 + (slot & 2) * 0x8000);
        *address = (mode == 1 ? 0x06000000u : 0x06400000u) + offset;
    } else if (bank == 7 && mode == 1) *address = 0x06200000u;
    else if (bank == 8 && mode == 1) *address = 0x06208000u;
    else if (bank == 8 && mode == 2) *address = 0x06600000u;
    else return FALSE;
    return TRUE;
}

void *HGNativeMemorySpan(uaddr address, u32 *size) {
    if (!size || !*size || address > UINTPTR_MAX - *size) return NULL;
    if (address > UINT32_MAX) return (void *)address;
#ifdef HEARTGOLD_ABI32
    // Generated C uses bounded offsets below the original DS main-RAM address range.
    if (address && address < 0x02000000u && *size <= 0x02000000u - address &&
        (u64)address + *size <= (u64)__builtin_wasm_memory_size(0) * 65536) return (void *)address;
#endif
    u32 registerBytes=graphicsRegisterBytes(address);
    if (address>=0x04000064u && address<0x04000068u) registerBytes=(u32)(0x04000068u-address);
    if (registerBytes) {
        if (*size>registerBytes) *size=registerBytes;
        return (void *)HGNativeRegisterAddress((u32)address,1);
    }
    if (address >= 0x05000000u && address < 0x05000800u) {
        if (*size > 0x05000800u - address) *size = 0x05000800u - address;
        return (u8 *)(replayRow() ? rasterMemory.palettes : palettes) + (address - 0x05000000u);
    }
    if (address >= 0x07000000u && address < 0x07000800u) {
        if (*size > 0x07000800u - address) *size = 0x07000800u - address;
        return (u8 *)(replayRow() ? rasterMemory.objects : objects) + (address - 0x07000000u);
    }
    void *result = NULL;
    u32 count = *size;
    for (unsigned bank = 0; bank < 9; bank++) {
        u32 base;
        if (!bankAddress(bank, graphicsBankControl(bank), &base) ||
            address < base || address - base >= bankSize[bank]) continue;
        // shortcut: overlapping bank mappings need bus OR/broadcast semantics; reject until used.
        if (result) return NULL;
        u32 offset = (u32)(address - base);
        count = bankSize[bank] - offset;
        result = (u8 *)(replayRow() ? rasterMemory.vram : vram) + bankOffset[bank] + offset;
    }
    if (result && *size > count) *size = count;
    return result;
}

static void *captureSpan(const void *pointer,u32 bytes,u32 alignment) {
    if (!pointer || (uaddr)pointer%alignment) return NULL;
    u32 available=bytes;
    void *span=HGNativeMemorySpan((uaddr)pointer,&available);
    return available==bytes ? span : NULL;
}
static BOOL captureInfo(HGNativeDisplayCaptureInfo *info) {
    if (!rasterDriver || !rasterCommitted || captureCompleted) return FALSE;
    info->control=rasterRows[0].main[0x64/4];
    info->displayControl=rasterRows[0].main[0];
    info->epoch=captureEpoch;
    return (info->control&0x80000000u) && registers[0x64/4]==info->control;
}
static BOOL captureRequest(const HGNativeDisplayCaptureInfo *request,HGNativeDisplayCaptureInfo *info) {
    const void *span=captureSpan(request,sizeof(*request),4);
    return span && captureInfo(info) && !memcmp(span,info,sizeof(*info));
}
static u32 captureWidth(u32 control) { return ((control>>20)&3) ? 256 : 128; }
static u32 captureHeight(u32 control) { u32 size=(control>>20)&3; return size ? size*64 : 128; }
BOOL HGNativeBeginDisplayCapture(HGNativeDisplayCaptureInfo *output) {
    HGNativeDisplayCaptureInfo info;
    void *span=captureSpan(output,sizeof(*output),4);
    if (!span || !captureInfo(&info)) return FALSE;
    memcpy(span,&info,sizeof(info)); return TRUE;
}
BOOL HGNativeReadDisplayCapture(const HGNativeDisplayCaptureInfo *request,u32 line,u16 *output,u32 pixels) {
    HGNativeDisplayCaptureInfo info;
    if (!captureRequest(request,&info) || (info.control&(1u<<25)) ||
        line>=captureHeight(info.control) || pixels!=captureWidth(info.control)) return FALSE;
    u16 *destination=captureSpan(output,pixels*2,2);
    if (!destination) return FALSE;
    // shortcut: source B freezes at row zero; midline CPU VRAM feedback/writes need a per-line journal.
    const RasterRow *row=&rasterRows[line];
    u32 display=row->main[0],bank=(display>>18)&3;
    if ((row->banks[bank]&0x87)!=0x80) { memset(destination,0,pixels*2); return TRUE; }
    u32 offset=line*256;
    if (((display>>16)&3)!=2) offset+=((info.control>>26)&3)*0x4000;
    const u16 *source=(const u16 *)((const u8 *)rasterMemory.vram+bankOffset[bank]);
    memcpy(destination,source+(offset&0xFFFF),pixels*2);
    return TRUE;
}
BOOL HGNativeCompleteDisplayCapture(const HGNativeDisplayCaptureInfo *request,const u16 *source,u32 pixels) {
    HGNativeDisplayCaptureInfo info;
    if (!captureRequest(request,&info) || registers[0x64/4]!=info.control) return FALSE;
    u32 width=captureWidth(info.control),height=captureHeight(info.control);
    if (pixels!=width*height) return FALSE;
    const u16 *input=captureSpan(source,pixels*2,2);
    if (!input) return FALSE;
    if ((uaddr)input<(uaddr)vram+sizeof(vram) && (uaddr)input+pixels*2>(uaddr)vram) return FALSE;
    // shortcut: frame-end capture rejects midline reprogramming; support it with a per-line register/write journal.
    for (u32 line=0;line<height;line++)
        if (rasterRows[line].main[0x64/4]!=info.control) return FALSE;
    u32 bank=(info.control>>16)&3,offset=((info.control>>18)&3)*0x4000;
    u16 *destination=(u16 *)((u8 *)vram+bankOffset[bank]);
    for (u32 line=0;line<height;line++) {
        if ((rasterRows[line].banks[bank]&0x87)!=0x80 ||
            (((const u8 *)registers)[bankControl[bank]]&0x87)!=0x80) continue;
        memcpy(destination+((offset+line*width)&0xFFFF),input+line*width,width*2);
    }
    registers[0x64/4]&=~0x80000000u; captureCompleted=TRUE;
    return TRUE;
}

const void *HGNativeGraphicsBank(u32 bank) {
    GF_ASSERT(bank < 9); return (const u8 *)(replayRow() ? rasterMemory.vram : vram) + bankOffset[bank];
}
BOOL HGNativeFillVRAM(uaddr address,u32 value,u32 size,u32 width) {
    if ((width!=1 && width!=2 && width!=4) || address%width || size%width) return FALSE;
    const u32 bases[]={0x06000000,0x06200000,0x06400000,0x06600000,0x06800000};
    const u32 lengths[]={0x80000,0x20000,0x40000,0x20000,0xA4000};
    BOOL valid=FALSE;
    for (unsigned i=0;i<5;i++)
        if (address>=bases[i] && address-bases[i]<lengths[i] && size<=lengths[i]-(address-bases[i])) valid=TRUE;
    if (!valid) return FALSE;
    // Battle setup clears complete BG/OBJ windows, including addresses with no bank behind them.
    for (unsigned bank=0;bank<9;bank++) {
        u32 base;
        if (!bankAddress(bank,((u8 *)registers)[bankControl[bank]],&base)) continue;
        u32 first=address>base ? (u32)address : base;
        u32 end=address+size<base+bankSize[bank] ? (u32)(address+size) : base+bankSize[bank];
        for (u32 position=first;position<end;position+=width)
            memcpy((u8 *)vram+bankOffset[bank]+position-base,&value,width);
    }
    return TRUE;
}
BOOL HGNativeReadTextureMemory(BOOL palette,u32 address,void *output,u32 size) {
    u32 limit=palette ? 0x20000 : 0x80000;
    if (address>limit || size>limit-address || (size && !output)) return FALSE;
    if (!size) return TRUE;
    memset(output,0,size);
    for (u32 bank=palette ? 4 : 0;bank<(palette ? 7 : 4);bank++) {
        u8 control=graphicsBankControl(bank);
        if ((control&0x87)!=0x83) continue;
        u32 slot=(control>>3)&3,base=palette ? (bank==4 ? 0 : (slot&1)*0x4000+(slot&2)*0x8000) : slot*0x20000;
        u32 first=address>base ? address : base;
        u32 end=address+size<base+bankSize[bank] ? address+size : base+bankSize[bank];
        for (u32 offset=first;offset<end;offset++)
            ((u8 *)output)[offset-address]|=((const u8 *)(replayRow() ? rasterMemory.vram : vram))[bankOffset[bank]+offset-base];
    }
    return TRUE;
}

void HGNativeSnapshotGraphics(u32 address, void *destination, u32 size) {
    GF_ASSERT(destination && size && address <= UINT32_MAX - size);
    const u32 base[] = {0x05000000, 0x06000000, 0x06200000, 0x06400000, 0x06600000, 0x07000000};
    const u32 length[] = {0x800, 0x80000, 0x20000, 0x40000, 0x20000, 0x800};
    BOOL valid = FALSE;
    for (unsigned i = 0; i < 6; i++)
        if (address >= base[i] && address - base[i] < length[i] && size <= length[i] - (address - base[i])) valid = TRUE;
    GF_ASSERT(valid);
    if (address >= 0x05000000 && address + size <= 0x05000800) {
        memcpy(destination, (u8 *)(replayRow() ? rasterMemory.palettes : palettes) + (address - 0x05000000), size); return;
    }
    if (address >= 0x07000000 && address + size <= 0x07000800) {
        memcpy(destination, (u8 *)(replayRow() ? rasterMemory.objects : objects) + (address - 0x07000000), size); return;
    }
    memset(destination, 0, size); // Unmapped graphics reads are zero, overlapping banks are ORed.
    for (unsigned bank = 0; bank < 9; bank++) {
        u32 start;
        if (!bankAddress(bank, graphicsBankControl(bank), &start)) continue;
        u32 first = address > start ? address : start;
        u32 end = address + size < start + bankSize[bank] ? address + size : start + bankSize[bank];
        for (u32 position = first; position < end; position++)
            ((u8 *)destination)[position - address] |= ((u8 *)(replayRow() ? rasterMemory.vram : vram))[bankOffset[bank] + position - start];
    }
}
