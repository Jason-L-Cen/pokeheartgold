// SPDX-License-Identifier: GPL-3.0-or-later
#include <nitro.h>
#include <stddef.h>
#include <string.h>
#define CHECK(v) do { if (!(v)) return __LINE__; } while (0)
fx32 FX_DivS32(fx32,fx32);
fx32 FX_Sqrt(fx32);
fx64c FX_GetDivResultFx64c(void);
fx32 VEC_Mag(VecFx32 *);
fx32 VEC_Distance(const VecFx32 *,const VecFx32 *);

static u32 division(u16 mode,u64 numerator,u64 denominator,u64 quotient,u64 remainder,BOOL zero) {
    reg_CP_DIVCNT=mode; reg_CP_DIV_NUMER=numerator; reg_CP_DIV_DENOM=denominator;
    CHECK((reg_CP_DIVCNT&0xC003)==(mode|(zero ? 0x4000 : 0)));
    CHECK(reg_CP_DIV_RESULT==quotient && reg_CP_DIVREM_RESULT==remainder);
    CHECK(reg_CP_DIV_NUMER==numerator && reg_CP_DIV_DENOM==denominator);
    return 0;
}
u32 CheckOriginalMathUnit(void) {
    _Static_assert(sizeof(CPContext)==28 && offsetof(CPContext,div_mode)==24,"SDK coprocessor context layout");
    CHECK(!division(0,17,3,5,2,FALSE));
    CHECK(!division(0,(u64)-17,3,(u64)-5,(u64)-2,FALSE));
    CHECK(!division(0,17,(u64)-3,(u64)-5,2,FALSE));
    CHECK(!division(0,(u64)-17,(u64)-3,5,(u64)-2,FALSE));
    CHECK(!division(0,17,0,0x00000000FFFFFFFFULL,17,TRUE));
    CHECK(!division(0,(u64)-17,0,0xFFFFFFFF00000001ULL,(u64)-17,TRUE));
    CHECK(!division(0,0x80000000,0xFFFFFFFF,0x80000000,0,FALSE));
    CHECK(!division(0,17,0x100000000ULL,0x00000000FFFFFFFFULL,17,FALSE));
    for (u16 mode=1;mode<=3;mode++) {
        CHECK(!division(mode,0x100000002ULL,2,0x80000001ULL,0,FALSE));
        CHECK(!division(mode,0x8000000000000000ULL,(u64)-1,0x8000000000000000ULL,0,FALSE));
        CHECK(!division(mode,(u64)-17,0,1,(u64)-17,TRUE));
        CHECK(!division(mode,17,0,(u64)-1,17,TRUE));
    }
    CHECK(!division(2,0x100000002ULL,0x100000000ULL,1,2,FALSE));
    CHECK(!division(1,17,0x100000000ULL,(u64)-1,17,FALSE));

    u64 seed=0xFD28C531ABE90867ULL;
    for (u32 i=0;i<256;i++) {
        seed=seed*6364136223846793005ULL+1442695040888963407ULL;
        for (u16 mode=0;mode<2;mode++) {
            u64 value=mode ? seed : (u32)seed;
            reg_CP_SQRTCNT=mode; reg_CP_SQRT_PARAM=seed;
            CHECK(!(reg_CP_SQRTCNT&0x8000));
            u64 root=reg_CP_SQRT_RESULT;
            CHECK(root*root<=value && (root==0xFFFFFFFFULL || (root+1)*(root+1)>value));
        }
    }
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=~0ULL; CHECK(reg_CP_SQRT_RESULT==0xFFFFFFFFu);
    reg_CP_SQRTCNT=0; CHECK(reg_CP_SQRT_RESULT==65535);
    reg_CP_SQRT_PARAM=0; CHECK(reg_CP_SQRT_RESULT==0);

    struct { u32 before; CPContext context; u32 after; } saved;
    saved.before=0x12345678; saved.after=0x87654321;
    reg_CP_DIVCNT=2; reg_CP_DIV_NUMER=0x123456789ABCULL; reg_CP_DIV_DENOM=17;
    reg_CP_SQRTCNT=1; reg_CP_SQRT_PARAM=123456789;
    u64 quotient=reg_CP_DIV_RESULT;
    CP_SaveContext(&saved.context);
    CHECK(saved.context.div_numer==0x123456789ABCULL && saved.context.div_denom==17);
    CHECK(saved.context.sqrt==123456789 && saved.context.div_mode==2 && saved.context.sqrt_mode==1);
    CPContext original=saved.context;
    reg_CP_DIVCNT=0; reg_CP_DIV_NUMER=1; reg_CP_DIV_DENOM=0;
    reg_CP_SQRTCNT=0; reg_CP_SQRT_PARAM=9;
    CPi_RestoreContext(&saved.context);
    CHECK(reg_CP_DIV_RESULT==quotient && reg_CP_SQRT_RESULT==11111);
    CHECK(!memcmp(&original,&saved.context,sizeof(original)));
    CHECK(saved.before==0x12345678 && saved.after==0x87654321);

    CHECK(FX_DivS32(-17,3)==-5 && FX_ModS32(17,-3)==2);
    CHECK(FX_Div(6*FX32_ONE,-2*FX32_ONE)==-3*FX32_ONE && FX_Inv(2*FX32_ONE)==FX32_ONE/2);
    CHECK(FX_Sqrt(16*FX32_ONE)==4*FX32_ONE && FX_Sqrt(-FX32_ONE)==0);
    VecFx32 vector={3*FX32_ONE,4*FX32_ONE,0},unit,origin={0,0,0};
    CHECK(VEC_Mag(&vector)==5*FX32_ONE && VEC_Distance(&vector,&origin)==5*FX32_ONE);
    VEC_Normalize(&vector,&unit); CHECK(unit.x==2458 && unit.y==3277 && unit.z==0);
    MtxFx44 perspective,ortho;
    MTX_PerspectiveW(FX32_ONE,FX32_ONE,2*FX32_ONE,FX32_ONE,3*FX32_ONE,FX32_ONE,&perspective);
    const fx32 expectedPerspective[16]={2048,0,0,0,0,4096,0,0,0,0,-8192,-4096,0,0,-12288,0};
    CHECK(!memcmp(&perspective,expectedPerspective,sizeof(perspective)));
    MTX_OrthoW(FX32_ONE,-FX32_ONE,-FX32_ONE,FX32_ONE,FX32_ONE,3*FX32_ONE,FX32_ONE,&ortho);
    const fx32 expectedOrtho[16]={4096,0,0,0,0,4096,0,0,0,0,-4096,0,0,0,-8192,4096};
    CHECK(!memcmp(&ortho,expectedOrtho,sizeof(ortho)));
    return 0;
}
